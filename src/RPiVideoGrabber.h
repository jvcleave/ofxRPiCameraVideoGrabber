#pragma once 

#include "ofxRPiCameraVideoGrabber.h"


class RPiVideoGrabber: public ofBaseVideoGrabber
{
public:
    RPiVideoGrabber();
    
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
    void draw(int x, int y, int width, int height); 
    ofxRPiCameraVideoGrabber videoGrabber;
    
    float cameraWidth;
    float cameraHeight;
    bool didInit;
    ofPixels pixels;
    
    bool hasNewFrame;
    
    OMXCameraSettings omxCameraSettings;
    
    vector<ofVideoDevice> unused;
};