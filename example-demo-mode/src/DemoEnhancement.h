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
    
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doSharpness = false;
        doContrast = false;
        doBrightness = false;
        doSaturation = false;
        doValueReset = false;
        videoGrabber->enableAutoExposure();
    }
    
    void update()
    {

        if(doValueReset)
        {
            videoGrabber->setSharpness(0);
            videoGrabber->setContrast(0);
            videoGrabber->setBrightness(0);
            videoGrabber->setSaturation(0);
            doValueReset = false;
        }
        
        if (doSharpness) 
        {
            sharpness = videoGrabber->getSharpness();
            if(sharpness+1 < 100)
            {
                sharpness++;
            }else
            {
                sharpness = -100;
            }
            videoGrabber->setSharpness(sharpness);
        }
        if(doContrast)
        {
            contrast = videoGrabber->getContrast();
            if(contrast+1 < 100)
            {
                contrast++;
            }else
            {
                contrast = -100;
            }
            videoGrabber->setContrast(contrast);
            
        }
        
        
        if(doBrightness)
        {                
            brightness = videoGrabber->getBrightness();
            if(brightness+1 < 100)
            {
                brightness++;
            }else
            {
                brightness = 0;
            }
            videoGrabber->setBrightness(brightness);
        }
        
        
        if(doSaturation)
        {  
            saturation = videoGrabber->getSaturation();
            if(saturation+1 < 100)
            {
                saturation++;
            }else
            {
                saturation = -100;
            }
            videoGrabber->setSaturation(saturation);
        }

        
        stringstream info;

        info << "sharpness: "   << videoGrabber->getSharpness()    << "\n";
        info << "contrast: "    << videoGrabber->getContrast()     << "\n";
        info << "brightness: "  << videoGrabber->getBrightness()   << "\n";
        info << "saturation: "  << videoGrabber->getSaturation()   << "\n";
                
        info << "\n";
        info << "Press z to cycle sharpness" << "\n";
        info << "Press x to cycle contrast" << "\n";
        info << "Press c to cycle brightness" << "\n";
        info << "Press v to cycle saturation" << "\n";
        info << "Press b to set values to 0" << "\n";
        

        infoString = info.str();
   
    };
    
    void draw()
    {
        
    }
    
    void onKey(int key)
    {
        if (key == 'z')
        {
            doSharpness = !doSharpness;
        }
        
        if (key == 'x')
        {
            doContrast = !doContrast;
        }
        
        if (key == 'c')
        {
            doBrightness = !doBrightness;
        }
        if (key == 'v')
        {
            doSaturation = !doSaturation;
        }
        if (key == 'b')
        {
            doValueReset = true;
        }

        
    };
};
