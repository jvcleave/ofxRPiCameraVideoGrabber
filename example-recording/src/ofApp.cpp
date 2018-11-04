#include "ofApp.h"

void ofApp::onRecordingComplete(string filePath)
{
    ofLogNotice(__FUNCTION__) << "filePath: " << filePath;
}
//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	doDrawInfo	= true;
    
    doStartRecording = false;
    doStopRecording =false;
    consoleListener.setup(this);
    settings.enableTexture = false;
    settings.width = 1280;
    settings.height = 720;
    settings.videoGrabberListener = this; //calls onRecordingComplete
    
	//pass in the settings and it will start the camera
	videoGrabber.setup(settings);
	
	//ImageFilterCollection (filterCollection here) is helper class to iterate through available OpenMax filters
	filterCollection.setup();


}

//--------------------------------------------------------------
void ofApp::update()
{
	if(doStartRecording)
    {
        doStartRecording = false;
        videoGrabber.startRecording();
    }
    if(doStopRecording)
    {
        doStopRecording = false;
        videoGrabber.stopRecording();
    }
}


//--------------------------------------------------------------
void ofApp::draw(){

	//draws at camera resolution
    if(videoGrabber.isTextureEnabled())
    {
        videoGrabber.draw();
        
        //draw a smaller version
        int drawWidth = videoGrabber.getWidth()/4;
        int drawHeight = videoGrabber.getHeight()/4;
        videoGrabber.draw(ofGetWidth()-drawWidth, 
                          ofGetHeight()-drawHeight, 
                          drawWidth, drawHeight);
    }


	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << endl;
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< endl;
	
	info << endl;
	info << "Press 1 to START RECORDING" << endl;
	info << "Press 2 to STOP RECORDING" << endl;
    
    ofColor circleColor = ofColor::red;
	if (videoGrabber.isRecording()) 
	{
		circleColor = ofColor::green;
	}
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
    
    ofPushStyle();
        ofSetColor(circleColor);
        ofDrawCircle(ofGetWidth() - 200, 40, 10);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	
	if (key == '1')
	{
        doStartRecording = true;
	}
	
	if (key == '2')
	{
        ofLogVerbose(__func__) << "STOPPING RECORDING";
        doStopRecording = true;

       
	}
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

