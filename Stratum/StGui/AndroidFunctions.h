#ifndef ANDROIDFUNCTIONS_H
#define ANDROIDFUNCTIONS_H

#include <QString>

#ifdef Q_OS_ANDROID


#include <QAndroidJniObject>

class AndroidFunctions
{
public:
    static QString getOpeningFileName();
    static QString uriToPath(QAndroidJniObject uri);
};

#endif

#endif // ANDROIDFUNCTIONS_H
