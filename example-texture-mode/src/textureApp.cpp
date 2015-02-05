#include "textureApp.h"

//--------------------------------------------------------------
void textureApp::setup()
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


}

//--------------------------------------------------------------
void textureApp::update()
{
	//not working yet
	/*if (videoGrabber.isFrameNew())
	{
		
	}*/

}


//--------------------------------------------------------------
void textureApp::draw(){

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
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
}

//--------------------------------------------------------------
void textureApp::keyPressed  (int key)
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
	
	if (key == 'q')
	{
		ofLogVerbose(__func__) << "STOPPING RECORDING";
		videoGrabber.stopRecording();
	}
	
	if (key == 't')
	{
		videoGrabber.toggleLED();
	}
}

void textureApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

