#include "shaderApp.h"

//--------------------------------------------------------------
void shaderApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);
	
	doDrawInfo	= true;
		
	consoleListener.setup(this);
	
	
	omxCameraSettings.width = 1280;
	omxCameraSettings.height = 720;
	omxCameraSettings.framerate = 30;
	omxCameraSettings.isUsingTexture = true;
	videoGrabber.setup(omxCameraSettings);
	

	doShader = true;
	doPixels = true;
	
	if (doPixels) 
	{
		videoPixels = new unsigned char[omxCameraSettings.width * omxCameraSettings.height *4];
		videoTexture.allocate(omxCameraSettings.width, omxCameraSettings.height, GL_RGBA);
	}
	
	if (doShader) 
	{
		ofEnableAlphaBlending();
		
		filterCollection.setup();
		fbo.allocate(omxCameraSettings.width, omxCameraSettings.height);
		bool useShaderExample = true;
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
		
	}
	
		
}	

//--------------------------------------------------------------
void shaderApp::update()
{
	if (!doShader)
	{
		return;
	}
	fbo.begin();
		//ofClear(0, 0, 0, 0);
		shader.begin();
			shader.setUniformTexture("tex0", videoGrabber.getTextureReference(), videoGrabber.getTextureID());
			shader.setUniform1f("time", ofGetElapsedTimef());
			shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
			videoGrabber.draw();
		shader.end();
		if(doPixels)
		{
			glReadPixels(0,0, omxCameraSettings.width, omxCameraSettings.height, GL_RGBA, GL_UNSIGNED_BYTE, videoPixels);	
		}	
	fbo.end();
	
	if(doPixels)
	{
		videoTexture.loadData(videoPixels, omxCameraSettings.width, omxCameraSettings.height, GL_RGBA);	
	}

}


//--------------------------------------------------------------
void shaderApp::draw(){
	
	if (doShader)
	{
		fbo.draw(0, 0);
		if(doPixels)
		{
			videoTexture.draw(0, 0, omxCameraSettings.width/2, omxCameraSettings.height/2);
		}
		
	}else 
	{
		videoGrabber.draw();
	}

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
	if (key == 'p')
	{
		doPixels = !doPixels;
	}
}

void shaderApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}

