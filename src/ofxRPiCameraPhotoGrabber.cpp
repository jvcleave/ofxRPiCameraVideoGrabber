#include "ofxRPiCameraPhotoGrabber.h"


ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    resetValues();
}

void ofxRPiCameraPhotoGrabber::setup(OMXCameraSettings& omxCameraSettings_)
{
    settings = omxCameraSettings_;
    listener = settings.photoGrabberListener;
    ofLogNotice(__func__) << settings.toString();
   
    engine.setup(settings, this);
    camera = engine.camera;
    
}
void ofxRPiCameraPhotoGrabber::onPhotoEngineStart(OMX_HANDLETYPE camera_)
{
    camera = camera_;
    applyAllSettings();

}


void ofxRPiCameraPhotoGrabber::onPhotoEngineClose()
{
    ofLogNotice(__func__) << endl;
    //setup(settings);
}



bool ofxRPiCameraPhotoGrabber::isReady()
{
    return engine.isOpen();
}

void ofxRPiCameraPhotoGrabber::takePhoto()
{    
    engine.takePhoto();
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
    engine.directDisplay.setDisplayAlpha(alpha);
}

void ofxRPiCameraPhotoGrabber::setDisplayLayer(int layer)
{
    engine.directDisplay.setDisplayAlpha(layer);
}

void ofxRPiCameraPhotoGrabber::setDisplayRotation(int rotationDegrees)
{
    engine.directDisplay.setDisplayRotation(rotationDegrees);
}

void ofxRPiCameraPhotoGrabber::setDisplayDrawRectangle(ofRectangle drawRectangle)
{
    engine.directDisplay.setDisplayDrawRectangle(drawRectangle);
}

void ofxRPiCameraPhotoGrabber::setDisplayCropRectangle(ofRectangle cropRectangle)
{
    engine.directDisplay.setDisplayCropRectangle(cropRectangle);
}

void ofxRPiCameraPhotoGrabber::setDisplayMirror(bool doMirror)
{
    engine.directDisplay.setDisplayMirror(doMirror);
}


ofxRPiCameraPhotoGrabber::~ofxRPiCameraPhotoGrabber()
{
    listener = NULL;
}





