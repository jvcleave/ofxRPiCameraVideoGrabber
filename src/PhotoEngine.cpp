#include "PhotoEngine.h"


PhotoEngine::PhotoEngine()
{
	didOpen		= false;
    render = NULL;
    camera = NULL;
    encoder = NULL;
    encoderOutputBuffer = NULL;
    hasCreatedRenderTunnel = false;
}   


OMX_ERRORTYPE PhotoEngine::encoderFillBufferDone(OMX_HANDLETYPE encoder,
                                                       OMX_PTR photoEngine,
                                                       OMX_BUFFERHEADERTYPE* encoderOutputBuffer)
{	
    //ofLogVerbose(__func__);
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    PhotoEngine *engine = static_cast<PhotoEngine*>(photoEngine);

    engine->recordingFileBuffer.append((const char*) encoderOutputBuffer->pBuffer + encoderOutputBuffer->nOffset, 
                                        encoderOutputBuffer->nFilledLen);
    
    
    ofLogVerbose(__func__) << engine->recordingFileBuffer.size();
    bool endOfFrame = (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_ENDOFFRAME);
    bool endOfStream = (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_EOS);
    
    //ofLogVerbose(__func__) << "OMX_BUFFERFLAG_ENDOFFRAME: " << endOfFrame;
    //ofLogVerbose(__func__) << "OMX_BUFFERFLAG_EOS: " << endOfStream;
    if(endOfStream || endOfFrame)
    {
        engine->writeFile();
    }else
    {
        OMX_ERRORTYPE error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        OMX_TRACE(error);
    }   

    
    
    return error;
}


void PhotoEngine::setup(OMXCameraSettings& omxCameraSettings_)
{
    settings = omxCameraSettings_;
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
    
    //configureCameraResolution();
    error = DisableAllPortsForComponent(&camera);
    OMX_TRACE(error);
    
    
    OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
    OMX_INIT_STRUCTURE(cameraCallback);
    cameraCallback.nPortIndex    =    OMX_ALL;
    cameraCallback.nIndex        =    OMX_IndexParamCameraDeviceNumber;
    cameraCallback.bEnable        =    OMX_TRUE;
    
    error = OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
    OMX_TRACE(error);
    
    //Set the camera (always 0)
    OMX_PARAM_U32TYPE device;
    OMX_INIT_STRUCTURE(device);
    device.nPortIndex    = OMX_ALL;
    device.nU32            = 0;
    
    error = OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
    OMX_TRACE(error);
    
    //Set the resolution
    OMX_PARAM_PORTDEFINITIONTYPE stillPortConfig;
    OMX_INIT_STRUCTURE(stillPortConfig);
    stillPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
    OMX_TRACE(error, "stillPortConfig");
    if(error == OMX_ErrorNone) 
    {
        //ofLogVerbose() << "BEFORE SET";
        //ofLogVerbose() << "sessionConfig.width: " << sessionConfig.width;
        // ofLogVerbose() << "sessionConfig.height: " << sessionConfig.height;
        
       // PrintPortDef(stillPortConfig);
        stillPortConfig.format.image.nFrameWidth        = settings.width;
        stillPortConfig.format.image.nFrameHeight       = settings.height;
        
        stillPortConfig.format.video.nStride            = settings.width;
        
        
        //not setting also works
        //stillPortConfig.format.video.nSliceHeight    = round(settings.height / 16) * 16;
        
        error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
        OMX_TRACE(error);
        error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
       // ofLogVerbose() << "AFTER SET";
        //PrintPortDef(stillPortConfig);
    }
    
    //Set the preview resolution
    OMX_PARAM_PORTDEFINITIONTYPE previewPortConfig;
    OMX_INIT_STRUCTURE(previewPortConfig);
    previewPortConfig.nPortIndex = CAMERA_PREVIEW_PORT;
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
    OMX_TRACE(error, "previewPortConfig");
    if(error == OMX_ErrorNone) 
    {
        //ofLogVerbose() << "BEFORE SET";
        //ofLogVerbose() << "sessionConfig.width: " << sessionConfig.width;
        // ofLogVerbose() << "sessionConfig.height: " << sessionConfig.height;
        
        //PrintPortDef(previewPortConfig);
        previewPortConfig.format.video.nFrameWidth        = settings.stillPreviewWidth;
        previewPortConfig.format.video.nFrameHeight       = settings.stillPreviewHeight;
        
        previewPortConfig.format.video.nStride            = settings.stillPreviewWidth;
        
        
        //not setting also works
        //previewPortConfig.format.video.nSliceHeight    = round(settings.stillPreviewHeight / 16) * 16;
        
        error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
        OMX_TRACE(error);
        //error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
        //ofLogVerbose() << "AFTER SET";
        //PrintPortDef(previewPortConfig);
    }
}

