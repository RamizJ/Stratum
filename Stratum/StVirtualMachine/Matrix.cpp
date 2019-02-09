#include "Matrix.h"
#include "VmLog.h"
#include "VmGlobal.h"

#include <Class.h>
#include <Object.h>
#include <QFile>
#include <Sc2000DataStream.h>

using namespace StData;

namespace StVirtualMachine {

Matrix::Matrix() :
    m_minRow(0), m_minColumn(0),
    m_type(0),
    m_data(0, 0),
    m_project(nullptr)
{}

Matrix::Matrix(int handle, int minRow, int maxRow, int minColumn, int maxColumn, qint16 type) :
    HandleItem(handle),
    m_minRow(minRow), m_minColumn(minColumn),
    m_type(type),
    m_data(maxRow - minRow + 1, maxColumn - minColumn + 1),
    m_project(nullptr)
{}

Matrix::Matrix(const Matrix& m):
    HandleItem(m),
    m_minRow(m.minRow()),
    m_minColumn(m.minColumn()),
    m_type(m.type()),
    m_data(m.data()),
    m_project(m.project())
{
    setHandle(m.handle());
}

void Matrix::operator =(const Matrix& m)
{
    m_minRow = m.minRow();
    m_minColumn = m.minColumn();
    m_type = m.type();
    m_data = m.data();
    m_project = m.project();
    setHandle(m.handle());
}

double Matrix::get(int row, int column) const
{
    double value = m_data.get(row - m_minRow, column - m_minColumn);
    return value;
}

double Matrix::getCell(int row, int column) const
{
    return m_data.get(row, column);
}

void Matrix::put(int row, int column, double value)
{
    m_data.set(row - m_minRow, column - m_minColumn, value);
}

void Matrix::putCell(int row, int column, double value)
{
    m_data.set(row, column, value);
}

void Matrix::fill(double value)
{
    m_data.fill(value);
}

void Matrix::transp()
{
    MatrixData<double> data(m_data.columnCount(), m_data.rowCount());

    for(int i = 0; i < m_data.rowCount(); i++)
        for(int j = 0; j < m_data.columnCount(); j++)
            data.set(j, i, m_data.get(i, j));

    m_data = data;
}

bool Matrix::isSquare()
{
    return m_data.rowCount() == m_data.columnCount();
}

bool Matrix::save(const QString& filePath) const
{
    bool result = false;

    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        St2000DataStream stream(&file);

        stream.writeString(FileHeaderString);
        stream.writeInt32(rowCount());
        stream.writeInt32(columnCount());

        stream.writeInt32(minRow());
        stream.writeInt32(minColumn());

        stream.writeInt16(type());
        stream.writeInt16(0);

        double* data = new double[rowCount() * columnCount()];
        {
            for(int i = 0; i < rowCount(); i++)
                for(int j = 0; j < columnCount(); j++)
                    data[i*columnCount() + j] = getCell(i, j);

            stream.writeRawData((char*)data, rowCount() * columnCount() * sizeof(double));
        }
        delete data;

        file.flush();
        file.close();
        result = false;
    }
    return result;
}

bool Matrix::load(int id, const QString& filePath)
{
    bool result = false;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly))
    {
        St2000DataStream stream(&file);

        if(stream.readString() == FileHeaderString)
        {
            setHandle(id);
            int rowCount = stream.readInt32();
            int columnCount = stream.readInt32();

            m_minRow = stream.readInt32();
            m_minColumn = stream.readInt32();
            m_data = MatrixData<double>(rowCount, columnCount);

            m_type = stream.readInt16();
            stream.readInt16();

            double* data = new double[rowCount * columnCount];
            {
                stream.readRawData((char*)data, rowCount * columnCount * sizeof(double));
                for(int i = 0; i < rowCount; i++)
                    for(int j = 0; j < columnCount; j++)
                        putCell(i, j, data[i*columnCount + j]);
            }
            delete data;
            file.close();

            result = true;
        }
    }
    return result;
}

bool Matrix::msort(int rowOrColumn, int sortType)
{
    std::vector<double> vector;
    if(sortType == 1 || sortType == 2)
    {
        if(rowOrColumn < m_minColumn || rowOrColumn > m_minColumn + columnCount())
        {
            VmLog::instance().error(QString(QObject::tr("Ошибка матриц. Выход за границу диапазона. "
                                                        "Вызов из имиджа типа: %1. Функция MSort"))
                                    .arg(executedObject->cls()->originalName()));
            return false;
        }
        for(int i = m_minRow; i < m_minRow + rowCount(); i++)
            vector.push_back(get(i, rowOrColumn));
    }
    else
    {
        if(rowOrColumn < m_minRow || rowOrColumn > m_minRow + rowCount())
        {
            VmLog::instance().error(QString(QObject::tr("Ошибка матриц. Выход за границу диапазона. "
                                                        "Вызов из имиджа типа: %1. Функция MSort"))
                                    .arg(executedObject->cls()->originalName()));
            return false;
        }
        for(int i = m_minColumn; i < m_minColumn + columnCount(); i++)
            vector.push_back(get(rowOrColumn, i));
    }

    if(sortType == 1 || sortType == 3)
        std::sort(vector.begin(), vector.end());
    else
        std::sort(vector.begin(), vector.end(), std::greater<double>());

    if(sortType == 1 || sortType == 2)
    {
        for(int i = 0, j = m_minRow; i < vector.size(); i++, j++)
            put(j, rowOrColumn, vector.at(i));
    }
    else
    {
        for(int i = 0, j = m_minColumn; i < vector.size(); i++, j++)
            put(rowOrColumn, j, vector.at(i));
    }
    return true;
}

}
