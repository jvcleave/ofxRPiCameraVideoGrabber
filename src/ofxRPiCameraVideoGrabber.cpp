#include "ofxRPiCameraVideoGrabber.h"

#pragma mark SETUP
ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
	OMX_Maps::getInstance(); 
    resetValues();
    
    int zoomStepsSource[61] = 
    {
        65536,  68157,  70124,  72745,
        75366,  77988,  80609,  83231,
        86508,  89784,  92406,  95683,
        99615,  102892, 106168, 110100,
        114033, 117965, 122552, 126484,
        131072, 135660, 140247, 145490,
        150733, 155976, 161219, 167117,
        173015, 178913, 185467, 192020,
        198574, 205783, 212992, 220201,
        228065, 236585, 244449, 252969,
        262144, 271319, 281149, 290980,
        300810, 311951, 322437, 334234,
        346030, 357827, 370934, 384041,
        397148, 411566, 425984, 441057,
        456131, 472515, 488899, 506593,
        524288
    };
    vector<int> converted(zoomStepsSource, 
                          zoomStepsSource + sizeof zoomStepsSource / sizeof zoomStepsSource[0]);
    zoomLevels = converted;

    
    cropRectangle.set(0,0,100,100);

    hasAddedExithandler = false;
	updateFrameCounter = 0;
	frameCounter = 0;
	hasNewFrame = false;
	textureEngine = NULL;
	directEngine = NULL;
    camera = NULL;
	pixelsRequested = false;
    recordingRequested = false;
	ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
}

void ofxRPiCameraVideoGrabber::reset()
{
    resetValues();
    applyAllSettings();
}

void ofxRPiCameraVideoGrabber::resetValues()
{
    ofLogVerbose(__func__) << endl;
    exposurePreset = "Auto";
    
    meteringType = OMX_MeteringModeAverage;
    autoISO = true;
    ISO = 0;
    autoShutter = true;
    shutterSpeed = 0;
    
    sharpness=-50;
    contrast=-10;
    brightness=50;
    saturation=0;
    frameStabilization=false;
    flickerCancellation = false;
    whiteBalance="Auto";
    imageFilter="None";
    dreLevel=0;
    cropRectangle.set(0,0,100,100);
    zoomLevel=0;
    rotation=0;
    mirror="MIRROR_NONE";
    doDisableSoftwareSharpen = false;
    doDisableSoftwareSaturation = false;
    LED = true;
    
    OMX_INIT_STRUCTURE(exposurePresetConfig);
    exposurePresetConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(sharpnessConfig);
    sharpnessConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(framestabilizationConfig);
    framestabilizationConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(contrastConfig);
    contrastConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(brightnessConfig);
    brightnessConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(saturationConfig);
    saturationConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(whiteBalanceConfig);
    whiteBalanceConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(colorEnhancementConfig);
    colorEnhancementConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(imagefilterConfig);
    imagefilterConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(sensorCropConfig);
    sensorCropConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(digitalZoomConfig);
    digitalZoomConfig.nPortIndex = OMX_ALL;
        
    OMX_INIT_STRUCTURE(rotationConfig);
    rotationConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    
    OMX_INIT_STRUCTURE(mirrorConfig);
    mirrorConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    
    OMX_INIT_STRUCTURE(disableSoftwareSharpenConfig);
    OMX_INIT_STRUCTURE(disableSoftwareSaturationConfig);
        
    OMX_INIT_STRUCTURE(dreConfig);
      
    OMX_INIT_STRUCTURE(exposureConfig);
    exposureConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(flickerCancelConfig);
    flickerCancelConfig.nPortIndex = OMX_ALL;
    
    OMX_INIT_STRUCTURE(burstModeConfig);    
}

