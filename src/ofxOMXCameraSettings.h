#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608



class ofxOMXPhotoGrabberListener
{
public:
    virtual void onTakePhotoComplete(string filePath)=0;
    
};

class ofxOMXVideoGrabberListener
{
public:
    virtual void onRecordingComplete(string filePath)=0;
};

class ofxOMXCameraSettings
{
public:
    
	int width;
	int height;
	int framerate;
	
	bool enableTexture;
	bool enablePixels;
	string recordingFilePath;
    
    bool LED;
    string LED_PIN;
    string meteringType;
    bool autoISO;
    int ISO;
    bool autoShutter;
    int shutterSpeed;
    int sharpness;   // -100 to 100
    int contrast;    // -100 to 100 
    int brightness;  //    0 to 100
    int saturation;  // -100 to 100 
    int dreLevel;    //   -4 to 4
    ofRectangle cropRectangle;
    
    int zoomLevel;
    string mirror; 
    int rotation;
    string imageFilter;
    string exposurePreset;
    int evCompensation;
    string whiteBalance;
    bool burstModeEnabled;
    bool flickerCancellation;
    bool frameStabilization;
    bool doDisableSoftwareSharpen;
    bool doDisableSoftwareSaturation;
    
    int stillPreviewWidth;
    int stillPreviewHeight;
    int stillQuality;
    bool enableRaw;
    bool enableStillPreview;
    string savedPhotosFolderName;
    
    ofxOMXPhotoGrabberListener* photoGrabberListener;
    ofxOMXVideoGrabberListener* videoGrabberListener;
	ofxOMXCameraSettings()
	{
        photoGrabberListener = NULL;
        videoGrabberListener = NULL;
        OMX_Init();
        OMX_Maps::getInstance(); 
        resetValues();
	}
	
    void resetValues()
    {
        width = 1280;
        height = 720;
        framerate = 30;
        enableTexture = true;
        enablePixels = false;
        recordingFilePath = "";
        exposurePreset = "Auto";
        meteringType = "Average";
        autoISO = true;
        ISO = 0;
        autoShutter = true;
        shutterSpeed = 0;
        sharpness=-50;
        contrast=-10;
        brightness=50;
        saturation=0;
        frameStabilization=false;
        flickerCancellation = false;
        whiteBalance="Auto";
        imageFilter="None";
        dreLevel=0;
        cropRectangle.set(0,0,100,100);
        zoomLevel=0;
        rotation=0;
        mirror="None";
        doDisableSoftwareSharpen = false;
        doDisableSoftwareSaturation = false;
        LED = true;
        stillPreviewWidth = 640;
        stillPreviewHeight = 480;
        stillQuality = 100;
        enableRaw = false;
        enableStillPreview = true;
        burstModeEnabled = false;
        savedPhotosFolderName = "photos";
    }
    
    

    string toString()
    {
        stringstream info;
        info << "width " << width << endl;
        info << "height " << height << endl;
        info << "framerate " << framerate << endl;
        info << "enableTexture " << enableTexture << endl;
        info << "enablePixels " << enablePixels << endl;
        info << "exposurePreset " << exposurePreset << endl;
        info << "meteringType " << meteringType << endl;
        info << "autoISO " << autoISO << endl;
        info << "ISO " << ISO << endl;
        info << "autoShutter " << autoShutter << endl;
        info << "shutterSpeed " << shutterSpeed << endl;
        info << "sharpness " << sharpness << endl;
        info << "contrast " << contrast << endl;
        info << "brightness " << brightness << endl;
        info << "saturation " << saturation << endl;
        info << "frameStabilization " << frameStabilization << endl;
        info << "flickerCancellation " << flickerCancellation << endl;
        info << "dreLevel " << dreLevel << endl;
        info << "cropRectangle " << cropRectangle << endl;
        info << "zoomLevel " << zoomLevel << endl;
        info << "rotation " << rotation << endl;
        info << "mirror " << mirror << endl;
        info << "doDisableSoftwareSharpen " << doDisableSoftwareSharpen << endl;
        info << "doDisableSoftwareSaturation " << doDisableSoftwareSaturation << endl;
        info << "LED " << LED << endl;
        info << "stillPreviewWidth " << stillPreviewWidth << endl;
        info << "stillPreviewHeight " << stillPreviewHeight << endl;
        info << "stillQuality " << stillQuality << endl;
        info << "enableRaw " << enableRaw << endl;
        info << "enableStillPreview " << enableStillPreview << endl;
        info << "burstModeEnabled " << burstModeEnabled << endl;
        info << "savedPhotosFolderName " << savedPhotosFolderName << endl;

        
        return info.str();
    }
    
