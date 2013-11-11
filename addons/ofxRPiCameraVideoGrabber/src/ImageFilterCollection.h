/*
 *  ImageFilterCollection.h
 *  Created by jason van cleave on 8/7/13.
 */

#pragma once

#include "ofMain.h"

#include "OMX_Maps.h"




class ImageFilterCollection
{
	
public:
	ImageFilterCollection()
	{
		
		currentFilter = OMX_ImageFilterNone;
		
	}
	void setup(OMX_Maps* omxMaps, int initialIndex=0)
	{
		this->omxMaps = omxMaps;
		setCurrentFilter(initialIndex);
	}
	
	
	OMX_IMAGEFILTERTYPE setCurrentFilter(size_t index)
	{
		currentFilterIndex = index;
		size_t counter =0;
		
		map<OMX_IMAGEFILTERTYPE, string>::iterator imageFilterIterator = omxMaps->imageFilterTypes.begin();
		while (imageFilterIterator != omxMaps->imageFilterTypes.end()) 
		{
			if(counter == currentFilterIndex)
			{
				currentFilter = (*imageFilterIterator).first;
				break;
			}else 
			{
				++imageFilterIterator;
				++counter;
			}
		}
		return currentFilter;
	}
	
	OMX_IMAGEFILTERTYPE getNextFilter()
	{
		if (currentFilterIndex+1 < omxMaps->imageFilterTypes.size()) 
		{
			return setCurrentFilter(currentFilterIndex+1);
		}else 
		{
			return setCurrentFilter(0);
		}
		
	}
	string getCurrentFilterName()
	{
		return omxMaps->imageFilterTypes[currentFilter];
	}
	OMX_IMAGEFILTERTYPE currentFilter;
	size_t currentFilterIndex;
	OMX_Maps* omxMaps;
};