#ifndef CODEPOINTER_H
#define CODEPOINTER_H

#include <QtGlobal>
#include <QByteArray>
#include <QObject>
#include <QDataStream>
#include <memory>

namespace StData {
class ObjectVar;
class St2000DataStream;
}

namespace StVirtualMachine {

typedef std::shared_ptr<StData::St2000DataStream> DataStreamPtr;

class CodePointer : public QObject
{
    Q_OBJECT
public:
    explicit CodePointer(QObject* parent = 0);

    void setCodeBytes(const QByteArray& vmByteCode);
    int getCodesCount() const { return m_byteCode.length() / 2; }

    void reset();

    qint16 getCode(int offset = 0);
    qint16 toNextCode();

    int getPosition() const;
    void setPosition(int position);
    void incPosition(int incValue = 1);
    void setNullCode();
    QString popString();

//    inline qint16* getPtr(int offset = 0);
    double getDouble();
    int getInt32();
    QString getString();

private:
    QByteArray m_byteCode;
    DataStreamPtr m_byteCodeStream;
    bool m_isNullCode;
//    int m_currentIndex;
//    int m_codesCount;

//    qint16* m_codes;
//    qint16* m_pointer;
//    qint16 m_nullCode;
};

}

#endif // CODEPOINTER_H
