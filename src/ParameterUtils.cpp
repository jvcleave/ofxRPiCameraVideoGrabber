/*
 *  ParameterUtils.cpp
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 11/18/13.
 *  Copyright 2013 jasonvancleave.com. All rights reserved.
 *
 */

#include "ParameterUtils.h"

ParameterUtils::ParameterUtils()
{
	serializer = new ofXml();
}

ofParameter<bool> ParameterUtils::createBoolean(ofXml& xml)
{
    string childName = xml.getName();
    ofParameter<bool> item;
    item.set(childName, xml.getBoolValue());
    return item;
}

ofParameterGroup* ParameterUtils::createParameterGroup(ofXml& xml)
{
    ofParameterGroup* parameterGroup = new ofParameterGroup();
    string elementName = xml.getName();
	xml.setAttribute("type", "group");
    parameterGroup->setName(elementName);
    int numElementChildren = xml.getNumChildren();
    for(int i=0; i<numElementChildren; i++)
    {
        
        xml.setToChild(i);
		ofParameter<bool> item = createBoolean(xml);
		parameterGroup->add(item);
        xml.setToParent();
        
    }
    return parameterGroup;
}

void ParameterUtils::createXMLFromParam(ofAbstractParameter& parameter, ofXml& xml)
{
	
	string parameterName = xml.getName();
	if(parameter.type()==typeid(ofParameter<int>).name())
	{
		xml.setAttribute("type", "int");
		xml.setAttribute("min", ofToString(parameter.cast<int>().getMin()));
		xml.setAttribute("max", ofToString(parameter.cast<int>().getMax()));
	}
	else if(parameter.type()==typeid(ofParameter<float>).name())
	{
		
		xml.setAttribute("type", "float");
		xml.setAttribute("min", ofToString(parameter.cast<float>().getMin()));
		xml.setAttribute("max", ofToString(parameter.cast<float>().getMax()));
	}
	else if(parameter.type()==typeid(ofParameter<bool>).name())
	{
		xml.setAttribute("type", "bool");
	}
	else if(parameter.type()==typeid(ofParameter<string>).name())
	{
		xml.setAttribute("type", "string");
	}
	
}

void ParameterUtils::saveXML(ofParameterGroup& parameters, string filePath)
{
	serializer->serialize(parameters);
	
	ofXml xmlParser(*serializer);
	ofLogVerbose(__func__) << "xmlParser toString: " << xmlParser.toString();
	
	int numRootChildren =  xmlParser.getNumChildren();
    for(int i=0; i<numRootChildren; i++)
    {
        xmlParser.setToChild(i);
		int numElementChildren = xmlParser.getNumChildren();
		if (numElementChildren>0)
		{
			
			ofParameterGroup* parameterGroupPtr = createParameterGroup(xmlParser);
			for(int j=0; j<numElementChildren; j++)
			{
				xmlParser.setToChild(j);
				ofParameterGroup parameterGroup = *parameterGroupPtr;
				createXMLFromParam(parameterGroup[xmlParser.getName()], xmlParser);
				xmlParser.setToParent();
			}
			
		}else 
		{
			createXMLFromParam(parameters[xmlParser.getName()], xmlParser);
		}
		
        xmlParser.setToParent();
    }
	
	ofLogVerbose(__func__) << "xmlParser processed: " << xmlParser.toString();
	
	
	xmlParser.save(filePath);
	
	
}
