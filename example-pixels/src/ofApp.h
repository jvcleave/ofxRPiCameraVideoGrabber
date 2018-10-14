#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ImageFilterCollection.h"

class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
	ofxRPiCameraVideoGrabber videoGrabber;
	
	ImageFilterCollection filterCollection;
	
	bool doDrawInfo;
	ofTexture videoTexture;
	OMXCameraSettings omxCameraSettings;
	bool doPixels;
	bool doReloadPixels;
};

