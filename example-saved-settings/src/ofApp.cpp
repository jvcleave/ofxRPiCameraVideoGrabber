#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
	doDrawInfo	= true;
	
	consoleListener.setup(this);
    ofFile settingsFile("STATE_2016-03-04-18-31-10-762.txt");
    
    sessionConfig.loadFile(settingsFile.path(), &videoGrabber);
	



}

//--------------------------------------------------------------
void ofApp::update()
{
	

}


//--------------------------------------------------------------
void ofApp::draw(){

	//draws at camera resolution
	videoGrabber.draw();
	
	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << endl;
    info << "Camera Resolution: " << videoGrabber.getWidth();
    info << "x" << videoGrabber.getHeight();
    info << " @ "<< videoGrabber.getFrameRate() <<"FPS" << endl;
    info << endl;
    info << "CURRENT STATE" << endl;
	info << videoGrabber.currentStateToString() << endl;
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}

}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

