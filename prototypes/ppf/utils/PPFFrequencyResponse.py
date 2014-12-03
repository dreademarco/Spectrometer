from matplotlib import pyplot as plt
from math import sin, sqrt
import numpy as np
import os, sys, re
import subprocess
import struct
import pickle

if __name__ == "__main__":

    # Define global for run
    nthreads = 2
    nblocks  = 8
    bw       = 1024
    tobs     = 10
    ntaps    = 4
    nchans   = 16
    nfreq_points = (nchans * 21)
    freq_step = bw / float(nfreq_points)

    # Paths
    wdir        = "/home/andrea/Documents/ISSA/debug/"
    ppf         = os.path.join(wdir, "ProducerConsumer3")
    input_file  = os.path.join(wdir, "input_file.dat") 
    output_file = os.path.join(wdir, "output_file.dat")

    # Update environment
    #os.environ['LD_LIBRARY_PATH'] = os.environ['LD_LIBRARY_PATH'] + ":/usr/local/qwt-6.1.1/lib"

    # Change cwd
    os.chdir(wdir)

    # Loop over number of input signals
    counter = 0

    #nfreqs = nchans
    #freq_step = 0.05
    ## Create output 
    #output = np.zeros((nchans, nfreqs))
    #freq_range = np.arange(0, nfreqs * freq_step, freq_step)
    
    output = np.zeros((nchans, nfreq_points))


    freq_range = np.arange(sys.float_info.epsilon, bw, freq_step) #start from freq_step
    #freq_range = np.arange(freq_step, freq_step*2, freq_step)
    for i, freq in enumerate(freq_range):
        # Generate input signals
	    #print i, freq
        signal = np.sin(np.arange(1, tobs * bw, 1) * 2 * np.pi * freq / bw)

        # Save signal to file
        f = open(input_file, 'wb')
        f.write(struct.pack('f' * len(signal), *signal))
        f.close()

        # Run ppf
        p = subprocess.Popen([ ppf, "-ntaps", str(ntaps), "-nchans", str(nchans),
                              "-nblocks", str(nblocks), "-srate", str(bw),
                              "-tobs", str(tobs), "-nthreads", str(nthreads)],  stdout=subprocess.PIPE)
        out, err = p.communicate()
	#print out


        # Check for errors
        if out.find('PPF') < 0:
            # Error detected, break from inner loop
            print "KABOOM"
            break

        # Hopefully all OK, read output file
        f = open(output_file, "rb")
        data = f.read()
        data = struct.unpack('f' * (len(data) / 4), data)
        data = zip(*(iter(data),) * 2)
        f.close()    

        # Calculate power and reshape
        data = np.array([sqrt(val[0] * val[0] + val[1] * val[1]) for val in data])  
        data = np.reshape(data, (len(data) / nchans, nchans))
	#print data.shape

        # Add result to output
        output[:,i] = data[ntaps + 1,:]

        # Output ghax inkella jitilghu
        counter = counter + 1
        if (counter % 10 == 0):
            print "Processed iteration %d" % counter

    # Convert to decibels
    #output = 10 * np.log10(output / np.max(output))

    print "All done MOFO"
    output = 10 * np.log10(output / np.max(output))
    for i in range(nchans):
        plt.plot(freq_range, output[i,:])
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Power (dB)")
    plt.title("Channel frequency responce")
    plt.ylim([-70,5])
    plt.show()


#    sum_output = np.sum(output, axis=0)
#    plt.plot(freq_range, 10 * np.log10(sum_output/np.max(sum_output)))
#    plt.xlabel("Frequency (Hz)")
#    plt.ylabel("Power (dB)")
#    plt.title("Channel frequency responce")
#    plt.ylim([-40,5])
#    plt.show()


#        # TESTING: Plot 
#        plt.imshow(data.T, interpolation='nearest', aspect='auto')
#        plt.colorbar()
#        #plt.clim(0,300)

