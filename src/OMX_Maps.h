/*
 *  OMX_Maps.h
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 11/1/13.
 *  Copyright 2013 jasonvancleave.com. :video_camera:
 *
 */
#pragma once

#include "ofMain.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>




class OMX_Maps
{
public:
	static OMX_Maps& getInstance()
	{
		static OMX_Maps    instance;
		return instance;
	}
    
    map<OMX_DYNAMICRANGEEXPANSIONMODETYPE, int> dreTypes;
    
	vector<string> imageFilterNames;
	map<string, OMX_IMAGEFILTERTYPE> imageFilters;
    map<OMX_IMAGEFILTERTYPE, string> imageFilterTypes;
    
    vector<string>& getImageFilterNames()
    {
        return imageFilterNames;
    }
    string getImageFilter(OMX_IMAGEFILTERTYPE type)
    {
        return imageFilterTypes[type];
    }
    
    OMX_IMAGEFILTERTYPE getImageFilter(string name)
    {
        return imageFilters[name];
    }
    
    
	vector<string> whiteBalanceNames;
	map<string, OMX_WHITEBALCONTROLTYPE> whiteBalance;
    map<OMX_WHITEBALCONTROLTYPE, string> whiteBalanceTypes;
    vector<string>& getWhiteBalanceNames()
    {
        return whiteBalanceNames;
    }
    
    string getWhiteBalance(OMX_WHITEBALCONTROLTYPE type)
    {
        return whiteBalanceTypes[type];
    }
    
    OMX_WHITEBALCONTROLTYPE getWhiteBalance(string name)
    {
        return whiteBalance[name];
    }
    
    vector<string> focusNames;
    map<string, OMX_IMAGE_FOCUSCONTROLTYPE> focusControls;
    map<OMX_IMAGE_FOCUSCONTROLTYPE, string> focusControlTypes;
    
    vector<string>& getFocusNames()
    {
        return focusNames;
    }
    
    string getFocus(OMX_IMAGE_FOCUSCONTROLTYPE type)
    {
        return focusControlTypes[type];
    }
    
    OMX_IMAGE_FOCUSCONTROLTYPE getFocus(string name)
    {
        return focusControls[name];
    }

    
    
    
	vector<string> meteringNames;
	map<string, OMX_METERINGTYPE> metering;
    map<OMX_METERINGTYPE, string> meteringTypes;
    vector<string>& getMeteringNames()
    {
        return meteringNames;
    }
    string getMetering(OMX_METERINGTYPE type)
    {
        return meteringTypes[type];
    }
    
    OMX_METERINGTYPE getMetering(string name)
    {
        return metering[name];
    }
    
    
    
	vector<string> exposurePresetNames;
	map<string, OMX_EXPOSURECONTROLTYPE> exposurePresets;
    map<OMX_EXPOSURECONTROLTYPE, string> exposurePresetTypes;
    map<string, OMX_EXPOSURECONTROLTYPE>& getExposurePresets()
    {
        return exposurePresets;
    }
    
    vector<string>& getExposurePresetNames()
    {
        return exposurePresetNames;
    }
    string getExposurePreset(OMX_EXPOSURECONTROLTYPE type)
    {
        return exposurePresetTypes[type];
    }
    
    OMX_EXPOSURECONTROLTYPE getExposurePreset(string name)
    {
        return exposurePresets[name];
    }
    
    vector<string> mirrorNames;
    map<string, OMX_MIRRORTYPE> mirrors;
    map<OMX_MIRRORTYPE, string> mirrorTypes;
   
    string getMirror(OMX_MIRRORTYPE type)
    {
        return mirrorTypes[type];
    }
    
    OMX_MIRRORTYPE getMirror(string name)
    {
        return mirrors[name];
    }
    
	vector<string> videoCodingNames;
	map<string, OMX_VIDEO_CODINGTYPE> videoCoding;
    map<OMX_VIDEO_CODINGTYPE, string> videoCodingTypes;
    vector<string>& getVideoCodingNames()
    {
        return videoCodingNames;
    }
    
