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
	
	
	error = OMX_GetHandle(&camera, (OMX_STRING)"OMX.broadcom.camera", this , &cameraCallbacks);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL error: 0x%08x", error);
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
            ofLogError(__func__) << "OMX_EventError: " << OMX_Maps::getInstance().omxErrors[(OMX_ERRORTYPE)nData1];
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
        error = OMX_SendCommand(splitter, OMX_CommandPortEnable, VIDEO_SPLITTER_INPUT_PORT, NULL);
        if (error != OMX_ErrorNone) 
        {
            ofLog(OF_LOG_ERROR, "splitter OMX_SendCommand OMX_CommandPortEnable VIDEO_SPLITTER_INPUT_PORT FAIL error: 0x%08x", error);
        }else 
        {
            ofLogVerbose(__func__) << "splitter OMX_SendCommand OMX_CommandPortEnable VIDEO_SPLITTER_INPUT_PORT PASS";
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
	
	
	
	if(omxCameraSettings.doRecording)
	{
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
		startThread(doThreadBlocking);
	}
	isOpen = true;
	return error;
}

void TextureEngine::close()
{
    if(omxCameraSettings.doRecording)
    {
        //encoderOutputBuffer->nFlags = OMX_BUFFERFLAG_EOS;
        //OMX_FillThisBuffer(encoder, encoderOutputBuffer);
    }else 
    {
        //may have to revisit this if creating new instances of the videograbber
        //otherwise OMX components seem smart enough to clean up themselves on destruction
        //ofLogVerbose(__func__) << "END - just exiting";
        //isOpen = false;
        //return;
    }
    
    
    if(omxCameraSettings.doRecording && !didWriteFile)
    {
        writeFile();
        
    }
    ofLogVerbose(__func__) << " START";
    
    OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
    if(omxCameraSettings.doRecording)
    {
        OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_INPUT_PORT, NULL);
        OMX_SendCommand(encoder, OMX_CommandFlush, VIDEO_ENCODE_OUTPUT_PORT, NULL);
    }
    
    if(omxCameraSettings.doRecording)
    {
        OMXCameraUtils::disableAllPortsForComponent(&encoder);
    }
    
    
    
    OMXCameraUtils::disableAllPortsForComponent(&camera);
    
    if(omxCameraSettings.doRecording)
    {
        OMX_FreeBuffer(encoder, VIDEO_ENCODE_OUTPUT_PORT, encoderOutputBuffer);
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
        ofLog(OF_LOG_ERROR, "render OMX_FreeBuffer FAIL error: 0x%08x", error);
    }else
    {
        ofLogVerbose(__func__) << "render OMX_FreeBuffer PASS";
    }
    
    error =  OMX_FreeHandle(render);
    if (error != OMX_ErrorNone) 
    {
        ofLog(OF_LOG_ERROR, "render OMX_FreeHandle FAIL error: 0x%08x", error);
    }else
    {
        ofLogVerbose(__func__) << "render OMX_FreeHandle PASS";
    }
    
    ofLogVerbose(__func__) << " END";
}

TextureEngine::~TextureEngine()
{
    if(isOpen)
    {
        close();
    }
}

#if 0
void TextureEngine::close()
{
    ofLogVerbose(__func__) << "START";
   // OMXCameraUtils::disableAllPortsForComponent(&splitter);
    //OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
    //OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateLoaded, NULL);
    //OMX_FreeHandle(splitter);
    
    //BaseEngine::close();
    
    OMX_ERRORTYPE error = OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT, eglBuffer);

    if (error != OMX_ErrorNone) 
    {
        ofLog(OF_LOG_ERROR, "render OMX_FreeBuffer FAIL error: 0x%08x", error);
    }else
    {
        ofLogVerbose(__func__) << "render OMX_FreeBuffer PASS";
    }
    ofLogVerbose(__func__) << "OMX BREAKDOWN END";
    ofLogVerbose(__func__) << " END";
    isOpen = false;
}
#endif



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

