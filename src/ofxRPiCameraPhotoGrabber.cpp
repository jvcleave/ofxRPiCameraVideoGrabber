#include "ofxRPiCameraPhotoGrabber.h"


ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    hasOMXInit = false;
    hasExitHandler  = false;
    
    sessionConfig = NULL;
    camera = NULL;
    engine = NULL;
}


CameraSettings& ofxRPiCameraPhotoGrabber::getCameraSettings()
{
    return sessionConfig->getCameraSettings();
}

void ofxRPiCameraPhotoGrabber::setup(SessionConfig& sessionConfig_)
{
    setup(&sessionConfig_);
}
void ofxRPiCameraPhotoGrabber::setup(SessionConfig* sessionConfig_)
{
   
 
    if(engine)
    {
        delete engine;
        engine = NULL;
        camera = NULL;
        
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
    engine = new StillCameraEngine();
    engine->setup(sessionConfig);
    camera = engine->camera;
    getCameraSettings().camera = camera;
    getCameraSettings().applyAllSettings();
    
}

void ofxRPiCameraPhotoGrabber::loadCameraSettingsFromFile(string filePath)
{
    getCameraSettings().loadFromFile(filePath);
}

void ofxRPiCameraPhotoGrabber::saveCameraSettingsToFile(string filePath)
{
    getCameraSettings().saveToFile(filePath);    
}

StillCameraEngine* ofxRPiCameraPhotoGrabber::getEngine()
{
    if (engine) 
    {
        return engine;
    }
    return NULL;
}


bool ofxRPiCameraPhotoGrabber::isReady()
{
    if (engine) 
    {
        return engine->isOpen();
    }
    return false;
}

int ofxRPiCameraPhotoGrabber::getWidth()
{
    return sessionConfig->width;
}

int ofxRPiCameraPhotoGrabber::getHeight()
{
    return sessionConfig->height;
}



bool ofxRPiCameraPhotoGrabber::doExit = false;

void ofxRPiCameraPhotoGrabber::signal_handler(int signum)
{
    ofxRPiCameraPhotoGrabber::doExit = true;
}

inline
void ofxRPiCameraPhotoGrabber::onUpdateDuringExit(ofEventArgs& args)
{
    if (ofxRPiCameraPhotoGrabber::doExit)
    {
        ofLogVerbose(__func__) << " EXITING VIA SIGNAL";
        close();
        ofExit();
    }
}

inline
void ofxRPiCameraPhotoGrabber::addExitHandler()
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
        new_action.sa_handler = ofxRPiCameraPhotoGrabber::signal_handler;
        
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
    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraPhotoGrabber::onUpdateDuringExit);
    hasExitHandler = true;
}

ofxRPiCameraPhotoGrabber::~ofxRPiCameraPhotoGrabber()
{
    close();
    if(sessionConfig)
    {
        sessionConfig = NULL;
    }
}

void ofxRPiCameraPhotoGrabber::close()
{
    if(engine)
    {
        delete engine;
        engine = NULL;
    }
}


