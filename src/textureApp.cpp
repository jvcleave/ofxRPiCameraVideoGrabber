#include "textureApp.h"

//--------------------------------------------------------------
void textureApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	
	omxCameraSettings.width = 1280;
	omxCameraSettings.height = 720;
	//omxCameraSettings.framerate = 25;
	omxCameraSettings.isUsingTexture = true;
	omxCameraSettings.doRecording = true;
	
	videoGrabber.setup(omxCameraSettings);
	filterCollection.setup(&videoGrabber.omxMaps);


}

//--------------------------------------------------------------
void textureApp::update()
{
	/*if (videoGrabber.isFrameNew())
	{
		
	}*/

}


//--------------------------------------------------------------
void textureApp::draw(){

	
	videoGrabber.draw();
	
	int drawWidth = omxCameraSettings.width/4;
	int drawHeight = omxCameraSettings.height/4;
	videoGrabber.getTextureReference().draw(omxCameraSettings.width-drawWidth, omxCameraSettings.height-drawHeight, drawWidth, drawHeight);

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	//info << "Camera frameCounter: " << videoGrabber.frameCounter << "\n";
	//info << "App frameCounter: " << ofGetFrameNum() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.getCurrentFilterName() << "\n";
	//info << "DO RECORDING: " << omxCameraSettings.doRecording << "\n";
	
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press g to Toggle info" << "\n";
	if (omxCameraSettings.doRecording) 
	{
		info << "Press q to stop recording" << "\n";
	}
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
	
	//
}

//--------------------------------------------------------------
void textureApp::keyPressed  (int key)
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
	if (key == 'q')
	{
		ofLogVerbose() << "SENDING QUIT";
		//videoGrabber.engine->want_quit = true;
		videoGrabber.stopRecording();
	}
}

void textureApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

