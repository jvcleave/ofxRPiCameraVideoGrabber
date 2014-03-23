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
	
	void setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber, string remoteIP);
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
	string remoteIP;
	
	
	
	ofxPanel gui;
	ofParameterGroup* guiParamGroup;
	ParameterUtils parameterUtils;
	
	ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber;
	
	void onUpdate(ofEventArgs &args);
	
	
	
	
	ofParameter<int> sharpness;
	ofParameter<int> contrast;
	ofParameter<int> brightness;
	ofParameter<int> saturation;
	ofParameter<bool> frameStabilizationEnabled;
	ofParameter<bool> colorEnhancementEnabled;
	ofParameter<bool> ledEnabled;
	
	ofParameter<bool> drawGui;
	bool doDrawGui;
	
	ofParameter<bool> actionItem;
	bool doActionItem;
	
	void onSharpnessChanged(int &sharpness);
	void onContrastChanged(int &contrast);
	void onBrightnessChanged(int &brightness);
	void onSaturationChanged(int &saturation);
	
	void onFrameStabilizationChanged(bool &doFrameStabilization);
	void onColorEnhancementChanged(bool &doColorEnhancement);
	void onLEDEnabledChanged(bool &doEnableLED);
	void onDrawGuiChanged(bool &doDrawGui);
	
	
	void onActionItemChanged(bool &doAction);
	
	
	void onParameterGroupChanged(ofAbstractParameter & param);
	
	void onExposureControlToggleGroupChange(ToggleGroupListenerEventData & eventData);
	void onWhiteBalanceToggleGroupChange(ToggleGroupListenerEventData & eventData);
	void onMeteringNamesToggleGroupChange(ToggleGroupListenerEventData & eventData);
	void onImageFilterToggleGroupChange(ToggleGroupListenerEventData & eventData);

	
	template<typename ListenerMethod>
	ToggleGroup* createToggleGroup(ofParameterGroup* parameterGroup, string groupName, vector<string>* names, ListenerMethod callback);

};