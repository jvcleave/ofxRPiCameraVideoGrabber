#include "ofApp.h"



void ofApp::onTakePhotoComplete(string fileName)
{
    ofLog() << "onTakePhotoComplete fileName: " << fileName;
    
    
    
    
}
//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(0);
	ofSetLogLevel(OF_LOG_VERBOSE);
    
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
    ofFile settingsFile("settings.json");
    if(settingsFile.exists())
    {
        ofBuffer jsonBuffer = ofBufferFromFile("settings.json");
        cameraSettings.parseJSON(jsonBuffer.getText());
    }else
    {
        cameraSettings.width = 2592;
        cameraSettings.height = 1944;
        
        cameraSettings.stillPreviewWidth = 1280;
        cameraSettings.stillPreviewHeight = 720;
        
        //cameraSettings.stillPreviewWidth = cameraSettings.width;
        //cameraSettings.stillPreviewHeight = cameraSettings.height;
        cameraSettings.saturation = -100;
        cameraSettings.sharpness = 100;
        //cameraSettings.brightness = 75;
        cameraSettings.stillQuality = 100;
        cameraSettings.photoGrabberListener = this;
        cameraSettings.enableStillPreview = true;
        cameraSettings.burstModeEnabled = true;
        cameraSettings.saveJSONFile();
    }
	photoGrabber.setup(cameraSettings);

    ofLogNotice(__func__) << photoGrabber.settings.toString();
    
	filterCollection.setup();
	

}

//--------------------------------------------------------------
void ofApp::update()
{/*
    int saturation = photoGrabber.getSaturation();
    if(saturation+1 < 100)
    {
        saturation++;
    }else
    {
        saturation = -100;
    }
    photoGrabber.setSaturation(saturation);*/
}


//--------------------------------------------------------------
void ofApp::draw(){

    //photoGrabber.draw(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
    switch (key)
    {
        case 'e':
        {
            photoGrabber.setImageFilter(filterCollection.getNextFilter());
            break;
        }
        case 't':
        {
            photoGrabber.takePhoto(10);
            break;
        } 
    }

}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

