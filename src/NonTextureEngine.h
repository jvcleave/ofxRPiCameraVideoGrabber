#pragma once

#include "BaseEngine.h"



class NonTextureEngine: public BaseEngine
{
public:
	NonTextureEngine();
	~NonTextureEngine();
	void setup(OMXCameraSettings);

	
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE encoderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

	
    OMX_ERRORTYPE setupDisplay(int x, int y, int width, int height);
	
	int frameCounter;
	int getFrameCounter();
};