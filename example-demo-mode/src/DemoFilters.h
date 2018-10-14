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
            videoGrabber->setImageFilter(filterCollection.getNextFilter());
            doChangeFilter = false;
        }
        
        stringstream info;
        info << "CURRENT FILTER: "      << videoGrabber->getImageFilter()  << "\n";
        info << "\n";
        info << "Press 1 to increment filter" << "\n";
        infoString = info.str();
        
    };
    
    void draw()
    {
       
    };
    
    void onKey(int key)
    {
        if (key == '1')
        {
            doChangeFilter = !doChangeFilter;
        }
        
    };
};
