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
    OMX_METERINGTYPE meteringType;
    int evCompensation;
    int sensitivity;
    int shutterSpeedMS;
    bool autoShutter;
    bool autoSensitivity;
    bool autoAperture;
    int aperture;
    OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
    CameraMeteringMode()
    {
        OMX_INIT_STRUCTURE(exposurevalue);
        exposurevalue.nPortIndex = OMX_ALL;
        
        meteringType = OMX_MeteringModeMatrix;
        evCompensation=0; //-10 to +10
        
        aperture = 0;
        autoAperture = true;
        
        shutterSpeedMS = 9977; //default 1000
        autoShutter = true;
        
        sensitivity = 0;
        autoSensitivity = true;
        
        
        
    };
#if 0       
    
defaults:
xEVCompensation: 0
nApertureFNumber: 0
bAutoAperture: 0
nShutterSpeedMsec: 0
bAutoShutterSpeed: 1
nSensitivity: 0
bAutoSensitivity: 1
    
    OMX_METERINGTYPE eMetering;
    OMX_S32 xEVCompensation;      /**< Fixed point value stored as Q16 */
    OMX_U32 nApertureFNumber;     /**< e.g. nApertureFNumber = 2 implies "f/2" - Q16 format */
    OMX_BOOL bAutoAperture;		/**< Whether aperture number is defined automatically */
    OMX_U32 nShutterSpeedMsec;    /**< Shutterspeed in milliseconds */ 
    OMX_BOOL bAutoShutterSpeed;	/**< Whether shutter speed is defined automatically */ 
    OMX_U32 nSensitivity;         /**< e.g. nSensitivity = 100 implies "ISO 100" */
    OMX_BOOL bAutoSensitivity;	/**< Whether sensitivity is defined automatically */
    
#endif
    
};

class ofxRPiCameraVideoGrabber
{

public:
	
	
	
	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    
	void close();
	void setup(OMXCameraSettings);
	void draw();
	
	
	ofTexture& getTextureReference();
    
    
    OMX_ERRORTYPE applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter);
    
	OMX_ERRORTYPE setSharpness(int sharpness_);
	OMX_ERRORTYPE setContrast(int contrast_);
	OMX_ERRORTYPE setBrightness(int brightness_);
	OMX_ERRORTYPE setSaturation(int saturation_);
	
	OMX_ERRORTYPE setFrameStabilization(bool doStabilization);
    
    void setMeteringMode(CameraMeteringMode);
	OMX_ERRORTYPE setMeteringMode(OMX_METERINGTYPE meteringType, 
                         int evCompensation, 
                         int sensitivity,
                         int shutterSpeedMS,
                         bool autoShutter,
                         bool autoSensitivity,
                         bool autoAperture,
                         int aperture);
    
	OMX_ERRORTYPE setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode);
	
	OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType);
    
	OMX_ERRORTYPE setColorEnhancement(bool doColorEnhance, 
                             int U=128, 
                             int V=128);
	void setLEDState(bool status);
	bool LED_CURRENT_STATE;
	void toggleLED();
	
	GLuint getTextureID();
	
	int getWidth();
	int getHeight();
	int getFrameRate();
	bool isReady();
	
	
	OMX_ERRORTYPE setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel);
	void disableImageEffects();
	void enableImageEffects();
	bool isFrameNew();
		
	
	OMX_ERRORTYPE enableBurstMode();
	
	int getSharpness()		{ return sharpness; }
	int getContrast()		{ return contrast;	}
	int getBrightness()		{ return brightness; }
	int getSaturation()		{ return saturation; }
	
	OMXCameraSettings omxCameraSettings;
	
	void stopRecording();
	void enablePixels();
	void disablePixels();
	unsigned char * getPixels();
    
    CameraMeteringMode currentMeteringMode;
    void printMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE);
    OMX_ERRORTYPE printCurrentMeteringMode();
    
    //
    void setCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue);
    OMX_ERRORTYPE setAutoAperture(bool);
    OMX_ERRORTYPE setAutoShutter(bool);
    OMX_ERRORTYPE setAutoSensitivity(bool);
	
private:
	void addExitHandler();
	void onUpdate(ofEventArgs & args);
    void onUpdateDuringExit(ofEventArgs& args);

	bool hasNewFrame;
	
	int updateFrameCounter;
	OMX_HANDLETYPE camera;
	
	int sharpness;	//	-100 to 100
	int contrast;	//  -100 to 100 
	int brightness; //     0 to 100
	int saturation; //  -100 to 100 
	
	//void close();
	
	OMX_ERRORTYPE toggleImageEffects(bool doDisable);
	
	OMXCameraUtils omxCameraUtils;
	
	TextureEngine* textureEngine;
	NonTextureEngine* engine;
	
	int frameCounter;
	
	bool pixelsRequested;
	
};