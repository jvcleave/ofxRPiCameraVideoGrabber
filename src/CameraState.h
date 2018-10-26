#pragma once
#include "ofMain.h"
#include "OMXCameraSettings.h"


using namespace std;

class CameraState
{
public:
    
    OMXCameraSettings cameraSettings;
    map<string, string> keyValueMap;
    CameraState()
    {
        
    }

    
    void setup(string currentState)
    {
    
        ofBuffer buffer(currentState.c_str(), currentState.size());
        setup(buffer);        
    }
    
    void setup(ofBuffer buffer)
    {
        
        ofJson json;
        
        ofLogVerbose(__func__) << "buffer: " << buffer.getText();
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) 
        {
            string line = *it;
            vector<string> keyValuePairs = ofSplitString(line, " ");
            if(keyValuePairs.size() == 2)
            {
                
                string key = keyValuePairs[0];
                string value = keyValuePairs[1];
                //ofLogVerbose(__func__) << "key: " << key << " value: " << value;
                json[key] = value;
                keyValueMap[key] = value;
            }
        }
        //createSettings
        for(auto iterator = keyValueMap.begin(); iterator != keyValueMap.end(); iterator++) 
        {
            string key = iterator->first;
            string value = iterator->second;
            //ofLogVerbose(__func__) << "key: " << key << " value: " << value;
            
            if(key == "width")              cameraSettings.width                = ofToInt(value);
            if(key == "height")             cameraSettings.height               = ofToInt(value);
            if(key == "framerate")          cameraSettings.framerate            = ofToInt(value);
            if(key == "enableTexture")      cameraSettings.enableTexture       = ofToBool(value);
            if(key == "enablePixels")       cameraSettings.enablePixels         = ofToBool(value);
            if(key == "doRecording")        cameraSettings.doRecording          = ofToBool(value);
            if(key == "recordingFilePath")  cameraSettings.recordingFilePath    = value;
        }
        ofLog() << "json.dump: " << json.dump();
    }
    
    void setup(ofFile file)
    {
        ofLogVerbose(__func__) << "file: " << file.path();

        if(file.exists())
        {
            ofBuffer buffer = ofBufferFromFile(file.path(), false);
            
            //ofBuffer buffer(file);
            setup(buffer);
        }
    }
    
    
};
