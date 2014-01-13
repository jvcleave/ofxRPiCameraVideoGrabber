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
	frameCounter = 0;
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
	/*ofLog(OF_LOG_VERBOSE, 
	 "NonTextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
	 __func__, eEvent, nData1, nData2, pEventData);*/
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

OMX_ERRORTYPE NonTextureEngine::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	return OMX_ErrorNone;
}


OMX_ERRORTYPE NonTextureEngine::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose(__func__) << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE NonTextureEngine::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	//grabber->frameCounter++;
	//OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
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
	
	//Set up texture renderer
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
	region.dest_rect.width = ofGetWidth();
	region.dest_rect.height = ofGetHeight();
	
	error = OMX_SetParameter(render, OMX_IndexConfigDisplayRegion, &region);
	
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(__func__) << "render OMX_IndexConfigDisplayRegion PASS";
		//ready = true;
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_IndexConfigDisplayRegion FAIL error: 0x%08x", error);
	}
	ready = true;
	//start the buffer filling loop
	//once completed the callback will trigger and refill
	/*error = OMX_FillThisBuffer(render, eglBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(__func__) << "render OMX_FillThisBuffer PASS";
		ready = true;
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_FillThisBuffer FAIL error: 0x%08x", error);
	}*/
	return error;
}

