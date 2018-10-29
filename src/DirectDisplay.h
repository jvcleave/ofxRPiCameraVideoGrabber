#pragma once

#include "ofMain.h"
#include "OMX_Maps.h"



class DirectDisplay
{
public:
    
    OMX_CONFIG_DISPLAYREGIONTYPE displayConfig;
    OMX_CONFIG_DISPLAYREGIONTYPE displayConfigDefaults;
    bool isReady;
    
    OMX_HANDLETYPE renderComponent;
    

    
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
    int layer;
    
    DirectDisplay()
    {

        isReady = false;
        doFullScreen=false;
        noAspectRatio=false;
        doMirror=false;
        rotationIndex=0;
        
        doHDMISync = true;
        alpha = 255;
        layer = 0;
        doMirror = false;
        rotationIndex = 0;
        rotationDegrees =0; 
        doForceFill = false;
        
        OMX_INIT_STRUCTURE(displayConfig);
        displayConfig.nPortIndex = VIDEO_RENDER_INPUT_PORT;
        displayConfigDefaults = displayConfig;
    };
    
    OMX_ERRORTYPE setup(OMX_HANDLETYPE renderComponent_, int x, int y, int width, int height)
    {
        renderComponent = renderComponent_;
        drawRectangle.set(x, y, width, height);
        cropRectangle.set(x, y, width, height);

        
        isReady = true;
        
        OMX_ERRORTYPE error = OMX_ErrorNone;
        error = applyConfig();
        OMX_TRACE(error);
        return error;
    }
    

    
    OMX_ERRORTYPE applyConfig()
    {
        displayConfig.set = (OMX_DISPLAYSETTYPE)(OMX_DISPLAY_SET_DEST_RECT| OMX_DISPLAY_SET_SRC_RECT | OMX_DISPLAY_SET_FULLSCREEN | OMX_DISPLAY_SET_NOASPECT | OMX_DISPLAY_SET_TRANSFORM | OMX_DISPLAY_SET_ALPHA | OMX_DISPLAY_SET_LAYER | OMX_DISPLAY_SET_MODE);
        
        displayConfig.dest_rect.x_offset  = drawRectangle.x;
        displayConfig.dest_rect.y_offset  = drawRectangle.y;
        displayConfig.dest_rect.width     = drawRectangle.getWidth();
        displayConfig.dest_rect.height    = drawRectangle.getHeight();
        
        displayConfig.src_rect.x_offset  = cropRectangle.x;
        displayConfig.src_rect.y_offset  = cropRectangle.y;
        displayConfig.src_rect.width     = cropRectangle.getWidth();
        displayConfig.src_rect.height    = cropRectangle.getHeight();
        
        displayConfig.fullscreen = (OMX_BOOL)doFullScreen;
        displayConfig.noaspect   = (OMX_BOOL)noAspectRatio;    
        displayConfig.transform  = (OMX_DISPLAYTRANSFORMTYPE)rotationIndex;
        //int alpha = (ofGetFrameNum() % 255); 
        displayConfig.alpha  = alpha;
        displayConfig.layer  = layer;

        if(doForceFill)
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

        rotationIndex = (int)type;
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
        
        if(doMirror)
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
        return rotateDisplay(type);
    }
    


    string toString()
    {
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
        
        
        
        
        info << "drawRectangle: " << drawRectangle << endl;
        info << "drawRectangle.getArea(): " << drawRectangle.getArea() << endl;
        
        return info.str();
    }
    
    
    
 
               
};
