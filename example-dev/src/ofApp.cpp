#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
	doDrawInfo	= true;
	
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
	
	//ImageFilterCollection (filterCollection here) is helper class to iterate through available OpenMax filters
	filterCollection.setup();
    currentExposureName = "";
    //set local alias to silly static map
    exposurePresetNames = OMX_Maps::getInstance().getExposurePresetNames();
}

int exposurePresetIndex;
//--------------------------------------------------------------
void ofApp::update()
{
	//not working yet
	/*if (videoGrabber.isFrameNew())
	{
		
	}*/
    if (ofGetFrameNum() % 100 == 0) 
    {
        if(exposurePresetIndex+1<exposurePresetNames.size())
        {
            exposurePresetIndex++;
        }else
        {
            exposurePresetIndex = 0;
        }
        currentExposureName = exposurePresetNames[exposurePresetIndex];
        
        videoGrabber.setExposurePreset(OMX_Maps::getInstance().exposurePresets[currentExposureName]);
        videoGrabber.printCurrentMeteringMode();
        //ofLogVerbose() << "currentExposureName: " << currentExposureName;
    }
}


//--------------------------------------------------------------
void ofApp::draw(){

	//draws at camera resolution
	videoGrabber.draw();
	
	//draw a smaller version via the getTextureReference() method
	int drawWidth = omxCameraSettings.width/4;
	int drawHeight = omxCameraSettings.height/4;
	videoGrabber.getTextureReference().draw(omxCameraSettings.width-drawWidth, omxCameraSettings.height-drawHeight, drawWidth, drawHeight);

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	info << "Camera Resolution: "   << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: "      << filterCollection.getCurrentFilterName()  << "\n";
    info << "getSharpness(): "      << videoGrabber.getSharpness()              << "\n";
    info << "getContrast(): "       << videoGrabber.getContrast()               << "\n";
    info << "getBrightness(): "     << videoGrabber.getBrightness()             << "\n";
    info << "getSaturation(): "      << videoGrabber.getSaturation()             << "\n";
    
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press g to Toggle info" << "\n";
    info << "currentExposureName: " << currentExposureName << "\n";
    
	if (omxCameraSettings.doRecording) 
	{
		info << "Press q to stop recording" << "\n";
	}
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
    if(key == 'a')
    {
        videoGrabber.setAutoShutter(true);
    }
    if(key == 's')
    {
        videoGrabber.setAutoShutter(false);
    }
    if(key == 'd')
    {
        videoGrabber.setAutoSensitivity(true);
    }
    if(key == 'f')
    {
        videoGrabber.setAutoSensitivity(false);
    }
    if(key == 'g')
    {
        videoGrabber.setAutoAperture(true);
    }
    if(key == 'h')
    {
        videoGrabber.setAutoAperture(false);
    }
    
    
    if(key == ' ')
    {
        videoGrabber.printCurrentMeteringMode();
    }
	
#if 0
	if (key == 'e')
	{
		videoGrabber.applyImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	
	if (key == 'q')
	{
		ofLogVerbose(__func__) << "STOPPING RECORDING";
		videoGrabber.stopRecording();
	}
	
	if (key == 't')
	{
		videoGrabber.toggleLED();
	}
#endif    
    
   
    
    
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

