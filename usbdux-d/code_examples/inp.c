/*
 * A very small one-shot input demo
 * Part of Comedilib
 *
 * Copyright (c) 1999,2000 David A. Schleef <ds@schleef.org>
 *               2011 Bernd Porr tech@linux-usb-daq.co.uk
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */
/*
   A little input demo
 */

#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <math.h>

int main(int argc, char *argv[])
{
	comedi_t *device;
	lsampl_t data;
	int ret;
	comedi_range * range_info;
	lsampl_t maxdata;
	double physical_value;
	int subdevice = 0;
	int channel = 0;
	int range = 0;
	int aref = 0;
	char filename[] = "/dev/comedi0";
	
	// we expect the channel number as the second argument
	if (argc < 2) {
		fprintf(stderr,"%s <channel>\n",argv[0]);
		exit(0);
	}

	// we open the comedi device
	device = comedi_open(filename);
	if(!device){
		comedi_perror(filename);
		exit(-1);
	}

	channel = atoi(argv[1]);

	// we read one sample
	ret = comedi_data_read(device, subdevice, channel, range, aref, &data);

	// any errors?
	if(ret < 0){
		comedi_perror(filename);
		exit(-1);
	}


	printf("raw ADC value=%lu\n", (unsigned long)data);
		
	// set overflow behavour: min and max data produce NAN
	comedi_set_global_oor_behavior(COMEDI_OOR_NAN);
	// get the range of the ADC converter in physical units
	range_info = comedi_get_range(device, subdevice, channel, range);
	// get the maximum value of the ADC converter
	// remember that the raw data is always unsigned data from 0 to maxdata
	maxdata = comedi_get_maxdata(device, subdevice, channel);
	// ... which is shown in this output
	printf("[0,%d] -> [%g,%g]\n", maxdata,
	       range_info->min, range_info->max);
	// we convert the raw data to physical units
	physical_value = comedi_to_phys(data, range_info, maxdata);
	// clipping? 0 or maxdata has been reached?
	if(isnan(physical_value)) {
		printf("Out of range [%g,%g]",
				range_info->min, range_info->max);
	} else {
		printf("physical value = %g", physical_value);
		// the physical data has units. Just now it's either volt or mA:
		switch(range_info->unit) {
		case UNIT_volt: printf(" V"); break;
		case UNIT_mA: printf(" mA"); break;
		case UNIT_none: break;
		default: printf(" (unknown unit %d)",
				range_info->unit);
		}
	}
	printf("\n");
	
	return 0;
}
