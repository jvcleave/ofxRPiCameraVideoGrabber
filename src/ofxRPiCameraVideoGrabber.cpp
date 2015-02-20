/*
 *  ofxRPiCameraVideoGrabber.cpp
 *
 *  Created by jason van cleave on 6/1/13.
 *  Thanks to https://github.com/linuxstb/pidvbip for the example of configuring the camera via OMX
 *
 */

#include "ofxRPiCameraVideoGrabber.h"


ofxRPiCameraVideoGrabber::ofxRPiCameraVideoGrabber()
{
    OMX_Maps::getInstance();
    initStructures();
    hasOMXInit = false;
    updateFrameCounter = 0;
    frameCounter = 0;
    hasNewFrame = false;
    engine = NULL;
    textureEngine = NULL;
    appEGLWindow = NULL;
    pixels = NULL;
    eglImage = NULL;
    doPixels = false;
    textureID	= 0;
    hasExitHandler  = false;
    zoomLevel = 0;
    camera = NULL;
    doSaveImage = false;
    doRawSave = false;
    doStartRecording = false;
}

void ofxRPiCameraVideoGrabber::saveImage()
{
    doRawSave = false;
    doSaveImage = true;
}
void ofxRPiCameraVideoGrabber::saveRawImage()
{
    ofLogVerbose(__func__) << "";
    doRawSave = true;
    doSaveImage = true;
}


void ofxRPiCameraVideoGrabber::setup(OMXCameraSettings omxCameraSettings_)
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(engine)
    {
        delete engine;
        engine = NULL;
        
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
    }
    if(doStartRecording)
    {
        ofSleepMillis(2000); 
    }
     
    addExitHandler();
    if(!hasOMXInit)
    {
        OMX_ERRORTYPE error = OMX_Init();
        if (error == OMX_ErrorNone) 
        {
            ofLogVerbose(__func__) << "OMX_Init PASS";
            hasOMXInit = true;
        }  
    }
    
    omxCameraSettings = omxCameraSettings_;
    omxCameraSettings.applyPreset();
    if (omxCameraSettings.enablePixels) 
    {
        enablePixels();
    }
    
    if (omxCameraSettings.isUsingTexture) 
    {
        generateEGLImage(omxCameraSettings.width, omxCameraSettings.height);
        textureEngine = new TextureEngine();
        textureEngine->eglImage = eglImage;
        textureEngine->setup(omxCameraSettings);
        ofLogVerbose() << "textureEngine->setup";
        camera = textureEngine->camera;
        
    }else 
    {
        engine = new NonTextureEngine();
        engine->setup(omxCameraSettings);
        ofLogVerbose() << "engine->setup";
        camera = engine->camera;
    }
    
    setDefaultValues();
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
}

void ofxRPiCameraVideoGrabber::setDefaultValues()
{
    
    setExposurePreset(OMX_ExposureControlAuto); 
    
    if(omxCameraSettings.doManualExposure)
    {
        currentMeteringMode.autoShutter = false;
        currentMeteringMode.autoAperture = false;
        currentMeteringMode.autoISO = false;
        //currentMeteringMode.evCompensation = 6;
    }
    
    applyCurrentMeteringMode();
    setSharpness(-50);
    setContrast(-10);
    setBrightness(50);
    setSaturation(0);
    setFrameStabilization(false);
    setWhiteBalance(OMX_WhiteBalControlAuto);
    applyImageFilter(OMX_ImageFilterNone);
    setColorEnhancement(false);	 
    setDRC(0);
    cropRectangle.set(0, 0, 100, 100);
    setSensorCrop(cropRectangle);
    resetZoom();
    setRotation(ROTATION_0);
    setMirror(MIRROR_NONE);
    //Requires gpio program provided via wiringPi
    //https://projects.drogon.net/raspberry-pi/wiringpi/the-gpio-utility/
    
    ofFile gpioProgram("/usr/local/bin/gpio");
    if(gpioProgram.exists())
    {
        if(system("gpio export 5 out") == 0)
        {
           //silence compiler warning 
        }
        LED_CURRENT_STATE = true;
        setLEDState(LED_CURRENT_STATE);
    }  
    
    OMX_GetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
    OMX_GetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
    
    //disableSoftwareSaturation();
    //disableSoftwareSharpening();
    
    ofLogVerbose(__func__) << "isSoftwareSharpeningEnabled: " << isSoftwareSharpeningEnabled();
    
    ofLogVerbose(__func__) << "isSoftwareSaturationEnabled: " << isSoftwareSaturationEnabled();
    saveState();

}

