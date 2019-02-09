#ifndef SPACEITEMCOLLECTION_H
#define SPACEITEMCOLLECTION_H

#include <QObject>

namespace StCore {

class Sc2000DataStream;

class SpaceItemCollection
{
public:
    SpaceItemCollection();
    ~SpaceItemCollection();

    virtual void load(Sc2000DataStream& stream);

    qint16 count() const {return m_count;}
    void setCount(const qint16& count) {m_count = count;}

    qint16 limit() const {return m_limit;}
    void setLimit(const qint16& limit) {m_limit = limit;}

    qint16 delta() const {return m_delta;}
    void setDelta(const qint16& delta) {m_delta = delta;}

private:
    qint16 m_count;
    qint16 m_limit;
    qint16 m_delta;
};

class SpaceItemIdCollection : public SpaceItemCollection
{
public:
    SpaceItemIdCollection();
    ~SpaceItemIdCollection();

    void load(Sc2000DataStream& stream);

private:
    QList<qint16> m_ids;
};

}

#endif // SPACEITEMCOLLECTION_H
