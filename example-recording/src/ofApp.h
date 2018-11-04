#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxOMXVideoGrabber.h"
#include "ImageFilterCollection.h"

class ofApp : public ofBaseApp, public KeyListener, public ofxOMXVideoGrabberListener
{
    
public:
    
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void onCharacterReceived(KeyListenerEventData& e);
    TerminalListener consoleListener;
    ofxOMXVideoGrabber videoGrabber;
    
    ImageFilterCollection filterCollection;
    
    ofxOMXCameraSettings settings;
    bool doDrawInfo;
    bool doStartRecording;
    bool doStopRecording;    
    
    
    //required by ofxOMXVideoGrabberListener
    void onRecordingComplete(string filePath) override;
};


