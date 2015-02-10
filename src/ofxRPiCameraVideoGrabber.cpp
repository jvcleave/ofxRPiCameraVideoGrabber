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
    
    zoomLevel = 0;
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
    vector<int> converted(zoomStepsSource, zoomStepsSource + sizeof zoomStepsSource / sizeof zoomStepsSource[0]);
    zoomLevels = converted;

    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    
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
    
    if(omxCameraSettings.doManualExposure)
    {
        currentMeteringMode.autoShutter = false;
        currentMeteringMode.autoAperture = false;
        currentMeteringMode.autoISO = false;
        //currentMeteringMode.evCompensation = 6;
    }
    
    applyCurrentMeteringMode();
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
    cropRectangle.set(0, 0, 100, 100);
    setSensorCrop(cropRectangle);
    resetZoom();
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


void ofxRPiCameraVideoGrabber::printCameraInfo()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCameraInfo, &cameraInfoConfig);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << endl;
        ss << "cameraname: "        << cameraInfoConfig.cameraname                  << endl;
        ss << "lensname: "          << cameraInfoConfig.lensname                    << endl;
        ss << "nModelId: "          << fromQ16(cameraInfoConfig.nModelId)           << endl;
        ss << "nManufacturerId: "   << fromQ16(cameraInfoConfig.nManufacturerId)    << endl;
        ss << "nRevNum: "           << cameraInfoConfig.nRevNum                     << endl;
        ss << "sSerialNumber: "     << cameraInfoConfig.sSerialNumber               << endl;
        ss << "sEpromVersion: "     << cameraInfoConfig.sEpromVersion               << endl;
        
        ss << "xFNumber: "          << cameraInfoConfig.xFNumber                    << endl;
        ss << "xFNumber fromQ16: "  << fromQ16(cameraInfoConfig.xFNumber)           << endl;
        
        ss << "xFocalLength: "      << cameraInfoConfig.xFocalLength                << endl;
        ss << "xFocalLength fromQ16: "  << fromQ16(cameraInfoConfig.xFocalLength)   << endl;
        ss << "nShutterDelayTime fromQ16: "     << fromQ16(cameraInfoConfig.sLensCalibration.nShutterDelayTime)     << endl;
        ofLogVerbose(__func__) << ss.str();
    }
   
    
    

#if 0
    
    
    OMX_U16  nShutterDelayTime;
    OMX_U16  nNdTransparency;
    OMX_U16  nPwmPulseNearEnd;  /**< Num pulses to move lens 1um at near end */
    OMX_U16  nPwmPulseFarEnd;   /**< Num pulses to move lens 1um at far end */
    short unsigned int  nVoltagePIOutNearEnd[3];
    short unsigned int  nVoltagePIOut10cm[3];
    short unsigned int  nVoltagePIOutInfinity[3];
    OMX_U16  nVoltagePIOutFarEnd[3];
    OMX_U32  nAdcConversionNearEnd;
    OMX_U32  nAdcConversionFarEnd;
        /*ss << "nShutterDelayTime fromQ16: "     << fromQ16(cameraInfoConfig.sLensCalibration.nShutterDelayTime)     << endl;
         ss << "nNdTransparency fromQ16: "       << fromQ16(cameraInfoConfig.sLensCalibration.nNdTransparency)       << endl;
         ss << "nPwmPulseNearEnd fromQ16: "      << fromQ16(cameraInfoConfig.sLensCalibration.nPwmPulseNearEnd)      << endl;
         ss << "nPwmPulseFarEnd fromQ16: "       << fromQ16(cameraInfoConfig.sLensCalibration.nPwmPulseFarEnd)       << endl;
         ss << "nVoltagePIOutNearEnd: "          << fromQ16(cameraInfoConfig.sLensCalibration.nVoltagePIOutNearEnd)  << endl;
         ss << "nVoltagePIOut10cm: "             << fromQ16(cameraInfoConfig.sLensCalibration.nVoltagePIOut10cm)     << endl;
         ss << "nVoltagePIOutInfinity: "         << fromQ16(cameraInfoConfig.sLensCalibration.nVoltagePIOutInfinity) << endl;
         ss << "nVoltagePIOutFarEnd: "           << fromQ16(cameraInfoConfig.sLensCalibration.nVoltagePIOutFarEnd)   << endl;*/
        
        
        cout << "sSerialNumber: ";
        for (int i=0; i<20; i++) 
        {
            cout << fromQ16(cameraInfoConfig.sSerialNumber[i]);
            
        }
        cout << endl;
        
        cout << "nVoltagePIOutNearEnd: " <<  endl;
        for (int i=0; i<3; i++) {
            cout << fromQ16(cameraInfoConfig.sLensCalibration.nVoltagePIOutNearEnd[i]);
        }
        cout << endl;
        
        
       
    }
    
    
