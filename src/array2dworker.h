#ifndef ARRAY2DWORKER_H
#define ARRAY2DWORKER_H
#include <cstddef>
#include <iostream>
#include <stdio.h>


template <typename T>
class Array2DWorker
{
public:
    Array2DWorker();
    Array2DWorker(int rows, int columns);
    ~Array2DWorker();

    int getRowCount();
    int getColumnCount();
    T* getRawData();

    T at(int row, int column);
    T at(int row, int column) const;
    void set(int row, int column, T value);
    T* getRow(int rowIdx);
    T* getColumn(int colIdx);
    void setRow(int rowIdx, T* rowData);
    void setColumn(int colIdx, T* columnData);

    Array2DWorker<T> getRowSection(int offset, int length);
    Array2DWorker<T> getColumnSection(int offset, int length);
    Array2DWorker<T> transpose();
private:
    int columns;
    int rows;
    T *data;
    T *temp_rowdata;
    T *temp_coldata;

    int getLocation(int row, int column) const;
};

#endif // ARRAY2DWORKER_H
