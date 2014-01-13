#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ConsoleListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ImageFilterCollection.h"


class nonTextureApp : public ofBaseApp, public SSHKeyListener{

	public:

		void setup();
		void update();
		void draw();
		void keyPressed(int key);

	void onCharacterReceived(SSHKeyListenerEventData& e);
	ConsoleListener consoleListener;
	ofxRPiCameraVideoGrabber videoGrabber;
	
	ImageFilterCollection filterCollection;
	
	bool doDrawInfo;

};

