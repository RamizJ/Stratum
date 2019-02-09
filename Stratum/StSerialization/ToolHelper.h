#ifndef TOOLHELPER_H
#define TOOLHELPER_H

#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

template<typename T>
T* findTool(const QList<StData::Tool*>& tools, int handle)

{
    T* result = nullptr;
    for(StData::Tool* tool : tools)
    {
        if(T* specificTool = dynamic_cast<T*>(tool))
            if(specificTool->handle() == handle)
            {
                result = specificTool;
                break;
            }
    }
    return result;
}

}

#endif // TOOLHELPER_H
