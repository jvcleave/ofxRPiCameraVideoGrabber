#pragma once

#include "ofMain.h"
#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
	
	OMX_HANDLETYPE camera;
	OMX_CALLBACKTYPE cameraCallbacks;

	
	void listRoles(char *name);
	static OMX_ERRORTYPE EventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
													 OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	
	static OMX_ERRORTYPE cameraEventHandlerCallback(OMX_HANDLETYPE hComponent, OMX_PTR pAppData,
													 OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
	OMX_ERRORTYPE DisableAllPorts(OMX_HANDLETYPE* m_handle);
};

