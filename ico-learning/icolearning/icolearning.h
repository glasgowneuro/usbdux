/***********************************************************************
 * (C) Copyright 2005 Bernd Porr, http://www.berndporr.me.uk
 *
 * This is ICO learning
 *
 * Published under the GPL
 *
 * Vers 0.94
 *
 **********************************************************************/

#ifndef _Icolearning
#define _Icolearning


#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include"trace.h"



/**
 * Implements ICO-learning with the help of an array of filters.
 **/
class Icolearning {

 public:
  /**
   * Constructor. Number of inputs=2 means that there is one reflex pathway
   * and one predictive pathway. The predictive pathway has a filter
   * bank with 10 filters. If there are more inputs they will be automatically
   * predictive inputs with filter banks. The first input (reflex) has only one
   * filter.
   **/
  Icolearning(int nInputs=2, int nFi=10);

 public:
  /**
   * Destructor
   **/
  ~Icolearning();

 public:
  /**
   * Sets the input value for input i to the value f. Input 0 is always
   * the reflex pathway. Any other inputs are predictive pathways.
   **/
  void setInput(int i,float f);

 public:
  /**
   * Sets the learning rate
   **/
  void setLearningRate(float rate=0.0001) {learningRate=rate;};

 public:
  /**
   * Get the learning rate
   **/
  float getLearningRate(float rate) {return learningRate;};

  /**
   * sets a weight
   **/
  void setWeight(int channel, int index, float w) {
	  weight[channel][index]=w;
  }

  /**
   * get a pointer to a weight
   **/
  float getWeight(int channel, int index) {
	  return weight[channel][index];
  }

 public:
  /**
   * This is ICO learning. Every step the weights are updated and
   * the output is calculated.
   **/
  void prediction(int step);

 public:
  /**
   * Starts documentation. The prefix is used for all file names.
   **/
  void openDocu(const char* prefix);

 public:
  /**
   * Documents the output, the Filters and the weights and the step.
   * This has to be called every step or whenever you want to
   * have written the values to the files. The output format
   * is gnuplot compatible.
   **/
  void writeDocu(int step);

 public:
  /**
   * The output of the learner circuit
   **/
  float getOutput() {return actualActivity;};

 public:
  /**
   * set reflex
   **/
  void setReflex(float f0=0.01, // frequency
		  float q0=0.6  // quality
		 );

 public:
  /**
   * set the Predictor as a filter bank of resonators: 
   * The frequency is here the maximum frequency --- if
   * there is more than one Filter-filter per channel.
   **/
  void setPredictorsAsBandp(float f1=0.1,  // highest f in filterbank
			    float q1=0.6   // Q in filterbank for all resonators
			    );

 public:
  /**
   * set the Predictor as a filter bank of FIR traces: 
   * The number of taps define the maximum delay.
   * The taps and tau are devided by the filter index to
   * create a filter bank.
   **/
  void setPredictorsAsTraces(int nTaps=10,  // number of taps
			     float tau=0   // tau
			     );

 public:
  /**
   * Gets the weighted filter output
   **/
  float getProd(int channel, int index);

 public:
  /**
   * Only changes (drivatives) drive the predictive inputs
   **/
  void detectOnlyChanges(int changes=1) {onlyChange=changes;};

  ////////////////////////////////////////////////////////////////////

 private:
  /**
   * Input-field
   **/
  float* inputs;

 private:
  /**
   * Last input value
   **/
  float* lastInputs;

 private:
  /**
   * Docu of the weights
   **/
  FILE* fWeights;

 private:
  /**
   * Docu of the weights
   **/
  FILE* fProd;

 private:
  /**
   * Docu of the filter responses
   **/
  FILE* fFilter;

 private:
  /**
   * Docu of the output
   **/
  FILE* fOutput;

 private:
  /**
   * Docu if the weighted filter responses
   **/
  FILE* fweightFilter;

 private:
  /**
   * The learning rate
   **/
  float learningRate;

 private:
  /**
   * The actual output-activity of the predicting neuron
   **/
  float actualActivity;

 private:
  /**
   * The activity one timestep before
   **/
  float lastActivity;

 private:
  /**
   * The difference between the activities
   **/
  float delta;

 private:
  /**
   * Number of Filter-filters per channel
   **/
  int nFilters;

 private:
  /**
   * Number of input-channels (comes from the Inputs-class)
   **/
  int nChannels;

 private:
  /**
   * An array of traces
   **/
  Trace*** iir;

 private:
  /**
   * gets a pointer to an Filter-Filter
   **/
  Trace* getFilter(int channel, int index);

 private:
  /**
   * The weight-matrix
   **/
  float** weight;

  /**
   * last correlation. Needed for a proper integration.
   **/
  float** lastCorrel;

 private:
  /**
   * log file
   **/
  FILE* fLog;

 private:
  /**
   * The way how the frequencies are calculated
   * Mode 0: linear, f=f0/(i+1)
   **/
  float calcFrequ(float f0,int n);

 private:
  int doDocu;

 private:
  int onlyChange;

 private:
  int autocorrelCorrection;
};




#endif






