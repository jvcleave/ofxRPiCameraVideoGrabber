#pragma once

#include "ofMain.h"

#include "ofxOscParameterSync.h"
#include "ofxRPiCameraVideoGrabber.h"


class ControlPanel
{
public:
	ControlPanel();
	
	void setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber_);
	void onUpdate(ofEventArgs &args);
	ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber;
	void saveXML();
	void onParameterGroupChanged(ofAbstractParameter & param);
	void onVideoCodingNamesChanged(ofAbstractParameter & param);
private:
	ofParameter<int> sharpness;
	ofParameter<int> contrast;
	ofParameter<int> brightness;
	ofParameter<int> saturation;
	ofParameter<bool> frameStabilizationEnabled;
	ofParameter<bool> colorEnhancementEnabled;
	ofParameter<bool> ledEnabled;
	
	
	ofParameterGroup parameters;
	ofParameterGroup videoCodingNames;
	ofParameterGroup exposureControlNames;
	ofParameterGroup meteringNames;
	ofParameterGroup whiteBalanceNames;
	ofParameterGroup imageFilterNames;
	
	ofxOscParameterSync sync;
	
	int localPort;
	int remotePort;
	
	
	void onSharpnessChanged(int &sharpness_);
	void onContrastChanged(int &constrast_);
	void onBrightnessChanged(int &brightness_);
	void onSaturationChanged(int &saturation_);
	
	void onFrameStabilizationChanged(bool &doFrameStabilization);
	void onColorEnhancementChanged(bool &doColorEnhancement);
	void onLEDEnabledChanged(bool &doEnableLED);
	ofPtr<ofBaseFileSerializer> serializer;
	bool enableSync;
};