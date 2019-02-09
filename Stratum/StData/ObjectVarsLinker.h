#ifndef LINK_H
#define LINK_H

#include <QObject>
#include <QStack>
#include <memory>

namespace StData {

struct VarData;
class Object;
class LinkInfo;
class ObjectVar;

typedef std::shared_ptr<VarData> VarDataPtr;

class ObjectVarsLinker : public QObject
{
    Q_OBJECT

public:
    ObjectVarsLinker(QObject* parent = nullptr);
    ~ObjectVarsLinker();

    void link(bool saveVarsData);

    Object* rootObject() const { return m_rootObject; }
    void setRootObject(Object* rootObject) { m_rootObject = rootObject; }

private:
//    void linkObject(StData::Object* containerObject, Object* object, QList<StData::Object*>& handledObjects);
    void linkVars(Object* object);
    QList<ObjectVar*> findLinkedVars(Object* object, ObjectVar* var);

    void storeVarsData(Object* object, QStack<VarData>* varDataStack);
    void restoreVarsData(Object* object, QStack<VarData>* varDataStack);

    void cleanVarsData(Object* object);
    void cleanObjectVarsData(Object* object);

//    void createUnlinkedVarsData(Object* object);

//    QList<LinkInfo*> getObjectLinks(StData::Object* containerObject, Object* object);

//    void linkVars(LinkInfo* link, Object* sourceObj, Object* targetObj);

private:
    Object* m_rootObject;
    QList<Object*> m_handledLinkedObjects;
};

}

#endif // LINK_H
