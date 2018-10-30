/*
 *  ofxRPiCameraVideoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "RPICameraController.h"
#include "ofAppEGLWindow.h"
#include "VideoModeEngine.h"


using namespace std;



class ofxRPiCameraVideoGrabber : public RPICameraController, public RecordingListener
{

public:
	

	ofxRPiCameraVideoGrabber();
    ~ofxRPiCameraVideoGrabber();
        
    void setup(OMXCameraSettings&);
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
    
    VideoModeEngine videoEngine;
    
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
    void onRecordingComplete(string filePath) override;
    
private:
    ofTexture errorTexture;
    bool hasNewFrame;
    

    
    int updateFrameCounter;
    int frameCounter;
    bool pixelsRequested;


	void onUpdate(ofEventArgs & args);
    


	
};
