#pragma once
#include "CameraDemo.h"


class DemoEnhancement  : public CameraDemo
{
    
public:
    
    

    
    int sharpness;
    int contrast;
    int brightness;
    int saturation;
    
    bool doSharpness;
    bool doContrast; 
    bool doBrightness; 
    bool doSaturation;
    
    bool doValueReset;
    bool doValueResetToZero;
    bool doDRE;
    int dreLevel;
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doSharpness = false;
        doContrast = false;
        doBrightness = false;
        doSaturation = false;
        doDRE = false;
        dreLevel = 0;
        doValueReset = false;
        
        videoGrabber->cameraSettings.enableAutoExposure();
    }
    
    void update()
    {

        if(doValueReset)
        {
            videoGrabber->cameraSettings.setSharpness(-50);
            videoGrabber->cameraSettings.setContrast(-10);
            videoGrabber->cameraSettings.setBrightness(50);
            videoGrabber->cameraSettings.setSaturation(0);
            videoGrabber->cameraSettings.setDRE(0);
            doSharpness = false;
            doContrast = false;
            doBrightness = false;
            doSaturation = false;
            doDRE = false;
            dreLevel = 0;
            doValueReset = false;
            
        }
        
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
        
        if (doSharpness) 
        {
            sharpness = videoGrabber->cameraSettings.getSharpness();
            if(sharpness+1 < 100)
            {
                sharpness++;
            }else
            {
                sharpness = -100;
            }
            videoGrabber->cameraSettings.setSharpness(sharpness);
        }
        if(doContrast)
        {
            contrast = videoGrabber->cameraSettings.getContrast();
            if(contrast+1 < 100)
            {
                contrast++;
            }else
            {
                contrast = -100;
            }
            videoGrabber->cameraSettings.setContrast(contrast);
            
        }
        
        
        if(doBrightness)
        {                
            brightness = videoGrabber->cameraSettings.getBrightness();
            if(brightness+1 < 100)
            {
                brightness++;
            }else
            {
                brightness = 0;
            }
            videoGrabber->cameraSettings.setBrightness(brightness);
        }
        
        
        if(doSaturation)
        {  
            saturation = videoGrabber->cameraSettings.getSaturation();
            if(saturation+1 < 100)
            {
                saturation++;
            }else
            {
                saturation = -100;
            }
            videoGrabber->cameraSettings.setSaturation(saturation);
        }

        
        stringstream info;

        info << "sharpness: "   << videoGrabber->cameraSettings.getSharpness()    << "\n";
        info << "contrast: "    << videoGrabber->cameraSettings.getContrast()     << "\n";
        info << "brightness: "  << videoGrabber->cameraSettings.getBrightness()   << "\n";
        info << "saturation: "  << videoGrabber->cameraSettings.getSaturation()   << "\n";
                
        info << "\n";
        info << "Press 1 to cycle sharpness" << "\n";
        info << "Press 2 to cycle contrast" << "\n";
        info << "Press 3 to cycle brightness" << "\n";
        info << "Press 4 to cycle saturation" << "\n";
        info << "Press 5 to increment Dynamic Range Expansion (DRE): " << dreLevel << "\n";
        info << "Press 0 to reset above to defaults" << "\n";
        

        infoString = info.str();
   
    };
    
    void draw()
    {
        
    }
    
    void onKey(int key)
    {
        if (key == '1')
        {
            doSharpness = !doSharpness;
        }
        
        if (key == '2')
        {
            doContrast = !doContrast;
        }
        
        if (key == '3')
        {
            doBrightness = !doBrightness;
        }
        if (key == '4')
        {
            doSaturation = !doSaturation;
        }
        if (key == '5')
        {
            doDRE = true;
        }
        if (key == '0')
        {
            doValueReset = true;
        }

        
    };
};
