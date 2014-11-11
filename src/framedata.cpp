#include "framedata.h"
using namespace std;

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
FrameData<T>::FrameData()
{
    data = NULL;
    frames = 0;
    frame_size = 0;
    temp_framedata = NULL;
}

template <typename T>
FrameData<T>::FrameData(int frames, int frame_size)
{
    this->frames = frames;
    this->frame_size = frame_size;
    data = new T[frames*frame_size];
    temp_framedata = new T[frame_size];
}

template <typename T>
FrameData<T>::~FrameData()
{
    delete [] data;
    delete [] temp_framedata;
    frames = 0;
    frame_size = 0;
}

template <typename T>
int FrameData<T>::getFrameCount()
{
    return frames;
}

template <typename T>
int FrameData<T>::getFrameSize()
{
    return frame_size;
}

template <typename T>
T* FrameData<T>::getRawData()
{
    return data;
}

template <typename T>
int FrameData<T>::getLocation(int frame_index, int element_index) const
{
    return ((frame_index*frame_size)+element_index);
}

template <typename T>
T FrameData<T>::at(int frame_index, int element_index)
{
    return data[getLocation(frame_index,element_index)];
}

template <typename T>
T* FrameData<T>::getFrame(int frame_index)
{
    for (int elementIdx = 0; elementIdx < frame_size; ++elementIdx) {
        temp_framedata[elementIdx] = at(frame_index,elementIdx);
    }
    return temp_framedata;
}

template <typename T>
void FrameData<T>::loadMemCpy(FrameData<T> *sourceData){
    int cellsToCopy = frames*frame_size;
    memcpy(&this->data[0],&sourceData->data[0],cellsToCopy*sizeof(T));
}

template <typename T>
void FrameData<T>::pushFFTMemCpy(T *destData){
    int cellsToCopy = frames*frame_size;
    memcpy(&destData[0],&this->data[0],cellsToCopy*sizeof(T));
}

template <typename T>
void FrameData<T>::toString(){
    for (int frameIdx = 0; frameIdx < frames; ++frameIdx) {
        for (int elementIdx = 0; elementIdx < frame_size; ++elementIdx) {
            cout << this->at(frameIdx,elementIdx) << "\t";
        }
        cout << endl;
    }
}

template class FrameData<int>;
template class FrameData<float>;
template class FrameData<double>;
