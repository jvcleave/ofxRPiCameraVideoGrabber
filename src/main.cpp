#include "ofMain.h"

#include "testApp.h"
#include "ofGLES2Renderer.h"

//========================================================================
int main( ){
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCurrentRenderer(ofPtr<ofBaseRenderer>(new ofGLES2Renderer()));
	ofSetupOpenGL(1280, 720, OF_WINDOW);

	ofRunApp( new testApp());
}
