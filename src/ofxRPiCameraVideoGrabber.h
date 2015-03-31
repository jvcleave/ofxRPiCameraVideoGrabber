/*
 *  ofxRPiCameraVideoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"


#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include "OMXCameraUtils.h"
#include "SessionConfig.h"

#include "CameraEngine.h"



class ofxRPiCameraVideoGrabber
{
public:

	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    
    SessionConfig sessionConfig;
    CameraSettings cameraSettings;
	void setup(SessionConfig);
    void close();
    
    void draw();
	
    bool isReady();
    bool isFrameNew();
   
    CameraEngine* getEngine();
    ofTexture& getTextureReference();
    
	int getWidth();
	int getHeight();
	int getFrameRate();
    
    
    
    bool isTextureEnabled();
    
    bool isRecording();
    void startRecording();
    void stopRecording();

    ofFbo fbo;
    int getTextureID() { return fbo.getTextureReference().getTextureData().textureID; }
    bool forceEGLReuse;
    
    void enablePixels();
    void disablePixels();
    
    unsigned char * pixels;
    unsigned char * getPixels();

    void saveImage();
    void saveRawImage();
        
	
        
private:
    bool doStartRecording;
    
    
    bool hasExitHandler;
    bool hasOMXInit;
    
    
    void addExitHandler();
    void onUpdate(ofEventArgs & args);
    void onUpdateDuringExit(ofEventArgs& args);
    void updatePixels();
    
    bool hasNewFrame;
    int updateFrameCounter;
    int frameCounter;

    OMX_HANDLETYPE camera;
    

		
	CameraEngine* engine;
    
    bool doSaveImage;
    bool doRawSave;
    ofAppEGLWindow *appEGLWindow;
    EGLDisplay display;
    EGLContext context;
    EGLImageKHR eglImage;
    EGLSurface surface;
    EGLConfig eglConfig;
    bool doPixels;
    
    GLuint textureID;
    
    
    void generateEGLImage(int, int);
    void destroyEGLImage();
    
 
    
    
	
};