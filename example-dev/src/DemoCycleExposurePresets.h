#pragma once
#include "CameraDemo.h"


class DemoCycleExposurePresets  : public CameraDemo
{
    
public:
    
    string currentExposureName;	
    vector<string> exposurePresetNames;

    bool doDrawInfo;
    int exposurePresetIndex;
    
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
        doDrawInfo	= true;
        exposurePresetIndex = 0;
        currentExposureName = "";
        //set local alias to silly static map
        ;
        exposurePresetNames = OMX_Maps::getInstance().getExposurePresetNames();
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
            
            videoGrabber->setExposurePreset(OMX_Maps::getInstance().exposurePresets[currentExposureName]);
            videoGrabber->printMeteringMode();
            //ofLogVerbose() << "currentExposureName: " << currentExposureName;
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
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n";
        info << "CURRENT FILTER: "      << filterCollection.getCurrentFilterName()  << "\n";
        info << "getSharpness(): "      << videoGrabber->getSharpness()              << "\n";
        info << "getContrast(): "       << videoGrabber->getContrast()               << "\n";
        info << "getBrightness(): "     << videoGrabber->getBrightness()             << "\n";
        info << "getSaturation(): "      << videoGrabber->getSaturation()             << "\n";
        
        //info <<	filterCollection.filterList << "\n";
        
        info << "\n";
        info << "Press e to increment filter" << "\n";
        info << "Press g to Toggle info" << "\n";
        info << "currentExposureName: " << currentExposureName << "\n";
        
        if (omxCameraSettings.doRecording) 
        {
            info << "Press q to stop recording" << "\n";
        }
        
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
        }

    };
    
    void onKey(int key)
    {
        ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
        if(key == 'a')
        {
            videoGrabber->setAutoShutter(true);
        }
        if(key == 's')
        {
            videoGrabber->setAutoShutter(false);
        }
        if(key == 'd')
        {
            videoGrabber->setAutoSensitivity(true);
        }
        if(key == 'f')
        {
            videoGrabber->setAutoSensitivity(false);
        }
        if(key == 'g')
        {
            videoGrabber->setAutoAperture(true);
        }
        if(key == 'h')
        {
            videoGrabber->setAutoAperture(false);
        }
        
        
       /* if(key == ' ')
        {
            videoGrabber->printMeteringMode();
        }*/
        
#if 0
        if (key == 'e')
        {
            videoGrabber->applyImageFilter(filterCollection.getNextFilter());
        }
        
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        
        if (key == 'q')
        {
            ofLogVerbose(__func__) << "STOPPING RECORDING";
            videoGrabber->stopRecording();
        }
        
        if (key == 't')
        {
            videoGrabber->toggleLED();
        }
#endif    
        

    };
};
