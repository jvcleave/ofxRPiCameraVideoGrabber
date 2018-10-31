#include "VideoModeEngine.h"



VideoModeEngine::VideoModeEngine()
{
    isOpen        = false;
    numMBps = 2.0;
    stopRequested = false;     
    isStopping = false;
    isRecording = false;
    recordedFrameCounter = 0;
    videoModeEngineListener = NULL;
    frameCounter = 0;
    isClosing = false;
    pixels = NULL;
}

int VideoModeEngine::getFrameCounter()
{
    
    if (!isOpen) 
    {
        return 0;
    }
    if(!settings.enableTexture)
    {
        OMX_CONFIG_BRCMPORTSTATSTYPE stats;
        
        OMX_INIT_STRUCTURE(stats);
        stats.nPortIndex = VIDEO_RENDER_INPUT_PORT;
        OMX_ERRORTYPE error =OMX_GetParameter(render, OMX_IndexConfigBrcmPortStats, &stats);
        OMX_TRACE(error);
        if (error == OMX_ErrorNone)
        {
            /*OMX_U32 nImageCount;
             OMX_U32 nBufferCount;
             OMX_U32 nFrameCount;
             OMX_U32 nFrameSkips;
             OMX_U32 nDiscards;
             OMX_U32 nEOS;
             OMX_U32 nMaxFrameSize;
             
             OMX_TICKS nByteCount;
             OMX_TICKS nMaxTimeDelta;
             OMX_U32 nCorruptMBs;*/
            //ofLogVerbose(__func__) << "nFrameCount: " << stats.nFrameCount;
            frameCounter = stats.nFrameCount;
        }else
        {        
            ofLog(OF_LOG_ERROR, "error OMX_CONFIG_BRCMPORTSTATSTYPE FAIL error: 0x%08x", error);
            //frameCounter = 0;
        }
    }
    
    return frameCounter;
}


