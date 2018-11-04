#include "PhotoEngine.h"


PhotoEngine::PhotoEngine()
{
	didOpen = false;
    settings = NULL;
    render = NULL;
    camera = NULL;
    encoder = NULL;
    encoderOutputBuffer = NULL;
    listener = NULL;
    nullSink = NULL;
    OMX_INIT_STRUCTURE(previewPortConfig);
    previewPortConfig.nPortIndex = CAMERA_PREVIEW_PORT;
    saveFolderAbsolutePath.clear();
    renderInputPort = VIDEO_RENDER_INPUT_PORT;
    eglImageController = NULL;
}   


OMX_ERRORTYPE PhotoEngine::textureRenderFillBufferDone(OMX_IN OMX_HANDLETYPE render, OMX_IN OMX_PTR photoEngine, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{    
    //ofLogNotice(__func__) << endl;
    
    //PhotoEngine* engine = static_cast<PhotoEngine*>(photoEngine);
    OMX_ERRORTYPE error = OMX_FillThisBuffer(render, pBuffer);
    OMX_TRACE(error);
    return error;
}


void PhotoEngine::setup(ofxOMXCameraSettings* settings_, PhotoEngineListener* listener_, EGLImageController* eglImageController_)
{
    settings = settings_;
    listener = listener_;
    eglImageController = eglImageController_;
    //settings->width = omxcam_round(settings->width, 32);
    //settings->height = omxcam_round(settings->height, 16);

    
    //settings->stillPreviewWidth = omxcam_round(settings->stillPreviewWidth, 32);
    //settings->stillPreviewHeight = omxcam_round(settings->stillPreviewHeight, 16);
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    
    OMX_CALLBACKTYPE nullSinkCallbacks;
    
    nullSinkCallbacks.EventHandler       = &PhotoEngine::nullEventHandlerCallback;
    nullSinkCallbacks.EmptyBufferDone    = &PhotoEngine::nullEmptyBufferDone;
    nullSinkCallbacks.FillBufferDone     = &PhotoEngine::nullFillBufferDone;
    
    error =OMX_GetHandle(&nullSink, OMX_NULL_SINK, this , &nullSinkCallbacks);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&nullSink);
    OMX_TRACE(error);
    
    
    OMX_CALLBACKTYPE encoderCallbacks;
    
    encoderCallbacks.EventHandler       = &PhotoEngine::nullEventHandlerCallback;
    encoderCallbacks.EmptyBufferDone    = &PhotoEngine::nullEmptyBufferDone;
    encoderCallbacks.FillBufferDone     = &PhotoEngine::encoderFillBufferDone;
    
    error =OMX_GetHandle(&encoder, OMX_IMAGE_ENCODER, this , &encoderCallbacks);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&encoder);
    OMX_TRACE(error);
    
   if(settings->enableStillPreview) 
   {
       if(settings->enableTexture)
       {
           renderInputPort = EGL_RENDER_INPUT_PORT;
       }
       //Set up renderer
       OMX_CALLBACKTYPE renderCallbacks;
       renderCallbacks.EventHandler    = &PhotoEngine::nullEventHandlerCallback;
       renderCallbacks.EmptyBufferDone = &PhotoEngine::nullEmptyBufferDone;
       renderCallbacks.FillBufferDone  = &PhotoEngine::nullFillBufferDone;
       
       OMX_STRING renderType = OMX_VIDEO_RENDER; 
       
       if(settings->enableTexture)
       {
           //Implementation specific
           renderType = OMX_EGL_RENDER; 
           renderInputPort = EGL_RENDER_INPUT_PORT;
           renderCallbacks.FillBufferDone    = &PhotoEngine::textureRenderFillBufferDone;
       }else
       {
           renderCallbacks.FillBufferDone    = &PhotoEngine::nullFillBufferDone;
       }
       error = OMX_GetHandle(&render, renderType, this , &renderCallbacks);
       OMX_TRACE(error);
       
       error = DisableAllPortsForComponent(&render);
       OMX_TRACE(error);
       
   }

    
    
    
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
    
    


    if(settings->enableStillPreview) 
    { 
        error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
        OMX_TRACE(error);
        
        previewPortConfig.format.video.nFrameWidth  = settings->stillPreviewWidth;
        previewPortConfig.format.video.nFrameHeight = settings->stillPreviewHeight;
        
        previewPortConfig.format.video.nStride      = settings->stillPreviewWidth;
        
        
        //not setting also works
        previewPortConfig.format.video.nSliceHeight    = settings->stillPreviewHeight;
        
        error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
        OMX_TRACE(error);
        if(error != OMX_ErrorNone)
        {
            ofLogError(__func__) << GetOMXErrorString(error) << " WITH Camera state: " << PrintOMXState(camera);
        }
    }
    
    
    
    
    
    
    //Set the resolution
    OMX_PARAM_PORTDEFINITIONTYPE stillPortConfig;
    OMX_INIT_STRUCTURE(stillPortConfig);
    stillPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
    OMX_TRACE(error);
    
    stillPortConfig.format.image.nFrameWidth        = settings->width;
    stillPortConfig.format.image.nFrameHeight       = settings->height;
    stillPortConfig.format.image.nStride            = settings->width;
    stillPortConfig.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
    stillPortConfig.format.image.eColorFormat       = OMX_COLOR_FormatUnused;
    stillPortConfig.format.image.nStride            = settings->width;

    //ofLog() << "CAMERA_STILL_OUTPUT_PORT DEFAULT: " << GetColorFormatString(stillPortConfig.format.image.eColorFormat);
    //DEFAULT: YUV420PackedPlanar
    //YUV420PackedSemiPlanar
    //Unused
    //YUV420PackedPlanar
    //YUV420PackedSemiPlanar
    //YUV422PackedPlanar
    //YVU420PackedPlanar
    //YVU420PackedSemiPlanar
    error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &stillPortConfig);
    OMX_TRACE(error);

    
    
    
    //may need https://github.com/6by9/dcraw to make useful
    if(settings->enableRaw)
    {
        char dummy[] = "dummy";
        struct {
            //These two fields need to be together
            OMX_PARAM_CONTENTURITYPE rawConfig;
            char padding[5];
        } raw;
        OMX_INIT_STRUCTURE(raw.rawConfig);
        memcpy (raw.rawConfig.contentURI, dummy, 5);
        raw.rawConfig.nSize = sizeof (raw);
        error =  OMX_SetParameter(camera, OMX_IndexConfigCaptureRawImageURI, &raw);
        OMX_TRACE(error);
    }

    

}

