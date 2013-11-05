#include "ControlPanel.h"


ControlPanel::ControlPanel()
{
	localPort = 6666;
	remotePort = 6667;
	rpiCameraVideoGrabber = NULL;
	serializer = ofPtr<ofBaseFileSerializer> (new ofXml);
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
		
	
	enableSync = false;
	if (enableSync) 
	{
		sync.setup(parameters, localPort, "localhost", remotePort);
		ofAddListener(ofEvents().update, this, &ControlPanel::onUpdate);
	}
	
	
	//ofAddListener(parameters.parameterChangedE, this, &ControlPanel::onParameterGroupChanged);
	
	
}
void ControlPanel::saveXML()
{
	string filename = ofToDataPath("DocumentRoot/output.xml", true);
	
	ofFile existingFile(filename);
	existingFile.remove(false);
	
	serializer->load(filename);
	serializer->serialize(parameters);
	serializer->save(filename);
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

