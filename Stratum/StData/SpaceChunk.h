#ifndef SPACECHUNK_H
#define SPACECHUNK_H

#include <QObject>

namespace StData {
class St2000DataStream;
class Space;
class SpaceChunkResult;

class SpaceChunk
{
public:
    enum Type
    {
        SomeCollection = 1000, PenCollection = 1004, BrushCollection = 1005, DibCollection = 1006, DoubleDibCollection = 1007,
        FontCollection = 1008, StringCollection = 1009, RasterTextCollection = 1010, RichTextElementCollection = 1011, Item3dCollection = 1012,
        SpaceItemsCollection = 1020, PrimaryCollection = 1021, ItemDataCollection = 1022, Material3dCollection = 1024,
        GroupItem = 1, GroupItem2d = 3, RGroupItem2d = 4, LineItem2d = 20,  BitmapItem2d = 21, DoubleBitmapItem2d = 22, TextItem2d = 23,
        UserItem2d = 25, ControlItem2d = 26,
        Group3d = 5, Object3d = 10, Camera3d = 11, Light3d = 12, View3d = 24,
        EditFrame = 50, RotateCenter = 51,
        PenTool = 101, BrushTool = 102, DibTool = 103, DoubleDibTool = 104, FontTool = 105, StringTool = 106, RichTextTool = 107,
        RefToDib2d = 110, RefToDoubleDib2d = 111, Material3d = 112, PngDib2d = 200, Space2dType = 17458, Space3dType = 17459,
        SpaceItemName = 205
    };

public:
    explicit SpaceChunk(StData::Space* space2d, StData::St2000DataStream* stream);
    ~SpaceChunk();

    StData::Space* space2d() const {return m_space2d;}

    qint16 code() const {return m_code;}
    qint16 size() const {return m_size;}
    qint64 pos() const {return m_pos;}

    Type type() const {return m_type;}

    SpaceChunkResult* result() const {return m_result;}

    void read();
    SpaceChunkResult* load();
    void toEndChunk();
    void skip();

    static SpaceChunkResult* load(Space* space, St2000DataStream* stream);

private:
    StData::Space* m_space2d;

    qint16 m_code;
    qint32 m_size;
    qint64 m_pos;

    Type m_type;

    StData::St2000DataStream* m_stream;
    SpaceChunkResult* m_result;
};

class SpaceChunkResult
{
public:
    explicit SpaceChunkResult(StData::Space* space, StData::St2000DataStream* stream);
    virtual ~SpaceChunkResult(){}

    StData::St2000DataStream* stream() const {return m_stream;}
    StData::Space* space() const {return m_space2d;}

    virtual void load(SpaceChunk* parentChunk = NULL) = 0;

    int loadingType() const { return m_loadingType; }
    void setLoadingType(int loadingType) { m_loadingType = loadingType; }

protected:
    StData::Space* m_space2d;
    StData::St2000DataStream* m_stream;
    int m_loadingType;
};

template<class T>
T loadChunk(Space* space, St2000DataStream* stream)
{
    SpaceChunk chunk(space, stream);
    chunk.read();
    SpaceChunkResult* result = chunk.load();
    return static_cast<T>(result);
}

}

#endif // SPACECHUNK_H
