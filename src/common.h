#ifndef COMMON_H
#define COMMON_H

#include <QSemaphore>
#include <QVector>
#include <QThread>
#include <QUuid>
#include "myConstants.h"
#include <fftw3.h>
#include <omp.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <complex>
#include <sys/time.h>
////#include <xmmintrin.h> //m128
//#include <immintrin.h> //m256
////#include <zmmintrin.h> //m512
#include <x86intrin.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "DoubleBuffer.h"
#include "PacketChunker.h"
#include "fftwsequence.h"


#endif // COMMON_H
