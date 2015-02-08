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
    
    void setup(OMXCameraSettings omxCameraSettings_, ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup(omxCameraSettings_, videoGrabber_);
        doDrawInfo	= true;
        
    };
    
    void update()
    {
        videoGrabber->setAutoAperture(doAutoMode);
        videoGrabber->setAutoShutter(doAutoMode);
        videoGrabber->setAutoSensitivity(doAutoMode);
        if(doValueResetToZero)
        {
            videoGrabber->setSharpness(0);
            videoGrabber->setContrast(0);
            videoGrabber->setBrightness(0);
            videoGrabber->setSaturation(0);
            doValueResetToZero = false;
        }
        if(doValueReset)
        {
            videoGrabber->setDefaultValues();
            doValueReset = false;
            doAutoMode = true;
        }else
        {
            
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
        info << videoGrabber->meteringModetoString() << "\n";
        
        info << "\n";
        info << "sharpness: "   << videoGrabber->getSharpness()    << "\n";
        info << "contrast: "    << videoGrabber->getContrast()     << "\n";
        info << "brightness: "  << videoGrabber->getBrightness()   << "\n";
        info << "saturation: "  << videoGrabber->getSaturation()   << "\n";
        
        info << "doAutoMode: "  << doAutoMode   << "\n";
        
        info << "\n";
        info << "Press 1 to cycle sharpness" << "\n";
        info << "Press 2 to cycle contrast" << "\n";
        info << "Press 3 to cycle brightness" << "\n";
        info << "Press 4 to cycle saturation" << "\n";
        info << "Press r to reset values to defaults" << "\n";
        info << "Press 0 to set values to 0" << "\n";
        info << "Press m to toggle auto exposure" << "\n";
        
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
        if (key == 'm')
        {
            doAutoMode = !doAutoMode;
        }
        
        if (key == 'r')
        {
            doValueReset = true;
        }
        if (key == '0')
        {
            doValueResetToZero = true;
        }
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        
    };
};
