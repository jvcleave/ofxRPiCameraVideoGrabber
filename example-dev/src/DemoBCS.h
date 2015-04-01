#pragma once
#include "CameraDemo.h"


class DemoBCS  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doValueReset = false;
    bool doValueResetToZero = false;
    
    int sharpness;
    int contrast;
    int brightness;
    int saturation;
    
    bool doSharpness = false;
    bool doContrast = false;
    bool doBrightness = false;
    bool doSaturation = false;
    
    bool doAutoMode = true;
    
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doDrawInfo	= true;
        
    };
    
    void update()
    {
        videoGrabber->getCameraSettings().setAutoAperture(doAutoMode);
        videoGrabber->getCameraSettings().setAutoShutter(doAutoMode);
        videoGrabber->getCameraSettings().setAutoISO(doAutoMode);
        if(doValueResetToZero)
        {
            videoGrabber->getCameraSettings().setSharpness(0);
            videoGrabber->getCameraSettings().setContrast(0);
            videoGrabber->getCameraSettings().setBrightness(0);
            videoGrabber->getCameraSettings().setSaturation(0);
            doValueResetToZero = false;
        }
        if(doValueReset)
        {
            videoGrabber->getCameraSettings().applyAllSettings();
            doValueReset = false;
            doAutoMode = true;
        }else
        {
            
            if (doSharpness) 
            {
                sharpness = videoGrabber->getCameraSettings().getSharpness();
                if(sharpness+1 < 100)
                {
                    sharpness++;
                }else
                {
                    sharpness = -100;
                }
                videoGrabber->getCameraSettings().setSharpness(sharpness);
            }
            if(doContrast)
            {
                contrast = videoGrabber->getCameraSettings().getContrast();
                if(contrast+1 < 100)
                {
                    contrast++;
                }else
                {
                    contrast = -100;
                }
                videoGrabber->getCameraSettings().setContrast(contrast);
                
            }
            
 
            if(doBrightness)
            {                
                brightness = videoGrabber->getCameraSettings().getBrightness();
                if(brightness+1 < 100)
                {
                    brightness++;
                }else
                {
                    brightness = 0;
                }
                videoGrabber->getCameraSettings().setBrightness(brightness);
            }
            
            
            if(doSaturation)
            {  
                saturation = videoGrabber->getCameraSettings().getSaturation();
                if(saturation+1 < 100)
                {
                    saturation++;
                }else
                {
                    saturation = -100;
                }
                videoGrabber->getCameraSettings().setSaturation(saturation);
            }
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
        info << "Camera Resolution: "   << videoGrabber->getWidth() << "x" << videoGrabber->getHeight()	<< " @ "<< videoGrabber->getFrameRate() <<"FPS"<< "\n";
        info << videoGrabber->meteringModetoString() << "\n";
        
        info << "\n";
        info << "sharpness: "   << videoGrabber->getCameraSettings().getSharpness()    << "\n";
        info << "contrast: "    << videoGrabber->getCameraSettings().getContrast()     << "\n";
        info << "brightness: "  << videoGrabber->getCameraSettings().getBrightness()   << "\n";
        info << "saturation: "  << videoGrabber->getCameraSettings().getSaturation()   << "\n";
        
        info << "doAutoMode: "  << doAutoMode   << "\n";
        
        info << "\n";
        info << "Press z to cycle sharpness" << "\n";
        info << "Press x to cycle contrast" << "\n";
        info << "Press c to cycle brightness" << "\n";
        info << "Press v to cycle saturation" << "\n";
        info << "Press b to reset values to defaults" << "\n";
        info << "Press n to set values to 0" << "\n";
        info << "Press m to toggle auto exposure" << "\n";
        
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
        if (key == 'n')
        {
            doValueResetToZero = true;
        }
        if (key == 'm')
        {
            doAutoMode = !doAutoMode;
        }
        
       
        
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        
    };
};
