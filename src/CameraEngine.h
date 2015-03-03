/*
 *  CameraEngine.h
 *
 *  Created by jason van cleave on 2/10/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include "OMXCameraSettings.h"
#include "OMXCameraUtils.h"

enum EngineType {
	TEXTURE_ENGINE,
	NON_TEXTURE_ENGINE
};

class CameraEngine: public ofThread
{
public:
	CameraEngine();
    ~CameraEngine();
    void setup(OMXCameraSettings& omxCameraSettings_);
    
	int getFrameCounter();
    //void closeEngine();
    bool isRecording() {return isCurrentlyRecording;};
    void stopRecording();
    OMX_HANDLETYPE camera;

    
    EGLImageKHR eglImage;
    bool isOpen(){return didOpen;}
    
private:
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
    bool didOpen;
    int renderedFrameCounter; 
    OMX_ERRORTYPE setupDisplay();
	OMXCameraSettings omxCameraSettings;

	OMX_ERRORTYPE configureCameraResolution();
	OMX_ERRORTYPE configureEncoder();
    
	
	OMX_HANDLETYPE splitter;
	OMX_HANDLETYPE encoder;
	OMX_HANDLETYPE render;
    OMX_BUFFERHEADERTYPE* eglBuffer;

	
	bool didWriteFile;
	
	int recordingBitRate;
	float numMBps;
	
	bool stopRequested;
	bool isStopping;
	int isKeyframeValid;
	bool doFillBuffer;
	bool bufferAvailable;
    bool isCurrentlyRecording;
    
	void threadedFunction();
	bool writeFile();
	
	ofBuffer recordingFileBuffer;
	OMX_BUFFERHEADERTYPE* encoderOutputBuffer;
	
	int recordedFrameCounter;
    
    EngineType engineType;
    OMX_STRING engineTypeString;
    int renderInputPort;
    
    
    static  OMX_ERRORTYPE 
    nullEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                        OMX_PTR pAppData, 
                        OMX_BUFFERHEADERTYPE* pBuffer)
                        {return OMX_ErrorNone;};
    
    static  OMX_ERRORTYPE 
    nullFillBufferDone(OMX_HANDLETYPE hComponent, 
                       OMX_PTR pAppData, 
                       OMX_BUFFERHEADERTYPE* pBuffer)
                        {return OMX_ErrorNone;};
    
    static  OMX_ERRORTYPE 
    nullEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                 OMX_PTR pAppData, 
                                 OMX_EVENTTYPE eEvent, 
                                 OMX_U32 nData1, 
                                 OMX_U32 nData2, 
                                 OMX_PTR pEventData)
                                {return OMX_ErrorNone;};
/*
    static OMX_ERRORTYPE 
    egl_renderFillBufferDone(OMX_HANDLETYPE hComponent, 
                             OMX_PTR pAppData, 
                             OMX_BUFFERHEADERTYPE* pBuffer);
    
    
    static OMX_ERRORTYPE 
    cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                               OMX_PTR pAppData,  
                               OMX_EVENTTYPE eEvent, 
                               OMX_U32 nData1, 
                               OMX_U32 nData2, 
                               OMX_PTR pEventData);
    
    static OMX_ERRORTYPE
    encoderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                          OMX_IN OMX_PTR pAppData,
                          OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
 */
    
    static  OMX_ERRORTYPE egl_renderFillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
    {	
        CameraEngine *grabber = static_cast<CameraEngine*>(pAppData);
        grabber->renderedFrameCounter++;
        OMX_ERRORTYPE error = OMX_FillThisBuffer(hComponent, pBuffer);
        OMX_TRACE(error);
        return error;
    }
    
    static  OMX_ERRORTYPE encoderFillBufferDone(OMX_HANDLETYPE hComponent,
                                                      OMX_PTR pAppData,
                                                      OMX_BUFFERHEADERTYPE* pBuffer)
    {	
        CameraEngine *grabber = static_cast<CameraEngine*>(pAppData);
        grabber->lock();
        //ofLogVerbose(__func__) << "recordedFrameCounter: " << grabber->recordedFrameCounter;
        grabber->bufferAvailable = true;
        grabber->recordedFrameCounter++;
        grabber->unlock();
        return OMX_ErrorNone;
    }
    
    static  OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent,
                                                           OMX_PTR pAppData,
                                                           OMX_EVENTTYPE eEvent,
                                                           OMX_U32 nData1,
                                                           OMX_U32 nData2,
                                                           OMX_PTR pEventData)
    {
        /*ofLog(OF_LOG_VERBOSE, 
         "TextureEngine::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
         __func__, eEvent, nData1, nData2, pEventData);*/
        CameraEngine *grabber = static_cast<CameraEngine*>(pAppData);
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
    
     
    void closeEngine()
    {
        if(isCurrentlyRecording && !didWriteFile)
        {
            writeFile();
            
        }
        
        OMX_ERRORTYPE error;
        error =  OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
        OMX_TRACE(error, "camera: OMX_CommandFlush");
        
        if(encoder)
        {
            error =  OMX_SendCommand(encoder, OMX_CommandFlush, ENCODER_INPUT_PORT, NULL);
            OMX_TRACE(error, "encoder: OMX_CommandFlush ENCODER_INPUT_PORT");
            error =  OMX_SendCommand(encoder, OMX_CommandFlush, ENCODER_OUTPUT_PORT, NULL);
            OMX_TRACE(error, "encoder: OMX_CommandFlush ENCODER_OUTPUT_PORT");
        }
        
        //DisableAllPortsForComponent
        error = DisableAllPortsForComponent(&camera, "camera");
        OMX_TRACE(error, "DisableAllPortsForComponent: camera");
        
        if(splitter)
        {
            error = DisableAllPortsForComponent(&splitter, "splitter");
            OMX_TRACE(error, "DisableAllPortsForComponent splitter");
        }
        
        if(encoder)
        {
            error = DisableAllPortsForComponent(&encoder, "encoder");
            OMX_TRACE(error, "DisableAllPortsForComponent encoder");
        }
        
        switch(engineType)
        {
            case TEXTURE_ENGINE:
            {
                error = OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT, eglBuffer);
                OMX_TRACE(error, "OMX_FreeBuffer(render, EGL_RENDER_OUTPUT_PORT");
                error = DisableAllPortsForComponent(&render, "egl_render");
                OMX_TRACE(error, "DisableAllPortsForComponent: render");
                break;
            }
            case NON_TEXTURE_ENGINE:
            {
                error = DisableAllPortsForComponent(&render, "video_render");
                OMX_TRACE(error, "DisableAllPortsForComponent: render");
                break;
            }
        }
        
        
        
        
        //OMX_FreeBuffer
        if(encoder)
        {
            error = OMX_FreeBuffer(encoder, ENCODER_OUTPUT_PORT, encoderOutputBuffer);
            OMX_TRACE(error, "OMX_FreeBuffer(encoder, ENCODER_OUTPUT_PORT");
        }
        
        
        //OMX_StateIdle
        error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error, "camera->OMX_StateIdle");
        
        if(splitter)
        {
            error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateIdle, NULL);
            OMX_TRACE(error, "splitter->OMX_StateIdle");
        }
        
        
        if(encoder)
        {
            error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateIdle, NULL);
            OMX_TRACE(error, "encoder->OMX_StateIdle");
            
            
            OMX_STATETYPE encoderState;
            error = OMX_GetState(encoder, &encoderState);
            OMX_TRACE(error, "encoderState: "+ getStateString(encoderState));
        }
        
        error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
        OMX_TRACE(error, "render->OMX_StateIdle");
        
        
        
        //OMX_StateLoaded
        error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateLoaded, NULL);
        OMX_TRACE(error, "camera->OMX_StateLoaded");
        
        if(splitter)
        {
            error = OMX_SendCommand(splitter, OMX_CommandStateSet, OMX_StateLoaded, NULL);
            OMX_TRACE(error, "splitter->OMX_StateLoaded");
        }
        
        if(encoder)
        {
            error = OMX_SendCommand(encoder, OMX_CommandStateSet, OMX_StateLoaded, NULL);
            OMX_TRACE(error, "encoder->OMX_StateLoaded");
        }
        
        error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateLoaded, NULL);
        OMX_TRACE(error, "render->OMX_StateLoaded");
        
        
        //OMX_FreeHandle
        error = OMX_FreeHandle(camera);
        OMX_TRACE(error, "OMX_FreeHandle(camera)");
        
        if(splitter)
        {
            error = OMX_FreeHandle(splitter);
            OMX_TRACE(error, "OMX_FreeHandle(splitter)");
        }
        
        if(encoder)
        {
            error = OMX_FreeHandle(encoder);
            OMX_TRACE(error, "OMX_FreeHandle(encoder)"); 
        }  
        
        error =  OMX_FreeHandle(render);
        OMX_TRACE(error, "OMX_FreeHandle(render)");
        didOpen = false;
        
    }

	
};