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
	ofLogVerbose() << "EventHandlerCallback";
	return OMX_ErrorNone;
}

OMX_ERRORTYPE testApp::cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
											OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	ofLogVerbose() << "cameraEventHandlerCallback";
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

