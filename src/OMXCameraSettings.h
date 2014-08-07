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
	bool doRecordingPreview; //TODO make private
	
	int previewWidth;
	int previewHeight;
	bool isUsingTexture;
	bool enablePixels;
	string recordingFilePath;
	bool doConvertToMKV;
	OMXCameraSettings()
	{
		width = 1280;
		height = 720;
		//currently limited to 30fps
		//http://www.raspberrypi.org/forums/viewtopic.php?p=490143#p490143
		framerate = 30;
		isUsingTexture = true;
		enablePixels = false;
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
private:
	//bool doRecordingPreview;
};