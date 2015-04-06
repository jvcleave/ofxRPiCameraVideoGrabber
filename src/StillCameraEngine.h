/*
 *  StillCameraEngine.h
 *
 *  Created by jason van cleave on 2/10/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"


#include "OMXCameraUtils.h"
#include "SessionConfig.h"


class StillCameraEngine: public ofThread
{
public:
	StillCameraEngine();
    ~StillCameraEngine();
    void setup(SessionConfig* sessionConfig_);
    
    void closeEngine();
    OMX_HANDLETYPE camera;

    
    bool isOpen(){return didOpen;}
    
private:
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
    bool didOpen;
	SessionConfig* sessionConfig;

	OMX_ERRORTYPE configureCameraResolution();
    OMX_ERRORTYPE configureEncoder();
	
	bool didWriteFile;
	
	void threadedFunction();
	
    OMX_HANDLETYPE render;
    OMX_HANDLETYPE encoder;    
    OMX_BUFFERHEADERTYPE* encoderOutputBuffer;

    OMX_PARAM_PORTDEFINITIONTYPE encoderOutputPortDefinition;
    
    static OMX_ERRORTYPE 
    nullEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                        OMX_PTR pAppData, 
                        OMX_BUFFERHEADERTYPE* pBuffer)
                        {return OMX_ErrorNone;};
    
    static OMX_ERRORTYPE 
    nullFillBufferDone(OMX_HANDLETYPE hComponent, 
                       OMX_PTR pAppData, 
                       OMX_BUFFERHEADERTYPE* pBuffer)
                        {return OMX_ErrorNone;};
    
    static OMX_ERRORTYPE 
    nullEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                 OMX_PTR pAppData, 
                                 OMX_EVENTTYPE eEvent, 
                                 OMX_U32 nData1, 
                                 OMX_U32 nData2, 
                                 OMX_PTR pEventData)
                                {return OMX_ErrorNone;};
    
    
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



