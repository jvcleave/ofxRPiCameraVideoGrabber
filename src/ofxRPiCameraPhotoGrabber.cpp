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
    getDisplayManager()->options.alpha = alpha;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayLayer(int layer)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->options.layer = layer;
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

    getDisplayManager()->options.drawRectangle = drawRectangle;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->options.cropRectangle = cropRectangle;
    getDisplayManager()->applyConfig();
}

void ofxRPiCameraPhotoGrabber::setDisplayMirror(bool doMirror)
{
    if(!getDisplayManager()) return;

    getDisplayManager()->options.doMirror = doMirror;
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





