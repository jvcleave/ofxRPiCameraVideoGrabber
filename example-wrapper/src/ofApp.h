#pragma once

#include "ofMain.h"
#include "TerminalListener.h"
#include "RPiVideoGrabber.h"

class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
    
    //wrapper class for drop-in replacement of ofVideoGrabber
	RPiVideoGrabber vidGrabber;
	
    ofPixels videoInverted;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;
    		
};

