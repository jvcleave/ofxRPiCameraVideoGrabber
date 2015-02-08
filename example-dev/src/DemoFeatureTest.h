#pragma once
#include "CameraDemo.h"


class DemoFeatureTest  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doDRC;
    bool doROI;
    
    int drcLevel;
    
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
        doDrawInfo	= true;
        doDRC = false;
        doROI = false;
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
        if (doROI) 
        {
            
            int randomPercentage = ofRandom(25, 100);
            
            //int randomHeight = ofRandom(videoGrabber->getHeight()/2, videoGrabber->getHeight());
            //int randomHeightPercentage = ofMap(randomHeight, 0, videoGrabber->getHeight(), 0, 100);
            
            videoGrabber->setROI(0, 0,  randomPercentage, randomPercentage);
            //doROI = false;
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

        info << "Press a to increment DRC: "    << drcLevel << "\n";
        info << "Press r to randomize ROI" <<  "\n";
        info << "ROI LEFT %: "      << videoGrabber->getROIRectangle().getLeft()    <<  "\n";
        info << "ROI TOP %: "       << videoGrabber->getROIRectangle().getTop()     <<  "\n";
        info << "ROI WIDTH %: "     << videoGrabber->getROIRectangle().getWidth()   <<  "\n";
        info << "ROI HEIGHT %: "    << videoGrabber->getROIRectangle().getHeight()  <<  "\n";
        info << "Press g to Toggle info" << "\n";
        
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
            doROI = !doROI;
        }
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        
    };
};