string ofxRPiCameraVideoGrabber::currentStateToString()
{
    stringstream info;
    info << "sharpness " << getSharpness() << endl;
    info << "contrast " << getContrast() << endl;
    info << "brightness " << getBrightness() << endl;
    info << "saturation " << getSaturation() << endl;

    info << "ISO " << getISO() << endl;
    info << "AutoISO " << getAutoISO() << endl;

    info << "DRE " << getDRE() << endl;
    info << "cropRectangle " << getCropRectangle() << endl;
    info << "zoomLevelNormalized " << getZoomLevelNormalized() << endl;
    info << "mirror " << getMirror() << endl;
    info << "rotation " << getRotation() << endl;
    info << "imageFilter " << getImageFilter() << endl;
    info << "exposurePreset " << getExposurePreset() << endl;
    info << "evCompensation " << getEvCompensation() << endl;
    info << "autoShutter " << getAutoShutter() << endl;
    info << "shutterSpeed " << getShutterSpeed() << endl;
    info << "meteringType " << getMeteringType() << endl;
    info << "SoftwareSaturationEnabled " << isSoftwareSaturationEnabled() << endl;
    info << "SoftwareSharpeningEnabled " << isSoftwareSharpeningEnabled() << endl;

    //OMXCameraSettings
    info << omxCameraSettings.toString() << endl;
    return info.str();
}




CameraState ofxRPiCameraVideoGrabber::getCameraState()
{
    CameraState cameraState;
    cameraState.setup(currentStateToString());
    return cameraState;    
}


void ofxRPiCameraVideoGrabber::saveStateToFile(string fileName)
{
    ofBuffer buffer(currentStateToString());
    if(fileName.empty())
    {
        fileName = "STATE_"+ofGetTimestampString()+".txt";
    }
    ofBufferToFile(fileName, buffer);    
}

void ofxRPiCameraVideoGrabber::setup(CameraState cameraState)
{
    setup(cameraState.cameraSettings);
    map<string, string> keyValueMap = cameraState.keyValueMap;
    for(auto iterator  = keyValueMap.begin(); iterator != keyValueMap.end(); iterator++) 
    {
        string key = iterator->first;
        string value = iterator->second;
        //ofLogVerbose(__func__) << "key: " << key << " value: " << value;

        if(key == "sharpness")  setSharpness(ofToInt(value));
        if(key == "contrast")   setContrast(ofToInt(value));
        if(key == "brightness") setBrightness(ofToInt(value));
        if(key == "saturation") setSaturation(ofToInt(value));
        if(key == "ISO")        setISO(ofToInt(value));
        if(key == "AutoISO")    setAutoISO(ofToBool(value));
        if(key == "DRE")        setDRE(ofToInt(value));
        if(key == "cropRectangle") 
        {
            vector<string> rectValues = ofSplitString(value, ",");
            if(rectValues.size() == 4)             
            {
                setSensorCrop(ofToInt(rectValues[0]),
                                            ofToInt(rectValues[1]),
                                            ofToInt(rectValues[2]),
                                            ofToInt(rectValues[3])); 
            }
        }
        if(key == "zoomLevelNormalized")    setZoomLevelNormalized(ofToFloat(value));
        if(key == "mirror")                 setMirror(value);
        if(key == "rotation")               setRotation(ofToInt(value));
        if(key == "imageFilter")            setImageFilter(value);
        if(key == "exposurePreset")         setExposurePreset(value);
        if(key == "evCompensation")         setEvCompensation(ofToInt(value));
        if(key == "autoShutter")            setAutoShutter(ofToBool(value));
        if(key == "shutterSpeed")           setShutterSpeed(ofToInt(value));
        if(key == "meteringType")           setMeteringType(value);
        
        if(key == "SoftwareSaturationEnabled") setSoftwareSaturation(ofToBool(value));
        if(key == "SoftwareSharpeningEnabled") setSoftwareSharpening(ofToBool(value));
    }
}

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings omxCameraSettings_)
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

    OMX_ERRORTYPE error = OMX_ErrorNone;
    error = OMX_Init();
    OMX_TRACE(error);
    
    omxCameraSettings = omxCameraSettings_;
    
    ofLogVerbose(__func__) << "omxCameraSettings: " << omxCameraSettings.toString();

    
    
    if(!hasAddedExithandler)
    {
        addExitHandler();
        hasAddedExithandler = true; 
    }
    if(directEngine)
    {
        delete directEngine;
        directEngine = NULL;
        camera = NULL;
        ofLogVerbose() << "deleted directEngine";
        resetValues();
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
        camera = NULL;
        ofLogVerbose() << "deleted textureEngine";
        resetValues();
    }
    
    if (omxCameraSettings.enableTexture) 
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
        
        directEngine = new DirectEngine(); 
        directEngine->setup(omxCameraSettings);
        camera = directEngine->camera;
    }
    
    checkBurstMode();
    error = applyExposure(__func__);
    OMX_TRACE(error);
    checkFlickerCancellation();
    applyAllSettings();
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

}

