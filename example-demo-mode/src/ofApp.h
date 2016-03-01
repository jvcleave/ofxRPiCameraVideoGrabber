#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "TerminalListener.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "CameraDemo.h"

#include "DemoEnhancement.h"
#include "DemoZoomCrop.h"
#include "DemoMirrorMode.h"
#include "DemoRotation.h"
#include "DemoFilters.h"
#include "DemoExposurePresets.h"
#include "DemoMetering.h"
#include "DemoOptimizations.h"
#include "DemoWhiteBalance.h"

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
    
    map<int, CameraDemo*> demos;
    int currentDemoID;
    CameraDemo* currentDemo;
    bool doNextDemo;
    bool doDrawInfo;
    bool doPrintInfo;
};
