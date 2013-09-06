/*
 *  ofxRPiCameraVideoGrabber.h
 *
 *  Created by jason van cleave on 6/1/13.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

class ofxRPiCameraVideoGrabber
{

public:
	ofxRPiCameraVideoGrabber();

	void setup(int videoWidth, int videoHeight, int framerate);
	void draw();
	
	void applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter);
	ofTexture& getTextureReference();

	void setSharpness(int sharpness_);
	void setContrast(int contrast_);
	void setBrightness(int brightness_);
	void setSaturation(int saturation_);
	
	void setFrameStabilization(bool doStabilization);
	void setMeteringMode(OMX_METERINGTYPE meteringType, int evCompensation=0, int sensitivity = 100, bool autoSensitivity = false);
	void setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode);
	
	void setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType);
	void setColorEnhancement(bool doColorEnhance, int U=128, int V=128);
	void setLEDStatus(bool status);
	
	GLuint getTextureID();
	
	int getWidth();
	int getHeight();
	int getFrameRate();
	bool isReady();
	int frameCounter;
	
private:
	void onCameraEventParamOrConfigChanged();
	OMX_ERRORTYPE disableAllPortsForComponent(OMX_HANDLETYPE* m_handle);
	
	OMX_HANDLETYPE camera;
	OMX_HANDLETYPE render;
		
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	
	static OMX_ERRORTYPE renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	static OMX_ERRORTYPE renderEmptyBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE renderFillBufferDone	(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	
	void generateEGLImage();
	GLuint textureID;
	ofTexture tex;
	
	int videoWidth;
	int videoHeight;
	int framerate;
	
	EGLImageKHR eglImage;
	EGLDisplay display;
	EGLContext context;
	
	OMX_BUFFERHEADERTYPE* eglBuffer;
	
	bool ready;
	
	
	static string LOG_NAME;
	
	int sharpness;	//	-100 to 100
	int contrast;	//  -100 to 100 
	int brightness; //     0 to 100
	int saturation; //  -100 to 100 
	
	void close();
	

	
};