OMX_ERRORTYPE VideoModeEngine::textureRenderFillBufferDone(OMX_IN OMX_HANDLETYPE render, OMX_IN OMX_PTR videoModeEngine, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{    
    OMX_ERRORTYPE error = OMX_ErrorNone;

    VideoModeEngine* engine = static_cast<VideoModeEngine*>(videoModeEngine);
    if(!engine->isClosing)
    {
        engine->frameCounter++;
        error = OMX_FillThisBuffer(render, pBuffer);
        OMX_TRACE(error);
    }
    return error;
}

void VideoModeEngine::setup(OMXCameraSettings& settings_, VideoModeEngineListener* videoModeEngineListener_)
{
    isClosing = false;
    settings = settings_;
    videoModeEngineListener = videoModeEngineListener_;
    
    ofLogVerbose(__func__) << "settings: " << settings.toString();
    if(settings.enableTexture)
    {
        generateEGLImage();
        
    }
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    OMX_CALLBACKTYPE cameraCallbacks;
    cameraCallbacks.EventHandler    = &VideoModeEngine::cameraEventHandlerCallback;
    
    error = OMX_GetHandle(&camera, OMX_CAMERA, this , &cameraCallbacks);
    OMX_TRACE(error);
    
    if (settings.enableTexture && settings.enablePixels) 
    {
        enablePixels();
        updatePixels();
    }
    configureCameraResolution();
    
}


OMX_ERRORTYPE VideoModeEngine::cameraEventHandlerCallback(OMX_HANDLETYPE camera, OMX_PTR videoModeEngine_, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
    /*ofLog(OF_LOG_VERBOSE, 
     "VideoModeEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
     __func__, eEvent, nData1, nData2, pEventData);*/
    VideoModeEngine *videoModeEngine = static_cast<VideoModeEngine*>(videoModeEngine_);
    //ofLogVerbose(__func__) << OMX_Maps::getInstance().eventTypes[eEvent];
    switch (eEvent) 
    {
        case OMX_EventParamOrConfigChanged:
        {
            
            return videoModeEngine->onCameraEventParamOrConfigChanged();
        }            
        default: 
        {
            break;
        }
    }
    return OMX_ErrorNone;
}



OMX_ERRORTYPE VideoModeEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE encoder,
                                                OMX_IN OMX_PTR engine,
                                                OMX_IN OMX_BUFFERHEADERTYPE* encoderOutputBuffer)
{    
    VideoModeEngine *grabber = static_cast<VideoModeEngine*>(engine);
    
    bool isKeyframeValid = false;
    grabber->recordedFrameCounter++;
    // The user wants to quit, but don't exit
    // the loop until we are certain that we have processed
    // a full frame till end of the frame, i.e. we're at the end
    // of the current key frame if processing one or until
    // the next key frame is detected. This way we should always
    // avoid corruption of the last encoded at the expense of
    // small delay in exiting.
    if(grabber->stopRequested && !grabber->isStopping) 
    {
        ofLogVerbose(__func__) << "Exit signal detected, waiting for next key frame boundry before exiting...";
        grabber->isStopping = true;
        isKeyframeValid = encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME;
    }
    if(grabber->isStopping && (isKeyframeValid ^ (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME))) 
    {
        ofLogVerbose(__func__) << "Key frame boundry reached, exiting loop...";
        grabber->writeFile();
    }else 
    {
        grabber->recordingFileBuffer.append((const char*) encoderOutputBuffer->pBuffer + encoderOutputBuffer->nOffset, encoderOutputBuffer->nFilledLen);
        //ofLogVerbose(__func__) << "encoderOutputBuffer->nFilledLen: " << encoderOutputBuffer->nFilledLen;
        ofLog() << grabber->recordingFileBuffer.size();
        OMX_ERRORTYPE error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        if(error != OMX_ErrorNone) 
        {
            ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);
            if(!grabber->didWriteFile)
            {
                ofLogError() << "HAD ERROR FILLING BUFFER, JUST WRITING WHAT WE HAVE";
                grabber->writeFile();

            }
        }
    }
    return OMX_ErrorNone;
}


void VideoModeEngine::configureCameraResolution()
{
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	error = DisableAllPortsForComponent(&camera);
	OMX_TRACE(error);
    
	OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
	OMX_INIT_STRUCTURE(cameraCallback);
	cameraCallback.nPortIndex	=	OMX_ALL;
	cameraCallback.nIndex		=	OMX_IndexParamCameraDeviceNumber;
	cameraCallback.bEnable		=	OMX_TRUE;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
    OMX_TRACE(error);

	//Set the camera (always 0)
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex	= OMX_ALL;
	device.nU32			= 0;
	
	error = OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
    OMX_TRACE(error);

	//Set the resolution
	OMX_PARAM_PORTDEFINITIONTYPE cameraOutputPortDefinition;
	OMX_INIT_STRUCTURE(cameraOutputPortDefinition);
	cameraOutputPortDefinition.nPortIndex = CAMERA_OUTPUT_PORT;
	
	error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &cameraOutputPortDefinition);
    OMX_TRACE(error);

	
	cameraOutputPortDefinition.format.video.nFrameWidth		= settings.width;
    cameraOutputPortDefinition.format.video.nFrameHeight	= settings.height;
	cameraOutputPortDefinition.format.video.xFramerate		= settings.framerate << 16; //currently always 30
    cameraOutputPortDefinition.format.video.nStride			= settings.width;
    //cameraOutputPortDefinition.format.video.eColorFormat    = OMX_COLOR_FormatYUV420PackedPlanar;

    error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraOutputPortDefinition);
    OMX_TRACE(error);
    //PrintPortDef(cameraOutputPortDefinition);
	
	//camera color spaces
	/*
	 OMX_COLOR_Format24bitRGB888
	 OMX_COLOR_FormatYUV420PackedPlanar
	 OMX_COLOR_FormatYUV422PackedPlanar
	 OMX_COLOR_FormatYCbYCr
	 OMX_COLOR_FormatYCrYCb
	 OMX_COLOR_FormatCbYCrY
	 OMX_COLOR_FormatCrYCbY
	 OMX_COLOR_FormatYUV420PackedSemiPlanar
	 */
	
	//egl_render color spaces
	/*
	 OMX_COLOR_Format18bitRGB666
	 OMX_COLOR_FormatYUV420PackedPlanar
	 OMX_COLOR_FormatYUV422PackedPlanar
	 OMX_COLOR_Format32bitABGR8888
	 */
	/*
	ofLogVerbose(__func__) << "CHECK: cameraOutputPortDefinition.format.video eCompressionFormat: " << OMX_Maps::getInstance().videoCodingTypes[cameraOutputPortDefinition.format.video.eCompressionFormat];

	ofLogVerbose(__func__) << "CHECK: cameraOutputPortDefinition.format.video eColorFormat: " << OMX_Maps::getInstance().colorFormatTypes[cameraOutputPortDefinition.format.video.eColorFormat];
	 */
}

