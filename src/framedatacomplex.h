#ifndef FRAMEDATACOMPLEX_H
#define FRAMEDATACOMPLEX_H
#include <framedata.h>
#include <fftw3.h>

/*
 * Represtents a (complex) 2D Array in relation to data frames. Logically, the array is stored as a 1D vector for speed.
 * The order of data input is interpreted as follows:
 *
 * [ | El.1R | El.1C | El.2R | El.2C | El.3R | El.3C | .. | El.NR | El.NC || El.1R | El.1C | El.2R | El.2C | El.3R | El.3C | .. | El.NR | El.NC|| ]
 *
 * Therefore, all elements for a single frame are contiguous, and each element is split over 2 cells
 * covering the real and imaginary parts, followed by the elements for the next frame etc.
 *
 * A "row" represents the elements of an entire frame. A "column" represents a particular element (real or complex part) for all the frames.
 */

template <typename T>
class FrameDataComplex
{
public:
    FrameDataComplex();
    FrameDataComplex(int frames, int frame_size);
    ~FrameDataComplex();

    int getFrameCount();
    int getFrameSize();
    int getFullFrameSize();
    T* getRawData();

    T* at(int frame_index, int element_index);
    T* at(int frame_index, int element_index) const;
    void set(int frame_index, int element_index, T* value);
    T* getFrame(int frame_index);
    void setFrame(int frame_index, T* frameData);

    void loadMemCpy(FrameDataComplex<T> *sourceData);
    void pushFFTMemCpy(fftw_complex *destData);
    void toString();

    T *data;

private:
    int frames;
    int frame_size;
    int fullSize;
    int getLocation(int frame_index, int element_index) const;
    int getRawLocation(int frame_index, int element_index) const;
    T rawAt(int frame_index, int element_index);
    T *temp_framedata;
    T *temp_frameelement;

};

#endif // FRAMEDATACOMPLEX_H
