#include "ofxRPiCameraPhotoGrabber.h"


ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    OMX_Init();
    OMX_Maps::getInstance(); 
    resetValues();
    engine = NULL;
}

void ofxRPiCameraPhotoGrabber::setup()
{
    if(engine)
    {
        delete engine;
    }
    engine = new StillCameraEngine();
    engine->setup();
    camera = engine->camera;
    
}
/*
void ofxRPiCameraPhotoGrabber::loadCameraSettingsFromFile(string filePath)
{
    getCameraSettings().loadFromFile(filePath);
}

void ofxRPiCameraPhotoGrabber::saveCameraSettingsToFile(string filePath)
{
    getCameraSettings().saveToFile(filePath);    
}
*/


bool ofxRPiCameraPhotoGrabber::isReady()
{
    if(engine)
    {
        return engine->isOpen();
    }
    return false;
}

void ofxRPiCameraPhotoGrabber::takePhoto()
{
    if(!engine) return;
    
    if(engine->takePhoto())
    {
        ofLog() << "TAKE PHOTO SUCCESS";
    }else
    {
        ofLogError() << "TAKE PHOTO FAILED";
        engine->closeEngine();
        setup();
    }

}

int ofxRPiCameraPhotoGrabber::getWidth()
{
    if(!engine) return 0;

    return engine->frameWidth;
}

int ofxRPiCameraPhotoGrabber::getHeight()
{
    if(!engine) return 0;

    return engine->frameHeight;
}





ofxRPiCameraPhotoGrabber::~ofxRPiCameraPhotoGrabber()
{
    if(engine)
    {
        delete engine;
    }
}


#if 0
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
#endif