    string getVideoCoding(OMX_VIDEO_CODINGTYPE type)
    {
        return videoCodingTypes[type];
    }
    
    OMX_VIDEO_CODINGTYPE getVideoCoding(string name)
    {
        return videoCoding[name];
    }
	
	vector<string> colorFormatNames;
	map<string, OMX_COLOR_FORMATTYPE> colorFormats;
    map<OMX_COLOR_FORMATTYPE, string> colorFormatTypes;
    vector<string>& getColorFormatNames()
    {
        return colorFormatNames;
    }
    string getColorFormat(OMX_COLOR_FORMATTYPE type)
    {
        return colorFormatTypes[type];
    }
    
    OMX_COLOR_FORMATTYPE getColorFormat(string name)
    {
        return colorFormats[name];
    }
	
    
    vector<string> workingColorFormatNames;
    map<string, OMX_COLOR_FORMATTYPE> workingColorFormats;
    map<OMX_COLOR_FORMATTYPE, string> workingColorFormatTypes;
    vector<string>& getWorkingColorFormatNames()
    {
        return workingColorFormatNames;
    }
    string getWorkingColorFormat(OMX_COLOR_FORMATTYPE type)
    {
        return workingColorFormatTypes[type];
    }
    
    OMX_COLOR_FORMATTYPE getWorkingColorFormat(string name)
    {
        return workingColorFormats[name];
    }
    
    
	vector<string> algorithmNames;
	map<string, OMX_CAMERADISABLEALGORITHMTYPE> algorithms;
    map<OMX_CAMERADISABLEALGORITHMTYPE, string> algorithmTypes;
    vector<string>& getAlgorithmNames()
    {
        return algorithmNames;
    }
    
    
    
