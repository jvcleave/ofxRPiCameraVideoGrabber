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
		
	}
	
	void setup(OMX_Maps* omxMaps, string initialFilterName = "None")
	{
		this->omxMaps = omxMaps;
		
		map<string, OMX_IMAGEFILTERTYPE>::iterator imageFilterIterator = omxMaps->imageFilterTypes.begin();
		
		size_t counter =0;
		while (imageFilterIterator != omxMaps->imageFilterTypes.end()) 
		{
			if ((*imageFilterIterator).first == initialFilterName) 
			{
				currentFilterName = initialFilterName; 
				currentFilter = omxMaps->imageFilterTypes[currentFilterName];
				currentFilterIndex = counter;
				break;
			}else
			{
				++imageFilterIterator;
				++counter;
			}
		}

	}
	
	
	OMX_IMAGEFILTERTYPE setCurrentFilter(size_t index)
	{
		currentFilterIndex = index;
		size_t counter =0;
		
		map<string, OMX_IMAGEFILTERTYPE>::iterator imageFilterIterator = omxMaps->imageFilterTypes.begin();
		while (imageFilterIterator != omxMaps->imageFilterTypes.end()) 
		{
			if(counter == currentFilterIndex)
			{
				currentFilter = (*imageFilterIterator).second;
				currentFilterName = (*imageFilterIterator).first;
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
		return currentFilterName;
	}
	
	OMX_IMAGEFILTERTYPE currentFilter;
	string currentFilterName;
	size_t currentFilterIndex;
	OMX_Maps* omxMaps;
};