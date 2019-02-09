#ifndef MATRIX_H
#define MATRIX_H

#include <HandleItem.h>

#include <QVector>
#include <MatrixData.h>

namespace StData {
class Project;
class St2000DataStream;
}

namespace StVirtualMachine {

class Matrix : public StData::HandleItem
{
public:
    Matrix();
    Matrix(int handle, int minRow, int maxRow, int minColumn, int maxColumn, qint16 type);
    Matrix(const Matrix& m);
    void operator = (const Matrix& m);

    StData::Project* project() const { return m_project; }
    void setProject(StData::Project* project) { m_project = project; }

    double get(int row, int column) const;
    double getCell(int row, int column) const;

    void put(int row, int column, double value);
    void putCell(int row, int column, double value);

    int rowCount() const {return m_data.rowCount();}
    int columnCount() const {return m_data.columnCount();}

    int minRow() const { return m_minRow; }
    int minColumn() const { return m_minColumn; }
    void setMinRow(int minRow) { m_minRow = minRow; }
    void setMinColumn(int minColumn) { m_minColumn = minColumn; }

    int maxRow() const {return m_minRow + rowCount() - 1;}
    int maxColumn() const {return m_minColumn + columnCount() - 1;}

    qint16 type() const { return m_type; }

    void fill(double value);
    void transp();

    bool isSquare();

    MatrixData<double> data() const { return m_data; }

    bool save(const QString& filePath) const;
    bool load(int id, const QString& filePath);

    bool msort(int rowOrColumn, int sortType);

private:
    MatrixData<double> m_data;
    int m_minRow, m_minColumn;
    qint16 m_type;
    StData::Project* m_project;
    const QString FileHeaderString = "MATRIX FILE.";
};

}

#endif // MATRIX_H
