#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
	doDrawInfo	= true;
	
	//allows keys to be entered via terminal remotely (ssh)
	consoleListener.setup(this);
	
	
	omxCameraSettings.width = 1920; //default 1280
	omxCameraSettings.height = 1080; //default 720
	omxCameraSettings.isUsingTexture = true; //default true
	omxCameraSettings.doRecording = false;   //default false
	

	//pass in the settings and it will start the camera
	videoGrabber.setup(omxCameraSettings);
	
	//ImageFilterCollection (filterCollection here) is helper class to iterate through available OpenMax filters
	filterCollection.setup();


}
bool doStartRecording = false;
bool doStopRecording = false;
//--------------------------------------------------------------
void ofApp::update()
{
    if (doStartRecording) 
    {
        ofLogVerbose(__func__) << "STARTING RECORDING";
        videoGrabber.startRecording();
        doStartRecording = false;
    }
    if(doStopRecording)
    {
        ofLogVerbose(__func__) << "STOPPING RECORDING";

        videoGrabber.stopRecording();
        doStopRecording = false;
    }
}


//--------------------------------------------------------------
void ofApp::draw()
{
    if(!videoGrabber.isTextureEnabled())
    {
        return;
    }
	//draws at camera resolution
	videoGrabber.draw();
	
	//draw a smaller version via the getTextureReference() method
	int drawWidth = omxCameraSettings.width/4;
	int drawHeight = omxCameraSettings.height/4;
	videoGrabber.getTextureReference().draw(omxCameraSettings.width-drawWidth, omxCameraSettings.height-drawHeight, drawWidth, drawHeight);

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.getCurrentFilterName() << "\n";	
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press g to Toggle info" << "\n";
	if (omxCameraSettings.doRecording) 
	{
		info << "Press q to stop recording" << "\n";
	}
    ofColor circleColor;
    if (videoGrabber.isRecording()) 
    {
        circleColor = ofColor::green;
    }else
    {
        circleColor = ofColor::red;
    }
    
    
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
        ofPushStyle();
            ofSetColor(circleColor, 90);
            ofCircle(ofPoint(ofGetWidth() - 200, 100), 50);
        ofPopStyle();
	}
    
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	
	if (key == 'e')
	{
		videoGrabber.applyImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	
    if (key == 's')
    {
        doStartRecording = true;
        
    }
    
	if (key == 'q')
	{
	
        doStopRecording = true;
	}
	
	if (key == 't')
	{
		videoGrabber.toggleLED();
	}
    
    if(key == 'm')
    {
        videoGrabber.printMeteringMode();
    }
    
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

