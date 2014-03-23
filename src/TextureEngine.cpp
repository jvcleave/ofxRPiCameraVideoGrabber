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
	isOpen		= false;
	textureID	= 0;
	eglBuffer	= NULL;
	renderedFrameCounter = 0;
	recordedFrameCounter = 0;
	
	didWriteFile = false;
	

	numMBps = 2.0;
	
	stopRequested = false;	 
	isStopping = false;
	isKeyframeValid = false;
	doFillBuffer = false;
	bufferAvailable = false;
	engineType = TEXTURE_ENGINE;
}

void TextureEngine::setup(OMXCameraSettings& omxCameraSettings)
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
	
	configureCameraResolution();
	
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



OMX_ERRORTYPE TextureEngine::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	TextureEngine *grabber = static_cast<TextureEngine*>(pAppData);
	grabber->renderedFrameCounter++;
	OMX_ERRORTYPE error = OMX_FillThisBuffer(hComponent, pBuffer);
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
	
	if(omxCameraSettings.doRecording)
	{
		//Set up video splitter
		OMX_CALLBACKTYPE splitterCallbacks;
		splitterCallbacks.EventHandler    = &BaseEngine::splitterEventHandlerCallback;

		string splitterComponentName = "OMX.broadcom.video_splitter";
		OMX_GetHandle(&splitter, (OMX_STRING)splitterComponentName.c_str(), this , &splitterCallbacks);
		OMXCameraUtils::disableAllPortsForComponent(&splitter);
		
		//Set splitter to Idle
		error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "splitter OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
		}else 
		{
			ofLogVerbose(__func__) << "splitter OMX_SendCommand OMX_StateIdle PASS";
		}
	}

	
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler	= &BaseEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &BaseEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
	
	string renderComponentName = "OMX.broadcom.egl_render";
	
	OMX_GetHandle(&render, (OMX_STRING)renderComponentName.c_str(), this , &renderCallbacks);
	OMXCameraUtils::disableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	if(omxCameraSettings.doRecording)
	{
		//Create encoder
		
		OMX_CALLBACKTYPE encoderCallbacks;
		encoderCallbacks.EventHandler		= &BaseEngine::encoderEventHandlerCallback;
		encoderCallbacks.EmptyBufferDone	= &BaseEngine::encoderEmptyBufferDone;
		encoderCallbacks.FillBufferDone		= &TextureEngine::encoderFillBufferDone;
		
		
		string encoderComponentName = "OMX.broadcom.video_encode";
		
		error =OMX_GetHandle(&encoder, (OMX_STRING)encoderComponentName.c_str(), this , &encoderCallbacks);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_GetHandle FAIL error: 0x%08x", error);
		}
		
		configureEncoder();
		
	}
	
	//Create camera->splitter Tunnel
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, splitter, VIDEO_SPLITTER_INPUT_PORT);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera->splitter OMX_SetupTunnel FAIL error: 0x%08x", error);
	}
	
	if(omxCameraSettings.doRecording)
	{
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
	}else 
	{
		//Create camera->egl_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "camera->egl_render OMX_SetupTunnel FAIL error: 0x%08x", error);
		}
	}
	
	//Enable camera output port
	error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera enable output port FAIL error: 0x%08x", error);
	}
	
	if(omxCameraSettings.doRecording)
	{
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
	
	if(omxCameraSettings.doRecording)
	{
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
		
		// Configure encoder output buffer
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
			
		}

		error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, VIDEO_ENCODE_OUTPUT_PORT, NULL, encoderOutputPortDefinition.nBufferSize);
		if (error != OMX_ErrorNone) 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_AllocateBuffer VIDEO_ENCODE_OUTPUT_PORT FAIL error: 0x%08x", error);
			
		}
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
	
	if(omxCameraSettings.doRecording)
	{
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
	
	if(omxCameraSettings.doRecording)
	{
		error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
		if(error == OMX_ErrorNone)
		{
			ofLogVerbose(__func__) << "encoder OMX_FillThisBuffer PASS";
			
		}else 
		{
			ofLog(OF_LOG_ERROR, "encoder OMX_FillThisBuffer FAIL error: 0x%08x", error);
		}
		bool doThreadBlocking	= true;
		bool threadVerboseMode	= false;
		startThread(doThreadBlocking, threadVerboseMode);
	}
	isOpen = true;
	return error;
}






#pragma mark encoder callbacks

OMX_ERRORTYPE TextureEngine::encoderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	TextureEngine *grabber = static_cast<TextureEngine*>(pAppData);
	grabber->lock();
		//ofLogVerbose(__func__) << "recordedFrameCounter: " << grabber->recordedFrameCounter;
		grabber->bufferAvailable = true;
		grabber->recordedFrameCounter++;
	grabber->unlock();
	return OMX_ErrorNone;
}




#if 0
void TextureEngine::close()
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
	ofLogVerbose(__func__) << "END";
	return;
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "camera OMX_StateIdle FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(__func__) << "render OMX_StateIdle FAIL";
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
	isOpen = false;
}

#endif