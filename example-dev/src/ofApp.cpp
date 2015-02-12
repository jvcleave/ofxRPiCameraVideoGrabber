#include "ofApp.h"

bool doPrintInfo = false;
bool doPresetChange = false;
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
        
    
    //allows keys to be entered via terminal remotely (ssh)
    consoleListener.setup(this);
    
    
    //new preset option
    presets.push_back(OMXCameraSettings::PRESET_1080P_30FPS_TEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_1080P_30FPS_NONTEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_480P_30FPS_NONTEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_480P_90FPS_TEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_720P_30FPS_TEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_480P_30FPS_TEXTURE);
    presets.push_back(OMXCameraSettings::PRESET_480P_90FPS_NONTEXTURE);
    
    // presets.push_back(OMXCameraSettings::PRESET_480P_30FPS_NONTEXTURE);
    
    currentPreset = 0;
    omxCameraSettings.preset = presets[currentPreset];
    
    //pass in the settings and it will start the camera
    videoGrabber.setup(omxCameraSettings);
    
    DemoCycleExposurePresets* demo1 = new DemoCycleExposurePresets();
    demo1->setup(&videoGrabber);
    demo1->name = "CYCLE EXPOSURE DEMO";
    demos.push_back(demo1);
    
    DemoCycleFilters* demo2 = new DemoCycleFilters();
    demo2->setup(&videoGrabber);
    demo2->name = "CYCLE FILTER DEMO";
    demos.push_back(demo2);
    
    DemoManualMode* demo3 = new DemoManualMode();
    demo3->setup(&videoGrabber);
    demo3->name = "MANUAL MODE DEMO";
    demos.push_back(demo3);
    
    DemoBCS* demo4 = new DemoBCS();
    demo4->setup(&videoGrabber);
    demo4->name = "SHARP/BRIGHT/CONTRAST/SATURATION DEMO";
    demos.push_back(demo4);
    
    
    DemoMeteringModes* demo5 = new DemoMeteringModes();
    demo5->setup(&videoGrabber);
    demo5->name = "METERING MODES";
    demos.push_back(demo5);
    
    DemoFeatureTest* demo6 = new DemoFeatureTest();
    demo6->setup(&videoGrabber);
    demo6->name = "FEATURE TEST";
    demos.push_back(demo6);
    
    
    doNextDemo = false;
    currentDemoID =0;
    currentDemo = demos[currentDemoID];
}


//--------------------------------------------------------------
void ofApp::update()
{
    if (doPresetChange) 
    {
        if((unsigned int)currentPreset+1 < presets.size())
        {
            currentPreset++;
        }else
        {
            currentPreset = 0;
        }
        OMXCameraSettings* settings = new OMXCameraSettings();
        
        settings->preset = presets[currentPreset];
        omxCameraSettings = *settings;
        
        //pass in the settings and it will start the camera
        videoGrabber.setup(omxCameraSettings);
        doPresetChange = false;
    }
    
    if (doNextDemo)
    {
        if((unsigned int) currentDemoID+1 < demos.size())
        {
            currentDemoID++;
        }else
        {
            currentDemoID = 0;
        }
        currentDemo = demos[currentDemoID];
        videoGrabber.setDefaultValues();
        doNextDemo = false;
        doPrintInfo = true;
        
    }else
    {
        currentDemo->update();
    }
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    
    
    
    currentDemo->draw();
    if (doPrintInfo) 
    {
        ofLogVerbose() << currentDemo->infoString;
        doPrintInfo = false;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
    switch (key) 
    {
        case ' ':
        {
            //videoGrabber.printCameraInfo();
            doNextDemo = true;
            break;
        }
        case 'I':
        {
            ofLogVerbose() << currentDemo->infoString;
            break;
        }
        case 'P' :
        {
            doPresetChange = true;
            break;
        }
        case '9' :
        {
            videoGrabber.saveImage();
            break;
        }
        default:
        {
            currentDemo->onKey(key);
            break;
        }
            
    }
}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
    keyPressed((int)e.character);
}
