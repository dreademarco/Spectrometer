#include "array2dworker.h"
using namespace std;

template <typename T>
Array2DWorker<T>::Array2DWorker()
{
    data = NULL;
    rows = 0;
    columns = 0;
    temp_rowdata = NULL;
    temp_coldata = NULL;
}

template <typename T>
Array2DWorker<T>::Array2DWorker(int rows, int columns)
{
    this->rows = rows;
    this->columns = columns;
    data = new T[rows*columns];
    temp_rowdata = new T[columns];
    temp_coldata = new T[rows];
}

template <typename T>
Array2DWorker<T>::~Array2DWorker(){
    delete [] data;
    delete [] temp_rowdata;
    delete [] temp_coldata;
    rows = 0;
    columns = 0;
}

template <typename T>
int Array2DWorker<T>::getRowCount()
{
    return rows;
}

template <typename T>
int Array2DWorker<T>::getColumnCount()
{
    return columns;
}

template <typename T>
T* Array2DWorker<T>::getRawData(){
    return data;
}

template <typename T>
int Array2DWorker<T>::getLocation(int row, int column) const
{
    return ((row*columns)+column);
}

template <typename T>
T Array2DWorker<T>::at(int row, int column)
{
    return data[getLocation(row,column)];
}

template <typename T>
T Array2DWorker<T>::at(int row, int column) const
{
    return data[getLocation(row,column)];
}

template <typename T>
void Array2DWorker<T>::set(int row, int column, T value)
{
    data[getLocation(row,column)] = value;
}

template <typename T>
T* Array2DWorker<T>::getRow(int rowIdx)
{
    //T temp_data[columns];
    //T* temp_data = new T[columns];
    for (int i = 0; i < columns; ++i) {
        temp_rowdata[i] = at(rowIdx,i);
    }
    return temp_rowdata;
}

template <typename T>
T* Array2DWorker<T>::getColumn(int colIdx)
{
    //T* temp_data = new T[rows];
    //T temp_data[rows];
    for (int i = 0; i < rows; ++i) {
        temp_coldata[i] = at(i,colIdx);
    }
    return temp_coldata;
}

template <typename T>
void Array2DWorker<T>::setRow(int rowIdx, T* rowData)
{
    for (int i = 0; i < columns; ++i) {
        set(rowIdx,i,rowData[i]);
    }
}

template <typename T>
void Array2DWorker<T>::setColumn(int colIdx, T* columnData)
{
    for (int i = 0; i < rows; ++i) {
        set(i,colIdx,columnData[i]);
    }
}

template <typename T>
Array2DWorker<T> Array2DWorker<T>::getRowSection(int offset, int length)
{
    Array2DWorker<T> rowSection(length,columns);
    for (int i = 0; i < length; ++i) {
        rowSection.setRow(i,getRow(offset+i));
    }
    return rowSection;
}

template <typename T>
Array2DWorker<T> Array2DWorker<T>::getColumnSection(int offset, int length){
    Array2DWorker<T> columnSection(rows,length);
    for (int i = 0; i < length; ++i) {
        columnSection.setColumn(i,getColumn(offset+i));
    }
    return columnSection;
}

template <typename T>
Array2DWorker<T> Array2DWorker<T>::transpose(){
    Array2DWorker<T> transposed(columns,rows);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            transposed.set(j,i,at(i,j));
        }
    }
    return transposed;
}

template class Array2DWorker<int>;
template class Array2DWorker<float>;
template class Array2DWorker<double>;
