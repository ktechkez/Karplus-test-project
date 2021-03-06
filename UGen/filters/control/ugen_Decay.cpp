// $Id: ugen_Decay.cpp 350 2011-07-22 09:07:56Z 0x4d52 $
// $HeadURL: http://ugen.googlecode.com/svn/trunk/UGen/filters/control/ugen_Decay.cpp $

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

#ifndef UGEN_NOEXTGPL

#include "../../core/ugen_StandardHeader.h"

BEGIN_UGEN_NAMESPACE

#include "ugen_Decay.h"
#include "../../core/ugen_Constants.h"
#include "../../basics/ugen_InlineUnaryOps.h"
#include "../../basics/ugen_InlineBinaryOps.h"

DecayUGenInternal::DecayUGenInternal(UGen const& input, UGen const& decayTime) throw()
:	UGenInternal(NumInputs),
	b1(0.f),
	y1(0.f),
	currentDecayTime(0.f)
{
	inputs[Input] = input;
	inputs[DecayTime] = decayTime;
}

UGenInternal* DecayUGenInternal::getKr() throw()
{
	DecayUGenInternalK* internal = new DecayUGenInternalK(inputs[Input].kr(), 
														  inputs[DecayTime].kr()); 
	
	internal->b1 = calculateb1(currentDecayTime, UGen::getControlRateBlockSize());
	internal->y1 = y1;
	internal->currentDecayTime = currentDecayTime;
	
	return internal;
}

UGenInternal* DecayUGenInternal::getChannel(const int channel) throw()
{
	DecayUGenInternal* internal = new DecayUGenInternal(inputs[Input].getChannel(channel), 
														inputs[DecayTime].getChannel(channel));	
	internal->b1 = b1;
	internal->y1 = y1;
	internal->currentDecayTime = currentDecayTime;
	
	return internal;
}

void DecayUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw()
{
	int numSamplesToProcess = uGenOutput.getBlockSize();
	float* outputSamples = uGenOutput.getSampleData();
	float* inputSamples = inputs[Input].processBlock(shouldDelete, blockID, channel);
	float* decaySamples = inputs[DecayTime].processBlock(shouldDelete, blockID, channel);
	float newDecayTime = *decaySamples;
	
	ugen_assert(newDecayTime >= 0.f);
	
	if(newDecayTime != currentDecayTime)
	{
		float next_b1 = calculateb1(newDecayTime, 1);
		float b1_slope = (next_b1 - b1) / (float)numSamplesToProcess;
		
		while(numSamplesToProcess--)
		{
			float y0 = *inputSamples++; 
			*outputSamples++ = y1 = y0 + b1 * y1;
			b1 += b1_slope;
		}
		
		b1 = next_b1;
		currentDecayTime = newDecayTime;
	}
	else
	{
		while(numSamplesToProcess--)
		{
			float y0 = *inputSamples++; 
			*outputSamples++ = y1 = y0 + b1 * y1;
		}
	}
	
	y1 = zap(y1); // denormal!
}

void DecayUGenInternal::initValue(const float value) throw()
{
	float checkedValue = zap(value);
	UGenInternal::initValue(checkedValue);
	y1 = checkedValue;
}

void DecayUGenInternal::initb1(const float time, const int blockSize) throw()
{
	currentDecayTime = time;
	b1 = calculateb1(time, blockSize);
}

void DecayUGenInternalK::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw()
{
	const int krBlockSize = UGen::getControlRateBlockSize();
	unsigned int blockPosition = blockID % krBlockSize;
	int numSamplesToProcess = uGenOutput.getBlockSize();
	float* outputSamples = uGenOutput.getSampleData();
	float* inputSamples = inputs[Input].processBlock(shouldDelete, blockID, channel);
	float* decaySamples = inputs[DecayTime].processBlock(shouldDelete, blockID, channel);
	float newDecayTime = *decaySamples;
	
	ugen_assert(newDecayTime >= 0.f);
	
	int numKrSamples = blockPosition % krBlockSize;
	
	if(newDecayTime != currentDecayTime)
	{
		float next_b1 = calculateb1(newDecayTime, krBlockSize);
		float b1_slope = (next_b1 - b1) * krBlockSize / (float)numSamplesToProcess;
				
		while(numSamplesToProcess > 0)
		{
			float nextValue = value;
			if(numKrSamples == 0)			
			{
				b1 += b1_slope;
				float y0 = *inputSamples; 
				nextValue = y1 = y0 + b1 * y1;
			}
			
			numKrSamples = krBlockSize - numKrSamples;
			
			blockPosition		+= numKrSamples;
			inputSamples		+= numKrSamples;
			
			if(nextValue == value)
			{
				while(numSamplesToProcess && numKrSamples)
				{
					*outputSamples++ = nextValue;
					--numSamplesToProcess;
					--numKrSamples;
				}
			} 
			else
			{
				float valueSlope = (nextValue - value) / (float)UGen::getControlRateBlockSize();
				
				while(numSamplesToProcess && numKrSamples)
				{
					*outputSamples++ = value;
					value += valueSlope;
					--numSamplesToProcess;
					--numKrSamples;
				}
				
				value = nextValue;
			}
		}
		
		b1 = next_b1;
		currentDecayTime = newDecayTime;
	}
	else
	{
		while(numSamplesToProcess > 0)
		{
			float nextValue = value;
			if(numKrSamples == 0)			
			{
				float y0 = *inputSamples; 
				nextValue = y1 = y0 + b1 * y1;
			}
			
			numKrSamples = krBlockSize - numKrSamples;
			
			blockPosition		+= numKrSamples;
			inputSamples		+= numKrSamples;
			
			if(nextValue == value)
			{
				while(numSamplesToProcess && numKrSamples)
				{
					*outputSamples++ = nextValue;
					--numSamplesToProcess;
					--numKrSamples;
				}
			} 
			else
			{
				float valueSlope = (nextValue - value) / (float)UGen::getControlRateBlockSize();
				
				while(numSamplesToProcess && numKrSamples)
				{
					*outputSamples++ = value;
					value += valueSlope;
					--numSamplesToProcess;
					--numKrSamples;
				}
				
				value = nextValue;
			}
		}
	}
	
	y1 = zap(y1);
}

Decay::Decay(UGen const& input, UGen const& decayTime) throw()
{
	UGen inputs[] = { input, decayTime };
	const int numInputChannels = findMaxInputChannels(numElementsInArray(inputs), inputs);		
	initInternal(numInputChannels);
	
	for(unsigned int i = 0; i < numInternalUGens; i++)
	{
		DecayUGenInternal* internal = new DecayUGenInternal(input, decayTime);
		internal->initValue(input.getValue(i));
		internal->initb1(decayTime.getValue(i), 1);
		internalUGens[i] = internal;
	}
}


END_UGEN_NAMESPACE

#endif // gpl