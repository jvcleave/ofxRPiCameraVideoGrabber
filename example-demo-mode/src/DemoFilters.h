#pragma once
#include "CameraDemo.h"


class DemoFilters  : public CameraDemo
{
    
public:
    
    
    bool doChangeFilter;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doChangeFilter = false;
    };
    
    void update()
    {
        if (doChangeFilter) 
        {
            videoGrabber->applyImageFilter(filterCollection.getNextFilter());
            doChangeFilter = false;
        }
        
        stringstream info;
        info << "CURRENT FILTER: "      << filterCollection.getCurrentFilterName()  << "\n";
        info << "\n";
        info << "Press z to increment filter" << "\n";
        infoString = info.str();
        
    };
    
    void draw()
    {
       
    };
    
    void onKey(int key)
    {
        if (key == 'z')
        {
            doChangeFilter = !doChangeFilter;
        }
        
    };
};
