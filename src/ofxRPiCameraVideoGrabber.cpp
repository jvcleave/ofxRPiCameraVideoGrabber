/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"

ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
    OMX_Maps::getInstance();
    initStructures();
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
    
    setDefaultValues();
}

void ofxRPiCameraVideoGrabber::setDefaultValues()
{
    
    setExposurePreset(OMX_ExposureControlAuto); 
    
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
    setDRC(0);
    
    //Requires gpio program provided via wiringPi
    //https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/
    
    ofFile gpioProgram("/usr/local/bin/gpio");
    if(gpioProgram.exists())
    {
        system("gpio export 5 out");
        LED_CURRENT_STATE = true;
        setLEDState(LED_CURRENT_STATE);
    }    
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableBurstMode()
{
    burstModeConfig.bEnabled = OMX_TRUE;
    return OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &burstModeConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::toggleImageEffects(bool doDisable)
{
    cameraDisableAlgorithmConfig.eAlgorithm = OMX_CameraDisableAlgorithmImageEffects;
    
    cameraDisableAlgorithmConfig.bDisabled = toOMXBool(doDisable);
    
    return OMX_SetConfig(camera, OMX_IndexParamCameraDisableAlgorithm, &cameraDisableAlgorithmConfig);	
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
    
    
    flickerCancelConfig.nPortIndex = OMX_ALL;
    
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << "camera OMX_GetConfig OMX_IndexConfigCommonFlickerCancellation PASS ";
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

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setExposurePreset(OMX_EXPOSURECONTROLTYPE exposureMode)
{
    exposurePresetConfig.eExposureControl = exposureMode;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposurePresetConfig);
}

/*
 int
 ofxRPiCameraVideoGrabber::getMeteringEvCompensation()
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


string ofxRPiCameraVideoGrabber::meteringModetoString()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error != OMX_ErrorNone)
    {
        ofLogError() << "COULD NOT UPDATE METERING MODE" << error;
    }

    stringstream ss;

    if(currentMeteringMode.meteringType == OMX_MeteringModeAverage)
    {
        ss << "OMX_MeteringModeAverage" << "\n";
    }
    if(currentMeteringMode.meteringType == OMX_MeteringModeSpot)
    {
        ss << "OMX_MeteringModeSpot" << "\n";
    }
    if(currentMeteringMode.meteringType == OMX_MeteringModeMatrix)
    {
        ss << "OMX_MeteringModeMatrix" << "\n";
    }
    if(currentMeteringMode.meteringType == OMX_MeteringModeBacklit)
    {
        ss << "OMX_MeteringModeBacklit" << "\n";
    }
    
    ss << "xEVCompensation: "   << toQ16(currentMeteringMode.exposurevalue.xEVCompensation)   << "\n";
    ss << "nApertureFNumber: "  << fromQ16(currentMeteringMode.exposurevalue.nApertureFNumber)  << "\n";
    ss << "bAutoAperture: "     << currentMeteringMode.exposurevalue.bAutoAperture              << "\n";
    ss << "nShutterSpeedMsec: " << currentMeteringMode.exposurevalue.nShutterSpeedMsec          << "\n";
    ss << "bAutoShutterSpeed: " << currentMeteringMode.exposurevalue.bAutoShutterSpeed          << "\n";
    ss << "nSensitivity: "      << currentMeteringMode.exposurevalue.nSensitivity               << "\n";
    ss << "bAutoSensitivity: "  << currentMeteringMode.exposurevalue.bAutoSensitivity           << "\n";
    return ss.str();
}

void ofxRPiCameraVideoGrabber::printMeteringMode()
{
    ofLogVerbose() << meteringModetoString();
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
                                                        int evCompensation,         //default 0
                                                        int sensitivity,            //default 100
                                                        int shutterSpeedMS,
                                                        bool autoShutter,           //default false
                                                        bool autoSensitivity,       //default false
                                                        bool autoAperture,          //default true
                                                        int aperture)
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);

    
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
    currentMeteringMode.exposurevalue.xEVCompensation =fromQ16(evCompensation); 
    
    
    currentMeteringMode.exposurevalue.bAutoAperture =toOMXBool(autoAperture);
    if(autoAperture)
    {
        if(aperture != 0)
        {
            ofLogWarning(__func__) << "aperture will be set to 0 because autoAperture is enabled";
        }
        currentMeteringMode.exposurevalue.nApertureFNumber = 0;
        
    }else
    {
        currentMeteringMode.exposurevalue.nApertureFNumber = toQ16(aperture);
    }
    
    currentMeteringMode.exposurevalue.bAutoSensitivity = toOMXBool(autoSensitivity);
    if(autoSensitivity)
    {
        if(sensitivity != 0)
        {
            ofLogWarning(__func__) << "sensitivity (ISO) will be set to 0 because autoSensitivity is enabled";
        }
        currentMeteringMode.exposurevalue.nSensitivity = 0;
    }else
    {
        currentMeteringMode.exposurevalue.nSensitivity = sensitivity; 
    }
    
    currentMeteringMode.exposurevalue.bAutoShutterSpeed = toOMXBool(autoShutter);
    
    if(autoShutter)
    {
        if(shutterSpeedMS != 0)
        {
            ofLogWarning(__func__) << "shutterSpeedMS will be set to 0 because autoShutter is enabled";
        }
        currentMeteringMode.exposurevalue.nShutterSpeedMsec = 0;
    }else
    {
        currentMeteringMode.exposurevalue.nShutterSpeedMsec = shutterSpeedMS;
    }
    currentMeteringMode.exposurevalue.eMetering = meteringType; 
    
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    
    if(error == OMX_ErrorNone)
    {
        setCurrentMeteringMode(currentMeteringMode.exposurevalue); 
    }else
    {
        ofLogError(__func__) << "FAILED TO SET METERING MODE " ;
        printMeteringMode();
        ofLogError(__func__) << "^^^ FAILED VALUES ^^^ " ;
    }
    
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
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
{
    framestabilizationConfig.bStab = toOMXBool(doStabilization);
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSharpness(int sharpness) //-100 to 100
{
    sharpnessConfig.nSharpness = sharpness; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSharpnessNormalized(int sharpnessNormalized)
{
    return setSharpness(ofMap(sharpnessNormalized, 0, 1, -100, 100)); 
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setContrast(int contrast ) //-100 to 100 
{
    contrastConfig.nContrast = contrast; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setContrastNormalized(int contrastNormalized )
{
    return setContrast(ofMap(contrastNormalized, 0, 1, -100, 100)); 
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setBrightness(int brightness ) //0 to 100
{
    
    brightnessConfig.nBrightness = brightness;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setBrightnessNormalized(int brightnessNormalized ) //0 to 100
{
    return setBrightness(ofMap(brightnessNormalized, 0, 1, 0, 100));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSaturation(int saturation) //-100 to 100
{
    
    saturationConfig.nSaturation = saturation; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSaturationNormalized(int saturationNormalized)
{
    
    return setSaturation(ofMap(saturationNormalized, 0, 1, -100, 100));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{	
    whiteBalanceConfig.eWhiteBalControl = controlType;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &whiteBalanceConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
{
    colorEnhancementConfig.bColorEnhancement = toOMXBool(doColorEnhance);
    colorEnhancementConfig.nCustomizedU = U;
    colorEnhancementConfig.nCustomizedV = V;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &colorEnhancementConfig);
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
    system(command.c_str());
    
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
    
    imagefilterConfig.eImageFilter = imageFilter;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setDRC(int level)
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
    
    drcConfig.eMode = type;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigDynamicRangeExpansion, &drcConfig);
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << "FAIL " << level;
    }else{
    
        ofLogVerbose(__func__) << "SUCCESS " << level;
    }
    
    return error;
}


#if 0
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyImageFilterWithParams(OMX_CONFIG_IMAGEFILTERPARAMSTYPE imageFilter)
{
    OMX_INIT_STRUCTURE(imageFilter);
    imageFilter.nPortIndex = OMX_ALL;
    imageFilter.nNumParams = 1;
    imageFilter.eImageFilter = OMX_ImageFilterBlur;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilterParameters, &imageFilter);
}


OMX_CONFIG_IMAGEFILTERPARAMSTYPE image_filter;
OMX_INIT_STRUCTURE(image_filter);

image_filter.nPortIndex = m_omx_image_fx.GetOutputPort();
if (m_anaglyph != OMX_ImageFilterAnaglyphNone)
{
    image_filter.nNumParams = 1;
    image_filter.nParams[0] = m_anaglyph;
    image_filter.eImageFilter = OMX_ImageFilterAnaglyph;
}
else
{
    image_filter.nNumParams = 1;
    image_filter.nParams[0] = 3;
    if (!advanced_deinterlace)
        image_filter.eImageFilter = OMX_ImageFilterDeInterlaceFast;
        else
            image_filter.eImageFilter = OMX_ImageFilterDeInterlaceAdvanced;
            }
omx_err = m_omx_image_fx.SetConfig(OMX_IndexConfigCommonImageFilterParameters, &image_filter);
if(omx_err != OMX_ErrorNone)
{
    CLog::Log(LOGERROR, "%s::%s - OMX_IndexConfigCommonImageFilterParameters omx_err(0x%08x)", CLASSNAME, __func__, omx_err);
    return false;
}

#endif

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






