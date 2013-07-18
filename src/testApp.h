#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ConsoleListener.h"
#include "ofxOMXVideoGrabber.h"


class testApp : public ofBaseApp, public SSHKeyListener{

	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	

	ofShader shader;
	
	bool doShader;
	void onCharacterReceived(SSHKeyListenerEventData& e);
	ConsoleListener consoleListener;
	ofxOMXVideoGrabber omxVideoGrabber;
	vector<OMX_IMAGEFILTERTYPE> imageFilters;
	int imageFiltersCounter;
	
};

