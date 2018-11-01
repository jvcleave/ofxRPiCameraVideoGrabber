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
    settings = omxCameraSettings_;
    ofLogNotice(__func__) << settings.toJSON().dump();
    ofLogNotice(__func__) << "settings: " << settings.toString();
    engine.setup(settings, this);
}

void ofxRPiCameraVideoGrabber::onVideoEngineStart()
{
    ofLogVerbose(__func__) << endl;

    OMX_ERRORTYPE error = OMX_ErrorNone;
    camera = engine.camera;
    resetValues();
    
    //checkBurstMode();
    error = applyExposure();
    OMX_TRACE(error);
    
    //checkFlickerCancellation();
    
    applyAllSettings();
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
}

void ofxRPiCameraVideoGrabber::onVideoEngineClose()
{
    ofLogVerbose(__func__) << endl;
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
}

bool ofxRPiCameraVideoGrabber::isReady()
{
    return engine.isOpen;
}

#pragma mark UPDATE
void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
    
    frameCounter = engine.getFrameCounter();
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
				engine.updatePixels();
			}
		}
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
	return engine.getTexture().texData.textureID;
}

void ofxRPiCameraVideoGrabber::enablePixels()
{
    if(settings.enableTexture)
    {
        engine.enablePixels();
        pixelsRequested = true;
    }
}

void ofxRPiCameraVideoGrabber::disablePixels()
{
    if(settings.enableTexture)
    {
        engine.disablePixels();
        pixelsRequested = false;
    }
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    return engine.getPixels();
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	return engine.getTexture();
}

#pragma mark RECORDING


bool ofxRPiCameraVideoGrabber::isRecording()
{
    return engine.isRecording;
}

void ofxRPiCameraVideoGrabber::startRecording()
{
    engine.startRecording();
}

void ofxRPiCameraVideoGrabber::stopRecording()
{
	engine.stopRecording();
}

#pragma mark DRAW
void ofxRPiCameraVideoGrabber::setDisplayAlpha(int alpha)
{
    engine.directDisplay.setDisplayAlpha(alpha);
}

void ofxRPiCameraVideoGrabber::setDisplayLayer(int layer)
{
    engine.directDisplay.setDisplayLayer(layer);
}

void ofxRPiCameraVideoGrabber::setDisplayRotation(int rotationDegrees)
{
    engine.directDisplay.setDisplayRotation(rotationDegrees);
}

void ofxRPiCameraVideoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    engine.directDisplay.setDisplayDrawRectangle(drawRectangle);

}

void ofxRPiCameraVideoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    engine.directDisplay.setDisplayCropRectangle(cropRectangle);

}

void ofxRPiCameraVideoGrabber::setDisplayMirror(bool doMirror)
{
    engine.directDisplay.setDisplayMirror(doMirror);
}

void ofxRPiCameraVideoGrabber::draw()
{
	if (settings.enableTexture)
	{
		engine.getTexture().draw(0, 0);
	}else
    {
        engine.directDisplay.setDisplayDrawRectangle(ofRectangle(0, 0, getWidth(), getHeight()));
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
        engine.getTexture().draw(x, y);
    }else
    {
        setDisplayDrawRectangle(ofRectangle(x, y, getWidth(), getHeight()));

    }
}


void ofxRPiCameraVideoGrabber::draw(int x, int y, int width, int height)
{
    if (settings.enableTexture)
    {
        engine.getTexture().draw(x, y, width, height);
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
    engine.close();    
    cout << "~ofxRPiCameraVideoGrabber::close END" << endl;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    cout << "~ofxRPiCameraVideoGrabber" << endl;
    close();
}