void ofxRPiCameraVideoGrabber::saveState()
{
    stringstream state;
    //Exposure Preset
    for (map<string, OMX_EXPOSURECONTROLTYPE>::iterator it=OMX_Maps::getInstance().exposurePresets.begin(); 
         it!=OMX_Maps::getInstance().exposurePresets.end(); 
         ++it)
    {
        if(it->second == exposurePresetConfig.eExposureControl)
        {
            
            state << "exposurePresets: " << it->first << "\n";
        }
    }
    
    //Metering Mode
    
    state << "meteringType="               << currentMeteringMode.getMeteringTypeString();
    state << "evCompensation="             << currentMeteringMode.evCompensation           << "\n";
    state << "autoShutter="                << currentMeteringMode.autoShutter              << "\n";
    state << "shutterSpeedMicroSeconds="   << currentMeteringMode.shutterSpeedMicroSeconds << "\n";
    state << "autoAperture="               << currentMeteringMode.autoAperture             << "\n";
    state << "aperture="                   << currentMeteringMode.aperture                 << "\n";
    state << "autoISO="                    << currentMeteringMode.autoISO                  << "\n";
    state << "ISO="                        << currentMeteringMode.ISO                      << "\n";
    
    state << "sharpness="  << getSharpness()   << "\n";
    state << "contrast="   << getContrast()    << "\n";
    state << "brightness=" << getBrightness()  << "\n";
    state << "saturation=" << getSaturation()  << "\n";
    
    
    state << "framestabilization=" << fromOMXBool(framestabilizationConfig.bStab)          << "\n";
    
    //White Balance
    for (map<string, OMX_WHITEBALCONTROLTYPE>::iterator it=OMX_Maps::getInstance().whiteBalanceControls.begin(); 
         it!=OMX_Maps::getInstance().whiteBalanceControls.end(); 
         ++it)
    {
        if(it->second == whiteBalanceConfig.eWhiteBalControl)
        {
            
            state << "whiteBalance=" << it->first << "\n";
        }
    }
    
    //Image Filter
    for (map<string, OMX_IMAGEFILTERTYPE>::iterator it=OMX_Maps::getInstance().imageFilters.begin(); 
         it!=OMX_Maps::getInstance().imageFilters.end(); 
         ++it)
    {
        if(it->second == imagefilterConfig.eImageFilter)
        {
            
            state << "imageFilter=" << it->first << "\n";
        }
    }
    //DRC
    
    state << "drcLevel=" << OMX_Maps::getInstance().drcTypes[drcConfig.eMode]         << "\n";
    state << "cropRectangle=" << cropRectangle.x  << "," << cropRectangle.y << "," << cropRectangle.width << "," << cropRectangle.height << "\n";
    state << "zoomLevel="   << zoomLevel                    << "\n";
    state << "rotation="    << rotationConfig.nRotation     << "\n";
    state << "mirror="      << getMirrorAsString()     << "\n";
    
    //OMX_MIRRORTYPE mirrorType = OMX_Maps::getInstance().mirrorTypes[getMirrorAsString()];
    
    ofLogVerbose(__func__) << state.str();
}

string ofxRPiCameraVideoGrabber::getMirrorAsString()
{
    string value;
    switch (mirrorConfig.eMirror) {
        case MIRROR_NONE:       {value = "MIRROR_NONE";}        break;
        case MIRROR_VERTICAL:   {value = "MIRROR_VERTICAL";}    break;
        case MIRROR_HORIZONTAL: {value = "MIRROR_HORIZONTAL";}  break;
        case MIRROR_BOTH:       {value = "MIRROR_BOTH";}        break;
        default:                {value = "MIRROR_NONE";}        break;
    }
    return value;
}


void ofxRPiCameraVideoGrabber::printCameraInfo()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCameraInfo, &cameraInfoConfig);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << endl;
        ss << "cameraname: "        << cameraInfoConfig.cameraname                  << endl;
        ss << "lensname: "          << cameraInfoConfig.lensname                    << endl;
        ss << "nModelId: "          << fromQ16(cameraInfoConfig.nModelId)           << endl;
        ss << "nManufacturerId: "   << fromQ16(cameraInfoConfig.nManufacturerId)    << endl;
        ss << "nRevNum: "           << cameraInfoConfig.nRevNum                     << endl;
        ss << "sSerialNumber: "     << cameraInfoConfig.sSerialNumber               << endl;
        ss << "sEpromVersion: "     << cameraInfoConfig.sEpromVersion               << endl;
        
        ss << "xFNumber: "          << cameraInfoConfig.xFNumber                    << endl;
        ss << "xFNumber fromQ16: "  << fromQ16(cameraInfoConfig.xFNumber)           << endl;
        
        ss << "xFocalLength: "      << cameraInfoConfig.xFocalLength                << endl;
        ss << "xFocalLength fromQ16: "  << fromQ16(cameraInfoConfig.xFocalLength)   << endl;
        ss << "nShutterDelayTime fromQ16: "     << fromQ16(cameraInfoConfig.sLensCalibration.nShutterDelayTime)     << endl;
        ofLogVerbose(__func__) << ss.str();
    }
   
    frameRateRangeConfig.nPortIndex = CAMERA_OUTPUT_PORT;
    error = OMX_GetConfig(camera, OMX_IndexParamBrcmFpsRange, &frameRateRangeConfig);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << "CAMERA_OUTPUT_PORT" << endl;
        ss << "xFramerateLow: "     << fromQ16(frameRateRangeConfig.xFramerateLow)  << endl;
        ss << "xFramerateHigh: "    << fromQ16(frameRateRangeConfig.xFramerateHigh) << endl;
        ofLogVerbose(__func__) << ss.str();
    }else
    {
        ofLogError(__func__) << omxErrorToString(error);
    }
    
    frameRateRangeConfig.nPortIndex = CAMERA_PREVIEW_PORT;
    error = OMX_GetConfig(camera, OMX_IndexParamBrcmFpsRange, &frameRateRangeConfig);
    if(error == OMX_ErrorNone) 
    {
        stringstream ss;
        ss << "CAMERA_PREVIEW_PORT" << endl;
        ss << "xFramerateLow: "     << fromQ16(frameRateRangeConfig.xFramerateLow)  << endl;
        ss << "xFramerateHigh: "    << fromQ16(frameRateRangeConfig.xFramerateHigh) << endl;
        ofLogVerbose(__func__) << ss.str();
    }else
    {
        ofLogError(__func__) << omxErrorToString(error);
    }
}


