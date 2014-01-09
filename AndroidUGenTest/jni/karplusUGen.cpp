
#include "../../UGen/UGen.h"

#include "karplusUGen.h"

karplusUGenInternal::karplusUGenInternal(UGen const& input,
										const float minFreq,
										UGen const& fundamental,
										UGen const& decayTime,
										UGen const& cutoff) throw()
:	UGenInternal(NumInputs) // Initialise the UGenIntenal's UGen inputs array using the enum size.
{
	inputs[Input] = input;
	inputs[Fundamental] = fundamental;
	inputs[DecayTime] = decayTime;
	inputs[Cutoff] = cutoff;
	//..
	buffer = Buffer(BufferSpec((1/minFreq) * UGen::getSampleRate() + 50, 1, true));
	writePosition = 0;
}

static inline float calcFeedback(float delay, float decay)
	{
		if (delay == 0.f) {
			return 0.f;
		} else if (decay > 0.f) {
			return (float)exp(log001 * delay / decay);
		} else if (decay < 0.f) {
			return (float)-exp(log001 * delay / -decay);
		} else {
			return 0.f;
		}
	}

void karplusUGenInternal::processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw()
{
	// Get the number of samples to process this block.
	int numSamplesToProcess = uGenOutput.getBlockSize();
	
	// Get a pointer to the output sample array.
	float* outputSamples = uGenOutput.getSampleData();
	
	// Get a pointer to the input sample array.
	float* inputSamples = inputs[Input].processBlock(shouldDelete, blockID, channel);
	float fundamental = inputs[Fundamental].processBlock(shouldDelete, blockID, channel)[0];
	float decayTime = inputs[DecayTime].processBlock(shouldDelete, blockID, channel)[0];
	float cutoff = inputs[Cutoff].processBlock(shouldDelete, blockID, channel)[0];
	float* bufferSamples = buffer.getData (0);
	const int bufferLength = buffer.size();
	float delayInSeconds = 1.f/fundamental;
	float feedback = calcFeedback(delayInSeconds, decayTime);
	filter.setFc(cutoff * UGen::getReciprocalSampleRate());

	float delayTime = delayInSeconds * UGen::getSampleRate();

	// The dsp loop:
	while(numSamplesToProcess--)
	{
		float bufferReadPos = writePosition - delayTime;
		if (bufferReadPos < 0)
			bufferReadPos += bufferLength;

		const int iIndex0 = (int)bufferReadPos;
		const int iIndex1 = iIndex0+1;
		const float frac = bufferReadPos - (float)iIndex0;
		const float value0 = bufferSamples[iIndex0];
		float value1;

		if(iIndex1 == bufferLength) {
			value1 = bufferSamples[0];
		} else {
			value1 = bufferSamples[iIndex1];
		}

		float value = filter.process(value0 + frac * (value1 - value0));

		bufferSamples[writePosition] = value * feedback + *inputSamples++;

		*outputSamples++ = value;

		writePosition ++;
		if (writePosition == bufferLength)
			writePosition = 0;
	}
}

karplusUGen::karplusUGen(UGen const& input,
						const float minFreq,
						UGen const& fundamental,
						UGen const& decayTime,
						UGen const& cutoff) throw()
{
	initInternal(input.getNumChannels());
	
	for(unsigned int i = 0; i < numInternalUGens; i++)
	{
		internalUGens[i] = new karplusUGenInternal(input, minFreq, fundamental, decayTime, cutoff);
	}
}
