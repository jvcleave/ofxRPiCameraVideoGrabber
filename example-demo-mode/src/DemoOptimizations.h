#pragma once
#include "CameraDemo.h"


class DemoOptimizations  : public CameraDemo
{
    
public:
    
    
    bool doChangeSharpening;
    bool doChangeSaturation;
    bool doChangeStabilization;
    bool doChangeBurstMode;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doChangeSharpening = false;
        doChangeSaturation = false;
        doChangeStabilization = false;
        doChangeBurstMode = false;
        
    };
    
    void update()
    {
        if (doChangeSharpening) 
        {
            videoGrabber->setSoftwareSharpening(!videoGrabber->isSoftwareSharpeningEnabled());
            doChangeSharpening = false;
        }
        if (doChangeSaturation) 
        {
            videoGrabber->setSoftwareSaturation(!videoGrabber->isSoftwareSaturationEnabled());
            doChangeSaturation = false;
        }
        if (doChangeStabilization) 
        {
            videoGrabber->setFrameStabilization(!videoGrabber->getFrameStabilization());
            doChangeStabilization = false;
        }
        if (doChangeBurstMode) 
        {
            videoGrabber->setBurstMode(!videoGrabber->isBurstModeEnabled());
            doChangeBurstMode = false;
        }
        
        stringstream info;
        info << "Press 1 to toggle Software Sharpening: "   <<  videoGrabber->isSoftwareSharpeningEnabled() << endl;
        info << "Press 2 to toggle Software Saturation"     <<  videoGrabber->isSoftwareSaturationEnabled() << endl;
        info << "Press 3 to toggle Frame Stabilization"     <<  videoGrabber->getFrameStabilization()       << endl;
        info << "Press 4 to toggle Burst Mode: "            <<  videoGrabber->isBurstModeEnabled()          << endl;
    
        infoString = info.str();
        
    };
    
    void draw()
    {
        
    };
    
    void onKey(int key)
    {
        if (key == '1')
        {
            doChangeSharpening = true;
        }
        if (key == '2')
        {
            doChangeSaturation = true;
        }
        if (key == '3')
        {
            doChangeStabilization = true;
        }
        if (key == '4')
        {
            doChangeBurstMode = true;
        }
    };
};
