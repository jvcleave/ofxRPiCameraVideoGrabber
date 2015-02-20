#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608



class CameraState
{
public:
    
    string exposurePreset;
    string meteringType;
    int evCompensation;
    bool autoShutter;
    int shutterSpeedMicroSeconds;
    bool autoAperture;
    int aperture;
    bool autoISO;
    int ISO;
    int sharpness;
    int contrast;
    int brightness;
    int saturation;
    bool framestabilization;
    string whiteBalance;
    string imageFilter;
    int drcLevel;
    ofRectangle cropRectangle;
    int zoomLevel;
    int rotation;
    string mirror; 
    
    bool disableSoftwareSharpen;
    bool disableSoftwareSaturation;
    
    CameraState()
    {
        exposurePreset = "Auto";
        meteringType="OMX_MeteringModeAverage";
        evCompensation=0;
        autoShutter=true;
        shutterSpeedMicroSeconds=0;
        autoAperture=true;
        aperture=0;
        autoISO=true;
        ISO=0;
        sharpness=-50;
        contrast=-10;
        brightness=50;
        saturation=0;
        framestabilization=false;
        whiteBalance="Auto";
        imageFilter="None";
        drcLevel=0;
        cropRectangle.set(0,0,100,100);
        zoomLevel=0;
        rotation=0;
        mirror="MIRROR_NONE";
        disableSoftwareSharpen = false;
        disableSoftwareSaturation = false;
    }
    void validate()
    {
        //TODO
    }
    void saveToFile(string filePath="")
    {
        
        stringstream state;
        
        //Metering Mode
        state << "exposurePreset="              << exposurePreset           << "\n";
        state << "meteringType="                << meteringType             << "\n";
        state << "evCompensation="              << evCompensation           << "\n";
        state << "autoShutter="                 << autoShutter              << "\n";
        state << "shutterSpeedMicroSeconds="    << shutterSpeedMicroSeconds << "\n";
        state << "autoAperture="                << autoAperture             << "\n";
        state << "aperture="                    << aperture                 << "\n";
        state << "autoISO="                     << autoISO                  << "\n";
        state << "ISO="                         << ISO                      << "\n";
        
        state << "sharpness="                   << sharpness                << "\n";
        state << "contrast="                    << contrast                 << "\n";
        state << "brightness="                  << brightness               << "\n";
        state << "saturation="                  << saturation               << "\n";
        
        state << "framestabilization="          << framestabilization       << "\n";
        state << "whiteBalance="                << whiteBalance             << "\n";
        state << "imageFilter="                 << imageFilter              << "\n";        
        state << "drcLevel="                    << drcLevel                 << "\n";
        state << "cropRectangle=" << cropRectangle.x  << "," << cropRectangle.y << "," << cropRectangle.width << "," << cropRectangle.height << "\n";
        state << "zoomLevel="                   << zoomLevel                << "\n";
        state << "rotation="                    << rotation                 << "\n";
        state << "mirror="                      << mirror                   << "\n";
        
        
        ofLogVerbose(__func__) << state.str();
        if(filePath.empty())
        {
            filePath = ofToDataPath("CameraState.ini", true);
        }
        ofBuffer buffer(state.str());
        ofBufferToFile(filePath, buffer);
    }

};

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
    
    Preset preset;
    CameraState state;
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