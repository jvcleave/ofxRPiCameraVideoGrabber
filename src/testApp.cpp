#include "testApp.h"






//--------------------------------------------------------------
void testApp::setup()
{
	
	doShader = false;
	ofSetLogLevel(OF_LOG_VERBOSE);
	consoleListener.setup(this);
	shader.load("PostProcessing.vert", "PostProcessing.frag", "");
	omxVideoGrabber.setup(640, 360);
	
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
		shader.setUniformTexture("tex0", omxVideoGrabber.tex, omxVideoGrabber.textureID);
		shader.setUniform1f("time", ofGetElapsedTimef());
		shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
		ofRect(0, 0, ofGetWidth(), ofGetHeight());
		shader.end();
	}else 
	{
		omxVideoGrabber.draw();
	}

	
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 100, 100, ofColor::black, ofColor::yellow);
}



void testApp::exit()
{
	if (!omxVideoGrabber.isClosed) 
	{
		ofLogVerbose() << "closing manually";
		omxVideoGrabber.close();
	}
	bcm_host_deinit();

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
	if (key == 'e')
	{
		omxVideoGrabber.applyEffect();
		
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