void ofxRPiCameraVideoGrabber::applyAllSettings()
{
    
    setExposurePreset(exposurePreset); 
    setMeteringType(meteringType);
    setAutoISO(autoISO);
    setISO(ISO);    
    setAutoShutter(autoShutter);
    setShutterSpeed(shutterSpeed);
    
    setSharpness(sharpness);
    setContrast(contrast);
    setBrightness(brightness);
    setSaturation(saturation);
    setFrameStabilization(frameStabilization);
    setWhiteBalance(whiteBalance);
    setImageFilter(imageFilter);
    setColorEnhancement(false);	 //TODO implement
    setDRE(dreLevel);
    setSensorCrop(cropRectangle);
    setDigitalZoom();
    setRotation(rotation);
    setMirror(mirror);
    
    setSoftwareSharpening(doDisableSoftwareSharpen);
    setSoftwareSaturation(doDisableSoftwareSaturation);
    applyExposure(__func__);
    
    //Requires gpio program provided via wiringPi
    //sudo apt-get install wiringpi
    ofFile gpioProgram("/usr/bin/gpio");
    hasGPIOProgram = gpioProgram.exists();
    
    if(hasGPIOProgram)
    {
        LED_PIN = getLEDPin();
        
        stringstream command;
        command << "gpio export ";
        command << LED_PIN;
        command << " out";
        
        if(system(command.str().c_str()) == 0)
        {
            //silence compiler warning 
        }
        LED = true;
        setLEDState(LED);
    } 
}

string ofxRPiCameraVideoGrabber::getLEDPin()
{
    //default as RPI1 GPIO Layout
    string result = "5";
    if(hasGPIOProgram)
    {
        string command = "gpio -v";
        FILE* myPipe = popen(command.c_str(), "r");
        char buffer[128];
        string commandOutput = "";
        while (!feof(myPipe)) 
        {
            if (fgets(buffer, 128, myPipe) != NULL)
            {
                commandOutput += buffer;
            }
        } 
        
        pclose (myPipe);
        //ofLogVerbose(__func__) << "result: " << result;
        if(!commandOutput.empty())
        {
            vector<string> contents = ofSplitString(commandOutput, "Type:");
            string secondLine = contents[1];
            //ofLogVerbose(__func__) << "secondLine: " << secondLine;
            vector<string> secondLineContents = ofSplitString(secondLine, ",");
            string modelValue = secondLineContents[0];
            ofLogVerbose(__func__) << "modelValue: " << modelValue;
            //assuming RPI2 and RPI3 GPIO layout is the same
            //TODO: check RPI3
            if(ofIsStringInString(modelValue, "2") || ofIsStringInString(modelValue, "3"))
            {
                result = "32";
            }
        }
    }
    
    return result;
    
}

bool ofxRPiCameraVideoGrabber::isReady()
{
    
    if (directEngine) 
    {
        return directEngine->isOpen;
    }
    
    if (textureEngine) 
    {
        return textureEngine->isOpen;
    }
    
    return false;
}

