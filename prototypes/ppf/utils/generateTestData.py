from matplotlib import pyplot as plt
from scipy import signal
import numpy as np
import struct

nsamp = 65536 * 64


data = signal.chirp(np.array(range(nsamp)), 0, nsamp, 100, method='linear')
data = np.array(data, dtype=complex)
fp = open('generated_data.data','wb')
#fp.write(struct.pack('f'*nsamp, *data), dtype=complex)
fp.write(bytearray(data))
fp.close()


#plt.plot(data)
#plt.show()
