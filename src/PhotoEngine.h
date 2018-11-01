/*
 *  PhotoEngine.h
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

class PhotoEngineListener
{
public:
    virtual void onTakePhotoComplete(string filePath)=0;
    virtual void onPhotoEngineStart(OMX_HANDLETYPE)=0;
    virtual void onPhotoEngineClose() = 0;

};
class PhotoEngine
{
public:
	PhotoEngine();
    ~PhotoEngine();
    void setup(OMXCameraSettings&, PhotoEngineListener*);
    bool isOpen(){return didOpen;}
    void takePhoto();
    void close(bool isExiting);
    
    
    OMX_HANDLETYPE camera;
    OMXCameraSettings settings;
    DirectDisplay directDisplay;
    
    PhotoEngineListener* listener;
    OMX_ERRORTYPE onEncoderInputPortEnabled();
private:
    
    bool didOpen;

    OMX_HANDLETYPE render;
    OMX_HANDLETYPE encoder;

    
    void writeFile();
    
    OMX_U32 encoderInputBufferSize;
    OMX_U32 encoderOutputBufferSize;
    OMX_U32 cameraOutputBufferSize;

    ofBuffer recordingFileBuffer;

    
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();


    
    OMX_BUFFERHEADERTYPE* encoderOutputBuffer;
    OMX_BUFFERHEADERTYPE* encoderInputBuffer;
    OMX_BUFFERHEADERTYPE* cameraOutputBuffer;


    
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
    cameraFillBufferDone(OMX_HANDLETYPE hComponent,
                          OMX_PTR pAppData,
                          OMX_BUFFERHEADERTYPE* pBuffer);
    
    static OMX_ERRORTYPE 
    cameraEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                        OMX_PTR pAppData, 
                        OMX_BUFFERHEADERTYPE* pBuffer);
    
    
    static OMX_ERRORTYPE 
    encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                               OMX_PTR pAppData,  
                               OMX_EVENTTYPE eEvent, 
                               OMX_U32 nData1, 
                               OMX_U32 nData2, 
                               OMX_PTR pEventData);
    static OMX_ERRORTYPE
    encoderFillBufferDone(OMX_HANDLETYPE hComponent,
                          OMX_PTR pAppData,
                          OMX_BUFFERHEADERTYPE* pBuffer);
    
    static OMX_ERRORTYPE
    encoderEmptyBufferDone(OMX_HANDLETYPE hComponent,
                         OMX_PTR pAppData,
                         OMX_BUFFERHEADERTYPE* pBuffer);
};



