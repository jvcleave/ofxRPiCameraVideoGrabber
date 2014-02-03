/*
 Thanks to Tuomas Jormola for the recording demos!
 
 https://github.com/tjormola/rpi-openmax-demos/
 */

#include "NonTextureEngine.h"
NonTextureEngine::NonTextureEngine()
{
	isOpen		= false;
	frameCounter = 0;
	
	didWriteFile = false;
	
	MEGABYTE_IN_BITS = 8388608;
	numMBps = 2.0;
	
	stopRequested = false;	 
	isStopping = false;
	isKeyframeValid = false;
	doFillBuffer = false;
	bufferAvailable = false;
			
}

void NonTextureEngine::setup(OMXCameraSettings omxCameraSettings)
{
	
	
	this->omxCameraSettings = omxCameraSettings;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;

	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &NonTextureEngine::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL error: 0x%08x", error);
	}
	
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
		ofLogVerbose() << "camera OMX_GetParameter OMX_IndexParamPortDefinition PASS";
	}
	
	cameraOutputPortDefinition.format.video.nFrameWidth		= omxCameraSettings.width;
    cameraOutputPortDefinition.format.video.nFrameHeight	= omxCameraSettings.height;
	cameraOutputPortDefinition.format.video.xFramerate		= omxCameraSettings.framerate << 16; //currently always 30
    cameraOutputPortDefinition.format.video.nStride			= omxCameraSettings.width;
	//cameraOutputPortDefinition.format.video.nSliceHeight	= omxCameraSettings.height;
	
	error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraOutputPortDefinition);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "cameraOutputPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "cameraOutputPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);

	}
	
	OMX_PARAM_PORTDEFINITIONTYPE cameraPreviewPortDefinition;
	OMX_INIT_STRUCTURE(cameraPreviewPortDefinition);
	cameraPreviewPortDefinition.nPortIndex = CAMERA_PREVIEW_PORT;
	error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &cameraPreviewPortDefinition);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "cameraPreviewPortDefinition  OMX_GetParameter OMX_IndexParamPortDefinition PASS";
		
		/*switch(cameraPreviewPortDefinition.eDomain)
		{
			case OMX_PortDomainVideo:
			{
				ofLogVerbose() << "OMX_VIDEO_PORTDEFINITIONTYPE";
				stringstream videoInfo;
				
				videoInfo << "nFrameWidth: "	<< cameraPreviewPortDefinition.format.video.nFrameWidth			<< "\n";
                videoInfo << "nFrameHeight: "	<< cameraPreviewPortDefinition.format.video.nFrameHeight		<< "\n";
                videoInfo << "nStride: "		<< cameraPreviewPortDefinition.format.video.nStride				<< "\n";
                videoInfo << "nSliceHeight: "	<< cameraPreviewPortDefinition.format.video.nSliceHeight		<< "\n";
                videoInfo << "nBitrate: "		<< cameraPreviewPortDefinition.format.video.nBitrate			<< "\n";
				
				ofLogVerbose(__func__) << "videoInfo: \n" << videoInfo.str();
				
				break;
			}
			case OMX_PortDomainImage:
			{
				ofLogVerbose() << "OMX_IMAGE_PORTDEFINITIONTYPE";
				ofLogVerbose() << "OMX_VIDEO_PORTDEFINITIONTYPE";
				stringstream videoInfo;
				
				videoInfo << "nFrameWidth: "	<< cameraPreviewPortDefinition.format.video.nFrameWidth			<< "\n";
                videoInfo << "nFrameHeight: "	<< cameraPreviewPortDefinition.format.video.nFrameHeight		<< "\n";
                videoInfo << "nStride: "		<< cameraPreviewPortDefinition.format.video.nStride				<< "\n";
                videoInfo << "nSliceHeight: "	<< cameraPreviewPortDefinition.format.video.nSliceHeight		<< "\n";
                videoInfo << "nBitrate: "		<< cameraPreviewPortDefinition.format.video.nBitrate			<< "\n";
				
				ofLogVerbose(__func__) << "videoInfo: \n" << videoInfo.str();
				break;
			}
			default:
			{
				ofLogVerbose() << "NOT DEFINED";
			}
		}*/
		
		//Set the preview to the same size as the recording
		/*cameraPreviewPortDefinition.format.video = cameraOutputPortDefinition.format.video;
		error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraPreviewPortDefinition);
		if(error == OMX_ErrorNone) 
		{
			ofLogVerbose() << "cameraPreviewPortDefinition  OMX_SetParameter OMX_IndexParamPortDefinition PASS";
			
		}else 
		{
			ofLog(OF_LOG_ERROR, "cameraPreviewPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}*/
		
	}else 
	{
		ofLog(OF_LOG_ERROR, "cameraPreviewPortDefinition OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);

	}
	cameraPreviewPortDefinition.format.video.nFrameWidth		= omxCameraSettings.previewWidth;
	cameraPreviewPortDefinition.format.video.nFrameHeight		= omxCameraSettings.previewHeight;
	cameraPreviewPortDefinition.format.video.nStride			= omxCameraSettings.previewWidth;
	//cameraPreviewPortDefinition.format.video.nSliceHeight		= omxCameraSettings.previewHeight;
	error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraPreviewPortDefinition);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "cameraPreviewPortDefinition  OMX_SetParameter OMX_IndexParamPortDefinition PASS";
		
	}else 
	{
		ofLog(OF_LOG_ERROR, "cameraPreviewPortDefinition OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
	}

	
	//Set the framerate 
	/*OMX_CONFIG_FRAMERATETYPE framerateConfig;
	OMX_INIT_STRUCTURE(framerateConfig);
	framerateConfig.nPortIndex = CAMERA_OUTPUT_PORT;
	//framerateConfig.xEncodeFramerate = omxCameraSettings.framerate * (1<<16); //Q16 format - 25fps
	//framerateConfig.xEncodeFramerate = omxCameraSettings.framerate << 16;
	error =  OMX_SetConfig(camera, OMX_IndexConfigVideoFramerate, &framerateConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexParamVideoPortFormat FAIL error: 0x%08x", error);
	}else 
	{
		ofLogVerbose() << "camera OMX_SetConfig OMX_IndexParamVideoPortFormat PASS";
	}*/
	
	
	bool doThreadBlocking	= true;
	bool threadVerboseMode	= false;
	startThread(doThreadBlocking, threadVerboseMode);
	
}


