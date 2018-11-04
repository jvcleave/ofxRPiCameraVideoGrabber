#include "ofApp.h"


bool doCrop = false;
bool doRotation = false;
bool doAlpha = false;
bool doMirror = false;
bool doReset = false;

ofRectangle cropRect;
vector<ofRectangle> grids;
int gridCounter = 0;

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	doDrawInfo	= true;
    
	consoleListener.setup(this);
	
	settings.width					= 1280;
	settings.height				= 720;
	settings.enableTexture		= false;
	
	
	
	videoGrabber.setup(settings);
	filterCollection.setup();
    
    int step=200;
    for(int i=0; i<settings.width; i+=step)
    {
        for(int j=0; j<settings.height; j+=step)
        {
            ofRectangle grid(i, j, step, step);
            grids.push_back(grid);
        }
        
        
    }


}
bool forceFill = false;
bool doDisplayInCropArea = true;
//--------------------------------------------------------------
void ofApp::update()
{
    if (doReset) 
    {
        doReset = false;
        doCrop = false;
        doAlpha = false;
        doMirror = false;
        forceFill = false;
        doRotation = false;
        doDisplayInCropArea = true;
        ofRectangle drawRectangle(0,
                                  0, 
                                  videoGrabber.getWidth(),
                                  videoGrabber.getHeight());
        videoGrabber.setDisplayDrawRectangle(drawRectangle);
        videoGrabber.setDisplayCropRectangle(drawRectangle);
        videoGrabber.setDisplayAlpha(255);
        videoGrabber.setDisplayRotation(0);
        videoGrabber.setDisplayMirror(false);
        return;
    }
    
    if(doCrop)
    {
        videoGrabber.setDisplayCropRectangle(grids[gridCounter]);
        
        if (doDisplayInCropArea) 
        {
            //display where the cropping was done
            videoGrabber.setDisplayDrawRectangle(grids[gridCounter]);
        }else
        {
            if(!forceFill)
            {
                //display at top left
                ofRectangle forceFillRectangle(0, 0, grids[gridCounter].width, grids[gridCounter].height);
                videoGrabber.setDisplayDrawRectangle(forceFillRectangle);
            }else
            {
                //fill whole screen
                ofRectangle forceFillRectangle(0, 0, ofGetWidth(), ofGetHeight());
                videoGrabber.setDisplayDrawRectangle(forceFillRectangle); 
            }
            
        }
        
        if(gridCounter+1<grids.size())
        {
            gridCounter++;
        }else
        {
            gridCounter = 0;
        }
    }
    if(doAlpha)
    {
        //range is 0-255
        int alpha = ofGetFrameNum() % 255;
        videoGrabber.setDisplayAlpha(alpha);
        
    }
    videoGrabber.setDisplayMirror(doMirror);

    //mirroring works with rotation as well
    //rotation is actually in 45 degree increments but method adjusts from 0-360
    if (doRotation) 
    {
        int rotation = ofGetFrameNum() % 360;
         videoGrabber.setDisplayRotation(rotation);
    }    
    //displayManager->applyConfig();
}


//--------------------------------------------------------------
void ofApp::draw(){

    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::black);
    for(size_t i=0; i<grids.size(); i++)
    {   
        ofDrawRectangle(grids[i]);
    }
    ofPopStyle();
    
    stringstream info;
    
    info << "PRESS 1 TO CROP" << endl;
    info << "PRESS 2 TO DISPLAY IN CROP AREA" << endl;
    info << "PRESS 3 TO FORCE FILL" << endl;
    info << "PRESS 4 FOR ALPHA" << endl;
    info << "PRESS 5 TO TOGGLE ROTATION" << endl;
    info << "PRESS 6 TO TOGGLE MIRROR " << endl;
    info << "PRESS 7 TO RESET" << endl;
    ofDrawBitmapStringHighlight(info.str(), 60, 20, ofColor::black, ofColor::yellow);

   
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key)
{
	
    ofLog(OF_LOG_VERBOSE, "%c keyPressed", key);
    switch (key) 
    {
            
        case '1':
        {
            doCrop = !doCrop;
            break;
        }
        case '2':
        {
            doDisplayInCropArea = !doDisplayInCropArea;
            break;
        }
        case '3':
        {
            forceFill = !forceFill;
            break;
        }
        case '4':
        {
            doAlpha = !doAlpha;
            break;
        }
        case '5':
        {
            doRotation = !doRotation;
            break;
        }
        case '6':
        {
            doMirror = !doMirror;
            break;
        }
        case '7':
        {
            doReset = true;
            break;
        }
        default:
        {
            break;
        }	
    }

}

void ofApp::onCharacterReceived(KeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

