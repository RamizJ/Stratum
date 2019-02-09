#ifndef ICON_H
#define ICON_H

#include "stdata_global.h"

#include <QObject>
#include <QImage>
#include <memory>

namespace StData {

class St2000DataStream;

class STDATASHARED_EXPORT Icon : public QObject
{
    Q_OBJECT
public:
    explicit Icon(QObject *parent = 0);
    ~Icon();

    qint16 flags() {return m_flags;}
    void setFlags(qint16 flags) {m_flags = flags;}

    QImage image() const {return m_image;}
    void setImage(const QImage& image) {m_image = image;}

    void load(St2000DataStream& stream);
    static void skip(St2000DataStream& stream);

private:
    QImage m_image;
    qint16 m_flags;
};

}

#endif // ICON_H
