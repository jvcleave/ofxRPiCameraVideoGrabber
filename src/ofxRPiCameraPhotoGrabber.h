/*
 *  ofxRPiCameraPhotoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include "RPICameraController.h"

#include "StillCameraEngine.h"



class ofxRPiCameraPhotoGrabber : public RPICameraController
{

public:
    
	ofxRPiCameraPhotoGrabber();
    ~ofxRPiCameraPhotoGrabber();
    void setup(OMXCameraSettings&);
    bool isReady();
	int getWidth();
	int getHeight();
    void takePhoto();
    StillCameraEngine* engine;
    
};
