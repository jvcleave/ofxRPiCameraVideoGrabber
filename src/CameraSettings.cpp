//
//  CameraSettings.cpp
//  OF_JVCRPI_WORKSPACE
//
//  Created by jason van cleave on 3/31/15.
//
//

#include "CameraSettings.h"


CameraSettings::CameraSettings()
{
    exposurePreset = "Auto";
    meteringType="OMX_MeteringModeAverage";
    evCompensation=0;
    autoShutter=true;
    shutterSpeedMicroSeconds=0;
    autoAperture=true;
    aperture=0;
    autoISO=true;
    ISO=0;
    sharpness=-50;
    contrast=-10;
    brightness=50;
    saturation=0;
    framestabilization=false;
    whiteBalance="Auto";
    imageFilter="None";
    dreLevel=0;
    cropRectangle.set(0,0,100,100);
    zoomLevel=0;
    rotation=0;
    mirror="MIRROR_NONE";
    doDisableSoftwareSharpen = false;
    doDisableSoftwareSaturation = false;
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
    
    OMX_INIT_STRUCTURE(frameRateRangeConfig);
    
    OMX_INIT_STRUCTURE(rotationConfig);
    rotationConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    
    OMX_INIT_STRUCTURE(mirrorConfig);
    mirrorConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    
    OMX_INIT_STRUCTURE(disableSoftwareSharpenConfig);
    OMX_INIT_STRUCTURE(disableSoftwareSaturationConfig);
    
    OMX_INIT_STRUCTURE(cameraInfoConfig);
    
    OMX_INIT_STRUCTURE(dreConfig);
    
    OMX_INIT_STRUCTURE(hdrConfig);
    
    OMX_INIT_STRUCTURE(burstModeConfig);
    
    OMX_INIT_STRUCTURE(cameraDisableAlgorithmConfig);
    
    OMX_INIT_STRUCTURE(flickerCancelConfig);
    flickerCancelConfig.nPortIndex = OMX_ALL;
    
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
    
}

void CameraSettings::saveToFile(string filePath)
{
    
    stringstream state;
    
    //Metering Mode
    state << "exposurePreset="              << exposurePreset           << "\n";
    state << "meteringType="                << meteringType             << "\n";
    state << "evCompensation="              << evCompensation           << "\n";
    state << "autoShutter="                 << autoShutter              << "\n";
    state << "shutterSpeedMicroSeconds="    << shutterSpeedMicroSeconds << "\n";
    state << "autoAperture="                << autoAperture             << "\n";
    state << "aperture="                    << aperture                 << "\n";
    state << "autoISO="                     << autoISO                  << "\n";
    state << "ISO="                         << ISO                      << "\n";
    
    state << "sharpness="                   << sharpness                << "\n";
    state << "contrast="                    << contrast                 << "\n";
    state << "brightness="                  << brightness               << "\n";
    state << "saturation="                  << saturation               << "\n";
    
    state << "framestabilization="          << framestabilization       << "\n";
    state << "whiteBalance="                << whiteBalance             << "\n";
    state << "imageFilter="                 << imageFilter              << "\n";        
    state << "dreLevel="                    << dreLevel                 << "\n";
    state << "cropRectangle=" << cropRectangle.x  << "," << cropRectangle.y << "," << cropRectangle.width << "," << cropRectangle.height << "\n";
    state << "zoomLevel="                   << zoomLevel                << "\n";
    state << "rotation="                    << rotation                 << "\n";
    state << "mirror="                      << mirror                   << "\n";
    
    
    ofLogVerbose(__func__) << state.str();
    if(filePath.empty())
    {
        filePath = ofToDataPath("CameraSettings.ini", true);
        ofFile file(filePath);
        if(file.exists())
        {
            //file.renameTo(ofGetTimestampString()+"CameraSettings.ini", false);
        }
    }
    ofBuffer buffer(state.str());
    ofBufferToFile(filePath, buffer);
}