void VideoModeEngine::configureEncoder()
{
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	error = DisableAllPortsForComponent(&encoder);
    OMX_TRACE(error);

	// Encoder input port definition is done automatically upon tunneling
	
	// Configure video format emitted by encoder output port
	OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
	OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
	encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	error = OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
    OMX_TRACE(error);
    if (error == OMX_ErrorNone) 
	{
        ofLogVerbose(__func__) << "encoderOutputPortDefinition buffer info";
        ofLog(OF_LOG_VERBOSE, 
              "nBufferCountMin(%u)					\n \
              nBufferCountActual(%u)				\n \
              nBufferSize(%u)						\n \
              nBufferAlignmen(%u) \n", 
              encoderOutputPortDefinition.nBufferCountMin, 
              encoderOutputPortDefinition.nBufferCountActual, 
              encoderOutputPortDefinition.nBufferSize, 
              encoderOutputPortDefinition.nBufferAlignment);
        
        ofLogVerbose(__func__) << "encoderOutputPortDefinition.format.video.eColorFormat: " << OMX_Maps::getInstance().colorFormatTypes[encoderOutputPortDefinition.format.video.eColorFormat];
	}
	
	//colorFormatTypes
	recordingBitRate = MEGABYTE_IN_BITS * numMBps;
	encoderOutputPortDefinition.format.video.nBitrate = recordingBitRate;
	error = OMX_SetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
    OMX_TRACE(error);

	// Configure encoding bitrate
	OMX_VIDEO_PARAM_BITRATETYPE encodingBitrate;
	OMX_INIT_STRUCTURE(encodingBitrate);
	encodingBitrate.eControlRate = OMX_Video_ControlRateVariable;
	//encodingBitrate.eControlRate = OMX_Video_ControlRateConstant;
	
	encodingBitrate.nTargetBitrate = recordingBitRate;
	encodingBitrate.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	
	error = OMX_SetParameter(encoder, OMX_IndexParamVideoBitrate, &encodingBitrate);
    OMX_TRACE(error);

	// Configure encoding format
	OMX_VIDEO_PARAM_PORTFORMATTYPE encodingFormat;
	OMX_INIT_STRUCTURE(encodingFormat);
	encodingFormat.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	encodingFormat.eCompressionFormat = OMX_VIDEO_CodingAVC;
	
	error = OMX_SetParameter(encoder, OMX_IndexParamVideoPortFormat, &encodingFormat);
    OMX_TRACE(error);

	error = OMX_GetParameter(encoder, OMX_IndexParamVideoPortFormat, &encodingFormat);
    OMX_TRACE(error);


}

void VideoModeEngine::stopRecording()
{
	
	stopRequested = true;
}

