#ifndef KEYSTATEGRABBER_H
#define KEYSTATEGRABBER_H

#include <QObject>

namespace StVirtualMachine {

class KeyStateObserver : public QObject
{
    Q_OBJECT
public:
    explicit KeyStateObserver(QObject *parent = 0);

    void start();
    void stop();

    // QObject interface
public:
    bool eventFilter(QObject*, QEvent* event);

private:
    bool m_observe;
};

}

#endif // KEYSTATEGRABBER_H
