#ifndef SCHEME_H
#define SCHEME_H

#include "stdata_global.h"

#include <QObject>

namespace StData {

class St2000DataStream;
class Class;
class Space;

class STDATASHARED_EXPORT Scheme : public QObject
{
public:
    enum Flag {External = 1, Editing = 2};
    Q_DECLARE_FLAGS(Flags, Flag)

public:
    explicit Scheme(Class* cls, QObject *parent = 0);
    ~Scheme();

    Class* cls() const { return m_class; }

    QString fileName() const {return m_fileName;}
    void setFileName(const QString& fileName) {m_fileName = fileName;}

    Space* getSpace();
    void setSpace2d(Space* space2d) {m_space = space2d;}

    qint32 flags() const {return m_flags;}
    void setFlags(const qint32& flags) {m_flags = flags;}

    qint16 spaceId() const {return m_spaceId;}
    void setSpaceId(const qint16& spaceId) {m_spaceId = spaceId;}

    QByteArray spaceData() const { return m_spaceData; }

    void load(St2000DataStream& stream);

public:
    static void skip(St2000DataStream& stream);

private:
    Space* createSpaceFromSpaceData();
    Space* createSpaceFromFileName();

private:
    QString m_fileName;
    QByteArray m_spaceData;

    qint16 m_spaceId;
    qint32 m_flags;

    Space* m_space;
    Class* m_class;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Scheme::Flags)

}

#endif // SCHEME_H
