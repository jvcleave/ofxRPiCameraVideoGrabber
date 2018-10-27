#include "ofxRPiCameraPhotoGrabber.h"


ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    resetValues();
    engine = NULL;
}

void ofxRPiCameraPhotoGrabber::setup(OMXCameraSettings& omxCameraSettings_)
{
    settings = omxCameraSettings_;
    if(engine)
    {
        delete engine;
    }
    engine = new StillCameraEngine();
    engine->setup(settings);
    camera = engine->camera;
    
}

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
        setup(settings);
    }

}

int ofxRPiCameraPhotoGrabber::getWidth()
{
    return settings.width;
}

int ofxRPiCameraPhotoGrabber::getHeight()
{
    return settings.height;
}





ofxRPiCameraPhotoGrabber::~ofxRPiCameraPhotoGrabber()
{
    if(engine)
    {
        delete engine;
    }
}