OMX_ERRORTYPE NonTextureEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLog(OF_LOG_VERBOSE, 
	 "NonTextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
	 __func__, eEvent, nData1, nData2, pEventData);
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	//ofLogVerbose(__func__) << grabber->omxMaps.eventTypes[eEvent];
	switch (eEvent) 
	{
		case OMX_EventParamOrConfigChanged:
		{
			
			return grabber->onCameraEventParamOrConfigChanged();
		}			
		default: 
		{
			break;
		}
	}
	return OMX_ErrorNone;
}



OMX_ERRORTYPE NonTextureEngine::onCameraEventParamOrConfigChanged()
{

	ofLogVerbose(__func__) << "onCameraEventParamOrConfigChanged";
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	

	
	//Enable Camera Output Port
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera enable Output Port FAIL error: 0x%08x", error);
	}
	
	
	
	if (omxCameraSettings.doRecording) 
	{		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Set up renderer
			OMX_CALLBACKTYPE renderCallbacks;
			renderCallbacks.EventHandler    = &NonTextureEngine::renderEventHandlerCallback;
			renderCallbacks.EmptyBufferDone	= &NonTextureEngine::renderEmptyBufferDone;
			renderCallbacks.FillBufferDone	= &NonTextureEngine::renderFillBufferDone;
			
			string renderComponentName = "OMX.broadcom.video_render";
			
			OMX_GetHandle(&render, (OMX_STRING)renderComponentName.c_str(), this , &renderCallbacks);
			OMXCameraUtils::disableAllPortsForComponent(&render);
			
			//Set renderer to Idle
			error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
			}
		}else 
		{
			OMX_CALLBACKTYPE nullSinkCallbacks;
			nullSinkCallbacks.EventHandler    = &NonTextureEngine::nullSinkEventHandlerCallback;
			nullSinkCallbacks.EmptyBufferDone	= &NonTextureEngine::nullSinkEmptyBufferDone;
			nullSinkCallbacks.FillBufferDone	= &NonTextureEngine::nullSinkFillBufferDone;
			
			string nullSinkComponentName = "OMX.broadcom.null_sink";
			
			error =OMX_GetHandle(&nullSink, (OMX_STRING)nullSinkComponentName.c_str(), this , &nullSinkCallbacks);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "nullSink OMX_GetHandle FAIL error: 0x%08x", error);
			}
			OMXCameraUtils::disableAllPortsForComponent(&nullSink);
		}

		
		
		OMX_CALLBACKTYPE encoderCallbacks;
		encoderCallbacks.EventHandler    = &NonTextureEngine::encoderEventHandlerCallback;
		encoderCallbacks.EmptyBufferDone	= &NonTextureEngine::encoderEmptyBufferDone;
		encoderCallbacks.FillBufferDone	= &NonTextureEngine::encoderFillBufferDone;
		
		
		string encoderComponentName = "OMX.broadcom.video_encode";
		
		error =OMX_GetHandle(&encoder, (OMX_STRING)encoderComponentName.c_str(), this , &encoderCallbacks);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_GetHandle FAIL error: 0x%08x", error);
		}
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
		}
		
		
		encoderOutputPortDefinition.format.video.nFrameWidth			= omxCameraSettings.width;
		encoderOutputPortDefinition.format.video.nFrameHeight			= omxCameraSettings.height;
		encoderOutputPortDefinition.format.video.xFramerate				= 0; //always 25
		//encoderOutputPortDefinition.format.video.xFramerate				= omxCameraSettings.framerate << 16;
		encoderOutputPortDefinition.format.video.nStride				= omxCameraSettings.width;
		//encoderOutputPortDefinition.format.video.nSliceHeight			= omxCameraSettings.height;
		//encoderOutputPortDefinition.format.video.eCompressionFormat		 = OMX_VIDEO_CodingMPEG4;
		//encoderOutputPortDefinition.format.video.bFlagErrorConcealment = OMX_TRUE;
	

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
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Create camera->video_render Tunnel
			error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, VIDEO_RENDER_INPUT_PORT);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "camera->video_render OMX_SetupTunnel FAIL error: 0x%08x", error);
			}
		}else 
		{
			// Tunnel camera preview output port and null sink input port
			error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, nullSink, NULL_SINK_INPUT_PORT);
			if(error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "CAMERA_PREVIEW_PORT->NULL_SINK_INPUT_PORT OMX_SetupTunnel FAIL error: 0x%08x", error);
			}
		}

		
		
		// Tunnel camera video output port and encoder input port
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, encoder, VIDEO_ENCODE_INPUT_PORT);
		if(error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "CAMERA_OUTPUT_PORT->VIDEO_ENCODE_INPUT_PORT OMX_SetupTunnel FAIL error: 0x%08x", error);
		}

		
		//Set encoder to Idle
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
		}
		
		//Set camera to Idle
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
		}
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//add renderer
		}else
		{
			//Set nullSink to Idle
			error = OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateIdle, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "nullSink OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
			}
		}
		
		
		
		
		//Enable camera preview port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_PREVIEW_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_CommandPortEnable CAMERA_PREVIEW_PORT FAIL error: 0x%08x", error);
		}
		
		//Enable camera input port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_INPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_CommandPortEnable CAMERA_INPUT_PORT FAIL error: 0x%08x", error);
		}
		
		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_CommandPortEnable CAMERA_OUTPUT_PORT FAIL error: 0x%08x", error);
		}
		
		//Enable encoder input port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_INPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_CommandPortEnable VIDEO_ENCODE_INPUT_PORT FAIL error: 0x%08x", error);
		}
		
		//Enable encoder output port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_OUTPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_CommandPortEnable VIDEO_ENCODE_OUTPUT_PORT FAIL error: 0x%08x", error);
		}
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Enable render input port
			error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "render enable output port FAIL error: 0x%08x", error);
			}
			
			
		}else 
		{
			//Enable nullSink port
			error = OMX_SendCommand(nullSink, OMX_CommandPortEnable, NULL_SINK_INPUT_PORT, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "nullSink OMX_CommandPortEnable NULL_SINK_INPUT_PORT FAIL error: 0x%08x", error);
			}
		}

		
		
		// Allocate camera input buffer and encoder output buffer,
		// buffers for tunneled ports are allocated internally by OMX
		OMX_PARAM_PORTDEFINITIONTYPE cameraInputPortDefinition;
		OMX_INIT_STRUCTURE(cameraInputPortDefinition);
		cameraInputPortDefinition.nPortIndex = CAMERA_INPUT_PORT;
		error = OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &cameraInputPortDefinition);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}else 
		{
			ofLogVerbose() << "cameraInputPortDefinition buffer info";
			ofLog(OF_LOG_VERBOSE, 
				  "nBufferCountMin(%u)					\n \
				  nBufferCountActual(%u)				\n \
				  nBufferSize(%u)						\n \
				  nBufferAlignmen(%u) \n", 
				  cameraInputPortDefinition.nBufferCountMin, 
				  cameraInputPortDefinition.nBufferCountActual, 
				  cameraInputPortDefinition.nBufferSize, 
				  cameraInputPortDefinition.nBufferAlignment);
			
		}

		
		error =  OMX_AllocateBuffer(camera, &cameraInputBuffer, CAMERA_INPUT_PORT, NULL, cameraInputPortDefinition.nBufferSize);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_AllocateBuffer CAMERA_INPUT_PORT FAIL error: 0x%08x", error);
		}
		
		
		OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
		encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
		error = OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}else 
		{
			ofLog(OF_LOG_VERBOSE, 
				  "nBufferCountMin(%u)					\n \
				  nBufferCountActual(%u)				\n \
				  nBufferSize(%u)						\n \
				  nBufferAlignmen(%u) \n", 
				  encoderOutputPortDefinition.nBufferCountMin, 
				  encoderOutputPortDefinition.nBufferCountActual, 
				  encoderOutputPortDefinition.nBufferSize, 
				  encoderOutputPortDefinition.nBufferAlignment);
			
		}
		
		
		error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, VIDEO_ENCODE_OUTPUT_PORT, NULL, encoderOutputPortDefinition.nBufferSize);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_AllocateBuffer VIDEO_ENCODE_OUTPUT_PORT FAIL error: 0x%08x", error);
			
		}

		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_StateExecuting FAIL error: 0x%08x", error);
		}
		
		//Start encoder
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_StateExecuting FAIL error: 0x%08x", error);		
		}
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			
			//Start renderer
			error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "render OMX_StateExecuting FAIL error: 0x%08x", error);		
			}
			
			OMX_CONFIG_DISPLAYREGIONTYPE region;
			
			OMX_INIT_STRUCTURE(region);
			region.nPortIndex = VIDEO_RENDER_INPUT_PORT; /* Video render input port */
			
			region.set = (OMX_DISPLAYSETTYPE)(OMX_DISPLAY_SET_DEST_RECT | OMX_DISPLAY_SET_FULLSCREEN | OMX_DISPLAY_SET_NOASPECT);
			
			region.fullscreen = OMX_FALSE;
			region.noaspect = OMX_TRUE;
			
			region.dest_rect.x_offset = 0;
			region.dest_rect.y_offset = 0;
			region.dest_rect.width = omxCameraSettings.width;
			region.dest_rect.height = omxCameraSettings.height;
			
			error = OMX_SetParameter(render, OMX_IndexConfigDisplayRegion, &region);
			
			if(error == OMX_ErrorNone)
			{
				ofLogVerbose(__func__) << "render OMX_IndexConfigDisplayRegion PASS";
			}else 
			{
				ofLog(OF_LOG_ERROR, "render OMX_IndexConfigDisplayRegion FAIL error: 0x%08x", error);
			}
			
			
		}else 
		{
			//Start nullSink
			error = OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateExecuting, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "nullSink OMX_StateExecuting FAIL error: 0x%08x", error);		
			}
		}

		
		
		error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
		
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);		
		}
		
	}else 
	{
		//Set up renderer
		OMX_CALLBACKTYPE renderCallbacks;
		renderCallbacks.EventHandler    = &NonTextureEngine::renderEventHandlerCallback;
		renderCallbacks.EmptyBufferDone	= &NonTextureEngine::renderEmptyBufferDone;
		renderCallbacks.FillBufferDone	= &NonTextureEngine::renderFillBufferDone;
		
		string renderComponentName = "OMX.broadcom.video_render";
		
		OMX_GetHandle(&render, (OMX_STRING)renderComponentName.c_str(), this , &renderCallbacks);
		OMXCameraUtils::disableAllPortsForComponent(&render);
		
		//Set renderer to Idle
		error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
		}
		
		
		//Create camera->video_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, VIDEO_RENDER_INPUT_PORT);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera->video_render OMX_SetupTunnel FAIL error: 0x%08x", error);
		}
		
		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera enable output port FAIL error: 0x%08x", error);
		}
		
		//Enable render input port
		error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "render enable output port FAIL error: 0x%08x", error);
		}
		
		
		//Start renderer
		error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "render OMX_StateExecuting FAIL error: 0x%08x", error);		
		}
		
		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_StateExecuting FAIL error: 0x%08x", error);
		}
		
		OMX_CONFIG_DISPLAYREGIONTYPE region;
		
		OMX_INIT_STRUCTURE(region);
		region.nPortIndex = VIDEO_RENDER_INPUT_PORT; /* Video render input port */
		
		region.set = (OMX_DISPLAYSETTYPE)(OMX_DISPLAY_SET_DEST_RECT | OMX_DISPLAY_SET_FULLSCREEN | OMX_DISPLAY_SET_NOASPECT);
		
		region.fullscreen = OMX_FALSE;
		region.noaspect = OMX_TRUE;
		
		region.dest_rect.x_offset = 0;
		region.dest_rect.y_offset = 0;
		region.dest_rect.width	= omxCameraSettings.width;
		region.dest_rect.height = omxCameraSettings.height;
		
		error = OMX_SetParameter(render, OMX_IndexConfigDisplayRegion, &region);
		
		if(error == OMX_ErrorNone)
		{
			ofLogVerbose(__func__) << "render OMX_IndexConfigDisplayRegion PASS";
		}else 
		{
			ofLog(OF_LOG_ERROR, "render OMX_IndexConfigDisplayRegion FAIL error: 0x%08x", error);
		}
		
	}

	isOpen = true;
	return error;
}

