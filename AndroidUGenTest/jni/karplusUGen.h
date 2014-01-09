
#ifndef _UGEN_karplusUGen_H_
#define _UGEN_karplusUGen_H_

class OnePole {
public:
    OnePole() {a0 = 1.0; b1 = 0.0; z1 = 0.0;};
    OnePole(double Fc) {z1 = 0.0; setFc(Fc);};
    void setFc(double Fc);
    float process(float in);

protected:
    double a0, b1, z1;

private:
    OnePole(OnePole const&);
    OnePole& operator= (OnePole const&);
};

inline void OnePole::setFc(double Fc) {
    b1 = exp(-2.0 * M_PI * Fc);
    a0 = 1.0 - b1;
}

inline float OnePole::process(float in) {
    return z1 = in * a0 + z1 * b1;
}


// A new UGen also needs a UGenInternal which does the actual processing. 
// UGen and its subclasses are primarily for constructing the UGen graphs.
// Simple internals should inherit from UGenInternal.
/** @ingroup UGenInternals */
class karplusUGenInternal : public UGenInternal
{
public:
	// Constructor.
	karplusUGenInternal(UGen const& input,
						const float minFreq,
						UGen const& fundamental,
						UGen const& decayTime = 1.f,
						UGen const& cutoff = 1200.f) throw();
	
	// This is called when the internal is needed to process a new block of samples.
	void processBlock(bool& shouldDelete, const unsigned int blockID, const int channel) throw();
	
	enum Inputs { Input, Fundamental, DecayTime, Cutoff, NumInputs }; // used mainly by the 'inputs' array for the UGenInternal's UGen inputs
	
protected:
	Buffer buffer;
	int writePosition;
	OnePole filter;
};

// A macro for declaring a UGen subclass.
// The parentheses around the 2nd and 3rd arguments are required.
// Items within these parentheses should be comma-separated just as for argument declarations and calls.
UGenSublcassDeclarationNoDefault
(
	karplusUGen,			// The UGen name, this will inherit from UGen
	(input, minFreq, fundamental, decayTime, cutoff),				// argument list for the Constructor, AR and KR methods as they would be CALLED
	        (UGen const& input,
			const float minFreq,
			UGen const& fundamental,
			UGen const& decayTime = 1.f,
			UGen const& cutoff = 1200.f),	// argument list for the Constructor, AR and KR methods as they are to be DECLARED
	COMMON_UGEN_DOCS		// Documentation (for Doxygen)
);



#endif // _UGEN_karplusUGen_H_

