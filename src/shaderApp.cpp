#include "shaderApp.h"

//--------------------------------------------------------------
void shaderApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	OMXCameraSettings omxCameraSettings;
	omxCameraSettings.width = 1280;
	omxCameraSettings.height = 720;
	omxCameraSettings.framerate = 30;
	omxCameraSettings.isUsingTexture = true;
	videoGrabber.setup(omxCameraSettings);
	
	doShader = true;
	if (doShader) 
	{
		ofEnableAlphaBlending();
		
		filterCollection.setup(&videoGrabber.omxMaps);
		//fbo.allocate(omxCameraSettings.width, omxCameraSettings.height);
		fbo.createAndAttachTexture(videoGrabber.getTextureReference(), GL_RGBA, 0);
		bool useShaderExample = false;
		if (useShaderExample) 
		{
			shader.load("shaderExample");
		}else 
		{
			fbo.getTextureReference().getTextureData().bFlipTexture = true;
			shader.load("PostProcessing");
		}
		fbo.begin();
			ofClear(0, 0, 0, 0);
		fbo.end();
		
		videoImage.allocate(omxCameraSettings.width, omxCameraSettings.height, OF_IMAGE_COLOR_ALPHA);
		/*format = ofGetGLFormatFromInternal(pixelReader.settings.internalformat);	
		if(format == GL_RGBA)
		{
			//ofLogVerbose() << "USING GL_RGBA";
		}*/
		/*pixels.allocate(settings.width,settings.height,ofGetImageTypeFromGLType(settings.internalformat));
		;*/
	}
	
		
}	

//--------------------------------------------------------------
void shaderApp::update()
{

	fbo.begin();
	//ofClear(0, 0, 0, 0);
	shader.begin();
	shader.setUniformTexture("tex0", videoGrabber.getTextureReference(), videoGrabber.getTextureID());
	shader.setUniform1f("time", ofGetElapsedTimef());
	shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	videoGrabber.draw();
	shader.end();
	//glReadPixels(0,0, videoImage.getWidth(), videoImage.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, videoImage.getPixels());
	fbo.end();
	
	fbo.bind();
	//fbo.readToPixels(videoImage.getPixelsRef());
	glReadPixels(0,0, videoImage.getWidth(), videoImage.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, videoImage.getPixels());
	fbo.unbind();
	videoImage.reloadTexture();
	
	return;
	
	
	pixelReader.bind();
	glReadPixels(0,0, videoImage.getWidth(), videoImage.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, videoImage.getPixels());
	pixelReader.unbind();
	//int format = ofGetGLFormatFromInternal(settings.internalformat);
		
		//pixelReader.unbind();
	//videoImage.reloadTexture();
	//pixelReader.readToPixels(videoImage.getPixelsRef());
		//int format = ofGetGLFormatFromInternal(settings.internalformat);
		//glReadPixels(0,0, videoImage.getWidth(), videoImage.getHeight(), format, GL_UNSIGNED_BYTE, videoImage.getPixels());
	//pixelReader.unbind();
}


//--------------------------------------------------------------
void shaderApp::draw(){
	
	fbo.draw(0, 0);
	
	//fbo.getTextureReference().draw(0, 0, 640, 480);
	/*if (doShader) 
	{
		fbo.draw(0, 0);
	}else 
	{
		videoGrabber.draw();
	}*/
	videoImage.draw(0, 0, videoImage.getWidth()/2, videoImage.getHeight()/2);
	

	
	

	stringstream info;
	info << "App FPS: " << ofGetFrameRate() << "\n";
	//info << "Camera frameCounter: " << videoGrabber.frameCounter << "\n";
	//info << "App frameCounter: " << ofGetFrameNum() << "\n";
	info << "Camera Resolution: " << videoGrabber.getWidth() << "x" << videoGrabber.getHeight()	<< " @ "<< videoGrabber.getFrameRate() <<"FPS"<< "\n";
	info << "CURRENT FILTER: " << filterCollection.getCurrentFilterName() << "\n";
	info << "SHADER ENABLED: " << doShader << "\n";
	//info <<	filterCollection.filterList << "\n";
	
	info << "\n";
	info << "Press e to increment filter" << "\n";
	info << "Press g to Toggle info" << "\n";
	
	if (doDrawInfo) 
	{
		ofDrawBitmapStringHighlight(info.str(), 100, 100, ofColor::black, ofColor::yellow);
	}
	
	//
}

//--------------------------------------------------------------
void shaderApp::keyPressed  (int key)
{
	ofLogVerbose(__func__) << key;
	
	
	if (key == 'e')
	{
		videoGrabber.applyImageFilter(filterCollection.getNextFilter());
	}
	
	if (key == 'g')
	{
		doDrawInfo = !doDrawInfo;
	}
	if (key == 'c')
	{
		//controlPanel.increaseContrast();
	}
	if (key == 'd')
	{
		doShader = !doShader;
	}
	
}

void shaderApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

