#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	doDrawInfo	= true;
    //currentConfigFileIndex = 0;
    //doSwitchConfig = false;
	consoleListener.setup(this);
    
    ofDirectory dataFolder(ofToDataPath(""));
    
    configFiles  = dataFolder.getFiles();
    for(size_t i=0; i<configFiles.size(); i++)
    {
        ofLogVerbose(__func__) << configFiles[i].path();
    }
    int currentConfigFileIndex = ofRandom(0, configFiles.size());
    sessionConfig.setup(configFiles[currentConfigFileIndex]);
    videoGrabber.setup(sessionConfig);



}

//--------------------------------------------------------------
void ofApp::update()
{
    /*
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
        SessionConfig sessionConfig;
        sessionConfig.setup(configFiles[currentConfigFileIndex]);
        videoGrabber.setup(sessionConfig);
        
    }*/

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
    if (key == 's')
    {
        //doSwitchConfig = true;
    }

}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