#pragma mark UPDATE
void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
	if(textureEngine)
	{
		frameCounter  = textureEngine->getFrameCounter();
		
	}else
	{
		if (directEngine) 
		{
			frameCounter  = directEngine->getFrameCounter();
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
    if (recordingRequested) 
    {
        recordingRequested = false;
        CameraState currentState = getCameraState();
        currentState.cameraSettings.doRecording = true;
        //ofLogVerbose() << "CALLING SETUP: " << currentStateToString();
        setup(currentState);
    }
	//ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}



#pragma mark GETTERS

bool ofxRPiCameraVideoGrabber::isFrameNew()
{
    return hasNewFrame;
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

#pragma mark PIXELS/TEXTURE
GLuint ofxRPiCameraVideoGrabber::getTextureID()
{
	if (!textureEngine) 
	{
        ofLogError(__func__) << "TEXTURE NOT ENABLED - RETURN UNALLOCATED TEXTURE ID";
        return errorTexture.texData.textureID;
	}
	return textureEngine->getTexture().texData.textureID;
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

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    unsigned char * pixels = NULL;
    if (textureEngine) {
        pixels = textureEngine->getPixels();
    }
    return pixels;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	if (!textureEngine) 
	{
		ofLogError(__func__) << "TEXTURE NOT ENABLED - RETURN UNALLOCATED TEXTURE";
        return errorTexture;
	}
	return textureEngine->getTexture();
}

#pragma mark RECORDING


bool ofxRPiCameraVideoGrabber::isRecording()
{
   
    if (!directEngine && !textureEngine)
    {
        return false;
    }
    
    bool result = false;
    if (directEngine) 
    {
        result = directEngine->isThreadRunning();
    }
    if (textureEngine) 
    {
       result =  textureEngine->isThreadRunning();
    }
    return result;
    
    
}

void ofxRPiCameraVideoGrabber::startRecording()
{
    if (!directEngine && !textureEngine)
    {
        return;
    }
    bool isCurrentlyRecording = isRecording();
    ofLogVerbose(__func__) << "isCurrentlyRecording: " << isCurrentlyRecording;
    if(!isCurrentlyRecording)
    {
        recordingRequested = true;
    }
    

}


void ofxRPiCameraVideoGrabber::stopRecording()
{
	if (directEngine) 
	{
		directEngine->stopRecording();
        /*
         
         direct mode has to use a lower resolution for display while recording
         set it back after recording
         */
        CameraState currentState = getCameraState();
        currentState.cameraSettings.doRecording = false;
        setup(currentState);
	}
	if (textureEngine) 
	{
		textureEngine->stopRecording();
	}
}

#pragma mark DRAW
void ofxRPiCameraVideoGrabber::setDisplayAlpha(int alpha)
{
    if(directEngine)
    {
        getDisplayManager()->options.alpha = alpha;
    }
}

void ofxRPiCameraVideoGrabber::setDisplayRotation(int rotationDegrees)
{
    if(directEngine)
    {
        getDisplayManager()->rotateDisplay(rotationDegrees);
    }
}

void ofxRPiCameraVideoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    if(directEngine)
    {
        getDisplayManager()->options.drawRectangle = drawRectangle;
        
    }
}

void ofxRPiCameraVideoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    if(directEngine)
    {
        getDisplayManager()->options.cropRectangle = cropRectangle;
        
    }
}

void ofxRPiCameraVideoGrabber::setDisplayMirror(bool doMirror)
{
    if(directEngine)
    {
        getDisplayManager()->options.doMirror = doMirror;
    }
}

DirectDisplay* ofxRPiCameraVideoGrabber::getDisplayManager()
{
    if(directEngine)
    {
        return &directEngine->displayManager;
    }
    
    return NULL;
}


void ofxRPiCameraVideoGrabber::draw()
{
	if (textureEngine)
	{
		textureEngine->getTexture().draw(0, 0);
        return;
	}
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(0, 0, getWidth(), getHeight()));
    }
    
	
}

void ofxRPiCameraVideoGrabber::draw(int x, int y)
{
    if (textureEngine)
    {
        textureEngine->getTexture().draw(x, y);
        return;
    }
    
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(x, y, getWidth(), getHeight()));
    }
}


void ofxRPiCameraVideoGrabber::draw(int x, int y, int width, int height)
{
    if (textureEngine)
    {
        textureEngine->getTexture().draw(x, y, width, height);
        return;
    }
    
    
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(x, y, width, height));
    }
}

