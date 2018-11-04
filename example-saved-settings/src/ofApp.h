#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxOMXVideoGrabber.h"

class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
    
	ofxOMXVideoGrabber videoGrabber;
		
	bool doDrawInfo;
    ofDirectory dataFolder;	
    vector<ofFile> configFiles;
    int currentConfigFileIndex;
    bool doSwitchConfig;
};

