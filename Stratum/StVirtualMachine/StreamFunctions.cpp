#include "StreamFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <QString>

namespace StVirtualMachine {

void setupStreamFunctions()
{
    operations[V_CREATESTREAM] = v_createstream;
    operations[V_CLOSESTREAM] = v_closestream;

    operations[V_SEEK] = v_seek;
    operations[V_STREAMSTATUS] = v_streamstatus;
    operations[V_EOF] = v_eof;

    operations[V_GETPOS] = v_getpos;
    operations[V_GETSIZE] = v_getsize;
    operations[V_SETWIDTH] = v_setwidth;

    operations[V_FREAD] = v_fread;
    operations[V_SREAD] = v_sread;
    operations[V_GETLINE] = v_getline;

    operations[V_FWRITE] = v_fwrite;
    operations[V_SWRITE] = v_swrite;
    operations[V_COPYBLOCK] = v_copyblock;
    operations[V_TRUNCATE] = v_truncate;
}

void v_createstream()
{
    QString flags = valueStack->popString();
    QString name = valueStack->popString();
    QString device = valueStack->popString();

    qint32 handle = streamManager->createStream(device, name, flags);
    valueStack->pushInt32(handle);
}

void v_closestream()
{
    int handle = valueStack->popInt32();
    valueStack->pushDouble(streamManager->closeStream(handle));
}

void v_seek()
{
    long pos = valueStack->popDouble();
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->seek(h, pos));
}

void v_streamstatus()
{
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->status(h));
}

void v_eof()
{
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->eof(h));
}

void v_getpos()
{
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->pos(h));
}

void v_getsize()
{
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->getSize(h));
}

void v_setwidth()
{
    int w = (int)valueStack->popDouble();
    int h = valueStack->popInt32();
    valueStack->pushDouble(streamManager->setWidth(h, w));
}

void v_fread()
{
    int h = valueStack->popInt32();
    double d;
    if(!streamManager->readFloat(h, d))
        d = 0;
    valueStack->pushDouble(d);
}

void v_sread()
{
    int h = valueStack->popInt32();
    QString str = streamManager->readLine(h);
    valueStack->pushString(str);
}

void v_fwrite()
{
    double f = valueStack->popDouble();
    int h = valueStack->popInt16();
    valueStack->pushDouble(streamManager->writeFloat(h, f));
}

void v_copyblock()
{
    double size = valueStack->popDouble();
    double from = valueStack->popDouble();
    int targetHandle = valueStack->popInt32();
    int sourceHandle = valueStack->popInt32();

    valueStack->pushDouble(streamManager->copyBlock(sourceHandle, targetHandle, from, size));
}

void v_truncate()
{
    int handle = valueStack->popInt32();
    valueStack->pushDouble(streamManager->trunc(handle));
}

void v_swrite()
{
    QString str = valueStack->popString();
    qint16 handle = valueStack->popInt32();

    valueStack->pushDouble(streamManager->writeLine(handle, str));
}

void v_getline()
{
    QString separator = valueStack->popString();
    int size = valueStack->popDouble();
    int handle = valueStack->popInt32();

    QString line = separator.isEmpty() ?
                       streamManager->getLine(handle, size) :
                       streamManager->getLine(handle, size, separator.at(0).toLatin1());
    valueStack->pushString(line);
}


}
