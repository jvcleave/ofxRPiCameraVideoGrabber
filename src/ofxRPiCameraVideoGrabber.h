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

class ofxRPiCameraVideoGrabber
{

public:
	
	
    
    
	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    void addExitHandler();
	void close();
	void setup(OMXCameraSettings omxCameraSettings);
	void draw();
	
	void applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter);
	ofTexture& getTextureReference();

	void setSharpness(int sharpness_);
	void setContrast(int contrast_);
	void setBrightness(int brightness_);
	void setSaturation(int saturation_);
	
	void setFrameStabilization(bool doStabilization);
	void setMeteringMode(OMX_METERINGTYPE meteringType, int evCompensation=0, int sensitivity = 100, bool autoSensitivity = false);
	void setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode);
	
	void setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType);
	void setColorEnhancement(bool doColorEnhance, int U=128, int V=128);
	void setLEDState(bool status);
    void setDRE(int level);

	bool LED_CURRENT_STATE;
	void toggleLED();
	
	GLuint getTextureID();
	
	int getWidth();
	int getHeight();
	int getFrameRate();
	bool isReady();

	
	void setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel);
	void disableImageEffects();
	void enableImageEffects();
	bool isFrameNew();
    bool isTextureEnabled();	
	
	void enableBurstMode();
	
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
    
    OMX_CONFIG_SCALEFACTORTYPE digitalZoomConfig;

    int zoomLevel;
    OMX_ERRORTYPE zoomIn();
    OMX_ERRORTYPE zoomOut();
    OMX_ERRORTYPE resetZoom();
    OMX_ERRORTYPE setZoomLevelNormalized(float);
    float getZoomLevelNormalized();

private:
	
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
	
};