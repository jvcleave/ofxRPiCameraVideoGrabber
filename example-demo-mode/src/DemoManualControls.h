#pragma once
#include "CameraDemo.h"


class DemoManualControls  : public CameraDemo
{
    
public:
    
    bool doStepShutterSpeed;
    bool doCycleShutterSpeed;
    bool doChangeMeteringMode;
    bool doChangeWhiteBalance;
    bool doEvCompensation;
    size_t currentWhiteBalanceIndex;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doStepShutterSpeed = false;
        doCycleShutterSpeed = false;
        doChangeMeteringMode = false;
        doChangeWhiteBalance = false;
        doEvCompensation = false;
        currentWhiteBalanceIndex = 0;
    };
    
    void update()
    {
        if(videoGrabber->getExposureMode() != ofxRPiCameraVideoGrabber::EXPOSURE_MODE_MANUAL)
        {
            videoGrabber->enableManualExposure();
            string currentWhiteBalanceName = videoGrabber->getCurrentWhiteBalanceName();
            for(size_t i =0; i<OMX_Maps::getInstance().getWhiteBalanceNames().size(); ++i)
            {
                if(currentWhiteBalanceName == OMX_Maps::getInstance().getWhiteBalanceNames()[i])
                {
                    currentWhiteBalanceIndex = i;
                }
            }
        }
        if(doCycleShutterSpeed)
        {
            doStepShutterSpeed = false;
            unsigned int currentShutterSpeed = videoGrabber->getShutterSpeed();
            if(currentShutterSpeed == 0)
            {
                currentShutterSpeed =100;
            }else
            {
                if(currentShutterSpeed+100 < MAX_SHUTTER_SPEED_MICROSECONDS)
                {
                    currentShutterSpeed+=100;
                }else
                {
                    currentShutterSpeed = 100;
                }
            }
            videoGrabber->setShutterSpeed(currentShutterSpeed); 
            //doStepShutterSpeed = false;
        }
        if(doStepShutterSpeed)
        {
            doCycleShutterSpeed = false;
            unsigned int currentShutterSpeed = videoGrabber->getShutterSpeed();
            if(currentShutterSpeed == 0)
            {
                currentShutterSpeed =100;
            }else
            {
                if(currentShutterSpeed*2 <= MAX_SHUTTER_SPEED_MICROSECONDS)
                {
                    currentShutterSpeed*=2;
                }else
                {
                    currentShutterSpeed = 100;
                }
            }
            videoGrabber->setShutterSpeed(currentShutterSpeed); 
            doStepShutterSpeed = false;
        }
        
        if(doChangeWhiteBalance)
        {
            if(currentWhiteBalanceIndex+1 < OMX_Maps::getInstance().getWhiteBalanceNames().size())
            {
                currentWhiteBalanceIndex++;
            }else
            {
                currentWhiteBalanceIndex = 0;
            }
            string currentWhiteBalanceName = OMX_Maps::getInstance().getWhiteBalanceNames()[currentWhiteBalanceIndex];
            videoGrabber->setWhiteBalance(OMX_Maps::getInstance().getWhiteBalance(currentWhiteBalanceName));
            doChangeWhiteBalance = false;
        }
        if(doEvCompensation)
        {
            int ev = videoGrabber->getEvCompensation();
            
            if(ev+1 <= 4)
            {
                ev++;
            }else
            {
                ev = -4;
            }
            videoGrabber->setEvCompensation(ev);
            doEvCompensation = false;
        }
        stringstream info;
        
        info << "CURRENT SHUTTER SPEED (Microseconds): " <<videoGrabber->getShutterSpeed() << "\n";
        info << "\n";
        info << "Press 1 to Step Shutter Speed x2"          <<  "\n";
        info << "Press 2 to Cycle Shutter Speed +100"       <<  "\n";
        info << "Press 3 to Reset Shutter to 100"           <<  "\n";
        info << "Press 4 to Change White Balance: " << videoGrabber->getCurrentWhiteBalanceName()<<  "\n";
        info << "Press 5 to Change EV Compensation: " << videoGrabber->getEvCompensation()<<  "\n";
        
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
                doStepShutterSpeed = true;
                break;
            }
            case '2':
            {
                doCycleShutterSpeed = !doCycleShutterSpeed;
                break;
            }
            case '3':
            {
                videoGrabber->setShutterSpeed(100);
                break;
            }
            case '4':
            {
                doChangeWhiteBalance = true;
                break;
            }
            case '5':
            {
                doEvCompensation = true;
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
