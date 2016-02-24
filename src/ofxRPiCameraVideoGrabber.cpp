/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"

bool doExit = false;
void signal_handler(int signum)
{
    cout << "ofxRPiCameraVideoGrabber caught signal " << signum;
    doExit = true;
}

void ofxRPiCameraVideoGrabber::onUpdateDuringExit(ofEventArgs& args)
{
    if (doExit)
    {
        ofLogVerbose(__func__) << " EXITING VIA SIGNAL";
        close();
        ofExit();
    }
}

void ofxRPiCameraVideoGrabber::addExitHandler()
{
    
    vector<int> signals;
    signals.push_back(SIGINT);
    signals.push_back(SIGQUIT);
    
    for (size_t i=0; i<signals.size(); i++)
    {
        int SIGNAL_TO_BLOCK = signals[i];
        //http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
        
        //Struct for the new action associated to the SIGNAL_TO_BLOCK
        struct sigaction new_action;
        new_action.sa_handler = signal_handler;
        
        //Empty the sa_mask. This means that no signal is blocked while the signal_handler runs.
        sigemptyset(&new_action.sa_mask);
        
        //Block the SEGTERM signal so while the signal_handler runs, the SIGTERM signal is ignored
        sigaddset(&new_action.sa_mask, SIGTERM);
        
        //Remove any flag from sa_flag. See documentation for flags allowed
        new_action.sa_flags = 0;
        
        struct sigaction old_action;
        //Read the old signal associated to SIGNAL_TO_BLOCK
        sigaction(SIGNAL_TO_BLOCK, NULL, &old_action);
        
        //If the old handler wasn't SIG_IGN it is a handler that just "ignores" the signal
        if (old_action.sa_handler != SIG_IGN)
        {
            //Replace the signal handler of SIGNAL_TO_BLOCK with the one described by new_action
            sigaction(SIGNAL_TO_BLOCK, &new_action, NULL);
        }

    }
    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdateDuringExit);
}

ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	//appEGLWindow->setThreadTimeout(1000);
	OMX_Maps::getInstance();
	updateFrameCounter = 0;
	frameCounter = 0;
	hasNewFrame = false;
	textureEngine = NULL;
	engine = NULL;
	pixelsRequested = false;
	ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    cout << "~ofxRPiCameraVideoGrabber" << endl;
    close();
}
void ofxRPiCameraVideoGrabber::close()
{
    
    cout << "ofxRPiCameraVideoGrabber::close" << endl;
	ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
	if(engine)
	{
         cout << "~ofxRPiCameraVideoGrabber delete engine" << endl;
		delete engine;
		engine = NULL;
	}
	if(textureEngine)
	{
		delete textureEngine;
		textureEngine = NULL;
	}
    
    cout << "~ofxRPiCameraVideoGrabber::close END" << endl;
}

bool ofxRPiCameraVideoGrabber::isTextureEnabled()
{
    bool result = false;
    if(textureEngine)
    {
        result = true;
    }else
    {
        result = false;
    }
    return result;
}

void ofxRPiCameraVideoGrabber::enablePixels()
{
	if(textureEngine)
	{
		textureEngine->enablePixels();
		pixelsRequested = true;
	}
}

void ofxRPiCameraVideoGrabber::disablePixels()
{
	if(textureEngine)
	{
		textureEngine->disablePixels();
		pixelsRequested = false;
	}
}


void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
	if(textureEngine)
	{
		frameCounter  = textureEngine->getFrameCounter();
		
	}else
	{
		if (engine) 
		{
			frameCounter  = engine->getFrameCounter();
		}
	}
	
	if (frameCounter > updateFrameCounter) 
	{
		updateFrameCounter = frameCounter;
		hasNewFrame = true;
		
	}else
	{
		hasNewFrame = false;
	}
	if (hasNewFrame) 
	{
		if (textureEngine) 
		{
			if (pixelsRequested) 
			{
				textureEngine->updatePixels();
			}
		}
	}
	//ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
	unsigned char * pixels = NULL;
	if (textureEngine) {
		pixels = textureEngine->getPixels();
	}
	return pixels;
}

