#pragma once

#include "ofMain.h"
#include "OMX_Maps.h"

class DirectDisplayOptions
{
public:
    
    DirectDisplayOptions()
    {
        doFullScreen=false;
        noAspectRatio=false;
        doMirror=false;
        rotationIndex=0;
        
        doHDMISync = true;
        alpha = 255;
        doMirror = false;
        rotationIndex = 0;
        rotationDegrees =0; 
        doForceFill = false;
    };
    
    //decoder related
    
    bool doHDMISync;
    
    ofRectangle drawRectangle;
    ofRectangle cropRectangle;
    bool doFullScreen;
    bool noAspectRatio;
    bool doMirror;
    int rotationIndex;
    int rotationDegrees;
    int alpha;
    bool doForceFill;
};

class DirectDisplay
{
public:
    
    OMX_CONFIG_DISPLAYREGIONTYPE displayConfig;
    OMX_CONFIG_DISPLAYREGIONTYPE displayConfigDefaults;
    DirectDisplayOptions options;
    bool isReady;
    bool previousMirror;
    int previousRotationDegrees;
    
    OMX_HANDLETYPE renderComponent;
    
    DirectDisplay()
    {

        isReady = false;
        previousRotationDegrees = 0;
        OMX_INIT_STRUCTURE(displayConfig);
        displayConfigDefaults = displayConfig;
    };
    
    ~DirectDisplay()
    {
        ofRemoveListener(ofEvents().update, this, &DirectDisplay::onUpdate);
    }
    
    
    DirectDisplayOptions& getConfig()
    {
        return options;
    }
    
    OMX_ERRORTYPE setup(OMX_HANDLETYPE renderComponent_, int x, int y, int width, int height)
    {
        
        
        renderComponent = renderComponent_;
        options.drawRectangle.set(x, y, width, height);
        options.cropRectangle.set(x, y, width, height);
        previousMirror = options.doMirror;        

        displayConfig.nPortIndex = VIDEO_RENDER_INPUT_PORT;
        
        isReady = true;
        
        OMX_ERRORTYPE error = OMX_ErrorNone;
        error = applyConfig();
        OMX_TRACE(error);
        
        ofAddListener(ofEvents().update, this, &DirectDisplay::onUpdate);

        return error;
        
    }
    
    void onUpdate(ofEventArgs& args)
    {
        if(!isReady)
        {
            return;
        }
        
        if(previousMirror != options.doMirror)
        {
            rotateDisplay(previousRotationDegrees);
            previousMirror = options.doMirror;
        }
        
        OMX_ERRORTYPE error =applyConfig();
        OMX_TRACE(error); 

#ifdef DEBUG_VIDEO_DISPLAY
        stringstream info;
        info << "fullscreen: " << displayConfig.fullscreen << endl; 
        info << "noaspect: " << displayConfig.noaspect << endl;
        info << "src_rect x: " << displayConfig.src_rect.x_offset << endl;  
        info << "src_rect y: " << displayConfig.src_rect.y_offset << endl;  
        info << "src_rect width: " << displayConfig.src_rect.width << endl;    
        info << "src_rect height: " << displayConfig.src_rect.height << endl;    
        
        info << "dest_rect x: " << displayConfig.dest_rect.x_offset << endl; 
        info << "dest_rect y: " << displayConfig.dest_rect.y_offset << endl; 
        info << "dest_rect width: " << displayConfig.dest_rect.width << endl;    
        info << "dest_rect height: " << displayConfig.dest_rect.height << endl;
        
        info << "transform: " << displayConfig.transform << endl;
        
        
        info << "transform: " << displayConfig.transform << endl;
        
        info << "mode: " << displayConfig.mode << endl;
        
        info << "layer: " << displayConfig.layer << endl;
        info << "alpha: " << displayConfig.alpha << endl;
        
        
    
        
        ofLogVerbose() << info.str();
        ofLogVerbose() << "options.drawRectangle: " << options.drawRectangle;
        ofLogVerbose() << "options.drawRectangle.getArea(): " << options.drawRectangle.getArea();
#endif
        
    }
    
