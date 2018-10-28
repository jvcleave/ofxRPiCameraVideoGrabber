#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraPhotoGrabber.h"
#include "ImageFilterCollection.h"


class ofApp : public ofBaseApp, public KeyListener, public ofxRPiCameraPhotoGrabberListener
{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e) override;
	TerminalListener consoleListener;
	ofxRPiCameraPhotoGrabber photoGrabber;
	
	ImageFilterCollection filterCollection;
    OMXCameraSettings cameraSettings;

	bool doDrawInfo;
    vector<string>photoFiles;
    void onTakePhotoComplete(string fileName) override;
    
};

