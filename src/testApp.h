#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ConsoleListener.h"
#include "ofxOMXVideoGrabber.h"
#include "ImageFilterCollection.h"

class testApp : public ofBaseApp, public SSHKeyListener{

	public:

		void setup();
		void update();
		void draw();
	
		void keyPressed(int key);


	ofShader shader;
	
	bool doShader;
	bool isShaderLoaded;
	void onCharacterReceived(SSHKeyListenerEventData& e);
	ConsoleListener consoleListener;
	ofxOMXVideoGrabber omxVideoGrabber;
	
	ImageFilterCollection filterCollection;
	
};

