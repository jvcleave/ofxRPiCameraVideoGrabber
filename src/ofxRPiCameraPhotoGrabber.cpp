#include "ofxRPiCameraPhotoGrabber.h"


ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    resetValues();
    engine = NULL;
}

void ofxRPiCameraPhotoGrabber::setup(OMXCameraSettings& omxCameraSettings_, bool doApplySettings)
{
    settings = omxCameraSettings_;
    listener = settings.photoGrabberListener;
    ofLog() << settings.toString();
    if(engine)
    {
        delete engine;
    }
    engine = new StillCameraEngine();
    engine->listener = this;
    engine->setup(settings);
    camera = engine->camera;
    if(doApplySettings)
    {
        resetValues();
        applyAllSettings();
    }
    
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
        ofLog() << "PRE RESET: " << settings.toString();
        //settings = engine->settings;
        engine->closeEngine();
        setup(settings, true);
        

    }

}

void ofxRPiCameraPhotoGrabber::onTakePhotoComplete(string filePath)
{
    photosTaken.push_back(filePath);
    if(listener)
    {
        
        listener->onTakePhotoComplete(filePath);
    }else
    {
        ofLogWarning(__func__) << filePath << " WRITTEN BUT NO LISTENER SET";
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


void ofxRPiCameraPhotoGrabber::draw(ofRectangle& rectangle)
{
    draw(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}

void ofxRPiCameraPhotoGrabber::draw(int x, int y, int width, int height)
{
    setDisplayDrawRectangle(ofRectangle(x, y, width, height));

}

void ofxRPiCameraPhotoGrabber::setDisplayAlpha(int alpha)
{
    if(!getDisplayManager()) return;
    getDisplayManager()->alpha = alpha;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayLayer(int layer)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->layer = layer;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayRotation(int rotationDegrees)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->rotateDisplay(rotationDegrees);
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->drawRectangle = drawRectangle;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->cropRectangle = cropRectangle;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayMirror(bool doMirror)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->doMirror = doMirror;
    getDisplayManager()->applyConfig();
}

DirectDisplay* ofxRPiCameraPhotoGrabber::getDisplayManager()
{
    if(!engine && !engine->isOpen())
    {
        return NULL;
    }
    
    return engine->getDisplayManager();
}


ofxRPiCameraPhotoGrabber::~ofxRPiCameraPhotoGrabber()
{
    if(engine)
    {
        delete engine;
    }
}





