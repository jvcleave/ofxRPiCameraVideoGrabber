/*
 *  OMXCameraUtils.cpp
 *  openFrameworksRPi
 *
 *  Created by jason van cleave on 1/7/14.
 *  Copyright 2014 jasonvancleave.com. All rights reserved.
 *
 */

#include "OMXCameraUtils.h"

string omxErrorToString(OMX_ERRORTYPE error)
{
    return OMX_Maps::getInstance().omxErrors[error];
}

const char* omxErrorToCString(OMX_ERRORTYPE error)
{
    return OMX_Maps::getInstance().omxErrors[error].c_str();
}

OMX_BOOL toOMXBool(bool boolean)
{
    if(boolean) { return OMX_TRUE; } else { return OMX_FALSE; }
}

bool fromOMXBool(OMX_BOOL omxBool)
{
    if(omxBool == OMX_TRUE) { return true; } else { return false; } 
}

float toQ16(float n) 
{
    return n* 65536; 
}

float fromQ16(float n) 
{ 
    return n*(1/65536.0); 
}


OMX_ERRORTYPE disableAllPortsForComponent(OMX_HANDLETYPE* handle)
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
				
				error = OMX_SendCommand(*handle, OMX_CommandPortDisable, ports.nStartPortNumber+j, NULL);
				if(error != OMX_ErrorNone)
				{
					ofLog(OF_LOG_VERBOSE, "disableAllPortsForComponent - Error disable port %d on component %s error: 0x%08x", 
						  (int)(ports.nStartPortNumber) + j, "m_componentName", (int)error);
				}
			}
			
		}
	}
	
	return OMX_ErrorNone;
}
