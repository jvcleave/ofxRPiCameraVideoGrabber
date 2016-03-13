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
	
	bool isUsingTexture;
	bool enablePixels;
	string recordingFilePath;
	OMXCameraSettings()
	{
		width = 1280;
		height = 720;
		framerate = 30;
		isUsingTexture = true;
		enablePixels = false;
		doRecording = false;
		recordingFilePath = "";
	}
	
    
    string toString()
    {
        stringstream info;
        info << "width " << width << endl;
        info << "height " << height << endl;
        info << "framerate " << framerate << endl;
        info << "isUsingTexture " << isUsingTexture << endl;
        info << "enablePixels " << enablePixels << endl;
        info << "doRecording " << doRecording << endl;
        info << "recordingFilePath " << recordingFilePath << endl;
        return info.str();
    }
};