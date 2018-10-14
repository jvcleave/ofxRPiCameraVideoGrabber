#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{	
	//allows keys to be entered via terminal remotely (ssh)
	consoleListener.setup(this);
	
    camWidth = 320; 
    camHeight = 240;
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.initGrabber(camWidth, camHeight);

    videoInverted.allocate(camWidth, camHeight, OF_PIXELS_RGBA);
    videoTexture.allocate(videoInverted);
}

//--------------------------------------------------------------
void ofApp::update()
{
    ofBackground(100, 100, 100);
    vidGrabber.update();
    
    if(vidGrabber.isFrameNew())
    {
        ofPixels& pixels = vidGrabber.getPixels();
        for (int i=0; i<pixels.getWidth(); i++) 
        { 
            for (int j=0; j<pixels.getHeight(); j++)
            {
                int pos = (j * pixels.getWidth() + i);
                videoInverted.getData()[pos * 4]   =	255-pixels.getData()[pos * 4];
                videoInverted.getData()[pos * 4+1] =	255-pixels.getData()[pos * 4+1];
                videoInverted.getData()[pos * 4+2] =	255-pixels.getData()[pos * 4+2];
                videoInverted.getData()[pos * 4+3] =	255;
            }	
        }
        
        videoTexture.loadData(videoInverted);
    }

}


//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(ofColor::white);
    ofPushMatrix();
    ofTranslate(20, 20);
    vidGrabber.draw(0, 0);
    videoTexture.draw(camWidth, 0, camWidth, camHeight);
    
	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	info << "Camera Resolution: " << vidGrabber.getWidth() << "x" << vidGrabber.getHeight()	<< " @ "<< vidGrabber.getFrameRate() <<"FPS"<< "\n";
	
	ofDrawBitmapStringHighlight(info.str(), 0, vidGrabber.getHeight(), ofColor::black, ofColor::yellow);
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
    if (key == 'e')
    {
    }
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

