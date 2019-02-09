#include "JsonTextureToolSerializer.h"

#include <PathManager.h>
#include <QBuffer>
#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonTextureToolSerializer::JsonTextureToolSerializer()
{}

QJsonObject JsonTextureToolSerializer::serialize(TextureTool* tool)
{
    QJsonObject result = JsonToolSerializer::serialize(tool);

    if(auto textureRefTool = dynamic_cast<TextureRefTool*>(tool))
    {
        result[KEY_TEXTURE_PATH] = textureRefTool->reference();
    }
    else
    {
        QByteArray pixmapBytes;
        QBuffer buffer(&pixmapBytes);
        buffer.open(QBuffer::WriteOnly);
        tool->pixmap().save(&buffer, "PNG");

        result[KEY_TEXTURE_PIXMAP] = QString::fromLatin1(pixmapBytes.toBase64());

        if(!tool->pixmap().mask().isNull())
        {
            QByteArray pixmapBytes;
            QBuffer buffer(&pixmapBytes);
            buffer.open(QBuffer::WriteOnly);
            tool->pixmap().mask().save(&buffer, "PNG");
            result[KEY_TEXTURE_MASK] = QString::fromLatin1(pixmapBytes.toBase64());
        }
    }

    return result;
}

TextureTool* JsonTextureToolSerializer::deserialize(const QJsonObject& toolData, const QDir& spaceDir)
{
    if(toolData.isEmpty()) return nullptr;

    int textureHandle = toolData.value(KEY_TOOL_HANDLE).toInt();

    if(toolData.contains(KEY_TEXTURE_PATH))
    {
        QJsonValue texturePath = toolData.value(KEY_TEXTURE_PATH);

        auto refTextureTool = new TextureRefTool;
        refTextureTool->setHandle(textureHandle);

        QString ref = texturePath.toString();
        QString pixmapPath = PathManager::absolutePath(spaceDir, ref);
        refTextureTool->setPixmapPath(ref);
        refTextureTool->setPixmap(QPixmap(pixmapPath));

        return refTextureTool;
    }

    QPixmap pixmap;
    QByteArray pixmapBytes = QByteArray::fromBase64(toolData.value(KEY_TEXTURE_PIXMAP).toString().toLatin1());
    pixmap.loadFromData(pixmapBytes, "PNG");

    bool containMask = toolData.contains(KEY_TEXTURE_MASK);
    if(containMask)
    {
        QJsonValue texureMask = toolData.value(KEY_TEXTURE_MASK);
        QByteArray maskBytes = QByteArray::fromBase64(texureMask.toString().toLatin1());
        QBitmap mask;
        mask.loadFromData(maskBytes, "PNG");
        pixmap.setMask(mask);
    }

    auto textureTool = containMask ? new MaskedTextureTool : new TextureTool;
    textureTool->setHandle(textureHandle);
    textureTool->setPixmap(pixmap);

    return textureTool;
}

}
