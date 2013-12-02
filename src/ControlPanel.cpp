#include "ControlPanel.h"

ControlPanel::ControlPanel()
{
	localPort = 6666;
	remotePort = 7777;
	rpiCameraVideoGrabber = NULL;
	doDrawGui = true;
}

void ControlPanel::onParameterGroupChanged(ofAbstractParameter & param)
{
	ofLogVerbose(__func__) << "param.getName: " << param.getName();
	sync->sender.sendParameter(param);
	
}

void ControlPanel::onVideoCodingNamesChanged(ofAbstractParameter & param)
{
	ofLogVerbose(__func__) << "param.getName(): " << param.getName();
	ofRemoveListener(videoCodingNames.parameterChangedE, this, &ControlPanel::onVideoCodingNamesChanged);	
	for (int i=0; i<videoCodingNames.size(); i++) 
	{
		if(videoCodingNames.getName(i) != param.getName())
		{
			videoCodingNames.getBool(i) = false;
			//ofLogVerbose(__func__) << "setting " << videoCodingNames.getName(i) << " to false";
		}
		
	}
	ofAddListener(videoCodingNames.parameterChangedE, this, &ControlPanel::onVideoCodingNamesChanged);	

}



void ControlPanel::onImageFilterNamesChanged(ofAbstractParameter & param)
{
	
	ofRemoveListener(imageFilterNames.parameterChangedE, this, &ControlPanel::onImageFilterNamesChanged);
	for (int i=0; i<imageFilterNames.size(); i++) 
	{
		if(imageFilterNames.getName(i) != param.getName())
		{
			imageFilterNames.getBool(i) = false;
		}else 
		{
			string selectedName = imageFilterNames.getName(i);
			map<OMX_IMAGEFILTERTYPE, string>::iterator it;			
			for (it = rpiCameraVideoGrabber->omxMaps.imageFilterTypes.begin(); it != rpiCameraVideoGrabber->omxMaps.imageFilterTypes.end(); ++it)
			{
				if (it->second == selectedName)
				{
					rpiCameraVideoGrabber->applyImageFilter(it->first);
					break;
				}
			}
		}
		
	}
	ofAddListener(imageFilterNames.parameterChangedE, this, &ControlPanel::onImageFilterNamesChanged);
}

void ControlPanel::onWhiteBalanceNamesChanged(ofAbstractParameter & param)
{
	
	//gui.getGroup("whiteBalanceNames").maximize();
	ofRemoveListener(whiteBalanceNames.parameterChangedE, this, &ControlPanel::onWhiteBalanceNamesChanged);
	for (int i=0; i<whiteBalanceNames.size(); i++) 
	{
		if(whiteBalanceNames.getName(i) != param.getName())
		{
			whiteBalanceNames.getBool(i) = false;
		}else 
		{
			string selectedName = whiteBalanceNames.getName(i);
			map<OMX_WHITEBALCONTROLTYPE, string>::iterator it;			
			for (it = rpiCameraVideoGrabber->omxMaps.whiteBalanceControlTypes.begin(); it != rpiCameraVideoGrabber->omxMaps.whiteBalanceControlTypes.end(); ++it)
			{
				if (it->second == selectedName)
				{
					rpiCameraVideoGrabber->setWhiteBalance(it->first);
					break;
				}
			}
		}
		
	}
	ofAddListener(whiteBalanceNames.parameterChangedE, this, &ControlPanel::onWhiteBalanceNamesChanged);
}
void ControlPanel::onExposureControlToggleGroupChange(ToggleGroupListenerEventData & eventData)
{
	ofLogVerbose(__func__) << "eventData.selectedName:" << eventData.selectedName;
	map<OMX_EXPOSURECONTROLTYPE, string>::iterator it;
	for (it = rpiCameraVideoGrabber->omxMaps.exposureControlTypes.begin(); it != rpiCameraVideoGrabber->omxMaps.exposureControlTypes.end(); ++it)
	{
		if (it->second == eventData.selectedName)
		{
			rpiCameraVideoGrabber->setExposureMode(it->first);
			break;
		}
	}
}
void ControlPanel::onExposureControlNamesChanged(ofAbstractParameter & param)
{
	ofLogVerbose(__func__) << "onExposureControlNamesChanged";
	ofRemoveListener(exposureControlNames.parameterChangedE, this, &ControlPanel::onExposureControlNamesChanged);
	for (int i=0; i<exposureControlNames.size(); i++) 
	{
		if(exposureControlNames.getName(i) != param.getName())
		{
			exposureControlNames.getBool(i) = false;
		}else 
		{
			string selectedName = exposureControlNames.getName(i);
			map<OMX_EXPOSURECONTROLTYPE, string>::iterator it;
			for (it = rpiCameraVideoGrabber->omxMaps.exposureControlTypes.begin(); it != rpiCameraVideoGrabber->omxMaps.exposureControlTypes.end(); ++it)
			{
				if (it->second == selectedName)
				{
					rpiCameraVideoGrabber->setExposureMode(it->first);
					break;
				}
			}
			
			
			
		}

	}
	ofAddListener(exposureControlNames.parameterChangedE, this, &ControlPanel::onExposureControlNamesChanged);
}

