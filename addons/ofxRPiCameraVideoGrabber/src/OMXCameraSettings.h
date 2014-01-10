#pragma once

#include "ofMain.h"

class OMXCameraSettings
{
public:
	int width;
	int height;
	int framerate;
	bool isUsingTexture;
	OMXCameraSettings()
	{
		width = 1280;
		height = 720;
		framerate = 60;
		isUsingTexture = true;
	}
};