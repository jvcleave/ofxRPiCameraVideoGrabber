#include "BaseEngine.h"


BaseEngine::BaseEngine()
{
	isOpen		= false;
	
	
	numMBps = 2.0;
	
	stopRequested = false;	 
	isStopping = false;
	
    isRecording = false;
	recordedFrameCounter = 0;
    recordingListener = NULL;
}



OMX_ERRORTYPE BaseEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE encoder,
                                                OMX_IN OMX_PTR engine,
                                                OMX_IN OMX_BUFFERHEADERTYPE* encoderOutputBuffer)
{    
    BaseEngine *grabber = static_cast<BaseEngine*>(engine);
    
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


void BaseEngine::configureCameraResolution()
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
    PrintPortDef(cameraOutputPortDefinition);
	
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

void BaseEngine::configureEncoder()
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

void BaseEngine::stopRecording()
{
	
	if(settings.doRecording)
	{
        stopRequested = true;
	}
	
}

OMXCameraSettings& BaseEngine::getSettings()
{
    return settings;
}

void BaseEngine::writeFile()
{
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
        if(recordingListener)
        {
            recordingListener->onRecordingComplete(filePath);
        }
	}
    else
	{
		ofLogVerbose(__func__) << filePath << " FAIL";
	}
    isRecording = false;
    recordedFrameCounter = 0;
    
    
}

