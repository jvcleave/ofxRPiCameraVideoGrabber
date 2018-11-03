#include "ofxRPiCameraPhotoGrabber.h"

int shotsRequested;
int shotsTaken;

ofxRPiCameraPhotoGrabber::ofxRPiCameraPhotoGrabber()
{
    camera = NULL;
    shotsRequested = 0;
    shotsTaken = 0;
}

int totalTime = 0;

void ofxRPiCameraPhotoGrabber::setup(OMXCameraSettings omxCameraSettings_)
{
    settings = omxCameraSettings_;
    listener = settings.photoGrabberListener;
    
    
    
    
    ofLogNotice(__func__) << settings.toString();
   
    engine.setup(&settings, this); //wait until onPhotoEngineStart to do anything   

}


void ofxRPiCameraPhotoGrabber::onPhotoEngineStart(OMX_HANDLETYPE camera_)
{
    camera = camera_;
    
    applyAllSettings();
    ofLogNotice(__func__) << "shotsRequested: " << shotsRequested << " shotsTaken: " << shotsTaken;
    if(shotsTaken)
    {
        //setBurstMode(settings.burstModeEnabled);
        if(shotsTaken < shotsRequested)
        {
            takePhoto();
        }else
        {
            ofLog() << shotsTaken << " TOOK totalTime: " << totalTime << " EACH " << totalTime/shotsTaken;
        }
    }
}


bool ofxRPiCameraPhotoGrabber::isReady()
{
    return engine.isOpen();
}


int photoStart = 0;
void ofxRPiCameraPhotoGrabber::takePhoto(int numShots)
{   
    shotsRequested+=numShots;
    if(camera)
    {
        photoStart = ofGetElapsedTimeMillis();
        engine.takePhoto();
    }
    
    camera = NULL;
}

void ofxRPiCameraPhotoGrabber::onTakePhotoComplete(string filePath)
{
    
    int end = ofGetElapsedTimeMillis();
    int total = end-photoStart;
    ofLog() << "PHOTO TOOK: " <<  total << "MS";
    totalTime +=total;
    photoStart = 0;
    
    photosTaken.push_back(filePath);
    shotsTaken++;
    ofLogNotice(__func__) << "shotsRequested: " << shotsRequested << " shotsTaken: " << shotsTaken;

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





