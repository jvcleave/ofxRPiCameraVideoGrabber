#include "testApp.h"


#define OMX_INIT_STRUCTURE(a) \
memset(&(a), 0, sizeof(a)); \
(a).nSize = sizeof(a); \
(a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
(a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
(a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
(a).nVersion.s.nStep = OMX_VERSION_STEP


void testApp::listRoles(char *name) {
    OMX_U32 numRoles;
	vector<OMX_U8*> roles;
    /* get the number of roles by passing in a NULL roles param */
    OMX_ERRORTYPE err = OMX_GetRolesOfComponent(name, &numRoles, NULL);
    if (err != OMX_ErrorNone) 
	{
		ofLogVerbose() << "Getting roles failed";
		return;
    }
	ofLogVerbose() << "numRoles: " << numRoles;
	
    /* now get the roles */
	
    for (int i = 0; i < numRoles; i++) 
	{
		unsigned char* role = new unsigned char[OMX_MAX_STRINGNAME_SIZE];
		roles.push_back(role);
    }
    err = OMX_GetRolesOfComponent(name, &numRoles, &roles[0]);
    if (err != OMX_ErrorNone) 
	{
		ofLogVerbose() << "OMX_GetRolesOfComponent FAIL";
		return;
    }
    for (int i = 0; i < roles.size(); i++) 
	{
		ofLogVerbose() << "role: " << roles[i];
    }
	
}
OMX_ERRORTYPE testApp::EventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
									  OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	//ofLogVerbose() << "EventHandlerCallback";
	
	ofLog(OF_LOG_VERBOSE, 
		  "testApp::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
		  __func__, eEvent, nData1, nData2, pEventData);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE testApp::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
											OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLogVerbose() << "cameraEventHandlerCallback";
	ofLog(OF_LOG_VERBOSE, 
		  "testApp::%s - eEvent(0x%x), nData1(0x%lx), nData2(0x%lx), pEventData(0x%p)\n",
		  __func__, eEvent, nData1, nData2, pEventData);
	return OMX_ErrorNone;
}
//--------------------------------------------------------------
void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	bcm_host_init();
	char name[OMX_MAX_STRINGNAME_SIZE];
	OMX_ERRORTYPE error = OMX_Init();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "OMX_Init PASS";
	}
	error = OMX_ErrorNone;
    for (int i = 0; OMX_ErrorNoMore != error; i++)
	{
		error = OMX_ComponentNameEnum(name, OMX_MAX_STRINGNAME_SIZE, i);
		if (OMX_ErrorNone == error) {
			ofLog(OF_LOG_VERBOSE, "Component is %s\n", name);
			listRoles(name);
		}
    }
	
	const std::string componentName = "OMX.broadcom.clock";
	clockCallbacks.EventHandler    = &testApp::EventHandlerCallback;
	error = OMX_GetHandle(&clock, (OMX_STRING)componentName.c_str(), NULL /*appPriv */, &clockCallbacks);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "clock OMX_GetHandle PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "clock OMX_GetHandle FAIL omx_err(0x%08x)\n", error);
	}
	OMX_TIME_CONFIG_CLOCKSTATETYPE omxTimeConfigClockStateType;
	OMX_INIT_STRUCTURE(omxTimeConfigClockStateType);
	
	omxTimeConfigClockStateType.eState = OMX_TIME_ClockStateWaitingForStartTime;
	
	error = OMX_SetConfig(clock, OMX_IndexConfigTimeClockState, &omxTimeConfigClockStateType);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "clock OMX_SetConfig PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "clock OMX_SetConfig FAIL omx_err(0x%08x)\n", error);
	}
	
	OMX_CONFIG_REQUESTCALLBACKTYPE omxConfigRequestCallbackType;
	OMX_INIT_STRUCTURE(omxConfigRequestCallbackType);
	
	const std::string cameraComponentName = "OMX.broadcom.camera";
	cameraCallbacks.EventHandler    = &testApp::cameraEventHandlerCallback;
	
	error = OMX_GetHandle(&camera, (OMX_STRING)cameraComponentName.c_str(), NULL /*appPriv */, &cameraCallbacks);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_GetHandle PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetHandle FAIL omx_err(0x%08x)\n", error);
	}
	
	
	
	/*typedef struct OMX_CONFIG_CAMERAINFOTYPE
	{
		OMX_U32 nSize;
		OMX_VERSIONTYPE nVersion;
		OMX_U8 cameraname[OMX_CONFIG_CAMERAINFOTYPE_NAME_LEN];
		OMX_U8 lensname[OMX_CONFIG_CAMERAINFOTYPE_NAME_LEN];
		OMX_U16 nModelId;
		OMX_U8 nManufacturerId;
		OMX_U8 nRevNum;
		OMX_U8 sSerialNumber[OMX_CONFIG_CAMERAINFOTYPE_SERIALNUM_LEN];
		OMX_U8 sEpromVersion[OMX_CONFIG_CAMERAINFOTYPE_EPROMVER_LEN];
		OMX_CONFIG_LENSCALIBRATIONVALUETYPE sLensCalibration;
		OMX_U32 xFNumber;
		OMX_U32 xFocalLength;
	} OMX_CONFIG_CAMERAINFOTYPE;*/
	
	
	OMX_PARAM_CAMERAISPTUNERTYPE omxParamCameraISPTunerType;
	OMX_INIT_STRUCTURE(omxParamCameraISPTunerType);
	error = OMX_SetParameter(camera, OMX_IndexParamISPTunerName, &omxParamCameraISPTunerType);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_SetParameter OMX_IndexParamISPTunerName PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter OMX_IndexParamISPTunerName FAIL omx_err(0x%08x)\n", error);
	}
	
	OMX_ERRORTYPE didDisable = DisableAllPorts(&camera);
	if(didDisable == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera didDisable PASS";
	}else 
	{
		ofLog(OF_LOG_ERROR, "camera didDisable FAIL omx_err(0x%08x)\n", didDisable);
	}
	
	OMX_PORT_PARAM_TYPE port_param;
	OMX_INIT_STRUCTURE(port_param);
	
	error = OMX_GetParameter(camera, OMX_IndexParamVideoInit, &port_param);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_GetParameter OMX_IndexParamOtherInit PASS";
		
		ofLogVerbose() << "port_param.nStartPortNumber: " << port_param.nStartPortNumber;
		ofLogVerbose() << "port_param.nPorts: " << port_param.nPorts;
		
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamOtherInit FAIL omx_err(0x%08x)\n", error);
	}
	
	/*OMX_PARAM_U32TYPE cameraDeviceNumber;
	OMX_INIT_STRUCTURE(cameraDeviceNumber);
	cameraDeviceNumber.nPortIndex = port_param.nStartPortNumber;
	cameraDeviceNumber.nU32 = 0;
	error = OMX_SetParameter(camera, OMX_IndexParamCameraDeviceNumber, &cameraDeviceNumber);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber PASS";
		//ofLogVerbose() << "cameraDeviceNumber: " << cameraDeviceNumber.nU32;
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter OMX_IndexParamCameraDeviceNumber FAIL omx_err(0x%08x)\n", error);
	}*/
	OMX_CONFIG_CAMERASENSORMODETYPE cameraSensorModeType;
	OMX_INIT_STRUCTURE(cameraSensorModeType);
	cameraSensorModeType.nPortIndex = port_param.nStartPortNumber;
	error = OMX_GetParameter(camera, OMX_IndexConfigCameraSensorModes, &cameraSensorModeType);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_GetParameter OMX_IndexConfigCameraSensorModes PASS";

	}else 
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexConfigCameraSensorModes FAIL omx_err(0x%08x)\n", error);
	}

	
	/*OMX_PARAM_CAMERAFLASHTYPE cameraFlashType;
	OMX_INIT_STRUCTURE(cameraFlashType);
	cameraFlashType.eFlashType = OMX_CameraFlashDefault;
	cameraFlashType.nPortIndex = 73;
	error = OMX_SetParameter(camera, OMX_IndexParamCameraFlashType, &cameraFlashType);
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_SetParameter OMX_IndexParamCameraFlashType PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_SetParameter OMX_IndexParamCameraFlashType FAIL omx_err(0x%08x)\n", error);
	}*/
	
	
	
	/*OMX_CONFIG_CAMERAINFOTYPE cameraInfoType;
	OMX_INIT_STRUCTURE(cameraInfoType);
	
	error = OMX_GetParameter(camera, OMX_IndexConfigCameraInfo, &cameraInfoType);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_GetParameter OMX_IndexConfigCameraInfo PASS";
	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexConfigCameraInfo FAIL omx_err(0x%08x)\n", error);
	}*/
	
	
	/*OMX_PORT_PARAM_TYPE port_param;
	OMX_INIT_STRUCTURE(port_param);
	
	error = OMX_GetParameter(camera, OMX_IndexParamOtherInit, &port_param);
	
	if(error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "camera OMX_GetParameter OMX_IndexParamOtherInit PASS";
		
		ofLogVerbose() << "port_param.nStartPortNumber: " << port_param.nStartPortNumber;
		ofLogVerbose() << "port_param.nPorts: " << port_param.nPorts;
		OMX_ERRORTYPE didDisable = DisableAllPorts(&camera);
		if(didDisable == OMX_ErrorNone) 
		{
			ofLogVerbose() << "camera didDisable PASS";
		}else 
		{
			ofLog(OF_LOG_ERROR, "camera didDisable FAIL omx_err(0x%08x)\n", didDisable);
		}

	}else
	{
		ofLog(OF_LOG_ERROR, "camera OMX_GetParameter OMX_IndexParamOtherInit FAIL omx_err(0x%08x)\n", error);
	}*/
	
	
	
	
	
}
OMX_ERRORTYPE testApp::DisableAllPorts(OMX_HANDLETYPE* m_handle)
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
					ofLog(OF_LOG_VERBOSE, "\nCOMXCoreComponent::DisableAllPorts - Error disable port %d on component %s omx_err(0x%08x)", 
						  (int)(ports.nStartPortNumber) + j, "m_componentName", (int)omx_err);
				}
				/*omx_err = WaitForCommand(OMX_CommandPortDisable, ports.nStartPortNumber+j);
				if(omx_err != OMX_ErrorNone && omx_err != OMX_ErrorSameState)
				{
					UnLock();
					return omx_err;
				}*/
			}
		}
	}
	
	
	return OMX_ErrorNone;
}

//--------------------------------------------------------------
void testApp::update()
{
	
}


//--------------------------------------------------------------
void testApp::draw(){
	}



void testApp::exit()
{
	OMX_ERRORTYPE error = OMX_Deinit();
	if (error == OMX_ErrorNone) 
	{
		ofLogVerbose() << "OMX_Deinit PASS";
	}
	bcm_host_deinit();
	
	
}
//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

