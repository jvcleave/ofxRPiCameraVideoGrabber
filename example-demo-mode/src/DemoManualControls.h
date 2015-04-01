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
    bool doMeteringChange;
    size_t currentWhiteBalanceIndex;
    size_t currentMeteringTypeIndex;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doStepShutterSpeed = false;
        doCycleShutterSpeed = false;
        doChangeMeteringMode = false;
        doChangeWhiteBalance = false;
        doEvCompensation = false;
        doMeteringChange = false;
        currentWhiteBalanceIndex = 0;
        currentMeteringTypeIndex = 0;
    };
    
    void update()
    {
        if(videoGrabber->getCameraSettings().getExposureMode() != CameraSettings::EXPOSURE_MODE_MANUAL)
        {
            videoGrabber->getCameraSettings().enableManualExposure();
            string currentWhiteBalanceName = videoGrabber->getCameraSettings().getWhiteBalance();
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
            unsigned int currentShutterSpeed = videoGrabber->getCameraSettings().getShutterSpeed();
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
            videoGrabber->getCameraSettings().setShutterSpeed(currentShutterSpeed); 
            //doStepShutterSpeed = false;
        }
        if(doStepShutterSpeed)
        {
            doCycleShutterSpeed = false;
            unsigned int currentShutterSpeed = videoGrabber->getCameraSettings().getShutterSpeed();
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
            videoGrabber->getCameraSettings().setShutterSpeed(currentShutterSpeed); 
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
            string whiteBalanceName = OMX_Maps::getInstance().getWhiteBalanceNames()[currentWhiteBalanceIndex];
            videoGrabber->getCameraSettings().setWhiteBalance(whiteBalanceName);
            doChangeWhiteBalance = false;
        }
        if(doEvCompensation)
        {
            int ev = videoGrabber->getCameraSettings().getEvCompensation();
            
            if(ev+1 <= 4)
            {
                ev++;
            }else
            {
                ev = -4;
            }
            videoGrabber->getCameraSettings().setEvCompensation(ev);
            doEvCompensation = false;
        }
        if(doMeteringChange)
        {
            if(currentMeteringTypeIndex+1<OMX_Maps::getInstance().meteringNames.size())
            {
                currentMeteringTypeIndex++;
            }else
            {
                currentMeteringTypeIndex = 0;
            }
            videoGrabber->getCameraSettings().setMeteringType(OMX_Maps::getInstance().meteringNames[currentMeteringTypeIndex]);
            doMeteringChange = false;
        }
        
        stringstream info;
        
        info << "CURRENT SHUTTER SPEED (Microseconds): " <<videoGrabber->getCameraSettings().getShutterSpeed() << "\n";
        info << "\n";
        info << "Press 1 to Step Shutter Speed x2"          <<  "\n";
        info << "Press 2 to Cycle Shutter Speed +100"       <<  "\n";
        info << "Press 3 to Reset Shutter to 100"           <<  "\n";
        info << "Press 4 to Change White Balance: "     << videoGrabber->getCameraSettings().getWhiteBalance()      <<  "\n";
        info << "Press 5 to Change EV Compensation: "   << videoGrabber->getCameraSettings().getEvCompensation()    <<  "\n";
        info << "Press 6 to Change Metering Type: "     << videoGrabber->getCameraSettings().getMeteringType()      <<  "\n";
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
                videoGrabber->getCameraSettings().setShutterSpeed(100);
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
            case '6':
            {
                doMeteringChange = true;
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
