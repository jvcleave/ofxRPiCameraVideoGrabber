#pragma once

#include "ofMain.h"
//#include "ofAppEGLWindow.h"
#include "ofxOMXVideoGrabber.h"
#include "ImageFilterCollection.h"

class CameraDemo
{
public:
    
    ofxOMXVideoGrabber* videoGrabber;
    ImageFilterCollection filterCollection;
    
    string name;
    string infoString;
    CameraDemo()
    {
        name = "";
        infoString = "";
        filterCollection.setup();
    }
    
    void setup(ofxOMXVideoGrabber* videoGrabber_)
    {
        videoGrabber = videoGrabber_;
    }
    virtual void update()=0;
    virtual void draw()=0;
    virtual void onKey(int key)=0;
};