	map<OMX_EVENTTYPE, string> eventTypes;
	
    
    map<OMX_ERRORTYPE, string> omxErrors;
    
 
    map<OMX_STATETYPE, string>omxStateNames;
    map<EGLint, string> eglErrors;
    
private:	
    OMX_Maps()
    {

        

        focusControls["On"] = OMX_IMAGE_FocusControlOn;
        focusControls["Off"] = OMX_IMAGE_FocusControlOff;
        focusControls["Auto"] = OMX_IMAGE_FocusControlAuto;
        focusControls["AutoLock"] = OMX_IMAGE_FocusControlAutoLock;
        focusControls["Hyperfocal"] = OMX_IMAGE_FocusControlHyperfocal;
        focusControls["AutoMacro"] = OMX_IMAGE_FocusControlAutoMacro;
        focusControls["AutoInfinity"] = OMX_IMAGE_FocusControlAutoInfinity;
        focusControls["AutoLockMacro"] = OMX_IMAGE_FocusControlAutoLockMacro;
        focusControls["AutoLockInfinity"] = OMX_IMAGE_FocusControlAutoLockInfinity;
        focusControls["NearFixed"] = OMX_IMAGE_FocusControlNearFixed;
        focusControls["AutoNear"] = OMX_IMAGE_FocusControlAutoNear;
        focusControls["AutoLockNear"] = OMX_IMAGE_FocusControlAutoLockNear;
        focusControls["InfinityFixed"] = OMX_IMAGE_FocusControlInfinityFixed;
        focusControls["MacroFixed"] = OMX_IMAGE_FocusControlMacroFixed;
        focusControls["AutoFast"] = OMX_IMAGE_FocusControlAutoFast;
        focusControls["AutoMacroFast"] = OMX_IMAGE_FocusControlAutoMacroFast;
        focusControls["AutoNearFast"] = OMX_IMAGE_FocusControlAutoNearFast;
        focusControls["AutoInfinityFast"] = OMX_IMAGE_FocusControlAutoInfinityFast;
        focusControls["CurrentFixed"] = OMX_IMAGE_FocusControlCurrentFixed;
        
        collectNames<OMX_IMAGE_FOCUSCONTROLTYPE>(focusControls, focusNames, focusControlTypes);
        
        dreTypes[OMX_DynRangeExpOff] = 0;
        dreTypes[OMX_DynRangeExpLow] = 1;
        dreTypes[OMX_DynRangeExpMedium] = 2;
        dreTypes[OMX_DynRangeExpHigh] = 3;



        mirrors["None"] = OMX_MirrorNone;
        mirrors["Vertical"] = OMX_MirrorVertical;
        mirrors["Horizontal"] = OMX_MirrorHorizontal;
        mirrors["Both"] = OMX_MirrorBoth;


        imageFilters["None"] = OMX_ImageFilterNone;
        imageFilters["Noise"] = OMX_ImageFilterNoise;
        imageFilters["Emboss"] = OMX_ImageFilterEmboss;
        imageFilters["Negative"] = OMX_ImageFilterNegative;
        imageFilters["Sketch"] = OMX_ImageFilterSketch;
        imageFilters["OilPaint"] = OMX_ImageFilterOilPaint;
        imageFilters["Hatch"] = OMX_ImageFilterHatch;
        imageFilters["Gpen"] = OMX_ImageFilterGpen;
        imageFilters["Antialias"] = OMX_ImageFilterAntialias;
        imageFilters["DeRing"] = OMX_ImageFilterDeRing;
        imageFilters["Solarize"] = OMX_ImageFilterSolarize;
        imageFilters["Watercolor"] = OMX_ImageFilterWatercolor;
        imageFilters["Pastel"] = OMX_ImageFilterPastel;
        imageFilters["Sharpen"] = OMX_ImageFilterSharpen;
        imageFilters["Film"] = OMX_ImageFilterFilm;
        imageFilters["Blur"] = OMX_ImageFilterBlur;
        imageFilters["Saturation"] = OMX_ImageFilterSaturation;
        imageFilters["DeInterlaceLineDouble"] = OMX_ImageFilterDeInterlaceLineDouble;
        imageFilters["DeInterlaceAdvanced"] = OMX_ImageFilterDeInterlaceAdvanced;
        imageFilters["ColourSwap"] = OMX_ImageFilterColourSwap;
        imageFilters["WashedOut"] = OMX_ImageFilterWashedOut;
        imageFilters["ColourPoint"] = OMX_ImageFilterColourPoint;
        imageFilters["Posterise"] = OMX_ImageFilterPosterise;
        imageFilters["ColourBalance"] = OMX_ImageFilterColourBalance;
        imageFilters["Cartoon"] = OMX_ImageFilterCartoon;

        collectNames<OMX_IMAGEFILTERTYPE>(imageFilters, imageFilterNames, imageFilterTypes);


        whiteBalance["Off"] = OMX_WhiteBalControlOff;
        whiteBalance["Auto"] = OMX_WhiteBalControlAuto;
        whiteBalance["SunLight"] = OMX_WhiteBalControlSunLight;
        whiteBalance["Cloudy"] = OMX_WhiteBalControlCloudy;
        whiteBalance["Shade"] = OMX_WhiteBalControlShade;
        whiteBalance["Tungsten"] = OMX_WhiteBalControlTungsten;
        whiteBalance["Fluorescent"] = OMX_WhiteBalControlFluorescent;
        whiteBalance["Incandescent"] = OMX_WhiteBalControlIncandescent;
        whiteBalance["Flash"] = OMX_WhiteBalControlFlash;
        whiteBalance["Horizon"] = OMX_WhiteBalControlHorizon;

        collectNames<OMX_WHITEBALCONTROLTYPE>(whiteBalance, whiteBalanceNames, whiteBalanceTypes);
        
        metering["Average"] = OMX_MeteringModeAverage;
        metering["Spot"] = OMX_MeteringModeSpot;
        metering["Matrix"] = OMX_MeteringModeMatrix;
        metering["Backlit"] = OMX_MeteringModeBacklit;

        collectNames<OMX_METERINGTYPE>(metering, meteringNames, meteringTypes);


        exposurePresets["Off"] = OMX_ExposureControlOff;
        exposurePresets["Auto"] = OMX_ExposureControlAuto;
        exposurePresets["Night"] = OMX_ExposureControlNight;
        exposurePresets["BackLight"] = OMX_ExposureControlBackLight;
        exposurePresets["SpotLight"] = OMX_ExposureControlSpotLight;
        exposurePresets["Sports"] = OMX_ExposureControlSports;
        exposurePresets["Snow"] = OMX_ExposureControlSnow;
        exposurePresets["Beach"] = OMX_ExposureControlBeach;
        exposurePresets["LargeAperture"] = OMX_ExposureControlLargeAperture;
        exposurePresets["SmallAperture"] = OMX_ExposureControlSmallAperture;
        exposurePresets["VeryLong"] = OMX_ExposureControlVeryLong;
        exposurePresets["FixedFps"] = OMX_ExposureControlFixedFps;
        exposurePresets["NightWithPreview"] = OMX_ExposureControlNightWithPreview;
        exposurePresets["Antishake"] = OMX_ExposureControlAntishake;
        exposurePresets["Fireworks"] = OMX_ExposureControlFireworks;

        collectNames<OMX_EXPOSURECONTROLTYPE>(exposurePresets, exposurePresetNames, exposurePresetTypes);



        colorFormats["Unused"] = OMX_COLOR_FormatUnused;
        colorFormats["Monochrome"] = OMX_COLOR_FormatMonochrome;
        colorFormats["8bitRGB332"] = OMX_COLOR_Format8bitRGB332;
        colorFormats["12bitRGB444"] = OMX_COLOR_Format12bitRGB444;
        colorFormats["16bitARGB4444"] = OMX_COLOR_Format16bitARGB4444;
        colorFormats["16bitARGB1555"] = OMX_COLOR_Format16bitARGB1555;
        colorFormats["16bitRGB565"] = OMX_COLOR_Format16bitRGB565;
        colorFormats["16bitRGB565"] = OMX_COLOR_Format16bitBGR565;
        colorFormats["16bitRGB565"] = OMX_COLOR_Format18bitRGB666;
        colorFormats["18bitARGB1665"] = OMX_COLOR_Format18bitARGB1665;
        colorFormats["19bitARGB1666"] = OMX_COLOR_Format19bitARGB1666; 
        colorFormats["24bitRGB888"] = OMX_COLOR_Format24bitRGB888;
        colorFormats["24bitBGR888"] = OMX_COLOR_Format24bitBGR888;
        colorFormats["24bitARGB1887"] = OMX_COLOR_Format24bitARGB1887;
        colorFormats["25bitARGB1888"] = OMX_COLOR_Format25bitARGB1888;
        colorFormats["32bitBGRA8888"] = OMX_COLOR_Format32bitBGRA8888;
        colorFormats["32bitARGB8888"] = OMX_COLOR_Format32bitARGB8888;
        colorFormats["YUV411Planar"] = OMX_COLOR_FormatYUV411Planar;
        colorFormats["YUV411PackedPlanar"] = OMX_COLOR_FormatYUV411PackedPlanar;
        colorFormats["YUV420Planar"] = OMX_COLOR_FormatYUV420Planar;
        colorFormats["YUV420PackedPlanar"] = OMX_COLOR_FormatYUV420PackedPlanar;
        colorFormats["YUV420SemiPlanar"] = OMX_COLOR_FormatYUV420SemiPlanar;
        colorFormats["YUV422Planar"] = OMX_COLOR_FormatYUV422Planar;
        colorFormats["YUV422PackedPlanar"] = OMX_COLOR_FormatYUV422PackedPlanar;
        colorFormats["YUV422SemiPlanar"] = OMX_COLOR_FormatYUV422SemiPlanar;
        colorFormats["YCbYCr"] = OMX_COLOR_FormatYCbYCr;
        colorFormats["YCrYCb"] = OMX_COLOR_FormatYCrYCb;
        colorFormats["CbYCrY"] = OMX_COLOR_FormatCbYCrY;
        colorFormats["CrYCbY"] = OMX_COLOR_FormatCrYCbY;
        colorFormats["YUV444Interleaved"] = OMX_COLOR_FormatYUV444Interleaved;
        colorFormats["RawBayer8bit"] = OMX_COLOR_FormatRawBayer8bit;
        colorFormats["RawBayer10bit"] = OMX_COLOR_FormatRawBayer10bit;
        colorFormats["RawBayer8bitcompressed"] = OMX_COLOR_FormatRawBayer8bitcompressed;
        colorFormats["L2"] = OMX_COLOR_FormatL2; 
        colorFormats["L4"] = OMX_COLOR_FormatL4; 
        colorFormats["L8"] = OMX_COLOR_FormatL8; 
        colorFormats["L16"] = OMX_COLOR_FormatL16; 
        colorFormats["L24"] = OMX_COLOR_FormatL24; 
        colorFormats["L32"] = OMX_COLOR_FormatL32;
        colorFormats["YUV420PackedSemiPlanar"] = OMX_COLOR_FormatYUV420PackedSemiPlanar;
        colorFormats["YUV422PackedSemiPlanar"] = OMX_COLOR_FormatYUV422PackedSemiPlanar;
        colorFormats["18BitBGR666"] = OMX_COLOR_Format18BitBGR666;
        colorFormats["24BitARGB6666"] = OMX_COLOR_Format24BitARGB6666;
        colorFormats["24BitABGR6666"] = OMX_COLOR_Format24BitABGR6666;
        colorFormats["32bitABGR8888"] = OMX_COLOR_Format32bitABGR8888;
        colorFormats["8bitPalette"] = OMX_COLOR_Format8bitPalette;
        colorFormats["YUVUV128"] = OMX_COLOR_FormatYUVUV128;
        colorFormats["RawBayer12bit"] = OMX_COLOR_FormatRawBayer12bit;
        colorFormats["BRCMEGL"] = OMX_COLOR_FormatBRCMEGL;
        colorFormats["BRCMOpaque"] = OMX_COLOR_FormatBRCMOpaque;
        colorFormats["YVU420PackedPlanar"] = OMX_COLOR_FormatYVU420PackedPlanar;
        colorFormats["YVU420PackedSemiPlanar"] = OMX_COLOR_FormatYVU420PackedSemiPlanar;


        collectNames<OMX_COLOR_FORMATTYPE>(colorFormats, colorFormatNames, colorFormatTypes);
                 
        workingColorFormats["Unused"] = OMX_COLOR_FormatUnused;
        workingColorFormats["YUV420PackedPlanar"] = OMX_COLOR_FormatYUV420PackedPlanar;
        workingColorFormats["YUV420PackedSemiPlanar"] = OMX_COLOR_FormatYUV420PackedSemiPlanar;
        workingColorFormats["YUV422PackedPlanar"] = OMX_COLOR_FormatYUV422PackedPlanar;
        workingColorFormats["YVU420PackedPlanar"] = OMX_COLOR_FormatYVU420PackedPlanar;
        workingColorFormats["YVU420PackedSemiPlanar"] = OMX_COLOR_FormatYVU420PackedSemiPlanar;

        collectNames<OMX_COLOR_FORMATTYPE>(workingColorFormats, workingColorFormatNames, workingColorFormatTypes);
        

        videoCoding["Unused"] = OMX_VIDEO_CodingUnused;
        videoCoding["AutoDetect"] = OMX_VIDEO_CodingAutoDetect;
        videoCoding["MPEG2"] = OMX_VIDEO_CodingMPEG2;
        videoCoding["H263"] = OMX_VIDEO_CodingH263;
        videoCoding["MPEG4"] = OMX_VIDEO_CodingMPEG4;
        videoCoding["WMV"] = OMX_VIDEO_CodingWMV;
        videoCoding["RV"] = OMX_VIDEO_CodingRV;
        videoCoding["AVC"] = OMX_VIDEO_CodingAVC;
        videoCoding["MJPEG"] = OMX_VIDEO_CodingMJPEG;
        videoCoding["VP6"] = OMX_VIDEO_CodingVP6;
        videoCoding["VP7"] = OMX_VIDEO_CodingVP7;
        videoCoding["VP8"] = OMX_VIDEO_CodingVP8;
        videoCoding["YUV"] = OMX_VIDEO_CodingYUV;
        videoCoding["Sorenson"] = OMX_VIDEO_CodingSorenson;
        videoCoding["Theora"] = OMX_VIDEO_CodingTheora;
        videoCoding["MVC"] = OMX_VIDEO_CodingMVC; 	


        collectNames<OMX_VIDEO_CODINGTYPE>(videoCoding, videoCodingNames, videoCodingTypes);


        algorithms["Facetracking"] = OMX_CameraDisableAlgorithmFacetracking;
        algorithms["RedEyeReduction"] = OMX_CameraDisableAlgorithmRedEyeReduction;
        algorithms["VideoStabilisation"] = OMX_CameraDisableAlgorithmVideoStabilisation;
        algorithms["WriteRaw"] = OMX_CameraDisableAlgorithmWriteRaw;
        algorithms["VideoDenoise"] = OMX_CameraDisableAlgorithmVideoDenoise;
        algorithms["StillsDenoise"] = OMX_CameraDisableAlgorithmStillsDenoise;
        algorithms["AntiShake"] = OMX_CameraDisableAlgorithmAntiShake;
        algorithms["ImageEffects"] = OMX_CameraDisableAlgorithmImageEffects;
        algorithms["DarkSubtract"] = OMX_CameraDisableAlgorithmDarkSubtract;
        algorithms["DynamicRangeExpansion"] = OMX_CameraDisableAlgorithmDynamicRangeExpansion;
        algorithms["FaceRecognition"] = OMX_CameraDisableAlgorithmFaceRecognition;
        algorithms["FaceBeautification"] = OMX_CameraDisableAlgorithmFaceBeautification;
        algorithms["SceneDetection"] = OMX_CameraDisableAlgorithmSceneDetection;
        algorithms["HighDynamicRange"] = OMX_CameraDisableAlgorithmHighDynamicRange;


        collectNames<OMX_CAMERADISABLEALGORITHMTYPE>(algorithms, algorithmNames, algorithmTypes);


        omxStateNames[OMX_StateInvalid] = "OMX_StateInvalid";
        omxStateNames[OMX_StateLoaded] = "OMX_StateLoaded";
        omxStateNames[OMX_StateIdle] = "OMX_StateIdle";
        omxStateNames[OMX_StateExecuting] = "OMX_StateExecuting";
        omxStateNames[OMX_StatePause] = "OMX_StatePause";

        omxErrors[OMX_ErrorNone] =  "OMX_ErrorNone";
        omxErrors[OMX_ErrorInsufficientResources] =  "OMX_ErrorInsufficientResources";
        omxErrors[OMX_ErrorUndefined] =  "OMX_ErrorUndefined";
        omxErrors[OMX_ErrorInvalidComponentName] =  "OMX_ErrorInvalidComponentName";
        omxErrors[OMX_ErrorComponentNotFound] =  "OMX_ErrorComponentNotFound";
        omxErrors[OMX_ErrorInvalidComponent] =  "OMX_ErrorInvalidComponent";
        omxErrors[OMX_ErrorBadParameter] =  "OMX_ErrorBadParameter";
        omxErrors[OMX_ErrorNotImplemented] =  "OMX_ErrorNotImplemented";
        omxErrors[OMX_ErrorUnderflow] =  "OMX_ErrorUnderflow";
        omxErrors[OMX_ErrorOverflow] =  "OMX_ErrorOverflow";
        omxErrors[OMX_ErrorHardware] =  "OMX_ErrorHardware";
        omxErrors[OMX_ErrorInvalidState] =  "OMX_ErrorInvalidState";
        omxErrors[OMX_ErrorStreamCorrupt] =  "OMX_ErrorStreamCorrupt";
        omxErrors[OMX_ErrorPortsNotCompatible] =  "OMX_ErrorPortsNotCompatible";
        omxErrors[OMX_ErrorResourcesLost] =  "OMX_ErrorResourcesLost";
        omxErrors[OMX_ErrorNoMore] =  "OMX_ErrorNoMore";
        omxErrors[OMX_ErrorVersionMismatch] =  "OMX_ErrorVersionMismatch";
        omxErrors[OMX_ErrorNotReady] =  "OMX_ErrorNotReady";
        omxErrors[OMX_ErrorTimeout] =  "OMX_ErrorTimeout";
        omxErrors[OMX_ErrorSameState] =  "OMX_ErrorSameState";
        omxErrors[OMX_ErrorResourcesPreempted] =  "OMX_ErrorResourcesPreempted";
        omxErrors[OMX_ErrorPortUnresponsiveDuringAllocation] =  "OMX_ErrorPortUnresponsiveDuringAllocation";
        omxErrors[OMX_ErrorPortUnresponsiveDuringDeallocation] =  "OMX_ErrorPortUnresponsiveDuringDeallocation";
        omxErrors[OMX_ErrorPortUnresponsiveDuringStop] =  "OMX_ErrorPortUnresponsiveDuringStop";
        omxErrors[OMX_ErrorIncorrectStateTransition] =  "OMX_ErrorIncorrectStateTransition";
        omxErrors[OMX_ErrorIncorrectStateOperation] =  "OMX_ErrorIncorrectStateOperation";
        omxErrors[OMX_ErrorUnsupportedSetting] =  "OMX_ErrorUnsupportedSetting";
        omxErrors[OMX_ErrorUnsupportedIndex] =  "OMX_ErrorUnsupportedIndex";
        omxErrors[OMX_ErrorBadPortIndex] =  "OMX_ErrorBadPortIndex";
        omxErrors[OMX_ErrorPortUnpopulated] =  "OMX_ErrorPortUnpopulated";
        omxErrors[OMX_ErrorComponentSuspended] =  "OMX_ErrorComponentSuspended";
        omxErrors[OMX_ErrorDynamicResourcesUnavailable] =  "OMX_ErrorDynamicResourcesUnavailable";
        omxErrors[OMX_ErrorMbErrorsInFrame] =  "OMX_ErrorMbErrorsInFrame";
        omxErrors[OMX_ErrorFormatNotDetected] =  "OMX_ErrorFormatNotDetected";
        omxErrors[OMX_ErrorContentPipeOpenFailed] =  "OMX_ErrorContentPipeOpenFailed";
        omxErrors[OMX_ErrorContentPipeCreationFailed] =  "OMX_ErrorContentPipeCreationFailed";
        omxErrors[OMX_ErrorSeperateTablesUsed] =  "OMX_ErrorSeperateTablesUsed";
        omxErrors[OMX_ErrorTunnelingUnsupported] =  "OMX_ErrorTunnelingUnsupported";
        omxErrors[OMX_ErrorKhronosExtensions] =  "OMX_ErrorKhronosExtensions";
        omxErrors[OMX_ErrorVendorStartUnused] =  "OMX_ErrorVendorStartUnused";
        omxErrors[OMX_ErrorDiskFull] =  "OMX_ErrorDiskFull";
        omxErrors[OMX_ErrorMaxFileSize] =  "OMX_ErrorMaxFileSize";
        omxErrors[OMX_ErrorDrmUnauthorised] =  "OMX_ErrorDrmUnauthorised";
        omxErrors[OMX_ErrorDrmExpired] =  "OMX_ErrorDrmExpired";
        omxErrors[OMX_ErrorDrmGeneral] =  "OMX_ErrorDrmGeneral";

        eventTypes[OMX_EventError] = "OMX_EventError";
        eventTypes[OMX_EventCmdComplete] = "OMX_EventCmdComplete";
        eventTypes[OMX_EventMark] = "OMX_EventMark";
        eventTypes[OMX_EventPortSettingsChanged] = "OMX_EventPortSettingsChanged";
        eventTypes[OMX_EventBufferFlag] = "OMX_EventBufferFlag";
        eventTypes[OMX_EventResourcesAcquired] = "OMX_EventResourcesAcquired";
        eventTypes[OMX_EventComponentResumed] = "OMX_EventComponentResumed";
        eventTypes[OMX_EventDynamicResourcesAvailable] = "OMX_EventDynamicResourcesAvailable";
        eventTypes[OMX_EventKhronosExtensions] = "OMX_EventKhronosExtensions";
        eventTypes[OMX_EventVendorStartUnused] = "OMX_EventVendorStartUnused";
        eventTypes[OMX_EventParamOrConfigChanged] = "OMX_EventParamOrConfigChanged";
        
        
        eglErrors[EGL_SUCCESS]="EGL_SUCCESS";
        eglErrors[EGL_NOT_INITIALIZED]="EGL_NOT_INITIALIZED";
        eglErrors[EGL_BAD_ACCESS]="EGL_BAD_ACCESS";
        eglErrors[EGL_BAD_ALLOC]="EGL_BAD_ALLOC";
        eglErrors[EGL_BAD_ATTRIBUTE]="EGL_BAD_ATTRIBUTE";
        eglErrors[EGL_BAD_CONFIG]="EGL_BAD_CONFIG";
        eglErrors[EGL_BAD_CONTEXT]="EGL_BAD_CONTEXT";
        eglErrors[EGL_BAD_CURRENT_SURFACE]="EGL_BAD_CURRENT_SURFACE";
        eglErrors[EGL_BAD_DISPLAY]="EGL_BAD_DISPLAY";
        eglErrors[EGL_BAD_MATCH]="EGL_BAD_MATCH";
        eglErrors[EGL_BAD_NATIVE_PIXMAP]="EGL_BAD_NATIVE_PIXMAP";
        eglErrors[EGL_BAD_NATIVE_WINDOW]="EGL_BAD_NATIVE_WINDOW";
        eglErrors[EGL_BAD_PARAMETER]="EGL_BAD_PARAMETER";
        eglErrors[EGL_BAD_SURFACE]="EGL_BAD_SURFACE";
        eglErrors[EGL_CONTEXT_LOST]="EGL_CONTEXT_LOST";
        
        //~3MS

	}
	~OMX_Maps(){};
	OMX_Maps(OMX_Maps const&);
	void operator=(OMX_Maps const&);
	
    
	template<typename OMXEnum>
	void collectNames(map<string, OMXEnum>& sourceMap, vector<string>& names, map<OMXEnum, string>& reverseMap)
	{
		typename map<string, OMXEnum>::iterator mapIterator = sourceMap.begin();
		while (mapIterator != sourceMap.end()) 
		{
			names.push_back((*mapIterator).first);
            reverseMap[(*mapIterator).second] = (*mapIterator).first;
			++mapIterator;
		}
	}
    
    template<typename OMXEnum>
    void collectNames(map<string, OMXEnum>& sourceMap, vector<string>& names)
    {
        typename map<string, OMXEnum>::iterator mapIterator = sourceMap.begin();
        while (mapIterator != sourceMap.end()) 
        {
            names.push_back((*mapIterator).first);
            ++mapIterator;
        }
    }
    
	
	
};
