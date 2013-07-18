#include "ofMain.h"

#include "testApp.h"

#ifdef PROGRAMMABLE_PRESENT
	#include "ofGLProgrammableRenderer.h"
#else
	#include "ofGLES2Renderer.h"
#endif

//========================================================================
int main( ){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
#ifdef PROGRAMMABLE_PRESENT
	ofSetCurrentRenderer(ofPtr<ofBaseRenderer>(new ofGLProgrammableRenderer()));
#else
	ofSetCurrentRenderer(ofPtr<ofBaseRenderer>(new ofGLES2Renderer()));
#endif

	ofSetupOpenGL(1280, 720, OF_WINDOW);

	ofRunApp( new testApp());
}