OMX_ERRORTYPE PhotoEngine::encoderEventHandlerCallback(OMX_HANDLETYPE hComponent,
                                                            OMX_PTR photoEngine,
                                                            OMX_EVENTTYPE eEvent,
                                                            OMX_U32 nData1,
                                                            OMX_U32 nData2,
                                                            OMX_PTR pEventData)
{
    //PhotoEngine *engine = static_cast<PhotoEngine*>(photoEngine);

    if (eEvent == OMX_EventError)
    {
         OMX_TRACE((OMX_ERRORTYPE)nData1);
    }
    return OMX_ErrorNone;
}


OMX_ERRORTYPE PhotoEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent,
                                                     OMX_PTR photoEngine,
                                                     OMX_EVENTTYPE eEvent,
                                                     OMX_U32 nData1,
                                                     OMX_U32 nData2,
                                                     OMX_PTR pEventData)
{

    PhotoEngine* engine = static_cast<PhotoEngine*>(photoEngine);
    //ofLogVerbose(__func__) << OMX_Maps::getInstance().getEvent(eEvent);
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
            /*ofLog(OF_LOG_VERBOSE, 
             "TextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
             __func__, eEvent, nData1, nData2, pEventData);*/
            
            break;
        }
    }
    return OMX_ErrorNone;
}








OMX_ERRORTYPE PhotoEngine::onCameraEventParamOrConfigChanged()
{
    

    OMX_ERRORTYPE error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error, "camera->OMX_StateIdle");

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
   
    error = buildNonCapturePipeline();
        

    didOpen = true;
    return error;
}

OMX_ERRORTYPE PhotoEngine::buildNonCapturePipeline()
{
    OMX_ERRORTYPE error;
    
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error);

    
    error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, VIDEO_RENDER_INPUT_PORT);
    OMX_TRACE(error);
    
    //Enable camera preview port
    error = EnableComponentPort(camera, CAMERA_PREVIEW_PORT);
    OMX_TRACE(error);
    
    
    //Enable render input port
    error = EnableComponentPort(render, VIDEO_RENDER_INPUT_PORT);
    OMX_TRACE(error);
    
    
    //Start renderer
    error = SetComponentState(render, OMX_StateExecuting);
    OMX_TRACE(error);
    
    directDisplay.setup(render, 0, 0, settings.stillPreviewWidth, settings.stillPreviewHeight);

    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);
    return error;

}