OMXCameraSettings& VideoModeEngine::getSettings()
{
    return settings;
}

void VideoModeEngine::writeFile()
{
    
    OMX_ERRORTYPE error = OMX_ErrorNone;

    //stop encoder
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    
    
	//format is raw H264 NAL Units
	ofLogVerbose(__func__) << "START";

    stringstream fileName;
	fileName << ofGetTimestampString() << "_";
	
	fileName << settings.width << "x";
	fileName << settings.height << "_";
	fileName << settings.framerate << "fps_";
	
	fileName << numMBps << "MBps_";
	
	fileName << recordedFrameCounter << "numFrames";
	
	string mkvFilePath = fileName.str() + ".mkv";
	
	fileName << ".h264";
	
	string filePath;
	
	if (settings.recordingFilePath == "") 
	{
		filePath = ofToDataPath(fileName.str(), true);
	}else
	{
		filePath = settings.recordingFilePath;
	}
	
	didWriteFile = ofBufferToFile(filePath, recordingFileBuffer, true);
	if(didWriteFile)
	{
		ofLogVerbose(__func__) << filePath  << " WRITE PASS";
        if(videoModeEngineListener)
        {
            videoModeEngineListener->onRecordingComplete(filePath);
        }
	}
    else
	{
		ofLogVerbose(__func__) << filePath << " FAIL";
	}
    
    
    encoderOutputBuffer->nFlags = 0;
    encoderOutputBuffer->nFilledLen  = 0;
    encoderOutputBuffer->nOffset= 0;

    recordingFileBuffer.clear();
    isRecording = false;
    recordedFrameCounter = 0;
    stopRequested = false;
    
}


#pragma mark PIXELS
void VideoModeEngine::enablePixels()
{
    doPixels = true;
    
}


void VideoModeEngine::disablePixels()
{
    doPixels = false;
}


void VideoModeEngine::updatePixels()
{
    if (!doPixels) 
    {
        return;
    }
    
    if (!fbo.isAllocated()) 
    {
        fbo.allocate(settings.width, settings.height, GL_RGBA);
    }
    int dataSize = settings.width * settings.height * 4;
    if (pixels == NULL)
    {
        pixels = new unsigned char[dataSize];
    }
    fbo.begin();
    ofClear(0, 0, 0, 0);
    texture.draw(0, 0);
    glReadPixels(0,0, settings.width, settings.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);    
    fbo.end();
}

unsigned char * VideoModeEngine::getPixels()
{
    return pixels;
}


void VideoModeEngine::generateEGLImage()
{
    
    ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
    display = appEGLWindow->getEglDisplay();
    context = appEGLWindow->getEglContext();
    
    
    texture.allocate(settings.width, settings.height, GL_RGBA);
    //texture.getTextureData().bFlipTexture = true;
    
    texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    GLuint textureID = texture.getTextureData().textureID;
    
    glEnable(GL_TEXTURE_2D);
    
    // setup first texture
    int dataSize = settings.width * settings.height * 4;
    
    GLubyte* pixelData = new GLubyte [dataSize];
    
    
    memset(pixelData, 0xff, dataSize);  // white texture, opaque
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, settings.width, settings.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    
    delete[] pixelData;
    
    
    // Create EGL Image
    eglImage = eglCreateImageKHR(
                                 display,
                                 context,
                                 EGL_GL_TEXTURE_2D_KHR,
                                 (EGLClientBuffer)textureID,
                                 0);
    glDisable(GL_TEXTURE_2D);
    if (eglImage == EGL_NO_IMAGE_KHR)
    {
        ofLogError()    << "Create EGLImage FAIL";
        return;
    }
    else
    {
        ofLogVerbose(__func__)    << "Create EGLImage PASS";
    }
}

