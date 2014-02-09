#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ConsoleListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ImageFilterCollection.h"

class shaderApp : public ofBaseApp, public SSHKeyListener{

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
	
	ofFbo fbo;
	ofShader shader;
	bool doShader;
		
	unsigned char * videoPixels;
	ofTexture videoTexture;
	OMXCameraSettings omxCameraSettings;
	bool doPixels;
};

