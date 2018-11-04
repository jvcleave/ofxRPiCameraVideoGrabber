#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxOMXPhotoGrabber.h"
#include "ImageFilterCollection.h"


class ofApp : public ofBaseApp, public KeyListener, public ofxOMXPhotoGrabberListener
{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(KeyListenerEventData& e) override;
	TerminalListener consoleListener;
	ofxOMXPhotoGrabber photoGrabber;
	
	ImageFilterCollection filterCollection;
    ofxOMXCameraSettings cameraSettings;

	bool doDrawInfo;
    vector<string>photoFiles;
    void onTakePhotoComplete(string fileName) override;
    
};

