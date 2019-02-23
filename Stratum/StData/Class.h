#ifndef CLASS_H
#define CLASS_H

#include "stdata_global.h"
#include "FlagHelper.h"

#include <QFileInfo>
#include <QMap>

namespace StData
{
class Project;
class Library;
class Equation;
class St2000DataStream;
class ClassVarInfo;
class LinkInfo;
class ObjectInfo;
class Scheme;
class Icon;
class Project;
class Object;

class STDATASHARED_EXPORT Class
{
public:
    enum class Flag
    {
        Protected = 1, OneFase = 2, Compiled = 4,
        Virtual = 8,
        Scheme = 16,
        Modify = 32,
        Disabled = 64,
        BuildIn = 256, Icon = 512, Procedure = 1024, Structure = 2048, UseDefIcon = 4096, Sizeable = 8192, UnDisabled = 16384,
        NoSaveVar = 32768ul, AutoLink = 0x10000L, AutoRmLink = 0x20000L,
    };
    Q_DECLARE_FLAGS(Flags, Flag)

public:
    explicit Class(const QFileInfo& fileInfo, Library* library);
    ~Class();

    QFileInfo fileInfo() const {return m_fileInfo;}
    Library* library() {return m_library;}

    QString originalName();
    void setOriginalName(const QString& name);

    QString cleanName() const { return m_cleanName; }

    quint32 fileVersion() const {return m_fileVersion;}
    void setFileVersion(const quint32& fileVersion) {m_fileVersion = fileVersion;}

    Equation* equation();
    void setEquation(Equation* equation) {m_equation = equation;}

    QString helpFile();
    void setHelpFile(const QString& helpFile) {m_helpFile = helpFile;}

    quint32 classId() const {return m_classId;}
    void setClassId(const quint32& classId) {m_classId = classId;}

    QString iconFileName();
    void setIconFileName(const QString& iconFileName) {m_iconFileName = iconFileName;}

    qint16 defIcon();
    void setDefIcon(qint16 defIcon) {m_defIcon = defIcon;}

    qint32 flags();
    void setFlags(const qint32& flags) {m_flags = flags;}

    QString text();
    void setText(const QString& text) {m_text = text;}

    QString description();
    void setDescription(const QString& description) {m_description = description;}

    Icon* icon() {return m_icon;}
    void setIcon(Icon* icon) {m_icon = icon;}

    Scheme* image();
    void setImage(Scheme* image) {m_image = image;}

    Scheme* scheme();
    void setScheme(Scheme* scheme) {m_scheme = scheme;}

    const QByteArray& vmCode() const { return m_codeBytes; }
    void setCodeBytes(const QByteArray& codeBytes) { m_codeBytes = codeBytes; }
    void clearCodeBytes() { m_codeBytes.clear(); }

    qint16 varSize() const { return m_varSize; }

    QMap<qint16, LinkInfo*> linkDictionary();
    QList<LinkInfo*> links();
    bool isLinkExist(qint16 linkId);
    LinkInfo* linkById(qint16 linkId);
    void addLink(LinkInfo* link);

    QList<ObjectInfo*> objectInfoList() const { return m_childInfoList; }
    QMap<qint16, ObjectInfo*> objectInfoDictionary() { return m_objectInfoForHandle; }
    bool isObjectInfoExist(qint16 objectInfoId) const;
    ObjectInfo* objectInfoByHandle(qint16 handle) const;
    void addObjectInfo(ObjectInfo* objectInfo);
    int childsCount() const;
    int childHandleByIndex(int index);
    int childIndexByHandle(int handle);
    bool setChildCalcOrder(int handle, int order);

    QList<ClassVarInfo*> varInfoList();
    bool addVarInfo(ClassVarInfo* var);
    void deleteAllVariables();
    ClassVarInfo* varInfoByName(const QString& varName);
    ClassVarInfo* varInfoByIndex(int index);

    void load();
    void loadName();

    bool isNameLoaded() const {return m_isNameLoaded;}
    bool isLoaded() const {return m_isLoaded;}

    void inline setModify() {m_flags |= (qint32)Flag::Modify;}
    bool isProtected();

    bool isProcedure() {return (m_flags & (qint32)Flag::Procedure) > 0;}
    bool isStructure() {return (m_flags & (qint32)Flag::Structure) > 0;}

    void addObject(Object* object);
    QList<Object*> objects() const {return m_objects;}

    void recreateObjectVars();
    void updateDisableVars();

private:
    enum class DataCode
    {
        Undefined = 0, Flags = 4, Vars = 5, Childs = 6, Links = 7, Ico = 8, Schm = 9, Text = 10,
        VarSize = 11, Image = 12, Code = 13, Info = 14, VarsWithDefValue = 15, ChildsName = 16, ChildsNameXY = 17,
        DefIcon = 20, ClassTime = 21, IconFile = 22, HelpFile = 25, Equ = 30,
        ClassSignature = 0x4253, PackSignature = 0x4C53,
    };

private:
    DataCode readDataCode(StData::St2000DataStream& stream);

private:
    QFileInfo m_fileInfo;
    Library* m_library;

    QString m_originalName;
    QString m_cleanName;
    QString m_helpFile;
    QString m_iconFileName;
    qint16 m_defIcon;
    QString m_text;
    QString m_description;

    quint32 m_fileVersion;
    quint32 m_classId;
    qint32 m_codeSize;
    qint16 m_varSize;
    qint32 m_flags;
    quint32 m_classDataPos;
    quint32 m_classDataSize;
//    quint32 m_loadingFlags;

//    qint16* m_vmCode;
    QByteArray m_codeBytes;
    Equation* m_equation;
    Scheme* m_scheme;
    Scheme* m_image;
    Icon* m_icon;

    QList<ObjectInfo*> m_childInfoList;
    QMap<qint16, ObjectInfo*> m_objectInfoForHandle;

    QMap<qint16, LinkInfo*> m_linkForHandle;

    QList<ClassVarInfo*> m_varInfoList;
    QMap<QString, ClassVarInfo*> m_varInfoForName;

    bool m_isLoaded;
    bool m_isNameLoaded;

    QList<Object*> m_objects;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Class::Flags)


class ClassComparer
{
public:
    ClassComparer();
    bool operator < (const ClassComparer& other);

private:
};

}
#endif // CLASS_H
