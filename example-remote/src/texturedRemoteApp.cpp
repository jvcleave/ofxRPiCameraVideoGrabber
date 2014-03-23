#include "texturedRemoteApp.h"

//--------------------------------------------------------------
void texturedRemoteApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.width = 1280;
	omxCameraSettings.height = 720;
	omxCameraSettings.framerate = 30;
	
	
	videoGrabber.setup(omxCameraSettings);
	filterCollection.setup();
	
	ofx::HTTP::BasicServerSettings settings;
    settings.setPort(8998);
	server = ofx::HTTP::BasicServer::makeShared(settings);
    server->start();
	controlPanel.setup(&videoGrabber, "JVCTOWER.local");
}

//--------------------------------------------------------------
void texturedRemoteApp::update()
{
	if (videoGrabber.isFrameNew())
	{
		
	}
}


//--------------------------------------------------------------
void texturedRemoteApp::draw(){

	videoGrabber.draw();

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	//info << "Camera frameCounter: " << videoGrabber.frameCounter << "\n";
	//info << "App frameCounter: " << ofGetFrameNum() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.getCurrentFilterName() << "\n";
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press g to Toggle info" << "\n";
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
	
	
	controlPanel.draw();
	//
}

//--------------------------------------------------------------
void texturedRemoteApp::keyPressed  (int key)
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
		controlPanel.increaseContrast();
	}
	
}

void texturedRemoteApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

