#pragma once
#include "CameraDemo.h"


class DemoExposurePresets  : public CameraDemo
{
    
public:
    
    string currentExposureName;	

    size_t exposurePresetIndex;
    bool changePreset;
    size_t numPresets;
    vector<string> exposurePresetNames;
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(videoGrabber_);
        exposurePresetIndex = 0;
        currentExposureName = "";
        changePreset = false;
        exposurePresetNames = OMX_Maps::getInstance().getExposurePresetNames();
        numPresets = exposurePresetNames.size();
        currentExposureName = videoGrabber->getExposurePreset();
        for(size_t i=0; i<exposurePresetNames.size(); i++)
        {
            if(currentExposureName == exposurePresetNames[i])
            {
                exposurePresetIndex = i;
            }
        }
        
    };
    
    void update()
    {
        if (changePreset) 
        {
            if(exposurePresetIndex+1 < numPresets)
            {
                exposurePresetIndex++;
            }else
            {
                exposurePresetIndex = 0;
            }
            currentExposureName = exposurePresetNames[exposurePresetIndex];
            
            videoGrabber->setExposurePreset(GetExposurePreset(currentExposureName));
            changePreset = false;
        }
        stringstream info;

        info << "Press 1 to change Exposure Preset: " << videoGrabber->getExposurePreset() << "\n";

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
