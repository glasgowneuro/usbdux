#!/usr/bin/python3

import pyusbdux as dux

class DataCallback(dux.Callback):

    def hasSample(self,s):
        print("s:",s)

cb = DataCallback()

# open default USBDUX
dux.open()

# sampling from 8 channels
# at a sampling rate of 250Hz.
dux.start(cb,8,250)

# Just idles till the user presses return.
input()

# Stops the data acquisition.
dux.stop()
