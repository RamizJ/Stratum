#ifndef VARSTACK_H
#define VARSTACK_H

#include <QStack>
#include <QPair>

namespace StData {
class Class;
class St2000DataStream;
}

namespace StCompiler {

class VarStack : public QStack<QPair<StData::Class*, qint16>>
{
public:
    VarStack();
    ~VarStack();

    void push(StData::Class* cl, qint16 pos = -1);
    StData::Class* pop(int& pos);

    StData::Class* top();

    bool setPtr(StData::St2000DataStream& stream);
};

}

#endif // VARSTACK_H
