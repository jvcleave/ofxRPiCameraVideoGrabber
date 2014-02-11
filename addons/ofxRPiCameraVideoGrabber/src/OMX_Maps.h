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
	static OMX_Maps& getInstance()
	{
		static OMX_Maps    instance;
		return instance;
	}
	vector<string> imageFilterNames;
	map<string, OMX_IMAGEFILTERTYPE> imageFilters;
	
	vector<string> whiteBalanceNames;
	map<string, OMX_WHITEBALCONTROLTYPE> whiteBalanceControls;
	
	vector<string> meteringNames;
	map<string, OMX_METERINGTYPE> metering;
	
	vector<string> exposureControlNames;
	map<string, OMX_EXPOSURECONTROLTYPE> exposureControls;
	
	vector<string> videoCodingNames;
	map<string, OMX_VIDEO_CODINGTYPE> videoCoding;
	
	vector<string> colorFormatNames;
	map<string, OMX_COLOR_FORMATTYPE> colorFormats;
	
	
	map<OMX_COLOR_FORMATTYPE, string> colorFormatTypes;
	
	
	vector<string> algorithmNames;
	map<string, OMX_CAMERADISABLEALGORITHMTYPE> algorithms;
	
	map<OMX_EVENTTYPE, string> eventTypes;
	map<OMX_VIDEO_CODINGTYPE, string> videoCodingTypes;
