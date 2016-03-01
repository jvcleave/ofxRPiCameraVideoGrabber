#pragma once
#include "CameraDemo.h"


class DemoRotation  : public CameraDemo
{
    
public:
    
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        
    };
    
    void update()
    {
        
        stringstream info;
        
        info << "\n";
        info << "CURRENT ROTATION MODE: " << videoGrabber->getRotation() << "\n";
        info << "Press 1 for 0"     << "\n";
        info << "Press 2 for 90"    << "\n";
        info << "Press 3 for 180"   << "\n";
        info << "Press 4 for 270"   << "\n";
        infoString = info.str();
        
    };
    
    void draw()
    {
        
        
        
        
        
    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        switch (key)
        {
                
            case '1':
            {
                videoGrabber->setRotation(ROTATION_0);
                break;
            }
            case '2':
            {
                videoGrabber->setRotation(ROTATION_90);
                break;
            }
            case '3':
            {
                videoGrabber->setRotation(ROTATION_180);
                break;
            }
            case '4':
            {
                videoGrabber->setRotation(ROTATION_270);
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
