#ifndef ANDROIDFILEDIALOG_H
#define ANDROIDFILEDIALOG_H

#include <QObject>

#include <QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidActivityResultReceiver>

class ResultReceiver;

class AndroidFileDialog : public QObject
{
    Q_OBJECT
public:
    explicit AndroidFileDialog(QObject *parent = nullptr);
    virtual ~AndroidFileDialog();

    bool provideExistingFileName();


signals:
     void existingFileNameReady(QString result);

public slots:

private:
     class ResultReceiver : public QAndroidActivityResultReceiver
     {
     public:
         ResultReceiver(AndroidFileDialog *dialog);
         virtual ~ResultReceiver();

         void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);
         QString uriToPath(QAndroidJniObject uri);

     private:
         AndroidFileDialog* m_dialog;
     };
    void emitExistingFileNameReady(QString result);

private:
    static const int EXISTING_FILE_NAME_REQUEST = 1;
    ResultReceiver* m_receiver;
};

#endif // ANDROIDFILEDIALOG_H