void ControlPanel::onMeteringNamesChanged(ofAbstractParameter & param)
{
	ofRemoveListener(meteringNames.parameterChangedE, this, &ControlPanel::onMeteringNamesChanged);
	for (int i=0; i<meteringNames.size(); i++) 
	{
		if(meteringNames.getName(i) != param.getName())
		{
			meteringNames.getBool(i) = false;
		}else 
		{
			string selectedName = meteringNames.getName(i);
			map<OMX_METERINGTYPE, string>::iterator it;
			for (it = rpiCameraVideoGrabber->omxMaps.meteringTypes.begin(); it != rpiCameraVideoGrabber->omxMaps.meteringTypes.end(); ++it)
			{
				if (it->second == selectedName)
				{
					rpiCameraVideoGrabber->setMeteringMode(it->first);
					break;
				}
			}
		}
		
	}
	ofAddListener(meteringNames.parameterChangedE, this, &ControlPanel::onMeteringNamesChanged);
}


void ControlPanel::setup(ofxRPiCameraVideoGrabber* rpiCameraVideoGrabber)
{
	
	ofParameter<int> sharpness;
	ofParameter<int> contrast;
	ofParameter<int> brightness;
	ofParameter<int> saturation;
	ofParameter<bool> frameStabilizationEnabled;
	ofParameter<bool> colorEnhancementEnabled;
	ofParameter<bool> ledEnabled;
	ofParameter<bool> drawGui;
	
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
	
	drawGui.set("drawGui", doDrawGui);
	drawGui.addListener(this, &ControlPanel::onDrawGuiChanged);
	
	
	
	
	size_t i=0;
	videoCodingNames.setName("videoCodingNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.videoCodingNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.videoCodingNames[i], false);
		videoCodingNames.add(item);
	}
	
	ofAddListener(videoCodingNames.parameterChangedE, this, &ControlPanel::onVideoCodingNamesChanged);	
	
	
	
	/*exposureControlNames.setName("exposureControlNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.exposureControlNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.exposureControlNames[i], false);
		exposureControlNames.add(item);
	}
	ofAddListener(exposureControlNames.parameterChangedE, this, &ControlPanel::onExposureControlNamesChanged);*/
	
	ToggleGroup* exposureControlToggleGroup = new ToggleGroup();
	exposureControlToggleGroup->setup(&exposureControlNames, "exposureControlNames", &rpiCameraVideoGrabber->omxMaps.getExposureControlNames());
	exposureControlToggleGroup->addListener(this, &ControlPanel::onExposureControlToggleGroupChange);
	//ofAddListener(exposureControlToggleGroup->changeDispatcher,this, &ControlPanel::onExposureControlToggleGroupChange);
	meteringNames.setName("meteringNames");	
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.meteringNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.meteringNames[i], false);
		meteringNames.add(item);
	}
	ofAddListener(meteringNames.parameterChangedE, this, &ControlPanel::onMeteringNamesChanged);
	
	
	
	whiteBalanceNames.setName("whiteBalanceNames");	
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.whiteBalanceNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.whiteBalanceNames[i], false);
		whiteBalanceNames.add(item);
	}
	ofAddListener(whiteBalanceNames.parameterChangedE, this, &ControlPanel::onWhiteBalanceNamesChanged);
	
	
	imageFilterNames.setName("imageFilterNames");
	for (i=0; i<rpiCameraVideoGrabber->omxMaps.imageFilterNames.size(); i++)
	{
		ofParameter<bool> item;
		item.set(rpiCameraVideoGrabber->omxMaps.imageFilterNames[i], false);
		imageFilterNames.add(item);
	}
	ofAddListener(imageFilterNames.parameterChangedE, this, &ControlPanel::onImageFilterNamesChanged);
	
	
	
	parameters.add(frameStabilizationEnabled);
	parameters.add(sharpness);
	parameters.add(brightness);
	parameters.add(contrast);
	parameters.add(saturation);
	parameters.add(colorEnhancementEnabled);
	parameters.add(ledEnabled);
	parameters.add(drawGui);
		//parameters.add(videoCodingNames);
		//parameters.add(exposureControlNames);
		parameters.add(exposureControlNames);
		
		parameters.add(meteringNames);
		parameters.add(whiteBalanceNames);
		parameters.add(imageFilterNames);
		
	
	gui.setup(parameters);
  
	guiParamGroup = (ofParameterGroup*)&gui.getParameter();

	sync = new OSCParameterSync();
	sync->setup(*guiParamGroup, localPort, "JVCTOWER.local", remotePort);
	string filename = ofToDataPath("DocumentRoot/modified.xml", true);
	parameterUtils.saveXML(parameters, filename);
	
	//ofLogVerbose(__func__) << "parameters: " << parameters.toString();
	ofAddListener(parameters.parameterChangedE, this, &ControlPanel::onParameterGroupChanged);
	
	gui.setPosition(300, 44);
	gui.minimizeAll();
	ofAddListener(ofEvents().update, this, &ControlPanel::onUpdate);
	
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
	rpiCameraVideoGrabber->setLEDStatus(doEnableLED);
}

void ControlPanel::onDrawGuiChanged(bool &doDrawGui)
{
	this->doDrawGui = doDrawGui;
}

