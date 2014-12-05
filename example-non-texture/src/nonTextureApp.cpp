#include "nonTextureApp.h"

//--------------------------------------------------------------
void nonTextureApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.width					= 1280;
	omxCameraSettings.height				= 720;
	omxCameraSettings.isUsingTexture		= false;
	omxCameraSettings.doRecording			= false;		//default: false
	if (omxCameraSettings.doRecording) 
	{		
		omxCameraSettings.doRecordingPreview	= true;
		omxCameraSettings.recordingFilePath		= "";		//will self generate if left blank
		omxCameraSettings.doConvertToMKV		= false;	//converts file to .mkv using mkvmerge(if present)
	}
	
	
	videoGrabber.setup(omxCameraSettings);
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
	ofLogVerbose(__func__) << key;
	
	
	if (key == 'e')
	{
		videoGrabber.applyImageFilter(filterCollection.getNextFilter());
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

void nonTextureApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

