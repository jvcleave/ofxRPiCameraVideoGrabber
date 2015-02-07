/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"

OMX_BOOL toOMXBool(bool boolean)
{
    OMX_BOOL result;
    if(boolean)
    {
        result = OMX_TRUE;
    }else
    {
        result = OMX_FALSE;
    }
    return result;
}

bool fromOMXBool(OMX_BOOL omxBool)
{
    bool result;
    if(omxBool)
    {
        result = true;
    }else
    {
        result = false;
    }
    return result;
}

float fromQ16(float n)
{
    return n* 65536;
}

float toQ16(float n)
{
    return n*(1/65536.0);
}

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
	//ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
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

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings omxCameraSettings_)
{
	
    addExitHandler();
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(__func__) << "OMX_Init PASS";
	}
	omxCameraSettings = omxCameraSettings_;
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

	
	
	setExposureMode(OMX_ExposureControlLargeAperture); 
                                                //    OMX_ExposureControlOff,
                                                //    OMX_ExposureControlAuto,
                                                //    OMX_ExposureControlNight,
                                                //    OMX_ExposureControlBackLight,
                                                //    OMX_ExposureControlSpotLight,
                                                //    OMX_ExposureControlSports,
                                                //    OMX_ExposureControlSnow,
                                                //    OMX_ExposureControlBeach,
                                                //    OMX_ExposureControlLargeAperture,
                                                //    OMX_ExposureControlSmallAperture,
                                                //    OMX_ExposureControlVeryLong,
                                                //    OMX_ExposureControlFixedFps,
                                                //    OMX_ExposureControlNightWithPreview,
                                                //    OMX_ExposureControlAntishake,
                                                //    OMX_ExposureControlFireworks,
    //currentMeteringMode.autoShutter = false;
    //currentMeteringMode.autoAperture = false;
    //currentMeteringMode.autoSensitivity = false;
    //currentMeteringMode.evCompensation = 6;
    if(!omxCameraSettings.doManualExposure)
    {
        setMeteringMode(currentMeteringMode);
    }else
    {
        currentMeteringMode.autoShutter = false;
        currentMeteringMode.autoAperture = false;
        currentMeteringMode.autoSensitivity = false;
        //currentMeteringMode.evCompensation = 6;
        setMeteringMode(currentMeteringMode);
    }
    
	//setMeteringMode(OMX_MeteringModeMatrix, 0, 0,  true); //OMX_MeteringModeMatrix, OMX_MeteringModeAverage, OMX_MeteringModeSpot, OMX_MeteringModeBacklit
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
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableBurstMode()
{
	OMX_CONFIG_BOOLEANTYPE doBurstMode;
	OMX_INIT_STRUCTURE(doBurstMode);
	doBurstMode.bEnabled = OMX_TRUE;
	return OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &doBurstMode);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::toggleImageEffects(bool doDisable)
{
	OMX_PARAM_CAMERADISABLEALGORITHMTYPE controlType;
	OMX_INIT_STRUCTURE(controlType);
	controlType.eAlgorithm = OMX_CameraDisableAlgorithmImageEffects;
    
	controlType.bDisabled = toOMXBool(doDisable);

	return OMX_SetConfig(camera, OMX_IndexParamCameraDisableAlgorithm, &controlType);
	
}

void ofxRPiCameraVideoGrabber::enableImageEffects()
{
	toggleImageEffects(true);
}

void ofxRPiCameraVideoGrabber::disableImageEffects()
{
	toggleImageEffects(false);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel)
{
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
        controlType.eFlickerCancel = eFlickerCancel;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &controlType);
	}
    
    return error;
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

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode)
{
	
	//Set exposure mode
	OMX_CONFIG_EXPOSURECONTROLTYPE exposure;
	OMX_INIT_STRUCTURE(exposure);
	exposure.nPortIndex = OMX_ALL;
	exposure.eExposureControl = exposureMode;
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposure);
}

/*
int ofxRPiCameraVideoGrabber::getMeteringEvCompensation()
{
    OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
    OMX_INIT_STRUCTURE(exposurevalue);
    exposurevalue.nPortIndex = OMX_ALL;
    
    error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error != OMX_ErrorNone) 
    {
        ofLog(OF_LOG_ERROR,	"getMeteringEvCompensation OMX_GetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
    }
}
 */


void ofxRPiCameraVideoGrabber::printMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue)
{
    stringstream ss;
    ss << "xEVCompensation: "   << fromQ16(exposurevalue.xEVCompensation)   << "\n";
    ss << "nApertureFNumber: "  << fromQ16(exposurevalue.nApertureFNumber)  << "\n";
    ss << "bAutoAperture: "     << exposurevalue.bAutoAperture              << "\n";
    ss << "nShutterSpeedMsec: " << exposurevalue.nShutterSpeedMsec          << "\n";
    ss << "bAutoShutterSpeed: " << exposurevalue.bAutoShutterSpeed          << "\n";
    ss << "nSensitivity: "      << exposurevalue.nSensitivity               << "\n";
    ss << "bAutoSensitivity: "  << exposurevalue.bAutoSensitivity           << "\n";
    ofLogVerbose() << ss.str();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::printCurrentMeteringMode()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    printMeteringMode(currentMeteringMode.exposurevalue);
    return error;
}