BaseEngine* ofxRPiCameraVideoGrabber::getEngine()
{
    if (engine) 
    {
        return engine;
    }
    
    if (textureEngine) 
    {
        return textureEngine;
    }
    return NULL;
}


void ofxRPiCameraVideoGrabber::startRecording()
{
    doStartRecording = true;
}

bool ofxRPiCameraVideoGrabber::isRecording()
{
    if (engine) 
    {
        return engine->isRecording();
    }
    
    if (textureEngine) 
    {
        return textureEngine->isRecording();
    }
    return false;
}

bool ofxRPiCameraVideoGrabber::isReady()
{
    
    if (engine) 
    {
        return engine->isOpen;
    }
    
    if (textureEngine) 
    {
        return textureEngine->isOpen;
    }
    
    return false;
}

int ofxRPiCameraVideoGrabber::getWidth()
{
    return omxCameraSettings.width;
}

int ofxRPiCameraVideoGrabber::getHeight()
{
    return omxCameraSettings.height;
}

int ofxRPiCameraVideoGrabber::getFrameRate()
{
    return omxCameraSettings.framerate;
}

ofTexture& ofxRPiCameraVideoGrabber::getTextureReference()
{
    if (!textureEngine) 
    {
      
        //ofLogWarning(__func__) << "You are in non-texture mode but asking for a texture";
    }
    return texture;
}

void ofxRPiCameraVideoGrabber::generateEGLImage(int width, int height)
{
    ofLogVerbose() << "width: " << width << " height: " << height;
    bool needsRegeneration = false;
   
    if(!eglImage)
    {
        needsRegeneration = true;
    }
    if (!texture.isAllocated())
    {
        needsRegeneration = true;
    }
    
    if (texture.getWidth() != width)
    {
        needsRegeneration = true;
    }
    if (texture.getHeight() != height)
    {
        needsRegeneration = true;
    }
    
    if(!needsRegeneration)
    {
        texture.clear();
        ofLogVerbose(__func__) << "NO CHANGES NEEDED - RETURNING EARLY";
        return;
    }
    
    if (appEGLWindow == NULL)
    {
        appEGLWindow = (ofAppEGLWindow *) ofGetWindowPtr();
    }
    
    if (appEGLWindow == NULL)
    {
        ofLogError(__func__) << "appEGLWindow is NULL - RETURNING";
        return;
    }
    if (display == NULL)
    {
        display = appEGLWindow->getEglDisplay();
    }
    if (context == NULL)
    {
        context = appEGLWindow->getEglContext();
    }
    
    
    
    if (needsRegeneration)
    {
        if(doPixels)//go ahead and take the allocate hit here
        {
            if(!fbo.isAllocated() ||
               fbo.getWidth() != width ||
               fbo.getHeight() != height)
            {
                fbo.allocate(width, height, GL_RGBA);
            }
        }
           

        texture.allocate(width, height, GL_RGBA);
        texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
        textureID = texture.getTextureData().textureID;
    }
    
    glEnable(GL_TEXTURE_2D);
    
    // setup first texture
    int dataSize = width * height * 4;
    
    if (pixels && needsRegeneration)
    {
        delete[] pixels;
        pixels = NULL;
    }
    
    if (pixels == NULL)
    {
        pixels = new unsigned char[dataSize];
    }
    
    //memset(pixels, 0xff, dataSize);  // white texture, opaque
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    
    if (eglImage && needsRegeneration)
    {
        destroyEGLImage();
    }
    
    // Create EGL Image
    eglImage = eglCreateImageKHR(
                                 display,
                                 context,
                                 EGL_GL_TEXTURE_2D_KHR,
                                 (EGLClientBuffer)textureID,
                                 NULL);
    glDisable(GL_TEXTURE_2D);
    if (eglImage == EGL_NO_IMAGE_KHR)
    {
        ofLogError()	<< "Create EGLImage FAIL <---------------- :(";
    }
    else
    {
        ofLogVerbose()	<< "Create EGLImage PASS <---------------- :)";
        
    }
}


void ofxRPiCameraVideoGrabber::destroyEGLImage()
{
    
    if (eglImage)
    {
        if (eglDestroyImageKHR(display, eglImage))
        {
            ofLogVerbose(__func__) << "eglDestroyImageKHR PASS <---------------- :)";
        }
        else
        {
            ofLogError(__func__) << "eglDestroyImageKHR FAIL <---------------- :(";
        }
        eglImage = NULL;
    }
    
}

bool ofxRPiCameraVideoGrabber::isTextureEnabled()
{
    bool isEnabled = false;
    if(textureEngine)
    {
        isEnabled = true;
    }
    return isEnabled;
}
void ofxRPiCameraVideoGrabber::updatePixels()
{
    if (!doPixels && !doSaveImage) 
    {
        return;
    }
    int width = getWidth();
    int height = getHeight();
    if (!fbo.isAllocated()) 
    {
        fbo.allocate(width, height, GL_RGBA);
    }
    int dataSize = width * height * 4;
    if (pixels == NULL)
    {
        pixels = new unsigned char[dataSize];
    }
    fbo.begin();
        ofClear(0, 0, 0, 0);
        texture.draw(0, 0);
        glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);	
    fbo.end();
    
    if(doSaveImage)
    {
        stringstream fileName;
        fileName << width;
        fileName << "px";
        fileName << "_x_";
        fileName << height;
        fileName << "px";
        fileName << "_4ch_";
        fileName << ofGetTimestampString();
        
        string imagePath;
        if(doRawSave)
        {
            imagePath = ofToDataPath(fileName.str()+".raw", true);
            ofBuffer buffer((const char*)pixels, dataSize);
            ofBufferToFile(imagePath, buffer, true);
            
            
        }else
        {
            imagePath = ofToDataPath(fileName.str()+".png", true);
            ofImage image;
            image.setUseTexture(false);
            image.setFromPixels(pixels, getWidth(), getHeight(), OF_IMAGE_COLOR_ALPHA);
            image.saveImage(imagePath);
        }
        
        ofLogVerbose(__func__) << "imagePath: " << imagePath;
        doRawSave = false;
        doSaveImage = false;
    }
}


