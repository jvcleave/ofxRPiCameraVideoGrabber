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
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &TextureEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &TextureEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
	
	string componentName = "OMX.broadcom.egl_render";
	
	OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
	OMXCameraUtils::disableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	//Create camera->egl_render Tunnel
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
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
		ready = true;
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_FillThisBuffer FAIL error: 0x%08x", error);
	}
	return error;
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