#define OMX_CONFIG_CAMERAINFOTYPE_NAME_LEN 16
#define OMX_CONFIG_CAMERAINFOTYPE_SERIALNUM_LEN 20
#define OMX_CONFIG_CAMERAINFOTYPE_EPROMVER_LEN 8
    typedef struct OMX_CONFIG_CAMERAINFOTYPE
    {
        OMX_U32 nSize;
        OMX_VERSIONTYPE nVersion;
        OMX_U8 cameraname[OMX_CONFIG_CAMERAINFOTYPE_NAME_LEN];
        OMX_U8 lensname[OMX_CONFIG_CAMERAINFOTYPE_NAME_LEN];
        OMX_U16 nModelId;
        OMX_U8 nManufacturerId;
        OMX_U8 nRevNum;
        OMX_U8 sSerialNumber[OMX_CONFIG_CAMERAINFOTYPE_SERIALNUM_LEN];
        OMX_U8 sEpromVersion[OMX_CONFIG_CAMERAINFOTYPE_EPROMVER_LEN];
        OMX_CONFIG_LENSCALIBRATIONVALUETYPE sLensCalibration;
        OMX_U32 xFNumber;
        OMX_U32 xFocalLength;
    } OMX_CONFIG_CAMERAINFOTYPE;
#endif
   // return error;
    
    
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



bool ofxRPiCameraVideoGrabber::isFrameNew()
{
    return hasNewFrame;
}

void ofxRPiCameraVideoGrabber::draw()
{
    if (!textureEngine)
    {
        return;
    }
    textureEngine->tex.draw(0, 0);
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
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMeteringMode(CameraMeteringMode cameraMeteringMode)
{
    currentMeteringMode = cameraMeteringMode;
    return applyCurrentMeteringMode();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyCurrentMeteringMode()
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
    
    currentMeteringMode.exposurevalue.eMetering         =   currentMeteringMode.meteringType; 
    currentMeteringMode.exposurevalue.xEVCompensation   =   toQ16(currentMeteringMode.evCompensation); 
    
    currentMeteringMode.exposurevalue.bAutoShutterSpeed =   toOMXBool(currentMeteringMode.autoShutter);
    currentMeteringMode.exposurevalue.nShutterSpeedMsec =   currentMeteringMode.shutterSpeedMicroSeconds;
    
    currentMeteringMode.exposurevalue.bAutoAperture     =   toOMXBool(currentMeteringMode.autoAperture);
    currentMeteringMode.exposurevalue.nApertureFNumber  =   toQ16(currentMeteringMode.aperture);
    
    currentMeteringMode.exposurevalue.bAutoSensitivity  =   toOMXBool(currentMeteringMode.autoISO);
    currentMeteringMode.exposurevalue.nSensitivity      =   currentMeteringMode.ISO;
    
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    
    if(error == OMX_ErrorNone)
    {
        updateCurrentMeteringMode(currentMeteringMode.exposurevalue); 
    }else
    {
        ofLogError(__func__) << "FAILED TO SET METERING MODE " ;
        printMeteringMode();
        ofLogError(__func__) << "^^^ FAILED VALUES ^^^ " ;
    }
    
    return error;
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

string ofxRPiCameraVideoGrabber::meteringModetoString()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error != OMX_ErrorNone)
    {
        ofLogError() << "COULD NOT UPDATE METERING MODE" << error;
    }
    
    stringstream ss;
    
    ss << "OMX_METERINGTYPE: "      <<  currentMeteringMode.getMeteringTypeString()  << "\n";
    ss << "xEVCompensation: Q16"    <<  currentMeteringMode.exposurevalue.xEVCompensation       << "\n";
    ss << "nApertureFNumber Q16: "  <<  currentMeteringMode.exposurevalue.nApertureFNumber      << "\n";
    ss << "bAutoAperture: "         <<  currentMeteringMode.exposurevalue.bAutoAperture         << "\n";
    ss << "nShutterSpeedMsec: "     <<  currentMeteringMode.exposurevalue.nShutterSpeedMsec     << "\n";
    ss << "bAutoShutterSpeed: "     <<  currentMeteringMode.exposurevalue.bAutoShutterSpeed     << "\n";
    ss << "nSensitivity: "          <<  currentMeteringMode.exposurevalue.nSensitivity          << "\n";
    ss << "bAutoSensitivity: "      <<  currentMeteringMode.exposurevalue.bAutoSensitivity      << "\n";
    
    ss << "\n";
    ss << currentMeteringMode.toString() << "\n";
    
    
    return ss.str();
}

