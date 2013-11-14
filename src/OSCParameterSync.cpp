#include "OSCParameterSync.h"

OSCParameterSync::OSCParameterSync() {
	syncGroup = NULL;
	updatingParameter = false;
}

OSCParameterSync::~OSCParameterSync(){
	if(syncGroup)
		ofRemoveListener(syncGroup->parameterChangedE,this,&OSCParameterSync::parameterChanged);
}


void OSCParameterSync::setup(ofParameterGroup & group, int localPort, string host, int remotePort){
	syncGroup = &group;
	ofAddListener(group.parameterChangedE,this,&OSCParameterSync::parameterChanged);
	sender.setup(host,remotePort);
	receiver.setup(localPort);
}

void OSCParameterSync::update(){
	if(receiver.hasWaitingMessages()){
		updatingParameter = true;
		receiver.getParameter(*syncGroup);
		updatingParameter = false;
		ofLogVerbose() << "receiver.hasWaitingMessages()";
	}
}

void OSCParameterSync::parameterChanged( ofAbstractParameter & parameter ){
	ofLogVerbose() << "parameterChanged updatingParameter: " << updatingParameter;
	if(updatingParameter) return;
	sender.sendParameter(parameter);
}
