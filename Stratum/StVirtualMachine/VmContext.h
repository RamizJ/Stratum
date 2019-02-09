#ifndef VMCONTEXT_H
#define VMCONTEXT_H

class VmContext
{
public:
    static bool isMessageBoxRaised();
    static void setIsMessageBoxRaised(bool isMessageBoxRaised);

    static void Send();

private:
    VmContext()
    {}

private:
    static bool m_isMessageBoxRaised;
};

#endif // VMCONTEXT_H
