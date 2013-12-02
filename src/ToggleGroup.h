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
	
	template<class ListenerClass, typename ListenerMethod>
	void addListener(ListenerClass * listener, ListenerMethod method){
		ofAddListener(changeDispatcher, listener, method);
	}
	ofParameterGroup* parameterGroup;
	vector<string>* labels;
	ofEvent<ToggleGroupListenerEventData> changeDispatcher;
};
