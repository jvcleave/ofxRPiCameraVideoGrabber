#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);			
	consoleListener.setup(this);
    
    omxCameraSettings.width = 640;
    omxCameraSettings.height = 480;
    omxCameraSettings.framerate = 30;
    omxCameraSettings.enableTexture = true;
    omxCameraSettings.enablePixels = true;
    videoGrabber.setup(omxCameraSettings);
    
    
    threshold1 = 0;
    threshold2 = 0;
    apertureSize = 3;
    L2gradient = false;

    cameraMat = cv::Mat(cvSize(omxCameraSettings.width,
                               omxCameraSettings.height),
                        CV_8UC4,
                        videoGrabber.getPixels(),
                        cv::Mat::AUTO_STEP);

}	

//--------------------------------------------------------------
void ofApp::update()
{
    if(videoGrabber.isFrameNew())
    {
        cvtColor(cameraMat, grayMat, CV_RGBA2GRAY);
        cv::Canny(grayMat, edgeMat, threshold1, threshold2, apertureSize, L2gradient);
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
	
	videoGrabber.draw();
    ofxCv::drawMat(edgeMat, videoGrabber.getWidth(), 0);
	stringstream info;
	info << "APP FPS: " << ofGetFrameRate() << endl;
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< endl;
    
	info << endl;
    info << "PRESS 1 TO INCREASE threshold1: " << threshold1 << endl;
    info << "PRESS 2 TO DECREASE threshold1: " << threshold1 << endl;
    info << "PRESS 3 TO INCREASE threshold2: " << threshold2 << endl;
    info << "PRESS 4 TO DECREASE threshold2: " << threshold2 << endl;
    info << "PRESS 5 TO TOGGLE L2gradient: " << L2gradient << endl;
    info << "PRESS r TO RESET: " << endl;

    ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
    

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
    switch (key) 
    {
        case '1':
        {
            threshold1++;
            break;
        }
        case '2':
        {
            threshold1--;
            break;
        }  
        case '3':
        {
            threshold2++;
            break;
        } 
        case '4':
        {
            threshold2--;
            break;
        }
        case '5':
        {
            L2gradient = !L2gradient;
            break;
        }
        case 'r':
        {
            threshold1 = 0;
            threshold2 = 0;
            L2gradient = false;
            break;
        } 
#if 0
        case '5':
        {
            //crashes
            apertureSize++;
            break;
        }
        case '6':
        {
            //crashes
            apertureSize--;
            break;
        }
#endif       
        default:
        {
            break;
        }
        
    }
}

