#pragma once

#include "ofMain.h"

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
		width = 1280;
		height = 720;
		//currently limited to 30fps
		//http://www.raspberrypi.org/forums/viewtopic.php?p=490143#p490143
		framerate = 30;
		isUsingTexture = true;
		doRecording = false;
		doRecordingPreview = false;
		previewWidth = width;
		previewHeight = height;
		recordingFilePath = "";
		doConvertToMKV = true;
		//doFlipTexture = false;
	}
};