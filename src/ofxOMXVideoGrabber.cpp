#include "ofxOMXVideoGrabber.h"

#pragma mark SETUP
ofxOMXVideoGrabber::ofxOMXVideoGrabber()
{
    resetValues();
	updateFrameCounter = 0;
	frameCounter = 0;
	hasNewFrame = false;
    camera = NULL;
	pixelsRequested = false;
	ofAddListener(ofEvents().update, this, &ofxOMXVideoGrabber::onUpdate);    
}

void ofxOMXVideoGrabber::reset()
{
    resetValues();
    //settings.resetValues();
    applyAllSettings();
}

void ofxOMXVideoGrabber::setup(ofxOMXCameraSettings& settings_)
{
    settings = settings_;
    ofLogNotice(__func__) << settings.toJSON().dump();
    ofLogNotice(__func__) << "settings: " << settings.toString();
    
    if(settings.enableTexture)
    {
        eglImageController.generateEGLImage(settings.width, settings.height);
        engine.setup(settings, this, &eglImageController);
        if(settings.enablePixels)
        {
            pixelsRequested = true;
        }
    }else
    {
        engine.setup(settings, this);

    }
    
}

void ofxOMXVideoGrabber::onVideoEngineStart()
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
    ofAddListener(ofEvents().update, this, &ofxOMXVideoGrabber::onUpdate);    
}

void ofxOMXVideoGrabber::onVideoEngineClose()
{
    ofLogVerbose(__func__) << endl;
}

void ofxOMXVideoGrabber::onRecordingComplete(string filePath)
{
    if(settings.videoGrabberListener)
    {
        settings.videoGrabberListener->onRecordingComplete(filePath);
    }else
    {
        ofLogWarning(__func__) << "RECEIVED " << filePath << " BUT NO LISTENER SET";
    }
}

bool ofxOMXVideoGrabber::isReady()
{
    return engine.isOpen;
}

#pragma mark UPDATE
void ofxOMXVideoGrabber::onUpdate(ofEventArgs & args)
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
            eglImageController.fbo.begin();
            ofClear(0, 0, 0, 0);
            eglImageController.texture.draw(0, 0);
			if (pixelsRequested) 
			{
                glReadPixels(0, 0,
                             eglImageController.texture.getWidth(),
                             eglImageController.texture.getHeight(),
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             eglImageController.pixels);    
			}
            eglImageController.fbo.end();
		}
	}
   
	//ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}



#pragma mark GETTERS

bool ofxOMXVideoGrabber::isFrameNew()
{
    return hasNewFrame;
}

bool ofxOMXVideoGrabber::isTextureEnabled()
{
    return settings.enableTexture;
}

int ofxOMXVideoGrabber::getWidth()
{
	return settings.width;
}

int ofxOMXVideoGrabber::getHeight()
{
	return settings.height;
}

int ofxOMXVideoGrabber::getFrameRate()
{
	return settings.framerate;
}

#pragma mark PIXELS/TEXTURE
GLuint ofxOMXVideoGrabber::getTextureID()
{
	return eglImageController.textureID;
}

void ofxOMXVideoGrabber::enablePixels()
{
    if(settings.enableTexture)
    {
        pixelsRequested = true;
    }
}

void ofxOMXVideoGrabber::disablePixels()
{
    if(settings.enableTexture)
    {
        pixelsRequested = false;
    }
}

unsigned char * ofxOMXVideoGrabber::getPixels()
{
    return eglImageController.pixels;
}

ofTexture& ofxOMXVideoGrabber::getTextureReference()
{
	return eglImageController.texture;
}

#pragma mark RECORDING


bool ofxOMXVideoGrabber::isRecording()
{
    return engine.isRecording;
}

void ofxOMXVideoGrabber::startRecording()
{
    engine.startRecording();
}

void ofxOMXVideoGrabber::stopRecording()
{
	engine.stopRecording();
}

#pragma mark DRAW
void ofxOMXVideoGrabber::setDisplayAlpha(int alpha)
{
    engine.directDisplay.setDisplayAlpha(alpha);
}

void ofxOMXVideoGrabber::setDisplayLayer(int layer)
{
    engine.directDisplay.setDisplayLayer(layer);
}

void ofxOMXVideoGrabber::setDisplayRotation(int rotationDegrees)
{
    engine.directDisplay.setDisplayRotation(rotationDegrees);
}

void ofxOMXVideoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    engine.directDisplay.setDisplayDrawRectangle(drawRectangle);

}

void ofxOMXVideoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    engine.directDisplay.setDisplayCropRectangle(cropRectangle);

}

void ofxOMXVideoGrabber::setDisplayMirror(bool doMirror)
{
    engine.directDisplay.setDisplayMirror(doMirror);
}

void ofxOMXVideoGrabber::draw()
{
	if (settings.enableTexture)
	{
		draw(0, 0);
        
	}else
    {
        engine.directDisplay.setDisplayDrawRectangle(ofRectangle(0, 0, getWidth(), getHeight()));
    }    
}
void ofxOMXVideoGrabber::draw(ofRectangle& rectangle)
{
    draw(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

void ofxOMXVideoGrabber::draw(int x, int y)
{
    if (settings.enableTexture)
    {
        eglImageController.fbo.draw(x, y);
    }else
    {
        setDisplayDrawRectangle(ofRectangle(x, y, getWidth(), getHeight()));

    }
}


void ofxOMXVideoGrabber::draw(int x, int y, int width, int height)
{
    if (settings.enableTexture)
    {
        eglImageController.fbo.draw(x, y, width, height);
    }else
    {
        setDisplayDrawRectangle(ofRectangle(x, y, width, height));
    }
}


#pragma mark EXIT

void ofxOMXVideoGrabber::close()
{
    
    cout << "ofxOMXVideoGrabber::close" << endl;
    ofRemoveListener(ofEvents().update, this, &ofxOMXVideoGrabber::onUpdate);
    engine.close();    
    cout << "~ofxOMXVideoGrabber::close END" << endl;
}

ofxOMXVideoGrabber::~ofxOMXVideoGrabber()
{
    cout << "~ofxOMXVideoGrabber" << endl;
    close();
}


