#include "GuiContext.h"

GuiContext::GuiContext()
{
}
GuiContext::~GuiContext()
{}

GuiContext& GuiContext::instance()
{
    static GuiContext guiHelper;
    return guiHelper;
}
