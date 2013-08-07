#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{

	
	doShader = false;
	ofSetLogLevel(OF_LOG_VERBOSE);
	consoleListener.setup(this);
	shader.load("PostProcessing.vert", "PostProcessing.frag", "");
	omxVideoGrabber.setup(1280, 720, 60);
	
	
	
}

//--------------------------------------------------------------
void testApp::update()
{
	if (!omxVideoGrabber.isReady) 
	{
		return;
	}
}


//--------------------------------------------------------------
void testApp::draw(){

	if (!omxVideoGrabber.isReady) 
	{
		return;
	}
	if (doShader) 
	{
		shader.begin();
		omxVideoGrabber.draw();
		//shader.setUniformTexture("tex0", omxVideoGrabber.tex, omxVideoGrabber.textureID);
		shader.setUniform1f("time", ofGetElapsedTimef());
		shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight()); 
		shader.end();
	}else 
	{
		omxVideoGrabber.draw();
	}

	stringstream info;
	info << "CURRENT FILTER: " << filterCollection.currentFilter.name << "\n";
	info << "APP FPS: " << ofToString(ofGetFrameRate());
	
	ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
}



void testApp::exit()
{
	/*if (!omxVideoGrabber.isClosed) 
	{
		ofLogVerbose() << "closing manually";
		omxVideoGrabber.close();
	}*/

}
//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

	if (key == 's') 
	{
		doShader = !doShader;
		//tex.getTextureData().bFlipTexture = !tex.getTextureData().bFlipTexture;
		//tex.relo
	}
	if (key == 'r')
	{
		omxVideoGrabber.applyImageFilter(filterCollection.getRandomFilter().type);
		
	}
	if (key == 'e')
	{
		omxVideoGrabber.applyImageFilter(filterCollection.getNextFilter().type);
		
	}

}

void testApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

