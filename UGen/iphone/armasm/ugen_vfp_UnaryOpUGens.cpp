// $Id: ugen_vfp_UnaryOpUGens.cpp 350 2011-07-22 09:07:56Z 0x4d52 $
// $HeadURL: http://ugen.googlecode.com/svn/trunk/UGen/iphone/armasm/ugen_vfp_UnaryOpUGens.cpp $

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

#include "../../core/ugen_StandardHeader.h"

#ifdef UGEN_VFP

BEGIN_UGEN_NAMESPACE

#include "ugen_vfp_Utilities.h"
#include "../../basics/ugen_UnaryOpUGens.h"


void UnaryNegUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel);
	VFP::neg16(inputSamples, outputSamples, numSamplesToProcess);
}

void UnaryAbsUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel); 
	VFP::abs16(inputSamples, outputSamples, numSamplesToProcess); 
}

void UnaryReciprocalUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel); 
	VFP::reciprocal(inputSamples, outputSamples, numSamplesToProcess);
}

void UnarySquaredUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel); 
	VFP::squared(inputSamples, outputSamples, numSamplesToProcess); 
}

void UnaryCubedUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel); 
	VFP::cubed(inputSamples, outputSamples, numSamplesToProcess); 
}

void UnarySqrtUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw() 
{ 
	const int numSamplesToProcess = uGenOutput.getBlockSize(); 
	float* const outputSamples = uGenOutput.getSampleData(); 
	const float* const inputSamples = inputs[Operand].processBlock(shouldDelete, blockID, channel); 
	VFP::sqrt16(inputSamples, outputSamples, numSamplesToProcess); 
}







END_UGEN_NAMESPACE

#endif
