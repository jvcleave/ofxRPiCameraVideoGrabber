#pragma once
#include "CameraDemo.h"


class DemoManualMode  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doAutoMode = false;
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
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
        int drawWidth = omxCameraSettings.width/4;
        int drawHeight = omxCameraSettings.height/4;
        videoGrabber->getTextureReference().draw(omxCameraSettings.width-drawWidth, omxCameraSettings.height-drawHeight, drawWidth, drawHeight);
        
        stringstream info;
        info << name << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n";
        info << videoGrabber->meteringModetoString() << "\n";
        info << "Press e to increment filter" << "\n";
        info << "Press g to Toggle info" << "\n";
        
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
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
