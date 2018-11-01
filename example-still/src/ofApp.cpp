#include "ofApp.h"


void ofApp::onTakePhotoComplete(string fileName)
{
    ofLog() << "fileName: " << fileName;
}
//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
    cameraSettings.width = 2592;
    cameraSettings.height = 1944;
    
    cameraSettings.stillPreviewWidth = 1280;
    cameraSettings.stillPreviewHeight = 720;

    
    //cameraSettings.stillPreviewWidth = cameraSettings.width;
    //cameraSettings.stillPreviewHeight = cameraSettings.height;
    cameraSettings.photoGrabberListener = this;
	photoGrabber.setup(cameraSettings);

    /*
    photoGrabber.setSharpness(100);
    photoGrabber.setContrast(100);
    photoGrabber.setBrightness(100);
    //photoGrabber.setZoomLevelNormalized(0.2);
    photoGrabber.setSaturation(-100);*/
    
    //photoGrabber.rotateClockwise();
    ofLogNotice(__func__) << photoGrabber.settings.toString();
    
	filterCollection.setup();
	

}

//--------------------------------------------------------------
void ofApp::update()
{
    int saturation = photoGrabber.getSaturation();
    if(saturation+1 < 100)
    {
        saturation++;
    }else
    {
        saturation = -100;
    }
    photoGrabber.setSaturation(saturation);
}


//--------------------------------------------------------------
void ofApp::draw(){

    photoGrabber.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	
	if (key == 'e')
	{
		photoGrabber.setImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	if (key == 'r')
	{
        photoGrabber.reset();
	}
	if (key == 't')
	{
		photoGrabber.takePhoto();
	}
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

