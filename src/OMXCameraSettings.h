#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608

class OMXCameraSettings
{
public:
    
    enum Preset 
    {
        PRESET_NONE =0,
        PRESET_1080P_30FPS_TEXTURE,
        PRESET_1080P_30FPS_NONTEXTURE,
        PRESET_720P_30FPS_TEXTURE,
        PRESET_720P_30FPS_NONTEXTURE,
        PRESET_480P_30FPS_TEXTURE,
        PRESET_480P_30FPS_NONTEXTURE,
        PRESET_480P_40FPS_TEXTURE,
        PRESET_480P_90FPS,
        PRESET_480P_60FPS,
        PRESET_480P_40FPS,
        PRESET_480P_30FPS,
        PRESET_720P_40FPS,
        PRESET_720P_30FPS,
        PRESET_1080P_30FPS,
        PRESET_1944P_15FPS,
        PRESET_1944P_1FPS,
        PRESET_FASTEST_FRAME_RATE,
        PRESET_HIGHEST_RES
    };
    
    int width;
    int height;
    int framerate;
    bool doRecording;
    bool doRecordingPreview; //TODO make private
    
    int previewWidth;
    int previewHeight;
    bool isUsingTexture;
    bool enablePixels;
    string recordingFilePath;
    bool doConvertToMKV;
    
    bool doManualExposure;
    Preset preset;
    // EGLImageKHR eglImage;
    //ofTexture* texture;
    OMXCameraSettings()
    {
        width = 1280;
        height = 720;
        framerate = 30;
        isUsingTexture = true;
        enablePixels = false;
        doRecording = false;
        doRecordingPreview = false;
        previewWidth = width;
        previewHeight = height;
        recordingFilePath = "";
        doConvertToMKV = false;
        preset = PRESET_NONE;
        // eglImage = NULL;
        //texture = NULL;
        doManualExposure = false;
        
        //doFlipTexture = false;
    }
    
    void enablePreview()
    {
        if(width<=1280)
        {
            previewWidth = width;
            previewHeight = height;
        }else 
        {
            previewWidth = 0;
            previewHeight = 0;
        }
        
        doRecordingPreview = true;
    }
    
    void checkForPreset()
    {
        switch(preset)
        {
            case PRESET_NONE :
            {
                break;
            }
            case PRESET_1080P_30FPS_TEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                isUsingTexture = true;
                break;
            }
            case PRESET_1080P_30FPS_NONTEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                isUsingTexture = false;
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
            case PRESET_480P_30FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                isUsingTexture = false;
                break;
            }
                
            case PRESET_480P_90FPS :
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
            case PRESET_720P_40FPS :
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
            case PRESET_1080P_30FPS :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                break;
            }
                
            case PRESET_1944P_15FPS :
            case PRESET_HIGHEST_RES :
            {
                width = 2592;
                height = 1944;
                framerate = 15;
                break;
            }
            case PRESET_1944P_1FPS :
            {
                width = 2592;
                height = 1944;
                framerate = 1;
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
    //bool doRecordingPreview;
};