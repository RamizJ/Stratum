#include "Icon.h"

#include "Sc2000DataStream.h"

#include "Log.h"
#include "GraphicsHelper.h"

namespace StData {

Icon::Icon(QObject *parent) :
    QObject(parent),
    m_flags(0)
{}

Icon::~Icon()
{}

void Icon::load(St2000DataStream& stream)
{
    stream.readInt32();

    setFlags(stream.readInt16());
    QByteArray colorIndexes = stream.readBytes(512);
    setImage(GraphicsHelper::imageFromBits(colorIndexes));
}

void Icon::skip(St2000DataStream& stream)
{
    stream.skipRawData(518);
}

}
