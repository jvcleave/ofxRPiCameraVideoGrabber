#pragma once

#include "ofMain.h"

class ParameterUtils
{
public:
	ParameterUtils();
	ofXml* serializer;
	ofParameter<bool> createBoolean(ofXml& xml);
	ofParameterGroup* createGuiToggleParameterGroup(ofXml& xml);
	void createXMLFromParam(ofAbstractParameter&  parameter, ofXml& xml);
	void saveXML(ofParameterGroup& parameters, string filePath);
	ofParameterGroup* createGuiParameterGroup(ofXml& xml);
    ofAbstractParameter* createGuiParameter(ofXml& xml);
};