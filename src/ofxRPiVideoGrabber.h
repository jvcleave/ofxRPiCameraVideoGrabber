#pragma once 

#include "ofMain.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ofBaseTypes.h"


class ofxRPiVideoGrabber: public ofBaseVideoGrabber
{
public:
    ofxRPiVideoGrabber();
    
    bool setup(int w, int h);
    bool initGrabber(int w, int h);
    bool isFrameNew() const;
    int getFrameRate();
    void update();
    float getHeight() const;
    float getWidth() const;
    ofTexture* getTexturePtr();
    vector<ofVideoDevice>	listDevices() const;
    void setDesiredFrameRate(int framerate);
    void close();
    bool setPixelFormat(ofPixelFormat pixelFormat);
    ofPixelFormat getPixelFormat() const;
    ofPixels&		getPixels();
    const ofPixels& getPixels() const;
    void videoSettings();
    bool isInitialized() const;
    
    void draw(int x, int y);
    
    ofxRPiCameraVideoGrabber videoGrabber;
    
    float cameraWidth;
    float cameraHeight;
    bool didInit;
    mutable ofPixelFormat internalPixelFormat;
    ofPixels pixels;
    
    bool hasNewFrame;
    
    OMXCameraSettings omxCameraSettings;
};