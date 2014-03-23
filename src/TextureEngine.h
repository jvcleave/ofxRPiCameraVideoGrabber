#pragma once

#include "BaseEngine.h"
#include "ofAppEGLWindow.h"


class TextureEngine : public BaseEngine
{
public:
	TextureEngine();
	void setup(OMXCameraSettings& omxCameraSettings);

	int renderedFrameCounter;
	
	void generateEGLImage();
	GLuint textureID;
	ofTexture tex;
	
	EGLImageKHR eglImage;
	EGLDisplay display;
	EGLContext context;
	
	OMX_BUFFERHEADERTYPE* eglBuffer;
	
	
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE encoderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE renderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	
	
};