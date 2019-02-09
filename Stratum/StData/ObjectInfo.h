#ifndef OBJECTINFO_H
#define OBJECTINFO_H

#include "HandleItem.h"

#include <QObject>
#include <QPointF>

namespace StData
{
class Project;
class Class;
class St2000DataStream;

class STDATASHARED_EXPORT ObjectInfo : public HandleItem
{
public:
    enum Flag {NeedCreate = 1, NeedUpdate = 2, WasSized = 4, PreProcess = 8, PostProcess = 16};
    Q_DECLARE_FLAGS(Flags, Flag)

public:
    explicit ObjectInfo();
    virtual ~ObjectInfo(){}

    QString name() const {return m_name;}
    void setName(const QString& name) {m_name = name;}

    qint8 flags() const {return m_flags;}
    void setFlags(const qint8& flags) {m_flags = flags;}

    QString absentClass() const {return m_absentClass;}
    void setAbsentClass(const QString& absentClass) {m_absentClass = absentClass;}

    QPointF position() const {return m_position;}
    void setPosition(const QPointF& position) {m_position = position;}

    Class* cls() const {return m_class;}
    void setClass(Class* cls) {m_class = cls;}

    void load(St2000DataStream& stream, qint32 fileVersion, bool loadName, bool loadPosition);
    static void skip(St2000DataStream& stream, qint32 fileVersion, bool loadName, bool loadPosition);

private:
    qint16 m_handle;
    qint8 m_flags;
    QString m_name;
    QString m_absentClass;
    QPointF m_position;

    Class* m_class;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ObjectInfo::Flags)

}

#endif // OBJECTINFO_H
