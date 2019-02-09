#ifndef GUICONTEXT_H
#define GUICONTEXT_H

class EditObjectDialog;
class TabManager;

class GuiContext
{
public:
    ~GuiContext();

public:
    static GuiContext& instance();

    TabManager* tabManager() const {return m_tabManager;}
    void setTabManager(TabManager* tabManager) {m_tabManager = tabManager;}

private:
    GuiContext();

    GuiContext(const GuiContext&);
    GuiContext& operator=(const GuiContext&);

private:
    TabManager* m_tabManager;
};

#endif // GUICONTEXT_H
