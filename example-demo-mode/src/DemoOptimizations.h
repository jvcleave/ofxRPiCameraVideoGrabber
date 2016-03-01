#pragma once
#include "CameraDemo.h"


class DemoOptimizations  : public CameraDemo
{
    
public:
    
    
    bool doChangeSharpening;
    bool doChangeSaturation;
    bool doChangeStabilization;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doChangeSharpening = false;
        doChangeSaturation = false;
        doChangeStabilization = false;
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
        
        
        stringstream info;
        info << " Software Sharpening: "      << videoGrabber->isSoftwareSharpeningEnabled()  << "\n";
        info << " Software Saturation: "      << videoGrabber->isSoftwareSaturationEnabled()  << "\n";
        info << " Frame Stabilization: "      << videoGrabber->getFrameStabilization()  << "\n";

        info << "\n";
        info << "Press 1 to toggle Sharpening" << "\n";
        info << "Press 2 to toggle Saturation" << "\n";
        info << "Press 3 to toggle Stabilization" << "\n";
        
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
    };
};
