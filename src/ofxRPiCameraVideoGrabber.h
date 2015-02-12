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


struct CameraMeteringMode
{
    OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
    
    OMX_METERINGTYPE meteringType;
    int evCompensation;
    
    
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
        
        
        evCompensation=0; //-10 to +10
        
        autoShutter = true;
        shutterSpeedMicroSeconds = 0;
        
        autoAperture = true;
        aperture = 0;
        
        autoISO = true;
        ISO=0;
    };
    
    string getMeteringTypeString()
    {
        stringstream ss;
        
        if(meteringType == OMX_MeteringModeAverage)
        {
            ss << "OMX_MeteringModeAverage" << "\n";
        }
        if(meteringType == OMX_MeteringModeSpot)
        {
            ss << "OMX_MeteringModeSpot" << "\n";
        }
        if(meteringType == OMX_MeteringModeMatrix)
        {
            ss << "OMX_MeteringModeMatrix" << "\n";
        }
        if(meteringType == OMX_MeteringModeBacklit)
        {
            ss << "OMX_MeteringModeBacklit" << "\n";
        }
        return ss.str();
    };
    
    string toString()
    {
        stringstream ss;
        
        ss << getMeteringTypeString() << "\n";
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

class ofxRPiCameraVideoGrabber
{

public:

	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    
    OMXCameraSettings omxCameraSettings;
	
	void setup(OMXCameraSettings);
    
    void setDefaultValues();
    void draw();
	void close();
	
    ofTexture& getTextureReference();
    GLuint getTextureID();
    
    bool isReady();
    bool isFrameNew();

	int getWidth();
	int getHeight();
	int getFrameRate();
    
    int getSharpness()  { return sharpnessConfig.nSharpness; }
    int getContrast()   { return contrastConfig.nContrast;	}
    int getBrightness() { return brightnessConfig.nBrightness; }
    int getSaturation() { return saturationConfig.nSaturation; }
    
    void stopRecording();
    void enablePixels();
    void disablePixels();
    unsigned char * getPixels();
	void disableImageEffects();
	void enableImageEffects();
	
	
    void toggleLED();
    void setLEDState(bool status);

    string meteringModetoString();
    void printMeteringMode();
    
    
    OMX_ERRORTYPE setMeteringMode(CameraMeteringMode);
    
    //fixed aperture - no effect
    OMX_ERRORTYPE setAutoAperture(bool);
    int getAperture();
    OMX_ERRORTYPE setAperture(int aperture);
    
    
    OMX_ERRORTYPE setAutoShutter(bool);
    int getShutterSpeed();
    OMX_ERRORTYPE setShutterSpeed(int shutterSpeedMicroSeconds);
    
    
    
    int getISO();
    OMX_ERRORTYPE setISO(int ISO);
    OMX_ERRORTYPE setAutoSensitivity(bool); //alias to setAutoISO
    OMX_ERRORTYPE setAutoISO(bool);
    
    OMX_ERRORTYPE applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter);
    
    
    OMX_ERRORTYPE setSharpness(int sharpness); //-100 to 100
    OMX_ERRORTYPE setSharpnessNormalized(int sharpnessNormalized);
    
    OMX_ERRORTYPE setContrast(int contrast); //-100 to 100
    OMX_ERRORTYPE setContrastNormalized(int contrastNormalized);
    
    OMX_ERRORTYPE setBrightness(int brightness); //0 to 100
    OMX_ERRORTYPE setBrightnessNormalized(int brightnessNormalized);
    
    OMX_ERRORTYPE setSaturation(int saturation); //-100 to 100
    OMX_ERRORTYPE setSaturationNormalized(int saturationNormalized);
    
    OMX_ERRORTYPE setFrameStabilization(bool doStabilization);
    
    
    //setExposureMode is now setExposurePreset - sorry :(
    //OMX_ERRORTYPE setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode){return setExposurePreset(exposureMode);};
    OMX_ERRORTYPE setExposurePreset(OMX_EXPOSURECONTROLTYPE exposureMode);
    
    OMX_ERRORTYPE setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType);
    
    OMX_ERRORTYPE setColorEnhancement(bool doColorEnhance, 
                                      int U=128, 
                                      int V=128);
    
    OMX_ERRORTYPE setDRC(int level);
    OMX_ERRORTYPE setHDR(bool doHDR); //doesn't seem to do anything
    
    ofRectangle cropRectangle;
    OMX_ERRORTYPE setSensorCrop(ofRectangle& rectangle);
    OMX_ERRORTYPE setSensorCrop(int left, int top, int width, int height);
    ofRectangle& getCropRectangle();
    OMX_ERRORTYPE updateSensorCrop();
    
 
    OMX_ERRORTYPE zoomIn();
    OMX_ERRORTYPE zoomOut();
    OMX_ERRORTYPE resetZoom();
    float getZoomLevelNormalized();
    OMX_ERRORTYPE setZoomLevelNormalized(float);
    //not sure if functional
    OMX_ERRORTYPE setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel);
    OMX_ERRORTYPE enableBurstMode();
    
    void printCameraInfo();
    
    
    ofTexture texture;
    
    GLuint textureID;
    void updatePixels();
    unsigned char * pixels;
    bool doPixels;
    ofFbo fbo;
    EGLImageKHR eglImage;
    void generateEGLImage(int, int);
    void destroyEGLImage();
    ofAppEGLWindow *appEGLWindow;
    EGLDisplay display;
    EGLContext context;
    
    BaseEngine* getEngine();
    void saveImage(bool doRaw=true);
    bool doSaveImage;
    bool doRawSave;
