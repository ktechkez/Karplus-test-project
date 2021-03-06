// $Id: ugen_Thru.cpp 352 2011-10-08 08:47:36Z 0x4d52 $
// $HeadURL: http://ugen.googlecode.com/svn/trunk/UGen/basics/ugen_Thru.cpp $

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

#include "../core/ugen_StandardHeader.h"

BEGIN_UGEN_NAMESPACE

#include "ugen_Thru.h"

ThruUGenInternal::ThruUGenInternal(Thru_InputsWithTypesOnly) throw()
:	UGenInternal(NumInputs)
{
	inputs[Input] = input;
}

void ThruUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw()
{
	const int numSamplesToProcess = uGenOutput.getBlockSize();
	float* const outputSamples = uGenOutput.getSampleData();
	float* const inputSamples = inputs[Input].processBlock(shouldDelete, blockID, channel);
	
	memcpy(outputSamples, inputSamples, numSamplesToProcess * sizeof(float));
}

Thru::Thru(Thru_InputsWithTypesOnly) throw()
{
	initInternal(input.getNumChannels());
	for(unsigned int i = 0; i < numInternalUGens; i++)
	{
		internalUGens[i] = new ThruUGenInternal(Thru_InputsNoTypes);
		internalUGens[i]->initValue(input.getValue(i));
	}
}

END_UGEN_NAMESPACE
