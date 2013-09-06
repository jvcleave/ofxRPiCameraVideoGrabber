/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"

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

string ofxRPiCameraVideoGrabber::LOG_NAME = "ofxRPiCameraVideoGrabber";

ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
	ready		= false;
	eglBuffer	= NULL;
	videoWidth	= 0;
	videoHeight	= 0;
	framerate	= 0;
	textureID	= 0;
	frameCounter = 0;
}


void ofxRPiCameraVideoGrabber::setup(int videoWidth=1280, int videoHeight=720, int framerate=60)
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
	cameraCallbacks.EventHandler    = &ofxRPiCameraVideoGrabber::cameraEventHandlerCallback;
	
	string cameraComponentName = "OMX.broadcom.camera";
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), this , &cameraCallbacks);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL error: 0x%08x", error);
	}
	
	disableAllPortsForComponent(&camera);
	
	OMX_CONFIG_REQUESTCALLBACKTYPE cameraCallback;
	OMX_INIT_STRUCTURE(cameraCallback);
	cameraCallback.nPortIndex	=	OMX_ALL;
	cameraCallback.nIndex		=	OMX_IndexParamCameraDeviceNumber;
	cameraCallback.bEnable		=	OMX_TRUE;
	
	OMX_SetConfig(camera, OMX_IndexConfigRequestCallback, &cameraCallback);
	
	//Set the camera (always 0)
	OMX_PARAM_U32TYPE device;
	OMX_INIT_STRUCTURE(device);
	device.nPortIndex	= OMX_ALL;
	device.nU32			= 0;
	
	OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &device);
	
	//Set the resolution
	OMX_PARAM_PORTDEFINITIONTYPE portdef;
	OMX_INIT_STRUCTURE(portdef);
	portdef.nPortIndex = CAMERA_OUTPUT_PORT;
	
	OMX_GetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	
	portdef.format.image.nFrameWidth = videoWidth;
	portdef.format.image.nFrameHeight = videoHeight;
	portdef.format.image.nStride = videoWidth;
	
	OMX_SetParameter(camera, OMX_IndexParamPortDefinition, &portdef);
	
	
	//Set the framerate 
	OMX_CONFIG_FRAMERATETYPE framerateConfig;
	OMX_INIT_STRUCTURE(framerateConfig);
	framerateConfig.nPortIndex = CAMERA_OUTPUT_PORT;
	framerateConfig.xEncodeFramerate = framerate << 16; //Q16 format - 25fps
	
	OMX_SetConfig(camera, OMX_IndexConfigVideoFramerate, &framerateConfig);
	
	
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

int ofxRPiCameraVideoGrabber::getWidth()
{
	return videoWidth;
}
int ofxRPiCameraVideoGrabber::getHeight()
{
	return videoHeight;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
	return framerate;
}

GLuint ofxRPiCameraVideoGrabber::getTextureID()
{
	return textureID;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
	return tex;
}

void ofxRPiCameraVideoGrabber::draw()
{
	tex.draw(0, 0);
}

