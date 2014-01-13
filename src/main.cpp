#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"

//#define use_texturedRemoteApp 1
//#define use_textureApp 1
#define use_nonTextureApp 1

#ifdef use_texturedRemoteApp
#include "texturedRemoteApp.h"
int main()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp( new texturedRemoteApp());
}
#endif

#ifdef use_textureApp
#include "textureApp.h"
int main()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp( new textureApp());
}
#endif

#ifdef use_nonTextureApp
#include "nonTextureApp.h"
int main()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp( new nonTextureApp());
}
#endif
