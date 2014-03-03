
#include "AndroidUGen.h"
#include <android/log.h>

// the JNI code needs to link to this function to create the host
AndroidIOHost* createHost(const double sampleRate, const int numInputs, const int numOutputs, const int preferredBufferSize)
{
	return new MyAndroidAudio(sampleRate, numInputs, numOutputs, preferredBufferSize);
}
//Initiliase Android audio class with defaults
MyAndroidAudio::MyAndroidAudio(const double sampleRate, const int numInputs, const int numOutputs, const int preferredBufferSize) throw()
:	AndroidIOHost(sampleRate, numInputs, numOutputs, preferredBufferSize),
	freqOne(0), freqTwo(0), freqThree(0), freqFour(0), freqFive(0), freqSix(0), cutoff(4000), Damping(0),
	decayOne(0), decayTwo(0), decayThree(0), decayFour(0), decayFive(0), decaySix(0), vibrato(0), pressure(0)
{
	__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::MyAndroidAudio()");
}

/*
 * The setParameter function allows constant values to be passed from Android
 * application. The index value passed in specifies which parameter, while the
 * floating point value carries the actual data changes
 */
int MyAndroidAudio::setParameter(const int index, const float value) throw()
{
	//__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::setParameter(%d, %f)", index, value);

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
		case Damping:		Damping.setValue(value);		return 0;
		case Vibrato: 		vibrato.setValue(value);		return 0;
		case Pressure: 		pressure = value;				return 0;
	}

	return -1; // param not found
}

/*
 * The sendTrigger function allows for one off trigger events in the main application
 * to switch audio parameter values from one condition to another.
 * In this case, its used simply to trigger the exctitation noise impulse
 * that is passed into the waveguide objects
 */
int MyAndroidAudio::sendTrigger(const int index) throw()
{
	if (index == TriggerOne)//If the first string is striggered...
	{
		//Create a percussive amplitude envelope with a sharp attack, quick release
		Env env = Env::perc(0.0001,0.05, pressure/2); //Amplitude is touch pressure scaled by 50%
		/* The enveloped above is generated and passed into a noise oscillator as
		 * the amplitude parameter. This creates a single sharp impulse
		 */
		UGen noise = WhiteNoise::AR(EnvGen::AR(env));
		lock(); //Lock audio thread
		stringOneEvents.add(noise); //Add noise to string event array
		unlock(); //Unlock audio thread

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

/*
 * The constructGraph function is where the meat of the audio processing takes place.
 * The audio data is used to construct the output graph which is then sent back
 * to the Android audioTrack object for buffering/output
 */
UGen MyAndroidAudio::constructGraph(UGen const& input) throw()
{
	__android_log_print(ANDROID_LOG_DEBUG, "ugen", "MyAndroidAudio::constructGraph()");
	//Create UGen mix objects with the above event arrays for passing into waveguides
	UGen stringOneMix = Mix::AR(stringOneEvents, false);
	UGen stringTwoMix = Mix::AR(stringTwoEvents, false);
	UGen stringThreeMix = Mix::AR(stringThreeEvents, false);
	UGen stringFourMix = Mix::AR(stringFourEvents, false);
	UGen stringFiveMix = Mix::AR(stringFiveEvents, false);
	UGen stringSixMix = Mix::AR(stringSixEvents, false);
	//Create a slow sine oscillator for vibrato modulation. The depth of which is specified by device tilt
	UGen vibOsc = SinOsc::AR(5, 0, UGen(vibrato).lag());
	/*
	 * The below objects are delay-line waveguides (one per string)
	 * The inputs are all taken from the specified user controls
	 * This is explained in detail in chapter 2 of the corresponding report.
	 */
	UGen stringOneLoop = karplusUGen::AR(stringOneMix, 30, UGen(freqOne) + vibOsc.ar().lag(0.005), UGen((10 - decayOne) - Damping), cutoff);
	UGen stringTwoLoop = karplusUGen::AR(stringTwoMix, 30, UGen(freqTwo) + vibOsc.ar().lag(0.005), UGen((10 - decayTwo) - Damping), cutoff);
	UGen stringThreeLoop = karplusUGen::AR(stringThreeMix, 30, UGen(freqThree) + vibOsc.ar().lag(0.005), UGen((10 - decayThree) - Damping), cutoff);
	UGen stringFourLoop = karplusUGen::AR(stringFourMix, 30, UGen(freqFour) + vibOsc.ar().lag(0.005), UGen((10 - decayFour) - Damping), cutoff);
	UGen stringFiveLoop = karplusUGen::AR(stringFiveMix, 30, UGen(freqFive) + vibOsc.ar().lag(0.005), UGen((10 - decayFive) - Damping), cutoff);
	UGen stringSixLoop = karplusUGen::AR(stringSixMix, 30, UGen(freqSix) + vibOsc.ar().lag(0.005), UGen((10 - decaySix) - Damping), cutoff);
	/* The below filters in series are used to model the resonant effect
	 * of a guitar body on the sound characteristic
	 * Filter peaks taken from https://courses.physics.illinois.edu/phys406/NSF_REU_Reports/2002_reu/Eric_Moon_REU_Report.pdf
	 */
	UGen filterOne = BPeakEQ::AR(stringOneLoop + stringTwoLoop + stringThreeLoop + stringFourLoop + stringFiveLoop + stringSixLoop, 90, 2, 0.2);
	UGen filterTwo = BPeakEQ::AR(filterOne, 180, 5, 0.2);
	UGen filterThree = BPeakEQ::AR(filterTwo, 786, 8, 0.2);
	//Create a final output mix and return
	UGen outputMix = Mix::AR(filterThree, false);
	return outputMix;
}


