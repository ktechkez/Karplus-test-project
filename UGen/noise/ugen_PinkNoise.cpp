// $Id: ugen_PinkNoise.cpp 371 2012-11-13 07:04:54Z 0x4d52 $
// $HeadURL: http://ugen.googlecode.com/svn/trunk/UGen/noise/ugen_PinkNoise.cpp $

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

#include "../core/ugen_StandardHeader.h"

BEGIN_UGEN_NAMESPACE

#include "ugen_PinkNoise.h"
#include "../core/ugen_Bits.h"

PinkNoiseUGenInternal::PinkNoiseUGenInternal() throw()
:	UGenInternal(NoInputs),
	//random((unsigned int)this * 123463463UL + 423815L + rand(19469146))
	random(rand(0x7fffffff))
{	
	unsigned int s1, s2, s3;
	random.get(s1, s2, s3);
	
	for (int i = 0; i < 16; ++i) 
	{
		unsigned int r = Ran088::next(s1, s2, s3) >> 13;
		total += r;
		dice[i] = r;
	}	
	
	random.set(s1, s2, s3);
	
	initValue(random.nextBiFloat());
}

void PinkNoiseUGenInternal::processBlock(bool& /*shouldDelete*/, const unsigned int /*blockID*/, const int /*channel*/) throw()
{
	int numSamplesToProcess = uGenOutput.getBlockSize();
	float* outputSamples = uGenOutput.getSampleData();
	
	unsigned int s1, s2, s3;
	random.get(s1, s2, s3);
	
	while(numSamplesToProcess--)
	{
		unsigned int counter = Ran088::next(s1, s2, s3);
		unsigned int newrand = counter >> 13;
		int k = Bits::countTrailingZeros(counter) & 15; 
		unsigned int prevrand = dice[k]; 
		dice[k] = newrand; 
		total += (newrand - prevrand); 
		newrand = Ran088::next(s1,s2,s3) >> 13;
		Element val;
		val.u = (total + newrand) | 0x40000000;
		*outputSamples++ = (val.f - 3.f);
	}
	
	random.set(s1, s2, s3);
}

PinkNoise::PinkNoise() throw()
{
	initInternal(1);
	internalUGens[0] = new PinkNoiseUGenInternal();
}

END_UGEN_NAMESPACE

#endif // gpl
