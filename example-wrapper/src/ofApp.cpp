#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	doDrawInfo	= true;
	
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
        ofPixels & pixels = vidGrabber.getPixels();
        ofLogVerbose() << "pixels.size(): " << pixels.size();
        for(int i = 0; i < pixels.size(); i++){
            videoInverted[i] = pixels[i];
        }
        videoTexture.loadData(videoInverted);
    }

}


//--------------------------------------------------------------
void ofApp::draw(){

    ofSetHexColor(0xffffff);
    vidGrabber.draw(20, 20);
    videoTexture.draw(20 + camWidth, 20, camWidth, camHeight);
    
#if 0	
	//draw a smaller version via the getTextureReference() method
	int drawWidth = omxCameraSettings.width/4;
	int drawHeight = omxCameraSettings.height/4;
	vidGrabber.getTextureReference().draw(omxCameraSettings.width-drawWidth, 
                                          omxCameraSettings.height-drawHeight, 
                                          drawWidth, 
                                          drawHeight);
#endif
	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	info << "Camera Resolution: " << vidGrabber.getWidth() << "x" << vidGrabber.getHeight()	<< " @ "<< vidGrabber.getFrameRate() <<"FPS"<< "\n";
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, vidGrabber.getHeight(), ofColor::black, ofColor::yellow);
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
#if 0	
	if (key == 'e')
	{
		vidGrabber.applyImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	
	if (key == 'q')
	{
		ofLogVerbose(__func__) << "STOPPING RECORDING";
		vidGrabber.stopRecording();
	}
	
	if (key == 't')
	{
		vidGrabber.toggleLED();
	}
#endif
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

