#include "PhotoEngine.h"


PhotoEngine::PhotoEngine()
{
	didOpen = false;
    render = NULL;
    camera = NULL;
    encoder = NULL;
    encoderOutputBuffer = NULL;
    listener = NULL;
}   

void PhotoEngine::setup(OMXCameraSettings& omxCameraSettings_, PhotoEngineListener* listener_)
{
    settings = omxCameraSettings_;
    listener = listener_;
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    OMX_CALLBACKTYPE cameraCallbacks;
    
    cameraCallbacks.EventHandler    = &PhotoEngine::cameraEventHandlerCallback;
    cameraCallbacks.EmptyBufferDone	= &PhotoEngine::nullEmptyBufferDone;
    cameraCallbacks.FillBufferDone	= &PhotoEngine::nullFillBufferDone;
    
    error = OMX_GetHandle(&camera, OMX_CAMERA, this , &cameraCallbacks);
    if(error != OMX_ErrorNone) 
    {
        OMX_TRACE(error, "camera OMX_GetHandle FAIL");
    }
    
    error = DisableAllPortsForComponent(&camera);
    OMX_TRACE(error);
    
    
    OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
    OMX_INIT_STRUCTURE(cameraCallback);
    cameraCallback.nPortIndex   =    OMX_ALL;
    cameraCallback.nIndex       =    OMX_IndexParamCameraDeviceNumber;
    cameraCallback.bEnable      =    OMX_TRUE;
    
    error = OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
    OMX_TRACE(error);
    
    //Set the camera (usually 0 unless compute module)
    OMX_PARAM_U32TYPE device;
    OMX_INIT_STRUCTURE(device);
    device.nPortIndex   = OMX_ALL;
    device.nU32         = 0;
    
    error = OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
    OMX_TRACE(error);
    
    //Set the resolution
    OMX_PARAM_PORTDEFINITIONTYPE stillPortConfig;
    OMX_INIT_STRUCTURE(stillPortConfig);
    stillPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        stillPortConfig.format.image.nFrameWidth        = settings.width;
        stillPortConfig.format.image.nFrameHeight       = settings.height;
        
        stillPortConfig.format.video.nStride            = settings.width;
        
        
        //not setting also works
        //stillPortConfig.format.video.nSliceHeight    = round(settings.height / 16) * 16;
        
        error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
        OMX_TRACE(error);
        
    }
    
    //Set the preview resolution
    OMX_PARAM_PORTDEFINITIONTYPE previewPortConfig;
    OMX_INIT_STRUCTURE(previewPortConfig);
    previewPortConfig.nPortIndex = CAMERA_PREVIEW_PORT;
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        previewPortConfig.format.video.nFrameWidth        = settings.stillPreviewWidth;
        previewPortConfig.format.video.nFrameHeight       = settings.stillPreviewHeight;
        
        previewPortConfig.format.video.nStride            = settings.stillPreviewWidth;
        
        
        //not setting also works
        //previewPortConfig.format.video.nSliceHeight    = round(settings.stillPreviewHeight / 16) * 16;
        
        error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
        OMX_TRACE(error);
        if(error != OMX_ErrorNone)
        {
            ofLogError(__func__) << GetOMXErrorString(error) << " WITH Camera state: " << PrintOMXState(camera);
        }
    }
    listener->onPhotoEngineStart(camera);

}

OMX_ERRORTYPE PhotoEngine::encoderFillBufferDone(OMX_HANDLETYPE encoder, OMX_PTR photoEngine, OMX_BUFFERHEADERTYPE* encoderOutputBuffer)
{    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    PhotoEngine* engine = static_cast<PhotoEngine*>(photoEngine);
    
    engine->recordingFileBuffer.append((const char*) encoderOutputBuffer->pBuffer + encoderOutputBuffer->nOffset, 
                                       encoderOutputBuffer->nFilledLen);
    
    
    ofLogVerbose(__func__) << engine->recordingFileBuffer.size();
    bool endOfFrame = (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_ENDOFFRAME);
    bool endOfStream = (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_EOS);
    
    //ofLogVerbose(__func__) << "OMX_BUFFERFLAG_ENDOFFRAME: " << endOfFrame;
    //ofLogVerbose(__func__) << "OMX_BUFFERFLAG_EOS: " << endOfStream;
    if(endOfFrame || endOfStream)
    {
        engine->writeFile();
    }else
    {
        OMX_ERRORTYPE error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        OMX_TRACE(error);
    }   
    return error;
}


