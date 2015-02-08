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
    demo1->name = "CYCLE EXPOSURE DEMO";
    demos.push_back(demo1);
    
    DemoCycleFilters* demo2 = new DemoCycleFilters();
    demo2->setup(omxCameraSettings, &videoGrabber);
    demo2->name = "CYCLE FILTER DEMO";
    demos.push_back(demo2);
    
    DemoManualMode* demo3 = new DemoManualMode();
    demo3->setup(omxCameraSettings, &videoGrabber);
    demo3->name = "MANUAL MODE DEMO";
    demos.push_back(demo3);
    
    DemoBCS* demo4 = new DemoBCS();
    demo4->setup(omxCameraSettings, &videoGrabber);
    demo4->name = "SHARP/BRIGHT/CONTRAST/SATURATION DEMO";
    demos.push_back(demo4);
    
    
    DemoMeteringModes* demo5 = new DemoMeteringModes();
    demo5->setup(omxCameraSettings, &videoGrabber);
    demo5->name = "METERING MODES";
    demos.push_back(demo5);
    
    DemoFeatureTest* demo6 = new DemoFeatureTest();
    demo6->setup(omxCameraSettings, &videoGrabber);
    demo6->name = "FEATURE TEST";
    demos.push_back(demo6);
    
    
    doNextDemo = false;
    currentDemoID =0;
    currentDemo = demos[currentDemoID];
}


//--------------------------------------------------------------
void ofApp::update()
{
    if (doNextDemo)
    {
        if((unsigned int) currentDemoID+1 < demos.size())
        {
            currentDemoID++;
        }else
        {
            currentDemoID = 0;
        }
        currentDemo = demos[currentDemoID];
        videoGrabber.setDefaultValues();
        doNextDemo = false;
    }else
    {
         currentDemo->update();
    }
   
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    currentDemo->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	if(key == ' ')
    {
        doNextDemo = true;
    }else
    {
        currentDemo->onKey(key);
    }
	
       
    
    
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

