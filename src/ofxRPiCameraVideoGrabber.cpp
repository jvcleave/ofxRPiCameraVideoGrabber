#include "ofxRPiCameraVideoGrabber.h"

#pragma mark SETUP
ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
    resetValues();
	updateFrameCounter = 0;
	frameCounter = 0;
	hasNewFrame = false;
    camera = NULL;
	pixelsRequested = false;
    recordingRequested = false;
	ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
}

void ofxRPiCameraVideoGrabber::reset()
{
    resetValues();
    settings.resetValues();
    applyAllSettings();
}

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings& omxCameraSettings_)
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
    
    ofLog() << "ofRemoveListener PASS";
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    settings = omxCameraSettings_;
    
    
    ofLog() << settings.toJSON().dump();

    ofLogVerbose(__func__) << "settings: " << settings.toString();
    if(videoEngine.isOpen)
    {
        videoEngine.close();
    }
    videoEngine.setup(settings, this);
    camera = videoEngine.camera;
    resetValues();
    
    
    //checkBurstMode();
    error = applyExposure(__func__);
    OMX_TRACE(error);
    checkFlickerCancellation();
    applyAllSettings();
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

}



bool ofxRPiCameraVideoGrabber::isReady()
{
    return videoEngine.isOpen;
}

#pragma mark UPDATE
void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
    
    frameCounter = videoEngine.getFrameCounter();
	if (frameCounter > updateFrameCounter) 
	{
		updateFrameCounter = frameCounter;
		hasNewFrame = true;
		
	}else
	{
		hasNewFrame = false;
	}
	if (hasNewFrame) 
	{
		if (settings.enableTexture) 
		{
			if (pixelsRequested) 
			{
				videoEngine.updatePixels();
			}
		}
	}
    if (recordingRequested) 
    {
        recordingRequested = false;
        settings.doRecording = true;
        setup(settings);
    }
	//ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}



#pragma mark GETTERS

bool ofxRPiCameraVideoGrabber::isFrameNew()
{
    return hasNewFrame;
}

bool ofxRPiCameraVideoGrabber::isTextureEnabled()
{
    return settings.enableTexture;
}

int ofxRPiCameraVideoGrabber::getWidth()
{
	return settings.width;
}

int ofxRPiCameraVideoGrabber::getHeight()
{
	return settings.height;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
	return settings.framerate;
}

#pragma mark PIXELS/TEXTURE
GLuint ofxRPiCameraVideoGrabber::getTextureID()
{
	return videoEngine.getTexture().texData.textureID;
}

void ofxRPiCameraVideoGrabber::enablePixels()
{
    if(settings.enableTexture)
    {
        videoEngine.enablePixels();
        pixelsRequested = true;
    }
}

void ofxRPiCameraVideoGrabber::disablePixels()
{
    if(settings.enableTexture)
    {
        videoEngine.disablePixels();
        pixelsRequested = false;
    }
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    return videoEngine.getPixels();
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	return videoEngine.getTexture();
}

#pragma mark RECORDING


bool ofxRPiCameraVideoGrabber::isRecording()
{
    return videoEngine.isRecording;
}

void ofxRPiCameraVideoGrabber::startRecording()
{
    bool isCurrentlyRecording = isRecording();
    ofLogVerbose(__func__) << "isCurrentlyRecording: " << isCurrentlyRecording;
    if(!isCurrentlyRecording)
    {
        recordingRequested = true;
    }
}


void ofxRPiCameraVideoGrabber::onRecordingComplete(string filePath)
{
    if(settings.videoGrabberListener)
    {
        settings.videoGrabberListener->onRecordingComplete(filePath);
    }else
    {
        ofLogWarning(__func__) << "RECEIVED " << filePath << " BUT NO LISTENER SET";
    }
    
#if 0
    if (directEngine) 
    {
        /* 
         direct mode has to use a lower resolution for display while recording
         set it back after recording
         */
        settings.doRecording = false;
        setup(settings);
    }
#endif   
    
}

void ofxRPiCameraVideoGrabber::stopRecording()
{
	videoEngine.stopRecording();
}

#pragma mark DRAW
void ofxRPiCameraVideoGrabber::setDisplayAlpha(int alpha)
{
    videoEngine.setDisplayAlpha(alpha);
}

void ofxRPiCameraVideoGrabber::setDisplayLayer(int layer)
{
    videoEngine.setDisplayLayer(layer);
}

void ofxRPiCameraVideoGrabber::setDisplayRotation(int rotationDegrees)
{
    videoEngine.setDisplayRotation(rotationDegrees);
}

void ofxRPiCameraVideoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    videoEngine.setDisplayDrawRectangle(drawRectangle);

}

void ofxRPiCameraVideoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    videoEngine.setDisplayCropRectangle(cropRectangle);

}

void ofxRPiCameraVideoGrabber::setDisplayMirror(bool doMirror)
{
    videoEngine.setDisplayMirror(doMirror);
}

void ofxRPiCameraVideoGrabber::draw()
{
	if (settings.enableTexture)
	{
		videoEngine.getTexture().draw(0, 0);
	}else
    {
        videoEngine.setDisplayDrawRectangle(ofRectangle(0, 0, getWidth(), getHeight()));
    }    
}
void ofxRPiCameraVideoGrabber::draw(ofRectangle& rectangle)
{
    draw(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

void ofxRPiCameraVideoGrabber::draw(int x, int y)
{
    if (settings.enableTexture)
    {
        videoEngine.getTexture().draw(x, y);
    }else
    {
        setDisplayDrawRectangle(ofRectangle(x, y, getWidth(), getHeight()));

    }
}


void ofxRPiCameraVideoGrabber::draw(int x, int y, int width, int height)
{
    if (settings.enableTexture)
    {
        videoEngine.getTexture().draw(x, y, width, height);
    }else
    {
        setDisplayDrawRectangle(ofRectangle(x, y, width, height));
    }
}


#pragma mark EXIT

void ofxRPiCameraVideoGrabber::close()
{
    
    cout << "ofxRPiCameraVideoGrabber::close" << endl;
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    videoEngine.close();    
    cout << "~ofxRPiCameraVideoGrabber::close END" << endl;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    cout << "~ofxRPiCameraVideoGrabber" << endl;
    close();
}


