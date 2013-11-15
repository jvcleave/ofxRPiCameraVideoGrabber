#pragma once

#include "ofxOscSender.h"
#include "ofxOscReceiver.h"
#include "ofParameter.h"
#include "ofParameterGroup.h"

class OSCParameterSync {
public:
	OSCParameterSync();
	~OSCParameterSync();
	
	/// the remote and local ports must be different to avoid collisions
	void setup(ofParameterGroup & group, int localPort, string remoteHost, int remotePort);
	void update();
	ofxOscSender sender;
	ofxOscReceiver receiver;
	bool enabled;
private:
	void parameterChanged( ofAbstractParameter & parameter );
	
	ofParameterGroup * syncGroup;
	bool updatingParameter;
};
