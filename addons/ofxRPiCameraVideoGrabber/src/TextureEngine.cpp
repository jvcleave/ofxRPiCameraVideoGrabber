/*
 *  TextureEngine.cpp
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 1/7/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#include "TextureEngine.h"

TextureEngine::TextureEngine()
{
	ready		= false;
	textureID	= 0;
	eglBuffer	= NULL;
	frameCounter = 0;
	
	MEGABYTE_IN_BITS = 8388608;
	numMBps = 2.0;
}

void TextureEngine::setup(OMXCameraSettings omxCameraSettings)
{
	this->omxCameraSettings = omxCameraSettings;
	generateEGLImage();
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &TextureEngine::cameraEventHandlerCallback;
	
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
	
	
}
void TextureEngine::generateEGLImage()
{
	
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	display = appEGLWindow->getEglDisplay();
	context = appEGLWindow->getEglContext();
	
	
	tex.allocate(omxCameraSettings.width, omxCameraSettings.height, GL_RGBA);
	//tex.getTextureData().bFlipTexture = true;
	
	tex.setTextureWrap(GL_REPEAT, GL_REPEAT);
	textureID = tex.getTextureData().textureID;
	
	glEnable(GL_TEXTURE_2D);
	
	// setup first texture
	int dataSize = omxCameraSettings.width * omxCameraSettings.height * 4;
	
	GLubyte* pixelData = new GLubyte [dataSize];
	
	
    memset(pixelData, 0xff, dataSize);  // white texture, opaque
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, omxCameraSettings.width, omxCameraSettings.height, 0,
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
		ofLogError()	<< "Create EGLImage FAIL";
		return;
	}
	else
	{
		ofLogVerbose(__func__)	<< "Create EGLImage PASS";
	}
}


OMX_ERRORTYPE TextureEngine::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	/*ofLog(OF_LOG_VERBOSE, 
	 "TextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
	 __func__, eEvent, nData1, nData2, pEventData);*/
	TextureEngine *grabber = static_cast<TextureEngine*>(pAppData);
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

OMX_ERRORTYPE TextureEngine::splitterEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	return OMX_ErrorNone;
}



OMX_ERRORTYPE TextureEngine::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	return OMX_ErrorNone;
}


OMX_ERRORTYPE TextureEngine::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose(__func__) << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE TextureEngine::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	TextureEngine *grabber = static_cast<TextureEngine*>(pAppData);
	grabber->frameCounter++;
	OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
	return error;
}

