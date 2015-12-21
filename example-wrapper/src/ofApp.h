#pragma once

#include "ofMain.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ofxRPiVideoGrabber.h"

class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
	ofxRPiVideoGrabber vidGrabber;
	
    ofPixels videoInverted;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;
    
    bool doDrawInfo;
		
};

