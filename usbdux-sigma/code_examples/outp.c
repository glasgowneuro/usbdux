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
	char filename[]="/dev/comedi0";

	if (argc!=3) {
		fprintf(stderr,"%s <chan> <value>\n",argv[0]);
		exit(-1);
	}

        device = comedi_open(filename);
	if(!device){
                comedi_perror(filename);
                exit(-1);
        }
	
	ch = atoi(argv[1]);
	data = atoi(argv[2]);
		
	ret = comedi_data_write(device,1,ch,0,0, data);
        if(ret < 0){
                comedi_perror(filename);
                exit(-1);
        }
	return 0;
}