NonTextureEngine::~NonTextureEngine()
{
	if(isOpen)
	{
		close();
	}
}

void NonTextureEngine::stopRecording()
{
	lock();
	if(omxCameraSettings.doRecording)
	{
		stopRequested = true;
		writeFile();
	}
	unlock();
}

void NonTextureEngine::threadedFunction()
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
				ofLogVerbose() << "Exit signal detected, waiting for next key frame boundry before exiting...";
				isStopping = true;
				isKeyframeValid = encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME;
			}
			if(isStopping && (isKeyframeValid ^ (encoderOutputBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME))) 
			{
				ofLogVerbose() << "Key frame boundry reached, exiting loop...";
				writeFile();
				
				doFillBuffer = false;
			}else 
			{
				recordingFileBuffer.append((const char*) encoderOutputBuffer->pBuffer + encoderOutputBuffer->nOffset, encoderOutputBuffer->nFilledLen);
				//ofLogVerbose() << "encoderOutputBuffer->nFilledLen: " << encoderOutputBuffer->nFilledLen;
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

void NonTextureEngine::writeFile()
{
	//format is raw H264 NAL Units
	
	stopThread();
	stringstream fileName;
	fileName << ofGetTimestampString() << "_";
	
	fileName << omxCameraSettings.width << "x";
	fileName << omxCameraSettings.height << "_";
	fileName << omxCameraSettings.framerate << "fps_";
	
	fileName << numMBps << "MBps_";
	
	fileName << frameCounter << "numFrames";
	
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
		if (omxCameraSettings.doConvertToMKV) 
		{
			ofFile mkvmerge("/usr/bin/mkvmerge");
			if(mkvmerge.exists())
			{
				string mkvmergePath = ofToDataPath("/usr/bin/mkvmerge", true);
				ofLogVerbose() << filePath << " SUCCESS";
				stringstream commandString;
				commandString << "/usr/bin/mkvmerge -o ";
				commandString << ofToDataPath(mkvFilePath, true);
				commandString << " " << filePath;
				commandString << " &";
				string commandName = commandString.str();
				ofLogVerbose() << "commandName: " << commandName;
				//ofSystem(commandName);
				
				system(commandName.c_str());
			}
			
		}else 
		{
			ofLogError(__func__) << "COULD NOT FIND mkvmerge, try: sudo apt-get install mkvtoolnix";
		}

		
	}else
	{
		ofLogVerbose() << filePath << " FAIL";
	}
}

