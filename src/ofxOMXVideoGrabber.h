/*
 *  ofxOMXVideoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "OMXCameraController.h"
#include "ofAppEGLWindow.h"
#include "VideoEngine.h"


using namespace std;



class ofxOMXVideoGrabber : public OMXCameraController, public VideoEngineListener
{

public:
	

	ofxOMXVideoGrabber();
    ~ofxOMXVideoGrabber();
        
    void setup(ofxOMXCameraSettings&);
    int getWidth();
    int getHeight();
    int getFrameRate();
    bool isFrameNew();
    bool isTextureEnabled();	
    GLuint getTextureID();
    ofTexture& getTextureReference();
    
    bool isRecording();
    void startRecording();
    void stopRecording();
    void enablePixels();
    void disablePixels();
    unsigned char * getPixels();
    
    
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
    
    void onRecordingComplete(string filePath) override;
    void onVideoEngineStart() override;
    void onVideoEngineClose() override;
    
    VideoEngine engine;
    
private:
    bool hasNewFrame;
    
    int updateFrameCounter;
    int frameCounter;
    bool pixelsRequested;
	void onUpdate(ofEventArgs & args);
    


	
};