    bool exists(ofJson& json, const string& key)
    {
        return json.find(key) != json.end();
    }
    bool parseJSON(const string& jsonString)
    {
        ofLog() << "jsonString: " << jsonString;
        
        ofJson json = ofJson::parse(jsonString);
        
        if(json.is_null())
        {
            return false;
        }
        if(exists(json, "width")) width = json["width"].get<int>();
        if(exists(json, "height")) height = json["height"].get<int>();
        if(exists(json, "framerate")) framerate = json["framerate"].get<int>();
        
        if(exists(json, "enableTexture")) enableTexture = json["enableTexture"].get<bool>();
        if(exists(json, "enablePixels")) enablePixels = json["enablePixels"].get<bool>();
        
        if(exists(json, "meteringType")) meteringType = json["meteringType"].get<string>();
        
        if(exists(json, "autoISO")) autoISO = json["autoISO"].get<bool>();
        if(exists(json, "ISO")) ISO = json["ISO"].get<int>();
        
        
        if(exists(json, "autoShutter")) autoShutter = json["autoShutter"].get<bool>();
        if(exists(json, "shutterSpeed")) shutterSpeed = json["shutterSpeed"].get<int>();
        
        if(exists(json, "sharpness")) sharpness = json["sharpness"].get<int>();
        if(exists(json, "contrast")) contrast = json["contrast"].get<int>();
        if(exists(json, "brightness")) brightness = json["brightness"].get<int>();
        if(exists(json, "saturation")) saturation = json["saturation"].get<int>();

        
        if(exists(json, "frameStabilization")) frameStabilization = json["frameStabilization"].get<bool>();
        if(exists(json, "flickerCancellation")) flickerCancellation = json["flickerCancellation"].get<bool>();
        if(exists(json, "dreLevel")) dreLevel = json["dreLevel"].get<int>();
        if(exists(json, "cropRectangle"))
        {
            cropRectangle.set(json["cropRectangle"]["x"].get<int>(),
                              json["cropRectangle"]["y"].get<int>(),
                              json["cropRectangle"]["width"].get<int>(),
                              json["cropRectangle"]["height"].get<int>());

        }
        if(exists(json, "zoomLevel")) zoomLevel = json["zoomLevel"].get<float>();
        if(exists(json, "rotation")) rotation = json["rotation"].get<int>();
        if(exists(json, "mirror")) mirror = json["mirror"].get<string>();
        if(exists(json, "doDisableSoftwareSharpen")) doDisableSoftwareSharpen = json["doDisableSoftwareSharpen"].get<bool>();
        if(exists(json, "doDisableSoftwareSaturation")) doDisableSoftwareSaturation = json["doDisableSoftwareSaturation"].get<bool>();
        if(exists(json, "LED")) LED = json["LED"].get<bool>();
        if(exists(json, "stillPreviewWidth")) stillPreviewWidth = json["stillPreviewWidth"].get<int>();
        if(exists(json, "stillPreviewHeight")) stillPreviewHeight = json["stillPreviewHeight"].get<int>();
        if(exists(json, "stillQuality")) stillQuality = json["stillQuality"].get<int>();
        if(exists(json, "enableRaw")) enableRaw = json["enableRaw"].get<bool>();
        if(exists(json, "enableStillPreview")) enableStillPreview = json["enableStillPreview"].get<bool>();
        if(exists(json, "burstModeEnabled")) burstModeEnabled = json["burstModeEnabled"].get<bool>();
        if(exists(json, "savedPhotosFolderName")) savedPhotosFolderName = json["savedPhotosFolderName"].get<string>();

        
        
        return true;


    }   
    ofJson toJSON()
    {
        ofJson result;
        
        result["width"]=width;
        result["height"]=height;
        result["framerate"]=framerate;
        result["enableTexture"]=enableTexture;
        result["enablePixels"]=enablePixels;
        result["exposurePreset"]=exposurePreset;
        result["meteringType"]=meteringType;
        result["autoISO"]=autoISO;
        result["ISO"]=ISO;
        result["autoShutter"]=autoShutter;
        result["shutterSpeed"]=shutterSpeed;
        result["sharpness"]=sharpness;
        result["contrast"]=contrast;
        result["brightness"]=brightness;
        result["saturation"]=saturation;
        result["frameStabilization"]=frameStabilization;
        result["flickerCancellation"]=flickerCancellation;
        result["dreLevel"]=dreLevel;
        
        result["cropRectangle"]["x"]= cropRectangle.x;
        result["cropRectangle"]["y"]= cropRectangle.y;
        result["cropRectangle"]["width"]= cropRectangle.width;
        result["cropRectangle"]["height"]= cropRectangle.height;
        result["zoomLevel"]=zoomLevel;
        result["rotation"]=rotation;
        result["mirror"]=mirror;
        result["doDisableSoftwareSharpen"]=doDisableSoftwareSharpen;
        result["doDisableSoftwareSaturation"]=doDisableSoftwareSaturation;
        result["LED"]=LED;
        result["stillPreviewWidth"]=stillPreviewWidth;
        result["stillPreviewHeight"]=stillPreviewHeight;
        result["stillQuality"]=stillQuality;
        result["enableRaw"]=enableRaw;
        result["enableStillPreview"]=enableStillPreview;
        result["burstModeEnabled"]=burstModeEnabled;
        result["savedPhotosFolderName"]=savedPhotosFolderName;

        
        
        return result;
    }
    
