//
//  ofxRPiVideoGrabber.cpp
//  JVCRPI2_LOCAL
//
//  Created by jason van cleave on 12/20/15.
//  Copyright © 2015 jason van cleave. All rights reserved.
//

#include "ofxRPiVideoGrabber.h"


ofxRPiVideoGrabber::ofxRPiVideoGrabber()
{
    cameraWidth = 0;
    cameraHeight = 0;
    didInit = false;
    hasNewFrame = true;
}


bool ofxRPiVideoGrabber::setup(int w, int h)
{
    return initGrabber(w, h);
}
bool ofxRPiVideoGrabber::initGrabber(int w, int h)
{
    
    
    omxCameraSettings.width = w; 
    omxCameraSettings.height = h;
    omxCameraSettings.enablePixels = true;
    cameraWidth = w;
    cameraHeight = h;
    
    omxCameraSettings.isUsingTexture = true; //default true
    videoGrabber.setup(omxCameraSettings);
    videoGrabber.enablePixels();
    
    pixels.allocate(cameraWidth, cameraHeight, OF_PIXELS_RGBA);
    didInit = true;
    return didInit;
}

bool ofxRPiVideoGrabber::isFrameNew() const
{
    
    return hasNewFrame;
}
int ofxRPiVideoGrabber::getFrameRate()
{
    return omxCameraSettings.framerate;
}
void ofxRPiVideoGrabber::update()
{
    hasNewFrame = videoGrabber.isFrameNew();
    pixels.setFromPixels((unsigned char *)videoGrabber.getPixels(), cameraWidth, cameraHeight, OF_PIXELS_RGBA);

}

float ofxRPiVideoGrabber::getHeight() const
{
    return cameraHeight;
}

float ofxRPiVideoGrabber::getWidth() const
{
    return cameraWidth;
}
void ofxRPiVideoGrabber::draw(int x, int y)
{
    videoGrabber.getTextureReference().draw(x, y);
}
ofTexture* ofxRPiVideoGrabber::getTexturePtr()
{
    return nullptr; 
}
vector<ofVideoDevice> ofxRPiVideoGrabber::listDevices() const
{
    vector<ofVideoDevice> result;
    return result;
}
void ofxRPiVideoGrabber::setDesiredFrameRate(int framerate)
{
    omxCameraSettings.framerate = framerate;
}
void ofxRPiVideoGrabber::videoSettings()
{

}

bool ofxRPiVideoGrabber::isInitialized() const
{
    return didInit;
}

bool ofxRPiVideoGrabber::setPixelFormat(ofPixelFormat pixelFormat)
{
    return false;
}

ofPixelFormat ofxRPiVideoGrabber::getPixelFormat() const
{
    return internalPixelFormat;
    
}

const ofPixels& ofxRPiVideoGrabber::getPixels() const
{
    return pixels;
}

ofPixels& ofxRPiVideoGrabber::getPixels()
{
    return pixels;
}

void ofxRPiVideoGrabber::close()
{
    
}
