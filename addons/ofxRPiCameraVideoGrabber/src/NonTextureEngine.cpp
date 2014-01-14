/*
 *  NonTextureEngine.cpp
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 1/7/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#include "NonTextureEngine.h"

NonTextureEngine::NonTextureEngine()
{
	ready		= false;
	doRecording = true;
	frameCounter = 0;
	want_quit = false;
	fd_out = NULL;
}

void NonTextureEngine::setup(OMXCameraSettings omxCameraSettings)
{
	this->omxCameraSettings = omxCameraSettings;
	
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "OMX_Init PASS";
	}
	
	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &NonTextureEngine::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL error: 0x%08x", error);
	}
	
	OMXCameraUtils::disableAllPortsForComponent(&camera);
	
	
	OMX_VIDEO_PARAM_PORTFORMATTYPE portFormatType;
	OMX_INIT_STRUCTURE(portFormatType);
	portFormatType.nPortIndex = CAMERA_OUTPUT_PORT;
	error = OMX_GetConfig(camera, OMX_IndexParamVideoPortFormat, &portFormatType);
	
	
	
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetConfig OMX_IndexParamVideoPortFormat FAIL error: 0x%08x", error);
	}else 
	{
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
		
		//ofLogVerbose() << "OMX_COLOR_FORMATTYPE is " << omxMaps.colorFormats[portFormatType.eColorFormat]; //OMX_COLOR_FormatYUV420PackedPlanar
		
		//ofLogVerbose() << "OMX_VIDEO_CODINGTYPE is " << omxMaps.videoCodingTypes[portFormatType.eCompressionFormat]; //OMX_VIDEO_CodingUnused
		ofLogVerbose() << "nIndex is " << portFormatType.nIndex;
		ofLogVerbose() << "xFramerate is " << portFormatType.xFramerate;
		
		//OMX_U32 nIndex;
		//OMX_VIDEO_CODINGTYPE eCompressionFormat; 
		//OMX_COLOR_FORMATTYPE eColorFormat;
		//OMX_U32 xFramerate;
	}
	
	
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
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	OMX_INIT_STRUCTURE(portdef);
	portdef.nPortIndex = CAMERA_OUTPUT_PORT;
	
	OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	
	portdef.format.image.nFrameWidth = omxCameraSettings.width;
	portdef.format.image.nFrameHeight = omxCameraSettings.height;
	portdef.format.image.nStride = omxCameraSettings.width;
	
	OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	
	
	//Set the framerate 
	OMX_CONFIG_FRAMERATETYPE framerateConfig;
	OMX_INIT_STRUCTURE(framerateConfig);
	framerateConfig.nPortIndex = CAMERA_OUTPUT_PORT;
	framerateConfig.xEncodeFramerate = omxCameraSettings.framerate << 16; //Q16 format - 25fps
	
	OMX_SetConfig(camera, OMX_IndexConfigVideoFramerate, &framerateConfig);
	
	
	OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
	OMX_INIT_STRUCTURE(exposurevalue);
	exposurevalue.nPortIndex = OMX_ALL;
	error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "OMX_CONFIG_EXPOSUREVALUETYPE PASS";
		stringstream info;
		info << "OMX_METERINGTYPE: " << exposurevalue.eMetering << "\n";
		info << "xEVCompensation: " << exposurevalue.xEVCompensation << "\n";
		info << "nApertureFNumber: " << exposurevalue.nApertureFNumber << "\n";
		info << "bAutoAperture: " << exposurevalue.bAutoAperture << "\n";
		info << "nShutterSpeedMsec: " << exposurevalue.nShutterSpeedMsec << "\n";
		info << "bAutoShutterSpeed: " << exposurevalue.bAutoShutterSpeed << "\n";
		info << "nSensitivity: " << exposurevalue.nSensitivity << "\n";
		info << "bAutoSensitivity: " << exposurevalue.bAutoSensitivity << "\n";
		ofLogVerbose() << "CAMERA EXPOSURE INFO: " << info.str();
		
	}
#if 0
	OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_METERINGTYPE eMetering;
    OMX_S32 xEVCompensation;      /**< Fixed point value stored as Q16 */
    OMX_U32 nApertureFNumber;     /**< e.g. nApertureFNumber = 2 implies "f/2" - Q16 format */
    OMX_BOOL bAutoAperture;		/**< Whether aperture number is defined automatically */
    OMX_U32 nShutterSpeedMsec;    /**< Shutterspeed in milliseconds */ 
    OMX_BOOL bAutoShutterSpeed;	/**< Whether shutter speed is defined automatically */ 
    OMX_U32 nSensitivity;         /**< e.g. nSensitivity = 100 implies "ISO 100" */
    OMX_BOOL bAutoSensitivity;	/**< Whether sensitivity is defined automatically */
