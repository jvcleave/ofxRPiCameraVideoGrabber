#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.width					= 1280;
	omxCameraSettings.height				= 720;
	omxCameraSettings.enableTexture         = false;
	omxCameraSettings.doRecording			= false;		//default: false
	if (omxCameraSettings.doRecording) 
	{		
		omxCameraSettings.recordingFilePath		= "";		//will self generate if left blank
	}
	
	
	videoGrabber.setup(omxCameraSettings);
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
		videoGrabber.setImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	if (key == 'q')
	{
		ofLogVerbose(__func__) << "SENDING QUIT";
		videoGrabber.stopRecording();
	}
	if (key == 't')
	{
		videoGrabber.toggleLED();
	}
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

