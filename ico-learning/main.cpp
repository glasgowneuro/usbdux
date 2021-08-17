#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <libv4l1-videodev.h>
#include <comedilib.h>
#include <signal.h>
#include <curses.h>

#include "icolearning.h"

#define LEARNING_RATE 0.00002

#define GAIN 40

#define DRIVE 400

#define BIASL 0
#define BIASR 0

#define SUBDEVICE 1
#define RANGE 0
#define AREF 0

#define CURRENT_THRES 2140

#define STEPS_BEFORE_TURN 2

#define STEPS_REV 5
#define STEPS_TURN 15
#define STEPS_AFTER_TURN 10

#define COMEDI_FILENAME "/dev/comedi0"

// Default input channel
#define INPUT 1

/* Default capture size */
#define WIDTH       96
#define HEIGHT      64

#define SCANLINE_REFLEX 50

#define SCANLINE_PRED   2

/* Default brightness and contrast value (0-65535) */
#define BRIGHTNESS  32767
#define CONTRAST    32767

#define THRES_GRAY 240

#define NORM    VIDEO_MODE_PAL

#define VIDEO_DEV "/dev/video0"

#define PALETTE VIDEO_PALETTE_RGB24
#define BPP 3

#define R_WEIGHT 1
#define G_WEIGHT 1
#define B_WEIGHT 1

#define MARGIN 2

#define SOBELX 6
#define SOBELY 6

#define SOBELLINE 1

#define NSOBEL 20

//#define WRITE_PPM

//////////////////////////////////////////////////////

#define MAKE_GRAY(r,g,b) ((((r)*R_WEIGHT+(g)*G_WEIGHT+(b)*B_WEIGHT)/(R_WEIGHT+G_WEIGHT+B_WEIGHT)))


struct video_capability vid_caps;
struct video_channel vid_chnl;
//struct video_picture vid_pict;
struct video_window vid_win;
struct video_mbuf vid_buf;
struct video_mmap vid_mmap;
int devfd=0;
unsigned char *map=NULL;
int mbufsize=0;
float zeroDphi=0.0F;
int steps_over_thres=0;
int rev=0;
int turn=0;
int after=STEPS_AFTER_TURN;
int dir=0;

comedi_t *comedidev=NULL;

Icolearning icolearning(2,5);

int running=1;

int thres(int t) {
	if (t>THRES_GRAY) {
		return t;
	} else {
		return 0;
	}
}

void writeppm(unsigned char *buffer,int step) {
	char tmp[80];
	sprintf(tmp,"video/%05d.pgm",step);
	FILE* f=fopen(tmp,"wb");
	if (!f) {
		fprintf(stderr,"Could not write to pgm\n");
		exit(1);
	}
	// binary greymap
	fprintf(f,"P5\n");
	// width x height
	fprintf(f,"%d %d\n",WIDTH,HEIGHT);
	// number of grey levels
	fprintf(f,"255\n");
	// binary data'll follow
	for(int y=0;y<HEIGHT;y++) {
		for(int x=0;x<WIDTH;x++) {
			int c=MAKE_GRAY(*(buffer++),*(buffer++),*(buffer++));
			fprintf(f,"%c",c);
		}
	}
	fclose(f);
}


inline lsampl_t limit_motor(float v) {
	if (v<=0) {
		mvaddstr(0,10,"underfl");
		return 0;
	}
	if (v>=4095) {
		mvaddstr(0,20,"overfl");
		return 4095;
	}
	return (lsampl_t)v;
}







float getDphi(unsigned char* buffer,int scanline) {
        int left=0;
        int right=0;
        int nleft=0;
        int nright=0;

        unsigned char* p=buffer+WIDTH*scanline*BPP+MARGIN*BPP;

        for (int i=MARGIN;i<WIDTH/2;i++) {
                int weight=(WIDTH/2-i);
                weight=weight*weight;
                left+=weight*thres(MAKE_GRAY(*(p++),*(p++),*(p++)));
                nleft++;
        }
        float leftavg=(float)left/(float)nleft/256.0/(float)WIDTH;

        for (int i=WIDTH/2;i<(WIDTH-MARGIN);i++) {
                int weight=(i-WIDTH/2);
                weight=weight*weight;
                right+=weight*thres(MAKE_GRAY(*(p++),*(p++),*(p++)));
                nright++;
        }
        float rightavg=(float)right/(float)nleft/256.0/(float)WIDTH;
        return (leftavg-rightavg);
}




