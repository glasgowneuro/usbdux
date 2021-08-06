#include <stdio.h>   /* for printf() */
#include <comedilib.h>
#include <stdlib.h>

int subdev = 0;         /* change this to your input subdevice */
int chan = 0;           /* change this to your channel */
int range = 0;          /* more on this later */
int aref = AREF_GROUND; /* more on this later */

int main(int argc,char *argv[])
{
	comedi_t *it;
	lsampl_t data;
	int i;
	
	if (argc < 2) {
		fprintf(stderr,"%s channel\n",argv[0]);
		exit(0);
	}
	
	it=comedi_open("/dev/comedi0");
	
	chan = atoi(argv[1]);
	
	comedi_data_read(it,subdev,chan,range,aref, & data);
	
	printf("%08x  ",data);
	for(i = 0;i<data/0x080000;i++) {
		printf(" ");
	}
	printf("o\n");

	return 0;
}
