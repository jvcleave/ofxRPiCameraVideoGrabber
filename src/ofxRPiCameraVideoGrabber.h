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
#include "OMXCameraSettings.h"

#include "TextureEngine.h"
#include "NonTextureEngine.h"

struct CameraMeteringMode
{
    OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
    
    OMX_METERINGTYPE meteringType;
    float evCompensation;
    
    
    bool autoShutter;
    int shutterSpeedMicroSeconds;
    
    bool autoAperture;
    int aperture;
    
    bool autoISO;
    int ISO;
    
    CameraMeteringMode()
    {
        OMX_INIT_STRUCTURE(exposurevalue);
        exposurevalue.nPortIndex = OMX_ALL;
        meteringType = OMX_MeteringModeAverage;
        
        
        evCompensation=0; //-4 to +4
        
        autoShutter = true;
        shutterSpeedMicroSeconds = 0;
        
        autoAperture = true;
        aperture = 0;
        
        autoISO = true;
        ISO=0;
    };
    
    
    
    string toString()
    {
        
        stringstream ss;
        ss << OMX_Maps::getInstance().getMetering(meteringType) << "\n";
        ss << "evCompensation: " << evCompensation << "\n";
        
        ss << "autoShutter: " << autoShutter << "\n";
        ss << "shutterSpeedMicroSeconds: " << shutterSpeedMicroSeconds << "\n";
        
        ss << "autoAperture: " << autoAperture << "\n";
        ss << "aperture: " << aperture << "\n";
        
        ss << "autoISO: " << autoISO << "\n";
        ss << "ISO: " << ISO << "\n";
        return ss.str();
        
    }
    
};


enum EXPOSURE_MODE
{
    EXPOSURE_MODE_AUTO,
    EXPOSURE_MODE_MANUAL,
    EXPOSURE_MODE_INVALID
};

enum MIRROR
{
    MIRROR_NONE=OMX_MirrorNone,
    MIRROR_VERTICAL=OMX_MirrorVertical,
    MIRROR_HORIZONTAL=OMX_MirrorHorizontal,
    MIRROR_BOTH=OMX_MirrorBoth,
};

enum ROTATION
{
    ROTATION_0=0,
    ROTATION_90=90,
    ROTATION_180=180,
    ROTATION_270=270,
};

class ofxRPiCameraVideoGrabber
{

public:
	
	
    
    
	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    void addExitHandler();
	void close();
	void setup(OMXCameraSettings omxCameraSettings);
	void draw();
	
	ofTexture& getTextureReference();

	void setSharpness(int sharpness_);
	void setContrast(int contrast_);
	void setBrightness(int brightness_);
	void setSaturation(int saturation_);
	
	void setFrameStabilization(bool doStabilization);
    /*
	void setMeteringMode(OMX_METERINGTYPE meteringType, 
                         int evCompensation=0, 
                         int sensitivity = 100, 
                         bool autoSensitivity = false);*/
	
	OMX_ERRORTYPE setColorEnhancement(bool doColorEnhance, int U=128, int V=128);
	void setLEDState(bool status);
    void setDRE(int level);

	bool LED_CURRENT_STATE;
	void toggleLED();
	
	GLuint getTextureID();
	