OMX_ERRORTYPE VideoModeEngine::onCameraEventParamOrConfigChanged()
{
    OMX_ERRORTYPE error;
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    //Enable Camera Output Port
    OMX_CONFIG_PORTBOOLEANTYPE cameraport;
    OMX_INIT_STRUCTURE(cameraport);
    cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
    cameraport.bEnabled = OMX_TRUE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);    
    OMX_TRACE(error);
    
    //Set up video splitter
    OMX_CALLBACKTYPE splitterCallbacks;
    splitterCallbacks.EventHandler    = &VideoModeEngine::splitterEventHandlerCallback;
    
    error = OMX_GetHandle(&splitter, OMX_VIDEO_SPLITTER, this , &splitterCallbacks);
    OMX_TRACE(error);
    error =DisableAllPortsForComponent(&splitter);
    OMX_TRACE(error);
    
    //Set splitter to Idle
    error = SetComponentState(splitter, OMX_StateIdle);
    OMX_TRACE(error);
    
    //Set up texture renderer
    OMX_CALLBACKTYPE renderCallbacks;
    renderCallbacks.EventHandler    = &VideoModeEngine::renderEventHandlerCallback;
    renderCallbacks.EmptyBufferDone    = &VideoModeEngine::renderEmptyBufferDone;
    
    if(settings.enableTexture)
    {
        //Implementation specific
        renderCallbacks.FillBufferDone    = &VideoModeEngine::textureRenderFillBufferDone;
    }else
    {
        renderCallbacks.FillBufferDone    = &VideoModeEngine::directRenderFillBufferDone;
    }
    
    OMX_STRING renderType = OMX_VIDEO_RENDER; 
    int renderInputPort = VIDEO_RENDER_INPUT_PORT;
    
    if(settings.enableTexture)
    {
        renderType = OMX_EGL_RENDER; 
        renderInputPort = EGL_RENDER_INPUT_PORT;
    }
    
    error = OMX_GetHandle(&render, renderType, this , &renderCallbacks);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&render);
    OMX_TRACE(error);
    
    //Set renderer to Idle
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error);
    
    
    OMX_CALLBACKTYPE encoderCallbacks;
    encoderCallbacks.EventHandler        = &VideoModeEngine::encoderEventHandlerCallback;
    encoderCallbacks.EmptyBufferDone    = &VideoModeEngine::encoderEmptyBufferDone;
    encoderCallbacks.FillBufferDone        = &VideoModeEngine::encoderFillBufferDone;
    
    error =OMX_GetHandle(&encoder, OMX_VIDEO_ENCODER, this , &encoderCallbacks);
    OMX_TRACE(error);
    
    configureEncoder();
    
    //Create camera->splitter Tunnel
    error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT,
                            splitter, VIDEO_SPLITTER_INPUT_PORT);
    OMX_TRACE(error);
    
    // Tunnel splitter2 output port and encoder input port
    error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT2,
                            encoder, VIDEO_ENCODE_INPUT_PORT);
    OMX_TRACE(error);
    
    
    //Create splitter->render Tunnel
    error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT1,
                            render, renderInputPort);
    
    //Enable camera output port
    error = EnableComponentPort(camera, CAMERA_OUTPUT_PORT);
    OMX_TRACE(error);
    
    //Enable splitter input port
    error = EnableComponentPort(splitter, VIDEO_SPLITTER_INPUT_PORT);
    OMX_TRACE(error);
    
    //Enable splitter output port
    error = EnableComponentPort(splitter, VIDEO_SPLITTER_OUTPUT_PORT1);
    OMX_TRACE(error);
    
    //Enable splitter output2 port
    error = EnableComponentPort(splitter, VIDEO_SPLITTER_OUTPUT_PORT2);
    OMX_TRACE(error);
    
    if(settings.enableTexture)
    {
        //Enable render output port
        error = EnableComponentPort(render, EGL_RENDER_OUTPUT_PORT);
        OMX_TRACE(error);
    }
    
    //Enable render input port
    error = EnableComponentPort(render, renderInputPort);
    OMX_TRACE(error);
    
    //Enable encoder input port
    error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_INPUT_PORT, NULL);
    OMX_TRACE(error);
    
    //Set encoder to Idle
    error = SetComponentState(encoder, OMX_StateIdle);
    OMX_TRACE(error);
    
    //Enable encoder output port
    error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_OUTPUT_PORT, NULL);
    OMX_TRACE(error);
    
    // Configure encoder output buffer
    OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
    OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
    encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
    error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
    OMX_TRACE(error);
    
    error =  OMX_AllocateBuffer(encoder, 
                                &encoderOutputBuffer, 
                                VIDEO_ENCODE_OUTPUT_PORT, 
                                NULL, 
                                encoderOutputPortDefinition.nBufferSize);
    
    OMX_TRACE(error);
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << "UNABLE TO RECORD - MAY REQUIRE MORE GPU MEMORY";
    }
    
    if(settings.enableTexture)
    {
        //Set renderer to use texture
        error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
        OMX_TRACE(error);
    }
    
    
    //Start camera
    error = SetComponentState(camera, OMX_StateExecuting);
    OMX_TRACE(error);
    

    
    //Start splitter
    error = SetComponentState(splitter, OMX_StateExecuting);
    OMX_TRACE(error);
    
    //Start renderer
    error = SetComponentState(render, OMX_StateExecuting);
    OMX_TRACE(error);
    
    if(settings.enableTexture)
    {
        //start the buffer filling loop
        //once completed the callback will trigger and refill
        error = OMX_FillThisBuffer(render, eglBuffer);
        OMX_TRACE(error);
    }else
    {
        directDisplay.setup(render, 0, 0, settings.width, settings.height);
    }
    
    
    isOpen = true;
    videoModeEngineListener->onVideoEngineStart();
    return error;
}


