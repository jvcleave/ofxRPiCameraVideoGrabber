#pragma once

#include "BaseEngine.h"
#include "ofAppEGLWindow.h"


class TextureEngine : public BaseEngine
{
public:
	TextureEngine();
	~TextureEngine();
	void setup(OMXCameraSettings);

	int renderedFrameCounter;
	
	void generateEGLImage();
	
	
    int getFrameCounter();
    
    void enablePixels();
    void disablePixels();
    unsigned char * getPixels();
    void updatePixels();
    unsigned char * pixels;
    bool doPixels;
    ofFbo fbo;
    ofTexture& getTexture()
    {
        return texture;
    }
private:
	EGLImageKHR eglImage;
	EGLDisplay display;
	EGLContext context;
    ofTexture texture;
    
	OMX_BUFFERHEADERTYPE* eglBuffer;
	
	
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE, 
                                                    OMX_PTR,  
                                                    OMX_EVENTTYPE, 
                                                    OMX_U32, 
                                                    OMX_U32, 
                                                    OMX_PTR);
	static OMX_ERRORTYPE encoderFillBufferDone	(OMX_IN OMX_HANDLETYPE, 
                                                 OMX_IN OMX_PTR, 
                                                 OMX_IN OMX_BUFFERHEADERTYPE*);
	static OMX_ERRORTYPE renderFillBufferDone	(OMX_IN OMX_HANDLETYPE, 
                                                 OMX_IN OMX_PTR, 
                                                 OMX_IN OMX_BUFFERHEADERTYPE*);
	

	
};