bool ofxRPiCameraVideoGrabber::isFrameNew()
{
	return hasNewFrame;
}

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings omxCameraSettings)
{
	
    addExitHandler();
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "OMX_Init PASS";
	}
	
	
	
	this->omxCameraSettings = omxCameraSettings;
	if (omxCameraSettings.isUsingTexture) 
	{
		textureEngine = new TextureEngine();
		textureEngine->setup(omxCameraSettings);
		camera = textureEngine->camera;
		if (omxCameraSettings.enablePixels) 
		{
			enablePixels();
		}
	}else 
	{
		engine = new NonTextureEngine();
		engine->setup(omxCameraSettings);
		camera = engine->camera;
	}

	
	
	setExposureMode(OMX_ExposureControlAuto);
	setMeteringMode(OMX_MeteringModeMatrix, 0, 0,  true); //OMX_MeteringModeMatrix, OMX_MeteringModeAverage, OMX_MeteringModeSpot, OMX_MeteringModeBacklit
	setSharpness(-50);
	setContrast(-10);
	setBrightness(50);
	setSaturation(0);
	setFrameStabilization(false);
    setDRE(0);
	setWhiteBalance(OMX_WhiteBalControlAuto);
	applyImageFilter(OMX_ImageFilterNone);
	setColorEnhancement(false);	 
	
	
	//Requires gpio program provided via wiringPi
	//https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/
	
	ofFile gpioProgram("/usr/local/bin/gpio");
	if(gpioProgram.exists())
	{
		system("gpio export 5 out");
		LED_CURRENT_STATE = true;
		setLEDState(LED_CURRENT_STATE);
	}
	//setLEDState(false);
	/*
	 OMX_COMMONFLICKERCANCEL_OFF,
	 OMX_COMMONFLICKERCANCEL_AUTO,
	 OMX_COMMONFLICKERCANCEL_50,
	 OMX_COMMONFLICKERCANCEL_60
	 */
	//setFlickerCancellation(OMX_COMMONFLICKERCANCEL_AUTO);
	//
	//enableFaceTracking();
	//disableImageEffects();
	
	
}
void ofxRPiCameraVideoGrabber::enableBurstMode()
{
	OMX_CONFIG_BOOLEANTYPE doBurstMode;
	OMX_INIT_STRUCTURE(doBurstMode);
	doBurstMode.bEnabled = OMX_TRUE;
	OMX_ERRORTYPE error =  OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &doBurstMode);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::toggleImageEffects(bool doDisable)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	OMX_PARAM_CAMERADISABLEALGORITHMTYPE controlType;
	OMX_INIT_STRUCTURE(controlType);
	controlType.eAlgorithm = OMX_CameraDisableAlgorithmImageEffects;
	if (doDisable) {
		controlType.bDisabled = OMX_TRUE;
	}else 
	{
		controlType.bDisabled = OMX_FALSE;
	}

	
	error = OMX_SetConfig(camera, OMX_IndexParamCameraDisableAlgorithm, &controlType);
    OMX_TRACE(error);

	
}

void ofxRPiCameraVideoGrabber::enableImageEffects()
{
	toggleImageEffects(true);
}

void ofxRPiCameraVideoGrabber::disableImageEffects()
{
	toggleImageEffects(false);
}

void ofxRPiCameraVideoGrabber::setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel)
{
	ofLogVerbose(__func__) << "setFlickerCancellation";
	OMX_CONFIG_FLICKERCANCELTYPE controlType;
	OMX_INIT_STRUCTURE(controlType);
	
	controlType.nPortIndex = OMX_ALL;
	
	OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &controlType);
    OMX_TRACE(error);

    if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "camera OMX_GetConfig OMX_IndexConfigCommonFlickerCancellation PASS ";
		switch (controlType.eFlickerCancel) 
		{
			case OMX_COMMONFLICKERCANCEL_OFF:
			{
				ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_OFF";
				break;
			}
			case OMX_COMMONFLICKERCANCEL_AUTO:
			{
				ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_AUTO";
				break;
			}
			case OMX_COMMONFLICKERCANCEL_50:
			{
				ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_50";
				break;
			}
			case OMX_COMMONFLICKERCANCEL_60:
			{
				ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_60";
				break;
			}
			default:
				break;
		}
		
	}

	controlType.eFlickerCancel = eFlickerCancel;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &controlType);
    OMX_TRACE(error);

}

int ofxRPiCameraVideoGrabber::getWidth()
{
	return omxCameraSettings.width;
}

int ofxRPiCameraVideoGrabber::getHeight()
{
	return omxCameraSettings.height;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
	return omxCameraSettings.framerate;
}

GLuint ofxRPiCameraVideoGrabber::getTextureID()
{
	if (!textureEngine) 
	{
		return 0;
	}
	return textureEngine->textureID;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	if (!textureEngine) 
	{
		ofLogError() << "TEXTURE NOT ENABLED - EXITING";
		ofExit(0);
	}
	return textureEngine->tex;
}


