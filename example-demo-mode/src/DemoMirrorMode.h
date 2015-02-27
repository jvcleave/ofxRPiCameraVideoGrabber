#pragma once
#include "CameraDemo.h"


class DemoMirrorMode  : public CameraDemo
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
        info << "CURRENT MIRROR MODE: " << videoGrabber->getMirror() << "\n";
        info << "Press 1 for MIRROR_NONE"       << "\n";
        info << "Press 2 for MIRROR_VERTICAL"   << "\n";
        info << "Press 3 for MIRROR_HORIZONTAL" << "\n";
        info << "Press 4 for MIRROR_BOTH"       << "\n";
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
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_NONE);
                break;
            }
            case '2':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_VERTICAL);
                break;
            }
            case '3':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_HORIZONTAL);
                break;
            }
            case '4':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_BOTH);
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
