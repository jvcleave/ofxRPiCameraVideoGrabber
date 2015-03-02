/*
 *  BaseEngine.h
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

class BaseEngine: public ofThread
{
public:
	BaseEngine();
    ~BaseEngine();
    void setup(OMXCameraSettings& omxCameraSettings_);
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	int getFrameCounter();
    void closeEngine();
    bool isRecording() {return isCurrentlyRecording;};
    void stopRecording();
    OMX_HANDLETYPE camera;
    EngineType engineType;
    OMX_STRING engineTypeString;
    bool isOpen;
    int renderInputPort;
    OMX_ERRORTYPE setupDisplay();
    EGLImageKHR eglImage;

    int renderedFrameCounter; 
protected:
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
    
	static OMX_ERRORTYPE 
    splitterEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                 OMX_PTR pAppData, 
                                 OMX_EVENTTYPE eEvent, 
                                 OMX_U32 nData1, 
                                 OMX_U32 nData2, 
                                 OMX_PTR pEventData)
                                {return OMX_ErrorNone;};

	static OMX_ERRORTYPE 
    encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                OMX_PTR pAppData, 
                                OMX_EVENTTYPE eEvent, 
                                OMX_U32 nData1, 
                                OMX_U32 nData2, 
                                OMX_PTR pEventData)
                                {return OMX_ErrorNone;};
    
	static OMX_ERRORTYPE 
    encoderEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                           OMX_PTR pAppData, 
                           OMX_BUFFERHEADERTYPE* pBuffer)
                            {return OMX_ErrorNone;};
	
	static OMX_ERRORTYPE 
    renderEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                               OMX_PTR pAppData, 
                               OMX_EVENTTYPE eEvent, 
                               OMX_U32 nData1, 
                               OMX_U32 nData2, 
                               OMX_PTR pEventData)
                                {return OMX_ErrorNone;};
    
	static OMX_ERRORTYPE 
    renderEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                          OMX_PTR pAppData, 
                          OMX_BUFFERHEADERTYPE* pBuffer)
                            {return OMX_ErrorNone;};
    
	static OMX_ERRORTYPE 
    renderFillBufferDone(OMX_HANDLETYPE hComponent, 
                         OMX_PTR pAppData, 
                         OMX_BUFFERHEADERTYPE* pBuffer)
                        {return OMX_ErrorNone;};
    
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

	
};