#pragma once
#include "ofMain.h"
#include "OMX_Maps.h"
#include "DirectDisplay.h"
#include "OMXCameraSettings.h"


enum MIRROR
{
    MIRROR_NONE=OMX_MirrorNone,
    MIRROR_VERTICAL=OMX_MirrorVertical,
    MIRROR_HORIZONTAL=OMX_MirrorHorizontal,
    MIRROR_BOTH=OMX_MirrorBoth,
};

enum ROTATION
{
    ROTATION_0=0,
    ROTATION_90=90,
    ROTATION_180=180,
    ROTATION_270=270,
};


class RPICameraController
{
public:
    
    
    OMX_HANDLETYPE camera;

    string getLEDPin();
    bool hasGPIOProgram;
    OMXCameraSettings settings;


    
    void checkBurstMode();
    void checkFlickerCancellation();
    OMX_ERRORTYPE applyMirror();
    void applyImageFilter(OMX_IMAGEFILTERTYPE);
    OMX_ERRORTYPE applyRotation();
    OMX_ERRORTYPE applyCurrentMeteringMode();
    OMX_ERRORTYPE applyExposure();
    OMX_ERRORTYPE updateSensorCrop();
    
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
    OMX_CONFIG_SCALEFACTORTYPE digitalZoomConfig;
    
    OMX_CONFIG_FLICKERCANCELTYPE flickerCancelConfig;
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE dreConfig;
    OMX_CONFIG_INPUTCROPTYPE sensorCropConfig;
    
    
    
    OMX_CONFIG_ROTATIONTYPE rotationConfig;
    OMX_CONFIG_MIRRORTYPE mirrorConfig;
    
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSharpenConfig;
    OMX_CONFIG_BOOLEANTYPE disableSoftwareSaturationConfig;
    
    void resetValues();
    void applyAllSettings();
    
    void setSharpness(int);
    int getSharpness() { return settings.sharpness; }
    
    void setContrast(int);
    int getContrast() { return settings.contrast; }
    
    void setBrightness(int);
    int getBrightness() { return settings.brightness; }
    
    void setSaturation(int);
    int getSaturation()        { return settings.saturation; }
    
    void setFrameStabilization(bool doStabilization);
    bool getFrameStabilization(){ return settings.frameStabilization;}
    
    OMX_ERRORTYPE setColorEnhancement(bool doColorEnhance, int U=128, int V=128);
    
    
    void setDRE(int level);
    int getDRE() { return settings.dreLevel; }
    
    void toggleLED();
    void setLEDState(bool status);
    bool getLEDState() { return settings.LED; }
    
    OMX_ERRORTYPE setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE);
    void setFlickerCancellation(bool);
    void enableFlickerCancellation();
    void disableFlickerCancellation();
    bool isFlickerCancellationEnabled() { return settings.flickerCancellation; }
    //TODO: enable explict 50/60 hz
    
    
    OMX_ERRORTYPE setSensorCrop(ofRectangle&);
    ofRectangle& getCropRectangle() { return settings.cropRectangle; }
    OMX_ERRORTYPE setSensorCrop(int left, int top, int width, int height);
    
    vector<int> zoomLevels;
    OMX_ERRORTYPE setDigitalZoom();
    OMX_ERRORTYPE zoomIn();
    OMX_ERRORTYPE zoomOut();
    OMX_ERRORTYPE resetZoom();
    OMX_ERRORTYPE setZoomLevelNormalized(float);
    float getZoomLevelNormalized();
    
    OMX_ERRORTYPE setMirror(int);
    OMX_ERRORTYPE setMirror(string);
    string getMirror();
    
    OMX_ERRORTYPE setRotation(int);
    OMX_ERRORTYPE setRotation(ROTATION);
    OMX_ERRORTYPE rotateClockwise();
    OMX_ERRORTYPE rotateCounterClockwise();
    int getRotation();
    
    OMX_ERRORTYPE setImageFilter(OMX_IMAGEFILTERTYPE);
    OMX_ERRORTYPE setImageFilter(string);
    string getImageFilter();
    
    OMX_ERRORTYPE setExposurePreset(OMX_EXPOSURECONTROLTYPE);
    OMX_ERRORTYPE setExposurePreset(string);
    string getExposurePreset();
    
    OMX_ERRORTYPE setEvCompensation(int); //-4 to 4
    int getEvCompensation();
    
    OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE);
    OMX_ERRORTYPE setWhiteBalance(string);
    string getWhiteBalance();
    
    OMX_ERRORTYPE setAutoShutter(bool);
    bool getAutoShutter();
    int getShutterSpeed();
    
    OMX_ERRORTYPE setShutterSpeed(int shutterSpeedMicroSeconds);
    
    OMX_ERRORTYPE setMeteringType(OMX_METERINGTYPE);
    OMX_ERRORTYPE setMeteringType(string);
    string getMeteringType();
    
    bool doDisableSoftwareSharpen;
    bool doDisableSoftwareSaturation;
    OMX_ERRORTYPE setSoftwareSharpening(bool);
    OMX_ERRORTYPE enableSoftwareSharpening();
    OMX_ERRORTYPE disableSoftwareSharpening();
    bool isSoftwareSharpeningEnabled() {return settings.doDisableSoftwareSharpen;}
    
    OMX_ERRORTYPE setSoftwareSaturation(bool);
    OMX_ERRORTYPE enableSoftwareSaturation();
    OMX_ERRORTYPE disableSoftwareSaturation();
    bool isSoftwareSaturationEnabled() {return settings.doDisableSoftwareSaturation;}
    
    void setBurstMode(bool);
    bool isBurstModeEnabled(){ return settings.burstModeEnabled;}
    
    OMX_ERRORTYPE setISO(int ISO);
    int getISO();
    
    OMX_ERRORTYPE setAutoISO(bool);
    bool getAutoISO();
    
    OMX_CONFIG_EXPOSUREVALUETYPE exposureConfig;
    
    
    string printExposure();
    
    string currentStateToString();
    void saveStateToFile(string fileName="");
    


};

