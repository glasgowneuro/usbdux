#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>

comedi_t *device;


int main(int argc, char *argv[])
{
        lsampl_t data;
	int ch;
        int ret;
	int subdevice;
	int stype;
	char filename[]="/dev/comedi0";

	if (argc!=2) {
		fprintf(stderr,"%s <chan>\n",argv[0]);
		exit(-1);
	}

        device = comedi_open(filename);
	if(!device){
                comedi_perror(filename);
                exit(-1);
        }
	
	ch = atoi(argv[1]);
	subdevice = 2;

        stype = comedi_get_subdevice_type(device, subdevice);
        if(stype != COMEDI_SUBD_DIO){
                printf("%d is not a digital I/O subdevice\n",subdevice);
                exit(-1);
        }

	ret = comedi_dio_config(device,subdevice,ch,COMEDI_INPUT);
        if(ret < 0){
                comedi_perror(filename);
                exit(-1);
        }
	
	ret = comedi_dio_read(device,subdevice,ch,&data);
        if(ret < 0){
                comedi_perror(filename);
                exit(-1);
        }

	printf("%d ",data);

	return 0;
}
