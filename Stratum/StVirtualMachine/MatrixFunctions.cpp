#include "MatrixFunctions.h"
#include "Matrix.h"
#include "MatrixData.h"
#include "MatrixEditor.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <ObjectVar.h>

using namespace StData;

namespace StVirtualMachine {

void setupMatrixFunctions()
{
    operations[V_MCREATE] = mcreate;
    operations[V_MDELETE] = mdelete;
    operations[V_MFILL] = v_mfill;
    operations[V_MGET] = v_mget;
    operations[V_MPUT] = v_mput;
    operations[V_MEDITOR] = v_meditor;
    operations[V_MDIAG] = v_mdiag;
    operations[V_MADDX] = v_maddx;
    operations[V_MSUBX] = v_msubx;
    operations[V_MDET] = v_mdet;
    operations[V_MDELTA] = v_mdelta;
    operations[V_MED] = v_med;
    operations[V_MDIVX] = v_mdivx;
    operations[V_MMULX] = v_mmulx;
    operations[V_TRANSP] = v_transp;
    operations[V_MADDC] = v_maddc;
    operations[V_MNOT] = v_mnot;
    operations[V_MSUM] = v_msum;
    operations[V_MSUBC] = v_msubc;
    operations[V_MMULC] = v_mmulc;

    operations[V_MDIVC] = v_mdivc;
    operations[V_MMUL] = v_mmul;
    operations[V_MGLUE] = v_mglue;
    operations[V_MCUT] = v_mcut;
    operations[V_MMOVE] = v_mmove;
    operations[V_MOBR] = v_mobr;

    operations[V_MSAVEAS] = v_msaveas;
    operations[V_MLOAD] = v_mload;

    operations[V_MDIM] = v_mdim;
    operations[V_MSORT] = v_msort;
}

void mcreate()
{
    double f = valueStack->popDouble();
    double maxColumns = valueStack->popDouble();
    double minColumns = valueStack->popDouble();
    double maxRows = valueStack->popDouble();
    double minRows = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f > 0)
        id = matrixManager->createMatrix(id, minRows, maxRows, minColumns, maxColumns, 0);

    valueStack->pushDouble(id);
}

void mdelete()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f > 0)
        valueStack->pushDouble(matrixManager->deleteMatrix(id));
    else
        valueStack->pushDouble(0.0);
}

void v_mfill()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f > 0)
        valueStack->pushDouble(matrixManager->fill(id, value));
    else
        valueStack->pushDouble(0.0);
}

void v_mdiag()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->diag(id, value));
    else
        valueStack->pushDouble(0.0);
}

void v_maddx()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->addX(id, value));
    else
        valueStack->pushDouble(0.0);
}

void v_msubx()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->addX(id, -value));
    else
        valueStack->pushDouble(0.0);
}

void v_mmulx()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->mulX(id, value));
    else
        valueStack->pushDouble(0.0);
}

void v_mdivx()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->divX(id, value));
    else
        valueStack->pushDouble(0.0);
}

void v_med()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->med(id));
    else
        valueStack->pushDouble(0.0);
}

void v_mdelta()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->delta(id));
    else
        valueStack->pushDouble(0.0);
}

void v_mdet()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->det(id));
    else
        valueStack->pushDouble(0.0);
}

void v_transp()
{
    double f = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->transp(id1, id2));
    else
        valueStack->pushDouble(0.0);
}

void v_maddc()
{
    double f = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->addC(id1, id2, id3));
    else
        valueStack->pushDouble(0.0);
}

void v_mnot()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->mNot(id));
    else
        valueStack->pushDouble(0.0);
}

void v_msum()
{
    double f = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->sum(id));
    else
        valueStack->pushDouble(0.0);
}

void v_msubc()
{
    double f = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->subC(id1, id2, id3));
    else
        valueStack->pushDouble(0.0);
}

