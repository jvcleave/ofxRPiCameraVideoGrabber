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
	
	bool enableTexture;
	bool enablePixels;
	bool enableMultipleCameras;
	int cameraIndex;
	string recordingFilePath;
	OMXCameraSettings()
	{
		width = 1280;
		height = 720;
		framerate = 30;
		enableTexture = true;
		enablePixels = false;
		doRecording = false;
		enableMultipleCameras = false;
		cameraIndex = 0;
		recordingFilePath = "";
	}
	
    
    string toString()
    {
        stringstream info;
        info << "width " << width << endl;
        info << "height " << height << endl;
        info << "framerate " << framerate << endl;
        info << "enableTexture " << enableTexture << endl;
        info << "enablePixels " << enablePixels << endl;
        info << "doRecording " << doRecording << endl;
        info << "recordingFilePath " << recordingFilePath << endl;
		info << "enableMultipleCameras " << enableMultipleCameras << endl;
		info << "cameraIndex " << cameraIndex << endl;

        return info.str();
    }
};