#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "CameraDemo.h"

#include "DemoExposureMode.h"
#include "DemoMirrorMode.h"


class ofApp : public ofBaseApp, public KeyListener{
    
public:
    
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void onCharacterReceived(KeyListenerEventData& e);
    TerminalListener consoleListener;
    
    OMXCameraSettings omxCameraSettings;
    ofxRPiCameraVideoGrabber videoGrabber;
    
    vector<CameraDemo*> demos;
    int currentDemoID;
    CameraDemo* currentDemo;
    bool doNextDemo;
    vector<OMXCameraSettings::Preset> presets;
    int currentPreset;
    bool doDrawInfo;
    bool doPrintInfo;
    bool doPresetChange;
};
