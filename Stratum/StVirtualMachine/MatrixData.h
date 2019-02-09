#ifndef MATRIXDATA_H
#define MATRIXDATA_H

#include <QVector>

namespace StVirtualMachine {

template<typename T>
class MatrixData
{
public:
    MatrixData(int rowCount, int columnCount);
    MatrixData(const MatrixData<T>& data);
    void operator = (const MatrixData<T>& data);

    int rowCount() const;
    int columnCount() const;

    T get(int rowIndex, int columnIndex) const;
    void set(int rowIndex, int columnIndex, const T& value);

    QVector<T> getColumn(int columnIndex) const;
    void setColumn(int columnIndex, const QVector<T>& column);

    QVector<T> getRow(int rowIndex) const;
    void setRow(int rowIndex, const QVector<T>& row);

    void fill(const T& value);

private:
    QVector<QVector<T>> m_data;
    int m_rowCount, m_columnCount;
};



template<typename T>
MatrixData<T>::MatrixData(int rows, int columns) :
    m_rowCount(rows),
    m_columnCount(columns)
{
    m_data = QVector<QVector<T>>(rows, QVector<T>(columns, 0));
}

template<typename T>
MatrixData<T>::MatrixData(const MatrixData<T>& data)
{
    *this = data;
}

template<typename T>
void MatrixData<T>::operator = (const MatrixData<T>& data)
{
    m_rowCount = data.rowCount();
    m_columnCount = data.columnCount();

    m_data.resize(m_rowCount);
    for(int i = 0; i < m_rowCount; i++)
        m_data[i] = data.getRow(i);
}

template<typename T>
int MatrixData<T>::rowCount() const
{
    return m_rowCount;
}

template<typename T>
int MatrixData<T>::columnCount() const
{
    return m_columnCount;
}

template<typename T>
T MatrixData<T>::get(int row, int columnIndex) const
{
    return m_data[row][columnIndex];
}

template<typename T>
void MatrixData<T>::set(int rowIndex, int columnIndex, const T& value)
{
    m_data[rowIndex][columnIndex] = value;
}

template<typename T>
QVector<T> MatrixData<T>::getColumn(int columnIndex) const
{
    QVector<T> column = QVector<T>(m_data.count());
    for(int i = 0;  i < column.count(); i++)
        column[i] = m_data[i][columnIndex];

    return column;
}

template<typename T>
void MatrixData<T>::setColumn(int columnIndex, const QVector<T>& column)
{
    for(int i = 0;  i < column.count(); i++)
        m_data[i][columnIndex] = column[i];
}

template<typename T>
QVector<T> MatrixData<T>::getRow(int rowIndex) const
{
    return m_data[rowIndex];
}

template<typename T>
void MatrixData<T>::setRow(int rowIndex, const QVector<T>& row)
{
    m_data[rowIndex] = row;
//    for(int i = 0;  i < row.count(); i++)
//        m_data[rowIndex][i] = row[i];
}

template<typename T>
void MatrixData<T>::fill(const T& value)
{
    for(QVector<T>& row : m_data)
        row.fill(value);
}

}

#endif // MATRIXDATA_H