void NonTextureEngine::close()
{
	
	if(omxCameraSettings.doRecording && !didWriteFile)
	{
		writeFile();
	}

	encoderOutputBuffer->nFlags = OMX_BUFFERFLAG_EOS;
	OMX_FillThisBuffer(encoder, encoderOutputBuffer);
	OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_INPUT_PORT, NULL);
	OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_PREVIEW_PORT, NULL);
	OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_INPUT_PORT, NULL);
	OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_OUTPUT_PORT, NULL);
	if(!omxCameraSettings.doRecordingPreview) 
	{
		OMX_SendCommand(nullSink, OMX_CommandFlush, NULL_SINK_INPUT_PORT, NULL);
	}
	OMXCameraUtils::disableAllPortsForComponent(&encoder);
	if(!omxCameraSettings.doRecordingPreview) OMXCameraUtils::disableAllPortsForComponent(&nullSink);
	OMXCameraUtils::disableAllPortsForComponent(&camera);
	OMX_FreeBuffer(camera, CAMERA_INPUT_PORT, cameraInputBuffer);
	OMX_FreeBuffer(encoder, VIDEO_ENCODE_OUTPUT_PORT, encoderOutputBuffer);
	OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if(!omxCameraSettings.doRecordingPreview) OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	if(!omxCameraSettings.doRecordingPreview) OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	OMX_FreeHandle(camera);
	OMX_FreeHandle(encoder);
	if(!omxCameraSettings.doRecordingPreview) OMX_FreeHandle(nullSink);

	ofLogVerbose(__func__) << " END";
	isOpen = false;
}

#pragma mark encoder callbacks
OMX_ERRORTYPE NonTextureEngine::encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "encoderEventHandlerCallback";
	return OMX_ErrorNone;
}


OMX_ERRORTYPE NonTextureEngine::encoderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose() << "encoderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE NonTextureEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	grabber->lock();
		//ofLogVerbose(__func__) << "frameCounter: " << grabber->frameCounter;
		grabber->bufferAvailable = true;
		grabber->frameCounter++;
	grabber->unlock();
	return OMX_ErrorNone;
}

#pragma mark nullsink callbacks

OMX_ERRORTYPE NonTextureEngine::nullSinkEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "nullSinkEventHandlerCallback";
	return OMX_ErrorNone;
}


OMX_ERRORTYPE NonTextureEngine::nullSinkEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "nullSinkEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE NonTextureEngine::nullSinkFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "nullSinkFillBufferDone ";
	return OMX_ErrorNone;
}


#pragma mark render callbacks
OMX_ERRORTYPE NonTextureEngine::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "renderEventHandlerCallback";
	return OMX_ErrorNone;
}


OMX_ERRORTYPE NonTextureEngine::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);s
	ofLogVerbose() << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE NonTextureEngine::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	return OMX_ErrorNone;
}

