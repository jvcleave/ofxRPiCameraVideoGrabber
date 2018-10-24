/*
 *  ofxRPiCameraPhotoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include "OMX_Maps.h"

#include "StillCameraEngine.h"



class ofxRPiCameraPhotoGrabber
{

public:
    
	ofxRPiCameraPhotoGrabber();
    ~ofxRPiCameraPhotoGrabber();
    
	void setup();
    	
    bool isReady();
   
    
	int getWidth();
	int getHeight();

    
    void takePhoto();

    OMX_HANDLETYPE camera;
    StillCameraEngine engine;
    
#if 0
    void loadCameraSettingsFromFile(string filePath="");
    void saveCameraSettingsToFile(string filePath="");
    int getFrameRate();

    
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
    
#endif
    
    


	
};
