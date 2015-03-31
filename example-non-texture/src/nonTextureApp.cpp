#include "nonTextureApp.h"

//--------------------------------------------------------------
void nonTextureApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	SessionConfig sessionConfig;
	sessionConfig.width					= 1280;
	sessionConfig.height				= 720;
    sessionConfig.MODE = SessionConfig::MODE_NONTEXTURE;
	sessionConfig.doRecording			= false;		//default: false
	if (sessionConfig.doRecording) 
	{		
		sessionConfig.recordingFilePath		= "";		//will self generate if left blank
	}
	
	
	videoGrabber.setup(sessionConfig);
	filterCollection.setup();
	

}

//--------------------------------------------------------------
void nonTextureApp::update()
{
	
}


//--------------------------------------------------------------
void nonTextureApp::draw(){
	//Nothing really to do here as the output is rendered directly to full screen
	
}

//--------------------------------------------------------------
void nonTextureApp::keyPressed  (int key)
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

void nonTextureApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