    OMX_ERRORTYPE applyConfig()
    {
        displayConfig.set = (OMX_DISPLAYSETTYPE)(OMX_DISPLAY_SET_DEST_RECT| OMX_DISPLAY_SET_SRC_RECT | OMX_DISPLAY_SET_FULLSCREEN | OMX_DISPLAY_SET_NOASPECT | OMX_DISPLAY_SET_TRANSFORM | OMX_DISPLAY_SET_ALPHA | OMX_DISPLAY_SET_MODE);
        
        displayConfig.dest_rect.x_offset  = options.drawRectangle.x;
        displayConfig.dest_rect.y_offset  = options.drawRectangle.y;
        displayConfig.dest_rect.width     = options.drawRectangle.getWidth();
        displayConfig.dest_rect.height    = options.drawRectangle.getHeight();
        
        displayConfig.src_rect.x_offset  = options.cropRectangle.x;
        displayConfig.src_rect.y_offset  = options.cropRectangle.y;
        displayConfig.src_rect.width     = options.cropRectangle.getWidth();
        displayConfig.src_rect.height    = options.cropRectangle.getHeight();
        
        displayConfig.fullscreen = (OMX_BOOL)options.doFullScreen;
        displayConfig.noaspect   = (OMX_BOOL)options.noAspectRatio;    
        displayConfig.transform  = (OMX_DISPLAYTRANSFORMTYPE)options.rotationIndex;
        //int alpha = (ofGetFrameNum() % 255); 
        displayConfig.alpha  = options.alpha;

        if(options.doForceFill)
        {
            displayConfig.mode  = OMX_DISPLAY_MODE_FILL;  
        }else
        {
            displayConfig.mode = displayConfigDefaults.mode;
        }
        //displayConfig.mode  = OMX_DISPLAY_MODE_FILL;
        //displayConfig.mode  = (OMX_DISPLAYMODETYPE)ofRandom(0, 5);
        //return OMX_ErrorNone;
        
        OMX_ERRORTYPE error  = OMX_SetParameter(renderComponent, OMX_IndexConfigDisplayRegion, &displayConfig);
        OMX_TRACE(error);
        
        return error;
    }

 
    
    
    OMX_ERRORTYPE rotateDisplay(OMX_DISPLAYTRANSFORMTYPE type)
    {

        options.rotationIndex = (int)type;
        return applyConfig();
    }
    
    
    OMX_ERRORTYPE rotateDisplay(int degreesClockWise)
    {
        OMX_DISPLAYTRANSFORMTYPE type = OMX_DISPLAY_ROT0;
        
        if(degreesClockWise<0)
        {
            type = OMX_DISPLAY_ROT0;
        }
        if(degreesClockWise >=90 && degreesClockWise < 180)
        {
            type = OMX_DISPLAY_ROT90;
        }
        if(degreesClockWise >=180 && degreesClockWise < 270)
        {
            type = OMX_DISPLAY_ROT270;
        }
        
        if(options.doMirror)
        {
            switch (type) 
            {
                case OMX_DISPLAY_ROT0:
                {
                    type = OMX_DISPLAY_MIRROR_ROT0;
                    break;
                }
                case OMX_DISPLAY_ROT90:
                {
                    type = OMX_DISPLAY_MIRROR_ROT90;
                    break;
                }
                case OMX_DISPLAY_ROT270:
                {
                    type = OMX_DISPLAY_MIRROR_ROT270;
                    break;
                }
                    
                default:
                    break;
            }
        }
        previousRotationDegrees = degreesClockWise;
        return rotateDisplay(type);
    }
    
    OMX_ERRORTYPE rotateRandom()
    {
        int randomRotation = ofRandom(0, 7);
        return rotateDisplay((OMX_DISPLAYTRANSFORMTYPE)randomRotation);
    }

    
    
    
    
 
               
};