private:	
	OMX_Maps()
	{
		
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
		
		collectNames<OMX_IMAGEFILTERTYPE>(imageFilters, imageFilterNames);
		
		
		whiteBalanceControls["Off"] = OMX_WhiteBalControlOff;
		whiteBalanceControls["Auto"] = OMX_WhiteBalControlAuto;
		whiteBalanceControls["SunLight"] = OMX_WhiteBalControlSunLight;
		whiteBalanceControls["Cloudy"] = OMX_WhiteBalControlCloudy;
		whiteBalanceControls["Shade"] = OMX_WhiteBalControlShade;
		whiteBalanceControls["Tungsten"] = OMX_WhiteBalControlTungsten;
		whiteBalanceControls["Fluorescent"] = OMX_WhiteBalControlFluorescent;
		whiteBalanceControls["Incandescent"] = OMX_WhiteBalControlIncandescent;
		whiteBalanceControls["Flash"] = OMX_WhiteBalControlFlash;
		whiteBalanceControls["Horizon"] = OMX_WhiteBalControlHorizon;
		
		collectNames<OMX_WHITEBALCONTROLTYPE>(whiteBalanceControls, whiteBalanceNames);
		
		metering["Average"] = OMX_MeteringModeAverage;
		metering["Spot"] = OMX_MeteringModeSpot;
		metering["Matrix"] = OMX_MeteringModeMatrix;
		metering["Backlit"] = OMX_MeteringModeBacklit;
		
		collectNames<OMX_METERINGTYPE>(metering, meteringNames);
		
		exposureControls["Off"] = OMX_ExposureControlOff;
		exposureControls["Auto"] = OMX_ExposureControlAuto;
		exposureControls["Night"] = OMX_ExposureControlNight;
		exposureControls["BackLight"] = OMX_ExposureControlBackLight;
		exposureControls["SpotLight"] = OMX_ExposureControlSpotLight;
		exposureControls["Sports"] = OMX_ExposureControlSports;
		exposureControls["Snow"] = OMX_ExposureControlSnow;
		exposureControls["Beach"] = OMX_ExposureControlBeach;
		exposureControls["LargeAperture"] = OMX_ExposureControlLargeAperture;
		exposureControls["SmallAperture"] = OMX_ExposureControlSmallAperture;
		exposureControls["VeryLong"] = OMX_ExposureControlVeryLong;
		exposureControls["FixedFps"] = OMX_ExposureControlFixedFps;
		exposureControls["NightWithPreview"] = OMX_ExposureControlNightWithPreview;
		exposureControls["Antishake"] = OMX_ExposureControlAntishake;
		exposureControls["Fireworks"] = OMX_ExposureControlFireworks;
		
		collectNames<OMX_EXPOSURECONTROLTYPE>(exposureControls, exposureControlNames);

		colorFormatTypes[OMX_COLOR_FormatUnused] = "OMX_COLOR_FormatUnused";
		colorFormatTypes[OMX_COLOR_FormatMonochrome] = "OMX_COLOR_FormatMonochrome";
		colorFormatTypes[OMX_COLOR_Format8bitRGB332] = "OMX_COLOR_Format8bitRGB332";
		colorFormatTypes[OMX_COLOR_Format12bitRGB444] = "OMX_COLOR_Format12bitRGB444";
		colorFormatTypes[OMX_COLOR_Format16bitARGB4444] = "OMX_COLOR_Format16bitARGB4444";
		colorFormatTypes[OMX_COLOR_Format16bitARGB1555] = "OMX_COLOR_Format16bitARGB1555";
		colorFormatTypes[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format16bitRGB565";
		colorFormatTypes[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format16bitBGR565";
		colorFormatTypes[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format18bitRGB666";
		colorFormatTypes[OMX_COLOR_Format18bitARGB1665] = "OMX_COLOR_Format18bitARGB1665";
		colorFormatTypes[OMX_COLOR_Format19bitARGB1666] = "OMX_COLOR_Format19bitARGB1666"; 
		colorFormatTypes[OMX_COLOR_Format24bitRGB888] = "OMX_COLOR_Format24bitRGB888";
		colorFormatTypes[OMX_COLOR_Format24bitBGR888] = "OMX_COLOR_Format24bitBGR888";
		colorFormatTypes[OMX_COLOR_Format24bitARGB1887] = "OMX_COLOR_Format24bitARGB1887";
		colorFormatTypes[OMX_COLOR_Format25bitARGB1888] = "OMX_COLOR_Format25bitARGB1888";
		colorFormatTypes[OMX_COLOR_Format32bitBGRA8888] = "OMX_COLOR_Format32bitBGRA8888";
		colorFormatTypes[OMX_COLOR_Format32bitARGB8888] = "OMX_COLOR_Format32bitARGB8888";
		colorFormatTypes[OMX_COLOR_FormatYUV411Planar] = "OMX_COLOR_FormatYUV411Planar";
		colorFormatTypes[OMX_COLOR_FormatYUV411PackedPlanar] = "OMX_COLOR_FormatYUV411PackedPlanar";
		colorFormatTypes[OMX_COLOR_FormatYUV420Planar] = "OMX_COLOR_FormatYUV420Planar";
		colorFormatTypes[OMX_COLOR_FormatYUV420PackedPlanar] = "OMX_COLOR_FormatYUV420PackedPlanar";
		colorFormatTypes[OMX_COLOR_FormatYUV420SemiPlanar] = "OMX_COLOR_FormatYUV420SemiPlanar";
		colorFormatTypes[OMX_COLOR_FormatYUV422Planar] = "OMX_COLOR_FormatYUV422Planar";
		colorFormatTypes[OMX_COLOR_FormatYUV422PackedPlanar] = "OMX_COLOR_FormatYUV422PackedPlanar";
		colorFormatTypes[OMX_COLOR_FormatYUV422SemiPlanar] = "OMX_COLOR_FormatYUV422SemiPlanar";
		colorFormatTypes[OMX_COLOR_FormatYCbYCr] = "OMX_COLOR_FormatYCbYCr";
		colorFormatTypes[OMX_COLOR_FormatYCrYCb] = "OMX_COLOR_FormatYCrYCb";
		colorFormatTypes[OMX_COLOR_FormatCbYCrY] = "OMX_COLOR_FormatCbYCrY";
		colorFormatTypes[OMX_COLOR_FormatCrYCbY] = "OMX_COLOR_FormatCrYCbY";
		colorFormatTypes[OMX_COLOR_FormatYUV444Interleaved] = "OMX_COLOR_FormatYUV444Interleaved";
		colorFormatTypes[OMX_COLOR_FormatRawBayer8bit] = "OMX_COLOR_FormatRawBayer8bit";
		colorFormatTypes[OMX_COLOR_FormatRawBayer10bit] = "OMX_COLOR_FormatRawBayer10bit";
		colorFormatTypes[OMX_COLOR_FormatRawBayer8bitcompressed] = "OMX_COLOR_FormatRawBayer8bitcompressed";
		colorFormatTypes[OMX_COLOR_FormatL2] = "OMX_COLOR_FormatL2"; 
		colorFormatTypes[OMX_COLOR_FormatL4] = "OMX_COLOR_FormatL4"; 
		colorFormatTypes[OMX_COLOR_FormatL8] = "OMX_COLOR_FormatL8"; 
		colorFormatTypes[OMX_COLOR_FormatL16] = "OMX_COLOR_FormatL16"; 
		colorFormatTypes[OMX_COLOR_FormatL24] = "OMX_COLOR_FormatL24"; 
		colorFormatTypes[OMX_COLOR_FormatL32] = "OMX_COLOR_FormatL32";
		colorFormatTypes[OMX_COLOR_FormatYUV420PackedSemiPlanar] = "OMX_COLOR_FormatYUV420PackedSemiPlanar";
		colorFormatTypes[OMX_COLOR_FormatYUV422PackedSemiPlanar] = "OMX_COLOR_FormatYUV422PackedSemiPlanar";
		colorFormatTypes[OMX_COLOR_Format18BitBGR666] = "OMX_COLOR_Format18BitBGR666";
		colorFormatTypes[OMX_COLOR_Format24BitARGB6666] = "OMX_COLOR_Format24BitARGB6666";
		colorFormatTypes[OMX_COLOR_Format24BitABGR6666] = "OMX_COLOR_Format24BitABGR6666";
		colorFormatTypes[OMX_COLOR_Format32bitABGR8888] = "OMX_COLOR_Format32bitABGR8888";
		colorFormatTypes[OMX_COLOR_Format8bitPalette] = "OMX_COLOR_Format8bitPalette";
		colorFormatTypes[OMX_COLOR_FormatYUVUV128] = "OMX_COLOR_FormatYUVUV128";
		colorFormatTypes[OMX_COLOR_FormatRawBayer12bit] = "OMX_COLOR_FormatRawBayer12bit";
		colorFormatTypes[OMX_COLOR_FormatBRCMEGL] = "OMX_COLOR_FormatBRCMEGL";
		colorFormatTypes[OMX_COLOR_FormatBRCMOpaque] = "OMX_COLOR_FormatBRCMOpaque";
		colorFormatTypes[OMX_COLOR_FormatYVU420PackedPlanar] = "OMX_COLOR_FormatYVU420PackedPlanar";
		colorFormatTypes[OMX_COLOR_FormatYVU420PackedSemiPlanar] = "OMX_COLOR_FormatYVU420PackedSemiPlanar";
		
		
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
		
		
		videoCodingTypes[OMX_VIDEO_CodingUnused] = "OMX_VIDEO_CodingUnused";
		videoCodingTypes[OMX_VIDEO_CodingAutoDetect] = "OMX_VIDEO_CodingAutoDetect";
		videoCodingTypes[OMX_VIDEO_CodingMPEG2] = "OMX_VIDEO_CodingMPEG2";
		videoCodingTypes[OMX_VIDEO_CodingH263] = "OMX_VIDEO_CodingH263";
		videoCodingTypes[OMX_VIDEO_CodingMPEG4] = "OMX_VIDEO_CodingMPEG4";
		videoCodingTypes[OMX_VIDEO_CodingWMV] = "OMX_VIDEO_CodingWMV";
		videoCodingTypes[OMX_VIDEO_CodingRV] = "OMX_VIDEO_CodingRV";
		videoCodingTypes[OMX_VIDEO_CodingAVC] = "OMX_VIDEO_CodingAVC";
		videoCodingTypes[OMX_VIDEO_CodingMJPEG] = "OMX_VIDEO_CodingMJPEG";
		videoCodingTypes[OMX_VIDEO_CodingVP6] = "OMX_VIDEO_CodingVP6";
		videoCodingTypes[OMX_VIDEO_CodingVP7] = "OMX_VIDEO_CodingVP7";
		videoCodingTypes[OMX_VIDEO_CodingVP8] = "OMX_VIDEO_CodingVP8";
		videoCodingTypes[OMX_VIDEO_CodingYUV] = "OMX_VIDEO_CodingYUV";
		videoCodingTypes[OMX_VIDEO_CodingSorenson] = "OMX_VIDEO_CodingSorenson";
		videoCodingTypes[OMX_VIDEO_CodingTheora] = "OMX_VIDEO_CodingTheora";
		videoCodingTypes[OMX_VIDEO_CodingMVC] = "OMX_VIDEO_CodingMVC"; 	
		
		
		
		
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
		

		collectNames<OMX_VIDEO_CODINGTYPE>(videoCoding, videoCodingNames);

		
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
	~OMX_Maps(){};
	OMX_Maps(OMX_Maps const&);
	void operator=(OMX_Maps const&);
	
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
