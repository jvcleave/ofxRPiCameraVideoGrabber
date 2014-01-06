/*
 *  OMX_Maps.h
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 11/1/13.
 *  Copyright 2013 jasonvancleave.com. All rights reserved.
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
	
	
	OMX_Maps()
	{
		
		imageFilterTypes["None"] = OMX_ImageFilterNone;
		imageFilterTypes["Noise"] = OMX_ImageFilterNoise;
		imageFilterTypes["Emboss"] = OMX_ImageFilterEmboss;
		imageFilterTypes["Negative"] = OMX_ImageFilterNegative;
		imageFilterTypes["Sketch"] = OMX_ImageFilterSketch;
		imageFilterTypes["OilPaint"] = OMX_ImageFilterOilPaint;
		imageFilterTypes["Hatch"] = OMX_ImageFilterHatch;
		imageFilterTypes["Gpen"] = OMX_ImageFilterGpen;
		imageFilterTypes["Antialias"] = OMX_ImageFilterAntialias;
		imageFilterTypes["DeRing"] = OMX_ImageFilterDeRing;
		imageFilterTypes["Solarize"] = OMX_ImageFilterSolarize;
		imageFilterTypes["Watercolor"] = OMX_ImageFilterWatercolor;
		imageFilterTypes["Pastel"] = OMX_ImageFilterPastel;
		imageFilterTypes["Sharpen"] = OMX_ImageFilterSharpen;
		imageFilterTypes["Film"] = OMX_ImageFilterFilm;
		imageFilterTypes["Blur"] = OMX_ImageFilterBlur;
		imageFilterTypes["Saturation"] = OMX_ImageFilterSaturation;
		imageFilterTypes["DeInterlaceLineDouble"] = OMX_ImageFilterDeInterlaceLineDouble;
		imageFilterTypes["DeInterlaceAdvanced"] = OMX_ImageFilterDeInterlaceAdvanced;
		imageFilterTypes["ColourSwap"] = OMX_ImageFilterColourSwap;
		imageFilterTypes["WashedOut"] = OMX_ImageFilterWashedOut;
		imageFilterTypes["ColourPoint"] = OMX_ImageFilterColourPoint;
		imageFilterTypes["Posterise"] = OMX_ImageFilterPosterise;
		imageFilterTypes["ColourBalance"] = OMX_ImageFilterColourBalance;
		imageFilterTypes["Cartoon"] = OMX_ImageFilterCartoon;
		
		collectNames<OMX_IMAGEFILTERTYPE>(imageFilterTypes, imageFilterNames);
		
		
		whiteBalanceControlTypes["Off"] = OMX_WhiteBalControlOff;
		whiteBalanceControlTypes["Auto"] = OMX_WhiteBalControlAuto;
		whiteBalanceControlTypes["SunLight"] = OMX_WhiteBalControlSunLight;
		whiteBalanceControlTypes["Cloudy"] = OMX_WhiteBalControlCloudy;
		whiteBalanceControlTypes["Shade"] = OMX_WhiteBalControlShade;
		whiteBalanceControlTypes["Tungsten"] = OMX_WhiteBalControlTungsten;
		whiteBalanceControlTypes["Fluorescent"] = OMX_WhiteBalControlFluorescent;
		whiteBalanceControlTypes["Incandescent"] = OMX_WhiteBalControlIncandescent;
		whiteBalanceControlTypes["Flash"] = OMX_WhiteBalControlFlash;
		whiteBalanceControlTypes["Horizon"] = OMX_WhiteBalControlHorizon;
		
		collectNames<OMX_WHITEBALCONTROLTYPE>(whiteBalanceControlTypes, whiteBalanceNames);
		
		meteringTypes["Average"] = OMX_MeteringModeAverage;
		meteringTypes["Spot"] = OMX_MeteringModeSpot;
		meteringTypes["Matrix"] = OMX_MeteringModeMatrix;
		meteringTypes["Backlit"] = OMX_MeteringModeBacklit;
		
		collectNames<OMX_METERINGTYPE>(meteringTypes, meteringNames);
		
		exposureControlTypes["Off"] = OMX_ExposureControlOff;
		exposureControlTypes["Auto"] = OMX_ExposureControlAuto;
		exposureControlTypes["Night"] = OMX_ExposureControlNight;
		exposureControlTypes["BackLight"] = OMX_ExposureControlBackLight;
		exposureControlTypes["SpotLight"] = OMX_ExposureControlSpotLight;
		exposureControlTypes["Sports"] = OMX_ExposureControlSports;
		exposureControlTypes["Snow"] = OMX_ExposureControlSnow;
		exposureControlTypes["Beach"] = OMX_ExposureControlBeach;
		exposureControlTypes["LargeAperture"] = OMX_ExposureControlLargeAperture;
		exposureControlTypes["SmallAperture"] = OMX_ExposureControlSmallAperture;
		exposureControlTypes["VeryLong"] = OMX_ExposureControlVeryLong;
		exposureControlTypes["FixedFps"] = OMX_ExposureControlFixedFps;
		exposureControlTypes["NightWithPreview"] = OMX_ExposureControlNightWithPreview;
		exposureControlTypes["Antishake"] = OMX_ExposureControlAntishake;
		exposureControlTypes["Fireworks"] = OMX_ExposureControlFireworks;
		
		collectNames<OMX_EXPOSURECONTROLTYPE>(exposureControlTypes, exposureControlNames);

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
		
	
		collectNames<OMX_COLOR_FORMATTYPE>(colorFormats, colorFormatNames);

		
		videoCodingTypes["Unused"] = OMX_VIDEO_CodingUnused;
		videoCodingTypes["AutoDetect"] = OMX_VIDEO_CodingAutoDetect;
		videoCodingTypes["MPEG2"] = OMX_VIDEO_CodingMPEG2;
		videoCodingTypes["H263"] = OMX_VIDEO_CodingH263;
		videoCodingTypes["MPEG4"] = OMX_VIDEO_CodingMPEG4;
		videoCodingTypes["WMV"] = OMX_VIDEO_CodingWMV;
		videoCodingTypes["RV"] = OMX_VIDEO_CodingRV;
		videoCodingTypes["AVC"] = OMX_VIDEO_CodingAVC;
		videoCodingTypes["MJPEG"] = OMX_VIDEO_CodingMJPEG;
		videoCodingTypes["VP6"] = OMX_VIDEO_CodingVP6;
		videoCodingTypes["VP7"] = OMX_VIDEO_CodingVP7;
		videoCodingTypes["VP8"] = OMX_VIDEO_CodingVP8;
		videoCodingTypes["YUV"] = OMX_VIDEO_CodingYUV;
		videoCodingTypes["Sorenson"] = OMX_VIDEO_CodingSorenson;
		videoCodingTypes["Theora"] = OMX_VIDEO_CodingTheora;
		videoCodingTypes["MVC"] = OMX_VIDEO_CodingMVC; 	
		

		collectNames<OMX_VIDEO_CODINGTYPE>(videoCodingTypes, videoCodingNames);

		
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
		
		
		collectNames<OMX_CAMERADISABLEALGORITHMTYPE>(algorithms, algorithmNames);
		
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
	
	vector<string> imageFilterNames;
	map<string, OMX_IMAGEFILTERTYPE> imageFilterTypes;
	
	vector<string> whiteBalanceNames;
	map<string, OMX_WHITEBALCONTROLTYPE> whiteBalanceControlTypes;
	
	vector<string> meteringNames;
	map<string, OMX_METERINGTYPE> meteringTypes;
	
	vector<string> exposureControlNames;
	map<string, OMX_EXPOSURECONTROLTYPE> exposureControlTypes;
	
	vector<string> videoCodingNames;
	map<string, OMX_VIDEO_CODINGTYPE> videoCodingTypes;
	
	vector<string> colorFormatNames;
	map<string, OMX_COLOR_FORMATTYPE> colorFormats;
	
	vector<string> algorithmNames;
	map<string, OMX_CAMERADISABLEALGORITHMTYPE> algorithms;
	
	map<OMX_EVENTTYPE, string> eventTypes;
	
	
};
