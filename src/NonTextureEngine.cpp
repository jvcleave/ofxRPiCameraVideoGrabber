/*
 Thanks to Tuomas Jormola for the recording demos!
 
 https://github.com/tjormola/rpi-openmax-demos/
 */

#include "NonTextureEngine.h"
NonTextureEngine::NonTextureEngine()
{
	engineType = NON_TEXTURE_ENGINE;
	frameCounter = 0;		
}

int NonTextureEngine::getFrameCounter()
{
	
	if (!isOpen) 
	{
		return 0;
	}
	OMX_CONFIG_BRCMPORTSTATSTYPE stats;
	
	OMX_INIT_STRUCTURE(stats);
	stats.nPortIndex = VIDEO_RENDER_INPUT_PORT;
	OMX_ERRORTYPE error =OMX_GetParameter(render, OMX_IndexConfigBrcmPortStats, &stats);
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
	return frameCounter;
}

void NonTextureEngine::setup(OMXCameraSettings& omxCameraSettings)
{
	
	this->omxCameraSettings = omxCameraSettings;
	
	if (omxCameraSettings.doRecordingPreview) 
	{
		//validate the settings
		omxCameraSettings.enablePreview();
	}
	
	OMX_ERRORTYPE error = OMX_ErrorNone;

	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &NonTextureEngine::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
    OMX_TRACE(error);

	
	configureCameraResolution();
	
	if (omxCameraSettings.doRecording && omxCameraSettings.doRecordingPreview) 
	{
		OMX_PARAM_PORTDEFINITIONTYPE cameraPreviewPortDefinition;
		OMX_INIT_STRUCTURE(cameraPreviewPortDefinition);
		cameraPreviewPortDefinition.nPortIndex = CAMERA_PREVIEW_PORT;
		error =  OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &cameraPreviewPortDefinition);
        OMX_TRACE(error);

		
		cameraPreviewPortDefinition.format.video.nFrameWidth		= omxCameraSettings.previewWidth;
		cameraPreviewPortDefinition.format.video.nFrameHeight		= omxCameraSettings.previewHeight;
		cameraPreviewPortDefinition.format.video.nStride			= omxCameraSettings.previewWidth;
		error =  OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &cameraPreviewPortDefinition);
        OMX_TRACE(error);

		
	}
	
}


OMX_ERRORTYPE NonTextureEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLog(OF_LOG_VERBOSE, 
	 "NonTextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
	 __func__, eEvent, nData1, nData2, pEventData);
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	//ofLogVerbose(__func__) << OMX_Maps::getInstance().eventTypes[eEvent];
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

	ofLogVerbose(__func__) << "START";
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);

	
	//Enable Camera Output Port
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
    OMX_TRACE(error);

	
	
	
	if (omxCameraSettings.doRecording) 
	{		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Set up renderer
			setupRenderer();
		} 
		
		
		//set up encoder
		OMX_CALLBACKTYPE encoderCallbacks;
		encoderCallbacks.EventHandler		= &BaseEngine::encoderEventHandlerCallback;
		encoderCallbacks.EmptyBufferDone	= &BaseEngine::encoderEmptyBufferDone;
		encoderCallbacks.FillBufferDone		= &NonTextureEngine::encoderFillBufferDone;
		
		
		string encoderComponentName = "OMX.broadcom.video_encode";
		
		error =OMX_GetHandle(&encoder, (OMX_STRING)encoderComponentName.c_str(), this , &encoderCallbacks);
        OMX_TRACE(error);

		
		configureEncoder();
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Create camera->video_render Tunnel
			error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, VIDEO_RENDER_INPUT_PORT);
            OMX_TRACE(error);

		}

		// Tunnel camera video output port and encoder input port
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, encoder, VIDEO_ENCODE_INPUT_PORT);
        OMX_TRACE(error);


		
		//Set encoder to Idle
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);

		
		//Set camera to Idle
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);

		
		if (omxCameraSettings.doRecordingPreview)
		{
			//Enable camera preview port
			error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_PREVIEW_PORT, NULL);
            OMX_TRACE(error);

		}
	
		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
        OMX_TRACE(error);

		
		//Enable encoder input port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_INPUT_PORT, NULL);
        OMX_TRACE(error);

		
		//Enable encoder output port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_OUTPUT_PORT, NULL);
        OMX_TRACE(error);

		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Enable render input port
			error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
            OMX_TRACE(error);

		}

		OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
		OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
		encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
		error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
        OMX_TRACE(error);


		error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, VIDEO_ENCODE_OUTPUT_PORT, NULL, encoderOutputPortDefinition.nBufferSize);
        OMX_TRACE(error);

		

		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error);

		
		//Start encoder
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error);

		
		if (omxCameraSettings.doRecordingPreview) 
		{
			
			//Start renderer
			error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
			if (error != OMX_ErrorNone) 
			{
				ofLog(OF_LOG_ERROR, "render OMX_StateExecuting FAIL error: 0x%08x", error);		
			}
			
			setupDisplay();
			
		}
		
		
		error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        OMX_TRACE(error);

		
		bool doThreadBlocking	= true;
		startThread(doThreadBlocking);
		
	}else 
	{
		setupRenderer();
		
		//Create camera->video_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, VIDEO_RENDER_INPUT_PORT);
        OMX_TRACE(error);

		
		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
        OMX_TRACE(error);

		
		//Enable render input port
		error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
        OMX_TRACE(error);

		//Start renderer
		error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error);

		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error);

		setupDisplay();
				
	}

	isOpen = true;
	return error;
}

OMX_ERRORTYPE NonTextureEngine::setupDisplay()
{

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
	
	OMX_ERRORTYPE error  = OMX_SetParameter(render, OMX_IndexConfigDisplayRegion, &region);
    OMX_TRACE(error);

	
	return error;
	
}

OMX_ERRORTYPE NonTextureEngine::setupRenderer()
{
	//Set up renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &BaseEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &BaseEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &BaseEngine::renderFillBufferDone;
	
	string renderComponentName = "OMX.broadcom.video_render";
	
	OMX_GetHandle(&render, (OMX_STRING)renderComponentName.c_str(), this , &renderCallbacks);
	DisableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	OMX_ERRORTYPE error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);

	return error;
}


OMX_ERRORTYPE NonTextureEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	grabber->lock();
		//ofLogVerbose(__func__) << "recordedFrameCounter: " << grabber->recordedFrameCounter;
		grabber->bufferAvailable = true;
		grabber->recordedFrameCounter++;
	grabber->unlock();
	return OMX_ErrorNone;
}




