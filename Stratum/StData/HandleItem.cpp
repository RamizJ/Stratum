#include "HandleItem.h"

namespace StData {

HandleItem::HandleItem(int handle) :
    m_handle(handle)
{}

HandleItem::HandleItem(const HandleItem& other) :
    m_handle(other.handle())
{}

}
