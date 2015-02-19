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
	engineType = TEXTURE_ENGINE;

}

int TextureEngine::getFrameCounter()
{
	return renderedFrameCounter;
	
}

void TextureEngine::setup(OMXCameraSettings& omxCameraSettings_)
{
	omxCameraSettings = omxCameraSettings_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &TextureEngine::cameraEventHandlerCallback;
	
	
	error = OMX_GetHandle(&camera, OMX_CAMERA, this , &cameraCallbacks);
	if(error != OMX_ErrorNone) 
	{
		OMX_TRACE(error, "camera OMX_GetHandle FAIL");
	}
	
	configureCameraResolution();
	
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
            
        case OMX_EventError:
        {
            OMX_TRACE((OMX_ERRORTYPE)nData1);
        }
		default: 
		{
            /*ofLog(OF_LOG_VERBOSE, 
                  "TextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
                  __func__, eEvent, nData1, nData2, pEventData);*/
            
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
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	OMX_TRACE(error, "camera->OMX_StateIdle");
    
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
		OMX_GetHandle(&splitter, OMX_VIDEO_SPLITTER, this , &splitterCallbacks);
		DisableAllPortsForComponent(&splitter);
		
		//Set splitter to Idle
		error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
		OMX_TRACE(error);
        
        error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_INPUT_PORT, NULL);
        OMX_TRACE(error);
    
	}

	
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler	= &BaseEngine::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &BaseEngine::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &TextureEngine::renderFillBufferDone;
	
	
	OMX_GetHandle(&render, OMX_EGL_RENDER, this , &renderCallbacks);
	DisableAllPortsForComponent(&render);
	
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
		
	}
	
	
	
	if(omxCameraSettings.doRecording)
	{
        //Create camera->splitter Tunnel
        error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, splitter, VIDEO_SPLITTER_INPUT_PORT);
        OMX_TRACE(error);

        
		// Tunnel splitter2 output port and encoder input port
		error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT2, encoder, ENCODER_INPUT_PORT);
        OMX_TRACE(error);

		
		//Create splitter->egl_render Tunnel
		error = OMX_SetupTunnel(splitter, VIDEO_SPLITTER_OUTPUT_PORT1, render, EGL_RENDER_INPUT_PORT);
        OMX_TRACE(error);

	}else 
	{
		//Create camera->egl_render Tunnel
		error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
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

	
    /*
     Boolean parameter to enable/disable EGL discard mode. With discard mode enabled (default), EGL render will only buffer up to one image. If a new image is received while an image is waiting to be processed, the old image will be dropped. With discard mode disabled, 32 VC images (used in tunnelled mode) can be buffered. Once the buffer is full, the upstream component is notified and should attempt to send the image again later. Non-discard mode only applies to the tunnelled case (it does not apply when called internally).
     */
    /*   
    bool disableDiscardMode = true;
    if(disableDiscardMode)
    {
        OMX_CONFIG_PORTBOOLEANTYPE discardMode;
        OMX_INIT_STRUCTURE(discardMode);
        discardMode.nPortIndex = EGL_RENDER_INPUT_PORT;
        discardMode.bEnabled = OMX_FALSE; //default true
        error = OMX_SetParameter(render, OMX_IndexParamBrcmVideoEGLRenderDiscardMode, &discardMode);
        OMX_TRACE(error);

    }
    */
    
	if(omxCameraSettings.doRecording)
	{
		//Enable encoder input port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, ENCODER_INPUT_PORT, NULL);
        OMX_TRACE(error);

	
		//Set encoder to Idle
		error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error);

		
		//Enable encoder output port
		error = OMX_SendCommand(encoder, OMX_CommandPortEnable, ENCODER_OUTPUT_PORT, NULL);
        OMX_TRACE(error);

		
		// Configure encoder output buffer
		OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
		OMX_INIT_STRUCTURE(encoderOutputPortDefinition);
		encoderOutputPortDefinition.nPortIndex = ENCODER_OUTPUT_PORT;
		error =OMX_GetParameter(encoder, OMX_IndexParamPortDefinition, &encoderOutputPortDefinition);
		if (error != OMX_ErrorNone) 
		{
			OMX_TRACE(error, "encoder OMX_GetParameter OMX_IndexParamPortDefinition FAIL");
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

		error =  OMX_AllocateBuffer(encoder, &encoderOutputBuffer, ENCODER_OUTPUT_PORT, NULL, encoderOutputPortDefinition.nBufferSize);
        OMX_TRACE(error);

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

void TextureEngine::close()
{
 
    if(omxCameraSettings.doRecording && !didWriteFile)
    {
        writeFile();
        
    }
    
    OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
    if(omxCameraSettings.doRecording)
    {
        OMX_SendCommand(encoder, OMX_CommandFlush, ENCODER_INPUT_PORT, NULL);
        OMX_SendCommand(encoder, OMX_CommandFlush, ENCODER_OUTPUT_PORT, NULL);
    }
    
    if(omxCameraSettings.doRecording)
    {
        DisableAllPortsForComponent(&encoder);
    }
    
    DisableAllPortsForComponent(&camera);
    
    if(omxCameraSettings.doRecording)
    {
        OMX_FreeBuffer(encoder, ENCODER_OUTPUT_PORT, encoderOutputBuffer);
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
    
    OMX_ERRORTYPE error = OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT, eglBuffer);
    
    if (error != OMX_ErrorNone) 
    {
        OMX_TRACE(error, "render OMX_FreeBuffer FAIL");
    }
    
    error =  OMX_FreeHandle(render);
    if (error != OMX_ErrorNone) 
    {
        OMX_TRACE(error, "render OMX_FreeHandle FAIL");
    }
    
    //DisableAllPortsForComponent
    error = DisableAllPortsForComponent(&camera, "camera");
    OMX_TRACE(error, "DisableAllPortsForComponent: camera");
    
    error = DisableAllPortsForComponent(&splitter, "splitter");
    OMX_TRACE(error, "DisableAllPortsForComponent splitter");
    
    error = DisableAllPortsForComponent(&encoder, "encoder");
    OMX_TRACE(error, "DisableAllPortsForComponent encoder");
    
    error = DisableAllPortsForComponent(&render, "egl_render");
    OMX_TRACE(error, "DisableAllPortsForComponent: render");
    
    //OMX_FreeBuffer
    error = OMX_FreeBuffer(encoder, ENCODER_OUTPUT_PORT, encoderOutputBuffer);
    OMX_TRACE(error, "OMX_FreeBuffer(encoder, ENCODER_OUTPUT_PORT");
    
    error = OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT, eglBuffer);
    OMX_TRACE(error, "OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT");
    
    //OMX_StateIdle
    error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error, "camera->OMX_StateIdle");
    
    error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error, "splitter->OMX_StateIdle");
    
    error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error, "encoder->OMX_StateIdle");
              
    ofSleepMillis(1000);
              
    OMX_STATETYPE encoderState;
    error = OMX_GetState(encoder, &encoderState);
    OMX_TRACE(error, "encoderState: "+ getStateString(encoderState));
    
    error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
    OMX_TRACE(error, "render->OMX_StateIdle");
    
    //OMX_StateLoaded
    error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    OMX_TRACE(error, "camera->OMX_StateLoaded");
    
    error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    OMX_TRACE(error, "splitter->OMX_StateLoaded");
    
    error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    OMX_TRACE(error, "encoder->OMX_StateLoaded");
    
    error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    OMX_TRACE(error, "render->OMX_StateLoaded");
    
    
    //OMX_FreeHandle
    error = OMX_FreeHandle(camera);
    OMX_TRACE(error, "OMX_FreeHandle(camera)");
    
    error = OMX_FreeHandle(splitter);
    OMX_TRACE(error, "OMX_FreeHandle(splitter)");
    
    error = OMX_FreeHandle(encoder);
    OMX_TRACE(error, "OMX_FreeHandle(encoder)");
    
    error =  OMX_FreeHandle(render);
    OMX_TRACE(error, "OMX_FreeHandle(render)");
}

TextureEngine::~TextureEngine()
{
    if(isOpen)
    {
        close();
    }
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

