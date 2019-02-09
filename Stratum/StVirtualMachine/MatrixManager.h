#ifndef MATRIXMANAGER_H
#define MATRIXMANAGER_H

#include <ErrorProvider.h>
#include <QDir>
#include <QObject>
#include <QVector>
#include <functional>

namespace StData {
class Project;
}

namespace StVirtualMachine {

class Matrix;

class MatrixManager : public QObject, public StData::ErrorHolder
{
    Q_OBJECT
public:
    const int EnvMatrixesCount = 1000;
    const int ProjMatrixesCount = 4096;

public:
    explicit MatrixManager(QObject* parent = 0);
    ~MatrixManager();

    int createMatrix(int id, int minY, int maxY, int minX, int maxX, qint16 type);

    bool deleteMatrix(int id);
    Matrix* getMatrix(int id);

    double get(int id, int row, int column);
    bool put(int id, int row, int column, double value);

    bool fill(int id, double value);
    bool diag(int id, double value);

    bool addX(int id, double value);
    bool mulX(int id, double value);
    bool divX(int Q,double value);

    int addC(int id1, int id2, int id3);
    int subC(int id1, int id2, int id3);
    int mulC(int id1, int id2, int id3);
    int divC(int id1, int id2, int id3);

    int mul(int id1,int id2,int id3);
    int transp(int id1,int id2);
    double det(int id);
    int glue(int id1,int id2,int id3,long y,long x);
    int cut(int id1,int id2, int y, int x, int sy, int sx);
    bool move(int id, int minRow, int minColumn);
    int obr(int id1,int id2);

    bool med(int id);
    bool delta(int id);
    int mNot(int id);
    double sum(int id);

    void deleteAllMatrixes(StData::Project* project = nullptr);
    void deletePlusMatrixes(StData::Project* project = nullptr);
    void deleteMinusMatrixes(StData::Project* project = nullptr);

    bool saveAs(int id, const QString& path);
    void saveAllPlusMatrixes(const QDir& projectDir);
    int load(int id, const QString& path);

private:
    bool isIdValid(int id);
    int matrixOperation(int id1, int id2, int id3,
                         std::function<double(double, double)> operation);

    Matrix* create(int id, int minY, int maxY, int minX, int maxX, qint16 type);
    Matrix* create(int id, int rowCount, int columnCount, qint16 type);
    int postCreate(Matrix* matrix);

    inline bool isRowColumnValid(Matrix* matrix, int row, int column);

    void errMatrixNumber(const QString& funcName);

private:
    QVector<Matrix*> m_plusMatrixes;
    QVector<Matrix*> m_minusMatrixes;
};

}

#endif // MATRIXMANAGER_H
