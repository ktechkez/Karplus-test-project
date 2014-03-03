#include "../../UGen/UGen.h"
#include "../../UGen/android/ugen_UGenAndroid.h"
#include"karplusUGen.h"

class MyAndroidAudio : public AndroidIOHost
{
public:
	//JNI functions passed from UGenAudio java class
	MyAndroidAudio(const double sampleRate, const int numInputs, const int numOutputs, const int preferredBufferSize) throw();
	UGen constructGraph(UGen const& input) throw();
	int setParameter(const int index, const float value) throw();
	int sendTrigger(const int index) throw();
	//Matched enumerator parameters for passing values between app and audio thread
	enum Params { FreqOne = 0, FreqTwo = 1, FreqThree = 2, FreqFour = 3, FreqFive = 4, FreqSix = 5,
				Cutoff = 6, DecayOne = 7, DecayTwo = 8, DecayThree = 9, DecayFour = 10, DecayFive = 11, DecaySix = 12,
				TriggerOne = 13, TriggerTwo = 14, TriggerThree = 15, TriggerFour = 16, TriggerFive = 17, TriggerSix = 18,
				Damping = 19, Vibrato = 20, Pressure = 21};

private:
	//Actual values for abve parameters
	Value freqOne, freqTwo, freqThree, freqFour, freqFive, freqSix;
	Value decayOne, decayTwo, decayThree, decayFour, decayFive, decaySix;
	Value Damping, vibrato, cutoff;
	double pressure; //Pressure is double floating point so it can be passed into envelopes
	//UGen object arrays for multiple audio events on each string
	UGenArray stringOneEvents, stringTwoEvents, stringThreeEvents, stringFourEvents, stringFiveEvents, stringSixEvents;
	int string; //String number index
};
