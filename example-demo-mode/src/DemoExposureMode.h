#pragma once
#include "CameraDemo.h"


class DemoExposureMode  : public CameraDemo
{
    
public:
    
    bool doSwitchModes;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doSwitchModes	= false;
        
    };
    
    void update()
    {
        if (doSwitchModes) 
        {
            if(videoGrabber->cameraSettings.getExposureMode() == CameraSettings::EXPOSURE_MODE_AUTO)
            {
                videoGrabber->cameraSettings.enableManualExposure();
            }else
            {
                videoGrabber->cameraSettings.enableAutoExposure();
            }
            doSwitchModes = false;
        }
        
        string currentModeString = "UNKNOWN";
        
        int currentMode = videoGrabber->cameraSettings.getExposureMode();
        switch(currentMode)
        {
            case CameraSettings::EXPOSURE_MODE_AUTO:
            {
                currentModeString = "AUTO";
                break;
            }
            case CameraSettings::EXPOSURE_MODE_MANUAL:
            {
                currentModeString = "MANUAL";
                break;
            }
            case CameraSettings::EXPOSURE_MODE_INVALID:
            {
                currentModeString = "INVALID";
                break;
            }
        }
        
        stringstream info;
        info << "Press 1 to change Exposure Mode: " << currentModeString << "\n";

        infoString = info.str();
        
        
    };
    
    void draw()
    {
        
        
    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        if (key == '1')
        {
            doSwitchModes = !doSwitchModes;
        }
        
                
    };
};
