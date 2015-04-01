#pragma once

#include "ofMain.h"
#include "CameraSettings.h"



class SessionConfig
{
public:
    enum MODE
    {
        MODE_TEXTURE,
        MODE_NONTEXTURE
    };
    enum Preset 
    {
        PRESET_NONE,
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
    
    bool enablePixels;
    string recordingFilePath;
    
    Preset preset;
    
    MODE mode;
    SessionConfig()
    {
        width = 1280;
        height = 720;
        framerate = 30;
        mode=MODE_TEXTURE;
        enablePixels = false;
        doRecording = false;
        recordingFilePath = "";
        preset = PRESET_NONE;
        //doFlipTexture = false;
    }
    CameraSettings& getCameraSettings()
    {
        return cameraSettings;
    }
    bool isUsingTexture()
    {
        return (mode==MODE_TEXTURE);
    }
    vector<SessionConfig::Preset> getAllPresets()
    {
        vector<SessionConfig::Preset> presets;
        for ( int preset = SessionConfig::PRESET_NONE; preset != SessionConfig::PRESET_480P_30FPS; ++preset )
        {
            if(preset != SessionConfig::PRESET_NONE)
            {
                presets.push_back((SessionConfig::Preset)preset);
            }
            
        }
        return presets;
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
                mode=MODE_TEXTURE;
                break;
            }
                
            case PRESET_1080P_30FPS_NONTEXTURE :
            case PRESET_HIGHEST_RES_NONTEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                mode=MODE_NONTEXTURE;
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
                mode=MODE_TEXTURE;
                break;
            }
                
            case PRESET_720P_40FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 40;
                mode=MODE_NONTEXTURE;
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
                mode=MODE_TEXTURE;
                break;
            }
                
            case PRESET_720P_30FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 30;
                mode=MODE_NONTEXTURE;
                break;
            }
                
            case PRESET_480P_90FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                mode=MODE_TEXTURE;
                break;
            } 
            case PRESET_480P_60FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                mode=MODE_TEXTURE;
                break;
            }    
                
            case PRESET_480P_40FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                mode=MODE_TEXTURE;
                break;
            }    
            
             
            case PRESET_480P_30FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                mode=MODE_TEXTURE;
                break;
            }
                
            case PRESET_480P_90FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                mode=MODE_NONTEXTURE;
                break;
            } 
            case PRESET_480P_60FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                mode=MODE_NONTEXTURE;
                break;
            } 
            case PRESET_480P_40FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                mode=MODE_NONTEXTURE;
                break;
            } 
            
            case PRESET_480P_30FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                mode=MODE_NONTEXTURE;
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
private:
    CameraSettings cameraSettings;
};