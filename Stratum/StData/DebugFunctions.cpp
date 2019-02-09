#include "DebugFunctions.h"

void DebugFunctions::SaveDataToFile(const QString& fileName, const QByteArray& data)
{
    QFile file(fileName);
    if(file.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        QTextStream textStream(&file);

        for (int i = 0; i < data.count(); ++i)
        {
            quint8 b = data[i];
            textStream << data[i] << "\t"
                       << b << "\n";
        }

        textStream.flush();
        file.close();
    }
}