bool ofxRPiCameraVideoGrabber::isReady()
{
	return ready;
}
void ofxRPiCameraVideoGrabber::setExposureMode(OMX_EXPOSURECONTROLTYPE exposureMode)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set exposure mode
	OMX_CONFIG_EXPOSURECONTROLTYPE exposure;
	OMX_INIT_STRUCTURE(exposure);
	exposure.nPortIndex = OMX_ALL;
	exposure.eExposureControl = exposureMode;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposure);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetConfig OMX_IndexConfigCommonExposure FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setMeteringMode(OMX_METERINGTYPE meteringType, int evCompensation, int sensitivity, bool autoSensitivity)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Set EV compensation, ISO and metering mode
	OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
	OMX_INIT_STRUCTURE(exposurevalue);
	exposurevalue.nPortIndex = OMX_ALL;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR,	"camera OMX_SetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
	}	 
		
	exposurevalue.xEVCompensation = evCompensation;	//Fixed point value stored as Q16 
	exposurevalue.nSensitivity = sensitivity;		//< e.g. nSensitivity = 100 implies "ISO 100" 
	if (autoSensitivity)
	{
		exposurevalue.bAutoSensitivity	= OMX_TRUE;
	}else 
	{
		exposurevalue.bAutoSensitivity	= OMX_FALSE;
	}
	
	exposurevalue.eMetering = meteringType; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &exposurevalue);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR,	"camera setMeteringMode FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setSharpness(int sharpness_) //-100 to 100
{
	sharpness = sharpness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SHARPNESSTYPE sharpnessConfig;
	OMX_INIT_STRUCTURE(sharpnessConfig);
	sharpnessConfig.nPortIndex = OMX_ALL;
	sharpnessConfig.nSharpness = sharpness; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setSharpness FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
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
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setFrameStabilization FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setContrast(int contrast_ ) //-100 to 100 
{
	contrast = contrast_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_CONTRASTTYPE contrastConfig;
	OMX_INIT_STRUCTURE(contrastConfig);
	contrastConfig.nPortIndex = OMX_ALL;
	contrastConfig.nContrast = contrast; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setContrast FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setBrightness(int brightness_ ) //0 to 100
{
	brightness = brightness_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_BRIGHTNESSTYPE brightnessConfig;
	OMX_INIT_STRUCTURE(brightnessConfig);
	brightnessConfig.nPortIndex = OMX_ALL;
	brightnessConfig.nBrightness = brightness;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setBrightness FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setSaturation(int saturation_) //-100 to 100
{
	saturation = saturation_;
	
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_SATURATIONTYPE saturationConfig;
	OMX_INIT_STRUCTURE(saturationConfig);
	saturationConfig.nPortIndex		= OMX_ALL;
	saturationConfig.nSaturation	= saturation_; 
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setSaturation FAIL error: 0x%08x", error);
	}
}


void ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	OMX_CONFIG_WHITEBALCONTROLTYPE awb;
	OMX_INIT_STRUCTURE(awb);
	awb.nPortIndex = OMX_ALL;
	awb.eWhiteBalControl = controlType;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &awb);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setWhiteBalance FAIL error: 0x%08x", error);
	}
}


void ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
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
	
	color.nCustomizedU = U;
	color.nCustomizedV = V;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &color);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setColorEnhancement FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::setLEDStatus(bool status)
{
	OMX_ERRORTYPE error = OMX_ErrorNone;
	
	//Turn off the LED - doesn't work! 
	OMX_CONFIG_PRIVACYINDICATORTYPE privacy;
	OMX_INIT_STRUCTURE(privacy);
	privacy.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
	
	error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &privacy);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera setLEDStatus FAIL error: 0x%08x", error);
	}
}
void ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
	OMX_CONFIG_IMAGEFILTERTYPE imagefilterConfig;
	OMX_INIT_STRUCTURE(imagefilterConfig);
	imagefilterConfig.nPortIndex = OMX_ALL;
	imagefilterConfig.eImageFilter = imageFilter;
	
	OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
	if(error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera applyImageFilter FAIL error: 0x%08x", error);
	}
}

