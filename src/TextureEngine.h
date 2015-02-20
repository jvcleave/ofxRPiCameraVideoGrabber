#pragma once

#include "BaseEngine.h"


class TextureEngine : public BaseEngine
{
public:
	TextureEngine();
	void setup(OMXCameraSettings& omxCameraSettings_);
    int getFrameCounter();
    int renderedFrameCounter;
	
	EGLImageKHR eglImage;

	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, 
                                                    OMX_PTR pAppData,  
                                                    OMX_EVENTTYPE eEvent, 
                                                    OMX_U32 nData1, 
                                                    OMX_U32 nData2, 
                                                    OMX_PTR pEventData);
    
	static OMX_ERRORTYPE encoderFillBufferDone(OMX_HANDLETYPE hComponent,
                                               OMX_PTR pAppData,
                                               OMX_BUFFERHEADERTYPE* pBuffer);
    
	static OMX_ERRORTYPE renderFillBufferDone(OMX_HANDLETYPE hComponent,
                                              OMX_PTR pAppData,
                                              OMX_BUFFERHEADERTYPE* pBuffer);
	

};