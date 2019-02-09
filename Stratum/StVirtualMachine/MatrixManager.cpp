#include "MatrixManager.h"
#include "Matrix.h"
#include "Sc2000DataStream.h"
#include "VmLog.h"
#include "VmGlobal.h"

#include <Class.h>
#include <Object.h>
#include <QFile>
#include <math.h>

using namespace std;
using namespace StData;

namespace StVirtualMachine {

MatrixManager::MatrixManager(QObject* parent) :
    QObject(parent),
    m_plusMatrixes(ProjMatrixesCount, nullptr),
    m_minusMatrixes(EnvMatrixesCount, nullptr)
{}

MatrixManager::~MatrixManager()
{
    qDeleteAll(m_plusMatrixes);
    qDeleteAll(m_minusMatrixes);
}

int MatrixManager::createMatrix(int id, int minY, int maxY, int minX, int maxX, qint16 type)
{
    Matrix* m = create(id, minY, maxY, minX, maxX, type);
    return postCreate(m);
}

bool MatrixManager::isIdValid(int id)
{
    if(id > 0)
        return id < ProjMatrixesCount;

    if(id < 0)
        return abs(id) < EnvMatrixesCount;

    return true;
}

int MatrixManager::matrixOperation(int id1, int id2, int id3,
                                   function<double (double, double)> operation)
{
    Matrix* m1 = getMatrix(id1);
    Matrix* m2 = getMatrix(id2);

    if(m1 && m2)
    {
        int minRow = qMax(m1->minRow(), m2->minRow());
        int minColumn = qMax(m1->minColumn(), m2->minColumn());
        int maxRow = qMin(m1->maxRow(), m2->maxRow());
        int maxColumn = qMin(m1->maxColumn(), m2->maxColumn());

        if(maxRow >= minRow && maxColumn >= minColumn)
        {
            if(Matrix* m3 = create(id3, minRow, maxRow, minColumn, maxColumn, 0))
            {
                for(int i = minRow; i <= maxRow; i++)
                    for(int j = minColumn; j <= maxColumn; j++)
                    {
                        double a = m1->get(i, j);
                        double b = m2->get(i, j);
                        m3->put(i, j, operation(a, b));
                    }
                return postCreate(m3);
            }
        }
    }
    else
        errMatrixNumber("MAddC, MSubC, MMulC, MDivC");

    return 0;
}

Matrix* MatrixManager::create(int id, int minY, int maxY, int minX, int maxX, qint16 type)
{
    int rowsCount = maxY - minY + 1;
    int columnsCount = maxX - minX + 1;

    if(!isIdValid(id))
    {
        setError(tr("Невозможно создать матрицу. Задан некорректный номер"));
        return nullptr;
    }

    if (rowsCount > 0 && columnsCount > 0 && (rowsCount * columnsCount < 400000L))
        return new Matrix(id, minY, maxY, minX, maxX, type);
    else
        setError(tr("Невозможно создать матрицу"));

    return nullptr;
}

Matrix* MatrixManager::create(int id, int rowCount, int columnCount, qint16 type)
{
    return create(id, 0, rowCount - 1, 0, columnCount - 1, type);
}

int MatrixManager::postCreate(Matrix* matrix)
{
    if(matrix)
    {
        int id = matrix->handle();

        if(Matrix* oldMatrix = getMatrix(id))
            delete oldMatrix;

        if(id > 0)
            m_plusMatrixes[id] = matrix;
        else if(id < 0)
            m_minusMatrixes[abs(id)] = matrix;
        else
        {
            for(int i = 1; i < m_minusMatrixes.count(); i++)
            {
                if(!m_minusMatrixes.at(i))
                {
                    id = -i;
                    break;
                }
            }
            matrix->setHandle(id);
            m_minusMatrixes[abs(id)] = matrix;
        }
        return id;
    }
    return 0;
}

bool MatrixManager::isRowColumnValid(Matrix* matrix, int row, int column)
{
    return row >= matrix->minRow() && row <= matrix->maxRow() &&
            column >= matrix->minColumn() && column <= matrix->maxColumn();
}

Matrix* MatrixManager::getMatrix(int id)
{
    if(id > 0 && id < m_plusMatrixes.count())
        return m_plusMatrixes.at(id);

    if(id < 0 && abs(id) < m_minusMatrixes.count())
        return m_minusMatrixes.at(abs(id));

    return nullptr;
}

bool MatrixManager::deleteMatrix(int id)
{
    if(Matrix* matrix = getMatrix(id))
    {
        delete matrix;
        if(id > 0)
            m_plusMatrixes[id] = nullptr;

        else if(id < 0)
            m_minusMatrixes[abs(id)] = nullptr;

        return true;
    }
    else
        errMatrixNumber("MDeleteMatrix");

    return false;
}

bool MatrixManager::fill(int id, double value)
{
    if(Matrix* matrix = getMatrix(id))
    {
        matrix->fill(value);
        return true;
    }
    else
        errMatrixNumber("MFill");

    return false;
}

bool MatrixManager::diag(int id, double value)
{
    fill(id, 0.0);
    if(Matrix* matrix = getMatrix(id))
    {
        int min = qMin(matrix->rowCount(), matrix->columnCount());
        for(int i = 0; i < min; i++)
            matrix->putCell(i, i, value);

        return true;
    }
    else
        errMatrixNumber("MDiag");

    return false;
}

double MatrixManager::get(int id, int row, int column)
{
    if(Matrix* matrix = getMatrix(id))
    {
        if(isRowColumnValid(matrix, row, column))
            return matrix->get(row, column);
    }
    else
    {
        errMatrixNumber("MGet");
    }

    return 0.0;
}

bool MatrixManager::put(int id, int row, int column, double value)
{
    if(Matrix* matrix = getMatrix(id))
    {
        if(isRowColumnValid(matrix, row, column))
        {
            matrix->put(row, column, value);
            return true;
        }
    }
    else
        errMatrixNumber("MPut");

    return false;
}

bool MatrixManager::addX(int id, double value)
{
    Matrix* matrix = getMatrix(id);
    if(matrix)
    {
        for(int i = 0; i < matrix->rowCount(); i++)
            for(int j = 0; j < matrix->columnCount(); j++)
                matrix->putCell(i, j, matrix->getCell(i, j) + value);

        return true;
    }
    else
        errMatrixNumber("MAddX");

    return false;
}

bool MatrixManager::mulX(int id, double value)
{
    Matrix* matrix = getMatrix(id);
    if(matrix)
    {
        for(int i = 0; i < matrix->rowCount(); i++)
            for(int j = 0; j < matrix->columnCount(); j++)
                matrix->putCell(i, j, matrix->getCell(i, j) * value);

        return true;
    }
    else
        errMatrixNumber("MMulX or MDivX");

    return false;
}

bool MatrixManager::divX(int id, double value)
{
    if (value != 0.0)
        return mulX(id, 1.0 / value);

    return false;
}

int MatrixManager::addC(int id1, int id2, int id3)
{
    return matrixOperation(id1, id2, id3, [](double a, double b)
    {
        return a + b;
    });
}

int MatrixManager::subC(int id1, int id2, int id3)
{
    return matrixOperation(id1, id2, id3, [](double a, double b)
    {
        return a - b;
    });
}

int MatrixManager::mulC(int id1, int id2, int id3)
{
    return matrixOperation(id1, id2, id3, [](double a, double b)
    {
        return a * b;
    });
}

int MatrixManager::divC(int id1, int id2, int id3)
{
    return matrixOperation(id1, id2, id3, [](double a, double b)
    {
        return b == 0 ? 0 : a / b;
    });
}

int MatrixManager::mul(int id1, int id2, int id3)
{
    Matrix* m1 = getMatrix(id1);
    Matrix* m2 = getMatrix(id2);

    if(m1 && m2)
    {
        if(m1->columnCount() == m2->rowCount())
        {
            int minRow = m1->minRow();
            int minColumn = m1->minColumn();
            int maxRow = minRow + m1->rowCount() - 1;
            int maxColumn = minColumn + m2->columnCount() - 1;

            //            createMatrix(id3, minRow, maxRow, minColumn, maxColumn, 0);
            if(Matrix* m3 = create(id3, minRow, maxRow, minColumn, maxColumn, 0))
            {
                for(int i = 0; i < m3->rowCount(); i++)
                    for(int j = 0; j < m3->columnCount(); j++)
                    {
                        double value = 0;
                        for(int k = 0; k < m1->columnCount(); k++)
                            value += m1->getCell(i, k) * m2->getCell(k, j);

                        m3->putCell(i, j, value);
                    }
                return postCreate(m3);
            }
        }
    }
    else
        errMatrixNumber("MMulC");

    return 0;
}

int MatrixManager::transp(int id1, int id2)
{
    Matrix* m1 = getMatrix(id1);
    if(m1)
    {
        if(id1 == id2)
        {
            m1->transp();
            return id1;
        }
        else
        {
            if(Matrix* m2 = create(id2, m1->minColumn(), m1->maxColumn(), m1->minRow(), m1->maxRow(), 0))
            {
                for(int i = 0; i < m1->rowCount(); i++)
                    for(int j = 0; j < m1->columnCount(); j++)
                        m2->putCell(j, i, m1->getCell(i, j));

                return postCreate(m2);
            }
        }
    }
    else
        errMatrixNumber("MTransp");

    return 0;
}

double MatrixManager::det(int id)
{
    Matrix* matrix = getMatrix(id);
    if(matrix)
    {
        if(!matrix->isSquare())
        {
            setError("Det: M_NOT_SQUARE");
            return 0;
        }

        Matrix m = *matrix;

        int n = matrix->rowCount(), i, j, k;
        double t, d = 1, max;

        for(k = 0; k < n; k++)
        {
            max=0;
            for(i = k; i < n; i++)
            {
                t = m.getCell(i, k);
                if (fabs(t) > fabs(max))
                {
                    max = t;
                    j = i;
                }
            }
            if (max == 0)
                return 0;

            if (j != k)
            {
                d = -d;
                for(i = k; i < n; i++)
                {
                    t = m.getCell(j, i);
                    m.putCell(j, i, m.getCell(k, i));
                    m.putCell(k, i, t);
                }
            }
            for(i = k + 1; i < n; i++)
            {
                t = m.getCell(i, k) / max;
                for(j = k + 1; j < n; j++)
                {
                    m.putCell(i, j, m.getCell(i, j) - t * m.getCell(k, j));
                }
            }//end i
            d = d * m.getCell(k, k);
        }
        return d;
    }
    else
        errMatrixNumber("MDet");

    return 0;
}

int MatrixManager::glue(int id1, int id2, int id3, long y, long x)
{
    Matrix* m1 = getMatrix(id1);
    Matrix* m2 = getMatrix(id2);

    if(m1 && m2)
    {
        int minX = m2->minColumn() + x;
        int maxX = m2->maxColumn() + x;
        int minY = m2->minRow() + y;
        int maxY = m2->maxRow() + y;

        int Xmin = qMin(m1->minColumn(), minX);
        int Xmax = qMax(m1->maxColumn(), maxX);
        int Ymin = qMin(m1->minRow(), minY);
        int Ymax = qMax(m1->maxColumn(), maxY);

        if(Matrix* m3 = create(id3, Ymin,Ymax, Xmin,Xmax,0))
        {
            for(int i = 0; i < m2->rowCount(); i++)
                for(int j = 0; j < m2->columnCount(); j++)
                    m3->put(i + minY, j + minX, m2->getCell(i,j));

            for(int i = 0; i < m1->rowCount(); i++)
                for(int  j = 0; j < m1->columnCount(); j++)
                    m3->put(i + m1->minRow(), j + m1->minColumn(), m1->getCell(i, j));

            return postCreate(m3);
        }
    }
    else
        errMatrixNumber("MGlue");

    return 0;
}

int MatrixManager::cut(int id1, int id2, int y, int x, int sy, int sx)
{
    Matrix* m1 = getMatrix(id1);
    if(m1)
    {
        int dx = x - m1->minColumn();
        int dy = y - m1->minRow();

        sy = qMin(sy, m1->rowCount() - dy);
        sx = qMin(sx, m1->columnCount() - dx);

        if(Matrix* m2 = create(id2, y, y + sy, x, sx + x, 0))
        {
            for(int i = 0; i < sy; i++)
                for(int j = 0; j < sx; j++)
                    m2->putCell(i, j, m1->getCell(i + dy, j + dx));

            return postCreate(m2);
        }
        else
            errMatrixNumber("MCut");
    }
    else
        errMatrixNumber("MCut");

    return 0;
}

bool MatrixManager::move(int id, int minRow, int minColumn)
{
    Matrix* m = getMatrix(id);
    if(m)
    {
        m->setMinRow(minRow);
        m->setMinColumn(minColumn);
        return true;
    }
    else
        errMatrixNumber("MMove");

    return false;
}

int MatrixManager::obr(int id1, int id2)
{
    Matrix* m1 = getMatrix(id1);
    int result = 0;
    if (m1)
    {
        int n, s, i, j, k, m;
        double t;
        n = m1->rowCount();
        m = 2 * n;
        s = 0;
        Matrix mt(0, 0, n - 1, 0, m - 1, 0);

        // наращивание матрицы еденичной матрицей
        for(i = 0; i < n; i++)
            for(j = 0; j < m; j++)
                mt.putCell(i, j, j < n ? m1->getCell(i, j) :
                                         j == n + i ? 1.0 : 0.0);

        // начало обращения
        for(i = 0; i < n; i++)
        {
            k = i;
            while(mt.getCell(k, i) == 0)
            {
                s = 1;
                if(k < n-1)
                    k += 1;
                else
                    return result;
            }

            if (s == 1)
                for(j = 0; j < m; j++)
                {
                    t = mt.getCell(k, j);
                    mt.putCell(k, j, mt.getCell(i, j));
                    mt.putCell(i, j, t);
                }

            for(j = m - 1; j >= i; j--)
                mt.putCell(i, j, mt.getCell(i, j) / mt.getCell(i, i));

            for(k = 0; k < n; k++)
                if (k != i)
                    for(j = m - 1; j >= i; j--)
                        mt.putCell(k, j, mt.getCell(k,j) - mt.getCell(i, j) * mt.getCell(k, i));

        }//end i
        Matrix* m2 = create(id2, m1->minRow(), m1->minRow() + m1->rowCount() - 1,
                            m1->minColumn(), m1->minColumn() + m1->columnCount() - 1, 0);
        if(m2)
        {
            for(i = 0; i < n; i++)
                for(j = 0; j < n; j++)
                    m2->putCell(i, j, mt.getCell(i, j + n));

            result = postCreate(m2);
        }
    }
    else
        errMatrixNumber("MObr");

    return result;
}

bool MatrixManager::med(int id)
{
    Matrix* m = getMatrix(id);
    if (m)
    {
        for(int y = 0; y < m->rowCount(); y++)
            for(int x = 0; x < m->columnCount(); x++)
                m->putCell(y, x, m->getCell(y, x) <= 0 ? 0 : 1);

        return true;
    }
    else
        errMatrixNumber("MEd");

    return false;
}

bool MatrixManager::delta(int id)
{
    Matrix* m = getMatrix(id);
    if (m)
    {
        for(long i = 0; i < m->rowCount(); i++)
            for(long j = 0; j < m->columnCount(); j++)
                m->putCell(i, j, m->getCell(i, j) == 0 ? 1.0 : 0.0);

        return true;
    }
    else
        errMatrixNumber("MDelta");

    return false;
}

int MatrixManager::mNot(int id)
{
    Matrix* m = getMatrix(id);
    if(m)
    {
        for(int y = 0; y < m->rowCount(); y++)
            for(long x = 0; x < m->columnCount(); x++)
            {
                double t = m->getCell(y,x) > 0 ? 0 : 1;
                m->putCell(y, x, t);
            }
        return true;
    }
    else
        errMatrixNumber("MNot");

    return false;
}

double MatrixManager::sum(int id)
{
    Matrix* m = getMatrix(id);
    if(m)
    {
        double t = 0;
        for(int i = 0; i < m->rowCount(); i++)
            for(int j = 0; j < m->columnCount(); j++)
                t += m->getCell(i, j);

        return t;
    }
    else
        errMatrixNumber("MSum");

    return 0;
}

void MatrixManager::deleteAllMatrixes(Project* project)
{
    deleteMinusMatrixes(project);
    deletePlusMatrixes(project);
}

void MatrixManager::deletePlusMatrixes(StData::Project* project)
{
    for(int i = 0; i < m_plusMatrixes.count(); i++)
    {
        Matrix * m = m_plusMatrixes.at(i);
        if (m && (m->project() == project || project == nullptr))
        {
            delete m;
            m_plusMatrixes[i] = nullptr;
        }
    }
}

void MatrixManager::deleteMinusMatrixes(StData::Project* project)
{
    for(int i = 0; i < m_minusMatrixes.count(); i++)
    {
        Matrix* m = m_minusMatrixes.at(i);
        if((m && m->project() == project) || project == nullptr)
        {
            delete m;
            m_minusMatrixes[i] = nullptr;
        }
    }
}

bool MatrixManager::saveAs(int id, const QString& path)
{
    if(Matrix* m = getMatrix(id))
        return m->save(path);

    return false;
}

int MatrixManager::load(int id, const QString& path)
{
    int result = 0;

    Matrix* m = new Matrix();
    if(m->load(id, path))
        result = postCreate(m);
    else
        delete m;

    return result;
}

void MatrixManager::saveAllPlusMatrixes(const QDir& projectDir)
{
    for(int i = 0; i < m_plusMatrixes.count(); i++)
        if(Matrix* m = m_plusMatrixes.at(i))
        {
            QString filePath = projectDir.absoluteFilePath("%1_.mat").arg(m->handle());
            m->save(filePath);
        }
}

void MatrixManager::errMatrixNumber(const QString& funcName)
{
    QString errMsg = QString(tr("Ошибка матриц. Неверный номер при обращении к матрице. Вызов из имиджа типа: '%1'. Функция: '%2'"))
                     .arg(executedObject->cls()->originalName())
                     .arg(funcName);

    VmLog::instance().error(errMsg);
}


}
