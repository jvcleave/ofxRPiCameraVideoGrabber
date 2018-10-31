/*
 *  StillCameraEngine.h
 *
 *  Created by jason van cleave on 2/10/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "OMX_Maps.h"
#include "OMXCameraSettings.h"
#include "DirectDisplay.h"

class StillCameraEngineListener
{
public:
    virtual void onTakePhotoComplete(string filePath)=0;
    
};
class StillCameraEngine
{
public:
	StillCameraEngine();
    ~StillCameraEngine();
    void setup(OMXCameraSettings&);
    
    void close();
    OMX_HANDLETYPE camera;

    
    bool isOpen(){return didOpen;}
    
    bool takePhoto();
    
    OMXCameraSettings settings;
    DirectDisplay directDisplay;
    
    StillCameraEngineListener* listener;
    
private:
    bool hasCreatedRenderTunnel;
    OMX_U32 encoderBufferSize;
    OMX_ERRORTYPE buildNonCapturePipeline();
    OMX_ERRORTYPE destroyEncoder();
    
    bool writeFile();
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
    bool didOpen;
    OMX_ERRORTYPE configureEncoder();
    ofBuffer recordingFileBuffer;

    OMX_HANDLETYPE render;
    OMX_HANDLETYPE encoder;    
    OMX_BUFFERHEADERTYPE* encoderOutputBuffer;

    
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
    encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, 
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



