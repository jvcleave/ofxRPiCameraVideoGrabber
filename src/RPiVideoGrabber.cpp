//
//  RPiVideoGrabber.cpp
//  JVCRPI2_LOCAL
//
//  Created by jason van cleave on 12/20/15.
//  Copyright © 2015 jason van cleave. All rights reserved.
//

#include "RPiVideoGrabber.h"


RPiVideoGrabber::RPiVideoGrabber()
{
    cameraWidth = 0;
    cameraHeight = 0;
    didInit = false;
    hasNewFrame = true;
}


bool RPiVideoGrabber::setup(int w, int h)
{
    return initGrabber(w, h);
}

bool RPiVideoGrabber::initGrabber(int w, int h)
{
    
    
    omxCameraSettings.width = w; 
    omxCameraSettings.height = h;
    omxCameraSettings.enablePixels = true;
    cameraWidth = w;
    cameraHeight = h;
    
    omxCameraSettings.enableTexture = true; //default true
    videoGrabber.setup(omxCameraSettings);
    videoGrabber.enablePixels();
    
    pixels.setFromExternalPixels((unsigned char *)videoGrabber.getPixels(), cameraWidth, cameraHeight, OF_PIXELS_RGBA);

    didInit = true;
    return didInit;
}

bool RPiVideoGrabber::isFrameNew() const
{
    return hasNewFrame;
}

void RPiVideoGrabber::setDesiredFrameRate(int framerate)
{
    omxCameraSettings.framerate = framerate;
}

int RPiVideoGrabber::getFrameRate()
{
    return omxCameraSettings.framerate;
}

void RPiVideoGrabber::update()
{
    hasNewFrame = videoGrabber.isFrameNew();
}

float RPiVideoGrabber::getHeight() const
{
    return cameraHeight;
}

float RPiVideoGrabber::getWidth() const
{
    return cameraWidth;
}

void RPiVideoGrabber::draw(int x, int y)
{
    videoGrabber.getTextureReference().draw(x, y);
}

void RPiVideoGrabber::draw(int x, int y, int width, int height)
{
    videoGrabber.getTextureReference().draw(x, y, width, height);
}

ofTexture* RPiVideoGrabber::getTexturePtr()
{
    return &videoGrabber.textureEngine->fbo.getTexture(); 
}

bool RPiVideoGrabber::isInitialized() const
{
    return didInit;
}

bool RPiVideoGrabber::setPixelFormat(ofPixelFormat pixelFormat)
{
    if (pixelFormat == OF_PIXELS_RGBA) 
    {
        return true;
    }
    return false;
}

ofPixelFormat RPiVideoGrabber::getPixelFormat() const
{
    return OF_PIXELS_RGBA;
    
}

const ofPixels& RPiVideoGrabber::getPixels() const
{
    return pixels;
}

ofPixels& RPiVideoGrabber::getPixels()
{
    return pixels;
}

void RPiVideoGrabber::close()
{
    videoGrabber.close();
}

vector<ofVideoDevice> RPiVideoGrabber::listDevices() const
{
    ofLogError(__func__) << "you don't need to call this";
    return unused;
}

void RPiVideoGrabber::videoSettings()
{
    ofLogError(__func__) << "not used";
}