void CameraSettings::loadFromFile(string filePath)
{
    
    if(filePath.empty())
    {
        filePath = ofToDataPath("CameraSettings.ini", true);
        
    }
    ofFile file(filePath);
    if(!file.exists())
    {
        ofLogError(__func__) << "NO FILE AT " << filePath;
    }else
    {
        ofBuffer buffer = file.readToBuffer();
        string contents = buffer.getText();
        ofLogVerbose() << "contents: " << contents;
        vector<string> lines = ofSplitString(contents, "\n");
        ofLogVerbose(__func__) << "lines.size(): " << lines.size();
        for(size_t i=0; i<lines.size(); i++)
        {
            string& line = lines[i];
            
            vector<string> keyValues = ofSplitString(line, "=");
            processKeyValues(keyValues);
            
        }
    }
}

void CameraSettings::processKeyValues(vector<string>& keyValues)
{
    string& key = keyValues[0];
    string& value = keyValues[1];
    if(key == "exposurePreset") exposurePreset = value; return;
    if(key == "meteringType") meteringType = value;  return;
    if(key == "evCompensation") evCompensation = ofToInt(value); return;
    if(key == "autoShutter") autoShutter = ofToBool(value); return;
    if(key == "shutterSpeedMicroSeconds") shutterSpeedMicroSeconds = ofToInt(value); return;
    if(key == "autoAperture") autoAperture = ofToBool(value); return;
    if(key == "aperture") aperture = ofToInt(value); return;
    if(key == "autoISO") autoISO = ofToBool(value); return;
    if(key == "ISO") ISO = ofToInt(value); return;
    if(key == "sharpness") sharpness = ofToInt(value); return;
    if(key == "contrast") contrast = ofToInt(value); return;
    if(key == "brightness") brightness = ofToInt(value); return;
    if(key == "saturation") saturation = ofToInt(value); return;
    if(key == "framestabilization") framestabilization = ofToBool(value); return;
    if(key == "whiteBalance") whiteBalance = value; return;
    if(key == "imageFilter") imageFilter = value; return;
    if(key == "dreLevel") dreLevel = ofToInt(value); return;
    if(key == "zoomLevel") zoomLevel = ofToInt(value); return;
    if(key == "rotation") rotation = ofToInt(value); return;
    if(key == "mirror") mirror = value; return;
    if(key == "doDisableSoftwareSharpen") doDisableSoftwareSharpen = ofToBool(value); return;
    if(key == "doDisableSoftwareSaturation") doDisableSoftwareSaturation = ofToBool(value); return;
    
    if(key == "cropRectangle")
    {
        vector<string> rectValues = ofSplitString(value, ",");
        cropRectangle.set(ofToInt(rectValues[0]),
                          ofToInt(rectValues[1]),
                          ofToInt(rectValues[2]),
                          ofToInt(rectValues[3])
                          );
    }
    
}

void CameraSettings::setDefaultValues()
{
    
    setExposurePreset(exposurePreset); 
    CameraMeteringMode cameraMeteringMode;
    cameraMeteringMode.meteringType = OMX_Maps::getInstance().getMetering(meteringType);
    cameraMeteringMode.evCompensation = evCompensation;
    cameraMeteringMode.autoShutter = autoShutter;
    cameraMeteringMode.shutterSpeedMicroSeconds = shutterSpeedMicroSeconds;
    cameraMeteringMode.autoAperture = autoAperture;
    cameraMeteringMode.aperture = aperture;
    cameraMeteringMode.autoISO = autoISO;
    cameraMeteringMode.ISO = ISO;
    applyCurrentMeteringMode();
    
    
    setSharpness(sharpness);
    setContrast(contrast);
    setBrightness(brightness);
    setSaturation(saturation);
    setFrameStabilization(framestabilization);
    setWhiteBalance(whiteBalance);
    setImageFilter(imageFilter);
    setColorEnhancement(false);	 //TODO implement
    setDRE(dreLevel);
    cropRectangle = cropRectangle;
    setSensorCrop(cropRectangle);
    zoomLevel = zoomLevel;
    setDigitalZoom();
    setRotation(rotation);
    setMirror(mirror);
    
    setSoftwareSharpening(doDisableSoftwareSharpen);
    setSoftwareSaturation(doDisableSoftwareSaturation);
    //Requires gpio program provided via wiringPi
    //https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/
    
    ofFile gpioProgram("/usr/local/bin/gpio");
    if(gpioProgram.exists())
    {
        if(system("gpio export 5 out") == 0)
        {
            //silence compiler warning 
        }
        LED_CURRENT_STATE = true;
        setLEDState(LED_CURRENT_STATE);
    } 

    
}
void CameraSettings::saveState()
{
    
    exposurePreset = OMX_Maps::getInstance().getExposurePreset(exposurePresetConfig.eExposureControl);
    meteringType=currentMeteringMode.getMeteringTypeAsString();
    evCompensation=currentMeteringMode.evCompensation;
    autoShutter=currentMeteringMode.autoShutter;
    shutterSpeedMicroSeconds=currentMeteringMode.shutterSpeedMicroSeconds;
    autoAperture=currentMeteringMode.autoAperture;
    aperture=currentMeteringMode.aperture;
    autoISO=currentMeteringMode.autoISO;
    ISO=currentMeteringMode.ISO;
    
    sharpness=getSharpness();
    contrast=getContrast();
    brightness=getBrightness();
    saturation=getSaturation();
    
    framestabilization=fromOMXBool(framestabilizationConfig.bStab);
    
    whiteBalance=OMX_Maps::getInstance().getWhiteBalance(whiteBalanceConfig.eWhiteBalControl);
    
    imageFilter=OMX_Maps::getInstance().getImageFilter(imagefilterConfig.eImageFilter);
    
    dreLevel=OMX_Maps::getInstance().dreTypes[dreConfig.eMode];
    cropRectangle=cropRectangle;
    zoomLevel=zoomLevel;
    rotation=rotationConfig.nRotation;
    mirror = getMirror();    
}