void ofxRPiCameraVideoGrabber::stopRecording()
{
	if (engine) 
	{
		engine->stopRecording();
	}
	if (textureEngine) 
	{
		textureEngine->stopRecording();
	}
}
void ofxRPiCameraVideoGrabber::draw()
{
	if (!textureEngine)
	{
		return;
	}
	textureEngine->tex.draw(0, 0);
}

bool ofxRPiCameraVideoGrabber::isReady()
{

	if (engine) 
	{
		return engine->isOpen;
	}
	
	if (textureEngine) 
	{
		return textureEngine->isOpen;
	}
	
	return false;
}

void ofxRPiCameraVideoGrabber::setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set exposure mode
	OMX_CONFIG_EXPOSURECONTROLTYPE exposure;
	OMX_INIT_STRUCTURE(exposure);
	exposure.nPortIndex = OMX_ALL;
	exposure.eExposureControl = exposureMode;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposure);
    OMX_TRACE(error);


}

void ofxRPiCameraVideoGrabber::setMeteringMode(OMX_METERINGTYPE meteringType, 
											   int evCompensation,	//default 0
											   int sensitivity,		//default 100
											   bool autoSensitivity //default false
											   )
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set EV compensation, ISO and metering mode
	OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
	OMX_INIT_STRUCTURE(exposurevalue);
	exposurevalue.nPortIndex = OMX_ALL;
	
	error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR,	"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
	}	 
		
	exposurevalue.xEVCompensation = evCompensation;	//Fixed point value stored as Q16 
	exposurevalue.nSensitivity = sensitivity;		//< e.g. nSensitivity = 100 implies "ISO 100" 
	exposurevalue.bAutoAperture = OMX_TRUE;
	//exposurevalue.nApertureFNumber = 2;
	if (autoSensitivity)
	{
		exposurevalue.bAutoSensitivity	= OMX_TRUE;
	}else 
	{
		exposurevalue.bAutoSensitivity	= OMX_FALSE;
	}
	
	exposurevalue.eMetering = meteringType; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setSharpness(int sharpness_) //-100 to 100
{
	sharpness = sharpness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
	OMX_INIT_STRUCTURE(sharpnessConfig);
	sharpnessConfig.nPortIndex = OMX_ALL;
	sharpnessConfig.nSharpness = sharpness; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_FRAMESTABTYPE framestabilizationConfig;
	OMX_INIT_STRUCTURE(framestabilizationConfig);
	framestabilizationConfig.nPortIndex = OMX_ALL;
	
	if (doStabilization) 
	{
		framestabilizationConfig.bStab = OMX_TRUE;
	}else 
	{
		framestabilizationConfig.bStab = OMX_FALSE;
	}

	error = OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setContrast(int contrast_ ) //-100 to 100 
{
	contrast = contrast_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_CONTRASTTYPE contrastConfig;
	OMX_INIT_STRUCTURE(contrastConfig);
	contrastConfig.nPortIndex = OMX_ALL;
	contrastConfig.nContrast = contrast; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setBrightness(int brightness_ ) //0 to 100
{
	brightness = brightness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
	OMX_INIT_STRUCTURE(brightnessConfig);
	brightnessConfig.nPortIndex = OMX_ALL;
	brightnessConfig.nBrightness = brightness;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setSaturation(int saturation_) //-100 to 100
{
	saturation = saturation_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SATURATIONTYPE saturationConfig;
	OMX_INIT_STRUCTURE(saturationConfig);
	saturationConfig.nPortIndex		= OMX_ALL;
	saturationConfig.nSaturation	= saturation_; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = controlType;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_COLORENHANCEMENTTYPE color;
	OMX_INIT_STRUCTURE(color);
	color.nPortIndex = OMX_ALL;
	
	if (doColorEnhance) 
	{
		color.bColorEnhancement = OMX_TRUE;
	}else
	{
		color.bColorEnhancement = OMX_FALSE;
	}
	
	color.nCustomizedU = U;
	color.nCustomizedV = V;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &color);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setDRE(int level)
{
    OMX_DYNAMICRANGEEXPANSIONMODETYPE type = OMX_DynRangeExpOff;
    switch (level) 
    {
        case 0:
        {
            type = OMX_DynRangeExpOff;
            break;
        }
        case 1:
        {
            type = OMX_DynRangeExpLow;
            break;
        }
        case 2:
        {
            type = OMX_DynRangeExpMedium;
            break;
        }
        case 3:
        {
            type = OMX_DynRangeExpHigh;
            break;
        }
        default:
        {
            type = OMX_DynRangeExpOff;
            break;
        }
            
    }
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE dreConfig;
    OMX_INIT_STRUCTURE(dreConfig);
    dreConfig.eMode = type;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigDynamicRangeExpansion, &dreConfig);
    OMX_TRACE(error);

    if(error == OMX_ErrorNone)
    {
        dreLevel = level;
    }
}

void ofxRPiCameraVideoGrabber::updateCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE omxExposureValue)
{
    currentMeteringMode.exposurevalue               = omxExposureValue;
    
    currentMeteringMode.meteringType                = omxExposureValue.eMetering;
    currentMeteringMode.evCompensation              = fromQ16(omxExposureValue.xEVCompensation);
    
    currentMeteringMode.autoShutter                 = fromOMXBool(omxExposureValue.bAutoShutterSpeed);
    currentMeteringMode.shutterSpeedMicroSeconds    = omxExposureValue.nShutterSpeedMsec;
    
    currentMeteringMode.autoAperture                = fromOMXBool(omxExposureValue.bAutoAperture);
    currentMeteringMode.aperture                    = fromQ16(omxExposureValue.nApertureFNumber);
    
    currentMeteringMode.autoISO                     = fromOMXBool(omxExposureValue.bAutoSensitivity);
    currentMeteringMode.ISO                         = omxExposureValue.nSensitivity;
    
    
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyCurrentMeteringMode()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    
    /*
     
     weird this is backwards from what is expected
     
     from : http://www.raspberrypi.org/forums/viewtopic.php?t=71915&p=519003
     "EV has a range of +/- 4 in steps of 1/6ths (because some people want steps of 1/3s, and others 1/2s). 
     It's MMAL that is awkward as it hasn't taken that into account and just uses an index counting in 1/6ths (ie 10 = 10/6 = +1 2/3).
     Do bear in mind that it the values will be clipped by the exposure mode and max analogue gain,
     so if you have a dark scene where the AGC algo has already turned everything up to max, 
     then selecting EV +4 isn't going to give you any further gain as there isn't any available."
     
     "For genuine EV +1, MMAL would take 6 (yuck! Really should fix that), and IL will take 65536.
     MMAL = Genuine * 6
     IL = Genuine * 65536
     
     In the GPU MMAL/RIL wrapper, we have param.xEVCompensation = (mmal_param->value << 16) / 6;
     Just watch out for rounding errors on your conversions - those 1/6ths don't always go the way you want."
     
     */
    
    currentMeteringMode.exposurevalue.eMetering         =   currentMeteringMode.meteringType; 
    currentMeteringMode.exposurevalue.xEVCompensation   =   toQ16(currentMeteringMode.evCompensation); 
    
    currentMeteringMode.exposurevalue.bAutoShutterSpeed =   toOMXBool(currentMeteringMode.autoShutter);
    currentMeteringMode.exposurevalue.nShutterSpeedMsec =   currentMeteringMode.shutterSpeedMicroSeconds;
    
    currentMeteringMode.exposurevalue.bAutoAperture     =   toOMXBool(currentMeteringMode.autoAperture);
    currentMeteringMode.exposurevalue.nApertureFNumber  =   toQ16(currentMeteringMode.aperture);
    
    currentMeteringMode.exposurevalue.bAutoSensitivity  =   toOMXBool(currentMeteringMode.autoISO);
    currentMeteringMode.exposurevalue.nSensitivity      =   currentMeteringMode.ISO;
    
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        updateCurrentMeteringMode(currentMeteringMode.exposurevalue); 
    }
    
    return error;
}



void ofxRPiCameraVideoGrabber::toggleLED()
{
	setLEDState(!LED_CURRENT_STATE);
}

void ofxRPiCameraVideoGrabber::setLEDState(bool status)
{
	//OMX doesn't work - using GPIO 
	/*OMX_ERRORTYPE error = OMX_ErrorNone;
	
	
	OMX_CONFIG_PRIVACYINDICATORTYPE privacy;
	OMX_INIT_STRUCTURE(privacy);
	privacy.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &privacy);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setLEDState FAIL error: 0x%08x", error);
	}*/
	
	LED_CURRENT_STATE = status;	
	string command = "gpio -g write 5 " + ofToString(LED_CURRENT_STATE);
	int result = system(command.c_str());
	ofLogVerbose(__func__) << "command: " << command << " result: " << result;
}

void ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
	OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
	OMX_INIT_STRUCTURE(imagefilterConfig);
	imagefilterConfig.nPortIndex = OMX_ALL;
	imagefilterConfig.eImageFilter = imageFilter;
	
	OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
    OMX_TRACE(error);

}







