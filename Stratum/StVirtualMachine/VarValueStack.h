#ifndef VARVALUESTACK_H
#define VARVALUESTACK_H

#include <QObject>
#include <QString>
#include <QVector>

namespace StData {
class ObjectVar;
struct VarData;
}

namespace StVirtualMachine {

class VarValueStack : public QObject
{
    Q_OBJECT
public:
    explicit VarValueStack(QObject* parent = 0);
    ~VarValueStack();

    void reset();

    void pushDouble(double value);
    void pushInt32(qint32 value);
    void pushInt16(qint16 value);
    void pushByte(quint8 value);
    void pushString(const QString& value);
    void pushVarData(StData::VarData* var);

    double popDouble();
    qint32 popInt32();
    qint16 popInt16();
    quint8 popByte();
    QString popString();
    StData::VarData* popVarData();

    double getDouble(int offset = 0) const;
    qint32 getInt32(int offset = 0) const;
    qint16 getInt16(int offset = 0) const;
    quint8 getByte(int offset = 0) const;
    QString getString(int offset = 0) const;
    StData::VarData* getVarData(int offset = 0) const;

    void setDouble(double value, int offset = 0);
    void setInt32(qint32 value, int offset = 0);
    void setInt16(qint16 value, int offset = 0);
    void setByte(quint8 value, int offset = 0);
    void setString(const QString& value, int offset = 0);
//    void setVarData(StData::VarData* value, int offset = 0);

    void incDouble(double value = 1, int offset = 0);
    void incInt(int value = 1, int offset = 0);

    void incPosition(int offset = 1);

    int getIndex() const;

private:
    QVector<StData::VarData> m_values;
    int m_i;
    StData::VarData* m_valuePointer;
};

}

#endif // VARVALUESTACK_H
