#!/usr/bin/python3
"""
Plots channels zero and one at 100Hz. Requires pyqtgraph.

Copyright (c) 2018-2022, Bernd Porr <mail@berndporr.me.uk>
see LICENSE file.

"""

import sys

import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtWidgets
import numpy as np
import pyusbdux as c

samplingRate = 250

class QtPanningPlot:

    def __init__(self,title):
        self.pw = pg.PlotWidget()
        self.pw.setYRange(-1.5,1.5)
        self.pw.setXRange(0,500/samplingRate)
        self.plt = self.pw.plot()
        self.data = []
        # any additional initalisation code goes here (filters etc)
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)

    def getWidget(self):
        return self.pw
        
    def update(self):
        self.data=self.data[-500:]
        if self.data:
            self.plt.setData(x=np.linspace(0,len(self.data)/samplingRate,len(self.data)),y=self.data)

    def addData(self,d):
        self.data.append(d)

app = pg.mkQApp()
mw = QtWidgets.QMainWindow()
mw.setWindowTitle("{}Hz dual PlotWidget".format(samplingRate))
mw.resize(800,400)
cw = QtWidgets.QWidget()
mw.setCentralWidget(cw)

# Let's arrange the two plots horizontally
layout = QtWidgets.QHBoxLayout()
cw.setLayout(layout)

# Let's create two instances of plot windows
qtPanningPlot1 = QtPanningPlot("DUX 1st channel")
layout.addWidget(qtPanningPlot1.getWidget())

qtPanningPlot2 = QtPanningPlot("DUX 2nd channel")
layout.addWidget(qtPanningPlot2.getWidget())

class DataCallback(c.Callback):
    def hasSample(self,s):
        qtPanningPlot1.addData(s[0])
        qtPanningPlot2.addData(s[1])

cb = DataCallback()
c.open()
print("ADC board:",c.get_board_name())
c.start(cb,8,samplingRate)
print("Actual samplingrate =",c.getSamplingRate(),"Hz")

# showing the plots
mw.show()

# Starting the QT GUI
# This is a blocking call and only returns when the user closes the window.
pg.exec()

c.stop()
c.close()

print("finished")
