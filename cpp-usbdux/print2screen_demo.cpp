#include "cpp-usbdux.h"
#include <stdio.h>

struct DUXCallback : CppUSBDUX::Callback {
	virtual void hasSample(const std::vector<float> &data) {
		for(auto &v:data) {
			printf("%f\t",v);
		}
		printf("\n");
	}
};

int main (int,char**) {
	CppUSBDUX usbdux;
	DUXCallback cb;
	try {
		usbdux.open();
		usbdux.start(&cb,8,250);
	} catch (const char* msg) {
		fprintf(stderr,"%s\n",msg);
		fprintf(stderr,"Check with 'sudo dmesg' if the USBDUX driver has been loaded.\n");
		exit(-1);
	}
	getc(stdin);
	usbdux.stop();
	return 0;
}