void ofxRPiCameraVideoGrabber::setMeteringMode(CameraMeteringMode cameraMeteringMode)
{
    setMeteringMode(cameraMeteringMode.meteringType,
                    cameraMeteringMode.evCompensation, 
                    cameraMeteringMode.sensitivity,
                    cameraMeteringMode.shutterSpeedMS,
                    cameraMeteringMode.autoShutter,
                    cameraMeteringMode.autoSensitivity, 
                    cameraMeteringMode.autoAperture,
                    cameraMeteringMode.aperture);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMeteringMode(OMX_METERINGTYPE meteringType, 
											   int evCompensation,	//default 0
											   int sensitivity,		//default 100
                                               int shutterSpeedMS,
                                               bool autoShutter, //default false
											   bool autoSensitivity, //default false
                                               bool autoAperture, //default true
                                               int aperture)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	

	error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
	//printMeteringMode(currentMeteringMode.exposurevalue);
		
	currentMeteringMode.exposurevalue.xEVCompensation = toQ16(evCompensation);	//-10 to +10
	currentMeteringMode.exposurevalue.nSensitivity = sensitivity;               //< e.g. nSensitivity = 100 implies "ISO 100" 
    
    currentMeteringMode.exposurevalue.bAutoAperture =toOMXBool(autoAperture);
    
    if(!autoAperture) //TODO set anyway? likely overridden by autoAperture
    {
        
        currentMeteringMode.exposurevalue.nApertureFNumber = toQ16(aperture);
    }
	
	//
    
    currentMeteringMode.exposurevalue.bAutoSensitivity = toOMXBool(autoSensitivity);
	
    currentMeteringMode.exposurevalue.bAutoShutterSpeed = toOMXBool(autoShutter);
	if(!autoShutter)
    {
        currentMeteringMode.exposurevalue.nShutterSpeedMsec = shutterSpeedMS;
    }
    
	currentMeteringMode.exposurevalue.eMetering = meteringType; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);

    setCurrentMeteringMode(currentMeteringMode.exposurevalue);
    return error;
}

void ofxRPiCameraVideoGrabber::setCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE omxExposureValue)
{
    currentMeteringMode.exposurevalue = omxExposureValue;
    
    currentMeteringMode.meteringType = omxExposureValue.eMetering;
    currentMeteringMode.evCompensation = fromQ16(omxExposureValue.xEVCompensation);
    
    currentMeteringMode.aperture = fromQ16(omxExposureValue.nApertureFNumber);
    currentMeteringMode.autoAperture = fromOMXBool(omxExposureValue.bAutoAperture);
    
    currentMeteringMode.shutterSpeedMS = omxExposureValue.nShutterSpeedMsec; 
    currentMeteringMode.autoShutter = fromOMXBool(omxExposureValue.bAutoShutterSpeed);
    
    currentMeteringMode.sensitivity = omxExposureValue.nSensitivity;
    currentMeteringMode.autoSensitivity = fromOMXBool(omxExposureValue.bAutoSensitivity);
    
    
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoAperture(bool doAutoAperture)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoAperture	= toOMXBool(doAutoAperture);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            setCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoSensitivity(bool doAutoSensitivity)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoSensitivity	= toOMXBool(doAutoSensitivity);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            setCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoShutter(bool doAutoShutter)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoShutterSpeed	= toOMXBool(doAutoShutter);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            setCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSharpness(int sharpness_) //-100 to 100
{
	sharpness = sharpness_;
		
	OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
	OMX_INIT_STRUCTURE(sharpnessConfig);
	sharpnessConfig.nPortIndex = OMX_ALL;
	sharpnessConfig.nSharpness = sharpness; 
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
{
	
	OMX_CONFIG_FRAMESTABTYPE framestabilizationConfig;
	OMX_INIT_STRUCTURE(framestabilizationConfig);
	framestabilizationConfig.nPortIndex = OMX_ALL;
    framestabilizationConfig.bStab = toOMXBool(doStabilization);
    
	return OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setContrast(int contrast_ ) //-100 to 100 
{
	contrast = contrast_;
		
	OMX_CONFIG_CONTRASTTYPE contrastConfig;
	OMX_INIT_STRUCTURE(contrastConfig);
	contrastConfig.nPortIndex = OMX_ALL;
	contrastConfig.nContrast = contrast; 
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setBrightness(int brightness_ ) //0 to 100
{
	brightness = brightness_;
	
	OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
	OMX_INIT_STRUCTURE(brightnessConfig);
	brightnessConfig.nPortIndex = OMX_ALL;
	brightnessConfig.nBrightness = brightness;
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSaturation(int saturation_) //-100 to 100
{
	saturation = saturation_;
	
	
	OMX_CONFIG_SATURATIONTYPE saturationConfig;
	OMX_INIT_STRUCTURE(saturationConfig);
	saturationConfig.nPortIndex		= OMX_ALL;
	saturationConfig.nSaturation	= saturation_; 
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{	
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = controlType;
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
{
	
	OMX_CONFIG_COLORENHANCEMENTTYPE color;
	OMX_INIT_STRUCTURE(color);
	color.nPortIndex = OMX_ALL;

    color.bColorEnhancement = toOMXBool(doColorEnhance);
	color.nCustomizedU = U;
	color.nCustomizedV = V;
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &color);
    
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

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
	OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
	OMX_INIT_STRUCTURE(imagefilterConfig);
	imagefilterConfig.nPortIndex = OMX_ALL;
	imagefilterConfig.eImageFilter = imageFilter;
	
	return OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
}







