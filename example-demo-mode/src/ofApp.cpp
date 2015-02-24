#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    doDrawInfo = true;    
    doPrintInfo = false;
    doPresetChange = false;
    //allows keys to be entered via terminal remotely (ssh)
    consoleListener.setup(this);
    

    presets = omxCameraSettings.getAllPresets();
    
    currentPreset = 0;
    omxCameraSettings.preset = presets[currentPreset];
    omxCameraSettings.enablePixels = true;
    
    //pass in the settings and it will start the camera
    videoGrabber.setup(omxCameraSettings);
    
    
    DemoExposureMode* exposureModeDemo = new DemoExposureMode();
    exposureModeDemo->setup(&videoGrabber);
    exposureModeDemo->name = "EXPOSURE MODE DEMO";
    demos.push_back(exposureModeDemo);
    
    DemoMirrorMode* mirrorModeDemo = new DemoMirrorMode();
    mirrorModeDemo->setup(&videoGrabber);
    mirrorModeDemo->name = "MIRROR MODE";
    demos.push_back(mirrorModeDemo);
    
    /*
    DemoCycleExposurePresets* demo1 = new DemoCycleExposurePresets();
    demo1->setup(&videoGrabber);
    demo1->name = "CYCLE EXPOSURE DEMO";
    demos.push_back(demo1);
    
    DemoCycleFilters* demo2 = new DemoCycleFilters();
    demo2->setup(&videoGrabber);
    demo2->name = "CYCLE FILTER DEMO";
    demos.push_back(demo2);
    

    
    DemoBCS* demo4 = new DemoBCS();
    demo4->setup(&videoGrabber);
    demo4->name = "SHARP/BRIGHT/CONTRAST/SATURATION DEMO";
    demos.push_back(demo4);*/
    
    

    
    
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
        doPrintInfo = true;
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
        videoGrabber.saveState();
        videoGrabber.setDefaultValues();
        doNextDemo = false;
        doPrintInfo = true;
        ofLogVerbose() << "isTextureEnabled: " << videoGrabber.isTextureEnabled();
    }else
    {
        currentDemo->update();
    }
    
}


//--------------------------------------------------------------
void ofApp::draw()
{
    if(videoGrabber.isTextureEnabled())
    {
        //draws at camera resolution
        videoGrabber.draw();
        
        //draw a smaller version via the getTextureReference() method
        int drawWidth = videoGrabber.getWidth()/4;
        int drawHeight = videoGrabber.getHeight()/4;
        videoGrabber.getTextureReference().draw(videoGrabber.getWidth()-drawWidth, videoGrabber.getHeight()-drawHeight, drawWidth, drawHeight);
    }
    
    if (doDrawInfo || doPrintInfo) 
    {
        stringstream info;
        info << "\n";
        info << "App FPS: " << ofGetFrameRate() << "\n";
        info << "Camera Resolution: "   << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
        info << currentDemo->name << "\n";
        info << currentDemo->infoString;
        
        info << "\n";
        info << "Press d to Toggle info" << "\n";
        info << "Press i to print info to Terminal" << "\n";
        info << "\n";
        info << "Press P for next Preset" << "\n";
        info << "Press SPACE for next Demo" << "\n";
        if (doDrawInfo) 
        {
            ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
        }
        if (doPrintInfo) 
        {
            ofLogVerbose() << info.str();
            doPrintInfo = false;
        }
    }
    ofColor circleColor;
    if (videoGrabber.isRecording()) 
    {
        circleColor = ofColor::green;
    }else
    {
        circleColor = ofColor::red;
    }
    ofPushStyle();
        ofSetColor(circleColor, 90);
        ofCircle(ofPoint(ofGetWidth() - 200, 100), 50);
    ofPopStyle();
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
        case 'd':
        {
            doDrawInfo = !doDrawInfo;
            break;
        }
        case 'i':
        {
            doPrintInfo = !doPrintInfo;
            break;
        }
        case 'P' :
        {
            doPresetChange = true;
            break;
        }
        case 'S' :
        {
            videoGrabber.saveCurrentStateToFile();
            break;
        }
        case 'L' :
        {
            videoGrabber.loadStateFromFile();
            break;
        }
        case 'R' :
        {
            videoGrabber.resetToCommonState();
            break;
        }
        case '0' :
        {
            
            break;
        }
        case '1' :
        {
            break;
        }
        case '2' :
        {
            break;
        }
        case '3' :
        {
            break;
        }    
        case '4' :
        {
            videoGrabber.setRotation(0);
            break;
        }
        case '5' :
        {
            videoGrabber.rotateCounterClockwise();
            break;
        }
        case '6' :
        {
            videoGrabber.rotateClockwise();
            break;
        }
        case '7' :
        {
            videoGrabber.toggleLED();
            break;
        }
        case '8' :
        {
            videoGrabber.startRecording();
            break;
        }
        case '9' :
        {
            videoGrabber.stopRecording();
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
