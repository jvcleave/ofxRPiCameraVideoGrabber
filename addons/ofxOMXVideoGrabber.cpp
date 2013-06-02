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

ofxOMXVideoGrabber::ofxOMXVideoGrabber()
{
	isReady = false;
	isClosed = false;
}
void ofxOMXVideoGrabber::close()
{
	isReady = false;
	/*OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	
	error = OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_INPUT_PORT, NULL);
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_OUTPUT_PORT, NULL);
	
	disableAllPortsForComponent(&render);
	disableAllPortsForComponent(&camera);
	
	OMX_FreeHandle(render);
	OMX_FreeHandle(camera);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "OMX_Deinit PASS";
	}
	error = OMX_Deinit(); 
	*/
	OMX_Deinit();
	if (eglImage != NULL)  
	{
		eglDestroyImageKHR(display, eglImage);
	}
	
	isClosed = true;
}
ofxOMXVideoGrabber::~ofxOMXVideoGrabber()
{
	close();
}


void ofxOMXVideoGrabber::setup(int videoWidth=1280, int videoHeight=720, int framerate=60)
{
	bcm_host_init();
	this->videoWidth = videoWidth;
	this->videoHeight = videoHeight;
	this->framerate = framerate;
	generateEGLImage();
	
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "OMX_Init PASS";
	}

	OMX_CALLBACKTYPE cameraCallbacks;
	cameraCallbacks.EventHandler    = &ofxOMXVideoGrabber::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_GetHandle PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL omx_err(0x%08x)\n", error);
	}
	
	OMX_ERRORTYPE didDisable = disableAllPortsForComponent(&camera);
	if(didDisable == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera didDisable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera didDisable FAIL omx_err(0x%08x)\n", didDisable);
	}
	
	OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
	OMX_INIT_STRUCTURE(cameraCallback);
	cameraCallback.nPortIndex=OMX_ALL;
	cameraCallback.nIndex=OMX_IndexParamCameraDeviceNumber;
	cameraCallback.bEnable = OMX_TRUE;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigRequestCallback PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigRequestCallback FAIL omx_err(0x%08x)\n", error);
	}
	
	
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex = OMX_ALL;
	device.nU32 = 0;
	error =  OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber FAIL omx_err(0x%08x)\n", error);
	}
	
	
	//Set the resolution
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	OMX_INIT_STRUCTURE(portdef);
	portdef.nPortIndex = CAMERA_OUTPUT_PORT;
	
	error = OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_GetParameter OMX_IndexParamPortDefinition PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamPortDefinition FAIL omx_err(0x%08x)\n", error);
	}
	
	portdef.format.image.nFrameWidth = videoWidth;
	portdef.format.image.nFrameHeight = videoHeight;
	portdef.format.image.nStride = videoWidth;
	error = OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetParameter OMX_IndexParamPortDefinition PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter OMX_IndexParamPortDefinition FAIL omx_err(0x%08x)\n", error);
	}
	
	
	//Set the framerate 
	OMX_CONFIG_FRAMERATETYPE framerateConfig;
	OMX_INIT_STRUCTURE(framerateConfig);
	framerateConfig.nPortIndex = CAMERA_OUTPUT_PORT;
	framerateConfig.xEncodeFramerate = framerate << 16; //Q16 format - 25fps
	error = OMX_SetConfig(camera, OMX_IndexConfigVideoFramerate, &framerateConfig);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigVideoFramerate PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigVideoFramerate FAIL omx_err(0x%08x)\n", error);
	}
	
	
	//Set the sharpness 
	OMX_CONFIG_SHARPNESSTYPE sharpness;
	OMX_INIT_STRUCTURE(sharpness);
	sharpness.nPortIndex = OMX_ALL;
	sharpness.nSharpness = -50; //-100 to 100 
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpness);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonSharpness PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonSharpness FAIL omx_err(0x%08x)\n", error);
	}
	//Set the contrast 
	OMX_CONFIG_CONTRASTTYPE contrast;
	OMX_INIT_STRUCTURE(contrast);
	contrast.nPortIndex = OMX_ALL;
	contrast.nContrast = -10; //-100 to 100 
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrast);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonContrast PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonContrast FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set the brightness
	OMX_CONFIG_BRIGHTNESSTYPE brightness;
	OMX_INIT_STRUCTURE(brightness);
	brightness.nPortIndex = OMX_ALL;
	brightness.nBrightness = 50; //0 to 100 
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightness);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonBrightness PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonBrightness FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set the saturation 
	OMX_CONFIG_SATURATIONTYPE saturation;
	OMX_INIT_STRUCTURE(saturation);
	saturation.nPortIndex = OMX_ALL;
	saturation.nSaturation = 0; //-100 to 100 
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturation);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonSaturation PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonSaturation FAIL omx_err(0x%08x)\n", error);
	}
	
	
	//Video stabilisation
	OMX_CONFIG_FRAMESTABTYPE framestab;
	OMX_INIT_STRUCTURE(framestab);
	framestab.nPortIndex = OMX_ALL;
	framestab.bStab = OMX_FALSE;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestab);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonFrameStabilisation PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonFrameStabilisation FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set EV compensation, ISO and metering mode
	OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
	OMX_INIT_STRUCTURE(exposurevalue);
	exposurevalue.nPortIndex = OMX_ALL;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL omx_err(0x%08x)\n", error);
	}	 
	
	ofLog(OF_LOG_VERBOSE, "nSensitivity=%d\n", exposurevalue.nSensitivity);
	
	exposurevalue.xEVCompensation = 0;  //Fixed point value stored as Q16 
	exposurevalue.nSensitivity = 100;	//< e.g. nSensitivity = 100 implies "ISO 100" 
	exposurevalue.bAutoSensitivity = OMX_FALSE;
	exposurevalue.eMetering = OMX_MeteringModeAverage; 
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL omx_err(0x%08x)\n", error);
	}
	
	
	//Set exposure mode
	OMX_CONFIG_EXPOSURECONTROLTYPE exposure;
	OMX_INIT_STRUCTURE(exposure);
	exposure.nPortIndex = OMX_ALL;
	exposure.eExposureControl = OMX_ExposureControlAuto;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposure);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonExposure PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonExposure FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set AWB mode 
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = OMX_WhiteBalControlAuto;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonWhiteBalance PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonWhiteBalance FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set image effect 
	OMX_CONFIG_IMAGEFILTERTYPE imagefilter;
	OMX_INIT_STRUCTURE(imagefilter);
	imagefilter.nPortIndex = OMX_ALL;
	imagefilter.eImageFilter = OMX_ImageFilterNone;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilter);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonImageFilter PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonImageFilter FAIL omx_err(0x%08x)\n", error);
	}
	
	//Set color effect 
	OMX_CONFIG_COLORENHANCEMENTTYPE color;
	OMX_INIT_STRUCTURE(color);
	color.nPortIndex = OMX_ALL;
	color.bColorEnhancement = OMX_FALSE;
	color.nCustomizedU = 128;
	color.nCustomizedV = 128;
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &color);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonColorEnhancement PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonColorEnhancement FAIL omx_err(0x%08x)\n", error);
	}
	
	//Turn off the LED - doesn't work! 
	OMX_CONFIG_PRIVACYINDICATORTYPE privacy;
	OMX_INIT_STRUCTURE(privacy);
	privacy.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
	error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &privacy);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigPrivacyIndicator PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigPrivacyIndicator FAIL omx_err(0x%08x)\n", error);
	}	 
	
}
void ofxOMXVideoGrabber::draw()
{
	tex.draw(0, 0);
}
OMX_ERRORTYPE ofxOMXVideoGrabber::disableAllPortsForComponent(OMX_HANDLETYPE* m_handle)
{
	
	OMX_ERRORTYPE omx_err = OMX_ErrorNone;
	
	
	OMX_INDEXTYPE idxTypes[] = {
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
		omx_err = OMX_GetParameter(*m_handle, idxTypes[i], &ports);
		if(omx_err == OMX_ErrorNone) {
			
			uint32_t j;
			for(j=0; j<ports.nPorts; j++)
			{
				OMX_PARAM_PORTDEFINITIONTYPE portFormat;
				OMX_INIT_STRUCTURE(portFormat);
				portFormat.nPortIndex = ports.nStartPortNumber+j;
				
				omx_err = OMX_GetParameter(*m_handle, OMX_IndexParamPortDefinition, &portFormat);
				if(omx_err != OMX_ErrorNone)
				{
					if(portFormat.bEnabled == OMX_FALSE)
						continue;
				}
				
				omx_err = OMX_SendCommand(*m_handle, OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL);
				if(omx_err != OMX_ErrorNone)
				{
					ofLog(OF_LOG_VERBOSE, "\nCOMXCoreComponent::disableAllPortsForComponent - Error disable port %d on component %s omx_err(0x%08x)", 
						  (int)(ports.nStartPortNumber) + j, "m_componentName", (int)omx_err);
				}
			}
			
		}
	}
	
	
	return OMX_ErrorNone;
}

void ofxOMXVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
	ofLogVerbose() << "applyEffect start";
	OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
	OMX_INIT_STRUCTURE(imagefilterConfig);
	imagefilterConfig.nPortIndex = OMX_ALL;
	imagefilterConfig.eImageFilter = imageFilter;
	OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetConfig OMX_IndexConfigCommonImageFilter PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonImageFilter FAIL omx_err(0x%08x)\n", error);
	}
	

}

void ofxOMXVideoGrabber::generateEGLImage()
{
	eglBuffer = NULL;
	
	//ofDisableArbTex();
	
	
	
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	display = appEGLWindow->getEglDisplay();
	context = appEGLWindow->getEglContext();
	
	
	tex.allocate(videoWidth, videoHeight, GL_RGBA);
	tex.getTextureData().bFlipTexture = false;
	tex.setTextureWrap(GL_REPEAT, GL_REPEAT);
	textureID = tex.getTextureData().textureID;
	
	//TODO - should be a way to use ofPixels for the getPixels() functions?
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
		ofLogVerbose()	<< "Create EGLImage PASS";
	}
}

OMX_ERRORTYPE ofxOMXVideoGrabber::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLog(OF_LOG_VERBOSE, 
		  "ofxOMXVideoGrabber::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
		  __func__, eEvent, nData1, nData2, pEventData);
	switch (eEvent) 
	{
		case OMX_EventCmdComplete:					ofLogVerbose() << __func__ <<  " OMX_EventCmdComplete";					break;
		case OMX_EventError:						ofLogVerbose() << __func__ <<  " OMX_EventError";						break;
		case OMX_EventMark:							ofLogVerbose() << __func__ <<  " OMX_EventMark";						break;
		case OMX_EventPortSettingsChanged:			ofLogVerbose() << __func__ <<  " OMX_EventPortSettingsChanged";			break;
		case OMX_EventBufferFlag:					ofLogVerbose() << __func__ <<  " OMX_EventBufferFlag";					break;
		case OMX_EventResourcesAcquired:			ofLogVerbose() << __func__ <<  " OMX_EventResourcesAcquired";			break;
		case OMX_EventComponentResumed:				ofLogVerbose() << __func__ <<  " OMX_EventComponentResumed";			break;
		case OMX_EventDynamicResourcesAvailable:	ofLogVerbose() << __func__ <<  " OMX_EventDynamicResourcesAvailable";	break;
		case OMX_EventPortFormatDetected:			ofLogVerbose() << __func__ <<  " OMX_EventPortFormatDetected";			break;
		case OMX_EventKhronosExtensions:			ofLogVerbose() << __func__ <<  " OMX_EventKhronosExtensions";			break;
		case OMX_EventVendorStartUnused:			ofLogVerbose() << __func__ <<  " OMX_EventVendorStartUnused";			break;
		case OMX_EventMax:							ofLogVerbose() << __func__ <<  " OMX_EventMax";							break;
		case OMX_EventParamOrConfigChanged:
		{
			ofLogVerbose() << __func__ <<  " OMX_EventParamOrConfigChanged";
			ofxOMXVideoGrabber *grabber = static_cast<ofxOMXVideoGrabber*>(pAppData);
			grabber->onCameraEventParamOrConfigChanged();
			break;
		}			
		default:									ofLogVerbose() << __func__ <<	"DEFAULT";								break;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxOMXVideoGrabber::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{

	switch (eEvent) 
	{
		case OMX_EventCmdComplete:					ofLogVerbose() << __func__ <<  " OMX_EventCmdComplete";					break;
		case OMX_EventError:						ofLogVerbose() << __func__ <<  " OMX_EventError";						break;
		case OMX_EventMark:							ofLogVerbose() << __func__ <<  " OMX_EventMark";						break;
		case OMX_EventPortSettingsChanged:			ofLogVerbose() << __func__ <<  " OMX_EventPortSettingsChanged";			break;
		case OMX_EventBufferFlag:					ofLogVerbose() << __func__ <<  " OMX_EventBufferFlag";					break;
		case OMX_EventResourcesAcquired:			ofLogVerbose() << __func__ <<  " OMX_EventResourcesAcquired";			break;
		case OMX_EventComponentResumed:				ofLogVerbose() << __func__ <<  " OMX_EventComponentResumed";			break;
		case OMX_EventDynamicResourcesAvailable:	ofLogVerbose() << __func__ <<  " OMX_EventDynamicResourcesAvailable";	break;
		case OMX_EventPortFormatDetected:			ofLogVerbose() << __func__ <<  " OMX_EventPortFormatDetected";			break;
		case OMX_EventKhronosExtensions:			ofLogVerbose() << __func__ <<  " OMX_EventKhronosExtensions";			break;
		case OMX_EventVendorStartUnused:			ofLogVerbose() << __func__ <<  " OMX_EventVendorStartUnused";			break;
		case OMX_EventMax:							ofLogVerbose() << __func__ <<  " OMX_EventMax";							break;
		case OMX_EventParamOrConfigChanged:			ofLogVerbose() << __func__ <<  " OMX_EventParamOrConfigChanged";		break;		
		default:									ofLogVerbose() << __func__ <<	"DEFAULT";								break;
	}
		return OMX_ErrorNone;
}


OMX_ERRORTYPE ofxOMXVideoGrabber::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose() << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxOMXVideoGrabber::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	ofxOMXVideoGrabber *grabber = static_cast<ofxOMXVideoGrabber*>(pAppData);
	OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
	//grabber->applyEffect();
	return error;
}

