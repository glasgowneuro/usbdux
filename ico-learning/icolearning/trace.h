// forward decl
class Trace;


#ifndef _trace
#define _trace

#include <complex>


class Trace {
	
 public:
	/**
	 * Constructor
	 **/
	Trace();

	/**
	 * Destructor
	 **/
	~Trace();
	
	/**
	 * Filter
	 **/
	float filter(float value);
	
	/**
	 * Calculates the coefficients from the typical traceass parameters q and f.
	 * The frequency is the normalized frequency in the range [0:0.5].
	 **/
	void calcCoeffBandp(float f,float q);

	/**
	 * Calculates the coefficients for a trace: n is the number of
	 * taps and tau is a decay constant.
	 **/
	void calcCoeffTrace(int nTaps, float tau);
	
	/**
	 * Calculates the coefficients from the two conjugate poles. The parameters
	 * are the coordinates in the s-plane (!). The transformation from the
	 * s-plane to the z-plane is just now the impulseinvariant transform.
	 **/
	void calcCoeffPoles(float r,float i);
	
	/**
	 * Generates an acsii file with the impulse response of the filter.
	 **/
	void impulse(const char* name);
	
	/**
	 * the transfer function
	 **/
	std::complex<float> h(std::complex<float> s);
	
	void calc_norm();
	
	/**
	 * Generates an ASCII file with the transfer function
	 **/
	void transfer(const char* name);
	
	/**
	 * Gets the actual output of the filter. Same as the return value
	 * of the function "filter()".
	 **/
	float getActualOutput() {return actualOutput;};

	/**
	 * Gets the input signal
	 **/
	float getInputSignal() {return inputSignal;};

	/**
	 * Gets the derivative of the signal
	 **/
	float getDeriv() {return diff;};

	////////////////////////////////////////////////////////////////////

 private:
	/**
	 * normalization
	 **/
	float norm;

	/**
	 * Real part of the pole
	 **/
	float realPart;
	
	/**
	 * Imaginary part of the pole
	 **/
	float imagPart;

	/**
	 * The first pole
	 **/
	std::complex<float> s1;

	/**
	 * The second pole
	 **/
	std::complex<float> s2;

	/**
	 * The coefficients of the denominator of H(z)
	 **/
	float denominator[3];

	/**
	 * The coefficients of the enumerator of H(z)
	 **/
	float enumerator[3];

	/**
	 * Delay lines for the IIR-Filter
	 **/
	float buffer[3];

	/**
	 * Number of taps of the FIR filter for the trace
	 **/
	int taps;

	/**
	 * Delay line for the IIR filter
	 **/
	float *bufferFIR;

	/**
	 * Coefficients of the FIR filter
	 **/
	float *coeffFIR;

	/**
	 * The actual output of the filter (the return value of the filter()
	 * function).
	 **/
	float actualOutput;

	/**
	 * Sets the IIR delay line to zero
	 **/
	void resetBuffer();

	float oldOutput;
	float diff;

	float inputSignal;

	float fre,qual;

	float e,w;

 public:
	float getF() {return fre;};

	float getQ() {return qual;};

	float getReal() {return -e;};

	float getImag() {return w;};

	std::complex<float> getPole() {return std::complex<float>(-e,w);};
};

#endif
