#include "testApp.h"






//--------------------------------------------------------------
void testApp::setup()
{
	imageFilters.push_back(OMX_ImageFilterNone);
	imageFilters.push_back(OMX_ImageFilterNoise);
	imageFilters.push_back(OMX_ImageFilterEmboss);
	imageFilters.push_back(OMX_ImageFilterNegative);
	imageFilters.push_back(OMX_ImageFilterSketch);
	imageFilters.push_back(OMX_ImageFilterOilPaint);
	imageFilters.push_back(OMX_ImageFilterHatch);
	imageFilters.push_back(OMX_ImageFilterGpen);
	imageFilters.push_back(OMX_ImageFilterAntialias); 
	imageFilters.push_back(OMX_ImageFilterDeRing);       
	imageFilters.push_back(OMX_ImageFilterSolarize);
	imageFilters.push_back(OMX_ImageFilterWatercolor);
	imageFilters.push_back(OMX_ImageFilterPastel);
	imageFilters.push_back(OMX_ImageFilterSharpen);
	imageFilters.push_back(OMX_ImageFilterFilm);
	imageFilters.push_back(OMX_ImageFilterBlur);
	imageFilters.push_back(OMX_ImageFilterSaturation);
	imageFilters.push_back(OMX_ImageFilterDeInterlaceLineDouble);
	imageFilters.push_back(OMX_ImageFilterDeInterlaceAdvanced);
	imageFilters.push_back(OMX_ImageFilterColourSwap);
	imageFilters.push_back(OMX_ImageFilterWashedOut);
	imageFilters.push_back(OMX_ImageFilterColourPoint);
	imageFilters.push_back(OMX_ImageFilterPosterise);
	imageFilters.push_back(OMX_ImageFilterColourBalance);
	imageFilters.push_back(OMX_ImageFilterCartoon);
	/*if ((unsigned int)imageFiltersCounter+1<imageFilters.size())
	{
		imageFiltersCounter++;
	}else 
	{
		imageFiltersCounter=0;
	}
	ofLogVerbose() << "imageFiltersCounter: " << imageFiltersCounter;*/
	
	doShader = false;
	ofSetLogLevel(OF_LOG_VERBOSE);
	consoleListener.setup(this);
	isShaderLoaded = false;
	//shader.load("PostProcessing.vert", "PostProcessing.frag", "");
	omxVideoGrabber.setup(1280, 720, 60);
	
}

//--------------------------------------------------------------
void testApp::update()
{
	if (!omxVideoGrabber.isReady) 
	{
		return;
	}
}


//--------------------------------------------------------------
void testApp::draw(){

	if (!omxVideoGrabber.isReady) 
	{
		return;
	}
	string info;
	if (doShader) 
	{
		if (!isShaderLoaded) 
		{
			isShaderLoaded  = shader.load("PostProcessing.vert", "PostProcessing.frag", "");
		}
		shader.begin();
		omxVideoGrabber.draw();
		shader.setUniformTexture("tex0", omxVideoGrabber.tex, omxVideoGrabber.textureID);
		shader.setUniform1f("time", ofGetElapsedTimef());
		shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight()); 
		shader.end();
		
		info = "ODDLY IF I USE MY A SECOND SHADER THE IMAGE IS SCALED CORRECTLY";
		
	}else 
	{
		omxVideoGrabber.draw();
		info = "HERE THE TEXTURE IS SCALED LARGER THAN 1280X720";
		
		
	}

	
	ofDrawBitmapStringHighlight(info, 100, 100, ofColor::black, ofColor::yellow);
}



void testApp::exit()
{
	/*if (!omxVideoGrabber.isClosed) 
	{
		ofLogVerbose() << "closing manually";
		omxVideoGrabber.close();
	}*/

}
//--------------------------------------------------------------
void testApp::keyPressed  (int key)
{

	if (key == 's') 
	{
		doShader = !doShader;
		//tex.getTextureData().bFlipTexture = !tex.getTextureData().bFlipTexture;
		//tex.relo
	}
	if (key == 'e')
	{
		omxVideoGrabber.applyImageFilter(imageFilters[ofRandom(imageFilters.size())]);
		
	}


}

void testApp::onCharacterReceived(SSHKeyListenerEventData& e)
{
	keyPressed((int)e.character);
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

