#pragma once
#include "CameraDemo.h"


class DemoMeteringModes  : public CameraDemo
{
    
public:
    
    
    bool doDrawInfo;
    bool doChangeMeteringMode = false;
    bool doShutterSpeed;
    bool doAperture;
    bool doISO;
    vector<CameraMeteringMode> meteringModes;
    int currentMeteringMode;
   
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
 
        CameraDemo::setup( videoGrabber_);
        doDrawInfo	= true;
        doShutterSpeed = false;
        doISO = false;
        
#if 0
        OMX_MeteringModeAverage,     /**< Center-weighted average metering. */
        OMX_MeteringModeSpot,  	      /**< Spot (partial) metering. */
        OMX_MeteringModeMatrix,      /**< Matrix or evaluative metering. */
        OMX_MeteringModeBacklit,
        
        
         CameraMeteringMode defaults
         meteringType = OMX_MeteringModeMatrix;
         evCompensation=0; //-10 to +10
         
         aperture = 0;
         autoAperture = true;
         
         shutterSpeedMicroSeconds = 9977; //default 1000
         autoShutter = true;
         
         ISO = 0;
         autoSensitivity = true;
#endif
        
       
        

        CameraMeteringMode autoMode; //default        
        meteringModes.push_back(autoMode);
        
        CameraMeteringMode mode2 = autoMode;
        mode2.meteringType = OMX_MeteringModeSpot;        
        meteringModes.push_back(mode2);
        
        CameraMeteringMode mode3 = autoMode;
        mode3.meteringType = OMX_MeteringModeAverage;        
        meteringModes.push_back(mode3);
        
        CameraMeteringMode mode4 = autoMode;
        mode4.meteringType = OMX_MeteringModeBacklit;        
        meteringModes.push_back(mode4);
        
        
        CameraMeteringMode manualMode;
        manualMode.autoAperture = false;
        manualMode.autoShutter = false;
        manualMode.autoISO = false;
        meteringModes.push_back(manualMode);
        
        CameraMeteringMode mode6 = manualMode;
        mode6.shutterSpeedMicroSeconds = 1000;
        meteringModes.push_back(mode6);
        
        CameraMeteringMode mode7 = manualMode;
        mode7.shutterSpeedMicroSeconds = 100000; //33158 highest (shutterSpeedMicroSeconds = 33900 will max out - tied to framerate?)
        meteringModes.push_back(mode7);
        
        CameraMeteringMode mode8 = manualMode;
        mode8.shutterSpeedMicroSeconds = 32000;
        meteringModes.push_back(mode8);
        
        CameraMeteringMode mode9 = manualMode;
        mode9.shutterSpeedMicroSeconds = 33900;
        mode9.ISO = 100;
        meteringModes.push_back(mode9);
        
        CameraMeteringMode mode10 = mode9;
        mode10.ISO = 200;
        meteringModes.push_back(mode10);
        
        CameraMeteringMode mode11 = manualMode;
        mode11.shutterSpeedMicroSeconds = 1000;
        mode11.ISO = 400;
        meteringModes.push_back(mode11);
        
        
        CameraMeteringMode mode12 = manualMode;
        mode12.shutterSpeedMicroSeconds = 33900;
        mode12.ISO = 800;
        meteringModes.push_back(mode12);
        
        CameraMeteringMode mode13 = manualMode;
        mode13.shutterSpeedMicroSeconds = 33300;
        mode13.ISO = 1600;
        meteringModes.push_back(mode13);
        
        CameraMeteringMode mode14 = manualMode;
        mode14.evCompensation = -10;
        meteringModes.push_back(mode14);
        
        CameraMeteringMode mode15 = manualMode;
        mode15.evCompensation = 10;
        meteringModes.push_back(mode15);
        
        currentMeteringMode = 0;
        
    };
    
    void update()
    {
        /*if (ofGetFrameNum() % 300 == 0) 
        {
            doChangeMeteringMode = true;
            
        }*/
        if (doChangeMeteringMode) 
        {
            if ((unsigned int)currentMeteringMode+1 < meteringModes.size()) 
            {
      
                currentMeteringMode++;
                
            }else
            {
                currentMeteringMode = 0;
            }
            doChangeMeteringMode = false;
            videoGrabber->setMeteringMode(meteringModes[currentMeteringMode]);
        }
        if(doShutterSpeed)
        {
            int currentShutterSpeed = videoGrabber->getShutterSpeed();
            if(currentShutterSpeed+1000 < 33300)
            {
                currentShutterSpeed+=1000;
            }else
            {
                currentShutterSpeed = 0;
            }
            videoGrabber->setShutterSpeed(currentShutterSpeed);
        }
        if(doAperture)
        {
            int currentAperture = videoGrabber->getAperture();
            if(currentAperture<32)
            {
                currentAperture++;
            }else
            {
                currentAperture = 0;
            }
            videoGrabber->setAperture(currentAperture);
        }
        if(doISO)
        {
            int currentISO = videoGrabber->getISO();
            if(currentISO+50 < 3200)
            {
                currentISO+=50;
            }else
            {
                currentISO = 0;
            }
            videoGrabber->setISO(currentISO);
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
        
        info << "\n";
        info << videoGrabber->meteringModetoString() << "\n";
        
        info << "\n";
        info << "Press m to increment Metering Mode: " << currentMeteringMode << "\n";
        info << "Press s to toggle Shutter Speed increase: " << videoGrabber->getShutterSpeed() << "\n";
        info << "Press a to increment aperture: " << videoGrabber->getAperture() << "\n";
        info << "Press i to increment ISO +50: " << videoGrabber->getISO() << "\n";
        
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
        if (key == 'e')
        {
            videoGrabber->applyImageFilter(filterCollection.getNextFilter());
        }
        
        if (key == 'g')
        {
            doDrawInfo = !doDrawInfo;
        }
        if (key == 's')
        {
            doShutterSpeed = !doShutterSpeed;
        }
        if (key == 'a')
        {
            doAperture = !doAperture;
        }
        if (key == 'i')
        {
            doISO = !doISO;
        }
        
        if (key == 'm')
        {
            doChangeMeteringMode = true;
        }
    };
};
