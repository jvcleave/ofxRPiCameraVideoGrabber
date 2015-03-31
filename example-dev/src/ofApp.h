#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "CameraDemo.h"
#include "DemoCycleExposurePresets.h"
#include "DemoCycleFilters.h"
#include "DemoManualMode.h"
#include "DemoBCS.h"
#include "DemoMeteringModes.h"
#include "DemoFeatureTest.h"


class ofApp : public ofBaseApp, public KeyListener{
    
public:
    
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
    void onCharacterReceived(KeyListenerEventData& e);
    TerminalListener consoleListener;
    
    SessionConfig sessionConfig;
    ofxRPiCameraVideoGrabber videoGrabber;
    
    vector<CameraDemo*> demos;
    int currentDemoID;
    CameraDemo* currentDemo;
    bool doNextDemo;
    vector<SessionConfig::Preset> presets;
    int currentPreset;
};
