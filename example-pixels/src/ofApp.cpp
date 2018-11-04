#include "ofApp.h"

//Pixel access is not implemented in the player
//so here is how to do it yourself

//it should be implemented along with an "isFrameNew" but that isn't working yet

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel("ofThread", OF_LOG_ERROR);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	settings.width = 1280;
	settings.height = 720;
	settings.framerate = 30;
	settings.enableTexture = true;
	
	
	
	filterCollection.setup();

	doPixels = true;
	doReloadPixels = true;
	if (doPixels) 
	{
		settings.enablePixels = true;
		videoTexture.allocate(settings.width, settings.height, GL_RGBA);
	}

	videoGrabber.setup(settings);
		
}	

//--------------------------------------------------------------
void ofApp::update()
{
    
    
	if (!doPixels)
	{
		return;
	}
	
	if(doReloadPixels)
	{
        if (videoGrabber.isFrameNew()) 
        {
            videoTexture.loadData(videoGrabber.getPixels(), settings.width, settings.height, GL_RGBA);
        }

	}


}


//--------------------------------------------------------------
void ofApp::draw(){
	
	videoGrabber.draw();
	if(doPixels && doReloadPixels)
	{
		videoTexture.draw(0, 0, settings.width/2, settings.height/2);
	}

	stringstream info;
	info << "APP FPS: " << ofGetFrameRate() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.getCurrentFilterName() << "\n";
	info << "PIXELS ENABLED: " << doPixels << "\n";
	info << "PIXELS RELOADING ENABLED: " << doReloadPixels << "\n";
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to Increment filter" << "\n";
	info << "Press p to Toggle pixel processing" << "\n";
	info << "Press r to Toggle pixel reloading" << "\n";
	info << "Press g to Toggle info" << "\n";
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
	
	//
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
	
	if (key == 'e')
	{
		videoGrabber.setImageFilter(filterCollection.getNextFilter());
	}
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	
	if (key == 'p')
	{
		doPixels = !doPixels;
		if (!doPixels) 
		{
			videoGrabber.disablePixels();
		}else
		{
			videoGrabber.enablePixels();
		}
	}
	if (key == 'r') {
		doReloadPixels = !doReloadPixels;
	}
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