void control(unsigned char* buffer, int step) {
	float dphi=getDphi(buffer,SCANLINE_REFLEX);
	float pred=getDphi(buffer,SCANLINE_PRED);
	char tmp[256];
	
	sprintf(tmp,"zerodphi=%f, dphi=%f, pred=%f",zeroDphi,dphi,pred);
	mvaddstr(3,2,tmp);

	dphi=dphi-zeroDphi;
	pred=pred-zeroDphi;

	// loerning
	icolearning.setInput(0,dphi*0.01);

	// fixme!
	icolearning.setInput(1,pred);

	icolearning.prediction(step);
	dphi=icolearning.getOutput();

	if (step%5==0) {
		icolearning.writeDocu(step);
	}

	sprintf(tmp,"dphi_motor=%f",dphi);
	mvaddstr(6,2,tmp);
	///////////////////////////////////////////////////////////////////////
	// obstacle avoidance
	lsampl_t current;
	int result=comedi_data_read(comedidev,0,7,0,0,&current);
	if (result<0) {
		comedi_perror("Could not read current\n");
        }

	sprintf(tmp,"Current: %d",current);
	mvaddstr(2,2,tmp);

	if (after) {
		after--;
	}
	if ((current>CURRENT_THRES)&&(!rev)&&(!turn)&&(!after)) {
		steps_over_thres++;
		if (steps_over_thres>STEPS_BEFORE_TURN) {
			rev=STEPS_REV;
		}
	} else {
	        steps_over_thres=0;
	}
	lsampl_t ldata;
	lsampl_t rdata;
	if (rev) {
	        ldata=2047-DRIVE+BIASL;
		rdata=2047-DRIVE+BIASR;
		rev--;
		int r=(int)(15.0*((float)random()/(float)RAND_MAX));
		if (rev==1) turn=(STEPS_TURN+r);
	} else {
		if (turn) {
			ldata=2047+BIASL;
			rdata=2047+BIASR;
			if (dir) {
				ldata=ldata-DRIVE;
				rdata=rdata+DRIVE;
			} else {
				rdata=rdata-DRIVE;
				ldata=ldata+DRIVE;
			}
			if (turn==1) {
				after=STEPS_AFTER_TURN;
				dir=!dir;
			}
			turn--;
		} else {
			ldata=limit_motor(2047+DRIVE-GAIN*dphi+BIASL);
			rdata=limit_motor(2047+DRIVE+GAIN*dphi+BIASR);
		}
	}

	///////////////////////////////////////
        int ret=comedi_data_write(comedidev,SUBDEVICE,0,RANGE,AREF,ldata);
        if(ret<0){
                comedi_perror("comedi_data_write");
                exit(0);
        }
        ret=comedi_data_write(comedidev,SUBDEVICE,1,RANGE,AREF,rdata);
        if(ret<0){
                comedi_perror("comedi_data_write");
                exit(0);
        }
	       //	if (!(step%10)) {
	       //		printf("%d %d\n",ldata,rdata);
	       //	}
}


