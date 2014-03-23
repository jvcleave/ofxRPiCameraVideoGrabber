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

BaseEngine::~BaseEngine()
{
	if(isOpen)
	{
		close();
	}
}

void BaseEngine::configureCameraResolution()
{
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMXCameraUtils::disableAllPortsForComponent(&camera);
	
	OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
	OMX_INIT_STRUCTURE(cameraCallback);
	cameraCallback.nPortIndex	=	OMX_ALL;
	cameraCallback.nIndex		=	OMX_IndexParamCameraDeviceNumber;
	cameraCallback.bEnable		=	OMX_TRUE;
	
	OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
	
	//Set the camera (always 0)
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex	= OMX_ALL;
	device.nU32			= 0;
	
	OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
	
	//Set the resolution
	OMX_PARAM_PORTDEFINITIONTYPE cameraOutputPortDefinition;
	OMX_INIT_STRUCTURE(cameraOutputPortDefinition);
	cameraOutputPortDefinition.nPortIndex = CAMERA_OUTPUT_PORT;
	
	error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &cameraOutputPortDefinition);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
	}else 
	{
		ofLogVerbose(__func__) << "camera OMX_GetParameter OMX_IndexParamPortDefinition PASS";
	}
	
	cameraOutputPortDefinition.format.video.nFrameWidth		= omxCameraSettings.width;
    cameraOutputPortDefinition.format.video.nFrameHeight	= omxCameraSettings.height;
	cameraOutputPortDefinition.format.video.xFramerate		= omxCameraSettings.framerate << 16; //currently always 30
    cameraOutputPortDefinition.format.video.nStride			= omxCameraSettings.width;
	//cameraOutputPortDefinition.format.video.nSliceHeight	= omxCameraSettings.height;
	
	error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraOutputPortDefinition);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "cameraOutputPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "cameraOutputPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		
	}
	
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
	
	OMXCameraUtils::disableAllPortsForComponent(&encoder);
	
	// Encoder input port definition is done automatically upon tunneling
	
	// Configure video format emitted by encoder output port
	OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
	OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
	encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
	}else 
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
	
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		
	}
	
	
	
	// Configure encoding bitrate
	OMX_VIDEO_PARAM_BITRATETYPE encodingBitrate;
	OMX_INIT_STRUCTURE(encodingBitrate);
	encodingBitrate.eControlRate = OMX_Video_ControlRateVariable;
	//encodingBitrate.eControlRate = OMX_Video_ControlRateConstant;
	
	encodingBitrate.nTargetBitrate = recordingBitRate;
	encodingBitrate.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	
	error = OMX_SetParameter(encoder, OMX_IndexParamVideoBitrate, &encodingBitrate);
	
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_SetParameter OMX_IndexParamVideoBitrate FAIL error: 0x%08x", error);
		
	}
	// Configure encoding format
	OMX_VIDEO_PARAM_PORTFORMATTYPE encodingFormat;
	OMX_INIT_STRUCTURE(encodingFormat);
	encodingFormat.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
	encodingFormat.eCompressionFormat = OMX_VIDEO_CodingAVC;
	
	error = OMX_SetParameter(encoder, OMX_IndexParamVideoPortFormat, &encodingFormat);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_SetParameter OMX_IndexParamVideoPortFormat FAIL error: 0x%08x", error);
		
	}
	error = OMX_GetParameter(encoder, OMX_IndexParamVideoPortFormat, &encodingFormat);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "CHECK: encodingFormat.eColorFormat: " << OMX_Maps::getInstance().colorFormatTypes[encodingFormat.eColorFormat];

	}

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
				close();
				
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


void BaseEngine::close()
{
	ofLogVerbose(__func__) << "START";
	if(omxCameraSettings.doRecording)
	{
		//encoderOutputBuffer->nFlags = OMX_BUFFERFLAG_EOS;
		//OMX_FillThisBuffer(encoder, encoderOutputBuffer);
	}else 
	{
		//may have to revisit this if creating new instances of the videograbber
		//otherwise OMX components seem smart enough to clean up themselves on destruction
		ofLogVerbose(__func__) << "END - just exiting";
		isOpen = false;
		return;
	}
	
	
	if(omxCameraSettings.doRecording && !didWriteFile)
	{
		writeFile();
		
	}
	isOpen = false;
	return;
	ofLogVerbose(__func__) << "OMX BREAKDOWN START";
	
	OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	if(omxCameraSettings.doRecording)
	{
		OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_INPUT_PORT, NULL);
		OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_OUTPUT_PORT, NULL);
	}
	
	if(omxCameraSettings.doRecording)
	{
		OMXCameraUtils::disableAllPortsForComponent(&encoder);
	}
	OMXCameraUtils::disableAllPortsForComponent(&camera);
	
	if(omxCameraSettings.doRecording)
	{
		OMX_FreeBuffer(encoder, VIDEO_ENCODE_OUTPUT_PORT, encoderOutputBuffer);
	}
	
	OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	
	if(omxCameraSettings.doRecording)
	{
		OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
	}
	
	OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	
	if(omxCameraSettings.doRecording)
	{
		OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	}
	
	OMX_FreeHandle(camera);
	if(omxCameraSettings.doRecording)
	{
		OMX_FreeHandle(encoder);
	}
	
	ofLogVerbose(__func__) << "OMX BREAKDOWN END";
	ofLogVerbose(__func__) << " END";
	isOpen = false;
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
		if (omxCameraSettings.doConvertToMKV) 
		{
			ofFile mkvmerge("/usr/bin/mkvmerge");
			if(mkvmerge.exists())
			{
				string mkvmergePath = ofToDataPath("/usr/bin/mkvmerge", true);
				ofLogVerbose(__func__) << filePath << " SUCCESS";
				stringstream commandString;
				commandString << "/usr/bin/mkvmerge -o ";
				commandString << ofToDataPath(mkvFilePath, true);
				commandString << " " << filePath;
				commandString << " &";
				string commandName = commandString.str();
				ofLogVerbose(__func__) << "commandName: " << commandName;
				//ofSystem(commandName);
				
				int commandResult = system(commandName.c_str());
				ofLogVerbose(__func__) << "commandResult: " << commandResult;
			}else 
			{
				ofLogError(__func__) << "COULD NOT FIND mkvmerge, try: sudo apt-get install mkvtoolnix";
			}
			
		}
		
		
	}else
	{
		ofLogVerbose(__func__) << filePath << " FAIL";
	}
}

