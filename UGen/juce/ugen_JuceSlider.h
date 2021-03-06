// $Id: ugen_JuceSlider.h 376 2013-11-20 12:06:37Z 0x4d52 $
// $HeadURL: http://ugen.googlecode.com/svn/trunk/UGen/juce/ugen_JuceSlider.h $

/*
 ==============================================================================
 
 This file is part of the UGEN++ library
 Copyright 2008-11 The University of the West of England.
 by Martin Robinson
 
 ------------------------------------------------------------------------------
 
 UGEN++ can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 UGEN++ is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with UGEN++; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA
 
 The idea for this project and code in the UGen implementations is
 derived from SuperCollider which is also released under the 
 GNU General Public License:
 
 SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
 http://www.audiosynth.com
 
 ==============================================================================
 */


#ifndef UGEN_JUCESLIDER_H
#define UGEN_JUCESLIDER_H


#include "../core/ugen_Value.h"


class SliderValueInternal :	public ValueInternal,
							public SliderListener
{
public:
	SliderValueInternal(Slider *slider) throw();
	~SliderValueInternal() throw();
	void sliderValueChanged(Slider* slider) throw();
	
protected:
	Component::SafePointer<Slider> slider_;
};

class SliderValue : public Value
{
public:
	SliderValue(Slider* slider) throw();
private:
	SliderValue();
};


class ButtonValueInternal :	public ValueInternal,
							public ButtonListener
{
public:
	ButtonValueInternal(Button* button) throw();
	~ButtonValueInternal() throw();
	void buttonClicked(Button* button) throw();
	
private:
	Component::SafePointer<Button> button_;
};

class ButtonValue : public Value 
{
public:
	ButtonValue(Button* button) throw();
};


class LabelValueInternal :	public ValueInternal,
							public LabelListener
{
public:
	LabelValueInternal(Label* label) throw();
	~LabelValueInternal() throw();
	void labelTextChanged(Label* label) throw();
	
private:
	Component::SafePointer<Label> label_;
};

class LabelValue : public Value 
{
public:
	LabelValue(Label* label) throw();
};


class PluginSlider : public Slider
{
public:
	PluginSlider(const String& componentName = String::empty,
				 double minimum = 0.0, 
				 double maximum = 1.0,
                 double nominal = 0.5,
				 bool isExponential = false,
				 String const& units = String::empty) throw();
	
	double getValueFromText (const String& text);
    
#if JUCE_MAJOR_VERSION < 2
    const
#endif
    String getTextFromValue (double value);
	
private:
	double minimum, maximum, nominal;
	bool isExponential;
};




#endif // UGEN_JUCESLIDER_H