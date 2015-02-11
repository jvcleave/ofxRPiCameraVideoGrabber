#pragma once
#include "CameraDemo.h"


class DemoFeatureTest  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doDRC;
    bool doCrop;
    bool doHDR;
    int drcLevel;
    bool hdrState;
    bool doDigitalZoom;
    bool doZoomIn;
    bool doRandomZoom;
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
               
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
        doDrawInfo	= true;
        doDRC = false;
        doCrop = false;
        doHDR = false;
        hdrState = false;
        doRandomZoom = false;
        doDigitalZoom = false;
        doZoomIn = true;
        
        drcLevel = 0;
    };
    
    void update()
    {
        if (doDRC) 
        {
            if (drcLevel+1 <= 3) 
            {
                drcLevel++;
            }else
            {
                drcLevel = 0;
            }
            ofLogVerbose() << "drcLevel: " << drcLevel;
            videoGrabber->setDRC(drcLevel);
            doDRC = false;
        }
        if (doCrop) 
        {
            
            int randomPercentage = ofRandom(25, 100);
            
            //int randomHeight = ofRandom(videoGrabber->getHeight()/2, videoGrabber->getHeight());
            //int randomHeightPercentage = ofMap(randomHeight, 0, videoGrabber->getHeight(), 0, 100);
            
            videoGrabber->setSensorCrop(0, 0,  randomPercentage, randomPercentage);
            //doCrop = false;
        }
        if (doHDR) 
        {
            videoGrabber->setHDR(hdrState);
            doHDR = false;
            
        }
        if(doDigitalZoom)
        {
            if(doZoomIn)
            {
                videoGrabber->zoomIn();
            }else
            {
                videoGrabber->zoomOut();
            }
        }
        if (doRandomZoom) 
        {
            doRandomZoom = false;
            videoGrabber->setZoomLevelNormalized(ofRandom(0.0, 1.0f));
        }
    };
    
    void draw()
    {
        //draws at camera resolution
        videoGrabber->draw();
        
        //draw a smaller version via the getTextureReference() method
        int drawWidth = omxCameraSettings.width/4;
        int drawHeight = omxCameraSettings.height/4;
        videoGrabber->getTextureReference().draw(omxCameraSettings.width-drawWidth, omxCameraSettings.height-drawHeight, drawWidth, drawHeight);
        
        stringstream info;
        info << name << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n\n";

        info << "\n";
        info << "Crop LEFT %: "      << videoGrabber->getCropRectangle().getLeft()    <<  "\n";
        info << "Crop TOP %: "       << videoGrabber->getCropRectangle().getTop()     <<  "\n";
        info << "Crop WIDTH %: "     << videoGrabber->getCropRectangle().getWidth()   <<  "\n";
        info << "Crop HEIGHT %: "    << videoGrabber->getCropRectangle().getHeight()  <<  "\n";
        
        info << "\n";
        info << "Press a to increment DRC: "    << drcLevel << "\n";
        info << "Press c to randomize Crop" <<  "\n";
        info << "\n";
        info << "Press z to toggle Digital Zoom: " << doDigitalZoom <<  "\n";
        info << "Press s to toggle Digital Zoom Direction: " <<  doZoomIn << "\n";
        info << "Press r for random Zoom" <<  "\n";
        info << "getZoomLevelNormalized: " <<  videoGrabber->getZoomLevelNormalized() << "\n";
        info << "\n";
        
        info << "Press h to toggle HDR: " <<  hdrState  << "\n";
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
        if (key == 'a')
        {
            doDRC = true;
        }
        if (key == 'c')
        {
            doCrop = !doCrop;
        }
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        if (key == 'h') 
        {
            hdrState = !hdrState;
            doHDR = true;
        }
        if (key == 'z') 
        {
            doDigitalZoom = !doDigitalZoom;
        }
        if (key == 'r') 
        {
            doRandomZoom = true;
        }
        if (key == 's') {
            doZoomIn = !doZoomIn;
        }
        
    };
};
