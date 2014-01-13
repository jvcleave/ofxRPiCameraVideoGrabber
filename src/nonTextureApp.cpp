#include "nonTextureApp.h"

//--------------------------------------------------------------
void nonTextureApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.isUsingTexture = false;
	
	
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
	if (key == 'c')
	{
		//controlPanel.increaseContrast();
	}
	
}

void nonTextureApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

