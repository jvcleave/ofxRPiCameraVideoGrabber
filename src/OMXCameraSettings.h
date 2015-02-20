#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608

class OMXCameraSettings
{
public:
    
    enum Preset 
    {
        PRESET_NONE =0,
        PRESET_FASTEST_480P,
        PRESET_FASTEST_720P,
        PRESET_FASTEST_FRAME_RATE,
        PRESET_HIGHEST_RES,
        PRESET_HIGHEST_RES_TEXTURE,
        PRESET_HIGHEST_RES_NONTEXTURE,
        
        PRESET_1080P_30FPS_TEXTURE,
        PRESET_1080P_30FPS_NONTEXTURE,
        PRESET_1080P_30FPS,
        
        PRESET_720P_40FPS_NONTEXTURE,
        PRESET_720P_30FPS_NONTEXTURE,
        
        PRESET_720P_40FPS_TEXTURE,
        PRESET_720P_30FPS_TEXTURE,
        
        PRESET_720P_40FPS,
        PRESET_720P_30FPS,
        
        PRESET_480P_90FPS_TEXTURE,
        PRESET_480P_60FPS_TEXTURE,
        PRESET_480P_40FPS_TEXTURE,
        PRESET_480P_30FPS_TEXTURE,
        
        PRESET_480P_90FPS_NONTEXTURE,
        PRESET_480P_60FPS_NONTEXTURE,
        PRESET_480P_40FPS_NONTEXTURE,
        PRESET_480P_30FPS_NONTEXTURE,
        
        PRESET_480P_90FPS,
        PRESET_480P_60FPS,
        PRESET_480P_40FPS,
        PRESET_480P_30FPS,
    };
    
    int width;
    int height;
    int framerate;
    bool doRecording;
    
    bool isUsingTexture;
    bool enablePixels;
    string recordingFilePath;
    
    bool doManualExposure;
    Preset preset;
    OMXCameraSettings()
    {
        width = 1280;
        height = 720;
        framerate = 30;
        isUsingTexture = true;
        enablePixels = false;
        doRecording = false;
        recordingFilePath = "";
        preset = PRESET_NONE;
        doManualExposure = false;
        
        //doFlipTexture = false;
    }
    
    
    void applyPreset()
    {
        switch(preset)
        {
            case PRESET_NONE :
            {
                break;
            }
            case PRESET_1080P_30FPS_TEXTURE :
            case PRESET_HIGHEST_RES_TEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                isUsingTexture = true;
                break;
            }
                
            case PRESET_1080P_30FPS_NONTEXTURE :
            case PRESET_HIGHEST_RES_NONTEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                isUsingTexture = false;
                break;
            }
            
            case PRESET_1080P_30FPS :
            case PRESET_HIGHEST_RES :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                break;
            }
                
            case PRESET_720P_40FPS_TEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 40;
                isUsingTexture = true;
                break;
            }
                
            case PRESET_720P_40FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 40;
                isUsingTexture = false;
                break;
            }
                
            case PRESET_720P_40FPS :
            case PRESET_FASTEST_720P:
            {
                width = 1280;
                height = 720;
                framerate = 40;
                break;
            }
            case PRESET_720P_30FPS :
            {
                width = 1280;
                height = 720;
                framerate = 30;
                break;
            }
             
            
            case PRESET_720P_30FPS_TEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 30;
                isUsingTexture = true;
                break;
            }
                
            case PRESET_720P_30FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 30;
                isUsingTexture = false;
                break;
            }
                
            case PRESET_480P_90FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                isUsingTexture = true;
                break;
            } 
            case PRESET_480P_60FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                isUsingTexture = true;
                break;
            }    
                
            case PRESET_480P_40FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                isUsingTexture = true;
                break;
            }    
            
             
            case PRESET_480P_30FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                isUsingTexture = true;
                break;
            }
                
            case PRESET_480P_90FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                isUsingTexture = false;
                break;
            } 
            case PRESET_480P_60FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                isUsingTexture = false;
                break;
            } 
            case PRESET_480P_40FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                isUsingTexture = false;
                break;
            } 
            
            case PRESET_480P_30FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                isUsingTexture = false;
                break;
            }
                
            case PRESET_480P_90FPS :
            case PRESET_FASTEST_480P:
            case PRESET_FASTEST_FRAME_RATE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                break;
            }
                
            case PRESET_480P_60FPS :
            {
                width = 640;
                height = 480;
                framerate = 60;
                break;
            }
                
            case PRESET_480P_40FPS :
            {
                width = 640;
                height = 480;
                framerate = 40;
                break;
            }
                
            case PRESET_480P_30FPS :
            {
                width = 640;
                height = 480;
                framerate = 30;
                break;
            }
                
            default:
            {
                width = 1280;
                height = 720;
                framerate = 30;
                break;
            }
                
        }
    }
};