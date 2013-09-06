#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{

	
	doShader = false;
	ofSetLogLevel(OF_LOG_VERBOSE);
	consoleListener.setup(this);
	shader.load("PostProcessing.vert", "PostProcessing.frag", "");
	videoGrabber.setup(1280, 720, 60);
	
	
	
}

//--------------------------------------------------------------
void testApp::update()
{
	if (!videoGrabber.isReady) 
	{
		return;
	}
}


//--------------------------------------------------------------
void testApp::draw(){

	if (!videoGrabber.isReady) 
	{
		return;
	}
	if (doShader) 
	{
		int width = ofGetWidth();
		int height = ofGetHeight();
		
		shader.begin();
			shader.setUniformTexture("tex0", videoGrabber.getTextureReference(), videoGrabber.textureID);
			shader.setUniform1f("time", ofGetElapsedTimef());
			shader.setUniform2f("resolution", (float)width, (float)height); 
			ofRect(0, 0, width, height);
		shader.end();
		
	}else 
	{
		videoGrabber.draw();
	}

	stringstream info;
	info << "SHADER ENABLED: "	<< doShader << "\n";
	info << "CURRENT FILTER: "	<< filterCollection.currentFilter.name << "\n";
	info << "APP FPS: "			<< ofToString(ofGetFrameRate()) << "\n";
	
	info <<	filterCollection.filterList << "\n";
	ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

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

}

void testApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

