#ifndef STDATATYPES_H
#define STDATATYPES_H

#include <memory>

using namespace std;

namespace StData {

//Tools
class Tool;
class BrushTool;
class PenTool;
class RasterTextTool;
class TextureTool;
class TextureRefTool;
class FontTool;
class StringTool;
class ToolVisitorBase;

//Items
class SpaceItem;
class SpaceItem2d;
class ShapeItem2d;
class GroupItem;
class MaskedTextureItem2d;
class PolylineItem2d;
class TextItem2d;
class TextureItem2d;
class UserObject;
class PolylineArrow;
class ControlItem2d;

class Project;
class Class;
class Scheme;
class Space;

class SpaceLoader;
class St2000DataStream;
class GroupItemResult;

class HyperDataItem;
class SpaceHyperKey;
class SpaceItemHyperKey;
class SpaceItemVisitorBase;

//Pointers
typedef shared_ptr<BrushTool> BrushToolPtr;
typedef shared_ptr<GroupItem> GroupItemPtr;
typedef shared_ptr<PenTool> PenToolPtr;
typedef shared_ptr<PolylineArrow> PolylineArrowPtr;
typedef shared_ptr<Project> ProjectPtr;
typedef shared_ptr<RasterTextTool> RasterTextToolPtr;
typedef shared_ptr<Space> SpacePtr;
typedef shared_ptr<SpaceItem> SpaceItemPtr;
typedef shared_ptr<SpaceItemHyperKey> SpaceItemHyperKeyPtr;
typedef shared_ptr<TextureTool> TextureToolPtr;

typedef shared_ptr<SpaceHyperKey> SpaceHyperKeyPtr;
typedef shared_ptr<SpaceItemHyperKey> SpaceItemHyperKeyPtr;
typedef shared_ptr<SpaceItemVisitorBase> SpaceItemVisitorBasePtr;

}

#endif // STDATATYPES_H
