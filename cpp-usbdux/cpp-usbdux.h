#ifndef _CPP_USBDUX_H
#define _CPP_USBDUX_H

#include <stdio.h>
#include <comedilib.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <thread>	
#include <array>
#include <fcntl.h>
#include <math.h>

#define N_CHANS 16
#define BUFSZ N_CHANS*sizeof(long int)

const char errorDevNotOpen[] = "Comedi device not open. Use open() first.";
const char errorDisconnect[] = "Device error. Possible disconnect.";

class CppUSBDUX {
public:

	/**
	 * Abstract Callback which needs to be implemented by the main program
	 **/
	struct Callback{
		virtual void hasSample(const std::array<float,N_CHANS> &data) = 0;
	};

	/**
	 * Opens a specific comedi device with device number /dev/comedi%d
	 **/
	void open(int comediDeviceNumber) {
		char filename[256];
		sprintf(filename,"/dev/comedi%d",comediDeviceNumber);
		
		/* open the device */
		dev = comedi_open(filename);
		if(!dev){
			throw "Could not open comedi device.";
		}
		if (strstr(comedi_get_board_name(dev),"usbdux") == NULL) {
			throw "Not a USBDUX board.";
		}
		memset(&cmd,0,sizeof(comedi_cmd));
	}


	/**
	 * Searches for a usbdux device and opens it
	 **/
	void open() {
		for(int i = 0;i<16;i++) {
			try {
				open(i);
				return;
			} catch (const char*) {
			}
		}
		throw "No USBDUX board found.";
	}

	/**
	 * Starts data acquisition and the Callback is
	 * called with the number of _nchannels at the sampling
	 * rate fs.
	 **/
	void start(Callback* cb, int n_channels, double fs) {
		if (dev == NULL) throw errorDevNotOpen;
		
		n_chan = n_channels;
		freq = fs;
		
		for(int i=0;i<N_CHANS;i++) {
			samples[i] = 0;
		}
		
		subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AI,0);
		
		// Print numbers for clipped inputs
		comedi_set_global_oor_behavior(COMEDI_OOR_NUMBER);
		
		/* Set up channel list */
		for(int i = 0; i < n_chan; i++){
			chanlist[i] = CR_PACK(i, range, aref);
			range_info[i] = comedi_get_range(dev, subdevice, 0, range);
			maxdata[i] = comedi_get_maxdata(dev, subdevice, 0);
		}
		
		subdev_flags = comedi_get_subdevice_flags(dev,subdevice);
		if(subdev_flags & SDF_LSAMPL) {
			bytes_per_sample = sizeof(lsampl_t);
		} else {
			bytes_per_sample = sizeof(sampl_t);
		}
		
		memset(&cmd,0,sizeof(comedi_cmd));
		int ret = comedi_get_cmd_generic_timed(dev, subdevice, &cmd, n_chan, 1e9 / freq);
		if(ret<0){
			throw "Failed to generate async timing information"
				"based on sampling rate and number of channels.";
		}
		
		cmd.chanlist = chanlist;
		cmd.chanlist_len = n_chan;
		cmd.stop_src = TRIG_NONE;
		cmd.stop_arg = 0;
		cmd.scan_end_arg = n_chan;
		
		// twice!
		ret = comedi_command_test(dev, &cmd);
		ret = comedi_command_test(dev, &cmd);
		if(ret!=0){
			throw "Error preparing async analogue in. Check sampling rate / number of channels.";
		}
		
