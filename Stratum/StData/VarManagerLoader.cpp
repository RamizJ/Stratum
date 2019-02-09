#include "VarManagerLoader.h"

#include "Sc2000DataStream.h"
#include "VarManager.h"
#include "StDataGlobal.h"

#include <QTextCodec>

namespace StData {

ProjectVarsLoader::ProjectVarsLoader()
{}

ProjectVarsLoader::~ProjectVarsLoader()
{}

void ProjectVarsLoader::load(St2000DataStream& stream, ProjectVarCollection* varManager)
{
    qint16 count = stream.readInt16();

    for (qint16 i=0; i < count; i++)
    {
        QByteArray varData = stream.readBytes();
        if(varData.size() > 0)
        {
            ProjectVariable* variable = new ProjectVariable;
            variable->setType(ProjectVariable::VarType((int)varData[0]));

            qint8 nameLength = (qint8)varData[1];
            QString name;
            for(int j = 2; j <= nameLength; j++)
                name.push_back(varData.at(j));
            variable->setName(name);

            QByteArray value;
            for(int j = nameLength+2; j < varData.length(); ++j)
                value.push_back(varData.at(j));

            St2000DataStream readValueStream(value);
            switch (variable->type())
            {
                case ProjectVariable::Int:
                {
                    qint32 intValue(0);
                    readValueStream >> intValue;
                    variable->setValue(intValue);
                }break;

                case ProjectVariable::Double:
                {
                    double doubleValue(0.0);
                    readValueStream >> doubleValue;
                    variable->setValue(doubleValue);
                }break;

                case ProjectVariable::String:
                {
//                    variable->setValue(QString::fromLocal8Bit(value));
                    variable->setValue(fromWindows1251(value));

                }break;
            }
            varManager->addVariable(variable);
        }
    }
}

}
