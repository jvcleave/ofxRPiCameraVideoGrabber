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
		ofLogError(__func__) << "error OMX_CONFIG_BRCMPORTSTATSTYPE FAIL " << omxErrorToString(error);
		//frameCounter = 0;
	}
	return frameCounter;
}

void NonTextureEngine::setup(OMXCameraSettings& omxCameraSettings_)
{
	
	omxCameraSettings = omxCameraSettings_;
	
	if (omxCameraSettings.doRecordingPreview) 
	{
		//validate the settings
		omxCameraSettings.enablePreview();
	}
	
	OMX_ERRORTYPE error = OMX_ErrorNone;

	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &NonTextureEngine::cameraEventHandlerCallback;
	
	
	error = OMX_GetHandle(&camera, OMX_CAMERA, this , &cameraCallbacks);
    OMX_TRACE(error, "camera->OMX_GetHandle");

	
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


OMX_ERRORTYPE NonTextureEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                                           OMX_PTR pAppData, 
                                                           OMX_EVENTTYPE eEvent, 
                                                           OMX_U32 nData1, 
                                                           OMX_U32 nData2, 
                                                           OMX_PTR pEventData)
{
	/*ofLog(OF_LOG_VERBOSE, 
	 "NonTextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
	 __func__, eEvent, nData1, nData2, pEventData);*/

    NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
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

	
    OMX_ERRORTYPE error;
    
    error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
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
		
		
		
		error =OMX_GetHandle(&encoder, OMX_VIDEO_ENCODER, this , &encoderCallbacks);
        OMX_TRACE(error);

		
		configureEncoder();
		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Create camera->video_render Tunnel
			error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, render, VIDEO_RENDER_INPUT_PORT);
            OMX_TRACE(error, "CAMERA_PREVIEW_PORT->VIDEO_RENDER_INPUT_PORT");

		}

		// Tunnel camera video output port and encoder input port
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, encoder, ENCODER_INPUT_PORT);
        OMX_TRACE(error, "CAMERA_OUTPUT_PORT->ENCODER_INPUT_PORT");


		
		//Set encoder to Idle
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
		OMX_TRACE(error, "encoder->OMX_StateIdle");
		
		//Set camera to Idle
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
		OMX_TRACE(error, "camera->OMX_StateIdle");
		
		if (omxCameraSettings.doRecordingPreview)
		{
			//Enable camera preview port
			error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_PREVIEW_PORT, NULL);
            OMX_TRACE(error, "camera CAMERA_PREVIEW_PORT Enable");

		}
	
		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
        OMX_TRACE(error, "camera CAMERA_OUTPUT_PORT Enable");

		
		//Enable encoder input port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, ENCODER_INPUT_PORT, NULL);
        OMX_TRACE(error, "encoder ENCODER_INPUT_PORT Enable");

		
		//Enable encoder output port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, ENCODER_OUTPUT_PORT, NULL);
        OMX_TRACE(error, "encoder ENCODER_OUTPUT_PORT Enable");

		
		if (omxCameraSettings.doRecordingPreview) 
		{
			//Enable render input port
			error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
            OMX_TRACE(error, "render VIDEO_RENDER_INPUT_PORT Enable");

		}

		OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
		OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
		encoderOutputPortDefinition.nPortIndex = ENCODER_OUTPUT_PORT;
		error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);


		error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, ENCODER_OUTPUT_PORT, NULL, encoderOutputPortDefinition.nBufferSize);
        OMX_TRACE(error, "encoder OMX_AllocateBuffer");

		

		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error, "camera OMX_StateExecuting");

		
		//Start encoder
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error, "encoder OMX_StateExecuting");

		
		if (omxCameraSettings.doRecordingPreview) 
		{
			
			//Start renderer
			error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
            OMX_TRACE(error, "render OMX_StateExecuting");
            
			setupDisplay();
			
		}
		
		
		error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        OMX_TRACE(error, "encoder OMX_FillThisBuffer");

		if (error != OMX_ErrorNone) 
		{
			ofLogError(__func__) << "encoder OMX_FillThisBuffer FAIL " << omxErrorToString(error);		
		}
		
		bool doThreadBlocking	= true;
		startThread(doThreadBlocking);
		
	}else 
	{
		setupRenderer();
		
		
		//Create camera->video_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, VIDEO_RENDER_INPUT_PORT);
        OMX_TRACE(error, "CAMERA_OUTPUT_PORT->VIDEO_RENDER_INPUT_PORT");

		//Enable camera output port
		error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
        OMX_TRACE(error, "camera CAMERA_OUTPUT_PORT Enable");

		
		//Enable render input port
		error = OMX_SendCommand(render, OMX_CommandPortEnable, VIDEO_RENDER_INPUT_PORT, NULL);
        OMX_TRACE(error, "render VIDEO_RENDER_INPUT_PORT Enable");

		
		//Start renderer
		error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error, "render OMX_StateExecuting");

		
		//Start camera
		error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
        OMX_TRACE(error, "camera OMX_StateExecuting");

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
	
    OMX_TRACE(error, "render OMX_IndexConfigDisplayRegion");

	
	return error;
	
}

OMX_ERRORTYPE NonTextureEngine::setupRenderer()
{
	//Set up renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &BaseEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &BaseEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &BaseEngine::renderFillBufferDone;
		
	OMX_GetHandle(&render, OMX_VIDEO_RENDER, this , &renderCallbacks);
	DisableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	OMX_ERRORTYPE error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error, "render OMX_StateIdle");

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




