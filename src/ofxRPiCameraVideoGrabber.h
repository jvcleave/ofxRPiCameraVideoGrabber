/*
 *  ofxRPiCameraVideoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "RPICameraController.h"

#include "ofAppEGLWindow.h"



#include "TextureEngine.h"
#include "DirectEngine.h"


using namespace std;


class ofxRPiCameraVideoGrabber : public RPICameraController
{

public:
	

	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
    
    //CameraMetering metering;
    
    void setup(OMXCameraSettings);
    void setup(CameraState);
    int getWidth();
    int getHeight();
    int getFrameRate();
    bool isFrameNew();
    bool isTextureEnabled();	
    GLuint getTextureID();
    ofTexture& getTextureReference();
    
    bool recordingRequested;
    bool isRecording();
    void startRecording();
    void stopRecording();
    void enablePixels();
    void disablePixels();
    unsigned char * getPixels();
    
    TextureEngine* textureEngine;
    DirectEngine* directEngine;
    
    bool isReady();
    
    void draw();
    void draw(int x, int y);
	void draw(int x, int y, int width, int height);
    void draw(ofRectangle&);
    
    void reset();
    void close();
    
    void setDisplayAlpha(int);
    void setDisplayLayer(int);
    void setDisplayRotation(int);
    void setDisplayDrawRectangle(ofRectangle);
    void setDisplayCropRectangle(ofRectangle);
    void setDisplayMirror(bool);
    
    DirectDisplay* getDisplayManager();

private:
    ofTexture errorTexture;
    bool hasNewFrame;
    

    
    int updateFrameCounter;
    int frameCounter;
    bool pixelsRequested;


	void onUpdate(ofEventArgs & args);
    


	
};
