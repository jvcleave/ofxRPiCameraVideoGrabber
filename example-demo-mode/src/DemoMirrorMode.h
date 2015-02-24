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
        info << "CURRENT MIRROR MODE: " << videoGrabber->getMirrorAsString() << "\n";
        info << "Press z for MIRROR_NONE"       << "\n";
        info << "Press x for MIRROR_VERTICAL"   << "\n";
        info << "Press c for MIRROR_HORIZONTAL" << "\n";
        info << "Press v for MIRROR_BOTH"       << "\n";
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
        
            case 'z':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_NONE);
                break;
            }
            case 'x':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_VERTICAL);
                break;
            }
            case 'c':
            {
                videoGrabber->setMirror(ofxRPiCameraVideoGrabber::MIRROR_HORIZONTAL);
                break;
            }
            case 'v':
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
