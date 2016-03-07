#include "ofMain.h"
#include "ofApp.h"


//#define FORCE_PROGRAMMMABLE 1

int main()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
#ifdef FORCE_PROGRAMMMABLE
    ofGLESWindowSettings settings;
    settings.width = 1280;
    settings.height = 720;
    settings.setGLESVersion(2);
    ofCreateWindow(settings);
#else
    ofSetupOpenGL(1280, 720, OF_WINDOW);
#endif
    
    ofRunApp( new ofApp());
}