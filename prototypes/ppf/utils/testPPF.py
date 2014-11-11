from matplotlib import pyplot as plt
from sys import argv
import numpy as np
import struct
import itertools
from math import *

nchans = 16384

def plot_input():
    # Load file
    # Assumption: data types is single precision complex (real first)
    fp = open(filename, 'rb')
    data = fp.read()
    data = struct.unpack('f' * (len(data) / 4), data)

    real_data = []
    for i in range(len(data) / 2):
        real_data.append(data[i*2])

    plt.plot(real_data)
    plt.show()

def plot_output():
    # Load file
    # Assumption: data types is single precision complex (real first)
    fp = open(filename, 'rb')
    data = fp.read()
    data = struct.unpack('f' * (len(data) / 4), data)
    data = zip(*(iter(data),) * 2)
    print data    
    data = np.array([sqrt(i[0] * i[0] + i[1] * i[1]) for i in data])
    data = np.reshape(data, (len(data) / nchans, nchans))
    plt.imshow(data.T, interpolation='nearest', aspect='auto')
    plt.colorbar()
    plt.show()


# Check if filename is in command line arguments
if len(argv) < 2:
    print "Filename requried"
filename = argv[1]

plot_output()




