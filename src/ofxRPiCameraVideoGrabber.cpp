#include "ofxRPiCameraVideoGrabber.h"

#pragma mark SETUP
ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
    resetValues();
	updateFrameCounter = 0;
	frameCounter = 0;
	hasNewFrame = false;
	textureEngine = NULL;
	directEngine = NULL;
    camera = NULL;
	pixelsRequested = false;
    recordingRequested = false;
	ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    
}

void ofxRPiCameraVideoGrabber::reset()
{
    resetValues();
    //settings.resetValues();
    applyAllSettings();
}

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings& omxCameraSettings_)
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

    OMX_ERRORTYPE error = OMX_ErrorNone;
    settings = omxCameraSettings_;
    
    
    ofLog() << settings.toJSON().dump();

    ofLogVerbose(__func__) << "settings: " << settings.toString();
    
    if(directEngine)
    {
        delete directEngine;
        directEngine = NULL;
        camera = NULL;
        ofLogVerbose() << "deleted directEngine";
        resetValues();
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
        camera = NULL;
        ofLogVerbose() << "deleted textureEngine";
        resetValues();
    }
    
    if (settings.enableTexture) 
    {
        
        textureEngine = new TextureEngine(); 
        textureEngine->setup(settings);
        camera = textureEngine->camera;
        if (settings.enablePixels) 
        {
            enablePixels();
        }
    }else 
    {
        
        directEngine = new DirectEngine(); 
        directEngine->setup(settings);
        camera = directEngine->camera;
    }
    
    checkBurstMode();
    error = applyExposure(__func__);
    OMX_TRACE(error);
    checkFlickerCancellation();
    applyAllSettings();
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

}



bool ofxRPiCameraVideoGrabber::isReady()
{
    
    if (directEngine) 
    {
        return directEngine->isOpen;
    }
    
    if (textureEngine) 
    {
        return textureEngine->isOpen;
    }
    
    return false;
}

#pragma mark UPDATE
void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
	if(textureEngine)
	{
		frameCounter  = textureEngine->getFrameCounter();
		
	}else
	{
		if (directEngine) 
		{
			frameCounter  = directEngine->getFrameCounter();
		}
	}
	
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
		if (textureEngine) 
		{
			if (pixelsRequested) 
			{
				textureEngine->updatePixels();
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
    bool result = false;
    if(textureEngine)
    {
        result = true;
    }else
    {
        result = false;
    }
    return result;
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
	if (!textureEngine) 
	{
        ofLogError(__func__) << "TEXTURE NOT ENABLED - RETURN UNALLOCATED TEXTURE ID";
        return errorTexture.texData.textureID;
	}
	return textureEngine->getTexture().texData.textureID;
}

void ofxRPiCameraVideoGrabber::enablePixels()
{
    if(textureEngine)
    {
        textureEngine->enablePixels();
        pixelsRequested = true;
    }
}

void ofxRPiCameraVideoGrabber::disablePixels()
{
    if(textureEngine)
    {
        textureEngine->disablePixels();
        pixelsRequested = false;
    }
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    unsigned char * pixels = NULL;
    if (textureEngine) {
        pixels = textureEngine->getPixels();
    }
    return pixels;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	if (!textureEngine) 
	{
		ofLogError(__func__) << "TEXTURE NOT ENABLED - RETURN UNALLOCATED TEXTURE";
        return errorTexture;
	}
	return textureEngine->getTexture();
}

#pragma mark RECORDING


bool ofxRPiCameraVideoGrabber::isRecording()
{
   
    if (!directEngine && !textureEngine)
    {
        return false;
    }
    
    bool result = false;
    if (directEngine) 
    {
        result = directEngine->isThreadRunning();
    }
    if (textureEngine) 
    {
       result =  textureEngine->isThreadRunning();
    }
    return result;
    
    
}

void ofxRPiCameraVideoGrabber::startRecording()
{
    if (!directEngine && !textureEngine)
    {
        return;
    }
    bool isCurrentlyRecording = isRecording();
    ofLogVerbose(__func__) << "isCurrentlyRecording: " << isCurrentlyRecording;
    if(!isCurrentlyRecording)
    {
        recordingRequested = true;
    }
    

}


void ofxRPiCameraVideoGrabber::stopRecording()
{
	if (directEngine) 
	{
		directEngine->stopRecording();
        /*
         
         direct mode has to use a lower resolution for display while recording
         set it back after recording
         */
        settings.doRecording = false;
        setup(settings);
	}
	if (textureEngine) 
	{
		textureEngine->stopRecording();
	}
}

#pragma mark DRAW
void ofxRPiCameraVideoGrabber::setDisplayAlpha(int alpha)
{
    if(directEngine)
    {
        getDisplayManager()->options.alpha = alpha;
        getDisplayManager()->applyConfig();
    }
}

void ofxRPiCameraVideoGrabber::setDisplayLayer(int layer)
{
    if(directEngine)
    {
        getDisplayManager()->options.layer = layer;
        getDisplayManager()->applyConfig();
    }
}



void ofxRPiCameraVideoGrabber::setDisplayRotation(int rotationDegrees)
{
    if(directEngine)
    {
        getDisplayManager()->rotateDisplay(rotationDegrees);
        getDisplayManager()->applyConfig();

    }
}

void ofxRPiCameraVideoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    if(directEngine)
    {
        getDisplayManager()->options.drawRectangle = drawRectangle;
        getDisplayManager()->applyConfig();

        
    }
}

void ofxRPiCameraVideoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    if(directEngine)
    {
        getDisplayManager()->options.cropRectangle = cropRectangle;
        getDisplayManager()->applyConfig();

        
    }
}

void ofxRPiCameraVideoGrabber::setDisplayMirror(bool doMirror)
{
    if(directEngine)
    {
        getDisplayManager()->options.doMirror = doMirror;
        getDisplayManager()->applyConfig();

    }
}

DirectDisplay* ofxRPiCameraVideoGrabber::getDisplayManager()
{
    if(directEngine)
    {
        return &directEngine->displayManager;
    }
    
    return NULL;
}


void ofxRPiCameraVideoGrabber::draw()
{
	if (textureEngine)
	{
		textureEngine->getTexture().draw(0, 0);
        return;
	}
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(0, 0, getWidth(), getHeight()));
    }
    
	
}
void ofxRPiCameraVideoGrabber::draw(ofRectangle& rectangle)
{
    draw(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

void ofxRPiCameraVideoGrabber::draw(int x, int y)
{
    if (textureEngine)
    {
        textureEngine->getTexture().draw(x, y);
        return;
    }
    
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(x, y, getWidth(), getHeight()));
    }
}


void ofxRPiCameraVideoGrabber::draw(int x, int y, int width, int height)
{
    if (textureEngine)
    {
        textureEngine->getTexture().draw(x, y, width, height);
        return;
    }
    
    
    if (directEngine)
    {
        setDisplayDrawRectangle(ofRectangle(x, y, width, height));
    }
}


#pragma mark EXIT

void ofxRPiCameraVideoGrabber::close()
{
    
    cout << "ofxRPiCameraVideoGrabber::close" << endl;
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(directEngine)
    {
        cout << "~ofxRPiCameraVideoGrabber delete directEngine" << endl;
        delete directEngine;
        directEngine = NULL;
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
    }
    
    cout << "~ofxRPiCameraVideoGrabber::close END" << endl;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    cout << "~ofxRPiCameraVideoGrabber" << endl;
    close();
}


