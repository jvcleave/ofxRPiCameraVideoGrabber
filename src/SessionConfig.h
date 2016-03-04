#pragma once
#include "ofMain.h"
#include "ofxRPiCameraVideoGrabber.h"


class SessionConfig
{
public:
    
    OMXCameraSettings cameraSettings;
    map<string, string> keyValueMap;
    SessionConfig()
    {
        
    }
    void createSettings()
    {
        for(map<string, string>::iterator it=keyValueMap.begin(); it != keyValueMap.end(); it++) 
        {
            string key = it->first;
            string value = it->second;
            
            if(key == "width")
            {
                cameraSettings.width = ofToInt(value);
            }
            if(key == "height")
            {
                cameraSettings.height = ofToInt(value);
            }
            if(key == "framerate")
            {
                cameraSettings.framerate = ofToInt(value);
            }
            if(key == "isUsingTexture")
            {
                cameraSettings.isUsingTexture = ofToBool(value);
            }
            if(key == "enablePixels")
            {
                cameraSettings.enablePixels = ofToBool(value);
            }
            if(key == "doRecording")
            {
                cameraSettings.doRecording = ofToBool(value);
            }
            if(key == "previewWidth")
            {
                cameraSettings.previewWidth = ofToInt(value);
            }
            if(key == "previewHeight")
            {
                cameraSettings.previewHeight = ofToInt(value);
            }
            if(key == "recordingFilePath")
            {
                cameraSettings.recordingFilePath = value;
            }
            if(key == "doConvertToMKV")
            {
                cameraSettings.doConvertToMKV = ofToBool(value);
            }

        }
    }
    
    void applySettings(ofxRPiCameraVideoGrabber* videoGrabber)
    {
        if(!videoGrabber) return;
        for(map<string, string>::iterator it=keyValueMap.begin(); it != keyValueMap.end(); it++) 
        {
            string key = it->first;
            string value = it->second;

            if(key == "sharpness")
            {
                videoGrabber->setSharpness(ofToInt(value));
            }
            if(key == "contrast")
            {
                videoGrabber->setContrast(ofToInt(value));
            }   
            if(key == "brightness")
            {
                videoGrabber->setBrightness(ofToInt(value));
            }   
            if(key == "saturation")
            {
                videoGrabber->setSaturation(ofToInt(value));
            }
            if(key == "ISO")
            {
                videoGrabber->setISO(ofToInt(value));
            } 
            if(key == "AutoISO")
            {
                videoGrabber->setAutoISO(ofToBool(value));
            } 
            if(key == "DRE")
            {
                videoGrabber->setDRE(ofToInt(value));
            } 
            if(key == "cropRectangle")
            {
                vector<string> rectValues = ofSplitString(value, ",");
                if(rectValues.size() == 4)
                {
                    videoGrabber->setSensorCrop(ofToInt(rectValues[0]),
                                               ofToInt(rectValues[1]),
                                               ofToInt(rectValues[2]),
                                               ofToInt(rectValues[3])); 
                }
                
            }
            if(key == "zoomLevelNormalized")
            {
                videoGrabber->setZoomLevelNormalized(ofToFloat(value));
            }
            
            if(key == "mirror")
            {
                videoGrabber->setMirror(value);
            }
            if(key == "rotation")
            {
                videoGrabber->setRotation(ofToInt(value));
            }   
            
            if(key == "imageFilter")
            {
                videoGrabber->setImageFilter(value);
            }

            if(key == "exposurePreset")
            {
                videoGrabber->setExposurePreset(value);
            }

            if(key == "evCompensation")
            {
                videoGrabber->setEvCompensation(ofToInt(value));
            }
            
            if(key == "autoShutter")
            {
                videoGrabber->setAutoShutter(ofToBool(value));
            }
            
            if(key == "shutterSpeed")
            {
                videoGrabber->setShutterSpeed(ofToInt(value));
            }
            
            if(key == "meteringType")
            {
                videoGrabber->setMeteringType(value);
            }
            
            if(key == "SoftwareSaturationEnabled")
            {
                videoGrabber->setSoftwareSaturation(ofToBool(value));
            }
            
            if(key == "SoftwareSharpeningEnabled")
            {
                videoGrabber->setSoftwareSharpening(ofToBool(value));
            }
        }
                
}
    
    void loadFile(string filePath, ofxRPiCameraVideoGrabber* videoGrabber)
    {
        if(!videoGrabber) return;
        
        ofFile file(filePath);
        if(file.exists())
        {
            ofBuffer buffer(file);
            while(buffer.isLastLine() == false) 
            {
                
                string line = buffer.getNextLine();
                if(!line.empty())
                {
                    vector<string> keyValuePairs = ofSplitString(line, " ");
                    if(keyValuePairs.size() == 2)
                    {
                        
                        string key = keyValuePairs[0];
                        string value = keyValuePairs[1]; 
                        keyValueMap[key] = value;
                    }
                }
            }
            createSettings();
            videoGrabber->setup(cameraSettings);
            applySettings(videoGrabber);
            
        }
    }
    
    
};