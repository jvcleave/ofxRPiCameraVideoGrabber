/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"


ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
    hasOMXInit = false;
    hasExitHandler  = false;
    
    updateFrameCounter = 0;
    frameCounter = 0;
    hasNewFrame = false;
    sessionConfig = NULL;
    camera = NULL;
    engine = NULL;
    appEGLWindow = NULL;
    pixels = NULL;
    eglImage = NULL;
    display = NULL;
    context = NULL;
    surface = NULL;
    eglConfig = NULL;
    doPixels = false;
    textureID	= -1;
    
    
    doSaveImage = false;
    doRawSave = false;
    doStartRecording = false;
    
    forceEGLReuse = true;
    
    fbo = NULL;
}


CameraSettings& ofxRPiCameraVideoGrabber::getCameraSettings()
{
    return sessionConfig->getCameraSettings();
}

void ofxRPiCameraVideoGrabber::setup(SessionConfig& sessionConfig_)
{
    setup(&sessionConfig_);
}
void ofxRPiCameraVideoGrabber::setup(SessionConfig* sessionConfig_)
{
   
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(engine)
    {
        delete engine;
        engine = NULL;
        camera = NULL;
        
    }
    if (appEGLWindow == NULL)
    {
        appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
    }
    if (display == NULL)
    {
        display = appEGLWindow->getEglDisplay();
    }
    if (context == NULL)
    {
        context = appEGLWindow->getEglContext();
    }
    if(surface == NULL)
    {
        surface = appEGLWindow->getEglSurface();
    }
    if(eglConfig == NULL)
    {
        eglConfig = appEGLWindow->getEglConfig();
    }

    /*
     In certain cases you can get away with not destroyEGLImage
     but texture allocation is fairly quick 
     1080P 77 milliseconds
     720P 50 milliseconds
     480P 10 milliseconds
     
     Not destroying was causing blank textures when switching modes
     */
    if(!forceEGLReuse)
    {
        destroyEGLImage();
    }
    
    
     
    addExitHandler();
    if(!hasOMXInit)
    {
        OMX_ERRORTYPE error = OMX_Init();
        OMX_TRACE(error);
        if (error == OMX_ErrorNone) 
        {
            hasOMXInit = true;
        }  
    }
    
    sessionConfig = sessionConfig_;
    sessionConfig->applyPreset();

    if (sessionConfig->enablePixels) 
    {
        enablePixels();
    }
    if (isTextureEnabled()) 
    {
        generateEGLImage(sessionConfig->width, sessionConfig->height);
    }
    engine = new CameraEngine();
    if (isTextureEnabled()) 
    {
        engine->eglImage = eglImage;
    }
    engine->setup(sessionConfig);
    camera = engine->camera;
    getCameraSettings().camera = camera;
    getCameraSettings().applyAllSettings();
    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
}

void ofxRPiCameraVideoGrabber::loadCameraSettingsFromFile(string filePath)
{
    getCameraSettings().loadFromFile(filePath);
}

void ofxRPiCameraVideoGrabber::saveCameraSettingsToFile(string filePath)
{
    getCameraSettings().saveToFile(filePath);    
}

CameraEngine* ofxRPiCameraVideoGrabber::getEngine()
{
    if (engine) 
    {
        return engine;
    }
    return NULL;
}


bool ofxRPiCameraVideoGrabber::isReady()
{
    if (engine) 
    {
        return engine->isOpen();
    }
    return false;
}

int ofxRPiCameraVideoGrabber::getWidth()
{
    return sessionConfig->width;
}

int ofxRPiCameraVideoGrabber::getHeight()
{
    return sessionConfig->height;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
    return sessionConfig->framerate;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
    if (!isTextureEnabled()) 
    {
      
        //ofLogWarning(__func__) << "You are in non-texture mode but asking for a texture";
    }
    return fbo->getTextureReference();
}



