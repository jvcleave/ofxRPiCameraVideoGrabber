/*
 *  ImageFilterCollection.h
 *  Created by jason van cleave on 8/7/13.
 */

#pragma once

#include "ofMain.h"

#include <IL/OMX_Core.h>
#include <IL/OMX_Component.h>
#include <IL/OMX_Index.h>
#include <IL/OMX_Image.h>
#include <IL/OMX_Video.h>
#include <IL/OMX_Broadcom.h>

struct ImageFilter
{
	OMX_IMAGEFILTERTYPE type;
	string name;
};

class ImageFilterCollection
{
public:
	ImageFilterCollection()
	{
		filterList = "FILTERS:\n";
		createImageFilter("None", OMX_ImageFilterNone);
		createImageFilter("Noise", OMX_ImageFilterNoise);
		createImageFilter("Emboss", OMX_ImageFilterEmboss);
		createImageFilter("Negative", OMX_ImageFilterNegative);
		createImageFilter("Sketch", OMX_ImageFilterSketch);
		createImageFilter("OilPaint", OMX_ImageFilterOilPaint);
		createImageFilter("Hatch", OMX_ImageFilterHatch);
		createImageFilter("Gpen", OMX_ImageFilterGpen);
		createImageFilter("Antialias", OMX_ImageFilterAntialias); 
		createImageFilter("DeRing", OMX_ImageFilterDeRing);       
		createImageFilter("Solarize", OMX_ImageFilterSolarize);
		createImageFilter("Watercolor", OMX_ImageFilterWatercolor);
		createImageFilter("Pastel", OMX_ImageFilterPastel);
		createImageFilter("Sharpen", OMX_ImageFilterSharpen);
		createImageFilter("Film", OMX_ImageFilterFilm);
		createImageFilter("Blur", OMX_ImageFilterBlur);
		createImageFilter("Saturation", OMX_ImageFilterSaturation);
		createImageFilter("DeInterlaceLineDouble", OMX_ImageFilterDeInterlaceLineDouble);
		createImageFilter("DeInterlaceAdvanced", OMX_ImageFilterDeInterlaceAdvanced);
		createImageFilter("ColourSwap", OMX_ImageFilterColourSwap);
		createImageFilter("WashedOut", OMX_ImageFilterWashedOut);
		createImageFilter("ColourPoint", OMX_ImageFilterColourPoint);
		createImageFilter("Posterise", OMX_ImageFilterPosterise);
		createImageFilter("ColourBalance", OMX_ImageFilterColourBalance);
		createImageFilter("Cartoon", OMX_ImageFilterCartoon);
		setCurrentFilter(0);
	}
	
	void createImageFilter(string name, OMX_IMAGEFILTERTYPE filterType)
	{
		ImageFilter filter;
		filter.name = name;
		filter.type = filterType;
		imageFilters.push_back(filter);
		filterList += ofToString(imageFilters.size()) + " " + name + "\n";
	}
	
	ImageFilter getRandomFilter()
	{
		return setCurrentFilter(ofRandom(imageFilters.size()));
	}
	
	ImageFilter setCurrentFilter(int index)
	{
		currentFilterIndex = index;
		currentFilter = imageFilters[currentFilterIndex];
		return currentFilter;
	}
	
	ImageFilter getNextFilter()
	{
		if (currentFilterIndex+1 < imageFilters.size()) 
		{
			return setCurrentFilter(currentFilterIndex+1);
		}else 
		{
			return setCurrentFilter(0);
		}
	}
	
	ImageFilter getFilterByName(string name)
	{
		bool didMatch = false;
		int matchingIndex = currentFilterIndex;
		
		for (unsigned int i =0; i<imageFilters.size(); i++) 
		{
			if (imageFilters[i].name == name) 
			{
				matchingIndex = i;
				didMatch = true;
				break;
			}
		}
		if (!didMatch) 
		{
			ofLogError(__func__) << "NO MATCH FOR FILTER NAME: " << name;
			return currentFilter;
		}else
		{
			return setCurrentFilter(matchingIndex);
		}
		
	}
	
	unsigned int currentFilterIndex;
	ImageFilter currentFilter;
	vector<ImageFilter> imageFilters;
	string filterList;
	
};