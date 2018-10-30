#pragma once

#include "BaseEngine.h"
#include "DirectDisplay.h"



class DirectEngine: public BaseEngine
{
public:
	DirectEngine();
	~DirectEngine();
	void setup(OMXCameraSettings&, RecordingListener*) override;
    int getFrameCounter() override;
    DirectDisplay* displayManager;
private:
    
    int frameCounter;
    
	OMX_ERRORTYPE onCameraEventParamOrConfigChanged();
	
	static OMX_ERRORTYPE
    cameraEventHandlerCallback(OMX_HANDLETYPE,
                               OMX_PTR,
                               OMX_EVENTTYPE,
                               OMX_U32,
                               OMX_U32,
                               OMX_PTR);


	
    

};