#pragma mark IMAGE ENHANCEMENT


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSoftwareSaturation(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        disableSoftwareSaturationConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
        OMX_TRACE(error); 
        if(error == OMX_ErrorNone)
        {
            doDisableSoftwareSaturation = state;
            ofLogVerbose() << "doDisableSoftwareSaturation: " << doDisableSoftwareSaturation;

        }
    }
    
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableSoftwareSaturation()
{
    return setSoftwareSaturation(false);
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::disableSoftwareSaturation()
{
    
    return setSoftwareSaturation(true);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSoftwareSharpening(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        disableSoftwareSharpenConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
        OMX_TRACE(error);   
        if(error == OMX_ErrorNone)
        {
            doDisableSoftwareSharpen = state;
            ofLogVerbose() << "doDisableSoftwareSharpen: " << doDisableSoftwareSharpen;
        }
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableSoftwareSharpening()
{
    return setSoftwareSharpening(false);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::disableSoftwareSharpening()
{
    
    return setSoftwareSharpening(true);
}

void ofxRPiCameraVideoGrabber::setSharpness(int sharpness_) //-100 to 100
{
	sharpness = sharpness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	sharpnessConfig.nSharpness = sharpness; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setContrast(int contrast_ ) //-100 to 100 
{
	contrast = contrast_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	contrastConfig.nContrast = contrast; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setBrightness(int brightness_ ) //0 to 100
{
	brightness = brightness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	brightnessConfig.nBrightness = brightness;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
    OMX_TRACE(error);

}

void ofxRPiCameraVideoGrabber::setSaturation(int saturation_) //-100 to 100
{
	saturation = saturation_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	saturationConfig.nSaturation	= saturation_; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
    OMX_TRACE(error);

}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
{
	
    colorEnhancementConfig.bColorEnhancement = toOMXBool(doColorEnhance);
    colorEnhancementConfig.nCustomizedU = U;
    colorEnhancementConfig.nCustomizedV = V;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &colorEnhancementConfig);
    OMX_TRACE(error);
    return error;

}
#pragma mark ISO

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoISO(bool doAutoISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposureConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        if(doAutoISO)
        {
            exposureConfig.bAutoSensitivity = OMX_TRUE; 
        }else
        {
            exposureConfig.bAutoSensitivity = OMX_FALSE; 
        }
        error = applyExposure(__func__);
        OMX_TRACE(error);
      
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setISO(int ISO_)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposureConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        exposureConfig.nSensitivity	= ISO_;
        error =  applyExposure(__func__);
        
        if(error == OMX_ErrorNone)
        {
            ISO = getISO();
        }
    }
    return error;
}

#pragma mark SHUTTER

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setShutterSpeed(int shutterSpeedMicroSeconds_)
{
   
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposureConfig);
    OMX_TRACE(error);
    exposureConfig.nShutterSpeedMsec = shutterSpeedMicroSeconds_;
    error =  applyExposure(__func__);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        shutterSpeed = getShutterSpeed();
    }
    ofLogVerbose(__func__) << "POST getShutterSpeed(): " << getShutterSpeed();
    return error;
    
}



OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoShutter(bool doAutoShutter)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposureConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        
        if(doAutoShutter)
        {
            exposureConfig.bAutoShutterSpeed = OMX_TRUE; 
        }else
        {
            exposureConfig.bAutoShutterSpeed = OMX_FALSE; 
        }
        error =  applyExposure(__func__);
        
        if(error == OMX_ErrorNone)
        {
            autoShutter = getAutoShutter();
        }
    }
    return error;
}

void ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    if (doStabilization) 
    {
        framestabilizationConfig.bStab = OMX_TRUE;
    }else 
    {
        framestabilizationConfig.bStab = OMX_FALSE;
    }
    
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
    if(error == OMX_ErrorNone) 
    {
        frameStabilization = doStabilization;
    }
    
    OMX_TRACE(error);
    
}

void ofxRPiCameraVideoGrabber::checkBurstMode()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigBurstCapture, &burstModeConfig);
    OMX_TRACE(error);
    burstModeEnabled = fromOMXBool(burstModeConfig.bEnabled);
}

void ofxRPiCameraVideoGrabber::setBurstMode(bool doBurstMode)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigBurstCapture, &burstModeConfig);
    OMX_TRACE(error);
    if(doBurstMode)
    {
        burstModeConfig.bEnabled = OMX_TRUE;  
    }else
    {
        burstModeConfig.bEnabled = OMX_FALSE;  

    }
    error = OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &burstModeConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        burstModeEnabled = doBurstMode;
    }
    ofLogVerbose() << "burstModeEnabled: " << burstModeEnabled;
}

#pragma mark FLICKER CANCELLATION

