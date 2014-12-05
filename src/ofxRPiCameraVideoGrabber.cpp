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
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "BURST MODE ENABLED PASS";
		
	}else
	{
		ofLog(OF_LOG_ERROR, "BURST MODE ENABLED  FAIL error: 0x%08x", error);
		
	}
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
	
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "toggleImageEffects PASS";
		
	}else
	{
		ofLog(OF_LOG_ERROR, "toggleImageEffects FAIL error: 0x%08x", error);
		
	}
	
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
		
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetConfig FAIL OMX_IndexConfigCommonFlickerCancellation error: 0x%08x", error);
		
	}

	controlType.eFlickerCancel = eFlickerCancel;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &controlType);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "camera OMX_SetConfig OMX_IndexConfigCommonFlickerCancellation OMX_COMMONFLICKERCANCEL_AUTO PASS ";

	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig FAIL OMX_IndexConfigCommonFlickerCancellation error: 0x%08x", error);

	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonExposure FAIL error: 0x%08x", error);
	}else 
	{
		ofLogVerbose(__func__) << "camera OMX_SetConfig OMX_IndexConfigCommonExposure PASS";
	}

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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR,	"camera setMeteringMode FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setSharpness FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setFrameStabilization FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setContrast FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setBrightness FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setSaturation FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = controlType;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setWhiteBalance FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setColorEnhancement FAIL error: 0x%08x", error);
	}
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera applyImageFilter FAIL error: 0x%08x", error);
	}
}







