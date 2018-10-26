#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	doDrawInfo	= true;
    currentConfigFileIndex = 0;
    doSwitchConfig = false;
	consoleListener.setup(this);
    
    ofDirectory dataFolder(ofToDataPath("savedSettings"));
    
    configFiles  = dataFolder.getFiles();
    for(size_t i=0; i<configFiles.size(); i++)
    {
        ofLogVerbose(__func__) << configFiles[i].path();
    }
    currentConfigFileIndex = ofRandom(0, configFiles.size());
    
    
    //can save like this
    //ofSavePrettyJson("settings.json", omxCameraSettings.toJSON());
    
    OMXCameraSettings omxCameraSettings;
    ofBuffer jsonBuffer = ofBufferFromFile("settings.json");
    omxCameraSettings.parseJSON(jsonBuffer.getText());
    //omxCameraSettings.applyPreset(OMXCameraSettings::PRESET_720P_40FPS);
    videoGrabber.setup(omxCameraSettings);



}

//--------------------------------------------------------------
void ofApp::update()
{
    
	if(doSwitchConfig)
    {
        doSwitchConfig = false;
        if (currentConfigFileIndex+1 < configFiles.size()) 
        {
            currentConfigFileIndex++;
        }else
        {
            currentConfigFileIndex=0;
        }
        ofLog() << "LOADING CONFIG: " << configFiles[currentConfigFileIndex].path();
        
        ofBuffer jsonBuffer = ofBufferFromFile(configFiles[currentConfigFileIndex]);
        OMXCameraSettings omxCameraSettings;
        omxCameraSettings.parseJSON(jsonBuffer.getText());
        videoGrabber.setup(omxCameraSettings);
        
    }

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
	info << videoGrabber.settings.toString() << endl;
	
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
    if (key == 's')
    {
        doSwitchConfig = true;
    }

}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

