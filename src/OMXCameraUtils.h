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



#define MEGABYTE_IN_BITS 8388608


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
#define CAMERA_STILL_OUTPUT_PORT 72

#define OMX_IMAGE_ENCODER (OMX_STRING)"OMX.broadcom.image_encode"
#define IMAGE_ENCODER_INPUT_PORT 340
#define IMAGE_ENCODER_OUTPUT_PORT 341

#define OMX_VIDEO_ENCODER (OMX_STRING)"OMX.broadcom.video_encode"
#define VIDEO_ENCODER_INPUT_PORT 200
#define VIDEO_ENCODER_OUTPUT_PORT 201

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

//really a guess - higher values didn't seem to make any difference

#define MAX_SHUTTER_SPEED_MICROSECONDS 51200 
#define __func__ __PRETTY_FUNCTION__
#define FORCEDINLINE __attribute__((always_inline))

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

#define ENABLE_OMX_TRACE

#define OMX_LOG_LEVEL_DEV 1
#define OMX_LOG_LEVEL_ERROR_ONLY 2
#define OMX_LOG_LEVEL_VERBOSE 3
#define OMX_LOG_LEVEL_SILENT 9

#ifndef OMX_LOG_LEVEL
#define OMX_LOG_LEVEL OMX_LOG_LEVEL_ERROR_ONLY
#endif

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
        case OMX_LOG_LEVEL_DEV:
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

extern inline  
string eglErrorToString(EGLint error)
{
    return OMX_Maps::getInstance().eglErrors[error];
}

extern inline  
void logEGLError(EGLint error, string comments="", string functionName="", int lineNumber=0)
{
    string commentLine = " ";
    if(!comments.empty())
    {
        commentLine = " " + comments + " ";
    }
    
    
    /*if(error != EGL_SUCCESS)
    {
        ofLogError(functionName) << lineNumber << commentLine << eglErrorToString(error);
    }else
    {
        ofLogVerbose(functionName) << lineNumber << commentLine << eglErrorToString(error);
    }*/
    ofLogVerbose(functionName) << lineNumber << commentLine << eglErrorToString(error);

}




#define OMX_TRACE_1_ARGS(error)                      logOMXError(error, "", __func__, __LINE__);
#define OMX_TRACE_2_ARGS(error, comments)            logOMXError(error, comments, __func__, __LINE__);
#define OMX_TRACE_3_ARGS(error, comments, whatever)  logOMXError(error, comments, __func__, __LINE__);

#define GET_OMX_TRACE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define OMX_TRACE_MACRO_CHOOSER(...) GET_OMX_TRACE_4TH_ARG(__VA_ARGS__, OMX_TRACE_3_ARGS, OMX_TRACE_2_ARGS, OMX_TRACE_1_ARGS, )

#if defined (ENABLE_OMX_TRACE)
    //#warning enabling OMX_TRACE
    #define OMX_TRACE(...) OMX_TRACE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#else
    #warning  disabling OMX_TRACE
    #warning  disabling -Wunused-but-set-variable -Wunused-variable
    #define OMX_TRACE(...)
    #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif


#define EGL_TRACE_1_ARGS(error)                      logEGLError(error, "", __func__, __LINE__);
#define EGL_TRACE_2_ARGS(error, comments)            logEGLError(error, comments, __func__, __LINE__);
#define EGL_TRACE_3_ARGS(error, comments, whatever)  logEGLError(error, comments, __func__, __LINE__);

#define GET_EGL_TRACE_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define EGL_TRACE_MACRO_CHOOSER(...) GET_EGL_TRACE_4TH_ARG(__VA_ARGS__, EGL_TRACE_3_ARGS, EGL_TRACE_2_ARGS, EGL_TRACE_1_ARGS, )


#define ENABLE_EGL_TRACE

#if defined (ENABLE_EGL_TRACE)
//#warning enabling EGL_TRACE
#define EGL_TRACE(...) EGL_TRACE_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#else
#warning  disabling EGL_TRACE
#warning  disabling -Wunused-but-set-variable -Wunused-variable
#define EGL_TRACE(...)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

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
int toQ16(float n) 
{
    return (int)(n* 65536); 
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
        OMX_TRACE(error);
        if(error == OMX_ErrorNone) 
        {
            
            uint32_t j;
            for(j=0; j<ports.nPorts; j++)
            {
                
                
                OMX_PARAM_PORTDEFINITIONTYPE portFormat;
                OMX_INIT_STRUCTURE(portFormat);
                portFormat.nPortIndex = ports.nStartPortNumber+j;
                
                error = OMX_GetParameter(*handle, OMX_IndexParamPortDefinition, &portFormat);
                OMX_TRACE(error);
                if(error != OMX_ErrorNone)
                {
                    if(portFormat.bEnabled == OMX_FALSE)
                    {
                        continue;
                    }
                }
                
                error = OMX_SendCommand(*handle, OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL);
                OMX_TRACE(error, componentName + " PORT # " + ofToString(ports.nStartPortNumber+j) + " DISABLED")
                
                
            }
            
        }
    }
    
    return error;
}




#endif
