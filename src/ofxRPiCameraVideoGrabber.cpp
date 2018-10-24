#include "ofxRPiCameraVideoGrabber.h"

#pragma mark SETUP
ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
	OMX_Maps::getInstance(); 
    resetValues();
    
    int zoomStepsSource[61] = 
    {
        65536,  68157,  70124,  72745,
        75366,  77988,  80609,  83231,
        86508,  89784,  92406,  95683,
        99615,  102892, 106168, 110100,
        114033, 117965, 122552, 126484,
        131072, 135660, 140247, 145490,
        150733, 155976, 161219, 167117,
        173015, 178913, 185467, 192020,
        198574, 205783, 212992, 220201,
        228065, 236585, 244449, 252969,
        262144, 271319, 281149, 290980,
        300810, 311951, 322437, 334234,
        346030, 357827, 370934, 384041,
        397148, 411566, 425984, 441057,
        456131, 472515, 488899, 506593,
        524288
    };
    vector<int> converted(zoomStepsSource, 
                          zoomStepsSource + sizeof zoomStepsSource / sizeof zoomStepsSource[0]);
    zoomLevels = converted;

    
    cropRectangle.set(0,0,100,100);

    hasAddedExithandler = false;
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
    applyAllSettings();
}

void ofxRPiCameraVideoGrabber::setup(CameraState cameraState)
{
    setup(cameraState.cameraSettings);
    map<string, string> keyValueMap = cameraState.keyValueMap;
    for(auto iterator  = keyValueMap.begin(); iterator != keyValueMap.end(); iterator++) 
    {
        string key = iterator->first;
        string value = iterator->second;
        //ofLogVerbose(__func__) << "key: " << key << " value: " << value;

        if(key == "sharpness")  setSharpness(ofToInt(value));
        if(key == "contrast")   setContrast(ofToInt(value));
        if(key == "brightness") setBrightness(ofToInt(value));
        if(key == "saturation") setSaturation(ofToInt(value));
        if(key == "ISO")        setISO(ofToInt(value));
        if(key == "AutoISO")    setAutoISO(ofToBool(value));
        if(key == "DRE")        setDRE(ofToInt(value));
        if(key == "cropRectangle") 
        {
            vector<string> rectValues = ofSplitString(value, ",");
            if(rectValues.size() == 4)             
            {
                setSensorCrop(ofToInt(rectValues[0]),
                                            ofToInt(rectValues[1]),
                                            ofToInt(rectValues[2]),
                                            ofToInt(rectValues[3])); 
            }
        }
        if(key == "zoomLevelNormalized")    setZoomLevelNormalized(ofToFloat(value));
        if(key == "mirror")                 setMirror(value);
        if(key == "rotation")               setRotation(ofToInt(value));
        if(key == "imageFilter")            setImageFilter(value);
        if(key == "exposurePreset")         setExposurePreset(value);
        if(key == "evCompensation")         setEvCompensation(ofToInt(value));
        if(key == "autoShutter")            setAutoShutter(ofToBool(value));
        if(key == "shutterSpeed")           setShutterSpeed(ofToInt(value));
        if(key == "meteringType")           setMeteringType(value);
        
        if(key == "SoftwareSaturationEnabled") setSoftwareSaturation(ofToBool(value));
        if(key == "SoftwareSharpeningEnabled") setSoftwareSharpening(ofToBool(value));
    }
}

void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings omxCameraSettings_)
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);    

    OMX_ERRORTYPE error = OMX_ErrorNone;
    error = OMX_Init();
    OMX_TRACE(error);
    
    omxCameraSettings = omxCameraSettings_;
    
    ofLogVerbose(__func__) << "omxCameraSettings: " << omxCameraSettings.toString();

    
    
    if(!hasAddedExithandler)
    {
        addExitHandler();
        hasAddedExithandler = true; 
    }
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
    
    if (omxCameraSettings.enableTexture) 
    {
        
        textureEngine = new TextureEngine(); 
        textureEngine->setup(omxCameraSettings);
        camera = textureEngine->camera;
        if (omxCameraSettings.enablePixels) 
        {
            enablePixels();
        }
    }else 
    {
        
        directEngine = new DirectEngine(); 
        directEngine->setup(omxCameraSettings);
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
        CameraState currentState = getCameraState();
        currentState.cameraSettings.doRecording = true;
        //ofLogVerbose() << "CALLING SETUP: " << currentStateToString();
        setup(currentState);
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
	return omxCameraSettings.width;
}

int ofxRPiCameraVideoGrabber::getHeight()
{
	return omxCameraSettings.height;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
	return omxCameraSettings.framerate;
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
        CameraState currentState = getCameraState();
        currentState.cameraSettings.doRecording = false;
        setup(currentState);
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

bool doExit = false;
void signal_handler(int signum)
{
    cout << "ofxRPiCameraVideoGrabber caught signal " << signum;
    doExit = true;
}

void ofxRPiCameraVideoGrabber::onUpdateDuringExit(ofEventArgs& args)
{
    if (doExit)
    {
        ofLogVerbose(__func__) << " EXITING VIA SIGNAL";
        close();
        ofExit();
    }
}

void ofxRPiCameraVideoGrabber::addExitHandler()
{
    
    vector<int> signals;
    signals.push_back(SIGINT);
    signals.push_back(SIGQUIT);
    
    for (size_t i=0; i<signals.size(); i++)
    {
        int SIGNAL_TO_BLOCK = signals[i];
        //http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
        
        //Struct for the new action associated to the SIGNAL_TO_BLOCK
        struct sigaction new_action;
        new_action.sa_handler = signal_handler;
        
        //Empty the sa_mask. This means that no signal is blocked while the signal_handler runs.
        sigemptyset(&new_action.sa_mask);
        
        //Block the SEGTERM signal so while the signal_handler runs, the SIGTERM signal is ignored
        sigaddset(&new_action.sa_mask, SIGTERM);
        
        //Remove any flag from sa_flag. See documentation for flags allowed
        new_action.sa_flags = 0;
        
        struct sigaction old_action;
        //Read the old signal associated to SIGNAL_TO_BLOCK
        sigaction(SIGNAL_TO_BLOCK, NULL, &old_action);
        
        //If the old handler wasn't SIG_IGN it is a handler that just "ignores" the signal
        if (old_action.sa_handler != SIG_IGN)
        {
            //Replace the signal handler of SIGNAL_TO_BLOCK with the one described by new_action
            sigaction(SIGNAL_TO_BLOCK, &new_action, NULL);
        }
        
    }
    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdateDuringExit);
}

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


