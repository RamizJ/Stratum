#include "ArrayFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

namespace StVirtualMachine {

void setupArrayFunctions()
{
    operations[VM_new] = dm_new;
    operations[VM_delete] = dm_delete;
    operations[VM_VINSERT] = dm_vinsert;
    operations[VM_VDELETE] = dm_vdelete;
    operations[VM_VGETCOUNT] = dm_vgetcount;
    operations[VM_VGETTYPE] = dm_gettype;
    operations[VM_VCLEARALL] = dm_clearall;
    operations[VM_VSETf] = dm_vsetf;
    operations[VM_VSETs] = dm_vsets;
    operations[VM_VSETh] = dm_vseth;
    operations[VM_VGETf] = dm_vgetf;
    operations[VM_VGETh] = dm_vgeth;
    operations[VM_VGETs] = dm_vgets;
    operations[VM_VSTORE] = dm_save;
    operations[VM_VLOAD] = dm_load;
    operations[VM_VSORT_ASC] = dm_sort;
    operations[VM_VSORT] = dm_sort_desc;
    operations[VM_VSORT_DIFFERENT] = dm_sort_different;
}

void dm_new()
{
    int handle = arrayManager->createArray();
    valueStack->pushInt32(handle);
}

void dm_delete()
{
    arrayManager->deleteArray(valueStack->popInt32());
}

void dm_clearall()
{
    arrayManager->reset();
}

void dm_vinsert()
{
    QString typeName = valueStack->popString();
    int handle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->insertItem(handle, typeName));
}

void dm_vdelete()
{
    int index = valueStack->popDouble();
    int handle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->deleteItem(handle, index));
}

void dm_vgetcount()
{
    int handle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->getCount(handle));
}

void dm_gettype()
{
    int index = valueStack->popDouble();
    int handle = valueStack->popInt32();
    valueStack->pushString(arrayManager->getItemTypeName(handle, index));
}

void dm_vsetf()
{
    double data = valueStack->popDouble();
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle = valueStack->popInt32();

    arrayManager->setVarF(handle, index, name, data);
}

void dm_vseth()
{
    int data = valueStack->popInt32();
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle = valueStack->popInt32();

    arrayManager->setVarH(handle, index, name, data);
}

void dm_vsets()
{
    QString data = valueStack->popString();
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle = valueStack->popInt32();

    arrayManager->setVarS(handle, index, name, data);
}

void dm_vgetf()
{
    double value;
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle  = valueStack->popInt32();

    arrayManager->getVarF(handle, index, name, value);
    valueStack->pushDouble(value);
}

void dm_vgeth()
{
    int value;
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle  = valueStack->popInt32();

    arrayManager->getVarH(handle, index, name, value);
    valueStack->pushInt32(value);
}

void dm_vgets()
{
    QString value;
    QString name = valueStack->popString();
    int index = valueStack->popDouble();
    int handle  = valueStack->popInt32();

    arrayManager->getVarS(handle, index, name, value);
    valueStack->pushString(value);
}

void dm_save()
{
    int arrayHandle = valueStack->popInt32();
    int streamHandle = valueStack->popInt32();

    StreamItem* streamItem = streamManager->getStreamItem(streamHandle);
    valueStack->pushDouble(arrayManager->save(arrayHandle, streamItem));
}

void dm_load()
{
    int streamHandle = valueStack->popInt32();
    StreamItem* streamItem = streamManager->getStreamItem(streamHandle);
    int arrayHandle = arrayManager->load(streamItem);
    valueStack->pushInt32(arrayHandle);
    //    int src=POP_LONGi;
    //    TStream *_s=streams->GetStream(src);
    //    dyno_t h=dyno_mem->Load(_s);
    //    PUSH_LONG(h);
}

void dm_sort()
{
    qint16 objectsCount = codePointer->getCode();
    codePointer->incPosition();

    QVector<QString> fieldNames(objectsCount);
    for(int i = 0; i < objectsCount; i++)
        fieldNames[i] = valueStack->popString();

    int arrayHandle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->sort(arrayHandle, false, fieldNames));

    //throw std::runtime_error("array sort function not implemented");
}

void dm_sort_desc()
{
    qint16 objectsCount = codePointer->getCode();
    codePointer->incPosition();

    QVector<QString> fieldNames(objectsCount);
    for(int i = 0; i < objectsCount; i++)
        fieldNames[i] = valueStack->popString();

    bool desc = valueStack->popDouble() > 0;
    int arrayHandle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->sort(arrayHandle, desc, fieldNames));
}

void dm_sort_different()
{
    qint16 objectsCount = codePointer->getCode() / 2;
    codePointer->incPosition();

    QVector<std::tuple<QString, bool>> fieldNames(objectsCount);
    for(int i = 0; i < objectsCount; i++)
    {
        bool desc = valueStack->popDouble() > 0;
        QString fieldName = valueStack->popString();

        fieldNames[i] = std::make_tuple(fieldName, desc);
    }

    int arrayHandle = valueStack->popInt32();
    valueStack->pushDouble(arrayManager->sort(arrayHandle, fieldNames));
}

}