void ofxRPiCameraVideoGrabber::printMeteringMode()
{
    ofLogVerbose() << meteringModetoString();
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
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoSensitivity(bool doAutoSensitivity)
{
    return setAutoISO(doAutoSensitivity);
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoISO(bool doAutoISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoSensitivity	= toOMXBool(doAutoISO);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setISO(int ISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.nSensitivity	= ISO;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }else
        {
            ofLogError(__func__) << printError(error);
        }
    }
    return error;
}

int ofxRPiCameraVideoGrabber::getISO()
{
    return currentMeteringMode.ISO;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAperture(int aperture)
{
    currentMeteringMode.aperture = aperture;
    return applyCurrentMeteringMode();
}

int ofxRPiCameraVideoGrabber::getAperture()
{ 
    return currentMeteringMode.aperture;
}
                
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setShutterSpeed(int shutterSpeedMicroSeconds)
{
    
    currentMeteringMode.shutterSpeedMicroSeconds = shutterSpeedMicroSeconds;
    return applyCurrentMeteringMode();
}

int ofxRPiCameraVideoGrabber::getShutterSpeed()
{
    return currentMeteringMode.shutterSpeedMicroSeconds;
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
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
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

ofRectangle& ofxRPiCameraVideoGrabber::getCropRectangle()
{
    return cropRectangle;
}

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
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << printError(error);
        if(error == OMX_ErrorBadParameter)
        {
            ofLogWarning(__func__) << "resetting cropRectangle to known good params (0, 0, 100, 100)";
            cropRectangle.set(0, 0, 100, 100);
            return updateSensorCrop(); 
        }
        
    }
    return error;

}
float ofxRPiCameraVideoGrabber::getZoomLevelNormalized()
{
    return ofMap(zoomLevel, 0, zoomLevels.size(), 0.0f, 1.0f);
}
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
        
        ofLogError(__func__) << "BAD zoomLevel: " << zoomLevel << " SETTING TO 0";
        zoomLevel = 0;
    }
   
    
    int value = zoomLevels[zoomLevel];
    if(digitalZoomConfig.xWidth != value && digitalZoomConfig.xHeight != value)
    {
        cout << "zoomLevel: " << zoomLevel << endl;
        digitalZoomConfig.xWidth  = value;
        digitalZoomConfig.xHeight = value;
        
        OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonDigitalZoom, &digitalZoomConfig);
        if(error != OMX_ErrorNone)
        {
            ofLogError(__func__) << printError(error);
        }
        return error;
    }
    return OMX_ErrorNone;
    
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
    
        //ofLogVerbose(__func__) << "SUCCESS " << level;
    }
    
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setHDR(bool doHDR)
{
    hdrConfig.bEnabled = toOMXBool(doHDR);
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &hdrConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << " PASS";
    }else{
        ofLogError(__func__) << "FAIL" << printError(error);
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableBurstMode()
{
    burstModeConfig.bEnabled = OMX_TRUE;
    return OMX_SetConfig(camera, OMX_IndexConfigBrcmHighDynamicRange, &burstModeConfig);
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




