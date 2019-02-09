#include "AndroidFunctions.h"
#include <QString>

#ifdef Q_OS_ANDROID

#include <QAndroidJniEnvironment>
#include <QtAndroid>


QString AndroidFunctions::getOpeningFileName()
{
    QString filePath;

    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid())
    {
        QAndroidJniObject intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
        if (intent.isValid())
        {
            QAndroidJniObject data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
            if (data.isValid())
            {
                filePath = uriToPath(data);
                // Here path.toString() returns the path of the input file
                //QMetaObject::invokeMethod(rootComponent, "setSourcePath", Q_ARG(QVariant, QVariant("file://" + path.toString())));
            }
        }
    }

    return filePath;
}

QString AndroidFunctions::uriToPath(QAndroidJniObject uri)
{
    if (uri.toString().startsWith("file:", Qt::CaseInsensitive))
    {
        return uri.callObjectMethod("getPath", "()Ljava/lang/String;").toString();
    }
    else
    {
        QAndroidJniObject mediaData = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");

        QAndroidJniEnvironment env;
        jobjectArray projection = env->NewObjectArray(1, env->FindClass("java/lang/String"), mediaData.object());


        QAndroidJniObject contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
        QAndroidJniObject cursor = contentResolver.callObjectMethod("query",
                                                                    "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;",
                                                                    uri.object<jobject>(), projection, 0, 0, 0);
        QAndroidJniObject DATA = QAndroidJniObject::fromString("_data");
        jint columnIndex = cursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I", DATA.object<jstring>());
        cursor.callMethod<jboolean>("moveToFirst", "()Z");
        QAndroidJniObject result = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex);
        return result.isValid() ? result.toString() : QString();
    }
}

#endif
