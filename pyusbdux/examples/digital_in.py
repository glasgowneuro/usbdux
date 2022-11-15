#!/usr/bin/python3

import pyusbdux as dux

dux.open()

# Warning: no sampling rate defined here.
# It's as fast as possible just for testing.
while True:
    print(dux.digital_in(0))