void ofxRPiCameraVideoGrabber::checkFlickerCancellation()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        switch (flickerCancelConfig.eFlickerCancel) 
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
        if(flickerCancelConfig.eFlickerCancel == OMX_COMMONFLICKERCANCEL_OFF)
        {
            flickerCancellation = false;
        }else
        {
            flickerCancellation = true;
        }
    }
    else
    {
        //error so assume it's not working
        flickerCancellation = false;
    }
}
void ofxRPiCameraVideoGrabber::setFlickerCancellation(bool enable)
{
    if(enable)
    {
       setFlickerCancellation(OMX_COMMONFLICKERCANCEL_AUTO); 
    }else
    {
        setFlickerCancellation(OMX_COMMONFLICKERCANCEL_OFF); 
    }
    
}

void ofxRPiCameraVideoGrabber::enableFlickerCancellation()
{
    setFlickerCancellation(true);
}

void ofxRPiCameraVideoGrabber::disableFlickerCancellation()
{
    setFlickerCancellation(false);
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel)
{
    
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        switch (flickerCancelConfig.eFlickerCancel) 
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
        flickerCancelConfig.eFlickerCancel = eFlickerCancel;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
        if(error == OMX_ErrorNone)
        {
            if(flickerCancelConfig.eFlickerCancel == OMX_COMMONFLICKERCANCEL_OFF)
            {
                flickerCancellation = false;
            }else
            {
                flickerCancellation = true;
            }
        }else
        {
            //error so assume it's not working
            flickerCancellation = false;
        }
        OMX_TRACE(error);
    }
    else
    {
        //error so assume it's not working
        flickerCancellation = false;
    }
    
    return error;
}

#pragma mark DYNAMIC RANGE EXPANSION (DRE)

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

#pragma mark METERING TYPE

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMeteringType(OMX_METERINGTYPE meteringType_)
{
    exposureConfig.eMetering = meteringType_;
    OMX_ERRORTYPE error = applyExposure(__func__);
    if(error == OMX_ErrorNone)
    {
        meteringType     = exposureConfig.eMetering;
    }

    return error;
}

