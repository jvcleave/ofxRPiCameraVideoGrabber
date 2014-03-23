#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608

class OMXCameraSettings
{
public:
	int width;
	int height;
	int framerate;
	bool doRecording;
	bool doRecordingPreview;
	int previewWidth;
	int previewHeight;
	bool isUsingTexture;
	string recordingFilePath;
	bool doConvertToMKV;
	OMXCameraSettings()
	{
		width = 1920;
		height = 1080;
		//currently limited to 30fps
		//http://www.raspberrypi.org/forums/viewtopic.php?p=490143#p490143
		framerate = 30;
		isUsingTexture = true;
		doRecording = false;
		doRecordingPreview = false;
		previewWidth = width;
		previewHeight = height;
		recordingFilePath = "";
		doConvertToMKV = false;
		//doFlipTexture = false;
	}
	
	void enablePreview()
	{
		if(width<=1280)
		{
			previewWidth = width;
			previewHeight = height;
		}else 
		{
			previewWidth = 0;
			previewHeight = 0;
		}

		
		doRecordingPreview = true;
	}
};