#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "OSCParameterSync.h"
#include "ofxRPiCameraVideoGrabber.h"
#include "ParameterUtils.h"
#include "ToggleGroup.h"

class ControlPanel
{
public:
	ControlPanel();
	
	void setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber);
	void draw();
	void increaseContrast();
	
private:

	ofParameterGroup parameters;
	ofParameterGroup videoCodingNames;
	ofParameterGroup exposureControlNames;
	ofParameterGroup meteringNames;
	ofParameterGroup whiteBalanceNames;
	ofParameterGroup imageFilterNames;
	
	
	
	OSCParameterSync* sync;
	
	int localPort;
	int remotePort;

	
	bool doDrawGui;
	
	ofxPanel gui;
	ofParameterGroup* guiParamGroup;
	ParameterUtils parameterUtils;
	
	ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber;
	
	void onUpdate(ofEventArgs &args);
	
	
	void onSharpnessChanged(int &sharpness);
	void onContrastChanged(int &constrast);
	void onBrightnessChanged(int &brightness);
	void onSaturationChanged(int &saturation);
	
	void onFrameStabilizationChanged(bool &doFrameStabilization);
	void onColorEnhancementChanged(bool &doColorEnhancement);
	void onLEDEnabledChanged(bool &doEnableLED);
	void onDrawGuiChanged(bool &doDrawGui);
	
	void onParameterGroupChanged(ofAbstractParameter & param);
	void onVideoCodingNamesChanged(ofAbstractParameter & param);
	void onExposureControlNamesChanged(ofAbstractParameter & param);
	void onWhiteBalanceNamesChanged(ofAbstractParameter & param);
	void onImageFilterNamesChanged(ofAbstractParameter & param);
	void onMeteringNamesChanged(ofAbstractParameter & param);
	
	void onExposureControlToggleGroupChange(ToggleGroupListenerEventData & eventData);
	
	
};