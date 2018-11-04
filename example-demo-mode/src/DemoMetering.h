#pragma once
#include "CameraDemo.h"


class DemoMetering  : public CameraDemo
{
    
public:
    
    bool doChangeMeteringMode;
    bool doMeteringChange;
    
    bool doChangeAutoISO;
    bool doChangeISO;
    
    bool doChangeShutter;
    bool doChangeAutoShutter;
    
    size_t currentMeteringTypeIndex;
    vector<string> meteringNames;
    int ISO;
    int shutterSpeed;
    void setup(ofxOMXVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doChangeMeteringMode = false;
        doMeteringChange = false;
        
        doChangeISO = false;
        doChangeAutoISO = false;
        
        doChangeAutoShutter =false;
        doChangeShutter =false;
    
        currentMeteringTypeIndex = 0;
        meteringNames = OMX_Maps::getInstance().meteringNames;
        ISO = videoGrabber->getISO();
        shutterSpeed = videoGrabber->getShutterSpeed();
    };
    
    void update()
    {

       
        if(doMeteringChange)
        {
            if(currentMeteringTypeIndex+1<meteringNames.size())
            {
                currentMeteringTypeIndex++;
            }else
            {
                currentMeteringTypeIndex = 0;
            }
            videoGrabber->setMeteringType(GetMetering(meteringNames[currentMeteringTypeIndex]));
            doMeteringChange = false;
        }
        
        if(doChangeISO)
        {
            
            int currentISO = videoGrabber->getISO();
            if(currentISO == 0)
            {
                currentISO = 100;
            }else
            {
                currentISO*=2;  
            }
            
            ISO = currentISO;
            if(ISO > 150000)
            {
                ISO=0;
            }
            videoGrabber->setISO(ISO);
            doChangeISO = false;
        }
        
        if(doChangeAutoISO)
        {
            videoGrabber->setAutoISO(!videoGrabber->getAutoISO());
            doChangeAutoISO = false;
        }
        
        if(doChangeAutoShutter)
        {
            videoGrabber->setAutoShutter(!videoGrabber->getAutoShutter());
            doChangeAutoShutter = false;
        }
        
        
        
        if(doChangeShutter)
        {
            
            int currentShutter = videoGrabber->getShutterSpeed();
            if(currentShutter == 0)
            {
                currentShutter = 100;
            }else
            {
                currentShutter*=2;  
            }
            
            if(currentShutter>=INT_MAX)
            {
                currentShutter = 100;
            }
            videoGrabber->setShutterSpeed(currentShutter);
            doChangeShutter = false;
        }
        stringstream info;
        
        info << "\n";
        info << "Press 1 to Change Metering Type: " << videoGrabber->getMeteringType()  <<  endl;
        info << "Press 2 to Toggle Auto ISO: "      << videoGrabber->getAutoISO()       <<  endl;
        info << "Press 3 to Increment ISO: "        << videoGrabber->getISO()           <<  endl;
        
        info << "Press 4 to Toggle Auto Shutter: "      << videoGrabber->getAutoShutter()   <<  endl;
        info << "Press 5 to Increment Shutter Speed: "  << videoGrabber->getShutterSpeed()  <<  endl;

        //info << "Exposure Settings: " << endl;
        //info << videoGrabber->printExposure() << endl;
        

        infoString = info.str();
        
    };
    
    void draw()
    {
        
        
        
        
        
    };
    
    void onKey(int key)
    {
        switch (key)
        {
            case '1':
            {
                 doMeteringChange = true;
                break;
            }
            case '2':
            {
                doChangeAutoISO = true;
                break;
            }
            case '3':
            {
                doChangeISO = true;
                break;
            }
            case '4':
            {
                doChangeAutoShutter = true;
                break;
            }
            case '5':
            {
                doChangeShutter = true;
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
