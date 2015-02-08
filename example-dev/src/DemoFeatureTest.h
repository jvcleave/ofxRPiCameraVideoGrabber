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
    int zoomLevel;
    vector<int> zoomSteps;
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        
        int zoomStepsSource[61] = {65536, 68157, 70124, 72745,
            75366, 77988, 80609, 83231,
            86508, 89784, 92406, 95683,
            99615, 102892, 106168, 110100,
            114033, 117965, 122552, 126484,
            131072, 135660, 140247, 145490,
            150733, 155976, 161219, 167117,
            173015, 178913, 185467, 192020,
            198574, 205783, 212992, 220201,
            228065, 236585, 244449, 252969,
            262144, 271319, 281149, 290980,
            300810, 311951, 322437, 334234,
            346030, 357827, 370934, 384041,
            397148, 411566, 425984, 441057,
            456131, 472515, 488899, 506593,
            524288};

        vector<int> converted(zoomStepsSource, zoomStepsSource + sizeof zoomStepsSource / sizeof zoomStepsSource[0]);
        zoomSteps = converted;
        
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
        doDrawInfo	= true;
        doDRC = false;
        doCrop = false;
        doHDR = false;
        hdrState = false;
        doDigitalZoom = false;
        drcLevel = 0;
        zoomLevel = 0;
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
            if ((unsigned int)zoomLevel+1< zoomSteps.size()) 
            {
                zoomLevel++;
            }else
            {
                zoomLevel = 0;
            }
            videoGrabber->setDigitalZoom(zoomSteps[zoomLevel]);
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
        info << "Press r to randomize Crop" <<  "\n";
        info << "Press z to toggle Digital Zoom" <<  "\n";
        info << "Press h to toggle HDR: " <<  hdrState  << "\n";
        info << "Press g to Toggle info" << "\n";
        
        
        info << "Press SPACE for next Demo" << "\n";
        
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
        }
    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        if (key == 'a')
        {
            doDRC = true;
        }
        if (key == 'r')
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
        
    };
};