OMX_ERRORTYPE PhotoEngine::encoderFillBufferDone(OMX_HANDLETYPE encoder,
                                                 OMX_PTR photoEngine,
                                                 OMX_BUFFERHEADERTYPE* encoderOutputBuffer)
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
    
    sensorMode.bOneShot = OMX_TRUE; //seems to determine whether OMX_BUFFERFLAG_EOS is passed
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
    

    OMX_CONFIG_BOOLEANTYPE SingleBufferCaptureInput;
    OMX_INIT_STRUCTURE(SingleBufferCaptureInput);
    SingleBufferCaptureInput.bEnabled = OMX_TRUE;

    //error =OMX_SetParameter(camera, OMX_IndexParamCameraSingleBufferCaptureInput, &SingleBufferCaptureInput);    
    //OMX_TRACE(error);
    
 
    if(settings->enableStillPreview) 
    { 
        
    
        
#if 0
        OMX_PARAM_CAMERAIMAGEPOOLTYPE pool;
        OMX_INIT_STRUCTURE(pool);
        error = OMX_GetParameter(camera, OMX_IndexParamCameraImagePool, &pool );
        //     pool.nNumHiResVideoFrames = 2;
        //     pool.nHiResVideoWidth = mWidth;
        //     pool.nHiResVideoHeight = mHeight;
        //     pool.eHiResVideoType = OMX_COLOR_FormatYUV420PackedPlanar;
        pool.nNumHiResStillsFrames = 2;
        pool.nHiResStillsWidth = settings->width;
        pool.nHiResStillsHeight = settings->height;
        pool.eHiResStillsType = OMX_COLOR_FormatYUV420PackedPlanar;
        //     pool.nNumLoResFrames = 2;
        //     pool.nLoResWidth = mWidth;
        //     pool.nLoResHeight = mHeight;
        //     pool.eLoResType = OMX_COLOR_FormatYUV420PackedPlanar;
        //     pool.nNumSnapshotFrames = 2;
        //     pool.eSnapshotType = OMX_COLOR_FormatYUV420PackedPlanar;
        pool.eInputPoolMode = OMX_CAMERAIMAGEPOOLINPUTMODE_TWOPOOLS;
        //     pool.nNumInputVideoFrames = 2;
        //     pool.nInputVideoWidth = mWidth;
        //     pool.nInputVideoHeight = mHeight;
        //     pool.eInputVideoType = OMX_COLOR_FormatYUV420PackedPlanar;
        pool.nNumInputStillsFrames = 2;
        pool.nInputStillsWidth = settings->width;
        pool.nInputStillsHeight = settings->height;
        pool.eInputStillsType = OMX_COLOR_FormatYUV420PackedPlanar;
        
        error =OMX_SetParameter(camera, OMX_IndexParamCameraImagePool, &pool);    

        
        OMX_PARAM_CAMERACAPTUREMODETYPE captureMode;
        OMX_INIT_STRUCTURE( captureMode );
        captureMode.nPortIndex = OMX_ALL;
        captureMode.eMode = OMX_CameraCaptureModeResumeViewfinderImmediately;
        error =OMX_SetParameter(camera, OMX_IndexParamCameraCaptureMode, &captureMode);    
        OMX_TRACE(error);  
#endif
        //Set renderer to Idle
        error = SetComponentState(render, OMX_StateIdle);
        OMX_TRACE(error);
       
        if(settings->enableTexture)
        {
            //Enable render output port
            error = EnableComponentPort(render, EGL_RENDER_OUTPUT_PORT);
            OMX_TRACE(error);
        }
        
        if(settings->enableTexture)
        {
            //Set renderer to use texture
            error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImageController->eglImage);
            OMX_TRACE(error);
        }
        
        
        error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, renderInputPort);
        OMX_TRACE(error);
        
        
        
        //Enable camera preview port
        error = WaitForPortEnable(camera, CAMERA_PREVIEW_PORT);
        OMX_TRACE(error);
        
        
        //Enable render input port
        error = WaitForPortEnable(render, renderInputPort);
        OMX_TRACE(error);
        

    }else
    {
        
        /*
         From raspistill:
         If preview is disabled, the null_sink component is used to 'absorb' the preview frames.
         It is necessary for the camera to produce preview frames even if not required for display,
         as they are used for calculating exposure and white balance settings->
         */
        error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, NULL_SINK_INPUT_PORT);
        OMX_TRACE(error);
        
        //Enable camera preview port
        error = WaitForPortEnable(camera, CAMERA_PREVIEW_PORT);
        OMX_TRACE(error);
        
        
        //Enable nullSink input port
        error = WaitForPortEnable(nullSink, NULL_SINK_INPUT_PORT);
        OMX_TRACE(error);
        
        //Start renderer
        error = SetComponentState(nullSink, OMX_StateExecuting);
        OMX_TRACE(error);
    }


    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);  

    
    //Start renderer
    error = SetComponentState(render, OMX_StateExecuting);
    OMX_TRACE(error);
    
    
    if(settings->enableStillPreview) 
    { 
        if(settings->enableTexture)
        {
            //start the buffer filling loop
            //once completed the callback will trigger and refill
            error = OMX_FillThisBuffer(render, eglBuffer);
            OMX_TRACE(error);
            ofLogNotice(__func__) << "TRIED OMX_FillThisBuffer";
        }else
        {
            directDisplay.setup(render, 0, 0, settings->stillPreviewWidth, settings->stillPreviewHeight);
        }
    }
    
    didOpen = true;
    
    listener->onPhotoEngineStart(camera);

    
    //debug actual preview size
    
    error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &previewPortConfig);
    OMX_TRACE(error);
    ofLogNotice(__func__) << "previewPortConfig: " << PrintPortDef(previewPortConfig);
    
    
    return error;
}