void VideoModeEngine::startRecording()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    isRecording = true;
    
    //Start encoder
    error = SetComponentState(encoder, OMX_StateExecuting);
    OMX_TRACE(error);
    
    error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    OMX_TRACE(error);
}

void VideoModeEngine::close()
{
    
    if(isClosing) return;
    
    isClosing = true;
    /*
    if(settings.doRecording && !didWriteFile)
    {
        writeFile();
    }
    */
    directDisplay.close();
    
    OMX_ERRORTYPE error = OMX_ErrorNone;

    OMX_CONFIG_PORTBOOLEANTYPE cameraport;
    OMX_INIT_STRUCTURE(cameraport);
    cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
    cameraport.bEnabled = OMX_FALSE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);    
    OMX_TRACE(error);
    
    
    error = DisableAllPortsForComponent(&camera);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&splitter);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&encoder);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&render);
    OMX_TRACE(error);
    
    
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error);
    
    error = OMX_FreeHandle(encoder);
    OMX_TRACE(error);
    
    error = OMX_FreeHandle(splitter);
    OMX_TRACE(error);
    
    error = OMX_FreeHandle(render);
    OMX_TRACE(error);
    
    if(videoModeEngineListener)
    {
        videoModeEngineListener->onVideoEngineClose();
    }
    
}

