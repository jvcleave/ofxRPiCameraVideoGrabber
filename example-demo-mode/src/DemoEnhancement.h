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
        
        //videoGrabber->enableAutoExposure();
    }
    
    void update()
    {

        if(doValueReset)
        {
            videoGrabber->setSharpness(-50);
            videoGrabber->setContrast(-10);
            videoGrabber->setBrightness(50);
            videoGrabber->setSaturation(0);
            videoGrabber->setDRE(0);
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
            videoGrabber->setDRE(dreLevel);
            doDRE = false;
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
        info << "Press 1 to cycle sharpness" << "\n";
        info << "Press 2 to cycle contrast" << "\n";
        info << "Press 3 to cycle brightness" << "\n";
        info << "Press 4 to cycle saturation" << "\n";
        info << "Press 5 to increment Dynamic Range Expansion (DRE): " << videoGrabber->getDRE() << "\n";
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
