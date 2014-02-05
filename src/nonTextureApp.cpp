#include "nonTextureApp.h"

//--------------------------------------------------------------
void nonTextureApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.width					= 1920;
	omxCameraSettings.height				= 1080;
	omxCameraSettings.isUsingTexture		= false;
	if (!omxCameraSettings.isUsingTexture) 
	{
		//some options available with direct-to-screen rendering
		omxCameraSettings.doRecording			= true;		//default: false
		omxCameraSettings.doRecordingPreview	= true;		//default: false
		omxCameraSettings.previewWidth			= 640;		//default: omxCameraSettings.width
		omxCameraSettings.previewHeight			= 480;		//default: omxCameraSettings.height
		omxCameraSettings.recordingFilePath		= "";		//will self generate if left blank
		omxCameraSettings.doConvertToMKV		= false;	//converts file to .mkv using mkvmerge(if present)
	}
	
	
	videoGrabber.setup(omxCameraSettings);
	filterCollection.setup(&videoGrabber.omxMaps);
	

}

//--------------------------------------------------------------
void nonTextureApp::update()
{
	
}


//--------------------------------------------------------------
void nonTextureApp::draw(){


	
	

	//
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
		ofLogVerbose() << "SENDING QUIT";
		//videoGrabber.engine->want_quit = true;
		videoGrabber.stopRecording();
	}
	
}

void nonTextureApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

