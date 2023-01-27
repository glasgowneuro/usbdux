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
	usbdux.open();
	usbdux.start(&cb,8,250);
	getc(stdin);
	usbdux.stop();
}