int main(int argc, char **argv) { 
	icolearning.setReflex(0.01,0.51);
	icolearning.setPredictorsAsTraces(100);
	icolearning.openDocu("ico_");
	icolearning.setLearningRate(LEARNING_RATE);

        comedidev=comedi_open(COMEDI_FILENAME);
	if (!comedidev) {
		fprintf(stderr, "Can't open comedi\n");
		exit(-1);
	}
		

	/*
	 * open the video4linux device
	 */
	int max_try = 2;
	while ((devfd = open(VIDEO_DEV, O_RDWR)) == -1 && --max_try)
		sleep(1);
	if (devfd == -1) {
		fprintf(stderr, "Can't open device %s\n", VIDEO_DEV);
		exit(-1);
	}

	/*
	 * get capabilities of the device
	 */
	if (ioctl(devfd, VIDIOCGCAP, &vid_caps) == -1) {
		perror("ioctl (VIDIOCGCAP)");
		exit(-1);
	}

	int maxinputs = vid_caps.channels;

	/* print channel names */
        for (int i = 0; i < maxinputs; i++) {
		vid_chnl.channel = i;
		if (ioctl(devfd, VIDIOCGCHAN, &vid_chnl) == -1) {
                perror("ioctl (VIDIOCGCHAN)");
		exit(-1);
		}
		fprintf(stderr,"The name of input channel #%d is \"%s\".\n", i, vid_chnl.name);
        }

        fprintf(stderr,"Total %d input channel(s). Using channel #%d.\n", maxinputs, INPUT);
        fprintf(stderr,
		"Supported resolution range: %dx%d => %dx%d\n\n", vid_caps.minwidth,
		vid_caps.minheight, vid_caps.maxwidth, vid_caps.maxheight);


       	/*
	 * get default and then set new channel and norm
	 */
	vid_chnl.channel = INPUT;
	if (ioctl(devfd, VIDIOCGCHAN, &vid_chnl) == -1) {
		perror("ioctl (VIDIOCGCHAN)");
		exit(-2);
	}
	vid_chnl.channel = INPUT;
	vid_chnl.norm = NORM;
	if (ioctl(devfd, VIDIOCSCHAN, &vid_chnl) == -1) {
		perror("ioctl (VIDIOCSCHAN)");
		exit(-1);
	}

	/*
	 * check if drvier support mmap mode and do init
	 */
	mbufsize=0;
	if (ioctl(devfd, VIDIOCGMBUF, &vid_buf) == -1) {
		fprintf(stderr, "No mmap support\n");
		if (ioctl(devfd, VIDIOCGWIN, &vid_win) != -1) {
			vid_win.width = WIDTH;
			vid_win.height = HEIGHT;
			if (ioctl(devfd, VIDIOCSWIN, &vid_win) == -1) {
				perror("ioctl (VIDIOCSWIN)");
				exit(-2);
			}
		}
		exit(1);
	} else {
		mbufsize = vid_buf.size;
	}

	map = (unsigned char*) mmap(0,
				    mbufsize, 
				    PROT_READ | PROT_WRITE,
				    MAP_SHARED, 
				    devfd, 
				    0);

        if ((unsigned char *) -1 == (unsigned char *) map) {
		perror("mmap()");
		return -1;
        }

        vid_mmap.format = PALETTE;
        vid_mmap.frame = 0;
        vid_mmap.width = WIDTH;
        vid_mmap.height = HEIGHT;

	initscr();
	cbreak();
	noecho();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);
	nodelay(stdscr,TRUE);	

	
	mvaddstr(0,0,"Robo-mat");

	// calibraete
	if (ioctl(devfd, VIDIOCMCAPTURE, &vid_mmap) == -1) {
		perror("VIDIOCMCAPTURE");
		munmap(map, mbufsize);
		return -1;
	}
	vid_mmap.frame = 0;
	if (ioctl(devfd, VIDIOCSYNC, &vid_mmap) == -1) {
		perror("VIDIOCSYNC");
		munmap(map, mbufsize);
		return -1;
	}
	zeroDphi=getDphi(map+vid_buf.offsets[0],SCANLINE_REFLEX);
	fprintf(stderr,"zeroDphi=%f\n",zeroDphi);

	if (ioctl(devfd, VIDIOCMCAPTURE, &vid_mmap) == -1) {
		perror("VIDIOCMCAPTURE");
		munmap(map, mbufsize);
		return -1;
	}
	int step=0;
	int key=0;
	/* looped grabbing, differencing and displaying */ 
	while (key != 27) {
		key = wgetch (stdscr);
		vid_mmap.frame = 1;
		if (ioctl(devfd, VIDIOCMCAPTURE, &vid_mmap) == -1) {
			perror("VIDIOCMCAPTURE");
			munmap(map, mbufsize);
			return -1;
		}
		vid_mmap.frame = 0;
		if (ioctl(devfd, VIDIOCSYNC, &vid_mmap) == -1) {
			perror("VIDIOCSYNC");
			munmap(map, mbufsize);
			return -1;
		}
		//printf("0");
		fflush(stdout);
#ifdef WRITE_PPM
		writeppm(map+vid_buf.offsets[0],step);
#endif
		control(map+vid_buf.offsets[0],step);
		step++;
		vid_mmap.frame = 0;
		if (ioctl(devfd, VIDIOCMCAPTURE, &vid_mmap) == -1) {
			perror("VIDIOCMCAPTURE");
			munmap(map, mbufsize);
			return -1;
		}
		vid_mmap.frame = 1;
		if (ioctl(devfd, VIDIOCSYNC, &vid_mmap) == -1) {
			perror("VIDIOCSYNC");
			munmap(map, mbufsize);
			return -1;
		}
		//printf("1");
		fflush(stdout);
#ifdef WRITE_PPM
		writeppm(map+vid_buf.offsets[0],step);
#endif
		control(map+vid_buf.offsets[0],step);
		step++;
	}
	vid_mmap.frame = 0;
	if (ioctl(devfd, VIDIOCSYNC, &vid_mmap) == -1) {
		perror("VIDIOCSYNC");
	}
	vid_mmap.frame = 1;
	if (ioctl(devfd, VIDIOCSYNC, &vid_mmap) == -1) {
		perror("VIDIOCSYNC");
	}
	munmap(map,mbufsize);
	endwin();
	close(devfd);
	comedi_data_write(comedidev,SUBDEVICE,0,RANGE,AREF,2047);
	comedi_data_write(comedidev,SUBDEVICE,1,RANGE,AREF,2047);
      	comedi_close(comedidev);
}
