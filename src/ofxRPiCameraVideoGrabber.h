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

class ofxRPiCameraVideoGrabber
{

public:
	
	
	
	ofxRPiCameraVideoGrabber();
	~ofxRPiCameraVideoGrabber();
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
		
	
	void enableBurstMode();
	
	int getSharpness()		{ return sharpness; }
	int getContrast()		{ return contrast;	}
	int getBrightness()		{ return brightness; }
	int getSaturation()		{ return saturation; }
	
	OMXCameraSettings omxCameraSettings;
	
	void stopRecording();
private:
	
	void onUpdate(ofEventArgs & args);
	bool hasNewFrame;
	
	int updateFrameCounter;
	OMX_HANDLETYPE camera;
	
	int sharpness;	//	-100 to 100
	int contrast;	//  -100 to 100 
	int brightness; //     0 to 100
	int saturation; //  -100 to 100 
	
	//void close();
	
	void toggleImageEffects(bool doDisable);
	
	OMXCameraUtils omxCameraUtils;
	
	TextureEngine* textureEngine;
	NonTextureEngine* engine;
	
	
	
};