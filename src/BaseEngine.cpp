#include "BaseEngine.h"


BaseEngine::BaseEngine()
{
	isOpen		= false;
	
	didWriteFile = false;
	
	numMBps = 2.0;
	
	stopRequested = false;	 
	isStopping = false;
	isKeyframeValid = false;
	doFillBuffer = false;
	bufferAvailable = false;
	
	recordedFrameCounter = 0;
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

	
	cameraOutputPortDefinition.format.video.nFrameWidth		= omxCameraSettings.width;
    cameraOutputPortDefinition.format.video.nFrameHeight	= omxCameraSettings.height;
	cameraOutputPortDefinition.format.video.xFramerate		= omxCameraSettings.framerate << 16; //currently always 30
    cameraOutputPortDefinition.format.video.nStride			= omxCameraSettings.width;
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
void BaseEngine::threadedFunction()
{
	while (isThreadRunning()) 
	{
		if(bufferAvailable) 
		{
			// The user wants to quit, but don't exit
			// the loop until we are certain that we have processed
			// a full frame till end of the frame, i.e. we're at the end
			// of the current key frame if processing one or until
			// the next key frame is detected. This way we should always
			// avoid corruption of the last encoded at the expense of
			// small delay in exiting.
			if(stopRequested && !isStopping) 
			{
				ofLogVerbose(__func__) << "Exit signal detected, waiting for next key frame boundry before exiting...";
				isStopping = true;
				isKeyframeValid = encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME;
			}
			if(isStopping && (isKeyframeValid ^ (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME))) 
			{
				ofLogVerbose(__func__) << "Key frame boundry reached, exiting loop...";
				writeFile();
				doFillBuffer = false;
			}else 
			{
				recordingFileBuffer.append((const char*) encoderOutputBuffer->pBuffer + encoderOutputBuffer->nOffset, encoderOutputBuffer->nFilledLen);
				//ofLogVerbose(__func__) << "encoderOutputBuffer->nFilledLen: " << encoderOutputBuffer->nFilledLen;
				doFillBuffer = true;
			}
		}
		// Buffer flushed, request a new buffer to be filled by the encoder component
		if(doFillBuffer) 
		{
			doFillBuffer	= false;
			bufferAvailable = false;
			OMX_ERRORTYPE error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
			if(error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);
				//close();
               //stopThread();
				
			}
		}
	}
}

void BaseEngine::stopRecording()
{
	
	if(omxCameraSettings.doRecording)
	{
		lock();
            stopRequested = true;
            writeFile();
		unlock();
	}
	
}

OMXCameraSettings& BaseEngine::getSettings()
{
    return omxCameraSettings;
}

void BaseEngine::writeFile()
{
	//format is raw H264 NAL Units
	ofLogVerbose(__func__) << "START";
	stopThread();
	ofLogVerbose(__func__) << "THREAD STOPPED";
	stringstream fileName;
	fileName << ofGetTimestampString() << "_";
	
	fileName << omxCameraSettings.width << "x";
	fileName << omxCameraSettings.height << "_";
	fileName << omxCameraSettings.framerate << "fps_";
	
	fileName << numMBps << "MBps_";
	
	fileName << recordedFrameCounter << "numFrames";
	
	string mkvFilePath = fileName.str() + ".mkv";
	
	fileName << ".h264";
	
	string filePath;
	
	if (omxCameraSettings.recordingFilePath == "") 
	{
		filePath = ofToDataPath(fileName.str(), true);
	}else
	{
		filePath = omxCameraSettings.recordingFilePath;
	}
	
	didWriteFile = ofBufferToFile(filePath, recordingFileBuffer, true);
	if(didWriteFile)
	{
		ofLogVerbose(__func__) << filePath  << " WRITE PASS";
	}
    else
	{
		ofLogVerbose(__func__) << filePath << " FAIL";
	}
    recordedFrameCounter = 0;
}

