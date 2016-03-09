#pragma once

#include "BaseEngine.h"



class NonTextureEngine: public BaseEngine
{
public:
	NonTextureEngine();
	~NonTextureEngine();
	void setup(OMXCameraSettings);
    int getFrameCounter();
    
private:
    
    OMX_ERRORTYPE setupDisplay(int x, int y, int width, int height);
    int frameCounter;
    
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	static OMX_ERRORTYPE
    cameraEventHandlerCallback(OMX_HANDLETYPE,
                               OMX_PTR,
                               OMX_EVENTTYPE,
                               OMX_U32,
                               OMX_U32,
                               OMX_PTR);
	static OMX_ERRORTYPE
    encoderFillBufferDone(OMX_IN OMX_HANDLETYPE,
                          OMX_IN OMX_PTR,
                          OMX_IN OMX_BUFFERHEADERTYPE*);

	
    

};