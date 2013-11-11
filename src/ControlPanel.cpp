#include "ControlPanel.h"


ControlPanel::ControlPanel()
{
	localPort = 6666;
	remotePort = 6667;
	rpiCameraVideoGrabber = NULL;
	serializer = new ofXml();
}

void ControlPanel::onParameterGroupChanged(ofAbstractParameter & param)
{
	ofLogVerbose() << "onParameterGroupChanged";
}

void ControlPanel::onVideoCodingNamesChanged(ofAbstractParameter & param)
{
	ofLogVerbose() << "onVideoCodingNamesChanged";
}

void ControlPanel::setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber)
{
	this->rpiCameraVideoGrabber = rpiCameraVideoGrabber;
	parameters.setName("root");
    
    sharpness.set("sharpness", rpiCameraVideoGrabber->getSharpness(), -100, 100);
	sharpness.addListener(this, &ControlPanel::onSharpnessChanged);
	
    contrast.set("contrast", rpiCameraVideoGrabber->getContrast(), -100, 100);
	contrast.addListener(this, &ControlPanel::onContrastChanged);
	
    brightness.set("brightness", rpiCameraVideoGrabber->getBrightness(), 0, 100);
	brightness.addListener(this, &ControlPanel::onBrightnessChanged);
	
    saturation.set("saturation", rpiCameraVideoGrabber->getSaturation(), -100, 100);
	saturation.addListener(this, &ControlPanel::onSaturationChanged);
	
    frameStabilizationEnabled.set("FrameStabilization", false);
	frameStabilizationEnabled.addListener(this, &ControlPanel::onFrameStabilizationChanged);
	
    colorEnhancementEnabled.set("ColorEnhancement", false);
	colorEnhancementEnabled.addListener(this, &ControlPanel::onColorEnhancementChanged);
	
    ledEnabled.set("LED", false);
	ledEnabled.addListener(this, &ControlPanel::onLEDEnabledChanged);
	
	size_t i=0;
	videoCodingNames.setName("videoCodingNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.videoCodingNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.videoCodingNames[i], false);
		videoCodingNames.add(item);
	}
	
	ofAddListener(videoCodingNames.parameterChangedE, this, &ControlPanel::onVideoCodingNamesChanged);	
	
	
	
	exposureControlNames.setName("exposureControlNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.exposureControlNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.exposureControlNames[i], false);
		exposureControlNames.add(item);
	}
	
	meteringNames.setName("meteringNames");	
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.meteringNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.meteringNames[i], false);
		meteringNames.add(item);
	}
	whiteBalanceNames.setName("whiteBalanceNames");	
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.whiteBalanceNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.whiteBalanceNames[i], false);
		whiteBalanceNames.add(item);
	}
	
	imageFilterNames.setName("imageFilterNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.imageFilterNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.imageFilterNames[i], false);
		imageFilterNames.add(item);
	}
	
	parameters.add(frameStabilizationEnabled);
	parameters.add(sharpness);
	parameters.add(brightness);
	parameters.add(contrast);
	parameters.add(saturation);
	parameters.add(colorEnhancementEnabled);
	parameters.add(ledEnabled);
		parameters.add(videoCodingNames);
		parameters.add(exposureControlNames);
		parameters.add(meteringNames);
		parameters.add(whiteBalanceNames);
		parameters.add(imageFilterNames);
		
	
	enableSync = true;
	if (enableSync) 
	{
		sync.setup(parameters, localPort, "localhost", remotePort);
		ofAddListener(ofEvents().update, this, &ControlPanel::onUpdate);
	}
	
	ofLogVerbose() << "parameters: " << parameters.toString();
	ofAddListener(parameters.parameterChangedE, this, &ControlPanel::onParameterGroupChanged);
	
	
}
ofParameter<bool> createBoolean(ofXml& xmlParser)
{
    string childName = xmlParser.getName();
    ofParameter<bool> item;
    item.set(childName, xmlParser.getBoolValue());
    return item;
}