void PhotoEngine::takePhoto()
{
    ofLogVerbose(__func__);
    OMX_ERRORTYPE error;
    
    //Set camera to Idle
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    
    //Set encoder to Idle
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    
    //config encoder
    OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
    OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
    encoderOutputPortDefinition.nPortIndex = IMAGE_ENCODER_OUTPUT_PORT;
    error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
    OMX_TRACE(error);
    if (error == OMX_ErrorNone) 
    {
        encoderOutputPortDefinition.format.image.nFrameWidth        = settings->width;
        encoderOutputPortDefinition.format.image.nFrameHeight       = settings->height;
        encoderOutputPortDefinition.format.image.nSliceHeight       = settings->height;
        encoderOutputPortDefinition.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
        encoderOutputPortDefinition.format.image.eColorFormat       = OMX_COLOR_FormatUnused;
        
        error =OMX_SetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
        OMX_TRACE(error);
        
        encoderOutputBufferSize = encoderOutputPortDefinition.nBufferSize;
        
        OMX_IMAGE_PARAM_QFACTORTYPE compressionConfig;
        OMX_INIT_STRUCTURE(compressionConfig);
        compressionConfig.nPortIndex = IMAGE_ENCODER_OUTPUT_PORT;
        error =OMX_GetParameter(encoder, OMX_IndexParamQFactor, &compressionConfig);
        OMX_TRACE(error);
        
        compressionConfig.nQFactor = settings->stillQuality;
        
        error = OMX_SetParameter(encoder, OMX_IndexParamQFactor, &compressionConfig);
        
    }
    
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
    
    //Enable encoderOutputBuffer
    error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, IMAGE_ENCODER_OUTPUT_PORT, NULL, encoderOutputBufferSize);
    OMX_TRACE(error);
    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);
    
    
    //Start encoder
    error = SetComponentState(encoder, OMX_StateExecuting);
    OMX_TRACE(error);
    
    //Start capturing
    error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    if (error != OMX_ErrorNone) 
    {
        ofLogError() << "TAKE PHOTO FAILED";
    }
}