void ofxRPiCameraVideoGrabber::enablePixels()
{
    if(!textureEngine)
    {
       // ofLogWarning(__func__) << "You are in non-texture mode but asking for a pixels";
    }
    doPixels = true;
}



void ofxRPiCameraVideoGrabber::disablePixels()
{
    if(!textureEngine)
    {
        ofLogWarning(__func__) << "You are in non-texture mode";
    }
    
    doPixels = false;
}

unsigned char * ofxRPiCameraVideoGrabber::getPixels()
{
    return pixels;
}

void ofxRPiCameraVideoGrabber::stopRecording()
{
    if (engine) 
    {
        engine->stopRecording();
    }
    if (textureEngine) 
    {
        textureEngine->stopRecording();
    }
}

void ofxRPiCameraVideoGrabber::onUpdate(ofEventArgs & args)
{
    if(doStartRecording)
    {
        ofLogVerbose(__func__) << "doStartRecording REQUESTED";
        omxCameraSettings.doRecording = true;
        omxCameraSettings.doRecordingPreview = true;
        setup(omxCameraSettings);
        doStartRecording = false;
    }else
    {
        //ofLogVerbose(__func__) << doStartRecording;

        if(textureEngine)
        {
            frameCounter  = textureEngine->getFrameCounter();
            
        }else
        {
            if (engine) 
            {
                frameCounter  = engine->getFrameCounter();
            }
        }
        
        if (frameCounter > updateFrameCounter) 
        {
            updateFrameCounter = frameCounter;
            hasNewFrame = true;
            
        }else
        {
            hasNewFrame = false;
        }
        if (hasNewFrame) 
        {
            if (textureEngine) 
            {
                updatePixels();
                
            }
        }
    }
        //ofLogVerbose() << "hasNewFrame: " << hasNewFrame;
}



bool ofxRPiCameraVideoGrabber::isFrameNew()
{
    return hasNewFrame;
}

void ofxRPiCameraVideoGrabber::draw()
{
    if (!textureEngine)
    {
        return;
    }
    texture.draw(0, 0);
}



OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setExposurePreset(OMX_EXPOSURECONTROLTYPE exposureMode)
{
    exposurePresetConfig.eExposureControl = exposureMode;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonExposure, &exposurePresetConfig);
}

/*
 int
 ofxRPiCameraVideoGrabber::getMeteringEvCompensation()
 {
 OMX_CONFIG_EXPOSUREVALUETYPE exposurevalue;
 OMX_INIT_STRUCTURE(exposurevalue);
 exposurevalue.nPortIndex = OMX_ALL;
 
 error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
 if(error != OMX_ErrorNone) 
 {
 ofLog(OF_LOG_ERROR,	"getMeteringEvCompensation OMX_GetConfig OMX_IndexConfigCommonExposureValue FAIL error: 0x%08x", error);
 }
 }
 */
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMeteringMode(CameraMeteringMode cameraMeteringMode)
{
    currentMeteringMode = cameraMeteringMode;
    return applyCurrentMeteringMode();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyCurrentMeteringMode()
{
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);

    
    /*
     
     weird this is backwards from what is expected
     
     from : http://www.raspberrypi.org/forums/viewtopic.php?t=71915&p=519003
     "EV has a range of +/- 4 in steps of 1/6ths (because some people want steps of 1/3s, and others 1/2s). 
     It's MMAL that is awkward as it hasn't taken that into account and just uses an index counting in 1/6ths (ie 10 = 10/6 = +1 2/3).
     Do bear in mind that it the values will be clipped by the exposure mode and max analogue gain,
     so if you have a dark scene where the AGC algo has already turned everything up to max, 
     then selecting EV +4 isn't going to give you any further gain as there isn't any available."
     
     "For genuine EV +1, MMAL would take 6 (yuck! Really should fix that), and IL will take 65536.
     MMAL = Genuine * 6
     IL = Genuine * 65536
     
     In the GPU MMAL/RIL wrapper, we have param.xEVCompensation = (mmal_param->value << 16) / 6;
     Just watch out for rounding errors on your conversions - those 1/6ths don't always go the way you want."
     
    */
    
    currentMeteringMode.exposurevalue.eMetering         =   currentMeteringMode.meteringType; 
    currentMeteringMode.exposurevalue.xEVCompensation   =   toQ16(currentMeteringMode.evCompensation); 
    
    currentMeteringMode.exposurevalue.bAutoShutterSpeed =   toOMXBool(currentMeteringMode.autoShutter);
    currentMeteringMode.exposurevalue.nShutterSpeedMsec =   currentMeteringMode.shutterSpeedMicroSeconds;
    
    currentMeteringMode.exposurevalue.bAutoAperture     =   toOMXBool(currentMeteringMode.autoAperture);
    currentMeteringMode.exposurevalue.nApertureFNumber  =   toQ16(currentMeteringMode.aperture);
    
    currentMeteringMode.exposurevalue.bAutoSensitivity  =   toOMXBool(currentMeteringMode.autoISO);
    currentMeteringMode.exposurevalue.nSensitivity      =   currentMeteringMode.ISO;
    
    error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    
    if(error == OMX_ErrorNone)
    {
        updateCurrentMeteringMode(currentMeteringMode.exposurevalue); 
    }else
    {
        ofLogError(__func__) << "FAILED TO SET METERING MODE " ;
        printMeteringMode();
        ofLogError(__func__) << "^^^ FAILED VALUES ^^^ " ;
    }
    
    return error;
}