#endif

	
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
	
	
	
	if (doRecording) 
	{
		string filePath = ofToDataPath(ofGetTimestampString()+".mp4", true);
		fd_out = fopen(filePath.c_str(), "w+");
		
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
		OMX_PARAM_PORTDEFINITIONTYPE encoder_portdef;
		OMX_INIT_STRUCTURE(encoder_portdef);
		encoder_portdef.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
		error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoder_portdef);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}
		
		
		encoder_portdef.format.video.nFrameWidth  = omxCameraSettings.width;
		encoder_portdef.format.video.nFrameHeight = omxCameraSettings.height;
		encoder_portdef.format.video.xFramerate   = omxCameraSettings.framerate << 16;
		encoder_portdef.format.video.nStride      = omxCameraSettings.width;
		
		
		// Which one is effective, this or the configuration just below?
		encoder_portdef.format.video.nBitrate     = 10000000;
		
		error = OMX_SetParameter(encoder, OMX_IndexParamPortDefinition, &encoder_portdef);
		
		if(error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
			
		}
		// Configure encoding bitrate
		OMX_VIDEO_PARAM_BITRATETYPE encodingBitrate;
		OMX_INIT_STRUCTURE(encodingBitrate);
		encodingBitrate.eControlRate = OMX_Video_ControlRateVariable;
		encodingBitrate.nTargetBitrate = encoder_portdef.format.video.nBitrate;
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
		
		// Tunnel camera preview output port and null sink input port
		error = OMX_SetupTunnel(camera, CAMERA_PREVIEW_PORT, nullSink, NULL_SINK_INPUT_PORT);
		if(error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "CAMERA_PREVIEW_PORT->NULL_SINK_INPUT_PORT OMX_SetupTunnel FAIL error: 0x%08x", error);
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
		
		//Set nullSink to Idle
		error = OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "nullSink OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
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
		
		//Enable nullSink port
		error = OMX_SendCommand(nullSink, OMX_CommandPortEnable, NULL_SINK_INPUT_PORT, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "nullSink OMX_CommandPortEnable NULL_SINK_INPUT_PORT FAIL error: 0x%08x", error);
		}
		
		// Allocate camera input buffer and encoder output buffer,
		// buffers for tunneled ports are allocated internally by OMX
		OMX_PARAM_PORTDEFINITIONTYPE camera_portdef;
		OMX_INIT_STRUCTURE(camera_portdef);
		camera_portdef.nPortIndex = CAMERA_INPUT_PORT;
		error = OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &camera_portdef);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}
		
		error =  OMX_AllocateBuffer(camera, &camera_ppBuffer_in, CAMERA_INPUT_PORT, NULL, camera_portdef.nBufferSize);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera OMX_AllocateBuffer CAMERA_INPUT_PORT FAIL error: 0x%08x", error);
		}
		
		
		OMX_INIT_STRUCTURE(encoder_portdef);
		encoder_portdef.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
		error = OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoder_portdef);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
		}
		
		error =  OMX_AllocateBuffer(encoder, &encoder_ppBuffer_out, VIDEO_ENCODE_OUTPUT_PORT, NULL, encoder_portdef.nBufferSize);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_AllocateBuffer VIDEO_ENCODE_OUTPUT_PORT FAIL error: 0x%08x", error);
		}
		
		
		
		/*
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
		*/
		
		
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
		
		//Start nullSink
		error = OMX_SendCommand(nullSink, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "nullSink OMX_StateExecuting FAIL error: 0x%08x", error);		
		}
		
		error = OMX_FillThisBuffer(encoder, encoder_ppBuffer_out);
		
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
		
		string componentName = "OMX.broadcom.video_render";
		
		OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
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
			ofLog(OF_LOG_ERROR, "camera->egl_render OMX_SetupTunnel FAIL error: 0x%08x", error);
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
		
	}

		ready = true;
		return error;
}

int quit_detected = 0;
int quit_in_keyframe = 0;
int need_next_buffer_to_be_filled = 1;
int encoder_output_buffer_available = 0;


void NonTextureEngine::readFrame()
{
	
    // fill_output_buffer_done_handler() has marked that there's
	// a buffer for us to flush
	if(encoder_output_buffer_available) 
	{
		// Print a message if the user wants to quit, but don't exit
		// the loop until we are certain that we have processed
		// a full frame till end of the frame, i.e. we're at the end
		// of the current key frame if processing one or until
		// the next key frame is detected. This way we should always
		// avoid corruption of the last encoded at the expense of
		// small delay in exiting.
		if(want_quit && !quit_detected) 
		{
			ofLogVerbose() << "Exit signal detected, waiting for next key frame boundry before exiting...";
			quit_detected = 1;
			quit_in_keyframe = encoder_ppBuffer_out->nFlags & OMX_BUFFERFLAG_SYNCFRAME;
		}
		if(quit_detected && (quit_in_keyframe ^ (encoder_ppBuffer_out->nFlags & OMX_BUFFERFLAG_SYNCFRAME))) 
		{
			ofLogVerbose() << "Key frame boundry reached, exiting loop...";
			fclose(fd_out);
			//break;
		}
		// Flush buffer to output file
		size_t output_written = fwrite(encoder_ppBuffer_out->pBuffer + encoder_ppBuffer_out->nOffset, 1, encoder_ppBuffer_out->nFilledLen, fd_out);
		if(output_written != encoder_ppBuffer_out->nFilledLen) 
		{
			ofLogError(__func__) << " Failed to write to output file: " << strerror(errno);
		}
		ofLogVerbose() << "Read from output buffer and wrote to output file: " <<  encoder_ppBuffer_out->nFilledLen <<  " "  << encoder_ppBuffer_out->nAllocLen;
		need_next_buffer_to_be_filled = 1;
	}
	// Buffer flushed, request a new buffer to be filled by the encoder component
	if(need_next_buffer_to_be_filled) 
	{
		need_next_buffer_to_be_filled = 0;
		encoder_output_buffer_available = 0;
		OMX_ERRORTYPE error = OMX_FillThisBuffer(encoder, encoder_ppBuffer_out);
		if(error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);

		}
	}
	
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
	ofLogVerbose(__func__) << "encoderFillBufferDone";
	encoder_output_buffer_available = 1;
	grabber->readFrame();
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
	ofLogVerbose() << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE NonTextureEngine::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	
	grabber->frameCounter++;
	ofLogVerbose(__func__) << "grabber->frameCounter: " << grabber->frameCounter;
	//OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
	return OMX_ErrorNone;
}

