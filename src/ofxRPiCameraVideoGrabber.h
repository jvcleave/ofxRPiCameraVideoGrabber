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
    OMX_ERRORTYPE setAutoAperture(bool b) { return getCameraSettings().setAutoAperture(b); }
    OMX_ERRORTYPE setMeteringMode(CameraMeteringMode m ) { return getCameraSettings().setMeteringMode(m); }
    OMX_ERRORTYPE setMeteringType(OMX_METERINGTYPE m ) { return getCameraSettings().setMeteringType(m); }
    OMX_ERRORTYPE setMeteringType(string s) { return getCameraSettings().setMeteringType(s); }
    string getMeteringType() { return getCameraSettings().getMeteringType(); }
    
    OMX_ERRORTYPE setAutoShutter(bool b){ return getCameraSettings().setAutoShutter(b); }
    bool getAutoShutter() { return getCameraSettings().getAutoShutter(); }
    int getShutterSpeed(){ return getCameraSettings().getShutterSpeed(); }
    
    OMX_ERRORTYPE setShutterSpeed(int shutterSpeedMicroSeconds){ return getCameraSettings().setShutterSpeed(shutterSpeedMicroSeconds); }
    
    OMX_ERRORTYPE setEvCompensation(int n){ return getCameraSettings().setEvCompensation(n); }
    int getEvCompensation(){ return getCameraSettings().getEvCompensation(); }
    
    
    OMX_ERRORTYPE setImageFilter(OMX_IMAGEFILTERTYPE t) { return getCameraSettings().setImageFilter(t); }
    OMX_ERRORTYPE setImageFilter(string t){ return getCameraSettings().setImageFilter(t); }
    string getImageFilter(){ return getCameraSettings().getImageFilter(); }
    
    int getSharpness(){ return getCameraSettings().getSharpness(); } 
    OMX_ERRORTYPE setSharpness(int n) { return getCameraSettings().setSharpness(n); } 
    OMX_ERRORTYPE setSharpnessNormalized(float f) { return getCameraSettings().setSharpnessNormalized(f); } 
    
    int getContrast(){ return getCameraSettings().getContrast(); } 
    OMX_ERRORTYPE setContrast(int n) { return getCameraSettings().setContrast(n); } 
    OMX_ERRORTYPE setContrastNormalized(float f){ return getCameraSettings().setContrastNormalized(f); } 
    
    int getBrightness(){ return getCameraSettings().getBrightness(); } 
    OMX_ERRORTYPE setBrightness(int n){ return getCameraSettings().setBrightness(n); } 
    OMX_ERRORTYPE setBrightnessNormalized(float f) { return getCameraSettings().setBrightnessNormalized(f); }
    
    int getSaturation() { return getCameraSettings().getSaturation(); }
    OMX_ERRORTYPE setSaturation(int n){ return getCameraSettings().setSaturation(n); }
    OMX_ERRORTYPE setSaturationNormalized(float f){ return getCameraSettings().setSaturationNormalized(f); }
    
    bool getFrameStabilization(){ return getCameraSettings().framestabilization; }
    OMX_ERRORTYPE setFrameStabilization(bool b){ return getCameraSettings().setFrameStabilization(b); }
    
    string getExposurePreset(){ return getCameraSettings().getExposurePreset(); }
    OMX_ERRORTYPE setExposurePreset(OMX_EXPOSURECONTROLTYPE t){ return getCameraSettings().setExposurePreset(t); }
    OMX_ERRORTYPE setExposurePreset(string s){ return getCameraSettings().setExposurePreset(s); }
    
    string getWhiteBalance(){ return getCameraSettings().getWhiteBalance(); }
    OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE t){ return getCameraSettings().setWhiteBalance(t); }
    OMX_ERRORTYPE setWhiteBalance(string s){ return getCameraSettings().setWhiteBalance(s); }
    
    //int getDRE();
    OMX_ERRORTYPE setDRE(int n){ return getCameraSettings().setDRE(n); }
    
    OMX_ERRORTYPE setSensorCrop(ofRectangle& rectangle){ return getCameraSettings().setSensorCrop(rectangle); }
    OMX_ERRORTYPE setSensorCrop(int left, int top, int width, int height){ return getCameraSettings().setSensorCrop(left, top, width, height); }
    ofRectangle& getCropRectangle(){ return getCameraSettings().getCropRectangle(); }
    //OMX_ERRORTYPE updateSensorCrop(){ return getCameraSettings().updateSensorCrop(); }
     
    OMX_ERRORTYPE zoomIn(){ return getCameraSettings().zoomIn(); }
    OMX_ERRORTYPE zoomOut(){ return getCameraSettings().zoomOut(); }
    OMX_ERRORTYPE resetZoom(){ return getCameraSettings().resetZoom(); }
    
    
    OMX_ERRORTYPE setZoomLevelNormalized(float f){ return getCameraSettings().setZoomLevelNormalized(f); }
    float getZoomLevelNormalized() { return getCameraSettings().getZoomLevelNormalized(); }
    
    OMX_ERRORTYPE setRotation(int n){ return getCameraSettings().setRotation(n); }
    OMX_ERRORTYPE setRotation(CameraSettings::ROTATION r){ return getCameraSettings().setRotation(r); }
    int getRotation(){ return getCameraSettings().getRotation(); }
    
    OMX_ERRORTYPE rotateClockwise(){ return getCameraSettings().rotateClockwise(); }
    OMX_ERRORTYPE rotateCounterClockwise(){ return getCameraSettings().rotateCounterClockwise(); }
    
    
    OMX_ERRORTYPE setMirror(int n){ return getCameraSettings().setMirror(n); }
    OMX_ERRORTYPE setMirror(string s){ return getCameraSettings().setMirror(s); }
    string getMirror(){ return getCameraSettings().getMirror(); }
    
    OMX_ERRORTYPE setSoftwareSharpening(bool b){ return getCameraSettings().setSoftwareSharpening(b); }
    OMX_ERRORTYPE enableSoftwareSharpening(){ return getCameraSettings().enableSoftwareSharpening(); }
    OMX_ERRORTYPE disableSoftwareSharpening(){ return getCameraSettings().disableSoftwareSharpening(); }
    bool isSoftwareSharpeningEnabled() {return getCameraSettings().isSoftwareSharpeningEnabled(); }
    
    
    CameraSettings::EXPOSURE_MODE getExposureMode(){ return getCameraSettings().getExposureMode(); }
    
    OMX_ERRORTYPE enableAutoExposure(){ return getCameraSettings().enableAutoExposure(); }
    OMX_ERRORTYPE enableManualExposure(){ return getCameraSettings().enableManualExposure(); }
    
    OMX_ERRORTYPE setSoftwareSaturation(bool b){ return getCameraSettings().setSoftwareSaturation(b); }
    OMX_ERRORTYPE enableSoftwareSaturation(){ return getCameraSettings().enableSoftwareSaturation(); }
    OMX_ERRORTYPE disableSoftwareSaturation(){ return getCameraSettings().disableSoftwareSaturation(); }
    bool isSoftwareSaturationEnabled() { return getCameraSettings().isSoftwareSaturationEnabled(); }
    
    

    
    
private:
    bool doStartRecording;
    ofFbo* fbo;
    
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