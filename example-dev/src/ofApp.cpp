#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    
    

	
	//allows keys to be entered via terminal remotely (ssh)
	consoleListener.setup(this);
	
	
	omxCameraSettings.width = 1280; //default 1280
	omxCameraSettings.height = 720; //default 720
	omxCameraSettings.isUsingTexture = true; //default true
	omxCameraSettings.doRecording = false;   //default false
    omxCameraSettings.doManualExposure = false;
	if (omxCameraSettings.doRecording) 
	{
		/*
		 If you are recording you have the option to display a "preview"
		 
		 This seems to have issues at 1080p so calling omxSettings.enablePreview()
		 will do some validation
		 
		 */
		if (omxCameraSettings.doRecordingPreview)
		{
			omxCameraSettings.enablePreview();
		}
		/*
		 
		 You can also specify a filename or it will generate one 
		 using a timestamp and put it in bin/data
		 
		 */
		omxCameraSettings.recordingFilePath = ""; //default "" will self-generate
		
	}

	//pass in the settings and it will start the camera
	videoGrabber.setup(omxCameraSettings);
	
    DemoCycleExposurePresets* demo1 = new DemoCycleExposurePresets();
    demo1->setup(omxCameraSettings, &videoGrabber);
    demos.push_back(demo1);
    
    
    
    DemoCycleFilters* demo2 = new DemoCycleFilters();
    demo2->setup(omxCameraSettings, &videoGrabber);
    demos.push_back(demo2);
    
    currentDemoID =0;
    currentDemo = demos[currentDemoID];
}


//--------------------------------------------------------------
void ofApp::update()
{
    if (ofGetFrameNum() % 500 == 0)
    {
        if((unsigned int) currentDemoID+1 < demos.size())
        {
            currentDemoID++;
        }else
        {
            currentDemoID = 0;
        }
        currentDemo = demos[currentDemoID];
    }else
    {
         currentDemo->update();
    }
   
	//not working yet
	/*if (videoGrabber.isFrameNew())
	{
		
	}*/
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    currentDemo->draw();
    ofDrawBitmapStringHighlight("DEMO ID: "+ ofToString(currentDemoID), ofGetWidth()-200, ofGetHeight()-100, ofColor::black, ofColor::red);
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	
	currentDemo->onKey(key);
       
    
    
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

