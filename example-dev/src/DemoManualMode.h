#pragma once
#include "CameraDemo.h"


class DemoManualMode  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doAutoMode = false;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doDrawInfo	= true;
        
    };
    
    void update()
    {
        if (ofGetFrameNum() % 100 == 0) 
        {
            doAutoMode = !doAutoMode;
            videoGrabber->setAutoAperture(doAutoMode);
            videoGrabber->setAutoShutter(doAutoMode);
            videoGrabber->setAutoSensitivity(doAutoMode);
        }
    };
    
    void draw()
    {
        //draws at camera resolution
        videoGrabber->draw();
        
        //draw a smaller version via the getTextureReference() method
        int drawWidth = videoGrabber->getWidth()/4;
        int drawHeight = videoGrabber->getHeight()/4;
        videoGrabber->getTextureReference().draw(videoGrabber->getWidth()-drawWidth, videoGrabber->getHeight()-drawHeight, drawWidth, drawHeight);
        
        stringstream info;
        info << name << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n";
        
        info << "\n";
        info << videoGrabber->meteringModetoString() << "\n";
        
        info << "\n";
        info << "Press e to increment filter" << "\n";
        info << "Press g to Toggle info" << "\n";
        info << "Press SPACE for next Demo" << "\n";
        
        infoString = info.str();
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(infoString, 100, 100, ofColor::black, ofColor::yellow);
        }
        
    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        if (key == 'e')
        {
            videoGrabber->applyImageFilter(filterCollection.getNextFilter());
        }
        
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        
    };
};