void CameraSettings::loadStateFromFile(string filePath)
{
    loadFromFile(filePath);
}

void CameraSettings::saveCameraSettingsToFile(string filePath)
{
    saveState();
    saveToFile(filePath);
    
}

void CameraSettings::resetToDefaults()
{
    setDefaultValues();
}


string CameraSettings::getMirror()
{
    
    return OMX_Maps::getInstance().mirrorTypes[mirrorConfig.eMirror];
    
}


void CameraSettings::printCameraInfo()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCameraInfo, &cameraInfoConfig);
    OMX_TRACE(error);
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
    
    frameRateRangeConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    error = OMX_GetConfig(camera, OMX_IndexParamBrcmFpsRange, &frameRateRangeConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << "CAMERA_OUTPUT_PORT" << endl;
        ss << "xFramerateLow: "     << fromQ16(frameRateRangeConfig.xFramerateLow)  << endl;
        ss << "xFramerateHigh: "    << fromQ16(frameRateRangeConfig.xFramerateHigh) << endl;
        ofLogVerbose(__func__) << ss.str();
    }
    
    frameRateRangeConfig.nPortIndex = CAMERA_PREVIEW_PORT;
    error = OMX_GetConfig(camera, OMX_IndexParamBrcmFpsRange, &frameRateRangeConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << "CAMERA_PREVIEW_PORT" << endl;
        ss << "xFramerateLow: "     << fromQ16(frameRateRangeConfig.xFramerateLow)  << endl;
        ss << "xFramerateHigh: "    << fromQ16(frameRateRangeConfig.xFramerateHigh) << endl;
        ofLogVerbose(__func__) << ss.str();
    }
}


CameraSettings::EXPOSURE_MODE
CameraSettings::getExposureMode()
{
    if(currentMeteringMode.autoShutter && currentMeteringMode.autoAperture)
    {
        return EXPOSURE_MODE_AUTO;
    }
    if(!currentMeteringMode.autoShutter && !currentMeteringMode.autoAperture)
    {
        return EXPOSURE_MODE_MANUAL;
    }
    return EXPOSURE_MODE_INVALID;
}


OMX_ERRORTYPE CameraSettings::enableAutoExposure()
{
    currentMeteringMode.autoShutter = true;
    currentMeteringMode.autoAperture = true;
    currentMeteringMode.autoISO = true;
    return applyCurrentMeteringMode();
}

OMX_ERRORTYPE CameraSettings::enableManualExposure()
{
    currentMeteringMode.autoShutter = false;
    currentMeteringMode.autoAperture = false;
    currentMeteringMode.autoISO = false;
    return applyCurrentMeteringMode();
}


