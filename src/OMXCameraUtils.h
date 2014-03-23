#pragma once

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

#define VIDEO_RENDER_INPUT_PORT	90

#define EGL_RENDER_INPUT_PORT	220
#define EGL_RENDER_OUTPUT_PORT	221

#define CAMERA_PREVIEW_PORT		70
#define CAMERA_OUTPUT_PORT		71

#define VIDEO_ENCODE_INPUT_PORT 200
#define VIDEO_ENCODE_OUTPUT_PORT 201

#define NULL_SINK_INPUT_PORT 240


#define VIDEO_SPLITTER_INPUT_PORT 250

#define VIDEO_SPLITTER_OUTPUT_PORT1 251
#define VIDEO_SPLITTER_OUTPUT_PORT2 252
#define VIDEO_SPLITTER_OUTPUT_PORT3 253
#define VIDEO_SPLITTER_OUTPUT_PORT4 254

class OMXCameraUtils
{
public:
	OMXCameraUtils();
	static OMX_ERRORTYPE disableAllPortsForComponent(OMX_HANDLETYPE* m_handle);

};