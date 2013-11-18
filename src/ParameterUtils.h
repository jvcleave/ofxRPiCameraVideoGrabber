#pragma once

#include "ofMain.h"

class ParameterUtils
{
public:
	ParameterUtils();
	ofXml* serializer;
	ofParameter<bool> createBoolean(ofXml& xml);
	ofParameterGroup* createParameterGroup(ofXml& xml);
	void createXMLFromParam(ofAbstractParameter&  parameter, ofXml& xml);
	void saveXML(ofParameterGroup& parameters, string filePath);
};