OMX_ERRORTYPE CameraSettings::setExposurePreset(OMX_EXPOSURECONTROLTYPE exposurePreset)
{
    exposurePresetConfig.eExposureControl = exposurePreset;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposurePresetConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setExposurePreset(string exposurePreset)
{
    return setExposurePreset(OMX_Maps::getInstance().getExposurePreset(exposurePreset));
}


string CameraSettings::getExposurePreset()
{
    
    return OMX_Maps::getInstance().getExposurePreset(exposurePresetConfig.eExposureControl);
}

OMX_ERRORTYPE CameraSettings::setEvCompensation(int value)
{
    if ((value >= -4) && (value <= 4)) 
    {
        currentMeteringMode.evCompensation = value;
    }
    return applyCurrentMeteringMode();
    
}
int CameraSettings::getEvCompensation()
{
    return currentMeteringMode.evCompensation;
}

OMX_ERRORTYPE CameraSettings::setMeteringMode(CameraMeteringMode cameraMeteringMode)
{
    currentMeteringMode = cameraMeteringMode;
    return applyCurrentMeteringMode();
}


OMX_ERRORTYPE CameraSettings::setMeteringType(OMX_METERINGTYPE meteringType)
{
    currentMeteringMode.meteringType = meteringType;
    return applyCurrentMeteringMode();    
}

OMX_ERRORTYPE CameraSettings::setMeteringType(string meteringType)
{
    
    currentMeteringMode.meteringType = OMX_Maps::getInstance().getMetering(meteringType);
    return applyCurrentMeteringMode();    
}

inline
OMX_ERRORTYPE CameraSettings::applyCurrentMeteringMode()
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

inline
void CameraSettings::updateCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE omxExposureValue)
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

string CameraSettings::getMeteringType()
{
    return OMX_Maps::getInstance().getMetering(currentMeteringMode.exposurevalue.eMetering);
}

/*
 string CameraSettings::meteringModetoString()
 {
 OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
 OMX_TRACE(error);
 
 stringstream ss;
 
 ss << "OMX_METERINGTYPE: "      <<  currentMeteringMode.getMeteringTypeAsString()  << "\n";
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
 
 */


OMX_ERRORTYPE CameraSettings::setAutoAperture(bool doAutoAperture)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoAperture	= toOMXBool(doAutoAperture);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        OMX_TRACE(error);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}