void ofxRPiCameraVideoGrabber::generateEGLImage()
{
	
	ofAppEGLWindow *appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
	display = appEGLWindow->getEglDisplay();
	context = appEGLWindow->getEglContext();
	
	
	tex.allocate(videoWidth, videoHeight, GL_RGBA);
	tex.getTextureData().bFlipTexture = false;
	tex.setTextureWrap(GL_REPEAT, GL_REPEAT);
	textureID = tex.getTextureData().textureID;
	
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

void logEvent(string callingFunction, OMX_EVENTTYPE eEvent)
{
	switch (eEvent) 
	{
		case OMX_EventError:						ofLogError(callingFunction)	  <<  " OMX_EventError";						break;
			
		case OMX_EventCmdComplete:					ofLogVerbose(callingFunction) <<  " OMX_EventCmdComplete";					break;
		case OMX_EventMark:							ofLogVerbose(callingFunction) <<  " OMX_EventMark";							break;
		case OMX_EventPortSettingsChanged:			ofLogVerbose(callingFunction) <<  " OMX_EventPortSettingsChanged";			break;
		case OMX_EventBufferFlag:					ofLogVerbose(callingFunction) <<  " OMX_EventBufferFlag";					break;
		case OMX_EventResourcesAcquired:			ofLogVerbose(callingFunction) <<  " OMX_EventResourcesAcquired";			break;
		case OMX_EventComponentResumed:				ofLogVerbose(callingFunction) <<  " OMX_EventComponentResumed";				break;
		case OMX_EventDynamicResourcesAvailable:	ofLogVerbose(callingFunction) <<  " OMX_EventDynamicResourcesAvailable";	break;
		case OMX_EventPortFormatDetected:			ofLogVerbose(callingFunction) <<  " OMX_EventPortFormatDetected";			break;
		case OMX_EventKhronosExtensions:			ofLogVerbose(callingFunction) <<  " OMX_EventKhronosExtensions";			break;
		case OMX_EventVendorStartUnused:			ofLogVerbose(callingFunction) <<  " OMX_EventVendorStartUnused";			break;
		case OMX_EventMax:							ofLogVerbose(callingFunction) <<  " OMX_EventMax";							break;
		case OMX_EventParamOrConfigChanged:			ofLogVerbose(callingFunction) <<  " OMX_EventParamOrConfigChanged";			break;
			
		default:									ofLogVerbose(callingFunction) <<	"DEFAULT";								break;
	}
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	/*ofLog(OF_LOG_VERBOSE, 
		  "ofxRPiCameraVideoGrabber::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
		  __func__, eEvent, nData1, nData2, pEventData);*/
	logEvent(__func__, eEvent);
	switch (eEvent) 
	{
		case OMX_EventParamOrConfigChanged:
		{
			ofxRPiCameraVideoGrabber *grabber = static_cast<ofxRPiCameraVideoGrabber*>(pAppData);
			grabber->onCameraEventParamOrConfigChanged();
			break;
		}			
		default: 
		{
			break;
		}
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::renderEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	logEvent(__func__, eEvent);
	return OMX_ErrorNone;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::renderEmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	ofLogVerbose(LOG_NAME) << "renderEmptyBufferDone";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::renderFillBufferDone(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_PTR pAppData, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{	
	ofxRPiCameraVideoGrabber *grabber = static_cast<ofxRPiCameraVideoGrabber*>(pAppData);
	grabber->frameCounter++;
	OMX_ERRORTYPE error = OMX_FillThisBuffer(grabber->render, grabber->eglBuffer);
	return error;
}

void ofxRPiCameraVideoGrabber::onCameraEventParamOrConfigChanged()
{
	ofLogVerbose(LOG_NAME) << "onCameraEventParamOrConfigChanged";
	
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	//Enable Camera Output Port
	OMX_CONFIG_PORTBOOLEANTYPE cameraport;
	OMX_INIT_STRUCTURE(cameraport);
	cameraport.nPortIndex = CAMERA_OUTPUT_PORT;
	cameraport.bEnabled = OMX_TRUE;
	
	error =OMX_SetParameter(camera, OMX_IndexConfigPortCapturing, &cameraport);	
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera enable Output Port FAIL error: 0x%08x", error);
	}
	
	//Set up texture renderer
	OMX_CALLBACKTYPE renderCallbacks;
	renderCallbacks.EventHandler    = &ofxRPiCameraVideoGrabber::renderEventHandlerCallback;
	renderCallbacks.EmptyBufferDone	= &ofxRPiCameraVideoGrabber::renderEmptyBufferDone;
	renderCallbacks.FillBufferDone	= &ofxRPiCameraVideoGrabber::renderFillBufferDone;
	
	string componentName = "OMX.broadcom.egl_render";
	
	OMX_GetHandle(&render, (OMX_STRING)componentName.c_str(), this , &renderCallbacks);
	disableAllPortsForComponent(&render);
	
	//Set renderer to Idle
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_SendCommand OMX_StateIdle FAIL error: 0x%08x", error);
	}
	
	//Create camera->egl_render Tunnel
	error = OMX_SetupTunnel(camera, CAMERA_OUTPUT_PORT, render, EGL_RENDER_INPUT_PORT);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera->egl_render OMX_SetupTunnel FAIL error: 0x%08x", error);
	}
	
	//Enable camera output port
	error = OMX_SendCommand(camera, OMX_CommandPortEnable, CAMERA_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera enable output port FAIL error: 0x%08x", error);
	}
	
	//Enable render output port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render enable output port FAIL error: 0x%08x", error);
	}
	
	//Enable render input port
	error = OMX_SendCommand(render, OMX_CommandPortEnable, EGL_RENDER_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render enable input port FAIL error: 0x%08x", error);
	}
	
	//Set renderer to use texture
	error = OMX_UseEGLImage(render, &eglBuffer, EGL_RENDER_OUTPUT_PORT, this, eglImage);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_UseEGLImage-----> FAIL error: 0x%08x", error);
	}
	
	//Start renderer
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "render OMX_StateExecuting FAIL error: 0x%08x", error);		
	}
	
	//Start camera
	error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_StateExecuting FAIL error: 0x%08x", error);
	}
	
	//start the buffer filling loop
	//once completed the callback will trigger and refill
	error = OMX_FillThisBuffer(render, eglBuffer);
	if(error == OMX_ErrorNone)
	{
		ofLogVerbose(LOG_NAME) << "render OMX_FillThisBuffer PASS";
		ready = true;
	}else 
	{
		ofLog(OF_LOG_ERROR, "render OMX_FillThisBuffer FAIL error: 0x%08x", error);
	}
}

