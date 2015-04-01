#pragma once

#include "ofMain.h"
#include "OMXCameraUtils.h"


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


class CameraSettings
{
public:
    
    CameraSettings();
    
    enum ROTATION
    {
        ROTATION_0=0,
        ROTATION_90=90,
        ROTATION_180=180,
        ROTATION_270=270,
    };
    
    enum MIRROR
    {
        MIRROR_NONE=OMX_MirrorNone,
        MIRROR_VERTICAL=OMX_MirrorVertical,
        MIRROR_HORIZONTAL=OMX_MirrorHorizontal,
        MIRROR_BOTH=OMX_MirrorBoth,
    };
    
    enum EXPOSURE_MODE
    {
        EXPOSURE_MODE_AUTO,
        EXPOSURE_MODE_MANUAL,
        EXPOSURE_MODE_INVALID,
    };
    
    string exposurePreset;
    string meteringType;
    int evCompensation;
    bool autoShutter;
    int shutterSpeedMicroSeconds;
    bool autoAperture;
    int aperture;
    bool autoISO;
    int ISO;
    int sharpness;
    int contrast;
    int brightness;
    int saturation;
    bool framestabilization;
    string whiteBalance;
    string imageFilter;
    int dreLevel;
    ofRectangle cropRectangle;
    int zoomLevel;
    int rotation;
    string mirror; 
    
    bool doDisableSoftwareSharpen;
    bool doDisableSoftwareSaturation;
    
    void resetValues();
    
    int getSharpness()  { return sharpnessConfig.nSharpness; }
    int getContrast()   { return contrastConfig.nContrast;	}
    int getBrightness() { return brightnessConfig.nBrightness; }
    int getSaturation() { return saturationConfig.nSaturation; }
    
    
    OMX_ERRORTYPE setMeteringMode(CameraMeteringMode);
    OMX_ERRORTYPE setMeteringType(OMX_METERINGTYPE);
    OMX_ERRORTYPE setMeteringType(string);
    string getMeteringType();
    
    OMX_ERRORTYPE setAutoShutter(bool);
    bool getAutoShutter(){return currentMeteringMode.autoShutter;}
    int getShutterSpeed();
    
    OMX_ERRORTYPE setShutterSpeed(int shutterSpeedMicroSeconds);
    
    OMX_ERRORTYPE setEvCompensation(int); //-4 to 4
    int getEvCompensation();
    
    
    OMX_ERRORTYPE setImageFilter(OMX_IMAGEFILTERTYPE);
    OMX_ERRORTYPE setImageFilter(string);
    string getImageFilter();
    
    OMX_ERRORTYPE setSharpness(int); //-100 to 100
    OMX_ERRORTYPE setSharpnessNormalized(float);
    
    OMX_ERRORTYPE setContrast(int); //-100 to 100
    OMX_ERRORTYPE setContrastNormalized(float);
    
    OMX_ERRORTYPE setBrightness(int); //0 to 100
    OMX_ERRORTYPE setBrightnessNormalized(float);
    
    OMX_ERRORTYPE setSaturation(int); //-100 to 100
    OMX_ERRORTYPE setSaturationNormalized(float);
    
    OMX_ERRORTYPE setFrameStabilization(bool doStabilization);
    
    OMX_ERRORTYPE setExposurePreset(OMX_EXPOSURECONTROLTYPE);
    OMX_ERRORTYPE setExposurePreset(string);
    string getExposurePreset();
    
    OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE);
    OMX_ERRORTYPE setWhiteBalance(string);
    string getWhiteBalance();
    
    OMX_ERRORTYPE setColorEnhancement(bool doColorEnhance, 
                                      int U=128, 
                                      int V=128);
    
    OMX_ERRORTYPE setDRE(int);//0-3
    
    OMX_ERRORTYPE setSensorCrop(ofRectangle& rectangle);
    OMX_ERRORTYPE setSensorCrop(int left, int top, int width, int height);
    ofRectangle& getCropRectangle();
    OMX_ERRORTYPE updateSensorCrop();
    
    
    OMX_ERRORTYPE zoomIn();
    OMX_ERRORTYPE zoomOut();
    OMX_ERRORTYPE resetZoom();
    
    
    OMX_ERRORTYPE setZoomLevelNormalized(float);
    float getZoomLevelNormalized();
    
    OMX_ERRORTYPE setRotation(int);
    OMX_ERRORTYPE setRotation(ROTATION);
    int getRotation();
    
    OMX_ERRORTYPE rotateClockwise();
    OMX_ERRORTYPE rotateCounterClockwise();
    

    OMX_ERRORTYPE setMirror(int);
    OMX_ERRORTYPE setMirror(string);
    string getMirror();
    
    OMX_ERRORTYPE setSoftwareSharpening(bool);
    OMX_ERRORTYPE enableSoftwareSharpening();
    OMX_ERRORTYPE disableSoftwareSharpening();
    bool isSoftwareSharpeningEnabled() {return !fromOMXBool(disableSoftwareSharpenConfig.bEnabled);}
    

    EXPOSURE_MODE getExposureMode();
    
    OMX_ERRORTYPE enableAutoExposure();
    OMX_ERRORTYPE enableManualExposure();
    
    OMX_ERRORTYPE setSoftwareSaturation(bool);
    OMX_ERRORTYPE enableSoftwareSaturation();
    OMX_ERRORTYPE disableSoftwareSaturation();
    bool isSoftwareSaturationEnabled() {return !fromOMXBool(disableSoftwareSaturationConfig.bEnabled);}

    
    //fixed aperture - no effect
    OMX_ERRORTYPE setAutoAperture(bool);
    int getAperture();
    OMX_ERRORTYPE setAperture(int);
    bool getAutoAperture(){return currentMeteringMode.autoAperture;}
    
    //no effect seen
    int getISO();
    OMX_ERRORTYPE setISO(int ISO);
    OMX_ERRORTYPE setAutoISO(bool);
    bool getAutoISO(){return currentMeteringMode.autoISO;} 
    
    //not sure if functional
    OMX_ERRORTYPE setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel);
    OMX_ERRORTYPE enableBurstMode();
    OMX_ERRORTYPE setHDR(bool doHDR); //doesn't seem to do anything

    
    CameraMeteringMode currentMeteringMode;
    void updateCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue);
    
    OMX_CONFIG_EXPOSURECONTROLTYPE exposurePresetConfig;
    
    OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
    OMX_CONFIG_FRAMESTABTYPE framestabilizationConfig;
    OMX_CONFIG_CONTRASTTYPE contrastConfig;
    OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
    OMX_CONFIG_SATURATIONTYPE saturationConfig;
    OMX_CONFIG_WHITEBALCONTROLTYPE whiteBalanceConfig;
    OMX_CONFIG_COLORENHANCEMENTTYPE colorEnhancementConfig;
    OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
    
    OMX_CONFIG_BOOLEANTYPE burstModeConfig;
    OMX_CONFIG_BOOLEANTYPE hdrConfig;
    OMX_CONFIG_SCALEFACTORTYPE digitalZoomConfig;
    
    OMX_PARAM_CAMERADISABLEALGORITHMTYPE cameraDisableAlgorithmConfig;
    OMX_CONFIG_FLICKERCANCELTYPE flickerCancelConfig;
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE dreConfig;
    OMX_CONFIG_INPUTCROPTYPE sensorCropConfig;
    
    
    OMX_CONFIG_CAMERAINFOTYPE cameraInfoConfig;
    OMX_PARAM_BRCMFRAMERATERANGETYPE frameRateRangeConfig;
    
    
    OMX_CONFIG_ROTATIONTYPE rotationConfig;
    OMX_CONFIG_MIRRORTYPE mirrorConfig;
    
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSharpenConfig;
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSaturationConfig;
    
    
    
    void processKeyValues(vector<string>& keyValues);
    
    
    void saveState();
    void saveToFile(string filePath="");
    void printCameraInfo();
    
    void loadFromFile(string filePath="");
    
    OMX_ERRORTYPE applyMirror();
    OMX_ERRORTYPE applyRotation();
    OMX_ERRORTYPE applyCurrentMeteringMode();
    
    vector<int> zoomLevels;
    OMX_ERRORTYPE setDigitalZoom();
    
    
    bool LED_CURRENT_STATE;
    
    OMX_ERRORTYPE toggleImageEffects(bool doDisable);
    
    OMX_HANDLETYPE camera;
    
    void resetCameraToDefaultSettings();
    
    void disableImageEffects();
    void enableImageEffects();
    
    bool toggleLED();
    bool getLEDState() { return LED_CURRENT_STATE; }
    bool setLEDState(bool turnLEDOn);
    
    void applyAllSettings();
    
    
};