ofParameterGroup* createParameterGroup(ofXml& xmlParser)
{
    ofParameterGroup* parameterGroup = new ofParameterGroup();
    string elementName = xmlParser.getName();
    parameterGroup->setName(elementName);
    int numElementChildren = xmlParser.getNumChildren();
    for(int j=0; j<numElementChildren; j++)
    {
        
        xmlParser.setToChild(j);
			ofParameter<bool> item = createBoolean(xmlParser);
			parameterGroup->add(item);
        xmlParser.setToParent();
        
    }
    return parameterGroup;
}


void ControlPanel::saveXML()
{
	string filename = ofToDataPath("DocumentRoot/output.xml", true);
	
	ofFile existingFile(filename);
	existingFile.remove(false);
	
	serializer->load(filename);
	serializer->serialize(parameters);
	
	ofXml xmlParser(*serializer);
	ofLogVerbose() << "xmlParser toString: " << xmlParser.toString();
	
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
	
	
	ofLogVerbose() << "xmlParser processed: " << xmlParser.toString();
	serializer->save(filename);
	
	
}

void ControlPanel::createXMLFromParam(ofAbstractParameter& parameter, ofXml& xml)
{

	string parameterName = xml.getName();
	if(parameter.type()==typeid(ofParameter<int>).name())
	{
		xml.removeContents();
		xml.addValue<string>("type", "int");
		xml.addValue<int>("value", parameter.cast<int>());
		xml.addValue<int>("min", parameter.cast<int>().getMin());
		xml.addValue<int>("max", parameter.cast<int>().getMax());
		
	}
	else if(parameter.type()==typeid(ofParameter<float>).name())
	{
		xml.removeContents();
		xml.addValue<string>("type", "float");
		xml.addValue<float>("value", parameter.cast<float>());
		xml.addValue<float>("min", parameter.cast<float>().getMin());
		xml.addValue<float>("max", parameter.cast<float>().getMax());
	}
	else if(parameter.type()==typeid(ofParameter<bool>).name())
	{
		xml.removeContents();
		xml.addValue<string>("type", "bool");
		xml.addValue<bool>("value", parameter.cast<bool>());
	}
	else if(parameter.type()==typeid(ofParameter<string>).name())
	{
		xml.removeContents();
		xml.addValue<string>("type", "string");
		xml.addValue<string>("value", parameter.cast<string>());
	}
		
}
void ControlPanel::onUpdate(ofEventArgs &args)
{
	sync.update();
}


void ControlPanel::onSharpnessChanged(int & sharpness_)
{
	ofLogVerbose(__func__) << sharpness_;
	rpiCameraVideoGrabber->setSharpness(sharpness_);
}

void ControlPanel::onContrastChanged(int & contrast_)
{
	ofLogVerbose(__func__) << contrast_;
	rpiCameraVideoGrabber->setContrast(contrast_);
}

void ControlPanel::onBrightnessChanged(int & brightness_)
{
	ofLogVerbose(__func__) << brightness_;
	rpiCameraVideoGrabber->setBrightness(brightness_);
}

void ControlPanel::onSaturationChanged(int & saturation_)
{
	ofLogVerbose(__func__) << saturation_;
	rpiCameraVideoGrabber->setSaturation(saturation_);
}
void ControlPanel::onFrameStabilizationChanged(bool & doFrameStabilization)
{
	rpiCameraVideoGrabber->setFrameStabilization(doFrameStabilization);
}

void ControlPanel::onColorEnhancementChanged(bool & doColorEnhancement)
{
	rpiCameraVideoGrabber->setColorEnhancement(doColorEnhancement);
}

void ControlPanel::onLEDEnabledChanged(bool & doEnableLED)
{
	rpiCameraVideoGrabber->setLEDStatus(doEnableLED);
}

