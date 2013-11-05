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
		
		
		
		meteringTypes[OMX_MeteringModeAverage] = "OMX_MeteringModeAverage";
		meteringTypes[OMX_MeteringModeSpot] = "OMX_MeteringModeSpot";
		meteringTypes[OMX_MeteringModeMatrix] = "OMX_MeteringModeMatrix";
		meteringTypes[OMX_MeteringKhronosExtensions] = "OMX_MeteringKhronosExtensions";
		meteringTypes[OMX_MeteringVendorStartUnused] = "OMX_MeteringVendorStartUnused";
		meteringTypes[OMX_MeteringModeBacklit] = "OMX_MeteringModeBacklit";
		
		map<OMX_METERINGTYPE, string>::iterator meteringIterator = meteringTypes.begin();
		while (meteringIterator != meteringTypes.end()) 
		{
			meteringNames.push_back((*meteringIterator).second);
			++meteringIterator;
		}
		
		exposureControlTypes[OMX_ExposureControlOff] = "OMX_ExposureControlOff";
		exposureControlTypes[OMX_ExposureControlAuto] = "OMX_ExposureControlAuto";
		exposureControlTypes[OMX_ExposureControlNight] = "OMX_ExposureControlNight";
		exposureControlTypes[OMX_ExposureControlBackLight] = "OMX_ExposureControlBackLight";
		exposureControlTypes[OMX_ExposureControlSpotLight] = "OMX_ExposureControlSpotLight";
		exposureControlTypes[OMX_ExposureControlSports] = "OMX_ExposureControlSports";
		exposureControlTypes[OMX_ExposureControlSnow] = "OMX_ExposureControlSnow";
		exposureControlTypes[OMX_ExposureControlBeach] = "OMX_ExposureControlBeach";
		exposureControlTypes[OMX_ExposureControlLargeAperture] = "OMX_ExposureControlLargeAperture";
		exposureControlTypes[OMX_ExposureControlSmallAperture] = "OMX_ExposureControlSmallAperture";
		exposureControlTypes[OMX_ExposureControlKhronosExtensions] = "OMX_ExposureControlKhronosExtensions";
		exposureControlTypes[OMX_ExposureControlVendorStartUnused] = "OMX_ExposureControlVendorStartUnused";
		exposureControlTypes[OMX_ExposureControlVeryLong] = "OMX_ExposureControlVeryLong";
		exposureControlTypes[OMX_ExposureControlFixedFps] = "OMX_ExposureControlFixedFps";
		exposureControlTypes[OMX_ExposureControlNightWithPreview] = "OMX_ExposureControlNightWithPreview";
		exposureControlTypes[OMX_ExposureControlAntishake] = "OMX_ExposureControlAntishake";
		exposureControlTypes[OMX_ExposureControlFireworks] = "OMX_ExposureControlFireworks";
		exposureControlTypes[OMX_ExposureControlMax] = "OMX_ExposureControlMax";
		
		map<OMX_EXPOSURECONTROLTYPE, string>::iterator exposureIterator = exposureControlTypes.begin();
		while (exposureIterator != exposureControlTypes.end()) 
		{
			exposureControlNames.push_back((*exposureIterator).second);
			++exposureIterator;
		}
		
		
		colorFormats[OMX_COLOR_FormatUnused] = "OMX_COLOR_FormatUnused";
		colorFormats[OMX_COLOR_FormatMonochrome] = "OMX_COLOR_FormatMonochrome";
		colorFormats[OMX_COLOR_Format8bitRGB332] = "OMX_COLOR_Format8bitRGB332";
		colorFormats[OMX_COLOR_Format12bitRGB444] = "OMX_COLOR_Format12bitRGB444";
		colorFormats[OMX_COLOR_Format16bitARGB4444] = "OMX_COLOR_Format16bitARGB4444";
		colorFormats[OMX_COLOR_Format16bitARGB1555] = "OMX_COLOR_Format16bitARGB1555";
		colorFormats[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format16bitRGB565";
		colorFormats[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format16bitBGR565";
		colorFormats[OMX_COLOR_Format16bitRGB565] = "OMX_COLOR_Format18bitRGB666";
		colorFormats[OMX_COLOR_Format18bitARGB1665] = "OMX_COLOR_Format18bitARGB1665";
		colorFormats[OMX_COLOR_Format19bitARGB1666] = "OMX_COLOR_Format19bitARGB1666"; 
		colorFormats[OMX_COLOR_Format24bitRGB888] = "OMX_COLOR_Format24bitRGB888";
		colorFormats[OMX_COLOR_Format24bitBGR888] = "OMX_COLOR_Format24bitBGR888";
		colorFormats[OMX_COLOR_Format24bitARGB1887] = "OMX_COLOR_Format24bitARGB1887";
		colorFormats[OMX_COLOR_Format25bitARGB1888] = "OMX_COLOR_Format25bitARGB1888";
		colorFormats[OMX_COLOR_Format32bitBGRA8888] = "OMX_COLOR_Format32bitBGRA8888";
		colorFormats[OMX_COLOR_Format32bitARGB8888] = "OMX_COLOR_Format32bitARGB8888";
		colorFormats[OMX_COLOR_FormatYUV411Planar] = "OMX_COLOR_FormatYUV411Planar";
		colorFormats[OMX_COLOR_FormatYUV411PackedPlanar] = "OMX_COLOR_FormatYUV411PackedPlanar";
		colorFormats[OMX_COLOR_FormatYUV420Planar] = "OMX_COLOR_FormatYUV420Planar";
		colorFormats[OMX_COLOR_FormatYUV420PackedPlanar] = "OMX_COLOR_FormatYUV420PackedPlanar";
		colorFormats[OMX_COLOR_FormatYUV420SemiPlanar] = "OMX_COLOR_FormatYUV420SemiPlanar";
		colorFormats[OMX_COLOR_FormatYUV422Planar] = "OMX_COLOR_FormatYUV422Planar";
		colorFormats[OMX_COLOR_FormatYUV422PackedPlanar] = "OMX_COLOR_FormatYUV422PackedPlanar";
		colorFormats[OMX_COLOR_FormatYUV422SemiPlanar] = "OMX_COLOR_FormatYUV422SemiPlanar";
		colorFormats[OMX_COLOR_FormatYCbYCr] = "OMX_COLOR_FormatYCbYCr";
		colorFormats[OMX_COLOR_FormatYCrYCb] = "OMX_COLOR_FormatYCrYCb";
		colorFormats[OMX_COLOR_FormatCbYCrY] = "OMX_COLOR_FormatCbYCrY";
		colorFormats[OMX_COLOR_FormatCrYCbY] = "OMX_COLOR_FormatCrYCbY";
		colorFormats[OMX_COLOR_FormatYUV444Interleaved] = "OMX_COLOR_FormatYUV444Interleaved";
		colorFormats[OMX_COLOR_FormatRawBayer8bit] = "OMX_COLOR_FormatRawBayer8bit";
		colorFormats[OMX_COLOR_FormatRawBayer10bit] = "OMX_COLOR_FormatRawBayer10bit";
		colorFormats[OMX_COLOR_FormatRawBayer8bitcompressed] = "OMX_COLOR_FormatRawBayer8bitcompressed";
		colorFormats[OMX_COLOR_FormatL2] = "OMX_COLOR_FormatL2"; 
		colorFormats[OMX_COLOR_FormatL4] = "OMX_COLOR_FormatL4"; 
		colorFormats[OMX_COLOR_FormatL8] = "OMX_COLOR_FormatL8"; 
		colorFormats[OMX_COLOR_FormatL16] = "OMX_COLOR_FormatL16"; 
		colorFormats[OMX_COLOR_FormatL24] = "OMX_COLOR_FormatL24"; 
		colorFormats[OMX_COLOR_FormatL32] = "OMX_COLOR_FormatL32";
		colorFormats[OMX_COLOR_FormatYUV420PackedSemiPlanar] = "OMX_COLOR_FormatYUV420PackedSemiPlanar";
		colorFormats[OMX_COLOR_FormatYUV422PackedSemiPlanar] = "OMX_COLOR_FormatYUV422PackedSemiPlanar";
		colorFormats[OMX_COLOR_Format18BitBGR666] = "OMX_COLOR_Format18BitBGR666";
		colorFormats[OMX_COLOR_Format24BitARGB6666] = "OMX_COLOR_Format24BitARGB6666";
		colorFormats[OMX_COLOR_Format24BitABGR6666] = "OMX_COLOR_Format24BitABGR6666";
		colorFormats[OMX_COLOR_FormatKhronosExtensions] = "OMX_COLOR_FormatKhronosExtensions";
		colorFormats[OMX_COLOR_FormatVendorStartUnused] = "OMX_COLOR_FormatVendorStartUnused";
		colorFormats[OMX_COLOR_Format32bitABGR8888] = "OMX_COLOR_Format32bitABGR8888";
		colorFormats[OMX_COLOR_Format8bitPalette] = "OMX_COLOR_Format8bitPalette";
		colorFormats[OMX_COLOR_FormatYUVUV128] = "OMX_COLOR_FormatYUVUV128";
		colorFormats[OMX_COLOR_FormatRawBayer12bit] = "OMX_COLOR_FormatRawBayer12bit";
		colorFormats[OMX_COLOR_FormatBRCMEGL] = "OMX_COLOR_FormatBRCMEGL";
		colorFormats[OMX_COLOR_FormatBRCMOpaque] = "OMX_COLOR_FormatBRCMOpaque";
		colorFormats[OMX_COLOR_FormatYVU420PackedPlanar] = "OMX_COLOR_FormatYVU420PackedPlanar";
		colorFormats[OMX_COLOR_FormatYVU420PackedSemiPlanar] = "OMX_COLOR_FormatYVU420PackedSemiPlanar";
		
		
		
		
		map<OMX_COLOR_FORMATTYPE, string>::iterator colorFormatsIterator = colorFormats.begin();
		while (colorFormatsIterator != colorFormats.end()) 
		{
			colorFormatNames.push_back((*colorFormatsIterator).second);
			++colorFormatsIterator;
			
		}
		
		
		
		
		videoCodingTypes[OMX_VIDEO_CodingUnused] = "OMX_VIDEO_CodingUnused";
		videoCodingTypes[OMX_VIDEO_CodingAutoDetect] = "OMX_VIDEO_CodingAutoDetect";
		videoCodingTypes[OMX_VIDEO_CodingMPEG2] = "OMX_VIDEO_CodingMPEG2";
		videoCodingTypes[OMX_VIDEO_CodingH263] = "OMX_VIDEO_CodingH263";
		videoCodingTypes[OMX_VIDEO_CodingMPEG4] = "OMX_VIDEO_CodingMPEG4";
		videoCodingTypes[OMX_VIDEO_CodingWMV] = "OMX_VIDEO_CodingWMV";
		videoCodingTypes[OMX_VIDEO_CodingRV] = "OMX_VIDEO_CodingRV";
		videoCodingTypes[OMX_VIDEO_CodingAVC] = "OMX_VIDEO_CodingAVC";
		videoCodingTypes[OMX_VIDEO_CodingMJPEG] = "OMX_VIDEO_CodingMJPEG";
		videoCodingTypes[OMX_VIDEO_CodingKhronosExtensions] = "OMX_VIDEO_CodingKhronosExtensions"; 
		videoCodingTypes[OMX_VIDEO_CodingVendorStartUnused] = "OMX_VIDEO_CodingVendorStartUnused";
		videoCodingTypes[OMX_VIDEO_CodingVP6] = "OMX_VIDEO_CodingVP6";
		videoCodingTypes[OMX_VIDEO_CodingVP7] = "OMX_VIDEO_CodingVP7";
		videoCodingTypes[OMX_VIDEO_CodingVP8] = "OMX_VIDEO_CodingVP8";
		videoCodingTypes[OMX_VIDEO_CodingYUV] = "OMX_VIDEO_CodingYUV";
		videoCodingTypes[OMX_VIDEO_CodingSorenson] = "OMX_VIDEO_CodingSorenson";
		videoCodingTypes[OMX_VIDEO_CodingTheora] = "OMX_VIDEO_CodingTheora";
		videoCodingTypes[OMX_VIDEO_CodingMVC] = "OMX_VIDEO_CodingMVC"; 	
		videoCodingTypes[OMX_VIDEO_CodingMax] = "OMX_VIDEO_CodingMax";
		
		map<OMX_VIDEO_CODINGTYPE, string>::iterator videoCodingTypesIterator = videoCodingTypes.begin();
		while (videoCodingTypesIterator != videoCodingTypes.end()) 
		{
			videoCodingNames.push_back((*videoCodingTypesIterator).second);
			++ videoCodingTypesIterator;
		}
		
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
		eventTypes[OMX_EventMax] = "OMX_EventMax";
		eventTypes[OMX_EventParamOrConfigChanged] = "OMX_EventParamOrConfigChanged";
		
		
		
		algorithms[OMX_CameraDisableAlgorithmFacetracking]			= "OMX_CameraDisableAlgorithmFacetracking";
		algorithms[OMX_CameraDisableAlgorithmRedEyeReduction]		= "OMX_CameraDisableAlgorithmRedEyeReduction";
		algorithms[OMX_CameraDisableAlgorithmVideoStabilisation]		= "OMX_CameraDisableAlgorithmVideoStabilisation";
		algorithms[OMX_CameraDisableAlgorithmWriteRaw]				= "OMX_CameraDisableAlgorithmWriteRaw";
		algorithms[OMX_CameraDisableAlgorithmVideoDenoise]			= "OMX_CameraDisableAlgorithmVideoDenoise";
		algorithms[OMX_CameraDisableAlgorithmStillsDenoise]			= "OMX_CameraDisableAlgorithmStillsDenoise";
		algorithms[OMX_CameraDisableAlgorithmAntiShake]				= "OMX_CameraDisableAlgorithmAntiShake";
		algorithms[OMX_CameraDisableAlgorithmImageEffects]			= "OMX_CameraDisableAlgorithmImageEffects";
		algorithms[OMX_CameraDisableAlgorithmDarkSubtract]			= "OMX_CameraDisableAlgorithmDarkSubtract";
		algorithms[OMX_CameraDisableAlgorithmDynamicRangeExpansion]	= "OMX_CameraDisableAlgorithmDynamicRangeExpansion";
		algorithms[OMX_CameraDisableAlgorithmFaceRecognition]		= "OMX_CameraDisableAlgorithmFaceRecognition";
		algorithms[OMX_CameraDisableAlgorithmFaceBeautification]		= "OMX_CameraDisableAlgorithmFaceBeautification";
		algorithms[OMX_CameraDisableAlgorithmSceneDetection]			= "OMX_CameraDisableAlgorithmSceneDetection";
		algorithms[OMX_CameraDisableAlgorithmHighDynamicRange]		= "OMX_CameraDisableAlgorithmHighDynamicRange";
		
		map<OMX_CAMERADISABLEALGORITHMTYPE, string>::iterator algorithmIterator = algorithms.begin();
		while (algorithmIterator != algorithms.end()) 
		{
			algorithmNames.push_back((*algorithmIterator).second);
			++algorithmIterator;
		}
		
		
	}
	
	
	
	
	vector<string> meteringNames;
	map<OMX_METERINGTYPE, string> meteringTypes;
	
	vector<string> exposureControlNames;
	map<OMX_EXPOSURECONTROLTYPE, string> exposureControlTypes;
	
	vector<string> videoCodingNames;
	map<OMX_VIDEO_CODINGTYPE, string> videoCodingTypes;
	
	vector<string> colorFormatNames;
	map<OMX_COLOR_FORMATTYPE, string> colorFormats;
	
	vector<string> algorithmNames;
	map<OMX_CAMERADISABLEALGORITHMTYPE, string> algorithms;
	
	map<OMX_EVENTTYPE, string> eventTypes;
	
	/*
	map<OMX_VIDEO_CODINGTYPE, string> & getVideoCodingTypes()
	{
		return videoCodingTypes;
	}
	
	map<OMX_COLOR_FORMATTYPE, string> & getColorFormats()
	{
		return colorFormats;
	}
	
	map<OMX_EVENTTYPE, string> & getEventTypes()
	{
		return eventTypes;
	}
	
	map<OMX_CAMERADISABLEALGORITHMTYPE, string> & getAlgorithms()
	{
		return algorithms;
	}*/
	
};
