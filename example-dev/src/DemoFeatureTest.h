#pragma once
#include "CameraDemo.h"


class DemoFeatureTest  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doDRE;
    bool doCrop;
    bool doHDR;
    int dreLevel;
    bool hdrState;
    bool doDigitalZoom;
    bool doZoomIn;
    bool doRandomZoom;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
               
        CameraDemo::setup( videoGrabber_);
        doDrawInfo	= true;
        doDRE = false;
        doCrop = false;
        doHDR = false;
        hdrState = false;
        doRandomZoom = false;
        doDigitalZoom = false;
        doZoomIn = true;
        
        dreLevel = 0;
    };
    
    void update()
    {
        if (doDRE) 
        {
            if (dreLevel+1 <= 3) 
            {
                dreLevel++;
            }else
            {
                dreLevel = 0;
            }
            ofLogVerbose() << "dreLevel: " << dreLevel;
            videoGrabber->cameraSettings.setDRE(dreLevel);
            doDRE = false;
        }
        if (doCrop) 
        {
            
            int randomPercentage = ofRandom(25, 100);
            
            //int randomHeight = ofRandom(videoGrabber->getHeight()/2, videoGrabber->getHeight());
            //int randomHeightPercentage = ofMap(randomHeight, 0, videoGrabber->getHeight(), 0, 100);
            
            videoGrabber->cameraSettings.setSensorCrop(0, 0,  randomPercentage, randomPercentage);
            //doCrop = false;
        }
        if (doHDR) 
        {
            videoGrabber->cameraSettings.setHDR(hdrState);
            doHDR = false;
            
        }
        if(doDigitalZoom)
        {
            if(doZoomIn)
            {
                videoGrabber->cameraSettings.zoomIn();
            }else
            {
                videoGrabber->cameraSettings.zoomOut();
            }
        }
        if (doRandomZoom) 
        {
            doRandomZoom = false;
            videoGrabber->cameraSettings.setZoomLevelNormalized(ofRandom(0.0, 1.0f));
        }
    };
    
    void draw()
    {
        //draws at camera resolution
        videoGrabber->draw();
        
        //draw a smaller version via the getTextureReference() method
        int drawWidth = videoGrabber->getWidth()/4;
        int drawHeight = videoGrabber->getHeight()/4;
        videoGrabber->getTextureReference().draw(videoGrabber->getWidth()-drawWidth, videoGrabber->getHeight()-drawHeight, drawWidth, drawHeight);
        
        stringstream info;
        info << name << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n\n";

        info << "\n";
        info << "Crop LEFT %: "      << videoGrabber->cameraSettings.getCropRectangle().getLeft()    <<  "\n";
        info << "Crop TOP %: "       << videoGrabber->cameraSettings.getCropRectangle().getTop()     <<  "\n";
        info << "Crop WIDTH %: "     << videoGrabber->cameraSettings.getCropRectangle().getWidth()   <<  "\n";
        info << "Crop HEIGHT %: "    << videoGrabber->cameraSettings.getCropRectangle().getHeight()  <<  "\n";
        
        info << "\n";
        info << "Press z to increment DRC: "    << dreLevel << "\n";
        info << "Press x to randomize Crop" <<  "\n";
        info << "\n";
        info << "Press c to toggle Digital Zoom: " << doDigitalZoom <<  "\n";
        info << "Press v to toggle Digital Zoom Direction: " <<  doZoomIn << "\n";
        info << "Press b for random Zoom" <<  "\n";
        info << "getZoomLevelNormalized: " <<  videoGrabber->cameraSettings.getZoomLevelNormalized() << "\n";
        info << "\n";
        
        info << "Press n to toggle HDR: " <<  hdrState  << "\n";
        info << "Press g to Toggle info" << "\n";
        
        
        info << "Press SPACE for next Demo" << "\n";
        
        infoString = info.str();
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(infoString, 100, 100, ofColor::black, ofColor::yellow);
        }
    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        if (key == 'z')
        {
            doDRE = true;
        }
        if (key == 'x')
        {
            doCrop = !doCrop;
        }
        if (key == 'c') 
        {
            doDigitalZoom = !doDigitalZoom;
        }
        if (key == 'v') {
            doZoomIn = !doZoomIn;
        }
        if (key == 'b') 
        {
            doRandomZoom = true;
        }
        if (key == 'n') 
        {
            hdrState = !hdrState;
            doHDR = true;
        }
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
    };
};
