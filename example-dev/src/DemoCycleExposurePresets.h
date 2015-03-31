#pragma once
#include "CameraDemo.h"


class DemoCycleExposurePresets  : public CameraDemo
{
    
public:
    
    string currentExposureName;	
    vector<string> exposurePresetNames;

    bool doDrawInfo;
    int exposurePresetIndex;
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(videoGrabber_);
        doDrawInfo	= true;
        exposurePresetIndex = 0;
        currentExposureName = "";
        //set local alias to silly static map
        ;
        exposurePresetNames = ExposurePresetNames();
    };
    
    void update()
    {
        if (ofGetFrameNum() % 100 == 0) 
        {
            if((unsigned int) exposurePresetIndex+1 < exposurePresetNames.size())
            {
                exposurePresetIndex++;
            }else
            {
                exposurePresetIndex = 0;
            }
            currentExposureName = exposurePresetNames[exposurePresetIndex];
            
            videoGrabber->cameraSettings.setExposurePreset(ExposurePresets[currentExposureName]);
            //videoGrabber->printMeteringMode();
            //ofLogVerbose() << "currentExposureName: " << currentExposureName;
        }
    };
    
    void draw()
    {
        //draws at camera resolution
        videoGrabber->draw();
        
        //draw a smaller version via the getTextureReference() method
        int drawWidth = videoGrabber->getWidth()/4;
        int drawHeight = videoGrabber->getHeight()/4;
        videoGrabber->getTextureReference().draw(videoGrabber->getWidth()-drawWidth, 
                                                 videoGrabber->getHeight()-drawHeight, 
                                                 drawWidth, 
                                                 drawHeight);
        
        stringstream info;
        info << name << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n";
        info << "getSharpness(): "      << videoGrabber->cameraSettings.getSharpness()              << "\n";
        info << "getContrast(): "       << videoGrabber->cameraSettings.getContrast()               << "\n";
        info << "getBrightness(): "     << videoGrabber->cameraSettings.getBrightness()             << "\n";
        info << "getSaturation(): "      << videoGrabber->cameraSettings.getSaturation()             << "\n";
        
        info << "\n";
        info << "currentExposureName: " << currentExposureName << "\n";
        
        info << "Press z to toggle autoShutter: " << videoGrabber->getAutoShutter() << "\n";
        info << "Press x to toggle autoISO: "     << videoGrabber->getAutoISO()   <<"\n";
        info << "Press c to toggle autoAperture: " << videoGrabber->getAutoAperture()   <<"\n";
               
        info << "\n";
        info << "Press e to increment filter" << "\n";
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
        if(key == 'z')
        {
            videoGrabber->cameraSettings.setAutoShutter(!videoGrabber->getAutoShutter());
        }
        if(key == 'x')
        {
            videoGrabber->cameraSettings.setAutoISO(!videoGrabber->getAutoISO());
        }
        if(key == 'c')
        {
            videoGrabber->cameraSettings.setAutoAperture(!videoGrabber->getAutoAperture());
        }
        
        
        

    };
};
