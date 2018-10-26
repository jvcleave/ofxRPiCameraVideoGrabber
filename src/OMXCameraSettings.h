#pragma once

#include "ofMain.h"

#define MEGABYTE_IN_BITS 8388608

class OMXCameraSettings
{
public:
	int width;
	int height;
	int framerate;
	bool doRecording;
	
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
    int sharpness;    //    -100 to 100
    int contrast;    //  -100 to 100 
    int brightness; //     0 to 100
    int saturation; //  -100 to 100 
    int dreLevel; 
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
	OMXCameraSettings()
	{
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
        doRecording = false;
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
    }
    
    string toString()
    {
        stringstream info;
        info << "width " << width << endl;
        info << "height " << height << endl;
        info << "framerate " << framerate << endl;
        info << "enableTexture " << enableTexture << endl;
        info << "enablePixels " << enablePixels << endl;
        info << "doRecording " << doRecording << endl;
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
        return info.str();
    }
};
