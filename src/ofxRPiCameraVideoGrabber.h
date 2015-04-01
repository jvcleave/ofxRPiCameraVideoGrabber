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
    
    SessionConfig* sessionConfig;
    CameraSettings& getCameraSettings();
	void setup(SessionConfig&);
    void setup(SessionConfig*);
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

    ofFbo& getFbo();
    
    int getTextureID() { return textureID; }
    bool forceEGLReuse;
    
    void enablePixels();
    void disablePixels();
    
    unsigned char * pixels;
    unsigned char * getPixels();

    void saveImage();
    void saveRawImage();
        
    void loadCameraSettingsFromFile(string filePath="");
    void saveCameraSettingsToFile(string filePath="");

    
    //interface
    bool  setAutoAperture(bool);
    bool  setMeteringMode(CameraMeteringMode);
    bool  setMeteringType(OMX_METERINGTYPE);
    bool  setMeteringType(string);
    string getMeteringType();
    
    bool  setAutoShutter(bool);
    bool getAutoShutter();
    int getShutterSpeed();
    
    bool  setShutterSpeed(int shutterSpeedMicroSeconds);
    bool  setEvCompensation(int n);
    int getEvCompensation();
    
    
    bool  setImageFilter(OMX_IMAGEFILTERTYPE);
    bool  setImageFilter(string);
    string getImageFilter();
    
    int getSharpness();
    bool  setSharpness(int) ;
    bool  setSharpnessNormalized(float) ;
    
    int getContrast();
    bool  setContrast(int n) ;
    bool  setContrastNormalized(float);
    
    int getBrightness();
    bool  setBrightness(int n);
    bool  setBrightnessNormalized(float) ;
    
    int getSaturation() ;
    bool  setSaturation(int n);
    bool  setSaturationNormalized(float);
    
    bool getFrameStabilization();
    bool  setFrameStabilization(bool);
    
    string getExposurePreset();
    bool  setExposurePreset(OMX_EXPOSURECONTROLTYPE);
    bool  setExposurePreset(string);
    
    string getWhiteBalance();
    bool  setWhiteBalance(OMX_WHITEBALCONTROLTYPE);
    bool  setWhiteBalance(string);
    
    //int getDRE();
    bool  setDRE(int n);
    
    bool  setSensorCrop(ofRectangle& rectangle);
    bool  setSensorCrop(int left, int top, int width, int height);
    ofRectangle& getCropRectangle();
    bool  updateSensorCrop();
    
    bool  zoomIn();
    bool  zoomOut();
    bool  resetZoom();
    
    bool  setZoomLevelNormalized(float);
    float getZoomLevelNormalized();
    
    bool  setRotation(int n);
    bool  setRotation(CameraSettings::ROTATION r);
    int getRotation();
    
    bool  rotateClockwise();
    bool  rotateCounterClockwise();
    
    bool  setMirror(int n);
    bool  setMirror(string);
    string getMirror();
    
    bool  setSoftwareSharpening(bool);
    bool  enableSoftwareSharpening();
    bool  disableSoftwareSharpening();
    bool isSoftwareSharpeningEnabled() ;
    
    CameraSettings::EXPOSURE_MODE getExposureMode();
    
    bool  enableAutoExposure();
    bool  enableManualExposure();
    
    bool  setSoftwareSaturation(bool);
    bool  enableSoftwareSaturation();
    bool  disableSoftwareSaturation();
    bool isSoftwareSaturationEnabled();    
    

    
    
private:
    bool doStartRecording;
    ofFbo fbo;
    
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
    
 
    static bool doExit;
    static void signal_handler(int signum);
	
};