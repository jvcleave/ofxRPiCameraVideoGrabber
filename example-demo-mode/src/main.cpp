#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"

//#define ENABLE_OMX_TRACE 1
#include "ofApp.h"
int main()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp( new ofApp());
}