		/* start the command */
		ret = comedi_command(dev, &cmd);
		if(ret < 0){
			throw "Async data acquisition could not be started.";
		}
		thr = std::thread([this](Callback* cb){readWorker(cb);},cb);
	}

	
	/**
	 * Starts the usbdux at a sampling rate of 1kHz
	 **/
	void start(Callback* cb, int nChan) {
		start(cb, nChan, 1000);
	}

	/**
	 * Gets the actual sampling rate
	 **/
	float getSamplingRate() {
		float sampling_rate = 0;
		// the timing is done channel by channel
		// this means that the actual sampling rate is divided by
		// number of channels
		if ((cmd.convert_src ==  TRIG_TIMER)&&(cmd.convert_arg)) {
			sampling_rate=((1E9 / cmd.convert_arg)/n_chan);
		}
		
		// the timing is done scan by scan (all channels at once)
		// the sampling rate is equivalent of the scan_begin_arg
		if ((cmd.scan_begin_src ==  TRIG_TIMER)&&(cmd.scan_begin_arg)) {
			sampling_rate=1E9 / cmd.scan_begin_arg;
		}
		return sampling_rate;
	}
	

	/**
	 * Stops the background acquisition
	 **/
	void stop() {
		if (dev == NULL) throw errorDevNotOpen;
		if (!running) return;
		running = false;
		thr.join();
		comedi_cancel(dev,subdevice);
		memset(&cmd,0,sizeof(comedi_cmd));
	}

	
	/**
	 * Changes value at a digital port
	 **/
	void digital_out(int channel, int value) {
		if (dev == NULL) throw errorDevNotOpen;
		int subdevice = 2;
		int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_OUTPUT);
		if(ret < 0){
			throw errorDisconnect;
		}
		ret = comedi_dio_write(dev,subdevice,channel,value);
		if (ret < 0) {
			throw errorDisconnect;
		}
	}
	
	
	/**
	 * Gets the status of a digital input port
	 **/
	int digital_in(int channel) {
		if (dev == NULL) throw errorDevNotOpen;
		int subdevice = 2;
		int ret = comedi_dio_config(dev,subdevice,channel,COMEDI_INPUT);
		if (ret < 0) throw errorDisconnect;
		unsigned int value;
		ret = comedi_dio_read(dev,subdevice,channel,&value);
		if (ret < 0) throw errorDisconnect;
		return (int)value;
	}


	/**
	 * Sets the value of an analogue output channel between 0..1
	 **/
	void analogue_out(int channel, float value) {
		if (dev == NULL) throw errorDevNotOpen;
		int subdevice = comedi_find_subdevice_by_type(dev,COMEDI_SUBD_AO,0);
		int max = comedi_get_maxdata(dev, subdevice, 0);
		int ret = comedi_data_write(dev,subdevice,channel,0,0, (int)round(value*max));
		if (ret<0) throw errorDisconnect;
	}

	/**
	 * Gets the boardname
	 **/
	const char* get_board_name() {
		if (dev == NULL) throw errorDevNotOpen;
		return comedi_get_board_name(dev);
	}


	/**
	 * Closes the comedi device
	 **/
	void close() {
		if (dev == NULL) throw errorDevNotOpen;
		comedi_close(dev);
		dev = NULL;
		memset(&cmd,0,sizeof(comedi_cmd));
	}

	~CppUSBDUX() {
		close();
	}


private:
	int getSampleFromBuffer(float sample[N_CHANS]) {
		if (dev == NULL) throw errorDevNotOpen;
		fd_set rdset;
		FD_ZERO(&rdset);
		FD_SET(comedi_fileno(dev),&rdset);
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;
		int ret = select(comedi_fileno(dev)+1,&rdset,NULL,NULL,&timeout);
		if (ret < 0) return ret;
		ret = read(comedi_fileno(dev),buffer,bytes_per_sample * n_chan);
		if (ret == 0) {
			return ret;
		}
		if (ret < 0) {
			return ret;
		} else if (ret > 0) {
			int subdev_flags = comedi_get_subdevice_flags(dev,subdevice);
			for(int i = 0; i < n_chan; i++) {
				int raw;
				if(subdev_flags & SDF_LSAMPL) {
					raw = ((lsampl_t *)buffer)[i];
				} else {
					raw = ((sampl_t *)buffer)[i];
				}
				sample[i] = comedi_to_phys(raw,
							   range_info[i],
							   maxdata[i]);
			}
		}
		return 1;
	}
	

	void readWorker(Callback* cb) {
		std::array<float,N_CHANS> sample;
		running = true;
		while (running) {
			float tmp[N_CHANS];
			int n = getSampleFromBuffer(tmp);
			if (n > 0) {
				for(int i = 0; i < N_CHANS; i++) {
					sample[i] = tmp[i];
				}
				//printf("%f\n",sample[0]);
				cb->hasSample(sample);
			}
			if (n < 0) {
				throw errorDisconnect;
			}
		}
	}

	
private:
	comedi_t *dev = NULL;
	int range = 0;
	int subdevice = 0;
	int aref  = AREF_GROUND;
	int n_chan = 0;
	double freq = 0;
	int bytes_per_sample = 0;
	int subdev_flags = 0;
	
	char buffer[BUFSZ];
	float samples[N_CHANS];
	
	unsigned int chanlist[N_CHANS];
	comedi_range* range_info[N_CHANS];
	lsampl_t maxdata[N_CHANS];
	comedi_cmd cmd;
	std::thread thr;
	bool running = false;

};

#endif