private:
    OMX_ERRORTYPE applyCurrentMeteringMode();
    bool hasExitHandler;
    bool hasOMXInit;
    int zoomLevel;
    vector<int> zoomLevels;
    OMX_ERRORTYPE setDigitalZoom();
    

    
	void addExitHandler();
	void onUpdate(ofEventArgs & args);
    void onUpdateDuringExit(ofEventArgs& args);

	bool hasNewFrame;
	int updateFrameCounter;
    int frameCounter;
    
	OMX_HANDLETYPE camera;
	bool LED_CURRENT_STATE;
  
	OMX_ERRORTYPE toggleImageEffects(bool doDisable);
		
	TextureEngine* textureEngine;
	NonTextureEngine* engine;
    
	
	
	
    

    
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
    OMX_CONFIG_DYNAMICRANGEEXPANSIONTYPE drcConfig;
    OMX_CONFIG_INPUTCROPTYPE sensorCropConfig;


    OMX_CONFIG_CAMERAINFOTYPE cameraInfoConfig;
    OMX_PARAM_BRCMFRAMERATERANGETYPE frameRateRangeConfig;
    
    
    
    
    
#if 0
    
    /** 
     * Structure defining percent to scale each frame dimension.  For example:  
     * To make the width 50% larger, use fWidth = 1.5 and to make the width
     * 1/2 the original size, use fWidth = 0.5
     */
    typedef struct OMX_CONFIG_SCALEFACTORTYPE {
        OMX_U32 nSize;            /**< Size of the structure in bytes */
        OMX_VERSIONTYPE nVersion; /**< OMX specification version info */ 
        OMX_U32 nPortIndex;       /**< Port that this struct applies to */
        OMX_S32 xWidth;           /**< Fixed point value stored as Q16 */
        OMX_S32 xHeight;          /**< Fixed point value stored as Q16 */
    }OMX_CONFIG_SCALEFACTORTYPE;
    
    OMX_CONFIG_ROTATIONTYPE OMX_IndexConfigCommonRotate
    OMX_CONFIG_MIRRORTYPE OMX_IndexConfigCommonMirror
    
    OMX_PARAM_U32TYPE OMX_IndexConfigCameraIsoReferenceValue
    OMX_CONFIG_ZEROSHUTTERLAGTYPE OMX_IndexParamCameraZeroShutterLag
    OMX_CONFIG_BRCMFOVTYPE OMX_IndexConfigFieldOfView
    //OMX_CONFIG_CAMERAINFOTYPE OMX_IndexConfigCameraInfo
    
    OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE OMX_IndexConfigFocusControl
    OMX_CONFIG_REDEYEREMOVALTYPE OMX_IndexConfigCommonRedEyeRemoval
    
    OMX_PARAM_CAPTURESTATETYPE OMX_IndexParamCaptureStatus
    OMX_CONFIG_FACEDETECTIONCONTROLTYPE OMX_IndexConfigCommonFaceDetectionControl
    OMX_CONFIG_BOOLEANTYPE OMX_IndexConfigDrawBoxAroundFaces
    OMX_IMAGE_PARAM_QFACTORTYPE OMX_IndexParamQFactor
    OMX_PARAM_BRCMTHUMBNAILTYPE OMX_IndexParamBrcmThumbnail
    OMX_PARAM_TIMESTAMPMODETYPE OMX_IndexParamCommonUseStcTimestamps
    
    OMX_CONFIG_SCALEFACTORTYPE OMX_IndexConfigCommonDigitalZoom
    OMX_CONFIG_FRAMESTABTYPE OMX_IndexConfigCommonFrameStabilisation
    //OMX_CONFIG_INPUTCROPTYPE OMX_IndexConfigInputCropPercentages
    OMX_PARAM_BRCMCONFIGFILETYPE OMX_IndexParamBrcmConfigFileRegisters
    OMX_PARAM_BRCMCONFIGFILECHUNKTYPE OMX_IndexParamBrcmConfigFileChunkRegisters
    
    OMX_PARAM_BRCMFRAMERATERANGETYPE OMX_IndexParamBrcmFpsRange
    OMX_PARAM_S32TYPE OMX_IndexParamCaptureExposureCompensation
    OMX_IndexParamSWSharpenDisable
    OMX_IndexParamSWSaturationDisable
 
#endif
    //OMX_CameraDisableAlgorithmDynamicRangeExpansion
    //OMX_CameraDisableAlgorithmHighDynamicRange
    //https://gist.github.com/jvcleave/83bbef779c0cde9589ab
    //https://github.com/raspberrypi/userland/blob/master/interface/mmal/openmaxil/
    
    
    void initStructures()
    {
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
        
        
        
        OMX_INIT_STRUCTURE(cameraInfoConfig);
        
        OMX_INIT_STRUCTURE(drcConfig);
        
        OMX_INIT_STRUCTURE(hdrConfig);
        
        OMX_INIT_STRUCTURE(burstModeConfig);
        
        OMX_INIT_STRUCTURE(cameraDisableAlgorithmConfig);
        
        OMX_INIT_STRUCTURE(flickerCancelConfig);
        
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
        
        
    };
    
    
	
};