#include "icolearning.h"
#include <unistd.h>


int trace=0;

void icoLearningWithOneFilterLow() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,1);
	icolearning.setLearningRate(0.000001);
	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.01,0.6);
	icolearning.openDocu("onef_low");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==525)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		icolearning.writeDocu(step);
	}
}




void icoLearningWithOneFilterHigh() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,1);
	icolearning.setLearningRate(0.0001);
	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.01,0.6);
	icolearning.openDocu("onef_high");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==525)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		if (step%10==0)
			icolearning.writeDocu(step);
	}
}





void icoLearningWithOneFilterHighest001() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,1);
	icolearning.setLearningRate(0.001);
	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.01,0.6);
	icolearning.openDocu("onef_highest");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==525)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		icolearning.writeDocu(step);
	}
}



void icoLearningWithOneFilterHighest005() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,1);
	icolearning.setLearningRate(0.001);
	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.01,0.6);
	icolearning.openDocu("onef_highest");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==503)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		icolearning.writeDocu(step);
	}
}





void icoLearningWith10FilterHigh() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,10);
	icolearning.setLearningRate(0.0001);

	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.1,0.6);
	icolearning.openDocu("10filt_high");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==525)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		if (step%10==0) {
			icolearning.writeDocu(step);
		}
	}
}




void icoLearningWith10FilterHighest() {
	// Two inputs: reflex and predictor
	// Just one filter in the filterbank
	Icolearning icolearning(2,10);
	icolearning.setLearningRate(0.01);
	icolearning.setReflex(0.01,0.6);
	if (trace)
		icolearning.setPredictorsAsTraces(50);
	else
		icolearning.setPredictorsAsBandp(0.1,0.6);
	icolearning.openDocu("10filt_highest");
	for(int step=0;step<150000;step++) {
		if (step%1000==500) {
			icolearning.setInput(1,1);
		} else {
			icolearning.setInput(1,0);
		}
		if ((step%1000==525)&&(step<100000)) {
			icolearning.setInput(0,1);
		} else {
			icolearning.setInput(0,0);
		}
		icolearning.prediction(step);
		if (step%10==0) {
			icolearning.writeDocu(step);
		}
	}
}







void stdpWithOneFilterLow() {
	FILE* f=fopen("stdp_low.dat","wt");
	for(int t=-200;t<=200;t++) {
		Icolearning* icolearning=new Icolearning(2,1);
		icolearning->setLearningRate(0.000001);
		if (trace)
			icolearning->setPredictorsAsTraces(50);
		else
			icolearning->setPredictorsAsBandp(0.1,0.6);
		fprintf(stderr,"%d\n",t);
		fflush(stderr);
		for(int step=0;step<5000;step++) {
			if ((step%1000)==500) {
				icolearning->setInput(1,1);
			} else {
				icolearning->setInput(1,0);
			}
			if ((step%1000)==(500+t)) {
				icolearning->setInput(0,1);
			} else {
				icolearning->setInput(0,0);
			}
			icolearning->prediction(step);
		}
		fprintf(f,"%d %f\n",t,icolearning->getWeight(1,0));
		delete icolearning;
	}
	fclose(f);
	fprintf(stderr,"\n");
}











int main(int argc, char *argv[]) {
        int c;
	int demoNumber=0;

        while (-1 != (c = getopt(argc, argv, "td:"))) {
                switch (c) {
                case 'd':
                        demoNumber = strtoul(optarg,NULL,0);
                        break;

		case 't':
			// switch to trace instead of bandpass
			trace = 1;
			fprintf(stderr,"Filter responses are traces\n");
			break;

		default:
			printf("bad option\n");
			exit(1);
		}
	}

	fprintf(stderr,"Demo #%d\n",demoNumber);
	switch (demoNumber) {
	case 0:
		icoLearningWithOneFilterLow();
		break;
	case 1:
		icoLearningWithOneFilterHigh();
		break;
	case 2:
		icoLearningWithOneFilterHighest001();
		break;
	case 3:
		icoLearningWithOneFilterHighest005();
		break;

	case 100:
		icoLearningWith10FilterHigh();
		break;

	case 101:
		icoLearningWith10FilterHighest();
		break;

	case 1000:
		stdpWithOneFilterLow();
		break;


	}

}