string ofxRPiCameraVideoGrabber::getMeteringType()
{
    return GetMeteringString(exposureConfig.eMetering);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMeteringType(string meteringType_)
{    
    return setMeteringType(GetMetering(meteringType_));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyExposure(string caller)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposureConfig);
    OMX_TRACE(error);
#if 0
    stringstream info;
    info << "eMetering: " << exposureConfig.eMetering << endl;
    info << "bAutoShutterSpeed: " << exposureConfig.xEVCompensation << endl;
    info << "xEVCompensation: " << exposureConfig.xEVCompensation << endl;
    info << "nShutterSpeedMsec: " << exposureConfig.nShutterSpeedMsec << endl;
    info << "bAutoSensitivity: " << exposureConfig.bAutoSensitivity << endl;
    info << "nSensitivity: " << exposureConfig.nSensitivity << endl;
    info << endl;
    info << "eMetering: " << GetMeteringString(exposureConfig.eMetering) << endl;
    info << "autoShutter: " << autoShutter << endl;
    info << "shutterSpeed: " << shutterSpeed << endl;
    info << "autoISO: " << autoISO << endl;
    info << "ISO: " << ISO << endl;
    ofLogVerbose(__func__) << " caller: " << caller;
    ofLogVerbose(__func__) << " info: " << info.str();
#endif
    /*if (error == OMX_ErrorNone) 
    {
       meteringType     = exposureConfig.eMetering;
       evCompensation   = fromQ16(exposureConfig.xEVCompensation);
       
       autoShutter      = fromOMXBool(exposureConfig.bAutoShutterSpeed);
       shutterSpeed     = exposureConfig.nShutterSpeedMsec;
       
       autoISO          = fromOMXBool(exposureConfig.bAutoSensitivity);
       ISO              = exposureConfig.nSensitivity;
    }*/
    //ofLogVerbose(__func__) << printExposure();
    
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setEvCompensation(int value)
{
    if ((value >= -4) && (value <= 4)) 
    {
        exposureConfig.xEVCompensation = toQ16(value);
    }
    OMX_ERRORTYPE error =  applyExposure(__func__);
    if(error == OMX_ErrorNone)
    {
        evCompensation = getEvCompensation();
    }
    return error;
}


#pragma mark WHITE BALANCE

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE whiteBalance_)
{	
    whiteBalanceConfig.eWhiteBalControl = whiteBalance_;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &whiteBalanceConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        whiteBalance = whiteBalance_;
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setWhiteBalance(string name)
{	
    
    return setWhiteBalance(GetWhiteBalance(name));
}

string ofxRPiCameraVideoGrabber::getWhiteBalance()
{
    return GetWhiteBalanceString(whiteBalanceConfig.eWhiteBalControl);
}

#pragma mark CROP

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::updateSensorCrop()
{
    return setSensorCrop(cropRectangle);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSensorCrop(int left, int top, int width, int height)
{
    cropRectangle.set(left, top, width, height);
    return updateSensorCrop();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSensorCrop(ofRectangle& rectangle)
{


    sensorCropConfig.xLeft   = ((uint32_t)rectangle.getLeft()   << 16)/100;
    sensorCropConfig.xTop    = ((uint32_t)rectangle.getTop()    << 16)/100;
    sensorCropConfig.xWidth  = ((uint32_t)rectangle.getWidth()  << 16)/100;
    sensorCropConfig.xHeight = ((uint32_t)rectangle.getHeight() << 16)/100;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigInputCropPercentages, &sensorCropConfig);
    OMX_TRACE(error);
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << omxErrorToString(error);
        if(error == OMX_ErrorBadParameter)
        {
            ofLogWarning(__func__) << "resetting cropRectangle to known good params (0, 0, 100, 100)";
            cropRectangle.set(0, 0, 100, 100);
            return updateSensorCrop(); 
        }
        
    }
    return error;
    
}

#pragma mark ZOOM

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setZoomLevelNormalized(float value)
{
    if(value<0 || value>1)
    {
        ofLogError(__func__) << value << "MUST BE BETWEEN 0.0 - 1.0";
        return OMX_ErrorBadParameter;
    }
    zoomLevel = (int) ofMap(value, 0.0f, 1.0f, 0, zoomLevels.size());
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::resetZoom()
{
    zoomLevel = 0;
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::zoomIn()
{
    if((unsigned int)zoomLevel+1 < zoomLevels.size())
    {
        zoomLevel++;
    }
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::zoomOut()
{
    zoomLevel--;
    if(zoomLevel<0)
    {
        zoomLevel = 0;
    }
    
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setDigitalZoom()
{
    
    if(zoomLevel<0 || (unsigned int) zoomLevel>zoomLevels.size())
    {
        
        ofLogError(__func__) << "BAD zoomLevel: " << zoomLevel << " SETTING TO 0" << " zoomLevels.size(): " << zoomLevels.size();
        zoomLevel = 0;
    }
    
    
    int value = zoomLevels[zoomLevel];
    if(digitalZoomConfig.xWidth != value && digitalZoomConfig.xHeight != value)
    {
        digitalZoomConfig.xWidth  = value;
        digitalZoomConfig.xHeight = value;
        
        OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonDigitalZoom, &digitalZoomConfig);
        OMX_TRACE(error);
        return error;
    }
    return OMX_ErrorNone;
    
}

float ofxRPiCameraVideoGrabber::getZoomLevelNormalized()
{
    return ofMap(zoomLevel, 0, zoomLevels.size(), 0.0f, 1.0f);
}

#pragma mark MIRROR

string ofxRPiCameraVideoGrabber::getMirror()
{
    
    return GetMirrorString(mirrorConfig.eMirror);    
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMirror(int mirrorType)
{
    mirrorConfig.eMirror = (OMX_MIRRORTYPE)mirrorType;
    return applyMirror();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMirror(string mirror_)
{
    return setMirror(GetMirror(mirror_));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyMirror()
{
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonMirror, &mirrorConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        mirror = GetMirrorString(mirrorConfig.eMirror);
    }
    return error;
}

#pragma mark ROTATION

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setRotation(int value)
{
    return setRotation((ROTATION) value);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setRotation(ROTATION value)
{
    switch (value)
    {
        case ROTATION_0: {rotationConfig.nRotation=0;} break;
        case ROTATION_90: {rotationConfig.nRotation=90;} break;
        case ROTATION_180: {rotationConfig.nRotation=180;} break;
        case ROTATION_270: {rotationConfig.nRotation=270;} break;
        default: {rotationConfig.nRotation=0;} break;
    }
    return applyRotation();
}

int ofxRPiCameraVideoGrabber::getRotation()
{
    return rotationConfig.nRotation;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyRotation()
{
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonRotate, &rotationConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        rotation = getRotation();
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::rotateClockwise()
{
    int currentRotation  = getRotation();
    if(currentRotation+90<360)
    {
        rotationConfig.nRotation+=90;
    }else{
        rotationConfig.nRotation = 0;
    }
    
    return applyRotation();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::rotateCounterClockwise()
{
    int currentRotation  = getRotation();
    if(currentRotation-90>=0)
    {
        rotationConfig.nRotation-=90;
    }else{
        rotationConfig.nRotation = 270;
    }
    
    return applyRotation();
}


#pragma mark FILTERS

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setImageFilter(OMX_IMAGEFILTERTYPE imageFilter_)
{
    
    imagefilterConfig.eImageFilter = imageFilter_;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        imageFilter = GetImageFilterString(imageFilter_);
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setImageFilter(string imageFilter_)
{
    return setImageFilter(GetImageFilter(imageFilter_));
}

string ofxRPiCameraVideoGrabber::getImageFilter()
{
    return GetImageFilterString(imagefilterConfig.eImageFilter);
}


void ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
    
    imagefilterConfig.eImageFilter = imageFilter;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
    OMX_TRACE(error);
    
}


#pragma mark EXPOSURE PRESETS

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setExposurePreset(OMX_EXPOSURECONTROLTYPE exposurePreset_)
{
    exposurePresetConfig.eExposureControl = exposurePreset_;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposurePresetConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone)
    {
        exposurePreset = GetExposurePresetString(exposurePreset_);
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setExposurePreset(string exposurePreset_)
{
   
    return setExposurePreset(GetExposurePreset(exposurePreset_));
}


string ofxRPiCameraVideoGrabber::getExposurePreset()
{
    return GetExposurePresetString(exposurePresetConfig.eExposureControl);
}

void ofxRPiCameraVideoGrabber::toggleLED()
{
	setLEDState(!LED);
}

void ofxRPiCameraVideoGrabber::setLEDState(bool stateRequested)
{
    if (!hasGPIOProgram) 
    {
        return;
    }
    stringstream command;
    command << "gpio -g write ";
    command << LED_PIN;
    if (stateRequested) 
    {
       command <<  " 1";
    }else
    {
       command <<  " 0";
    }
    
   
    int result = system(command.str().c_str());
    if(result == 0)
    {
        LED = stateRequested;
    }
}

int ofxRPiCameraVideoGrabber::getISO() 
{ 
    return exposureConfig.nSensitivity;
}

bool ofxRPiCameraVideoGrabber::getAutoISO()
{
    return fromOMXBool(exposureConfig.bAutoSensitivity);
}

bool ofxRPiCameraVideoGrabber::getAutoShutter()
{
    return fromOMXBool(exposureConfig.bAutoShutterSpeed);
}

int ofxRPiCameraVideoGrabber::getShutterSpeed()
{
    return exposureConfig.nShutterSpeedMsec;
}

int ofxRPiCameraVideoGrabber::getEvCompensation()
{
    return fromQ16(exposureConfig.xEVCompensation);
}

string ofxRPiCameraVideoGrabber::printExposure()
{
    
    
    stringstream ss;
    ss << "meteringType: " << OMX_Maps::getInstance().getMetering(exposureConfig.eMetering) << "\n";
    ss << "evCompensation: " << getEvCompensation() << "\n";
    
    ss << "autoShutter: " << getAutoShutter() << "\n";
    ss << "shutterSpeedMicroSeconds: " << getShutterSpeed() << "\n";
    
    ss << "autoISO: " << getAutoISO() << "\n";
    ss << "ISO: " << getISO() << "\n";
    return ss.str();
    
}


#pragma mark EXIT

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

void ofxRPiCameraVideoGrabber::close()
{
    
    cout << "ofxRPiCameraVideoGrabber::close" << endl;
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(directEngine)
    {
        cout << "~ofxRPiCameraVideoGrabber delete directEngine" << endl;
        delete directEngine;
        directEngine = NULL;
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
    }
    
    cout << "~ofxRPiCameraVideoGrabber::close END" << endl;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    cout << "~ofxRPiCameraVideoGrabber" << endl;
    close();
}


