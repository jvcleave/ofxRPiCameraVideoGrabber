/*
 *  ofxOMXPhotoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include "OMXCameraController.h"

#include "PhotoEngine.h"





class ofxOMXPhotoGrabber : public OMXCameraController, public PhotoEngineListener
{

public:
    
	ofxOMXPhotoGrabber();
    ~ofxOMXPhotoGrabber();
    void setup(ofxOMXCameraSettings);
    bool isReady();
	int getWidth();
	int getHeight();
    void takePhoto(int numShots=0);
    PhotoEngine engine;
    
    
    void draw(ofRectangle& rectangle);
    void draw(int x, int y, int width, int height);
    void setDisplayAlpha(int);
    void setDisplayLayer(int);
    void setDisplayRotation(int);
    void setDisplayDrawRectangle(ofRectangle);
    void setDisplayCropRectangle(ofRectangle);
    void setDisplayMirror(bool);

    ofxOMXPhotoGrabberListener* listener;
    
    void onTakePhotoComplete(string filePath) override;
    void onPhotoEngineStart(OMX_HANDLETYPE camera_) override;
    vector<string> photosTaken;

    
};
