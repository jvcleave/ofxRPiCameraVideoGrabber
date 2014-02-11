#include "ControlPanel.h"

ControlPanel::ControlPanel()
{
	localPort = 6666;
	remotePort = 7777;
	rpiCameraVideoGrabber = NULL;
	doDrawGui = true;
	doActionItem = false;
}


void ControlPanel::onParameterGroupChanged(ofAbstractParameter & param)
{
	ofLogVerbose(__func__) << "param.getName: " << param.getName();
	sync->sender.sendParameter(param);
	
}


void ControlPanel::onWhiteBalanceToggleGroupChange(ToggleGroupListenerEventData & eventData)
{
	
	rpiCameraVideoGrabber->setWhiteBalance(OMX_Maps::getInstance().whiteBalanceControls[eventData.selectedName]);
}


void ControlPanel::onImageFilterToggleGroupChange(ToggleGroupListenerEventData & eventData)
{
	
	rpiCameraVideoGrabber->applyImageFilter(OMX_Maps::getInstance().imageFilters[eventData.selectedName]);

}


void ControlPanel::onExposureControlToggleGroupChange(ToggleGroupListenerEventData & eventData)
{
	rpiCameraVideoGrabber->setExposureMode(OMX_Maps::getInstance().exposureControls[eventData.selectedName]);
	
}


void ControlPanel::onMeteringNamesToggleGroupChange(ToggleGroupListenerEventData & eventData)
{
	rpiCameraVideoGrabber->setMeteringMode(OMX_Maps::getInstance().metering[eventData.selectedName]);
}


void ControlPanel::setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber, string remoteIP)
{
	
	
	
	
	
	this->rpiCameraVideoGrabber = rpiCameraVideoGrabber;
	this->remoteIP = remoteIP;
	
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
	
	drawGui.set("drawGui", doDrawGui);
	drawGui.addListener(this, &ControlPanel::onDrawGuiChanged);
	
	
	actionItem.set("actionItem", doActionItem);
	actionItem.addListener(this, &ControlPanel::onActionItemChanged);
	ToggleGroup* exposureControlToggleGroup = createToggleGroup(&exposureControlNames, 
																"exposureControlNames", 
																&OMX_Maps::getInstance().exposureControlNames,
																&ControlPanel::onExposureControlToggleGroupChange);
	
	ToggleGroup* meteringToggleGroup = createToggleGroup(&meteringNames, 
																"meteringNames", 
																&OMX_Maps::getInstance().meteringNames,
																&ControlPanel::onMeteringNamesToggleGroupChange);
	
	ToggleGroup* whiteBalanceToggleGroup = createToggleGroup(&whiteBalanceNames, 
														 "whiteBalanceNames", 
														 &OMX_Maps::getInstance().whiteBalanceNames,
														 &ControlPanel::onWhiteBalanceToggleGroupChange);
	
	ToggleGroup* imageFilterToggleGroup = createToggleGroup(&imageFilterNames, 
															 "imageFilterNames", 
															 &OMX_Maps::getInstance().imageFilterNames,
															 &ControlPanel::onImageFilterToggleGroupChange);
	parameters.add(frameStabilizationEnabled);
	parameters.add(sharpness);
	parameters.add(brightness);
	parameters.add(contrast);
	parameters.add(saturation);
	parameters.add(colorEnhancementEnabled);
	parameters.add(ledEnabled);
	parameters.add(drawGui);
	parameters.add(actionItem);
		parameters.add(exposureControlNames);
		parameters.add(meteringNames);
		parameters.add(whiteBalanceNames);
		parameters.add(imageFilterNames);
		
	
	gui.setup(parameters);
  
	guiParamGroup = (ofParameterGroup*)&gui.getParameter();

	sync = new OSCParameterSync();
	sync->setup(*guiParamGroup, localPort, remoteIP, remotePort);
	string filename = ofToDataPath("DocumentRoot/modified.xml", true);
	parameterUtils.saveXML(parameters, filename);
	
	//ofLogVerbose(__func__) << "parameters: " << parameters.toString();
	ofAddListener(parameters.parameterChangedE, this, &ControlPanel::onParameterGroupChanged);
	
	gui.setPosition(300, 44);
	//gui.minimizeAll();
	ofAddListener(ofEvents().update, this, &ControlPanel::onUpdate);
	
}


template<typename ListenerMethod>
ToggleGroup* ControlPanel::createToggleGroup(ofParameterGroup* parameterGroup, string groupName, vector<string>* names, ListenerMethod callback)
{
	ToggleGroup* toggleGroup = new ToggleGroup();
	toggleGroup->setup(parameterGroup, groupName, names);
	toggleGroup->addListener(this, callback);
	return toggleGroup;
}


void ControlPanel::onUpdate(ofEventArgs &args)
{
	sync->update();
}


void ControlPanel::draw()
{
	if(!doDrawGui)
	{
		return;
	}
	gui.draw();
}


void ControlPanel::increaseContrast()
{
	guiParamGroup->get("contrast").cast<int>()++;
}


void ControlPanel::onSharpnessChanged(int & sharpness)
{
	//ofLogVerbose(__func__) << sharpness;
	rpiCameraVideoGrabber->setSharpness(sharpness);
}



void ControlPanel::onContrastChanged(int & contrast)
{
	//ofLogVerbose(__func__) << contrast;
	rpiCameraVideoGrabber->setContrast(contrast);
}


void ControlPanel::onBrightnessChanged(int & brightness)
{
	//ofLogVerbose(__func__) << brightness;
	rpiCameraVideoGrabber->setBrightness(brightness);
}


void ControlPanel::onSaturationChanged(int & saturation)
{
	//ofLogVerbose(__func__) << saturation;
	rpiCameraVideoGrabber->setSaturation(saturation);
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
	rpiCameraVideoGrabber->setLEDState(doEnableLED);
}


void ControlPanel::onDrawGuiChanged(bool &doDrawGui)
{
	this->doDrawGui = doDrawGui;
}

void ControlPanel::onActionItemChanged(bool &doAction)
{
	this->doActionItem = doActionItem;
	//ofSleepMillis(2000);
	ofLogVerbose(__func__) << "actionItem: " << actionItem;
	actionItem = false;
	this->doActionItem = actionItem;
}