void ofxRPiCameraVideoGrabber::updateCurrentMeteringMode(OMX_CONFIG_EXPOSUREVALUETYPE omxExposureValue)
{
    currentMeteringMode.exposurevalue               = omxExposureValue;
    
    currentMeteringMode.meteringType                = omxExposureValue.eMetering;
    currentMeteringMode.evCompensation              = fromQ16(omxExposureValue.xEVCompensation);
    
    currentMeteringMode.autoShutter                 = fromOMXBool(omxExposureValue.bAutoShutterSpeed);
    currentMeteringMode.shutterSpeedMicroSeconds    = omxExposureValue.nShutterSpeedMsec;
    
    currentMeteringMode.autoAperture                = fromOMXBool(omxExposureValue.bAutoAperture);
    currentMeteringMode.aperture                    = fromQ16(omxExposureValue.nApertureFNumber);

    currentMeteringMode.autoISO                     = fromOMXBool(omxExposureValue.bAutoSensitivity);
    currentMeteringMode.ISO                         = omxExposureValue.nSensitivity;
    
    
}

string ofxRPiCameraVideoGrabber::meteringModetoString()
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error != OMX_ErrorNone)
    {
        ofLogError() << "COULD NOT UPDATE METERING MODE" << error;
    }
    
    stringstream ss;
    
    ss << "OMX_METERINGTYPE: "      <<  currentMeteringMode.getMeteringTypeString()  << "\n";
    ss << "xEVCompensation: Q16"    <<  currentMeteringMode.exposurevalue.xEVCompensation       << "\n";
    ss << "nApertureFNumber Q16: "  <<  currentMeteringMode.exposurevalue.nApertureFNumber      << "\n";
    ss << "bAutoAperture: "         <<  currentMeteringMode.exposurevalue.bAutoAperture         << "\n";
    ss << "nShutterSpeedMsec: "     <<  currentMeteringMode.exposurevalue.nShutterSpeedMsec     << "\n";
    ss << "bAutoShutterSpeed: "     <<  currentMeteringMode.exposurevalue.bAutoShutterSpeed     << "\n";
    ss << "nSensitivity: "          <<  currentMeteringMode.exposurevalue.nSensitivity          << "\n";
    ss << "bAutoSensitivity: "      <<  currentMeteringMode.exposurevalue.bAutoSensitivity      << "\n";
    
    ss << "\n";
    ss << currentMeteringMode.toString() << "\n";
    
    
    return ss.str();
}

void ofxRPiCameraVideoGrabber::printMeteringMode()
{
    ofLogVerbose() << meteringModetoString();
}



OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoAperture(bool doAutoAperture)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoAperture	= toOMXBool(doAutoAperture);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}



OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoISO(bool doAutoISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoSensitivity	= toOMXBool(doAutoISO);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setISO(int ISO)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.nSensitivity	= ISO;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }else
        {
            ofLogError(__func__) << omxErrorToString(error);
        }
    }
    return error;
}

