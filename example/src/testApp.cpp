#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);
	
	doDrawInfo	= true;
	doShader	= false;
	shader.load("PostProcessing.vert", "PostProcessing.frag", "");
	
	consoleListener.setup(this);
	videoGrabber.setup(1280, 720, 60);
	
}

//--------------------------------------------------------------
void testApp::update()
{
	
}


//--------------------------------------------------------------
void testApp::draw(){

	if (doShader) 
	{
		int width = ofGetWidth();
		int height = ofGetHeight();
		
		shader.begin();
			shader.setUniform1f("time", ofGetElapsedTimef());
			shader.setUniform2f("resolution", (float)width, (float)height); 
			videoGrabber.draw();
		shader.end();
		
	}else 
	{
		videoGrabber.draw();
	}

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	info << "Camera frameCounter: " << videoGrabber.frameCounter << "\n";
	info << "App frameCounter: " << ofGetFrameNum() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.currentFilter.name << "\n";
	info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press s to enable Shader ENABLED: " << doShader << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press r for Random filter" << "\n";
	info << "Press g to Toggle info" << "\n";
	
	if (doDrawInfo && !doShader) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
	//
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{
	ofLogVerbose(__func__) << key;
	if (key == 's') 
	{
		doShader = !doShader;
		
	}
	if (key == 'r')
	{
		videoGrabber.applyImageFilter(filterCollection.getRandomFilter().type);
	}
	
	if (key == 'e')
	{
		videoGrabber.applyImageFilter(filterCollection.getNextFilter().type);
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
}

void testApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

