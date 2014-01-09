
#include "AndroidUGen.h"
#include <android/log.h>

// the JNI code needs to link to this function to create the host
AndroidIOHost* createHost(const double sampleRate, const int numInputs, const int numOutputs, const int preferredBufferSize)
{
	return new MyAndroidAudio(sampleRate, numInputs, numOutputs, preferredBufferSize);
}

MyAndroidAudio::MyAndroidAudio(const double sampleRate, const int numInputs, const int numOutputs, const int preferredBufferSize) throw()
:	AndroidIOHost(sampleRate, numInputs, numOutputs, preferredBufferSize),
	freqOne(0), freqTwo(0), freqThree(0), freqFour(0), freqFive(0), freqSix(0), cutoff(0), dampening(0),
	decayOne(0), decayTwo(0), decayThree(0), decayFour(0), decayFive(0), decaySix(0), vibrato(0), pressure(0)
{
	__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::MyAndroidAudio()");
}

int MyAndroidAudio::setParameter(const int index, const float value) throw()
{
	__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::setParameter(%d, %f)", index, value);

	switch(index)
	{
		case FreqOne:		freqOne.setValue(value);		return 0;
		case FreqTwo:		freqTwo.setValue(value);		return 0;
		case FreqThree:		freqThree.setValue(value);		return 0;
		case FreqFour:		freqFour.setValue(value);		return 0;
		case FreqFive:		freqFive.setValue(value);		return 0;
		case FreqSix:		freqSix.setValue(value);		return 0;
		case Cutoff:		cutoff.setValue(value);			return 0;
		case DecayOne:		decayOne.setValue(value);		return 0;
		case DecayTwo:		decayTwo.setValue(value);		return 0;
		case DecayThree:	decayThree.setValue(value);		return 0;
		case DecayFour:		decayFour.setValue(value);		return 0;
		case DecayFive:		decayFive.setValue(value);		return 0;
		case DecaySix:		decaySix.setValue(value);		return 0;
		case Dampening:		dampening.setValue(value);		return 0;
		case Vibrato: 		vibrato.setValue(value);		return 0;
		case Pressure: 		pressure = value;				return 0;
	}

	return -1; // param not found
}

int MyAndroidAudio::sendTrigger(const int index) throw()
{
	if (index == TriggerOne)
	{
		Env env = Env::perc(0.0001,0.05, pressure/2);
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));

		lock();
		stringOneEvents.add(noise);
		unlock();

		return 0;
	}
	if (index == TriggerTwo)
		{
			Env env = Env::perc(0.0001,0.05,pressure/2);
			UGen noise = WhiteNoise::AR(EnvGen::AR(env));

			lock();
			stringTwoEvents.add(noise);
			unlock();

			return 0;
		}
	if (index == TriggerThree)
		{
		Env env = Env::perc(0.0001,0.05,pressure/2);
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));

		lock();
		stringThreeEvents.add(noise);
		unlock();

		return 0;
		}
	if (index == TriggerFour)
		{
		Env env = Env::perc(0.0001,0.05,pressure/2);
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));

		lock();
		stringFourEvents.add(noise);
		unlock();

		return 0;
		}
	if (index == TriggerFive)
		{
		Env env = Env::perc(0.0001,0.05,pressure/2);
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));

		lock();
		stringFiveEvents.add(noise);
		unlock();

		return 0;
		}
	if (index == TriggerSix)
		{
		Env env = Env::perc(0.0001,0.05,pressure/2);
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));

		lock();
		stringSixEvents.add(noise);
		unlock();

		return 0;
		}
	return -1; // param not found
}

UGen MyAndroidAudio::constructGraph(UGen const& input) throw()
{
	__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::constructGraph()");
	UGen stringOneMix = Mix::AR(stringOneEvents, false);
	UGen stringTwoMix = Mix::AR(stringTwoEvents, false);
	UGen stringThreeMix = Mix::AR(stringThreeEvents, false);
	UGen stringFourMix = Mix::AR(stringFourEvents, false);
	UGen stringFiveMix = Mix::AR(stringFiveEvents, false);
	UGen stringSixMix = Mix::AR(stringSixEvents, false);
	UGen vibOsc = SinOsc::AR(5, 0, UGen(vibrato).lag());
	UGen stringOneLoop = karplusUGen::AR(stringOneMix, 30, UGen(293.66 + freqOne) + vibOsc.ar().lag(0.005), UGen((10 - decayOne) - dampening), 5000);
	UGen stringTwoLoop = karplusUGen::AR(stringTwoMix, 30, UGen(246.94 + freqTwo) + vibOsc.ar().lag(0.005), UGen((10 - decayTwo) - dampening), 5000);
	UGen stringThreeLoop = karplusUGen::AR(stringThreeMix, 30, UGen(196 + freqThree) + vibOsc.ar().lag(0.005), UGen((10 - decayThree) - dampening), 5000);
	UGen stringFourLoop = karplusUGen::AR(stringFourMix, 30, UGen(146.83 + freqFour) + vibOsc.ar().lag(0.005), UGen((10 - decayFour) - dampening), 5000);
	UGen stringFiveLoop = karplusUGen::AR(stringFiveMix, 30, UGen(98 + freqFive) + vibOsc.ar().lag(0.005), UGen((10 - decayFive) - dampening), 5000);
	UGen stringSixLoop = karplusUGen::AR(stringSixMix, 30, UGen(73.42 + freqSix) + vibOsc.ar().lag(0.005), UGen((10 - decaySix) - dampening), 5000);

	//Filter peaks taken from https://courses.physics.illinois.edu/phys406/NSF_REU_Reports/2002_reu/Eric_Moon_REU_Report.pdf
	UGen filterOne = BPeakEQ::AR(stringOneLoop + stringTwoLoop + stringThreeLoop + stringFourLoop + stringFiveLoop + stringSixLoop, 90, 2, 0.2);
	UGen filterTwo = BPeakEQ::AR(filterOne, 180, 5, 0.2);
	UGen filterThree = BPeakEQ::AR(filterTwo, 786, 8, 0.2);
	UGen outputMix = Mix::AR(filterThree, false);
	return outputMix;
}