OMX_ERRORTYPE PhotoEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent,
                                                     OMX_PTR photoEngine,
                                                     OMX_EVENTTYPE eEvent,
                                                     OMX_U32 nData1,
                                                     OMX_U32 nData2,
                                                     OMX_PTR pEventData)
{

    PhotoEngine* engine = static_cast<PhotoEngine*>(photoEngine);
    switch (eEvent) 
    {
        case OMX_EventParamOrConfigChanged:
        {
            
            return engine->onCameraEventParamOrConfigChanged();
        }	
            
        case OMX_EventError:
        {
            OMX_TRACE((OMX_ERRORTYPE)nData1);
        }
        default: 
        {
            break;
        }
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE PhotoEngine::onCameraEventParamOrConfigChanged()
{

    OMX_ERRORTYPE error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);

    //PrintSensorModes(camera);
    
    OMX_FRAMESIZETYPE frameSizeConfig;
    OMX_INIT_STRUCTURE(frameSizeConfig);
    frameSizeConfig.nPortIndex = OMX_ALL;
    
    OMX_PARAM_SENSORMODETYPE sensorMode;
    OMX_INIT_STRUCTURE(sensorMode);
    sensorMode.nPortIndex = OMX_ALL;
    sensorMode.sFrameSize = frameSizeConfig;
    error =OMX_GetParameter(camera, OMX_IndexParamCommonSensorMode, &sensorMode);
    
    OMX_TRACE(error);
    
    sensorMode.bOneShot = OMX_FALSE; //seems to determine whether OMX_BUFFERFLAG_EOS is passed
    error =OMX_SetParameter(camera, OMX_IndexParamCommonSensorMode, &sensorMode);

    OMX_TRACE(error);
    
    OMX_CONFIG_CAMERASENSORMODETYPE sensorConfig;
    OMX_INIT_STRUCTURE(sensorConfig);
    sensorConfig.nPortIndex = OMX_ALL;
    //sensorConfig.nModeIndex = 0;
    error =  OMX_GetParameter(camera, OMX_IndexConfigCameraSensorModes, &sensorConfig);
    OMX_TRACE(error);

    
    OMX_CONFIG_PORTBOOLEANTYPE cameraStillOutputPortConfig;
    OMX_INIT_STRUCTURE(cameraStillOutputPortConfig);
    cameraStillOutputPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    cameraStillOutputPortConfig.bEnabled = OMX_TRUE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraStillOutputPortConfig);	
    OMX_TRACE(error);
    

    
    OMX_PARAM_CAMERACAPTUREMODETYPE captureMode;
    OMX_INIT_STRUCTURE( captureMode );
    captureMode.nPortIndex = OMX_ALL;
    captureMode.eMode = OMX_CameraCaptureModeResumeViewfinderImmediately;
    error =OMX_SetParameter(camera, OMX_IndexParamCameraCaptureMode, &captureMode);    
    OMX_TRACE(error);    
    
    //Set up renderer
    OMX_CALLBACKTYPE renderCallbacks;
    renderCallbacks.EventHandler	= &PhotoEngine::nullEventHandlerCallback;
    renderCallbacks.EmptyBufferDone	= &PhotoEngine::nullEmptyBufferDone;
    renderCallbacks.FillBufferDone	= &PhotoEngine::nullFillBufferDone;
    

    OMX_GetHandle(&render, OMX_VIDEO_RENDER, this , &renderCallbacks);
    DisableAllPortsForComponent(&render);
    

    //Set renderer to Idle
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error);
   
    //Set camera to Idle
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    

    
    error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, VIDEO_RENDER_INPUT_PORT);
    OMX_TRACE(error);
    
    //Enable camera preview port
    error = WaitForPortEnable(camera, CAMERA_PREVIEW_PORT);
    OMX_TRACE(error);
    
    
    //Enable render input port
    error = WaitForPortEnable(render, VIDEO_RENDER_INPUT_PORT);
    OMX_TRACE(error);
    
    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);  
    
    //Start renderer
    error = SetComponentState(render, OMX_StateExecuting);
    OMX_TRACE(error);
    

        
    directDisplay.setup(render, 0, 0, settings.stillPreviewWidth, settings.stillPreviewHeight);
    
    didOpen = true;
    return error;
}




