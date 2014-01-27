#pragma once

#include "ofMain.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include "OMXCameraSettings.h"
#include "OMXCameraUtils.h"



class NonTextureEngine: public ofThread
{
public:
	NonTextureEngine();
	void close();
	void setup(OMXCameraSettings omxCameraSettings);
	OMXCameraSettings omxCameraSettings;
	//OMXCameraUtils omxCameraUtils;

	
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	OMX_HANDLETYPE camera;
	OMX_HANDLETYPE encoder;
	OMX_HANDLETYPE render;
	OMX_HANDLETYPE nullSink;
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	
	static OMX_ERRORTYPE renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE renderEmptyBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE renderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	static OMX_ERRORTYPE encoderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE encoderEmptyBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE encoderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	static OMX_ERRORTYPE nullSinkEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE nullSinkEmptyBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE nullSinkFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	
	
	int frameCounter;
	bool ready;
	bool doRecording;
	
	OMX_BUFFERHEADERTYPE *camera_ppBuffer_in;
	
	OMX_BUFFERHEADERTYPE *encoder_ppBuffer_out;
	
	void readFrame();
	void writeFile();
	bool want_quit;
	
	ofBuffer tmpBuffer;
	
	bool didWriteFile;
	
	int recordingBitRate;
	int MEGABYTE_IN_BITS;
	float numMBps;
	
	int quit_detected;
	int quit_in_keyframe;
	int need_next_buffer_to_be_filled;
	int encoder_output_buffer_available;
	bool usePreview;
	
	
	void threadedFunction();
	
};