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
	eglBuffer	= NULL;
    eglImage = NULL;

	renderedFrameCounter = 0;
	recordedFrameCounter = 0;
	
	didWriteFile = false;
	

	numMBps = 2.0;
	
	stopRequested = false;	 
	isStopping = false;
	isKeyframeValid = false;
	doFillBuffer = false;
	bufferAvailable = false;
	pixels = NULL;
	doPixels = false;
}

int TextureEngine::getFrameCounter()
{
	return renderedFrameCounter;
	
}

void TextureEngine::setup(OMXCameraSettings omxCameraSettings_)
{
	omxCameraSettings = omxCameraSettings_;
    ofLogVerbose(__func__) << "omxCameraSettings: " << omxCameraSettings.toString();
	generateEGLImage();
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &TextureEngine::cameraEventHandlerCallback;
		
	error = OMX_GetHandle(&camera, OMX_CAMERA, this , &cameraCallbacks);
    OMX_TRACE(error);

    if (omxCameraSettings.enablePixels) 
    {
        enablePixels();
        updatePixels();
    }
	configureCameraResolution();
	
}


void TextureEngine::enablePixels()
{
	doPixels = true;
	
}


void TextureEngine::disablePixels()
{
	doPixels = false;
}


void TextureEngine::updatePixels()
{
	if (!doPixels) 
	{
		return;
	}
	
	if (!fbo.isAllocated()) 
	{
		fbo.allocate(omxCameraSettings.width, omxCameraSettings.height, GL_RGBA);
	}
	int dataSize = omxCameraSettings.width * omxCameraSettings.height * 4;
	if (pixels == NULL)
	{
		pixels = new unsigned char[dataSize];
	}
	fbo.begin();
		ofClear(0, 0, 0, 0);
		texture.draw(0, 0);
		glReadPixels(0,0, omxCameraSettings.width, omxCameraSettings.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);	
	fbo.end();
}

unsigned char * TextureEngine::getPixels()
{
	return pixels;
}


void TextureEngine::generateEGLImage()
{
	
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	display = appEGLWindow->getEglDisplay();
	context = appEGLWindow->getEglContext();
	
	
	texture.allocate(omxCameraSettings.width, omxCameraSettings.height, GL_RGBA);
	//texture.getTextureData().bFlipTexture = true;
	
	texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
    GLuint textureID = texture.getTextureData().textureID;
	
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
    OMX_TRACE(error);
	return error;
}

