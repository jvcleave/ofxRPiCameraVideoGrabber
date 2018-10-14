#pragma once
#include "CameraDemo.h"


class DemoWhiteBalance  : public CameraDemo
{
    
public:
    
    bool doChangeWhiteBalance;
    bool doEvCompensation;
    bool doChangeFlickerCancellation;
    size_t currentWhiteBalanceIndex;
    vector<string> whiteBalanceNames;
    void setup(ofxRPiCameraVideoGrabber* videoGrabber_)
    {
        CameraDemo::setup( videoGrabber_);
        doChangeWhiteBalance = false;
        doEvCompensation = false;
        doChangeFlickerCancellation = false;        
        currentWhiteBalanceIndex = 0;
        whiteBalanceNames = OMX_Maps::getInstance().getWhiteBalanceNames();
    };
    
    void update()
    {

        if(doChangeWhiteBalance)
        {
            if(currentWhiteBalanceIndex+1 < whiteBalanceNames.size())
            {
                currentWhiteBalanceIndex++;
            }else
            {
                currentWhiteBalanceIndex = 0;
            }
            videoGrabber->setWhiteBalance(GetWhiteBalance(whiteBalanceNames[currentWhiteBalanceIndex]));
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
        if(doChangeFlickerCancellation)
        {

            videoGrabber->setFlickerCancellation(!videoGrabber->isFlickerCancellationEnabled());
            doChangeFlickerCancellation = false;
        }
        
        stringstream info;
        
        info << "\n";
        info << "Press 1 to Change White Balance: "     << videoGrabber->getWhiteBalance()      <<  "\n";
        info << "Press 2 to Change EV Compensation: "   << videoGrabber->getEvCompensation()    <<  "\n";
        info << "Press 3 to Toggle Flicker Cancellation: " << videoGrabber->isFlickerCancellationEnabled()      <<  "\n";        

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
                doChangeWhiteBalance = true;
                break;
            }
            case '2':
            {
                doEvCompensation = true;
                break;
            }
            case '3':
            {
                 doChangeFlickerCancellation = true;
                break;
            }
            default:
            {
                break;
            }
        }
        
    };
};
