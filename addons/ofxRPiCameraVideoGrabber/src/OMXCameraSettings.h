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
		//currently limited to 30fps
		//http://www.raspberrypi.org/forums/viewtopic.php?p=490143#p490143
		framerate = 30;
		isUsingTexture = true;
	}
};