OMX_ERRORTYPE TextureEngine::onCameraEventParamOrConfigChanged()
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
	
	//Set up video splitter
	OMX_CALLBACKTYPE splitterCallbacks;
	splitterCallbacks.EventHandler    = &TextureEngine::splitterEventHandlerCallback;
	//splitterCallbacks.EmptyBufferDone	= &TextureEngine::renderEmptyBufferDone;
	//splitterCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
	
	
	
	string componentName = "OMX.broadcom.video_splitter";
	OMX_GetHandle(&splitter, (OMX_STRING)componentName.c_str(), this , &splitterCallbacks);
	OMXCameraUtils::disableAllPortsForComponent(&splitter);
	
	//Set splitter to Idle
	error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}else 
	{
		ofLogVerbose() << "splitter OMX_SendCommand OMX_StateIdle PASS";
	}

	
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler	= &TextureEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &TextureEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
	
	componentName = "OMX.broadcom.egl_render";
	
	OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
	OMXCameraUtils::disableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	

	//Create encoder
	
	OMX_CALLBACKTYPE encoderCallbacks;
	encoderCallbacks.EventHandler		= &TextureEngine::encoderEventHandlerCallback;
	encoderCallbacks.EmptyBufferDone	= &TextureEngine::encoderEmptyBufferDone;
	encoderCallbacks.FillBufferDone		= &TextureEngine::encoderFillBufferDone;
	
	
	string encoderComponentName = "OMX.broadcom.video_encode";
	
	error =OMX_GetHandle(&encoder, (OMX_STRING)encoderComponentName.c_str(), this , &encoderCallbacks);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_GetHandle FAIL error: 0x%08x", error);
	}
	OMXCameraUtils::disableAllPortsForComponent(&encoder);
	
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
	
	
	//Create camera->splitter Tunnel
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, splitter, VIDEO_SPLITTER_INPUT_PORT);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera->splitter OMX_SetupTunnel FAIL error: 0x%08x", error);
	}
	
	// Tunnel splitter2 output port and encoder input port
	error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT2, encoder, VIDEO_ENCODE_INPUT_PORT);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "CAMERA_OUTPUT_PORT->VIDEO_ENCODE_INPUT_PORT OMX_SetupTunnel FAIL error: 0x%08x", error);
	}
	
	
	//Create splitter->egl_render Tunnel
	error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT1, render, EGL_RENDER_INPUT_PORT);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter->egl_render OMX_SetupTunnel FAIL error: 0x%08x", error);
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
		ofLog(OF_LOG_ERROR, "camera enable output port FAIL error: 0x%08x", error);
	}
	
	//Enable splitter input port
	error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter enable input port FAIL error: 0x%08x", error);
	}
	
	//Enable splitter output port
	error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_OUTPUT_PORT1, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter enable output port 1 FAIL error: 0x%08x", error);
	}
	
	//Enable splitter output2 port
	error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_OUTPUT_PORT2, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter enable output port 2 FAIL error: 0x%08x", error);
	}
	
	
	//Enable render output port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render enable output port FAIL error: 0x%08x", error);
	}
	
	//Enable render input port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render enable input port FAIL error: 0x%08x", error);
	}
	
	
	//Enable encoder input port
	error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_CommandPortEnable VIDEO_ENCODE_INPUT_PORT FAIL error: 0x%08x", error);
	}
	
	//Set encoder to Idle
	error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	//Enable encoder output port
	error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_CommandPortEnable VIDEO_ENCODE_OUTPUT_PORT FAIL error: 0x%08x", error);
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
	
	
	//Set renderer to use texture
	error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_UseEGLImage-----> FAIL error: 0x%08x", error);
	}
	
	//Start renderer
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_StateExecuting FAIL error: 0x%08x", error);		
	}
	
	//Start encoder
	error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_StateExecuting FAIL error: 0x%08x", error);		
	}
	
	//Start splitter
	error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "splitter OMX_StateExecuting FAIL error: 0x%08x", error);
	}
	
	//Start camera
	error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_StateExecuting FAIL error: 0x%08x", error);
	}
	
	//start the buffer filling loop
	//once completed the callback will trigger and refill
	error = OMX_FillThisBuffer(render, eglBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(__func__) << "render OMX_FillThisBuffer PASS";
		
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_FillThisBuffer FAIL error: 0x%08x", error);
	}
	error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(__func__) << "encoder OMX_FillThisBuffer PASS";
		
	}else 
	{
		ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);
	}
	ready = true;
	return error;
}


#pragma mark encoder callbacks
OMX_ERRORTYPE TextureEngine::encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	//NonTextureEngine *grabber = static_cast<NonTextureEngine*>(pAppData);
	ofLogVerbose() << "encoderEventHandlerCallback";
	return OMX_ErrorNone;
}


OMX_ERRORTYPE TextureEngine::encoderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose() << "encoderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE TextureEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	TextureEngine *grabber = static_cast<TextureEngine*>(pAppData);
	//grabber->lock();
	//ofLogVerbose(__func__) << "frameCounter: " << grabber->frameCounter;
	//grabber->bufferAvailable = true;
	//grabber->frameCounter++;
	//grabber->unlock();
	return OMX_ErrorNone;
}



#if 0
//untested - I guess could be used to close manually
//app and camera seem to close fine on exit
void TextureEngine::close()
{
	ready = false;
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "camera OMX_StateIdle FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "render OMX_StateIdle FAIL";;
	}
	
	error = OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "camera OMX_CommandFlush FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "render OMX_CommandFlush FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "render OMX_CommandFlush EGL_RENDER_OUTPUT_PORT FAIL";
	}
	
	OMXCameraUtils::disableAllPortsForComponent(&render);
	OMXCameraUtils::disableAllPortsForComponent(&camera);
	
	error = OMX_FreeHandle(render);
	if (error != OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "render OMX_FreeHandle FAIL";
	}
	
	error = OMX_FreeHandle(camera);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "camera OMX_FreeHandle FAIL";
	}
	
	error = OMX_Deinit(); 
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "OMX_Deinit FAIL";
	}
	
	if (eglImage != NULL)  
	{
		eglDestroyImageKHR(display, eglImage);
	}
	
}
#endif