//untested - I guess could be used to close manually
//app and camera seem to close fine on exit
void ofxRPiCameraVideoGrabber::close()
{
	ready = false;
	OMX_ERRORTYPE error = OMX_SendCommand(camera, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "camera OMX_StateIdle FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "render OMX_StateIdle FAIL";;
	}
	
	error = OMX_SendCommand(camera, OMX_CommandFlush, CAMERA_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "camera OMX_CommandFlush FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_INPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "render OMX_CommandFlush FAIL";
	}
	
	error = OMX_SendCommand(render, OMX_CommandFlush, EGL_RENDER_OUTPUT_PORT, NULL);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "render OMX_CommandFlush EGL_RENDER_OUTPUT_PORT FAIL";
	}
	
	disableAllPortsForComponent(&render);
	disableAllPortsForComponent(&camera);
	
	error = OMX_FreeHandle(render);
	if (error != OMX_ErrorNone) 
	{
		ofLogVerbose(LOG_NAME) << "render OMX_FreeHandle FAIL";
	}
	
	error = OMX_FreeHandle(camera);
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "camera OMX_FreeHandle FAIL";
	}
	
	error = OMX_Deinit(); 
	if (error != OMX_ErrorNone) 
	{
		ofLogError(LOG_NAME) << "OMX_Deinit FAIL";
	}
	
	if (eglImage != NULL)  
	{
		eglDestroyImageKHR(display, eglImage);
	}
	
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::disableAllPortsForComponent(OMX_HANDLETYPE* handle)
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
	
	for(int i=0; i < 4; i++)
	{
		error = OMX_GetParameter(*handle, indexTypes[i], &ports);
		if(error == OMX_ErrorNone) 
		{
			
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


