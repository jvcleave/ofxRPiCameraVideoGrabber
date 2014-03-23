#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ConsoleListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ImageFilterCollection.h"
#include "ControlPanel.h"
#include "ofxHTTP.h"


class texturedRemoteApp : public ofBaseApp, public SSHKeyListener{

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
	
	ControlPanel controlPanel;
	
	
	ofx::HTTP::BasicServer::SharedPtr server;
	
};

