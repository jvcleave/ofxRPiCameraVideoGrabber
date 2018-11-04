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
#include "ofxOMXCameraSettings.h"
#include "DirectDisplay.h"
#include "EGLImageController.h"
class PhotoEngineListener
{
public:
    virtual void onTakePhotoComplete(string filePath)=0;
    virtual void onPhotoEngineStart(OMX_HANDLETYPE)=0;

};
class PhotoEngine
{
public:
	PhotoEngine();
    ~PhotoEngine();
    void setup(ofxOMXCameraSettings*, PhotoEngineListener*, EGLImageController* eglImageController_ = NULL);
    bool isOpen(){return didOpen;}
    void takePhoto();
    void close();
    
    
    OMX_HANDLETYPE camera;
    ofxOMXCameraSettings* settings;
    DirectDisplay directDisplay;
    OMX_IMAGE_CODINGTYPE codingType;    
    PhotoEngineListener* listener;
    OMX_U32 stride;
    bool didOpen;

    OMX_HANDLETYPE render;
    OMX_HANDLETYPE encoder;
    OMX_HANDLETYPE nullSink;

    void writeFile();
    string saveFolderAbsolutePath;
    OMX_U32 encoderOutputBufferSize;

    ofBuffer recordingFileBuffer;

    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
    
    OMX_BUFFERHEADERTYPE* encoderOutputBuffer;
    
    EGLImageController* eglImageController;
    
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
                          OMX_BUFFERHEADERTYPE* pBuffer){return OMX_ErrorNone;};
    
    static OMX_ERRORTYPE 
    cameraEmptyBufferDone(OMX_HANDLETYPE hComponent, 
                        OMX_PTR pAppData, 
                        OMX_BUFFERHEADERTYPE* pBuffer){return OMX_ErrorNone;};
    
    
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
                         OMX_BUFFERHEADERTYPE* pBuffer){return OMX_ErrorNone;};
    
    
    OMX_PARAM_PORTDEFINITIONTYPE previewPortConfig;
    
    static OMX_ERRORTYPE textureRenderFillBufferDone( OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*);
    int renderInputPort;
    OMX_BUFFERHEADERTYPE* eglBuffer;

};