#pragma mark INTERFACE TO CAMERA SETTINGS
bool ofxRPiCameraPhotoGrabber::setAutoAperture(bool b)
{
    return getCameraSettings().setAutoAperture(b) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setMeteringMode(CameraMeteringMode m )
{
    return getCameraSettings().setMeteringMode(m) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setMeteringType(OMX_METERINGTYPE m )
{
    return getCameraSettings().setMeteringType(m) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setMeteringType(string s)
{
    return getCameraSettings().setMeteringType(s) == OMX_ErrorNone;
}

string ofxRPiCameraPhotoGrabber::getMeteringType()
{
    return getCameraSettings().getMeteringType();
}

bool ofxRPiCameraPhotoGrabber::setAutoShutter(bool b)
{
    return getCameraSettings().setAutoShutter(b) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::getAutoShutter()
{
    return getCameraSettings().getAutoShutter();
}

int ofxRPiCameraPhotoGrabber::getShutterSpeed()
{
    return getCameraSettings().getShutterSpeed();
}

bool ofxRPiCameraPhotoGrabber::setShutterSpeed(int shutterSpeedMicroSeconds)
{ 
    return getCameraSettings().setShutterSpeed(shutterSpeedMicroSeconds) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setEvCompensation(int n)
{
    return getCameraSettings().setEvCompensation(n) == OMX_ErrorNone;
}

int ofxRPiCameraPhotoGrabber::getEvCompensation()
{
    return getCameraSettings().getEvCompensation();
}

bool ofxRPiCameraPhotoGrabber::setImageFilter(OMX_IMAGEFILTERTYPE t)
{
    return getCameraSettings().setImageFilter(t) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setImageFilter(string t)
{
    return getCameraSettings().setImageFilter(t) == OMX_ErrorNone;
}

string ofxRPiCameraPhotoGrabber::getImageFilter()
{
    return getCameraSettings().getImageFilter();
}

int ofxRPiCameraPhotoGrabber::getSharpness()
{
    return getCameraSettings().getSharpness();
}

bool ofxRPiCameraPhotoGrabber::setSharpness(int n)
{
    return getCameraSettings().setSharpness(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setSharpnessNormalized(float f)
{
    return getCameraSettings().setSharpnessNormalized(f) == OMX_ErrorNone;
}


int ofxRPiCameraPhotoGrabber::getContrast()
{
    return getCameraSettings().getContrast();
}

bool ofxRPiCameraPhotoGrabber::setContrast(int n)
{
    return getCameraSettings().setContrast(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setContrastNormalized(float f)
{
    return getCameraSettings().setContrastNormalized(f) == OMX_ErrorNone;
}


int ofxRPiCameraPhotoGrabber::getBrightness()
{
    return getCameraSettings().getBrightness();
}

bool ofxRPiCameraPhotoGrabber::setBrightness(int n)
{
    return getCameraSettings().setBrightness(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setBrightnessNormalized(float f)
{
    return getCameraSettings().setBrightnessNormalized(f) == OMX_ErrorNone;
}


int ofxRPiCameraPhotoGrabber::getSaturation()
{
    return getCameraSettings().getSaturation();
}

bool ofxRPiCameraPhotoGrabber::setSaturation(int n)
{
    return getCameraSettings().setSaturation(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setSaturationNormalized(float f)
{
    return getCameraSettings().setSaturationNormalized(f) == OMX_ErrorNone;
}


bool ofxRPiCameraPhotoGrabber::getFrameStabilization()
{
    return getCameraSettings().framestabilization;
}

bool ofxRPiCameraPhotoGrabber::setFrameStabilization(bool b)
{
    return getCameraSettings().setFrameStabilization(b) == OMX_ErrorNone;
}


string ofxRPiCameraPhotoGrabber::getExposurePreset()
{
    return getCameraSettings().getExposurePreset();
}

bool ofxRPiCameraPhotoGrabber::setExposurePreset(OMX_EXPOSURECONTROLTYPE t)
{
    return getCameraSettings().setExposurePreset(t) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setExposurePreset(string s)
{
    return getCameraSettings().setExposurePreset(s) == OMX_ErrorNone;
}

string ofxRPiCameraPhotoGrabber::getWhiteBalance()
{
    return getCameraSettings().getWhiteBalance();
}

bool ofxRPiCameraPhotoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE t)
{
    return getCameraSettings().setWhiteBalance(t) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setWhiteBalance(string s)
{
    return getCameraSettings().setWhiteBalance(s) == OMX_ErrorNone;
}

//int getDRE();
bool ofxRPiCameraPhotoGrabber::setDRE(int n)
{
    return getCameraSettings().setDRE(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setSensorCrop(ofRectangle& rectangle)
{
    return getCameraSettings().setSensorCrop(rectangle) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setSensorCrop(int left, int top, int width, int height)
{
    return getCameraSettings().setSensorCrop(left, top, width, height) == OMX_ErrorNone;
}

ofRectangle& ofxRPiCameraPhotoGrabber::getCropRectangle()
{
    return getCameraSettings().getCropRectangle();
}

bool ofxRPiCameraPhotoGrabber::updateSensorCrop()
{
    return getCameraSettings().updateSensorCrop() == OMX_ErrorNone;
}


bool ofxRPiCameraPhotoGrabber::zoomIn()
{
    return getCameraSettings().zoomIn() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::zoomOut()
{
    return getCameraSettings().zoomOut() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::resetZoom()
{
    return getCameraSettings().resetZoom() == OMX_ErrorNone;
}


bool ofxRPiCameraPhotoGrabber::setZoomLevelNormalized(float f)
{
    return getCameraSettings().setZoomLevelNormalized(f) == OMX_ErrorNone;
}

float ofxRPiCameraPhotoGrabber::getZoomLevelNormalized()
{
    return getCameraSettings().getZoomLevelNormalized();
}


bool ofxRPiCameraPhotoGrabber::setRotation(int n)
{
    return getCameraSettings().setRotation(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setRotation(CameraSettings::ROTATION r)
{
    return getCameraSettings().setRotation(r) == OMX_ErrorNone;
}

int ofxRPiCameraPhotoGrabber::getRotation()
{
    return getCameraSettings().getRotation();
}


bool ofxRPiCameraPhotoGrabber::rotateClockwise()
{
    return getCameraSettings().rotateClockwise() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::rotateCounterClockwise()
{
    return getCameraSettings().rotateCounterClockwise() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setMirror(int n)
{
    return getCameraSettings().setMirror(n) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::setMirror(string s)
{
    return getCameraSettings().setMirror(s) == OMX_ErrorNone;
}

string ofxRPiCameraPhotoGrabber::getMirror()
{
    return getCameraSettings().getMirror();
}

bool ofxRPiCameraPhotoGrabber::setSoftwareSharpening(bool b)
{
    return getCameraSettings().setSoftwareSharpening(b) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::enableSoftwareSharpening()
{
    return getCameraSettings().enableSoftwareSharpening() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::disableSoftwareSharpening()
{
    return getCameraSettings().disableSoftwareSharpening() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::isSoftwareSharpeningEnabled()
{
    return getCameraSettings().isSoftwareSharpeningEnabled();
}


CameraSettings::EXPOSURE_MODE ofxRPiCameraPhotoGrabber::getExposureMode()
{
    return getCameraSettings().getExposureMode();
}


bool ofxRPiCameraPhotoGrabber::enableAutoExposure()
{
    return getCameraSettings().enableAutoExposure() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::enableManualExposure()
{
    return getCameraSettings().enableManualExposure() == OMX_ErrorNone;
}


bool ofxRPiCameraPhotoGrabber::setSoftwareSaturation(bool b)
{
    return getCameraSettings().setSoftwareSaturation(b) == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::enableSoftwareSaturation()
{
    return getCameraSettings().enableSoftwareSaturation() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::disableSoftwareSaturation()
{
    return getCameraSettings().disableSoftwareSaturation() == OMX_ErrorNone;
}

bool ofxRPiCameraPhotoGrabber::isSoftwareSaturationEnabled()
{
    return getCameraSettings().isSoftwareSaturationEnabled();
}




