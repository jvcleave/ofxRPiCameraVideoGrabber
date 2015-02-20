#ifndef OMXCameraUtils_
#define OMXCameraUtils_

#include "ofMain.h"
#include "ofAppEGLWindow.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

#include "OMX_Maps.h"



#define OMX_INIT_STRUCTURE(a) \
memset(&(a), 0, sizeof(a)); \
(a).nSize = sizeof(a); \
(a).nVersion.s.nVersionMajor = OMX_VERSION_MAJOR; \
(a).nVersion.s.nVersionMinor = OMX_VERSION_MINOR; \
(a).nVersion.s.nRevision = OMX_VERSION_REVISION; \
(a).nVersion.s.nStep = OMX_VERSION_STEP



#define OMX_CAMERA (OMX_STRING)"OMX.broadcom.camera"
#define CAMERA_PREVIEW_PORT		70
#define CAMERA_OUTPUT_PORT		71

#define OMX_VIDEO_ENCODER (OMX_STRING)"OMX.broadcom.video_encode"
#define ENCODER_INPUT_PORT 200
#define ENCODER_OUTPUT_PORT 201

#define OMX_VIDEO_DECODER (OMX_STRING)"OMX.broadcom.video_decode"
#define VIDEO_DECODE_INPUT_PORT 130
#define VIDEO_DECODE_OUTPUT_PORT 131


#define OMX_VIDEO_SPLITTER (OMX_STRING)"OMX.broadcom.video_splitter"
#define VIDEO_SPLITTER_INPUT_PORT 250

#define VIDEO_SPLITTER_OUTPUT_PORT1 251
#define VIDEO_SPLITTER_OUTPUT_PORT2 252
#define VIDEO_SPLITTER_OUTPUT_PORT3 253
#define VIDEO_SPLITTER_OUTPUT_PORT4 254

#define OMX_VIDEO_RENDER (OMX_STRING)"OMX.broadcom.video_render"
#define VIDEO_RENDER_INPUT_PORT	90

#define OMX_EGL_RENDER (OMX_STRING)"OMX.broadcom.egl_render"
#define EGL_RENDER_INPUT_PORT	220
#define EGL_RENDER_OUTPUT_PORT	221

#define OMX_NULL_SINK (OMX_STRING)"OMX.broadcom.null_sink"
#define NULL_SINK_INPUT_PORT 240


#define __func__ __PRETTY_FUNCTION__

//#define OMX_TRACE(error) ofLogVerbose(__func__) << __LINE__ << " " << omxErrorToString(error);



extern inline  
string getStateString(OMX_STATETYPE state)
{
    return OMX_Maps::getInstance().omxStateNames[state];
}

extern inline  
string omxErrorToString(OMX_ERRORTYPE error)
{
    return OMX_Maps::getInstance().omxErrors[error];
}

#define OMX_LOG_LEVEL 3

extern inline  
void logOMXError(OMX_ERRORTYPE error, string comments="", string functionName="", int lineNumber=0)
{
    string commentLine = " ";
    if(!comments.empty())
    {
        commentLine = " " + comments + " ";
    }
    
    switch(OMX_LOG_LEVEL)
    {
        case 1:
        {
            if(error != OMX_ErrorNone)
            {
                ofLogError(functionName) << lineNumber << commentLine << omxErrorToString(error);
            }else
            {
                ofLogVerbose(functionName) << lineNumber << commentLine << omxErrorToString(error);
            }
            break;
        }
        case 2:
        {
            if(error != OMX_ErrorNone)
            {
                ofLogError(functionName) << lineNumber << commentLine << omxErrorToString(error);
            }
            break;
        }
        case 3:
        {
            ofLogError(functionName)  << commentLine << omxErrorToString(error);
            break;
        }
        default:
        {
            break;
        }
    }
    
}


#define OMX_TRACE_1_ARGS(error)                      logOMXError(error, "", __func__, __LINE__);
#define OMX_TRACE_2_ARGS(error, comments)            logOMXError(error, comments, __func__, __LINE__);
#define OMX_TRACE_3_ARGS(error, comments, whatever)  logOMXError(error, comments, __func__, __LINE__);

#define GET_OMX_TRACE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define OMX_TRACE_MACRO_CHOOSER(...) GET_OMX_TRACE_4TH_ARG(__VA_ARGS__, OMX_TRACE_3_ARGS, OMX_TRACE_2_ARGS, OMX_TRACE_1_ARGS, )

#define OMX_TRACE(...) OMX_TRACE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

//#define OMX_TRACE(error)

extern inline 
const char* omxErrorToCString(OMX_ERRORTYPE error)
{
    return OMX_Maps::getInstance().omxErrors[error].c_str();
}

extern inline 
OMX_BOOL toOMXBool(bool boolean)
{
    if(boolean) { return OMX_TRUE; } else { return OMX_FALSE; }
}

extern inline  
bool fromOMXBool(OMX_BOOL omxBool)
{
    if(omxBool == OMX_TRUE) { return true; } else { return false; } 
}

extern inline 
float toQ16(float n) 
{
    return n* 65536; 
}

extern inline 
float fromQ16(float n) 
{ 
    return n*(1/65536.0); 
}

extern inline
OMX_ERRORTYPE DisableAllPortsForComponent(OMX_HANDLETYPE* handle, string componentName="")
{
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
    
    OMX_INDEXTYPE indexTypes[] = 
    {
        OMX_IndexParamAudioInit,
        OMX_IndexParamImageInit,
        OMX_IndexParamVideoInit, 
        OMX_IndexParamOtherInit
    };
    
    OMX_PORT_PARAM_TYPE ports;
    OMX_INIT_STRUCTURE(ports);
    
    for(int i=0; i < 4; i++)
    {
        error = OMX_GetParameter(*handle, indexTypes[i], &ports);
        if(error == OMX_ErrorNone) 
        {
            
            uint32_t j;
            for(j=0; j<ports.nPorts; j++)
            {
                
                
                OMX_PARAM_PORTDEFINITIONTYPE portFormat;
                OMX_INIT_STRUCTURE(portFormat);
                portFormat.nPortIndex = ports.nStartPortNumber+j;
                
                error = OMX_GetParameter(*handle, OMX_IndexParamPortDefinition, &portFormat);
                if(error != OMX_ErrorNone)
                {
                    if(portFormat.bEnabled == OMX_FALSE)
                    {
                        continue;
                    }
                }
                
                /*
                 
                 OMX_CONFIG_BRCMUSEPROPRIETARYCALLBACKTYPE cbType;
                 OMX_INIT_STRUCTURE(cbType);
                 cbType.nPortIndex = ports.nStartPortNumber+j;
                 error = OMX_GetParameter(*handle, OMX_IndexConfigBrcmUseProprietaryCallback, &cbType);
                 ofLogVerbose()<<  componentName << " PORT # " << ports.nStartPortNumber+j << " PROPRIETARY CALLBACK ENABLED: " << fromOMXBool(cbType.bEnable);
                 
                 */
                
                error = OMX_SendCommand(*handle, OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL);
                if(error != OMX_ErrorNone)
                {
                    ofLogError(__func__) << omxErrorToString(error);
                }else
                {
                    ofLogVerbose() << componentName << " PORT # " << ports.nStartPortNumber+j << " DISABLED";
                }
                
                
                
            }
            
        }
    }
    
    return error;
}

#endif
