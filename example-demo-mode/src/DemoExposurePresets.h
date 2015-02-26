#pragma once
#include "CameraDemo.h"


class DemoExposurePresets  : public CameraDemo
{
    
public:
    
    string currentExposureName;	

    int exposurePresetIndex;
    bool changePreset;
    unsigned int numPresets;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(videoGrabber_);
        exposurePresetIndex = 0;
        currentExposureName = "";
        changePreset = false;
        numPresets = OMX_Maps::getInstance().getExposurePresetNames().size();
    };
    
    void update()
    {
        if (changePreset) 
        {
            if((unsigned int) exposurePresetIndex+1 < numPresets)
            {
                exposurePresetIndex++;
            }else
            {
                exposurePresetIndex = 0;
            }
            currentExposureName = OMX_Maps::getInstance().getExposurePresetNames()[exposurePresetIndex];
            
            videoGrabber->setExposurePreset(OMX_Maps::getInstance().exposurePresets[currentExposureName]);
            changePreset = false;
        }
        stringstream info;

        info << "Press 1 to change Exposure Preset: " << videoGrabber->getCurrentExposurePresetName() << "\n";

        infoString = info.str();
    };
    
    void draw()
    {
                
     
        

    };
    
    void onKey(int key)
    {
        if(key == '1')
        {
            changePreset = true;
        }
    }
};
