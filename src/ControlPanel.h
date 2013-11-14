#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "OSCParameterSync.h"
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
	void onExposureControlNamesChanged(ofAbstractParameter & param);
	void increaseContrast();
	
	ofxPanel gui;
//private:
	
	
	
	ofParameterGroup parameters;
	ofParameterGroup videoCodingNames;
	ofParameterGroup exposureControlNames;
	ofParameterGroup meteringNames;
	ofParameterGroup whiteBalanceNames;
	ofParameterGroup imageFilterNames;
	
	OSCParameterSync* sync;
	
	int localPort;
	int remotePort;
	
	
	void onSharpnessChanged(int &sharpness_);
	void onContrastChanged(int &constrast_);
	void onBrightnessChanged(int &brightness_);
	void onSaturationChanged(int &saturation_);
	
	void onFrameStabilizationChanged(bool &doFrameStabilization);
	void onColorEnhancementChanged(bool &doColorEnhancement);
	void onLEDEnabledChanged(bool &doEnableLED);
	ofXml* serializer;
	bool enableSync;
	
	void createXMLFromParam(ofAbstractParameter&  parameter, ofXml& xml);
	
	ofParameterGroup* guiParamGroup;
	
};