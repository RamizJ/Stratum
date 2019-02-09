#include "VmContext.h"

bool VmContext::m_isMessageBoxRaised = false;


bool VmContext::isMessageBoxRaised()
{
    return m_isMessageBoxRaised;
}

void VmContext::setIsMessageBoxRaised(bool isMessageBoxRaised)
{
    m_isMessageBoxRaised = isMessageBoxRaised;
}
