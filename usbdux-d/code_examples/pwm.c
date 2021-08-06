#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>


int main(int argc, char *argv[])
{
        int ret,i,help;
	comedi_insn insn;
	lsampl_t d[5];
	comedi_t *device;

        int freq;
        int duty;

        device = comedi_open("/dev/comedi0");
        if(!device){
		fprintf(stderr,"Could not open comedi\n");
                exit(-1);
        }

	insn.insn=INSN_CONFIG;
	insn.data=d;
	insn.subdev=4;
	insn.chanspec=CR_PACK(0,0,0);

        for(i=1; i<argc; ++i)
        {
		if(!strcmp(argv[i], "-on")) {
			d[0] = INSN_CONFIG_ARM;
			d[1] = 0;
			insn.n=2;
			ret=comedi_do_insn(device,&insn);
			if(ret < 0){
				fprintf(stderr,"Could sw on:%d\n",ret);
				exit(-1);
			}
		}

		if(!strcmp(argv[i], "-off")) {
			d[0] = INSN_CONFIG_DISARM;
			d[1] = 0;
			insn.n=1;
			ret=comedi_do_insn(device,&insn);
			if(ret < 0){
				fprintf(stderr,"Could sw on:%d\n",ret);
				exit(-1);
			}
		}
#ifdef INSN_CONFIG_PWM_SET_PERIOD
		if(!strcmp(argv[i], "-freq")) {
			freq = atoi(argv[i+1]);
			d[0] = INSN_CONFIG_PWM_SET_PERIOD;
			d[1] = 1E9/freq;
			insn.n=2;
			ret=comedi_do_insn(device,&insn);
			if(ret < 0){
				fprintf(stderr,"Could set frequ:%d\n",ret);
				exit(-1);
			}
		}
#endif
		if(!strcmp(argv[i], "-duty")) duty = atoi(argv[i+1]);
		if(!strcmp(argv[i], "-help")) help = 1;
        }
        if(help)
        {
		printf("----------------------------------------------\n");
		printf("-on/-off \t Turn PWM on/off\n");
		printf("-freq \t \t Adjust frequency\n");
		printf("-duty \t \t Adjust duty cycle\n");
		printf("----------------------------------------------\n");
		exit(0);
        }

#ifdef INSN_CONFIG_GET_PWM_STATUS
	d[0] = INSN_CONFIG_GET_PWM_STATUS;
	insn.n=2;
	ret=comedi_do_insn(device,&insn);
	if(ret < 0){
		fprintf(stderr,"Could not get status:%d insn=%d\n",
			ret,
			d[0]);
		exit(-1);
	}
	if (d[1])
		fprintf(stderr,"PWM is on\n");
	else
		fprintf(stderr,"PWM is off\n");
#endif
#ifdef INSN_CONFIG_PWM_GET_PERIOD
	d[0] = INSN_CONFIG_PWM_GET_PERIOD;
	insn.n=2;
	ret=comedi_do_insn(device,&insn);
	if(ret < 0){
		fprintf(stderr,"Could get frequ:%d\n",ret);
		exit(-1);
	}
	freq = 1E9 / d[1];
      	printf("Frequency is %d\n", freq);
#endif

	int channel=0;
	// it's 0..511
	comedi_data_write(device,
			  4, 
			  channel,
			  0,
			  0,
			  duty);
	comedi_data_write(device,
			  4, 
			  channel+1,
			  0,
			  0,
			  400);
	comedi_data_write(device,
			  4, 
			  channel+2,
			  0,
			  0,
			  100);
	comedi_data_write(device,
			  4, 
			  channel+3,
			  0,
			  0,
			  200);

        return 0;
}
