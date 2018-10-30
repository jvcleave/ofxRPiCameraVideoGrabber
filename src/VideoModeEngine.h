/*
 *  BaseEngine.h
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
#include "ofAppEGLWindow.h"

class RecordingListener
{
public:
    virtual void onRecordingComplete(string filePath)=0;
};

class VideoModeEngine
{
public:
	VideoModeEngine();
    ~VideoModeEngine();

	void setup(OMXCameraSettings&, RecordingListener*);
    int getFrameCounter();
	OMXCameraSettings& getSettings();
    void stopRecording();
	OMX_HANDLETYPE camera;
	bool isOpen;
    bool isRecording;
    RecordingListener* recordingListener;
    DirectDisplay* directDisplay;

    
    
    int frameCounter;
    void generateEGLImage();
    void enablePixels();
    void disablePixels();
    unsigned char * getPixels();
    void updatePixels();
    unsigned char * pixels;
    bool doPixels;
    ofFbo fbo;
    ofTexture& getTexture()
    {
        return texture;
    }
    
    void setDisplayAlpha(int);
    void setDisplayLayer(int);
    void setDisplayRotation(int);
    void setDisplayDrawRectangle(ofRectangle);
    void setDisplayCropRectangle(ofRectangle);
    void setDisplayMirror(bool);
    
    void close();
    OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
    bool isClosing;
protected:
	
    OMXCameraSettings settings;
	void configureCameraResolution();
	void configureEncoder();
	
	OMX_HANDLETYPE splitter;
	OMX_HANDLETYPE encoder;
	OMX_HANDLETYPE render;
	
	
	bool didWriteFile;
	
	int recordingBitRate;
	float numMBps;
	
	bool stopRequested;
	bool isStopping;
	
	void writeFile();
	
	ofBuffer recordingFileBuffer;
	OMX_BUFFERHEADERTYPE* encoderOutputBuffer;
	
	int recordedFrameCounter;
	
	static OMX_ERRORTYPE splitterEventHandlerCallback(OMX_HANDLETYPE, OMX_PTR, OMX_EVENTTYPE, OMX_U32, OMX_U32, OMX_PTR) {return OMX_ErrorNone;};
	static OMX_ERRORTYPE encoderEventHandlerCallback(OMX_HANDLETYPE, OMX_PTR, OMX_EVENTTYPE, OMX_U32, OMX_U32, OMX_PTR){return OMX_ErrorNone;};
	static OMX_ERRORTYPE encoderEmptyBufferDone( OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*){return OMX_ErrorNone;};   
    static OMX_ERRORTYPE renderEventHandlerCallback(OMX_HANDLETYPE, OMX_PTR, OMX_EVENTTYPE, OMX_U32, OMX_U32, OMX_PTR){return OMX_ErrorNone;};
	static OMX_ERRORTYPE renderEmptyBufferDone(OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*){return OMX_ErrorNone;};
    static OMX_ERRORTYPE directRenderFillBufferDone( OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*){return OMX_ErrorNone;};
    
    static OMX_ERRORTYPE textureRenderFillBufferDone( OMX_HANDLETYPE, OMX_PTR, OMX_BUFFERHEADERTYPE*);

    
    static OMX_ERRORTYPE encoderFillBufferDone(OMX_HANDLETYPE encoder, OMX_PTR engine, OMX_BUFFERHEADERTYPE* encoderOutputBuffer);
    static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE camera, OMX_PTR videoModeEngine_, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);


    EGLImageKHR eglImage;
    EGLDisplay display;
    EGLContext context;
    ofTexture texture;
    
    OMX_BUFFERHEADERTYPE* eglBuffer;
    
    OMX_ERRORTYPE buildTexturePipeline();
    OMX_ERRORTYPE buildDirectPipeline();

    void destroyTexturePipeline();
    void destroyDirectPipeline();
	
};