inline
void ofxRPiCameraVideoGrabber::generateEGLImage(int width, int height)
{
    int startTime = ofGetElapsedTimeMillis();
    bool needsRegeneration = false;
    
    if(!eglImage)
    {
        needsRegeneration = true;
    }
    if(!fbo)
    {
        needsRegeneration = true;
    }else
    {
        if (!fbo->isAllocated())
        {
            needsRegeneration = true;
        }
        
        if (fbo->getWidth() != width)
        {
            needsRegeneration = true;
        }
        if (fbo->getHeight() != height)
        {
            needsRegeneration = true;
        }

    }
        
    if(!needsRegeneration)
    {
        fbo->begin();
            ofClear(0, 0, 0);
        fbo->end();
        ofLogVerbose(__func__) << "NO CHANGES NEEDED - RETURNING EARLY";
        return;
    }
    
        
    if (eglImage && needsRegeneration)
    {
        destroyEGLImage();
    }
    
    if (needsRegeneration)
    {
        if(fbo)
        {
            delete fbo;
            fbo = NULL;
        }
        fbo = new ofFbo();
        fbo->allocate(width, height);
        textureID = fbo->getTextureReference().getTextureData().textureID;
    }
    
    if (pixels && needsRegeneration)
    {
        delete[] pixels;
        pixels = NULL;
    }
    
    if (pixels == NULL)
    {   int dataSize = width * height * 4;
        pixels = new unsigned char[dataSize];
    }
    
    fbo->bind();
       EGLint eglImageAttributes[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
                                        EGL_NONE};
        eglImage = eglCreateImageKHR(
                                 display,
                                 context,
                                 EGL_GL_TEXTURE_2D_KHR,
                                 (EGLClientBuffer)textureID,
                                 NULL);

        

        if (eglImage == EGL_NO_IMAGE_KHR)
        {
            ofLogError()	<< "Create EGLImage FAIL <---------------- :(";
            EGL_TRACE(eglGetError());
        }
        else
        {
            ofLogVerbose()	<< "Create EGLImage PASS <---------------- :)";
            
        }
    
    fbo->unbind();
    int endTime = ofGetElapsedTimeMillis();
    ofLogVerbose(__func__) << "TOOK " << endTime -startTime << " MILLISECONDS";
    
}

inline
void ofxRPiCameraVideoGrabber::destroyEGLImage()
{
    
    if (eglImage)
    {
        if (eglDestroyImageKHR(display, eglImage))
        {
            ofLogVerbose(__func__) << "eglDestroyImageKHR eglImage PASS <---------------- :)";
        }
        else
        {
            ofLogError(__func__) << "eglDestroyImageKHR eglImage FAIL <---------------- :(";
        }
        eglImage = NULL;
    }
}

bool ofxRPiCameraVideoGrabber::isTextureEnabled()
{
    return (sessionConfig->mode == SessionConfig::MODE_TEXTURE);
}




void ofxRPiCameraVideoGrabber::updatePixels()
{
    if (!doPixels && !doSaveImage) 
    {
        return;
    }
    int width = getWidth();
    int height = getHeight();
    int dataSize = width * height * 4;
    if (pixels == NULL)
    {
        pixels = new unsigned char[dataSize];
    }    
    fbo->bind();
        //eglFlushBRCM();
        //EGL_TRACE(eglGetError());

        glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    fbo->unbind();

    if(doSaveImage)
    {
        stringstream fileName;
        fileName << width;
        fileName << "px";
        fileName << "_x_";
        fileName << height;
        fileName << "px";
        fileName << "_4ch_";
        fileName << ofGetTimestampString();
        
        string imagePath;
        if(doRawSave)
        {
            imagePath = ofToDataPath(fileName.str()+".raw", true);
            ofBuffer buffer((const char*)pixels, dataSize);
            ofBufferToFile(imagePath, buffer, true);
            
            
        }else
        {
            imagePath = ofToDataPath(fileName.str()+".png", true);
            ofImage image;
            image.setUseTexture(false);
            image.setFromPixels(pixels, getWidth(), getHeight(), OF_IMAGE_COLOR_ALPHA);
            image.saveImage(imagePath);
        }
        
        ofLogVerbose(__func__) << "imagePath: " << imagePath;
        doRawSave = false;
        doSaveImage = false;
    }
}