    void saveJSONFile(string path="")
    {
        if(path == "")
        {
            path = "settings.json";
        }
        ofSavePrettyJson(path, toJSON());
    }
/*
    https://www.raspberrypi.org/blog/new-camera-mode-released/
    
    2592×1944 1-15fps, video or stills mode, Full sensor full FOV, default stills capture
    1920×1080 1-30fps, video mode, 1080p30 cropped
    1296×972 1-42fps, video mode, 4:3 aspect binned full FOV. Used for stills preview in raspistill.
    1296×730 1-49fps, video mode, 16:9 aspect , binned, full FOV (width), used for 720p
    640×480 42.1-60fps, video mode, up to VGAp60 binned
    640×480 60.1-90fps, video mode, up to VGAp90 binned
 */
    enum RPI_CAMERA_PRESET 
    {
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
    void applyPreset(RPI_CAMERA_PRESET preset)
    {
        switch(preset)
        {
            case PRESET_1080P_30FPS_TEXTURE :
            case PRESET_HIGHEST_RES_TEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                enableTexture=false;
                break;
            }
                
            case PRESET_1080P_30FPS_NONTEXTURE :
            case PRESET_HIGHEST_RES_NONTEXTURE :
            {
                width = 1920;
                height = 1080;
                framerate = 30;
                enableTexture=false;
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
                enableTexture=true;
                break;
            }
                
            case PRESET_720P_40FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 40;
                enableTexture=false;
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
                enableTexture=true;
                break;
            }
                
            case PRESET_720P_30FPS_NONTEXTURE :
            {
                width = 1280;
                height = 720;
                framerate = 30;
                enableTexture=false;
                break;
            }
                
            case PRESET_480P_90FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                enableTexture=true;
                break;
            } 
            case PRESET_480P_60FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                enableTexture=true;
                break;
            }    
                
            case PRESET_480P_40FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                enableTexture=true;
                break;
            }    
                
                
            case PRESET_480P_30FPS_TEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                enableTexture=true;
                break;
            }
                
            case PRESET_480P_90FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 90;
                enableTexture=false;
                break;
            } 
            case PRESET_480P_60FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 60;
                enableTexture=false;
                break;
            } 
            case PRESET_480P_40FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 40;
                enableTexture=false;
                break;
            } 
                
            case PRESET_480P_30FPS_NONTEXTURE :
            {
                width = 640;
                height = 480;
                framerate = 30;
                enableTexture=false;
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