OMX_ERRORTYPE TextureEngine::onCameraEventParamOrConfigChanged()
{
	ofLogVerbose(__func__) << "onCameraEventParamOrConfigChanged";
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);

	
	//Enable Camera Output Port
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
    OMX_TRACE(error);

	
	if(omxCameraSettings.doRecording)
	{
		//Set up video splitter
		OMX_CALLBACKTYPE splitterCallbacks;
		splitterCallbacks.EventHandler    = &BaseEngine::splitterEventHandlerCallback;

        error = OMX_GetHandle(&splitter, OMX_VIDEO_SPLITTER, this , &splitterCallbacks);
        OMX_TRACE(error);
		error =DisableAllPortsForComponent(&splitter);
        OMX_TRACE(error);

		//Set splitter to Idle
		error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);

	}

	
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler	= &BaseEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &BaseEngine::renderEmptyBufferDone;
	
    //Implementation specific
    renderCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
		
	error = OMX_GetHandle(&render, OMX_EGL_RENDER, this , &renderCallbacks);
    OMX_TRACE(error);

	error = DisableAllPortsForComponent(&render);
    OMX_TRACE(error);

	//Set renderer to Idle
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);

	
	if(omxCameraSettings.doRecording)
	{
		//Create encoder
		
		OMX_CALLBACKTYPE encoderCallbacks;
		encoderCallbacks.EventHandler		= &BaseEngine::encoderEventHandlerCallback;
		encoderCallbacks.EmptyBufferDone	= &BaseEngine::encoderEmptyBufferDone;
		encoderCallbacks.FillBufferDone		= &TextureEngine::encoderFillBufferDone;
		
		error =OMX_GetHandle(&encoder, OMX_VIDEO_ENCODER, this , &encoderCallbacks);
        OMX_TRACE(error);
		
		configureEncoder();
		
        //Create camera->splitter Tunnel
        error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT,
                                splitter, VIDEO_SPLITTER_INPUT_PORT);
        OMX_TRACE(error);
        
		// Tunnel splitter2 output port and encoder input port
		error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT2,
                                encoder, VIDEO_ENCODE_INPUT_PORT);
        OMX_TRACE(error);

		
		//Create splitter->egl_render Tunnel
		error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT1,
                                render, EGL_RENDER_INPUT_PORT);
        OMX_TRACE(error);

	}else 
	{
		//Create camera->egl_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT,
                                render, EGL_RENDER_INPUT_PORT);
        OMX_TRACE(error);

	}
	
	//Enable camera output port
	error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
    OMX_TRACE(error);

	
	if(omxCameraSettings.doRecording)
	{
		//Enable splitter input port
		error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_INPUT_PORT, NULL);
        OMX_TRACE(error);

		//Enable splitter output port
		error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_OUTPUT_PORT1, NULL);
        OMX_TRACE(error);

		//Enable splitter output2 port
		error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_OUTPUT_PORT2, NULL);
        OMX_TRACE(error);

	}
	
	
	//Enable render output port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_OUTPUT_PORT, NULL);
    OMX_TRACE(error);
	
	//Enable render input port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_INPUT_PORT, NULL);
    OMX_TRACE(error);

	if(omxCameraSettings.doRecording)
	{
		//Enable encoder input port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_INPUT_PORT, NULL);
        OMX_TRACE(error);
	
		//Set encoder to Idle
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);
		
		//Enable encoder output port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, VIDEO_ENCODE_OUTPUT_PORT, NULL);
        OMX_TRACE(error);

		// Configure encoder output buffer
		OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
		OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
		encoderOutputPortDefinition.nPortIndex = VIDEO_ENCODE_OUTPUT_PORT;
		error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
        OMX_TRACE(error);
        
		error =  OMX_AllocateBuffer(encoder, 
                                    &encoderOutputBuffer, 
                                    VIDEO_ENCODE_OUTPUT_PORT, 
                                    NULL, 
                                    encoderOutputPortDefinition.nBufferSize);
        
        OMX_TRACE(error);
        if(error != OMX_ErrorNone)
        {
            ofLogError(__func__) << "UNABLE TO RECORD - MAY REQUIRE MORE GPU MEMORY";
        }
        

	}
	
	//Set renderer to use texture
	error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
    OMX_TRACE(error);
	
	//Start renderer
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    OMX_TRACE(error);
	
	if(omxCameraSettings.doRecording)
	{
		//Start encoder
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		OMX_TRACE(error);
		
		//Start splitter
		error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateExecuting, NULL);
		OMX_TRACE(error);
	}
	
	
	
	//Start camera
	error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    OMX_TRACE(error);

	
	//start the buffer filling loop
	//once completed the callback will trigger and refill
	error = OMX_FillThisBuffer(render, eglBuffer);
    OMX_TRACE(error);

	
	if(omxCameraSettings.doRecording)
	{
		error = OMX_FillThisBuffer(encoder, encoderOutputBuffer);
        OMX_TRACE(error);
		bool doThreadBlocking	= true;
		startThread(doThreadBlocking);
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

TextureEngine::~TextureEngine()
{
    ofLogVerbose(__func__) << "START isThreadRunning() " << isThreadRunning();
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    if(omxCameraSettings.doRecording)
    {
        //encoderOutputBuffer->nFlags = OMX_BUFFERFLAG_EOS;
        //OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    }
    
    if(omxCameraSettings.doRecording && !didWriteFile)
    {
        writeFile();
        
    }
    
    if(omxCameraSettings.doRecording)
    {
        error = OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_INPUT_PORT, NULL);
        OMX_TRACE(error);
        error = OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_OUTPUT_PORT, NULL);
        OMX_TRACE(error);
        error = DisableAllPortsForComponent(&encoder);
        OMX_TRACE(error);
    }
    
    error = OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
    OMX_TRACE(error);
    
    error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_INPUT_PORT, NULL);
    OMX_TRACE(error);
    
    error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&camera);
    OMX_TRACE(error);
    
    if(omxCameraSettings.doRecording)
    {
        error = OMX_FreeBuffer(encoder, VIDEO_ENCODE_OUTPUT_PORT, encoderOutputBuffer);
        OMX_TRACE(error);
        error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);
        
    }

    error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error);
    
    error = DisableAllPortsForComponent(&render);
    OMX_TRACE(error);
    
    
    error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT,  NULL, 0);
    OMX_TRACE(error);
    error = OMX_SetupTunnel(render, EGL_RENDER_INPUT_PORT,  NULL, 0);
    OMX_TRACE(error);
    
    ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
    display = appEGLWindow->getEglDisplay();
    context = appEGLWindow->getEglContext();
    if (eglImage)
    {
        if (!eglDestroyImageKHR(display, eglImage))
        {
            ofLogError(__func__) << "eglDestroyImageKHR FAIL <---------------- :(";
        }else
        {
            ofLogVerbose(__func__) << "eglDestroyImageKHR PASS <---------------- :)";

        }
        eglImage = NULL;
    }
    
    OMX_CONFIG_PORTBOOLEANTYPE cameraport;
    OMX_INIT_STRUCTURE(cameraport);
    cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
    cameraport.bEnabled = OMX_FALSE;
    
    error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
    OMX_TRACE(error);

    
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error);
    
    if(omxCameraSettings.doRecording)
    {
        error = OMX_FreeHandle(encoder);
        OMX_TRACE(error);
        
        error = OMX_FreeHandle(splitter);
        OMX_TRACE(error);
    }
    
    error = OMX_FreeHandle(render);
    OMX_TRACE(error);
    
    //error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
    
	if (pixels) 
	{
		delete[] pixels;
		pixels = NULL;
	}
    ofLogVerbose(__func__) << "END";
}