int ofxRPiCameraVideoGrabber::getISO()
{
    return currentMeteringMode.ISO;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAperture(int aperture)
{
    currentMeteringMode.aperture = aperture;
    return applyCurrentMeteringMode();
}

int ofxRPiCameraVideoGrabber::getAperture()
{ 
    return currentMeteringMode.aperture;
}
                
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setShutterSpeed(int shutterSpeedMicroSeconds)
{
    
    currentMeteringMode.shutterSpeedMicroSeconds = shutterSpeedMicroSeconds;
    return applyCurrentMeteringMode();
}

int ofxRPiCameraVideoGrabber::getShutterSpeed()
{
    return currentMeteringMode.shutterSpeedMicroSeconds;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setAutoShutter(bool doAutoShutter)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
    if(error == OMX_ErrorNone) 
    {
        currentMeteringMode.exposurevalue.bAutoShutterSpeed	= toOMXBool(doAutoShutter);
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonExposureValue, &currentMeteringMode.exposurevalue);
        if(error == OMX_ErrorNone) 
        {
            updateCurrentMeteringMode(currentMeteringMode.exposurevalue);
        }
    }
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFrameStabilization(bool doStabilization)
{
    framestabilizationConfig.bStab = toOMXBool(doStabilization);
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonFrameStabilisation, &framestabilizationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSharpness(int sharpness) //-100 to 100
{
    sharpnessConfig.nSharpness = sharpness; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonSharpness, &sharpnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSharpnessNormalized(int sharpnessNormalized)
{
    return setSharpness(ofMap(sharpnessNormalized, 0, 1, -100, 100)); 
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setContrast(int contrast ) //-100 to 100 
{
    contrastConfig.nContrast = contrast; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonContrast, &contrastConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setContrastNormalized(int contrastNormalized )
{
    return setContrast(ofMap(contrastNormalized, 0, 1, -100, 100)); 
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setBrightness(int brightness ) //0 to 100
{
    
    brightnessConfig.nBrightness = brightness;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonBrightness, &brightnessConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setBrightnessNormalized(int brightnessNormalized ) //0 to 100
{
    return setBrightness(ofMap(brightnessNormalized, 0, 1, 0, 100));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSaturation(int saturation) //-100 to 100
{
    
    saturationConfig.nSaturation = saturation; 
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonSaturation, &saturationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSaturationNormalized(int saturationNormalized)
{
    
    return setSaturation(ofMap(saturationNormalized, 0, 1, -100, 100));
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setWhiteBalance(OMX_WHITEBALCONTROLTYPE controlType)
{	
    whiteBalanceConfig.eWhiteBalControl = controlType;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonWhiteBalance, &whiteBalanceConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setColorEnhancement(bool doColorEnhance, int U, int V)
{
    colorEnhancementConfig.bColorEnhancement = toOMXBool(doColorEnhance);
    colorEnhancementConfig.nCustomizedU = U;
    colorEnhancementConfig.nCustomizedV = V;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonColorEnhancement, &colorEnhancementConfig);
}

ofRectangle& ofxRPiCameraVideoGrabber::getCropRectangle()
{
    return cropRectangle;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::updateSensorCrop()
{
    return setSensorCrop(cropRectangle);
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSensorCrop(int left, int top, int width, int height)
{
    cropRectangle.set(left, top, width, height);
    return updateSensorCrop();
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSensorCrop(ofRectangle& rectangle)
{
   
    sensorCropConfig.xLeft   = ((uint32_t)rectangle.getLeft()   << 16)/100;
    sensorCropConfig.xTop    = ((uint32_t)rectangle.getTop()    << 16)/100;
    sensorCropConfig.xWidth  = ((uint32_t)rectangle.getWidth()  << 16)/100;
    sensorCropConfig.xHeight = ((uint32_t)rectangle.getHeight() << 16)/100;
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigInputCropPercentages, &sensorCropConfig);
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << omxErrorToString(error);
        if(error == OMX_ErrorBadParameter)
        {
            ofLogWarning(__func__) << "resetting cropRectangle to known good params (0, 0, 100, 100)";
            cropRectangle.set(0, 0, 100, 100);
            return updateSensorCrop(); 
        }
        
    }
    return error;

}
float ofxRPiCameraVideoGrabber::getZoomLevelNormalized()
{
    return ofMap(zoomLevel, 0, zoomLevels.size(), 0.0f, 1.0f);
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setZoomLevelNormalized(float value)
{
    if(value<0 || value>1)
    {
        ofLogError(__func__) << value << "MUST BE BETWEEN 0.0 - 1.0";
        return OMX_ErrorBadParameter;
    }
    zoomLevel = (int) ofMap(value, 0.0f, 1.0f, 0, zoomLevels.size());
    return setDigitalZoom();
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::resetZoom()
{
    zoomLevel = 0;
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::zoomIn()
{
    if((unsigned int)zoomLevel+1 < zoomLevels.size())
    {
        zoomLevel++;
    }
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::zoomOut()
{
    zoomLevel--;
    if(zoomLevel<0)
    {
        zoomLevel = 0;
    }
    
    return setDigitalZoom();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setDigitalZoom()
{
    
    if(zoomLevel<0 || (unsigned int) zoomLevel>zoomLevels.size())
    {
        
        ofLogError(__func__) << "BAD zoomLevel: " << zoomLevel << " SETTING TO 0";
        zoomLevel = 0;
    }
   
    
    int value = zoomLevels[zoomLevel];
    if(digitalZoomConfig.xWidth != value && digitalZoomConfig.xHeight != value)
    {
        cout << "zoomLevel: " << zoomLevel << endl;
        digitalZoomConfig.xWidth  = value;
        digitalZoomConfig.xHeight = value;
        
        OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonDigitalZoom, &digitalZoomConfig);
        if(error != OMX_ErrorNone)
        {
            ofLogError(__func__) << omxErrorToString(error);
        }
        return error;
    }
    return OMX_ErrorNone;
    
}

bool ofxRPiCameraVideoGrabber::toggleLED()
{
    return setLEDState(!LED_CURRENT_STATE);
}

bool ofxRPiCameraVideoGrabber::setLEDState(bool state)
{
   
    /*
     https://github.com/raspberrypi/firmware/issues/186
     
     OMX_CONFIG_PRIVACYINDICATORTYPE ledConfig;
     OMX_INIT_STRUCTURE(ledConfig);
     
    if(turnLEDOn)
    {
        ledConfig.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOn;
    }
    else
    {
        ledConfig.ePrivacyIndicatorMode = OMX_PrivacyIndicatorOff;
    }
     
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigPrivacyIndicator, &ledConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << " PASS";
    }else{
        ofLogError(__func__) << "FAIL " << omxErrorToString(error);
    }
    LED_CURRENT_STATE = turnLEDOn;
    
    return error;
    */
     
    LED_CURRENT_STATE = state;	
    string command = "gpio -g write 5 " + ofToString(LED_CURRENT_STATE);
    if(system(command.c_str()) == 0)
    {
        //silence compiler warning
    }; 
    
    return LED_CURRENT_STATE;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setMirror(MIRROR mirrorType)
{
    mirrorConfig.eMirror = (OMX_MIRRORTYPE) mirrorType;
    return applyMirror();
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyMirror()
{
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigCommonMirror, &mirrorConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << " PASS";
    }else{
        ofLogError(__func__) << "FAIL " << omxErrorToString(error);
    }
    return error;
}



OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setRotation(int value)
{
    return setRotation((ROTATION) value);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setRotation(ROTATION value)
{
    switch (value)
    {
        case ROTATION_0: {rotationConfig.nRotation=0;} break;
        case ROTATION_90: {rotationConfig.nRotation=90;} break;
        case ROTATION_180: {rotationConfig.nRotation=180;} break;
        case ROTATION_270: {rotationConfig.nRotation=270;} break;
        default: {rotationConfig.nRotation=0;} break;
    }
    return applyRotation();
}
int ofxRPiCameraVideoGrabber::getRotation()
{
    return rotationConfig.nRotation;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyRotation()
{
    return OMX_SetConfig(camera, OMX_IndexConfigCommonRotate, &rotationConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::rotateClockwise()
{
    int currentRotation  = getRotation();
    if(currentRotation+90<360)
    {
        rotationConfig.nRotation+=90;
    }else{
        rotationConfig.nRotation = 0;
    }

    return applyRotation();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::rotateCounterClockwise()
{
    int currentRotation  = getRotation();
    if(currentRotation-90>=0)
    {
        rotationConfig.nRotation-=90;
    }else{
        rotationConfig.nRotation = 270;
    }
    
    return applyRotation();
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::toggleImageEffects(bool doDisable)
{
    cameraDisableAlgorithmConfig.eAlgorithm = OMX_CameraDisableAlgorithmImageEffects;
    
    cameraDisableAlgorithmConfig.bDisabled = toOMXBool(doDisable);
    
    return OMX_SetConfig(camera, OMX_IndexParamCameraDisableAlgorithm, &cameraDisableAlgorithmConfig);	
}

void ofxRPiCameraVideoGrabber::enableImageEffects()
{
    toggleImageEffects(true);
}

void ofxRPiCameraVideoGrabber::disableImageEffects()
{
    toggleImageEffects(false);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyImageFilter(OMX_IMAGEFILTERTYPE imageFilter)
{
    
    imagefilterConfig.eImageFilter = imageFilter;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilter, &imagefilterConfig);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setDRC(int level)
{
    OMX_DYNAMICRANGEEXPANSIONMODETYPE type = OMX_DynRangeExpOff;
    switch (level) 
    {
        case 0:
        {
            type = OMX_DynRangeExpOff;
            break;
        }
        case 1:
        {
            type = OMX_DynRangeExpLow;
            break;
        }
        case 2:
        {
            type = OMX_DynRangeExpMedium;
            break;
        }
        case 3:
        {
            type = OMX_DynRangeExpHigh;
            break;
        }
        default:
        {
            type = OMX_DynRangeExpOff;
            break;
        }
        
    }
    
    drcConfig.eMode = type;
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigDynamicRangeExpansion, &drcConfig);
    if(error != OMX_ErrorNone)
    {
        ofLogError(__func__) << "FAIL " << level;
    }else{
    
        //ofLogVerbose(__func__) << "SUCCESS " << level;
    }
    
    return error;
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setHDR(bool doHDR)
{
    hdrConfig.bEnabled = toOMXBool(doHDR);
    
    OMX_ERRORTYPE error = OMX_SetConfig(camera, OMX_IndexConfigBurstCapture, &hdrConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << " PASS";
    }else
    {
        ofLogError(__func__) << "FAIL " << omxErrorToString(error);
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableBurstMode()
{
    burstModeConfig.bEnabled = OMX_TRUE;
    return OMX_SetConfig(camera, OMX_IndexConfigBrcmHighDynamicRange, &burstModeConfig);
}


OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSoftwareSharpening(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
    
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << "OMX_GetConfig PASS";
        disableSoftwareSharpenConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSharpenDisable, &disableSoftwareSharpenConfig);
        if(error == OMX_ErrorNone) 
        {
            ofLogVerbose(__func__) << "OMX_SetConfig PASS";
        }else
        {
            ofLogError(__func__) << "OMX_SetConfig FAIL " << omxErrorToString(error);
        }
        
    }else
    {
        ofLogError(__func__) << "OMX_GetConfig FAIL " << omxErrorToString(error);
    }
    return error;
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableSoftwareSharpening()
{
    return setSoftwareSharpening(false);
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::disableSoftwareSharpening()
{
    
   return setSoftwareSharpening(true);
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setSoftwareSaturation(bool state)
{
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
    
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << "OMX_GetConfig PASS";
        disableSoftwareSaturationConfig.bEnabled = toOMXBool(state);
        
        error = OMX_SetConfig(camera, OMX_IndexParamSWSaturationDisable, &disableSoftwareSaturationConfig);
        if(error == OMX_ErrorNone) 
        {
            ofLogVerbose(__func__) << "OMX_SetConfig PASS";
        }else
        {
            ofLogError(__func__) << "OMX_SetConfig FAIL " << omxErrorToString(error);
        }
        
    }else
    {
        ofLogError(__func__) << "OMX_GetConfig FAIL " << omxErrorToString(error);
    }
    return error;
}

OMX_ERRORTYPE ofxRPiCameraVideoGrabber::enableSoftwareSaturation()
{
    return setSoftwareSaturation(false);
}
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::disableSoftwareSaturation()
{
    
    return setSoftwareSaturation(true);
}





OMX_ERRORTYPE ofxRPiCameraVideoGrabber::setFlickerCancellation(OMX_COMMONFLICKERCANCELTYPE eFlickerCancel)
{
    
    
    
    
    OMX_ERRORTYPE error = OMX_GetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
    if(error == OMX_ErrorNone) 
    {
        ofLogVerbose(__func__) << "camera OMX_GetConfig OMX_IndexConfigCommonFlickerCancellation PASS ";
        switch (flickerCancelConfig.eFlickerCancel) 
        {
            case OMX_COMMONFLICKERCANCEL_OFF:
            {
                ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_OFF";
                break;
            }
            case OMX_COMMONFLICKERCANCEL_AUTO:
            {
                ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_AUTO";
                break;
            }
            case OMX_COMMONFLICKERCANCEL_50:
            {
                ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_50";
                break;
            }
            case OMX_COMMONFLICKERCANCEL_60:
            {
                ofLogVerbose(__func__) << "OMX_COMMONFLICKERCANCEL_60";
                break;
            }
            default:
                break;
        }
        flickerCancelConfig.eFlickerCancel = eFlickerCancel;
        error = OMX_SetConfig(camera, OMX_IndexConfigCommonFlickerCancellation, &flickerCancelConfig);
    }
    
    return error;
}


#if 0
OMX_ERRORTYPE ofxRPiCameraVideoGrabber::applyImageFilterWithParams(OMX_CONFIG_IMAGEFILTERPARAMSTYPE imageFilter)
{
    OMX_INIT_STRUCTURE(imageFilter);
    imageFilter.nPortIndex = OMX_ALL;
    imageFilter.nNumParams = 1;
    imageFilter.eImageFilter = OMX_ImageFilterBlur;
    
    return OMX_SetConfig(camera, OMX_IndexConfigCommonImageFilterParameters, &imageFilter);
}


OMX_CONFIG_IMAGEFILTERPARAMSTYPE image_filter;
OMX_INIT_STRUCTURE(image_filter);

image_filter.nPortIndex = m_omx_image_fx.GetOutputPort();
if (m_anaglyph != OMX_ImageFilterAnaglyphNone)
{
    image_filter.nNumParams = 1;
    image_filter.nParams[0] = m_anaglyph;
    image_filter.eImageFilter = OMX_ImageFilterAnaglyph;
}
else
{
    image_filter.nNumParams = 1;
    image_filter.nParams[0] = 3;
    if (!advanced_deinterlace)
        image_filter.eImageFilter = OMX_ImageFilterDeInterlaceFast;
        else
            image_filter.eImageFilter = OMX_ImageFilterDeInterlaceAdvanced;
            }
omx_err = m_omx_image_fx.SetConfig(OMX_IndexConfigCommonImageFilterParameters, &image_filter);
if(omx_err != OMX_ErrorNone)
{
    CLog::Log(LOGERROR, "%s::%s - OMX_IndexConfigCommonImageFilterParameters omx_err(0x%08x)", CLASSNAME, __func__, omx_err);
    return false;
}

#endif

bool doExit = false;
void signal_handler(int signum)
{
    cout << "ofxRPiCameraVideoGrabber caught signal " << signum;
    doExit = true;
}

void ofxRPiCameraVideoGrabber::onUpdateDuringExit(ofEventArgs& args)
{
    if (doExit)
    {
        ofLogVerbose(__func__) << " EXITING VIA SIGNAL";
        close();
        ofExit();
    }
}

void ofxRPiCameraVideoGrabber::addExitHandler()
{
    if(hasExitHandler) return;
    
    vector<int> signals;
    signals.push_back(SIGINT);
    signals.push_back(SIGQUIT);
    
    for (size_t i=0; i<signals.size(); i++)
    {
        int SIGNAL_TO_BLOCK = signals[i];
        //http://stackoverflow.com/questions/11465148/using-sigaction-c-cpp
        
        //Struct for the new action associated to the SIGNAL_TO_BLOCK
        struct sigaction new_action;
        new_action.sa_handler = signal_handler;
        
        //Empty the sa_mask. This means that no signal is blocked while the signal_handler runs.
        sigemptyset(&new_action.sa_mask);
        
        //Block the SEGTERM signal so while the signal_handler runs, the SIGTERM signal is ignored
        sigaddset(&new_action.sa_mask, SIGTERM);
        
        //Remove any flag from sa_flag. See documentation for flags allowed
        new_action.sa_flags = 0;
        
        struct sigaction old_action;
        //Read the old signal associated to SIGNAL_TO_BLOCK
        sigaction(SIGNAL_TO_BLOCK, NULL, &old_action);
        
        //If the old handler wasn't SIG_IGN it is a handler that just "ignores" the signal
        if (old_action.sa_handler != SIG_IGN)
        {
            //Replace the signal handler of SIGNAL_TO_BLOCK with the one described by new_action
            sigaction(SIGNAL_TO_BLOCK, &new_action, NULL);
        }
        
    }
    
    ofAddListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdateDuringExit);
    hasExitHandler = true;
}

ofxRPiCameraVideoGrabber::~ofxRPiCameraVideoGrabber()
{
    close();
    if (pixels) 
    {
        delete[] pixels;
        pixels = NULL;
    }

}

void ofxRPiCameraVideoGrabber::close()
{
    ofRemoveListener(ofEvents().update, this, &ofxRPiCameraVideoGrabber::onUpdate);
    if(engine)
    {
        delete engine;
        engine = NULL;
    }
    if(textureEngine)
    {
        delete textureEngine;
        textureEngine = NULL;
        destroyEGLImage();
    }
}




