#include "framedatacomplex.h"
using namespace std;

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
FrameDataComplex<T>::FrameDataComplex()
{
    data = NULL;
    frames = 0;
    frame_size = 0;
    fullSize = 0;
    temp_framedata = NULL;
    temp_frameelement = NULL;
}

template <typename T>
FrameDataComplex<T>::FrameDataComplex(int frames, int frame_size)
{
    this->frames = frames;
    this->frame_size = frame_size;
    this->fullSize = frame_size*2;
    data = new T(frames*frame_size*2);
    temp_framedata = new T(frame_size*2);
    temp_frameelement = new T(2);
}

template <typename T>
FrameDataComplex<T>::~FrameDataComplex()
{
    delete [] data;
    delete [] temp_framedata;
    delete [] temp_frameelement;
    frames = 0;
    frame_size = 0;
    fullSize = 0;
}

template <typename T>
int FrameDataComplex<T>::getFrameCount()
{
    return frames;
}

template <typename T>
int FrameDataComplex<T>::getFrameSize()
{
    return frame_size;
}

template <typename T>
int FrameDataComplex<T>::getFullFrameSize()
{
    return fullSize;
}

template <typename T>
T* FrameDataComplex<T>::getRawData()
{
    return data;
}

template <typename T>
int FrameDataComplex<T>::getLocation(int frame_index, int element_index) const
{
    return ((frame_index*frame_size*2)+element_index);
}

template <typename T>
int FrameDataComplex<T>::getRawLocation(int frame_index, int element_index) const
{
    return ((frame_index*fullSize)+element_index);
}

template <typename T>
T* FrameDataComplex<T>::at(int frame_index, int element_index)
{
    int offset = getLocation(frame_index,element_index);
    temp_frameelement[0] = data[offset];
    temp_frameelement[1] = data[offset+1];
    return temp_frameelement;
}

template <typename T>
T FrameDataComplex<T>::rawAt(int frame_index, int element_index)
{
    return data[getRawLocation(frame_index,element_index)];
}

template <typename T>
T* FrameDataComplex<T>::getFrame(int frame_index)
{
    for (int elementIdx = 0; elementIdx < fullSize; ++elementIdx) {
        temp_framedata[elementIdx] = rawAt(frame_index,elementIdx);
    }
    return temp_framedata;
}

template <typename T>
void FrameDataComplex<T>::loadMemCpy(FrameDataComplex<T> *sourceData){
    int cellsToCopy = frames*frame_size*2;
    memcpy(&this->data[0],&sourceData->data[0],cellsToCopy*sizeof(T));
}

template <typename T>
void FrameDataComplex<T>::pushFFTMemCpy(fftw_complex *destData){
    int cellsToCopy = frames*frame_size*2;
    memcpy(&destData[0],&this->data[0],cellsToCopy*sizeof(T));
}

template <typename T>
void FrameDataComplex<T>::toString(){
    for (int frameIdx = 0; frameIdx < frames; ++frameIdx) {
        for (int elementIdx = 0; elementIdx < frame_size; ++elementIdx) {
            cout << this->at(frameIdx,elementIdx)[0] << "+" << this->at(frameIdx,elementIdx)[1] << "i" << "\t";
        }
        cout << endl;
    }
}

template class FrameDataComplex<int>;
template class FrameDataComplex<float>;
template class FrameDataComplex<double>;
