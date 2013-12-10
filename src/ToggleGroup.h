#pragma once

#include "ofMain.h"

class ToggleGroupListenerEventData
{
public:
	ToggleGroupListenerEventData(string selectedName_)
	{
		selectedName = selectedName_;
	}
	string selectedName;
};


class ToggleGroup
{
public:
	ToggleGroup()
	{
		labels = NULL;
		parameterGroup = NULL;
	}
	
	
	/*
		The group has changed so we avoid a recurvsive loop by removing the
		change listener
		
		We find all other items and set them to false
	 
		We then re-add the change listener
	*/
	void onGroupChanged(ofAbstractParameter & param)
	{
		
		ofRemoveListener(parameterGroup->parameterChangedE, this, &ToggleGroup::onGroupChanged);

		for (int i=0; i<labels->size(); i++) 
		{
			if(parameterGroup->getName(i) != param.getName())
			{
				parameterGroup->getBool(i) = false;

			}else 
			{
				
				ToggleGroupListenerEventData eventData(parameterGroup->getName(i));
				ofNotifyEvent(changeDispatcher, eventData);				
			}
			
		}
		ofAddListener(parameterGroup->parameterChangedE, this, &ToggleGroup::onGroupChanged);
	}
	
	
	/*
	 Given an empty ofParameterGroup and labels, we name it
	 and fill it with boolean items with each label
	 
	 We then listen for when this group is changed
	*/
	void setup(ofParameterGroup* parameterGroup, string groupName, vector<string>* labels)
	{
		size_t i=0;
		this->parameterGroup = parameterGroup;
		this->labels = labels;
		parameterGroup->setName(groupName);
		for (i=0; i<labels->size(); i++)
		{
			ofParameter<bool> item;
			item.set(labels->at(i), false);
			parameterGroup->add(item);
		}
		ofAddListener(parameterGroup->parameterChangedE, this, &ToggleGroup::onGroupChanged);	
	}
	
	
	//Kept this as a seperate method in case the controller didn't want to know for some reason
	
	template<class ListenerClass, typename ListenerMethod>
	void addListener(ListenerClass * listener, ListenerMethod method)
	{
		ofAddListener(changeDispatcher, listener, method);
	}
	
	ofParameterGroup* parameterGroup;
	vector<string>* labels;
	ofEvent<ToggleGroupListenerEventData> changeDispatcher;
};