void PhotoEngine::takePhoto()
{
    ofLogVerbose(__func__);
    OMX_ERRORTYPE error;
    
    
    //Set camera to Idle
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    
    OMX_CALLBACKTYPE encoderCallbacks;
    
    encoderCallbacks.EventHandler       = &PhotoEngine::nullEventHandlerCallback;
    encoderCallbacks.EmptyBufferDone    = &PhotoEngine::nullEmptyBufferDone;
    encoderCallbacks.FillBufferDone     = &PhotoEngine::encoderFillBufferDone;
    
    error =OMX_GetHandle(&encoder, OMX_IMAGE_ENCODER, this , &encoderCallbacks);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&encoder);
    OMX_TRACE(error);
    
    // Encoder input port definition is done automatically upon tunneling
    
    
    // Configure video format emitted by encoder output port
    
    OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
    OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
    encoderOutputPortDefinition.nPortIndex = IMAGE_ENCODER_OUTPUT_PORT;
    error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
    OMX_TRACE(error);
    if (error == OMX_ErrorNone) 
    {
        encoderOutputPortDefinition.format.image.nFrameWidth        = settings.width;
        encoderOutputPortDefinition.format.image.nFrameHeight       = settings.height;
        encoderOutputPortDefinition.format.image.nStride            = settings.width; 
        encoderOutputPortDefinition.format.image.eColorFormat       = OMX_COLOR_FormatUnused;
        encoderOutputPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
        error =OMX_SetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
        OMX_TRACE(error);
        
        
        encoderOutputBufferSize = encoderOutputPortDefinition.nBufferSize;
        
    }
    
    //Set encoder to Idle
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    
    
    //Create camera->encoder Tunnel
    error = OMX_SetupTunnel(camera, CAMERA_STILL_OUTPUT_PORT, encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);
    
    //Enable camera output port
    error = WaitForPortEnable(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    
    //Enable encoder input port
    error = WaitForPortEnable(encoder, IMAGE_ENCODER_INPUT_PORT);    
    OMX_TRACE(error); 
    
    //Enable encoder output port
    error = WaitForPortEnable(encoder, IMAGE_ENCODER_OUTPUT_PORT);
    OMX_TRACE(error); 
    
    
    
    error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, IMAGE_ENCODER_OUTPUT_PORT, NULL, encoderOutputBufferSize);
    OMX_TRACE(error);

        
    
    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);
    
    
    //Start encoder
    error = SetComponentState(encoder, OMX_StateExecuting);
    OMX_TRACE(error);
    
    
    error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    if (error != OMX_ErrorNone) 
    {
        ofLogError() << "TAKE PHOTO FAILED";
    }else
    {
        OMX_TRACE(error);
        //result = true;
        
    }
    
    
}

void PhotoEngine::writeFile()
{
        
    OMX_ERRORTYPE error;
    
    //error = SetComponentState(encoder, OMX_StateIdle);
    //OMX_TRACE(error);

    bool result = false;
    string filePath = ofToDataPath(ofGetTimestampString()+".jpg", true);
    
    if(recordingFileBuffer.size()>0)
    {
        result = ofBufferToFile(filePath, recordingFileBuffer);
    }
    recordingFileBuffer.clear();
    
    if(result)
    {
        if(listener)
        {
            listener->onTakePhotoComplete(filePath);
        }else
        {
            ofLogWarning(__func__) << filePath << " WRITTEN BUT NO LISTENER SET";
        }
    }
    
    close(false);
    setup(settings, listener);
}




PhotoEngine::~PhotoEngine()
{
    /*if(didOpen)
    {
        close(true);
    }*/
}


void PhotoEngine::close(bool isExiting)
{
    OMX_ERRORTYPE error;

    /*
    //Start camera
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    //Start camera
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    
    //Start camera
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error);
    */

    
    directDisplay.close();
 
    
    OMX_CONFIG_PORTBOOLEANTYPE cameraStillOutputPortConfig;
    OMX_INIT_STRUCTURE(cameraStillOutputPortConfig);
    cameraStillOutputPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    cameraStillOutputPortConfig.bEnabled = OMX_FALSE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraStillOutputPortConfig);    
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&camera);
    error = DisableAllPortsForComponent(&encoder);
    error = DisableAllPortsForComponent(&render);
    

    error = OMX_FreeBuffer(encoder, IMAGE_ENCODER_OUTPUT_PORT, encoderOutputBuffer);
    OMX_TRACE(error);
    encoderOutputBuffer = NULL;

    error = FlushOMXComponent(render, VIDEO_RENDER_INPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(camera, CAMERA_PREVIEW_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(encoder, IMAGE_ENCODER_OUTPUT_PORT);
    OMX_TRACE(error);
    
    
    error = OMX_SetupTunnel(camera, CAMERA_STILL_OUTPUT_PORT, NULL, 0);
    OMX_TRACE(error);
    
    error = OMX_SetupTunnel(encoder, IMAGE_ENCODER_INPUT_PORT, NULL, 0);
    OMX_TRACE(error);
    
    
    error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, NULL, 0);
    OMX_TRACE(error);
    
    
    error = OMX_SetupTunnel(render, VIDEO_RENDER_INPUT_PORT, NULL, 0);
    OMX_TRACE(error);
    
    
    
    //OMX_FreeHandle
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error);
    
    error =  OMX_FreeHandle(render);
    OMX_TRACE(error);
    
    error = OMX_FreeHandle(encoder);
    OMX_TRACE(error); 
    
    //sessionConfig = NULL;
    didOpen = false;

    if(!isExiting)
    {
        listener->onPhotoEngineClose();

    }
    

}


