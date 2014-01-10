#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include "OMXCameraSettings.h"
#include "OMXCameraUtils.h"

class TextureEngine
{
public:
	TextureEngine();
	void setup(OMXCameraSettings omxCameraSettings);
	OMXCameraSettings omxCameraSettings;
	//OMXCameraUtils omxCameraUtils;

	void generateEGLImage();
	GLuint textureID;
	ofTexture tex;
	
	EGLImageKHR eglImage;
	EGLDisplay display;
	EGLContext context;
	
	OMX_BUFFERHEADERTYPE* eglBuffer;
	
	
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	OMX_HANDLETYPE camera;
	OMX_HANDLETYPE render;
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	
	static OMX_ERRORTYPE renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE renderEmptyBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE renderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	
	int frameCounter;
	bool ready;
};