#if 0
void VideoModeEngine::close()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;

    if(isClosing) return;
    
    isClosing = true;
    ofLogVerbose(__func__) << "START";
    
    
    OMX_CONFIG_PORTBOOLEANTYPE cameraport;
    OMX_INIT_STRUCTURE(cameraport);
    cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
    cameraport.bEnabled = OMX_FALSE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);    
    OMX_TRACE(error);
    
    
    
    
    if(settings.doRecording && !didWriteFile)
    {
        writeFile();
    }
    
    if(settings.doRecording)
    {
        error = FlushOMXComponent(encoder, VIDEO_ENCODE_INPUT_PORT);
        OMX_TRACE(error);
        
        error = FlushOMXComponent(encoder, VIDEO_ENCODE_OUTPUT_PORT);
        OMX_TRACE(error);
        
        error = DisableAllPortsForComponent(&encoder);
        OMX_TRACE(error);
    }
    
    int videoRenderInputPort = VIDEO_RENDER_INPUT_PORT;
    if(settings.enableTexture)
    {
        videoRenderInputPort = EGL_RENDER_INPUT_PORT;
    }
    
    error = FlushOMXComponent(camera, CAMERA_OUTPUT_PORT);
    OMX_TRACE(error);
    
    error = FlushOMXComponent(render, videoRenderInputPort);
    OMX_TRACE(error);
    
    error = SetComponentState(camera, OMX_StateIdle);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&camera);
    OMX_TRACE(error);
    
    if(settings.doRecording)
    {
        
        error = SetComponentState(encoder, OMX_StatePause);
        OMX_TRACE(error);
        
        error = SetComponentState(encoder, OMX_StateIdle);
        OMX_TRACE(error);
        
        error = FlushOMXComponent(encoder, VIDEO_ENCODE_INPUT_PORT);
        OMX_TRACE(error);
        
        
        error = FlushOMXComponent(encoder, VIDEO_ENCODE_OUTPUT_PORT);
        OMX_TRACE(error);
        
        
        error = SetComponentState(encoder, OMX_StateLoaded);
        OMX_TRACE(error);
        
        error = OMX_FreeBuffer(encoder, VIDEO_ENCODE_OUTPUT_PORT, encoderOutputBuffer);
        OMX_TRACE(error);
        
        encoderOutputBuffer = NULL;
    }
  
    error = SetComponentState(render, OMX_StateIdle);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&render);
    OMX_TRACE(error);
    
    error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT,  NULL, 0);
    OMX_TRACE(error);
    
    error = OMX_SetupTunnel(render, videoRenderInputPort,  NULL, 0);
    OMX_TRACE(error);
    
    if(settings.enableTexture)
    {
        ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
        display = appEGLWindow->getEglDisplay();
        context = appEGLWindow->getEglContext();
        if (eglImage)
        {
            if (!eglDestroyImageKHR(display, eglImage))
            {
                ofLogError(__func__) << "eglDestroyImageKHR FAIL <---------------- :(";
            }else
            {
                ofLogVerbose(__func__) << "eglDestroyImageKHR PASS <---------------- :)";
                
            }
            eglImage = NULL;
        }
    }
    
    if(settings.doRecording)
    {
        
        error = SetComponentState(splitter, OMX_StateIdle);
        OMX_TRACE(error);
        
        error = SetComponentState(encoder, OMX_StateIdle);
        OMX_TRACE(error);
        
        error = SetComponentState(render, OMX_StateIdle);
        OMX_TRACE(error);
        
        //Destroy splitter->encoder Tunnel
        error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT2, NULL, 0);
        OMX_TRACE(error);
        
        error = OMX_SetupTunnel(encoder, VIDEO_ENCODE_INPUT_PORT, NULL, 0);
        OMX_TRACE(error);
        
        //Destroy splitter->render Tunnel
        error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT1, NULL, 0);
        OMX_TRACE(error);
        
        error = OMX_SetupTunnel(render, videoRenderInputPort, NULL, 0);
        OMX_TRACE(error);
    }
    

    
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error);
    
    if(settings.doRecording)
    {
        error = OMX_FreeHandle(encoder);
        OMX_TRACE(error);
        
        error = OMX_FreeHandle(splitter);
        OMX_TRACE(error);
    }
    
    error = OMX_FreeHandle(render);
    OMX_TRACE(error);
    
    if (pixels) 
    {
        delete[] pixels;
        pixels = NULL;
    }
    if(videoModeEngineListener)
    {
        videoModeEngineListener->onVideoEngineClose();
    }
}
#endif
VideoModeEngine::~VideoModeEngine()
{
    
    close();
}