void PhotoEngine::writeFile()
{
        
    OMX_ERRORTYPE error;
    
    //debug what encoder input is
    /*
    OMX_PARAM_PORTDEFINITIONTYPE encoderInputPortDefinition;
    OMX_INIT_STRUCTURE(encoderInputPortDefinition);
    encoderInputPortDefinition.nPortIndex = IMAGE_ENCODER_INPUT_PORT;
    error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderInputPortDefinition);
    OMX_TRACE(error);
    ofLog() << "encoderInputPortDefinition: " << PrintPortDef(encoderInputPortDefinition);
    */

    bool result = false;
    
    if(saveFolderAbsolutePath.empty())
    {
        ofDirectory saveFolder;
        
        
        if(!settings->savedPhotosFolderName.empty())
        {
            saveFolder = ofDirectory(ofToDataPath(settings->savedPhotosFolderName, true));
            
        }else
        {
            saveFolder = ofDirectory(ofToDataPath("", true)); 
            
        }
        if(!saveFolder.exists())
        {
            saveFolder.create();
        }
        saveFolderAbsolutePath = saveFolder.getAbsolutePath();
    }
    
    string filePath = saveFolderAbsolutePath + "/" + ofGetTimestampString()+"_Q" + ofToString(settings->stillQuality)+".jpg";
    
    if(recordingFileBuffer.size()>0)
    {
        int start = ofGetElapsedTimeMillis();
        result = ofBufferToFile(filePath, recordingFileBuffer);
        int end = ofGetElapsedTimeMillis();
        ofLog() << filePath << " WRITE TOOK: " << end-start << "MS";
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
    
    close();
    setup(settings, listener, eglImageController);
}




PhotoEngine::~PhotoEngine()
{
    if(didOpen)
    {
        close();
    }
    settings = NULL;
}


void PhotoEngine::close()
{
    
    OMX_ERRORTYPE error;
    directDisplay.close();
 
    if(camera)
    {
        OMX_CONFIG_PORTBOOLEANTYPE cameraStillOutputPortConfig;
        OMX_INIT_STRUCTURE(cameraStillOutputPortConfig);
        cameraStillOutputPortConfig.nPortIndex = CAMERA_STILL_OUTPUT_PORT;
        cameraStillOutputPortConfig.bEnabled = OMX_FALSE;
        
        error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraStillOutputPortConfig);    
        OMX_TRACE(error);
        
        error = DisableAllPortsForComponent(&camera);
    }

    if(encoder)
    {
        error = DisableAllPortsForComponent(&encoder);

    }
    
    if(render)
    {
        error = DisableAllPortsForComponent(&render);
        
    }
    
    error = FlushOMXComponent(render, renderInputPort);
    OMX_TRACE(error);
    if(settings->enableTexture)
    {
        error = FlushOMXComponent(render, EGL_RENDER_OUTPUT_PORT);
        OMX_TRACE(error);

    }
    
    
    if(encoder)
    {
        if(encoderOutputBuffer)
        {
            error = OMX_FreeBuffer(encoder, IMAGE_ENCODER_OUTPUT_PORT, encoderOutputBuffer);
            OMX_TRACE(error);
            encoderOutputBuffer = NULL;
        }
  
    }
    
    if(camera)
    {
        error = OMX_SetupTunnel(camera, CAMERA_STILL_OUTPUT_PORT, NULL, 0);
        OMX_TRACE(error);
    }
 
    if(encoder)
    {
        error = OMX_SetupTunnel(encoder, IMAGE_ENCODER_INPUT_PORT, NULL, 0);
        OMX_TRACE(error);
    }
    
    if(settings->enableStillPreview) 
    {    
        if(camera)
        {
            error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, NULL, 0);
            OMX_TRACE(error);
        }
        
        if(render)
        {
            error = OMX_SetupTunnel(render, renderInputPort, NULL, 0);
            OMX_TRACE(error);
            
            error =  OMX_FreeHandle(render);
            OMX_TRACE(error);
            render = NULL;
        }
    }
    
    if(camera)
    {
        error = OMX_FreeHandle(camera);
        OMX_TRACE(error);
        camera = NULL;
    }
    
    if(encoder)
    {
        error = OMX_FreeHandle(encoder);
        OMX_TRACE(error); 
    }
    if(nullSink)
    {
        error = OMX_FreeHandle(nullSink);
        OMX_TRACE(error); 
    }
    didOpen = false;


    

}


