#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup()
{
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetLogLevel("ofThread", OF_LOG_ERROR);
    doDrawInfo = true;    
    doPrintInfo = false;
    //allows keys to be entered via terminal remotely (ssh)
    consoleListener.setup(this);

    
    omxCameraSettings.width = 1280;
    omxCameraSettings.height = 720;
    omxCameraSettings.framerate = 30;
    omxCameraSettings.enableTexture = true;
    //pass in the settings and it will start the camera

    videoGrabber.setup(omxCameraSettings);
    int demoCount = 0;

    DemoMetering*  meteringDemo = new DemoMetering();
    meteringDemo->setup(&videoGrabber);
    meteringDemo->name = "METERING";
    demos[demoCount] = meteringDemo;
    demoCount++;
    
    DemoEnhancement* enhancementDemo = new DemoEnhancement();
    enhancementDemo->setup(&videoGrabber);
    enhancementDemo->name = "IMAGE ENHANCEMENT";
    demos[demoCount] = enhancementDemo;
    demoCount++;
    
    DemoZoomCrop* zoomCropDemo = new DemoZoomCrop();
    zoomCropDemo->setup(&videoGrabber);
    zoomCropDemo->name = "ZOOM/CROP";
    demos[demoCount] = zoomCropDemo;
    demoCount++;
    
    DemoMirrorMode* mirrorModeDemo = new DemoMirrorMode();
    mirrorModeDemo->setup(&videoGrabber);
    mirrorModeDemo->name = "MIRROR MODE";
    demos[demoCount] = mirrorModeDemo;
    demoCount++;
    
    DemoRotation* rotationDemo = new DemoRotation();
    rotationDemo->setup(&videoGrabber);
    rotationDemo->name = "ROTATION";
    demos[demoCount] = rotationDemo;
    demoCount++;
    
    DemoFilters* filterDemo = new DemoFilters();
    filterDemo->setup(&videoGrabber);
    filterDemo->name = "FILTERS";
    demos[demoCount] = filterDemo;
    demoCount++;
    
    DemoExposurePresets* exposurePresetDemo = new DemoExposurePresets();
    exposurePresetDemo->setup(&videoGrabber);
    exposurePresetDemo->name = "EXPOSURE PRESETS";    
    demos[demoCount] = exposurePresetDemo;
    demoCount++;

    DemoWhiteBalance*  wbDemo = new DemoWhiteBalance();
    wbDemo->setup(&videoGrabber);
    wbDemo->name = "WHITE BALANCE";
    demos[demoCount] = wbDemo;
    demoCount++;
    
    DemoOptimizations*  optimizationDemo = new DemoOptimizations();
    optimizationDemo->setup(&videoGrabber);
    optimizationDemo->name = "OPTIMIZATION";
    demos[demoCount] = optimizationDemo;
    demoCount++;
    
    doNextDemo = false;
    currentDemoID =0;
    currentDemo = demos[currentDemoID];
}


//--------------------------------------------------------------
void ofApp::update()
{

    
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
        doNextDemo = false;
        doPrintInfo = true;
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
        info << endl;
        info << "App FPS: " << ofGetFrameRate() << endl;
        info << "CAMERA RESOLUTION: "   << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< endl;
        info << endl;
        info << "DEMO: " << currentDemo->name << endl;
        info << endl;
        info << currentDemo->infoString;
        info << endl;
        info << "Press SPACE for next Demo" << endl;
        info << "Press r to reset camera settings" << endl;
        info << "Press z TO START RECORDING" << endl;
        info << "Press x TO STOP RECORDING" << endl;

        if (doDrawInfo) 
        {
            int x = 100;
            if(videoGrabber.getWidth()<1280)
            {
                x = videoGrabber.getWidth();
            }
            ofDrawBitmapStringHighlight(info.str(), x, 40, ofColor(ofColor::black, 50), ofColor::yellow);
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
        ofDrawCircle(ofPoint(ofGetWidth() - 200, 40), 20);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
    ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
    switch (key) 
    {
        case ' ':
        {
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
        case 'p' :
        {
            break;
        }
        case 'r' :
        {
            videoGrabber.reset();
            break;
        }
            
        case 'z' :
        {
            videoGrabber.startRecording();
            break;
        }
        case 'x' :
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