bool PhotoEngine::takePhoto()
{
    bool result = false;
    ofLogVerbose(__func__);
    OMX_ERRORTYPE error;
    
    error = configureEncoder();
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    //Create camera->encoder Tunnel
    error = OMX_SetupTunnel(camera, CAMERA_STILL_OUTPUT_PORT, encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    //Enable camera output port
    error = EnableComponentPort(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    //Enable encoder input port
    error = EnableComponentPort(encoder, IMAGE_ENCODER_INPUT_PORT);    
    OMX_TRACE(error); 
    if(error != OMX_ErrorNone) return false;
    
    //Enable encoder output port
    error = EnableComponentPort(encoder, IMAGE_ENCODER_OUTPUT_PORT);
    OMX_TRACE(error); 
    if(error != OMX_ErrorNone) return false;
    
    
    error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, IMAGE_ENCODER_OUTPUT_PORT, NULL, encoderBufferSize);
    OMX_TRACE(error, "encode OMX_AllocateBuffer");
    if(error != OMX_ErrorNone) return false;
    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    
    ofLogWarning() << __LINE__ << " PrintOMXState(encoder): " << PrintOMXState(encoder);
    
    // Start encoder
    error = SetComponentState(encoder, OMX_StateExecuting);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone) return false;
    
    
    error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    if (error != OMX_ErrorNone) 
    {
        ofLogError() << "TAKE PHOTO FAILED";
    }else
    {
        OMX_TRACE(error, "OMX_FillThisBuffer");
        result = true;

    }
    return result;
}
OMX_ERRORTYPE PhotoEngine::destroyEncoder()
{
    OMX_ERRORTYPE error;
    

    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    
    //Disable camera output port
    error = DisableComponentPort(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    
    //Disable encoder input port
    error = DisableComponentPort(encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);

   

    error = FlushOMXComponent(encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(encoder, IMAGE_ENCODER_OUTPUT_PORT);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&encoder, "encoder");
    OMX_TRACE(error, "DisableAllPortsForComponent encoder");
    
    error = OMX_FreeBuffer(encoder, IMAGE_ENCODER_OUTPUT_PORT, encoderOutputBuffer);
    OMX_TRACE(error, "encoder->OMX_FreeBuffer");
    encoderOutputBuffer = NULL;
    
    ofLog() << __LINE__ << " PrintOMXState(encoder): " << PrintOMXState(encoder);

    error = OMX_FreeHandle(encoder);
    OMX_TRACE(error, "OMX_FreeHandle(encoder)"); 
    encoder = NULL;
    return error;
    
}
bool PhotoEngine::writeFile()
{
        
  
    
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
    
    
    OMX_ERRORTYPE error;
    
    error = FlushOMXComponent(camera, CAMERA_STILL_OUTPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(encoder, IMAGE_ENCODER_INPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(encoder, IMAGE_ENCODER_OUTPUT_PORT);
    OMX_TRACE(error);

    destroyEncoder();
    
    //buildNonCapturePipeline();
    
    return result;
}


OMX_ERRORTYPE PhotoEngine::configureEncoder()
{
    
    OMX_ERRORTYPE error;
  

    //Create encoder
    
    OMX_CALLBACKTYPE encoderCallbacks;

    encoderCallbacks.EventHandler		= &PhotoEngine::encoderEventHandlerCallback;
    encoderCallbacks.EmptyBufferDone	= &PhotoEngine::nullEmptyBufferDone;
    encoderCallbacks.FillBufferDone		= &PhotoEngine::encoderFillBufferDone;
    
    error =OMX_GetHandle(&encoder, OMX_IMAGE_ENCODER, this , &encoderCallbacks);
    OMX_TRACE(error, "encoderCallbacks");
    
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
        //ofLogVerbose() << "ENCODER BEFORE SET";

        
        //PrintPortDef(encoderOutputPortDefinition);
        encoderOutputPortDefinition.format.image.nFrameWidth = settings.width;
        encoderOutputPortDefinition.format.image.nFrameHeight= settings.height;
        encoderOutputPortDefinition.format.image.nStride = settings.width; 
        encoderOutputPortDefinition.format.image.eColorFormat = OMX_COLOR_FormatUnused;
        encoderOutputPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
        error =OMX_SetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
        OMX_TRACE(error);
        //ofLogVerbose() << "ENCODER AFTER SET";
        //PrintPortDef(encoderOutputPortDefinition);
        
        encoderBufferSize = encoderOutputPortDefinition.nBufferSize;

        
    }
    return error;
    
}

PhotoEngine::~PhotoEngine()
{
    if(didOpen)
    {
        close();
    }
}


void PhotoEngine::close()
{
    
    directDisplay.close();
    OMX_ERRORTYPE error;
    //error =  OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_STILL_OUTPUT_PORT, NULL);
    //OMX_TRACE(error, "camera: OMX_CommandFlush");
    
    /*
    OMX_CONFIG_PORTBOOLEANTYPE cameraStillOutputPortConfig;
    OMX_INIT_STRUCTURE(cameraStillOutputPortConfig);
    cameraStillOutputPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    cameraStillOutputPortConfig.bEnabled = OMX_FALSE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraStillOutputPortConfig);    
    OMX_TRACE(error);
    */
    
    
    bool isCameraIdle = false;
    while(!isCameraIdle)
    {
        OMX_STATETYPE cameraState;
        OMX_GetState(camera, &cameraState);
        if(cameraState == OMX_StateIdle)
        {
            isCameraIdle = true;
        }else
        {
            error = SetComponentState(camera, OMX_StateIdle);
            OMX_TRACE(error, "camera->OMX_StateIdle");
        }
    }
    
    error = DisableAllPortsForComponent(&camera, "camera");
    OMX_TRACE(error, "DisableAllPortsForComponent: camera");
    
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error, "render->OMX_StateIdle");
    
    if (encoder) 
    {
        
        
        error = FlushOMXComponent(encoder, IMAGE_ENCODER_INPUT_PORT);
        OMX_TRACE(error);
        
        error = FlushOMXComponent(encoder, IMAGE_ENCODER_OUTPUT_PORT);
        OMX_TRACE(error);

        error = DisableAllPortsForComponent(&encoder, "encoder");
        OMX_TRACE(error, "DisableAllPortsForComponent encoder");
        
        error = OMX_FreeBuffer(encoder, IMAGE_ENCODER_OUTPUT_PORT, encoderOutputBuffer);
        OMX_TRACE(error);
        
        
        error = SetComponentState(encoder, OMX_StateIdle);
        OMX_TRACE(error);

        
        ofLogWarning() << __LINE__ << " PrintOMXState(encoder): " << PrintOMXState(encoder);
    }
   
    
    //OMX_StateLoaded
    error = SetComponentState(camera, OMX_StateLoaded);
    OMX_TRACE(error, "camera->OMX_StateLoaded");
    
    error = SetComponentState(render, OMX_StateLoaded);
    OMX_TRACE(error, "render->OMX_StateLoaded");
    
    if (encoder) 
    {
        error = SetComponentState(encoder, OMX_StateLoaded);
        OMX_TRACE(error, "encoder->OMX_StateLoaded");
    }

    
    //OMX_FreeHandle
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error, "OMX_FreeHandle(camera)");
    
    error =  OMX_FreeHandle(render);
    OMX_TRACE(error, "OMX_FreeHandle(render)");
    if (encoder) 
    {
        error = OMX_FreeHandle(encoder);
        OMX_TRACE(error, "OMX_FreeHandle(encoder)"); 
    }
    //sessionConfig = NULL;
    didOpen = false;

}