OMX_ERRORTYPE CameraSettings::setAutoISO(bool doAutoISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoSensitivity	= toOMXBool(doAutoISO);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        OMX_TRACE(error);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE CameraSettings::setISO(int ISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.nSensitivity	= ISO;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        OMX_TRACE(error);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}

int CameraSettings::getISO()
{
    return currentMeteringMode.ISO;
}


OMX_ERRORTYPE CameraSettings::setAperture(int aperture)
{
    currentMeteringMode.aperture = aperture;
    return applyCurrentMeteringMode();
}

int CameraSettings::getAperture()
{ 
    return currentMeteringMode.aperture;
}

OMX_ERRORTYPE CameraSettings::setShutterSpeed(int shutterSpeedMicroSeconds)
{
    
    currentMeteringMode.shutterSpeedMicroSeconds = shutterSpeedMicroSeconds;
    return applyCurrentMeteringMode();
}

int CameraSettings::getShutterSpeed()
{
    return currentMeteringMode.shutterSpeedMicroSeconds;
}

OMX_ERRORTYPE CameraSettings::setAutoShutter(bool doAutoShutter)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoShutterSpeed	= toOMXBool(doAutoShutter);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        OMX_TRACE(error);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE CameraSettings::setFrameStabilization(bool doStabilization)
{
    framestabilizationConfig.bStab = toOMXBool(doStabilization);
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setSharpness(int sharpness) //-100 to 100
{
    sharpnessConfig.nSharpness = sharpness; 
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setSharpnessNormalized(float sharpnessNormalized)
{
    return setSharpness(ofMap(sharpnessNormalized, 0.0f, 1.0f, -100, 100)); 
}

OMX_ERRORTYPE CameraSettings::setContrast(int contrast ) //-100 to 100 
{
    contrastConfig.nContrast = contrast; 
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setContrastNormalized(float contrastNormalized )
{
    return setContrast(ofMap(contrastNormalized, 0.0f, 1.0f, -100, 100)); 
}

OMX_ERRORTYPE CameraSettings::setBrightness(int brightness ) //0 to 100
{
    
    brightnessConfig.nBrightness = brightness;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setBrightnessNormalized(float brightnessNormalized ) //0 to 100
{
    return setBrightness(ofMap(brightnessNormalized, 0.0f, 1.0f, 0, 100));
}

OMX_ERRORTYPE CameraSettings::setSaturation(int saturation) //-100 to 100
{
    
    saturationConfig.nSaturation = saturation; 
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setSaturationNormalized(float saturationNormalized)
{
    
    OMX_ERRORTYPE error = setSaturation(ofMap(saturationNormalized, 0.0f, 1.0f, -100, 100));
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{	
    whiteBalanceConfig.eWhiteBalControl = controlType;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &whiteBalanceConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setWhiteBalance(string name)
{	
    
    return setWhiteBalance(OMX_Maps::getInstance().getWhiteBalance(name));
}

string CameraSettings::getWhiteBalance()
{
    return OMX_Maps::getInstance().getWhiteBalance(whiteBalanceConfig.eWhiteBalControl);
}

OMX_ERRORTYPE CameraSettings::setColorEnhancement(bool doColorEnhance, int U, int V)
{
    colorEnhancementConfig.bColorEnhancement = toOMXBool(doColorEnhance);
    colorEnhancementConfig.nCustomizedU = U;
    colorEnhancementConfig.nCustomizedV = V;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &colorEnhancementConfig);
    OMX_TRACE(error);
    return error;
}

ofRectangle& CameraSettings::getCropRectangle()
{
    return cropRectangle;
}

OMX_ERRORTYPE CameraSettings::updateSensorCrop()
{
    return setSensorCrop(cropRectangle);
}

OMX_ERRORTYPE CameraSettings::setSensorCrop(int left, int top, int width, int height)
{
    cropRectangle.set(left, top, width, height);
    return updateSensorCrop();
}

OMX_ERRORTYPE CameraSettings::setSensorCrop(ofRectangle& rectangle)
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

float CameraSettings::getZoomLevelNormalized()
{
    return ofMap(zoomLevel, 0, zoomLevels.size(), 0.0f, 1.0f);
}

OMX_ERRORTYPE CameraSettings::setZoomLevelNormalized(float value)
{
    if(value<0 || value>1)
    {
        ofLogError(__func__) << value << "MUST BE BETWEEN 0.0 - 1.0";
        return OMX_ErrorBadParameter;
    }
    zoomLevel = (int) ofMap(value, 0.0f, 1.0f, 0, zoomLevels.size());
    return setDigitalZoom();
}

OMX_ERRORTYPE CameraSettings::resetZoom()
{
    zoomLevel = 0;
    return setDigitalZoom();
}

OMX_ERRORTYPE CameraSettings::zoomIn()
{
    if((unsigned int)zoomLevel+1 < zoomLevels.size())
    {
        zoomLevel++;
    }
    return setDigitalZoom();
}

OMX_ERRORTYPE CameraSettings::zoomOut()
{
    zoomLevel--;
    if(zoomLevel<0)
    {
        zoomLevel = 0;
    }
    
    return setDigitalZoom();
}

OMX_ERRORTYPE CameraSettings::setDigitalZoom()
{
    
    if(zoomLevel<0 || (unsigned int) zoomLevel>zoomLevels.size())
    {
        
        ofLogError(__func__) << "BAD zoomLevel: " << zoomLevel << " SETTING TO 0";
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

bool CameraSettings::toggleLED()
{
    return setLEDState(!LED_CURRENT_STATE);
}

bool CameraSettings::setLEDState(bool state)
{
    
    /*
     https://github.com/raspberrypi/firmware/issues/186
     
     OMX_CONFIG_PRIVACYINDICATORTYPE ledConfig;
     OMX_INIT_STRUCTURE(ledConfig);
     
     if(turnLEDOn)
     {
     ledConfig.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOn;
     }
     else
     {
     ledConfig.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
     }
     
     OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &ledConfig);
     if(error == OMX_ErrorNone) 
     {
     ofLogVerbose(__func__) << " PASS";
     }else{
     ofLogError(__func__) << "FAIL " << omxErrorToString(error);
     }
     LED_CURRENT_STATE = turnLEDOn;
     
     return error;
     */
    
    LED_CURRENT_STATE = state;	
    string command = "gpio -g write 5 " + ofToString(LED_CURRENT_STATE);
    if(system(command.c_str()) == 0)
    {
        //silence compiler warning
    }; 
    
    return LED_CURRENT_STATE;
}


OMX_ERRORTYPE CameraSettings::setMirror(int mirrorType)
{
    mirrorConfig.eMirror = (OMX_MIRRORTYPE)mirrorType;
    return applyMirror();
}

OMX_ERRORTYPE CameraSettings::setMirror(string mirror)
{
    return setMirror(OMX_Maps::getInstance().getMirror(mirror));
}

inline
OMX_ERRORTYPE CameraSettings::applyMirror()
{
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonMirror, &mirrorConfig);
    OMX_TRACE(error);
    return error;
}



OMX_ERRORTYPE CameraSettings::setRotation(int value)
{
    return setRotation((ROTATION) value);
}

OMX_ERRORTYPE CameraSettings::setRotation(ROTATION value)
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

int CameraSettings::getRotation()
{
    return rotationConfig.nRotation;
}

inline
OMX_ERRORTYPE CameraSettings::applyRotation()
{
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonRotate, &rotationConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::rotateClockwise()
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

OMX_ERRORTYPE CameraSettings::rotateCounterClockwise()
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

OMX_ERRORTYPE CameraSettings::toggleImageEffects(bool doDisable)
{
    cameraDisableAlgorithmConfig.eAlgorithm = OMX_CameraDisableAlgorithmImageEffects;
    
    cameraDisableAlgorithmConfig.bDisabled = toOMXBool(doDisable);
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexParamCameraDisableAlgorithm, &cameraDisableAlgorithmConfig);
    OMX_TRACE(error);
    return error;
}

void CameraSettings::enableImageEffects()
{
    toggleImageEffects(true);
}

void CameraSettings::disableImageEffects()
{
    toggleImageEffects(false);
}

OMX_ERRORTYPE CameraSettings::setImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
    
    imagefilterConfig.eImageFilter = imageFilter;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::setImageFilter(string imageFilter)
{
    return setImageFilter(OMX_Maps::getInstance().getImageFilter(imageFilter));
}



OMX_ERRORTYPE CameraSettings::setDRE(int level)
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
    
    dreConfig.eMode = type;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigDynamicRangeExpansion, &dreConfig);
    OMX_TRACE(error);
    
    return error;
}


OMX_ERRORTYPE CameraSettings::setHDR(bool doHDR)
{
    hdrConfig.bEnabled = toOMXBool(doHDR);
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &hdrConfig);
    OMX_TRACE(error);
    return error;
}

OMX_ERRORTYPE CameraSettings::enableBurstMode()
{
    burstModeConfig.bEnabled = OMX_TRUE;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigBrcmHighDynamicRange, &burstModeConfig);
    OMX_TRACE(error);
    return error;
}


OMX_ERRORTYPE CameraSettings::setSoftwareSharpening(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        disableSoftwareSharpenConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
        OMX_TRACE(error);        
    }
    return error;
}
OMX_ERRORTYPE CameraSettings::enableSoftwareSharpening()
{
    return setSoftwareSharpening(false);
}
OMX_ERRORTYPE CameraSettings::disableSoftwareSharpening()
{
    
    return setSoftwareSharpening(true);
}

OMX_ERRORTYPE CameraSettings::setSoftwareSaturation(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
    OMX_TRACE(error);
    if(error == OMX_ErrorNone) 
    {
        disableSoftwareSaturationConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
        OMX_TRACE(error);        
    }
    
    return error;
}

OMX_ERRORTYPE CameraSettings::enableSoftwareSaturation()
{
    return setSoftwareSaturation(false);
}
OMX_ERRORTYPE CameraSettings::disableSoftwareSaturation()
{
    
    return setSoftwareSaturation(true);
}


OMX_ERRORTYPE CameraSettings::setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel)
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
        OMX_TRACE(error);
    }
    
    return error;
}


#if 0
OMX_ERRORTYPE CameraSettings::setImageFilterWithParams(OMX_CONFIG_IMAGEFILTERPARAMSTYPE imageFilter)
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



