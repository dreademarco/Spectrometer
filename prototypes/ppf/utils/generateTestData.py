from matplotlib import pyplot as plt
from scipy import signal
import numpy as np
import struct

nsamp = 65536 * 64

data = np.array(signal.chirp(np.array(range(nsamp)), 0, nsamp, 1, method='linear'), dtype=float)
fp = open("generated_data.dat", "wb")
fp.write(struct.pack('f' * nsamp, *data))
fp.close()
