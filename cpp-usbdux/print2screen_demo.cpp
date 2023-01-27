#include "cpp-usbdux.h"
#include <stdio.h>

struct DUXCallback : CppUSBDUX::Callback {
	virtual void hasSample(const std::array<float,N_CHANS> &data) {
		printf("%f\n",data[0]);
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