void v_mmulc()
{
    double f = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->mulC(id1, id2, id3));
    else
        valueStack->pushDouble(0.0);
}

void v_mdivc()
{
    double f = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->divC(id1, id2, id3));
    else
        valueStack->pushDouble(0.0);
}

void v_mmul()
{
    double f = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->mul(id1, id2, id3));
    else
        valueStack->pushDouble(0.0);
}

void v_mglue()
{
    double f = valueStack->popDouble();
    double x = valueStack->popDouble();
    double y = valueStack->popDouble();
    double id3 = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->glue(id1, id2, id3, y, x));
    else
        valueStack->pushDouble(0.0);
}

void v_mcut()
{
    double f = valueStack->popDouble();
    double sx = valueStack->popDouble();
    double sy = valueStack->popDouble();
    double x = valueStack->popDouble();
    double y = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->cut(id1, id2, y, x, sy, sx));
    else
        valueStack->pushDouble(0.0);
}

void v_mmove()
{
    double f = valueStack->popDouble();
    double x = valueStack->popDouble();
    double y = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->move(id1, y, x));
    else
        valueStack->pushDouble(0.0);
}

void v_mobr()
{
    double f = valueStack->popDouble();
    double id2 = valueStack->popDouble();
    double id1 = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->obr(id1, id2));
    else
        valueStack->pushDouble(0.0);
}

void v_mput()
{
    double f = valueStack->popDouble();
    double value = valueStack->popDouble();
    double x = valueStack->popDouble();
    double y = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->put(id, y, x, value));
    else
        valueStack->pushDouble(0.0);
}

void v_mget()
{
    double f = valueStack->popDouble();
    double x = valueStack->popDouble();
    double y = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(f>0)
        valueStack->pushDouble(matrixManager->get(id, y, x));
    else
        valueStack->pushDouble(0.0);
}

void v_meditor()
{
    double show = valueStack->popDouble();
    double id = valueStack->popDouble();

    if(show && id)
    {
        MatrixEditor editor;
        editor.setMatrix(matrixManager->getMatrix(id));
        editor.exec();
    }
}

void v_msaveas()
{
    double f = valueStack->popDouble();
    QString fileName = valueStack->popString();
    double id = valueStack->popDouble();

    if(f > 0)
    {
        QString path = executedProject->projectDir().absoluteFilePath(fileName);
        valueStack->pushDouble(matrixManager->saveAs(id, path));
    }
    else
        valueStack->pushDouble(0.0);
}

void v_mload()
{
    double f = valueStack->popDouble();
    QString fileName = valueStack->popString();
    double id = valueStack->popDouble();

    if(f > 0)
    {
        QString path = executedProject->projectDir().absoluteFilePath(fileName);
        valueStack->pushDouble(matrixManager->load(id, path));
    }
    else
        valueStack->pushDouble(0.0);
}

void v_mdim()
{
    VarData* maxColumn = valueStack->popVarData();
    VarData* maxRow = valueStack->popVarData();
    VarData* minColumn = valueStack->popVarData();
    VarData* minRow = valueStack->popVarData();
    int matrixId = valueStack->popDouble();

    if(Matrix* m = matrixManager->getMatrix(matrixId))
    {
        minRow->setDouble(m->minRow());
        minColumn->setDouble(m->minColumn());
        maxRow->setDouble(m->maxRow());
        maxColumn->setDouble(m->maxColumn());

        valueStack->pushDouble(matrixId);
    }
    else
        valueStack->pushDouble(0.0);
}

void v_msort()
{
    double f = valueStack->popDouble();
    double sortType = valueStack->popDouble();
    double rowOrColumn = valueStack->popDouble();
    double matrixId = valueStack->popDouble();

    bool result = false;
    if(f > 0)
        if(Matrix* m = matrixManager->getMatrix(matrixId))
           result = m->msort(rowOrColumn, sortType);

    valueStack->pushDouble(result ? matrixId : 0.0);
}

}
