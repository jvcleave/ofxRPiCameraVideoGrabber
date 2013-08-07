/*
 *  ofxOMXVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxOMXVideoGrabber.h"

#define OMX_INIT_STRUCTURE(a) \
memset(&(a), 0, sizeof(a)); \
(a).nSize = sizeof(a); \
(a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
(a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
(a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
(a).nVersion.s.nStep = OMX_VERSION_STEP

#define EGL_RENDER_INPUT_PORT	220
#define EGL_RENDER_OUTPUT_PORT	221
#define CAMERA_OUTPUT_PORT		71

string ofxOMXVideoGrabber::LOG_NAME = "ofxOMXVideoGrabber";

ofxOMXVideoGrabber::ofxOMXVideoGrabber()
{
	isReady = false;
	isClosed = false;
	eglBuffer = NULL;
}


void ofxOMXVideoGrabber::setup(int videoWidth=1280, int videoHeight=720, int framerate=60)
{
	this->videoWidth = videoWidth;
	this->videoHeight = videoHeight;
	this->framerate = framerate;
	
	generateEGLImage();
	
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "OMX_Init PASS";
	}

	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &ofxOMXVideoGrabber::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_GetHandle PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_GetHandle FAIL error: 0x%08x", error);
	}
	
	OMX_ERRORTYPE didDisable = disableAllPortsForComponent(&camera);
	if(didDisable == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera didDisable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"camera didDisable FAIL error: 0x%08x", didDisable);
	}
	
	OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
	OMX_INIT_STRUCTURE(cameraCallback);
	cameraCallback.nPortIndex	=	OMX_ALL;
	cameraCallback.nIndex		=	OMX_IndexParamCameraDeviceNumber;
	cameraCallback.bEnable		=	OMX_TRUE;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigRequestCallback PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigRequestCallback FAIL error: 0x%08x", error);
	}
	
	
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex	= OMX_ALL;
	device.nU32			= 0;
	
	error =  OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber FAIL error: 0x%08x", error);
	}
	
	
	//Set the resolution
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	OMX_INIT_STRUCTURE(portdef);
	portdef.nPortIndex = CAMERA_OUTPUT_PORT;
	
	error = OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_GetParameter OMX_IndexParamPortDefinition PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_GetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
	}
	
	portdef.format.image.nFrameWidth = videoWidth;
	portdef.format.image.nFrameHeight = videoHeight;
	portdef.format.image.nStride = videoWidth;
	
	error = OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetParameter OMX_IndexParamPortDefinition PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetParameter OMX_IndexParamPortDefinition FAIL error: 0x%08x", error);
	}
	
	
	//Set the framerate 
	OMX_CONFIG_FRAMERATETYPE framerateConfig;
	OMX_INIT_STRUCTURE(framerateConfig);
	framerateConfig.nPortIndex = CAMERA_OUTPUT_PORT;
	framerateConfig.xEncodeFramerate = framerate << 16; //Q16 format - 25fps
	
	error = OMX_SetConfig(camera, OMX_IndexConfigVideoFramerate, &framerateConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigVideoFramerate PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigVideoFramerate FAIL error: 0x%08x", error);
	}
	
	setExposureMode(OMX_ExposureControlAuto);
	setMeteringMode(OMX_MeteringModeAverage);
	setSharpness(-50);
	setContrast(-10);
	setBrightness(50);
	setSaturation(0);
	setFrameStabilization(false);
	setWhiteBalance(OMX_WhiteBalControlAuto);
	applyImageFilter(OMX_ImageFilterNone);
	setColorEnhancement(false);	 
	setLEDStatus(false);
}

void ofxOMXVideoGrabber::setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set exposure mode
	OMX_CONFIG_EXPOSURECONTROLTYPE exposure;
	OMX_INIT_STRUCTURE(exposure);
	exposure.nPortIndex = OMX_ALL;
	exposure.eExposureControl = exposureMode;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposure);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<			"camera OMX_SetConfig OMX_IndexConfigCommonExposure PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonExposure FAIL error: 0x%08x", error);
	}
}
void ofxOMXVideoGrabber::setMeteringMode(OMX_METERINGTYPE meteringType)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set EV compensation, ISO and metering mode
	OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
	OMX_INIT_STRUCTURE(exposurevalue);
	exposurevalue.nPortIndex = OMX_ALL;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<			"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
	}	 
	
	ofLog(OF_LOG_VERBOSE, "nSensitivity=%d\n", exposurevalue.nSensitivity);
	
	exposurevalue.xEVCompensation	= 0;  //Fixed point value stored as Q16 
	exposurevalue.nSensitivity		= 100;	//< e.g. nSensitivity = 100 implies "ISO 100" 
	exposurevalue.bAutoSensitivity	= OMX_FALSE;
	exposurevalue.eMetering = meteringType; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<			"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setSharpness(int sharpness_) //-100 to 100
{
	sharpness = sharpness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
	OMX_INIT_STRUCTURE(sharpnessConfig);
	sharpnessConfig.nPortIndex = OMX_ALL;
	sharpnessConfig.nSharpness = sharpness; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonSharpness PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonSharpness FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setFrameStabilization(bool doStabilization)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_FRAMESTABTYPE framestabilizationConfig;
	OMX_INIT_STRUCTURE(framestabilizationConfig);
	framestabilizationConfig.nPortIndex = OMX_ALL;
	
	if (doStabilization) 
	{
		framestabilizationConfig.bStab = OMX_TRUE;
	}else 
	{
		framestabilizationConfig.bStab = OMX_FALSE;
	}

	error = OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonFrameStabilisation PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonFrameStabilisation FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setContrast(int contrast_ ) //-100 to 100 
{
	contrast = contrast_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_CONTRASTTYPE contrastConfig;
	OMX_INIT_STRUCTURE(contrastConfig);
	contrastConfig.nPortIndex = OMX_ALL;
	contrastConfig.nContrast = contrast; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonContrast PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonContrast FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setBrightness(int brightness_ ) //0 to 100
{
	brightness = brightness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
	OMX_INIT_STRUCTURE(brightnessConfig);
	brightnessConfig.nPortIndex = OMX_ALL;
	brightnessConfig.nBrightness = brightness;  
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonBrightness PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonBrightness FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setSaturation(int saturation_) //-100 to 100
{
	saturation = saturation_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SATURATIONTYPE saturationConfig;
	OMX_INIT_STRUCTURE(saturationConfig);
	saturationConfig.nPortIndex		= OMX_ALL;
	saturationConfig.nSaturation	= saturation_; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonSaturation PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonSaturation FAIL error: 0x%08x", error);
	}
}


void ofxOMXVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = controlType;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonWhiteBalance PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonWhiteBalance FAIL error: 0x%08x", error);
	}
}


void ofxOMXVideoGrabber::setColorEnhancement(bool doColorEnhance)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_COLORENHANCEMENTTYPE color;
	OMX_INIT_STRUCTURE(color);
	color.nPortIndex = OMX_ALL;
	
	if (doColorEnhance) 
	{
		color.bColorEnhancement = OMX_TRUE;
	}else
	{
		color.bColorEnhancement = OMX_FALSE;
	}
	
	color.nCustomizedU = 128;
	color.nCustomizedV = 128;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &color);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonColorEnhancement PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonColorEnhancement FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::setLEDStatus(bool status)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Turn off the LED - doesn't work! 
	OMX_CONFIG_PRIVACYINDICATORTYPE privacy;
	OMX_INIT_STRUCTURE(privacy);
	privacy.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
	error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &privacy);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigPrivacyIndicator PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigPrivacyIndicator FAIL error: 0x%08x", error);
	}
}

void ofxOMXVideoGrabber::draw()
{
	tex.draw(0, 0);
}
OMX_ERRORTYPE ofxOMXVideoGrabber::disableAllPortsForComponent(OMX_HANDLETYPE* handle)
{
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	
	OMX_INDEXTYPE indexTypes[] = 
	{
		OMX_IndexParamAudioInit,
		OMX_IndexParamImageInit,
		OMX_IndexParamVideoInit, 
		OMX_IndexParamOtherInit
	};
	
	OMX_PORT_PARAM_TYPE ports;
	OMX_INIT_STRUCTURE(ports);
	
	int i;
	for(i=0; i < 4; i++)
	{
		error = OMX_GetParameter(*handle, indexTypes[i], &ports);
		if(error == OMX_ErrorNone) {
			
			uint32_t j;
			for(j=0; j<ports.nPorts; j++)
			{
				OMX_PARAM_PORTDEFINITIONTYPE portFormat;
				OMX_INIT_STRUCTURE(portFormat);
				portFormat.nPortIndex = ports.nStartPortNumber+j;
				
				error = OMX_GetParameter(*handle, OMX_IndexParamPortDefinition, &portFormat);
				if(error != OMX_ErrorNone)
				{
					if(portFormat.bEnabled == OMX_FALSE)
					{
						continue;
					}
				}
				
				error = OMX_SendCommand(*handle, OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL);
				if(error != OMX_ErrorNone)
				{
					ofLog(OF_LOG_VERBOSE, "disableAllPortsForComponent - Error disable port %d on component %s error: 0x%08x", 
						  (int)(ports.nStartPortNumber) + j, "m_componentName", (int)error);
				}
			}
			
		}
	}
	
	
	return OMX_ErrorNone;
}

void ofxOMXVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
	ofLogVerbose(LOG_NAME) << "applyEffect start";
	OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
	OMX_INIT_STRUCTURE(imagefilterConfig);
	imagefilterConfig.nPortIndex = OMX_ALL;
	imagefilterConfig.eImageFilter = imageFilter;
	OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetConfig OMX_IndexConfigCommonImageFilter PASS";
	}else
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetConfig OMX_IndexConfigCommonImageFilter FAIL error: 0x%08x", error);
	}
	

}

void ofxOMXVideoGrabber::generateEGLImage()
{
	
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	display = appEGLWindow->getEglDisplay();
	context = appEGLWindow->getEglContext();
	
	
	tex.allocate(videoWidth, videoHeight, GL_RGBA);
	tex.getTextureData().bFlipTexture = false;
	tex.setTextureWrap(GL_REPEAT, GL_REPEAT);
	textureID = tex.getTextureData().textureID;
	//tex.getTextureData().tex_t = 1.0f;
	//tex.getTextureData().tex_u = 1.0f;
	
	glEnable(GL_TEXTURE_2D);
	
	// setup first texture
	int dataSize = videoWidth * videoHeight * 4;
	
	GLubyte* pixelData = new GLubyte [dataSize];
	
	
    memset(pixelData, 0xff, dataSize);  // white texture, opaque
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, videoWidth, videoHeight, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
	
	delete[] pixelData;
	
	
	// Create EGL Image
	eglImage = eglCreateImageKHR(
								 display,
								 context,
								 EGL_GL_TEXTURE_2D_KHR,
								 (EGLClientBuffer)textureID,
								 0);
    glDisable(GL_TEXTURE_2D);
	if (eglImage == EGL_NO_IMAGE_KHR)
	{
		ofLogError()	<< "Create EGLImage FAIL";
		return;
	}
	else
	{
		ofLogVerbose(LOG_NAME)	<< "Create EGLImage PASS";
	}
}

OMX_ERRORTYPE ofxOMXVideoGrabber::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLog(OF_LOG_VERBOSE, 
		  "ofxOMXVideoGrabber::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
		  __func__, eEvent, nData1, nData2, pEventData);
	switch (eEvent) 
	{
		case OMX_EventCmdComplete:					ofLogVerbose(LOG_NAME) <<  " OMX_EventCmdComplete";					break;
		case OMX_EventError:						ofLogVerbose(LOG_NAME) <<  " OMX_EventError";						break;
		case OMX_EventMark:							ofLogVerbose(LOG_NAME) <<  " OMX_EventMark";						break;
		case OMX_EventPortSettingsChanged:			ofLogVerbose(LOG_NAME) <<  " OMX_EventPortSettingsChanged";			break;
		case OMX_EventBufferFlag:					ofLogVerbose(LOG_NAME) <<  " OMX_EventBufferFlag";					break;
		case OMX_EventResourcesAcquired:			ofLogVerbose(LOG_NAME) <<  " OMX_EventResourcesAcquired";			break;
		case OMX_EventComponentResumed:				ofLogVerbose(LOG_NAME) <<  " OMX_EventComponentResumed";			break;
		case OMX_EventDynamicResourcesAvailable:	ofLogVerbose(LOG_NAME) <<  " OMX_EventDynamicResourcesAvailable";	break;
		case OMX_EventPortFormatDetected:			ofLogVerbose(LOG_NAME) <<  " OMX_EventPortFormatDetected";			break;
		case OMX_EventKhronosExtensions:			ofLogVerbose(LOG_NAME) <<  " OMX_EventKhronosExtensions";			break;
		case OMX_EventVendorStartUnused:			ofLogVerbose(LOG_NAME) <<  " OMX_EventVendorStartUnused";			break;
		case OMX_EventMax:							ofLogVerbose(LOG_NAME) <<  " OMX_EventMax";							break;
		case OMX_EventParamOrConfigChanged:
		{
			ofLogVerbose(LOG_NAME) <<  " OMX_EventParamOrConfigChanged";
			ofxOMXVideoGrabber *grabber = static_cast<ofxOMXVideoGrabber*>(pAppData);
			grabber->onCameraEventParamOrConfigChanged();
			break;
		}			
		default:									ofLogVerbose(LOG_NAME) <<	"DEFAULT";								break;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxOMXVideoGrabber::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{

	switch (eEvent) 
	{
		case OMX_EventCmdComplete:					ofLogVerbose(LOG_NAME) <<  " OMX_EventCmdComplete";					break;
		case OMX_EventError:						ofLogVerbose(LOG_NAME) <<  " OMX_EventError";						break;
		case OMX_EventMark:							ofLogVerbose(LOG_NAME) <<  " OMX_EventMark";						break;
		case OMX_EventPortSettingsChanged:			ofLogVerbose(LOG_NAME) <<  " OMX_EventPortSettingsChanged";			break;
		case OMX_EventBufferFlag:					ofLogVerbose(LOG_NAME) <<  " OMX_EventBufferFlag";					break;
		case OMX_EventResourcesAcquired:			ofLogVerbose(LOG_NAME) <<  " OMX_EventResourcesAcquired";			break;
		case OMX_EventComponentResumed:				ofLogVerbose(LOG_NAME) <<  " OMX_EventComponentResumed";			break;
		case OMX_EventDynamicResourcesAvailable:	ofLogVerbose(LOG_NAME) <<  " OMX_EventDynamicResourcesAvailable";	break;
		case OMX_EventPortFormatDetected:			ofLogVerbose(LOG_NAME) <<  " OMX_EventPortFormatDetected";			break;
		case OMX_EventKhronosExtensions:			ofLogVerbose(LOG_NAME) <<  " OMX_EventKhronosExtensions";			break;
		case OMX_EventVendorStartUnused:			ofLogVerbose(LOG_NAME) <<  " OMX_EventVendorStartUnused";			break;
		case OMX_EventMax:							ofLogVerbose(LOG_NAME) <<  " OMX_EventMax";							break;
		case OMX_EventParamOrConfigChanged:			ofLogVerbose(LOG_NAME) <<  " OMX_EventParamOrConfigChanged";		break;		
		default:									ofLogVerbose(LOG_NAME) <<	"DEFAULT";								break;
	}
		return OMX_ErrorNone;
}


OMX_ERRORTYPE ofxOMXVideoGrabber::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose(LOG_NAME) << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxOMXVideoGrabber::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	ofxOMXVideoGrabber *grabber = static_cast<ofxOMXVideoGrabber*>(pAppData);
	OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
	return error;
}

void ofxOMXVideoGrabber::onCameraEventParamOrConfigChanged()
{
	ofLogVerbose(LOG_NAME) <<		"onCameraEventParamOrConfigChanged";
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SendCommand OMX_StateIdle PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SetParameter PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SetParameter FAIL error: 0x%08x", error);
	}
	
	
	
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &ofxOMXVideoGrabber::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &ofxOMXVideoGrabber::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &ofxOMXVideoGrabber::renderFillBufferDone;
	
	string componentName = "OMX.broadcom.egl_render";
	error = OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
	disableAllPortsForComponent(&render);
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_SendCommand OMX_StateIdle PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_SetupTunnel PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_SetupTunnel FAIL error: 0x%08x", error);
	}
	
	error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SendCommand OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SendCommand OMX_CommandPortEnable FAIL error: 0x%08x", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_SendCommand OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_SendCommand OMX_CommandPortEnable FAIL error: 0x%08x", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_INPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_SendCommand EGL_RENDER_INPUT_PORT OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_SendCommand EGL_RENDER_INPUT_PORT OMX_CommandPortEnable FAIL error: 0x%08x", error);
	}
	
	error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_UseEGLImage-----> PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_UseEGLImage-----> FAIL error: 0x%08x", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_SendCommand OMX_StateExecuting PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_SendCommand OMX_StateExecuting FAIL error: 0x%08x", error);
	}
	
	error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) <<	"camera OMX_SendCommand OMX_StateExecuting PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR,			"camera OMX_SendCommand OMX_StateExecuting FAIL error: 0x%08x", error);
	}
	
	error = OMX_FillThisBuffer(render, eglBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(LOG_NAME) <<	"render OMX_FillThisBuffer PASS";
		isReady = true;
	}else 
	{
		ofLog(OF_LOG_ERROR,			"render OMX_FillThisBuffer FAIL omx_err(0x%08x)", error);
	}
}

void ofxOMXVideoGrabber::close()
{
	isReady = false;
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "camera OMX_StateIdle PASS";
	}else 
	{
		ofLogVerbose(LOG_NAME) << "camera OMX_StateIdle FAIL";
		
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_StateIdle PASS";
	}else 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_StateIdle FAIL";
		
	}
	
	error = OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "camera OMX_CommandFlush PASS";
	}else 
	{
		ofLogVerbose(LOG_NAME) << "camera OMX_CommandFlush FAIL";
		
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_INPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_CommandFlush PASS";
	}else 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_CommandFlush FAIL";
		
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_CommandFlush EGL_RENDER_OUTPUT_PORT PASS";
	}else 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_CommandFlush EGL_RENDER_OUTPUT_PORT FAIL";
		
	}
	disableAllPortsForComponent(&render);
	disableAllPortsForComponent(&camera);
	
	error = OMX_FreeHandle(render);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_FreeHandle PASS";
		
	}
	
	error = OMX_FreeHandle(camera);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "camera OMX_FreeHandle PASS";
		
	}
	
	error = OMX_Deinit(); 
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "OMX_Deinit PASS";
	}
	
	
	if (eglImage != NULL)  
	{
		eglDestroyImageKHR(display, eglImage);
	}
	
	isClosed = true;
}

