#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	SessionConfig sessionConfig;
	sessionConfig.width					= 1280;
	sessionConfig.height				= 720;
    sessionConfig.mode = SessionConfig::MODE_NONTEXTURE;
	
	
	photoGrabber.setup(sessionConfig);
	filterCollection.setup();
	

}

//--------------------------------------------------------------
void ofApp::update()
{
	
}


//--------------------------------------------------------------
void ofApp::draw(){
	//Nothing really to do here as the output is rendered directly to full screen
	
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	
	if (key == 'e')
	{
		photoGrabber.setImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	if (key == 'q')
	{
	}
	if (key == 't')
	{
		photoGrabber.takePhoto();
	}
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