void ofxRPiCameraVideoGrabber::enablePixels()
{
    doPixels = true;
}



void ofxRPiCameraVideoGrabber::disablePixels()
{
    doPixels = false;
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    return pixels;
}

void ofxRPiCameraVideoGrabber::stopRecording()
{
    if (engine) 
    {
        engine->stopRecording();
    }
}

void ofxRPiCameraVideoGrabber::startRecording()
{
    if(isRecording())
    {
        stopRecording();
    }
    //getCameraSettings().saveState();
    doStartRecording = true;
}

bool ofxRPiCameraVideoGrabber::isRecording()
{
    if (getEngine()) 
    {
        return getEngine()->isRecording();
    }
    return false;
}

void ofxRPiCameraVideoGrabber::saveImage()
{
    doRawSave = false;
    doSaveImage = true;
}

void ofxRPiCameraVideoGrabber::saveRawImage()
{
    ofLogVerbose(__func__) << "";
    doRawSave = true;
    doSaveImage = true;
}

inline
void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
    if(doStartRecording)
    {
        ofLogVerbose(__func__) << "doStartRecording REQUESTED";
        sessionConfig->doRecording = true;
        setup(sessionConfig);
        doStartRecording = false;
    }else
    {
        //ofLogVerbose(__func__) << doStartRecording;

        if (engine) 
        {
            frameCounter  = engine->getFrameCounter();
        }
        
       if(isTextureEnabled())
       {
           if (frameCounter > updateFrameCounter) 
           {
               updateFrameCounter = frameCounter;
               hasNewFrame = true;
               
           }else
           {
               hasNewFrame = false;
           } 
       }else
       {
           //no way of currently telling with non-texure - assuming true
           hasNewFrame = true;
       }
        
        if (hasNewFrame) 
        {
            updatePixels();
        }
    }
        //ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}



bool ofxRPiCameraVideoGrabber::isFrameNew()
{
    return hasNewFrame;
}

void ofxRPiCameraVideoGrabber::draw()
{
    if (!isTextureEnabled())
    {
        return;
    }
    fbo->draw(0, 0);
}


bool ofxRPiCameraVideoGrabber::doExit = false;

void ofxRPiCameraVideoGrabber::signal_handler(int signum)
{
    ofxRPiCameraVideoGrabber::doExit = true;
}

inline
void ofxRPiCameraVideoGrabber::onUpdateDuringExit(ofEventArgs& args)
{
    if (ofxRPiCameraVideoGrabber::doExit)
    {
        ofLogVerbose(__func__) << " EXITING VIA SIGNAL";
        close();
        ofExit();
    }
}

inline
void ofxRPiCameraVideoGrabber::addExitHandler()
{
    if(hasExitHandler) return;
    
    vector<int> signals;
    signals.push_back(SIGINT);
    signals.push_back(SIGQUIT);
    
    for (size_t i=0; i<signals.size(); i++)
    {
        int SIGNAL_TO_BLOCK = signals[i];
        //http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
        
        //Struct for the new action associated to the SIGNAL_TO_BLOCK
        struct sigaction new_action;
        new_action.sa_handler = ofxRPiCameraVideoGrabber::signal_handler;
        
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
    hasExitHandler = true;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    close();
    if (pixels) 
    {
        delete[] pixels;
        pixels = NULL;
    }
    if(sessionConfig)
    {
        sessionConfig = NULL;
    }
    if(fbo)
    {
        delete fbo;
        fbo = NULL;
    }
}

void ofxRPiCameraVideoGrabber::close()
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(engine)
    {
        delete engine;
        engine = NULL;
    }
    destroyEGLImage();
}




