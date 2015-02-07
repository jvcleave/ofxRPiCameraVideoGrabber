#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ImageFilterCollection.h"
#include "CameraDemo.h"
#include "DemoCycleExposurePresets.h"
#include "DemoCycleFilters.h"

class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
    
    OMXCameraSettings omxCameraSettings;
	ofxRPiCameraVideoGrabber videoGrabber;
	
    vector<CameraDemo*> demos;
    int currentDemoID;
    CameraDemo* currentDemo;
};

