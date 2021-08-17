#include "trace.h"
#include <math.h>
#include <complex>
#include <stdio.h>


// the filter

void Trace::resetBuffer() {
	buffer[0]=0;
	buffer[1]=0;
	buffer[2]=0;
	actualOutput=0.0F;
	for(int i=0;i<taps;i++) {
		bufferFIR[i]=0.0F;
	}
}



Trace::Trace() {
	taps=0;
	oldOutput=0.0F;
	diff=0.0F;
	fre=0;
	qual=0;
	norm=1.0F;
	e=0.0F;
	w=0.0F;
	enumerator[0]=0;
	enumerator[1]=0;
	enumerator[2]=0;
	denominator[0]=0;
	denominator[1]=0;
	denominator[2]=0;
	bufferFIR=NULL;
	coeffFIR=NULL;
}


Trace::~Trace() {
	if (bufferFIR) {
		delete bufferFIR;
	}
	if (coeffFIR) {
		delete coeffFIR;
	}
}


void Trace::impulse(const char* name) {
	int steps=1000;
	fprintf(stderr,"Impulse resp: %s, %d steps\n",name,steps);
	for(int i=0;i<steps;i++) {
		filter(0);
	}
	float input=0.0;
	FILE* ff=fopen(name,"wt");
	if (!ff) {
		fprintf(stderr,"Couldn't open %s \n",name);
		return;
	}
	for(int i=0;i<steps;i++) {
		if (i==10) {
			input=1.0F;
		} else {
			input=0.0F;
		}
		fprintf(ff,"%d %f\n",i,filter(input));
	}
	fclose(ff);
	resetBuffer();
}


std::complex<float> Trace::h(std::complex<float> s) {
	return std::complex<float>(1,0)/((s+s1)*(s+s2));
}



void Trace::transfer(const char* name) {
	int steps=1000;
	fprintf(stderr,"Transfer function: %s, %d steps\n",name,steps);
	FILE* ff=fopen(name,"wt");
	if (!ff) {
		fprintf(stderr,"Couldn't open %s \n",name);
		return;
	}
	for(int i=0;i<steps;i++) {
		float f=((float)i)/((float)steps)*0.5;
		std::complex<float> iw=std::complex<float>(0,f*M_PI*2);
		fprintf(ff,"%f %f\n",f,(float)sqrt(real(h(iw)*h(-iw))));
	}
	fclose(ff);
	resetBuffer();
}






float Trace::filter(float value) {
	float output=0.0F;
	if (taps==0) {
		// IIR
		inputSignal=value;
		float input=0.0;
		// a wee bit cryptic but a wee bit optimized for speed
		input=value;
		output=(enumerator[1]*buffer[1]);
		input=input-(denominator[1]*buffer[1]);
		output=output+(enumerator[2]*buffer[2]);
		input=input-(denominator[2]*buffer[2]);
		output=output+input*enumerator[0];
		buffer[2]=buffer[1];
		buffer[1]=input;
	} else {
		// FIR
		// shift it
		for(int i=taps-1;i>0;i--) {
			bufferFIR[i]=bufferFIR[i-1];
		}
		// store new value
		bufferFIR[0]=value;
		// calculate result
		for(int i=0;i<taps;i++) {
			output+=bufferFIR[i]*coeffFIR[i];
		}
	}
	output=output/norm;
	actualOutput=output;
	diff=actualOutput-oldOutput;
	oldOutput=actualOutput;
	return output;
}




void Trace::calcCoeffPoles(float r,float i) {
  realPart=r;
  imagPart=i;
  s1=std::complex<float>(r,i);
  s2=std::complex<float>(r,-i);
  enumerator[0]=0;
  enumerator[1]=0;
  enumerator[2]=1;
  denominator[0]=1;
  denominator[1]=real(-exp(s2)-exp(s1));
  denominator[2]=real(exp(s1+s2));
}



void Trace::calcCoeffBandp(float f,float q) {
	fre=f;
	qual=q;
	float fTimesPi;
	if (q>0) { // quality is ok
		fTimesPi=f*M_PI*2;
		e=fTimesPi/(2.0*q);
		if ((fTimesPi*fTimesPi-e*e)>0) { // root ok
			w=sqrt(fTimesPi*fTimesPi-e*e);
			if (taps) {
				int middle=(taps-1)/2+1;
				for(int i=0;i<((taps-1)/2);i++) {
					float coeff=(1/w)*exp(-e*i)*sin(w*i);
					coeffFIR[middle+i]=coeff;
					coeffFIR[middle-i]=coeff;
				}
			} else {
				calcCoeffPoles(-e,w);
			}
		} else { // root
			fprintf(stderr,"bandpass: The quality is too low\n");
			exit(1);
		} // 
	} else { // Q bad
		fprintf(stderr,"bandpass: The quality is negative!\n");
		exit(1);
	} // Q bad
	resetBuffer();
}


/**
 * Calculates the coefficients for a trace: n is the number of
 * taps and tau is a decay constant.
 **/
void Trace::calcCoeffTrace(int nTaps, float tau) {
	taps=nTaps;
	if (taps>0) {
		if (!bufferFIR) {
			bufferFIR=new float[taps];
		}
		if (!coeffFIR) {
			coeffFIR=new float[taps];
		}
		for(int i=0;i<taps;i++) {
			coeffFIR[i]=exp(-((float)i)*tau);
		}
	}
}
