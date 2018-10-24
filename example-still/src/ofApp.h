#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraPhotoGrabber.h"
#include "ImageFilterCollection.h"


class ofApp : public ofBaseApp, public KeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e);
	TerminalListener consoleListener;
	ofxRPiCameraPhotoGrabber photoGrabber;
	
	//ImageFilterCollection filterCollection;
	
	bool doDrawInfo;

};