void ofxOMXVideoGrabber::onCameraEventParamOrConfigChanged()
{
	ofLogVerbose() << "onCameraEventParamOrConfigChanged";
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SendCommand OMX_StateIdle PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_StateIdle FAIL omx_err(0x%08x)\n", error);
	}
	
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SetParameter PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter FAIL omx_err(0x%08x)\n", error);
	}
	string componentName = "OMX.broadcom.egl_render";
	
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &ofxOMXVideoGrabber::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone = &ofxOMXVideoGrabber::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone = &ofxOMXVideoGrabber::renderFillBufferDone;
	error = OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
	disableAllPortsForComponent(&render);
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_SendCommand OMX_StateIdle PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL omx_err(0x%08x)\n", error);
	}
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_SetupTunnel PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SetupTunnel FAIL omx_err(0x%08x)\n", error);
	}
	error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SendCommand OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_CommandPortEnable FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_OUTPUT_PORT, NULL);
	
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_SendCommand OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_CommandPortEnable FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_INPUT_PORT, NULL);
	
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_SendCommand EGL_RENDER_INPUT_PORT OMX_CommandPortEnable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand EGL_RENDER_INPUT_PORT OMX_CommandPortEnable FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_UseEGLImage-----> PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_UseEGLImage-----> FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"render OMX_SendCommand OMX_StateExecuting PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateExecuting FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() <<	"camera OMX_SendCommand OMX_StateExecuting PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_StateExecuting FAIL omx_err(0x%08x)\n", error);
	}
	
	error = OMX_FillThisBuffer(render, eglBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose() <<	"render OMX_FillThisBuffer PASS";
		isReady = true;
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_FillThisBuffer FAIL omx_err(0x%08x)", error);
	}
}
