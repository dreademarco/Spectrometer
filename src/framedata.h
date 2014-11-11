#ifndef FRAMEDATA_H
#define FRAMEDATA_H
#include <cstddef>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <omp.h>

/*
 * Represtents a (real) 2D Array in relation to data frames. Logically, the array is stored as a 1D vector for speed.
 * The order of data input is interpreted as follows:
 *
 * [ | El.1 | El.2 | El.3 | El.4 | El.5 | .. | El.N || El.1 | El.2 | El.3 | El.4 | El.5 | .. | El.N || ]
 *
 * Therefore, all elements for a single frame are contiguous, followed by the elements for the next frame etc.
 *
 * A "row" represents the elements of an entire frame. A "column" represents a particular element for all the frames.
 */

template <typename T>
class FrameData
{
public:
    FrameData();
    FrameData(int frames, int frame_size);
    ~FrameData();

    int getFrameCount();
    int getFrameSize();
    T* getRawData();

    T at(int frame_index, int element_index);
    T at(int frame_index, int element_index) const;
    void set(int frame_index, int element_index, T value);
    T* getFrame(int frame_index);
    void setFrame(int frame_index, T* frameData);

    void loadMemCpy(FrameData<T> *sourceData);
    void pushFFTMemCpy(T *destData);
    void toString();

    T *data;

private:
    int frames;
    int frame_size;
    int getLocation(int frame_index, int element_index) const;
    T *temp_framedata;
};

#endif // FRAMEDATA_H
