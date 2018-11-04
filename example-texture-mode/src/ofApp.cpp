#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
	doDrawInfo	= true;
	
	//allows keys to be entered via terminal remotely (ssh)
	consoleListener.setup(this);
	
	
	settings.width = 1280; //default 1280
	settings.height = 720; //default 720
	settings.enableTexture = true; //default true


	//pass in the settings and it will start the camera
	videoGrabber.setup(settings);
	
	//ImageFilterCollection (filterCollection here) is helper class to iterate through available OpenMax filters
	filterCollection.setup();


}

//--------------------------------------------------------------
void ofApp::update()
{
	//not working yet
	/*if (videoGrabber.isFrameNew())
	{
		
	}*/

}


//--------------------------------------------------------------
void ofApp::draw(){

	//draws at camera resolution
	videoGrabber.draw();
	
	//draw a smaller version via the getTextureReference() method
	int drawWidth = settings.width/4;
	int drawHeight = settings.height/4;
	videoGrabber.getTextureReference().draw(settings.width-drawWidth, settings.height-drawHeight, drawWidth, drawHeight);

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
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
		ofLogVerbose(__func__) << "STOPPING RECORDING";
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

