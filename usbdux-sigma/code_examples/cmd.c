/*
 * Example of using commands - asynchronous input
 * Part of Comedilib
 *
 * Copyright (c) 1999,2000,2001 David A. Schleef <ds@schleef.org>
 *               2008 Bernd Porr <berndporr@f2s.com>
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */

/*
 * The program is used to test the usbdux sigma board
 */

#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern comedi_t *device;

struct parsed_options
{
	char *filename;
	double value;
	int subdevice;
	int channel;
	int aref;
	int range;
	int physical;
	int verbose;
	int n_chan;
	int n_scan;
	double freq;
};



#define BUFSZ 10000
char buf[BUFSZ];

#define N_CHANS 16
static unsigned int chanlist[N_CHANS];
static comedi_range * range_info[N_CHANS];
static lsampl_t maxdata[N_CHANS];


void do_cmd(comedi_t *dev,comedi_cmd *cmd);

void print_datum(lsampl_t raw, int channel_index, short physical);

char *cmdtest_messages[]={
	"success",
	"invalid source",
	"source conflict",
	"invalid argument",
	"argument conflict",
	"invalid chanlist",
};

int main(int argc, char *argv[])
{
	comedi_t *dev;
	comedi_cmd cmd;
	int ret;
	int total=0;
	int i;
	struct timeval start,end;
	int subdev_flags;
	lsampl_t raw;

	struct parsed_options options;

	/* The following variables used in this demo
	 * can be modified by command line
	 * options.  When modifying this demo, you may want to
	 * change them here. */
	options.filename = "/dev/comedi0";
	options.subdevice = 0;
	options.channel = 0;
	options.range = 0;
	options.aref = AREF_GROUND;
	options.n_chan = 16;
	options.n_scan = 10000;
	options.freq = 1000.0;

	/* open the device */
	dev = comedi_open(options.filename);
	if(!dev){
		comedi_perror(options.filename);
		exit(1);
	}

	// Print numbers for clipped inputs
	comedi_set_global_oor_behavior(COMEDI_OOR_NUMBER);

	/* Set up channel list */
	for(i = 0; i < options.n_chan; i++){
		chanlist[i] = CR_PACK(options.channel + i, options.range, options.aref);
		range_info[i] = comedi_get_range(dev, options.subdevice, options.channel, options.range);
		maxdata[i] = comedi_get_maxdata(dev, options.subdevice, options.channel);
	}

        ret = comedi_get_cmd_generic_timed(dev, options.subdevice, &cmd, options.n_chan, 1e9 / options.freq);
        if(ret<0){
                printf("comedi_get_cmd_generic_timed failed\n");
                return ret;
        }

	cmd.chanlist = chanlist;
        cmd.chanlist_len = options.n_chan;
        if(cmd.stop_src == TRIG_COUNT) cmd.stop_arg = options.n_scan;

	//prepare_cmd(dev, options.subdevice, options.n_scan, options.n_chan, 1e9 / options.freq, &cmd);

	/* comedi_command_test() tests a command to see if the
	 * trigger sources and arguments are valid for the subdevice.
	 * If a trigger source is invalid, it will be logically ANDed
	 * with valid values (trigger sources are actually bitmasks),
	 * which may or may not result in a valid trigger source.
	 * If an argument is invalid, it will be adjusted to the
	 * nearest valid value.  In this way, for many commands, you
	 * can test it multiple times until it passes.  Typically,
	 * if you can't get a valid command in two tests, the original
	 * command wasn't specified very well. */
	ret = comedi_command_test(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command_test");
		if(errno == EIO){
			fprintf(stderr,"Ummm... this subdevice doesn't support commands\n");
		}
		exit(1);
	}
	ret = comedi_command_test(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command_test");
		exit(1);
	}
	fprintf(stderr,"second test returned %d (%s)\n", ret,
			cmdtest_messages[ret]);
	if(ret!=0){
		fprintf(stderr, "Error preparing command\n");
		exit(1);
	}

	/* this is only for informational purposes */
	gettimeofday(&start, NULL);
	fprintf(stderr,"start time: %ld.%06ld\n", start.tv_sec, start.tv_usec);

	/* start the command */
	ret = comedi_command(dev, &cmd);
	if(ret < 0){
		comedi_perror("comedi_command");
		exit(1);
	}
	subdev_flags = comedi_get_subdevice_flags(dev, options.subdevice);
	while(1){
		ret = read(comedi_fileno(dev),buf,BUFSZ);
		if(ret < 0){
			/* some error occurred */
			perror("read");
			break;
		}else if(ret == 0){
			/* reached stop condition */
			break;
		}else{
			static int col = 0;
			int bytes_per_sample;
			total += ret;
			if(options.verbose)fprintf(stderr, "read %d %d\n", ret, total);
			if(subdev_flags & SDF_LSAMPL)
				bytes_per_sample = sizeof(lsampl_t);
			else
				bytes_per_sample = sizeof(sampl_t);
			for(i = 0; i < ret / bytes_per_sample; i++){
				if(subdev_flags & SDF_LSAMPL) {
					raw = ((lsampl_t *)buf)[i];
				} else {
					raw = ((sampl_t *)buf)[i];
				}
				print_datum(raw, col, options.physical);
				col++;
				if(col == options.n_chan){
					printf("\n");
					col=0;
				}
			}
		}
	}

	/* this is only for informational purposes */
	gettimeofday(&end,NULL);
	fprintf(stderr,"end time: %ld.%06ld\n", end.tv_sec, end.tv_usec);

	end.tv_sec -= start.tv_sec;
	if(end.tv_usec < start.tv_usec){
		end.tv_sec--;
		end.tv_usec += 1000000;
	}
	end.tv_usec -= start.tv_usec;
	fprintf(stderr,"time: %ld.%06ld\n", end.tv_sec, end.tv_usec);

	return 0;
}

void print_datum(lsampl_t raw, int channel_index, short physical) {
	double physical_value;
	if(!physical) {
		printf("%d ",raw);
	} else {
		physical_value = comedi_to_phys(raw, range_info[channel_index], maxdata[channel_index]);
		printf("%#8.6g ",physical_value);
	}
}