	int getWidth();
	int getHeight();
	int getFrameRate();
	bool isReady();

	
	OMX_ERRORTYPE setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE);
	void disableImageEffects();
	void enableImageEffects();
	bool isFrameNew();
    bool isTextureEnabled();	
		
	int getSharpness()		{ return sharpness; }
	int getContrast()		{ return contrast;	}
	int getBrightness()		{ return brightness; }
	int getSaturation()		{ return saturation; }
	int getDRE()            { return dreLevel; }
	OMXCameraSettings omxCameraSettings;
    CameraMeteringMode currentMeteringMode;

	void stopRecording();
	void enablePixels();
	void disablePixels();
	unsigned char * getPixels();
	
    TextureEngine* textureEngine;
    NonTextureEngine* engine;
    
    void  enableAutoExposure();
    void  enableManualExposure();
    EXPOSURE_MODE getExposureMode();
    
    vector<int> zoomLevels;
    OMX_ERRORTYPE setDigitalZoom();
    
    ofRectangle cropRectangle;
    OMX_ERRORTYPE setSensorCrop(ofRectangle&);
    ofRectangle& getCropRectangle() { return cropRectangle; }
    OMX_ERRORTYPE updateSensorCrop();
    OMX_ERRORTYPE setSensorCrop(int left, int top, int width, int height);
    

    int zoomLevel;
    OMX_ERRORTYPE zoomIn();
    OMX_ERRORTYPE zoomOut();
    OMX_ERRORTYPE resetZoom();
    OMX_ERRORTYPE setZoomLevelNormalized(float);
    float getZoomLevelNormalized();
    

    OMX_ERRORTYPE setMirror(int);
    OMX_ERRORTYPE setMirror(string);
    string getMirror();
    OMX_ERRORTYPE applyMirror();
    string mirror; 
    

    int rotation;
    OMX_ERRORTYPE setRotation(int);
    OMX_ERRORTYPE setRotation(ROTATION);
    int getRotation();
    OMX_ERRORTYPE applyRotation();

    OMX_ERRORTYPE rotateClockwise();
    OMX_ERRORTYPE rotateCounterClockwise();
    

    OMX_ERRORTYPE setImageFilter(OMX_IMAGEFILTERTYPE);
    OMX_ERRORTYPE setImageFilter(string);
    string getImageFilter();
    string imageFilter;
    
    void applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter);
    

    OMX_ERRORTYPE setExposurePreset(OMX_EXPOSURECONTROLTYPE);
    OMX_ERRORTYPE setExposurePreset(string);
    string getExposurePreset();
    string exposurePreset;

    OMX_ERRORTYPE setEvCompensation(int); //-4 to 4
    int getEvCompensation();
    int evCompensation;
    
    OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE);
    OMX_ERRORTYPE setWhiteBalance(string);
    string getWhiteBalance();
    string whiteBalance;

    
    OMX_ERRORTYPE setAutoShutter(bool);
    bool getAutoShutter(){return currentMeteringMode.autoShutter;}
    int getShutterSpeed();
    
    OMX_ERRORTYPE setShutterSpeed(int shutterSpeedMicroSeconds);
    
    OMX_ERRORTYPE setMeteringMode(CameraMeteringMode);
    OMX_ERRORTYPE setMeteringType(OMX_METERINGTYPE);
    OMX_ERRORTYPE setMeteringType(string);
    string getMeteringType();
    string meteringType;

    
    
    bool autoShutter;
    int shutterSpeedMicroSeconds;
    bool autoAperture;
    int aperture;
    bool autoISO;
    int ISO;
    bool framestabilization;
    
    bool doDisableSoftwareSharpen;
    bool doDisableSoftwareSaturation;
    OMX_ERRORTYPE setSoftwareSharpening(bool);
    OMX_ERRORTYPE enableSoftwareSharpening();
    OMX_ERRORTYPE disableSoftwareSharpening();
    bool isSoftwareSharpeningEnabled() {return doDisableSoftwareSharpen;}
    
    OMX_ERRORTYPE setSoftwareSaturation(bool);
    OMX_ERRORTYPE enableSoftwareSaturation();
    OMX_ERRORTYPE disableSoftwareSaturation();
    bool isSoftwareSaturationEnabled() {return doDisableSoftwareSaturation;}

    void reset();
 
private:

    void resetValues();
    void applyAllSettings();
	void onUpdate(ofEventArgs & args);
    void onUpdateDuringExit(ofEventArgs& args);

	bool hasNewFrame;
	
	int updateFrameCounter;
	OMX_HANDLETYPE camera;
	
	int sharpness;	//	-100 to 100
	int contrast;	//  -100 to 100 
	int brightness; //     0 to 100
	int saturation; //  -100 to 100 
    int dreLevel; 
	//void close();
	
	void toggleImageEffects(bool doDisable);
	
	OMXCameraUtils omxCameraUtils;
	
    OMX_ERRORTYPE applyCurrentMeteringMode();
    void updateCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue);

	
	int frameCounter;
	
	bool pixelsRequested;
    
    OMX_CONFIG_EXPOSURECONTROLTYPE exposurePresetConfig;
    
    OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
    OMX_CONFIG_FRAMESTABTYPE framestabilizationConfig;
    OMX_CONFIG_CONTRASTTYPE contrastConfig;
    OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
    OMX_CONFIG_SATURATIONTYPE saturationConfig;
    OMX_CONFIG_WHITEBALCONTROLTYPE whiteBalanceConfig;
    OMX_CONFIG_COLORENHANCEMENTTYPE colorEnhancementConfig;
    OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
    
    //OMX_CONFIG_BOOLEANTYPE burstModeConfig;
    OMX_CONFIG_BOOLEANTYPE hdrConfig;
    OMX_CONFIG_SCALEFACTORTYPE digitalZoomConfig;
    
    OMX_PARAM_CAMERADISABLEALGORITHMTYPE cameraDisableAlgorithmConfig;
    OMX_CONFIG_FLICKERCANCELTYPE flickerCancelConfig;
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE dreConfig;
    OMX_CONFIG_INPUTCROPTYPE sensorCropConfig;
    
    
    OMX_CONFIG_CAMERAINFOTYPE cameraInfoConfig;
    OMX_PARAM_BRCMFRAMERATERANGETYPE frameRateRangeConfig;
    
    
    OMX_CONFIG_ROTATIONTYPE rotationConfig;
    OMX_CONFIG_MIRRORTYPE mirrorConfig;
    
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSharpenConfig;
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSaturationConfig;
	
};