#include "HyperBase.h"
#include "Sc2000DataStream.h"
#include "SpaceItem.h"
#include "FlagHelper.h"

#include <QStringList>

namespace StData {

HyperBase::HyperBase()
{}

HyperBase::HyperBase(const HyperBase& other) :
    m_hyperData(other.m_hyperData)
{}

HyperBase::HyperBase(const QMap<int, HyperDataItem*>& hyperData) :
    m_hyperData(hyperData)
{}

SpaceHyperKeyPtr HyperBase::getSpaceHyperKey()
{
    if(HyperDataItem* dataItem = m_hyperData.value(HDT_HyperSpace, nullptr))
        return std::make_shared<SpaceHyperKey>(dataItem->data());

    return nullptr;
}

SpaceItemHyperKeyPtr HyperBase::getSpaceItemHyperKey()
{
    if(HyperDataItem* dataItem = m_hyperData.value(HDT_HyperKey, nullptr))
        return std::make_shared<SpaceItemHyperKey>(dataItem->data());

    return nullptr;
}

bool HyperBase::hasHyperKeyData() const
{
    return m_hyperData.contains(HDT_HyperKey);
}

void HyperBase::operator =(const HyperBase& other)
{
    m_hyperData = other.m_hyperData;
}

/*--------------------------------------------------------------------------------*/
SpaceHyperKey::SpaceHyperKey() :
    m_isValid(false)
{}

SpaceHyperKey::SpaceHyperKey(const SpaceHyperKey& other) :
    m_isValid(other.m_isValid),
    m_windowSize(other.m_windowSize),
    m_spaceWindowFlags(other.m_spaceWindowFlags)
{}

SpaceHyperKey::SpaceHyperKey(QByteArray data) :
    m_isValid(false)
{
    if(data.length() < 8)
        return;

    St2000DataStream stream(&data, QIODevice::ReadOnly);

    m_spaceWindowFlags = stream.readInt32();
    int wndWidth = stream.readInt16();
    int wndHeight = stream.readInt16();
    m_windowSize = QSize(wndWidth, wndHeight);

    m_isValid = true;
}

bool SpaceHyperKey::isAutoOrg() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_AUTOORG);
}

bool SpaceHyperKey::isSpaceSize() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_SPACESIZE);
}

bool SpaceHyperKey::isNoResize() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_NORESIZE);
}

bool SpaceHyperKey::isAutoScroll() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_AUTOSCROLL);
}

bool SpaceHyperKey::isHScrollEnabled() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_HSCROLL);
}

bool SpaceHyperKey::isVScrollEnabled() const
{
    return FLAG_EQUAL(m_spaceWindowFlags, SWF_VSCROLL);
}

Qt::WindowFlags SpaceHyperKey::windowFlags() const
{
    Qt::WindowFlags flags;
    if(FLAG_EQUAL(m_spaceWindowFlags, SWF_DIALOG))
        flags = Qt::Dialog | Qt::WindowCloseButtonHint;

    else if(FLAG_EQUAL(m_spaceWindowFlags, SWF_POPUP))
        flags = Qt::Popup;

//    if(FLAG_EQUAL(m_spaceWindowFlags, SWF_NORESIZE))
//        flags = Qt::Dialog | Qt::WindowCloseButtonHint;

    return flags;
}

Qt::WindowState SpaceHyperKey::windowState() const
{
    Qt::WindowState state = Qt::WindowActive;

    if(FLAG_EQUAL(m_spaceWindowFlags, SWF_MINIMIZE))
        state = Qt::WindowMinimized;

    else if(FLAG_EQUAL(m_spaceWindowFlags, SWF_MAXIMIZE))
        state = Qt::WindowMaximized;

    return state;
}

void SpaceHyperKey::operator =(const SpaceHyperKey& other)
{
    m_isValid = other.m_isValid;
    m_spaceWindowFlags = other.m_spaceWindowFlags;
    m_windowSize = other.m_windowSize;
}

/*--------------------------------------------------------------------------------*/
SpaceItemHyperKey::SpaceItemHyperKey()
{}

SpaceItemHyperKey::SpaceItemHyperKey(const SpaceItemHyperKey& other) :
    m_target(other.m_target),
    m_windowName(other.m_windowName),
    m_object(other.m_object),
    m_params(other.m_params),
    m_openMode(other.m_openMode),
    m_effect(other.m_effect),
    m_time(other.m_time)
{}

SpaceItemHyperKey::SpaceItemHyperKey(const QByteArray& data) :
    m_openMode(-1)
{
    load(data);
}

void SpaceItemHyperKey::load(const QByteArray& hyperKeyData)
{
    if(hyperKeyData.isEmpty())
        return;

    QByteArray data = hyperKeyData;

    St2000DataStream stream(&data, QIODevice::ReadOnly);
    int code = stream.readInt16();
    while(code != 0)
    {
        switch(code)
        {
            case Target:
                m_target = stream.readString();
                break;

            case WinName:
                m_windowName = stream.readString();
                break;

            case Object:
                m_object = stream.readString();
                break;

            case Mode:
                m_openMode = stream.readInt16();
                break;

            case Effect:
                m_effect = stream.readString();
                break;

            case Time:
                m_time = stream.readInt16();
                break;

            case Add:
                m_params = stream.readString();
                break;

            case Disabled:
                m_disabled = true;
                break;
        }
        code = stream.readInt16();
    }
}

void SpaceItemHyperKey::operator =(const SpaceItemHyperKey& other)
{
    m_disabled = other.m_disabled;
    m_effect = other.m_effect;
    m_object = other.m_object;
    m_openMode = other.m_openMode;
    m_params = other.m_params;
    m_target = other.m_target;
    m_time = other.m_time;
    m_windowName = other.m_windowName;
}

/*--------------------------------------------------------------------------------*/
HyperKeyParams::HyperKeyParams(const QString& params) :
    m_isReadOnly(false),
    m_autoRun(0),
    m_autoEdit(false),
    m_autoClose(false),
    m_editOnClose(false),
    m_hideWindows(false)
{
    QStringList keyWords = {"readonly","scheme:","state:","open:","stop:","save:","hidewindows"};
    QStringList args = {"run","edit","close","stop","prompt","no","return"};

    m_save = 0;
    for(int charIndex = 0; charIndex < params.length(); charIndex++)
    {
        if(params.at(charIndex) == ' ')
            continue;

        if(params.at(charIndex) != '/')
            continue;

        for(int keyWordIndex = 0; keyWordIndex < keyWords.length(); keyWordIndex++)
        {
            int keyWordLength = keyWords.at(keyWordIndex).length();

            QString keyWord = params.mid(charIndex+1, keyWordLength);
            if(QString::compare(keyWord, keyWords.at(keyWordIndex), Qt::CaseInsensitive) != 0)
                continue;

            charIndex += keyWordLength;
            switch(keyWordIndex)
            {
                case 0:
                    m_isReadOnly = true;
                    break;

                case 1:
                case 2:
                {
                    //TODO state
                }break;

                case 6:
                    m_hideWindows = true;
                    break;

                default:
                {
                    bool argsInProcess = false;
                    do
                    {
                        for(;charIndex < params.length() && params.at(charIndex) == ' '; charIndex++);
                        if(charIndex == params.length())
                            break;

                        for(int argIndex = 0; argIndex < args.length(); argIndex++)
                        {
                            int argLength = args.at(argIndex).length();

                            QString arg = params.mid(charIndex + 1, argLength);
                            if(QString::compare(arg, args.at(argIndex), Qt::CaseInsensitive) != 0)
                                continue;

                            charIndex += argLength;
                            switch(keyWordIndex)
                            {
                                case 3: //open
                                    switch(argIndex)
                                    {
                                        case 0: m_autoRun = 1; break;
                                        case 1: m_autoEdit = true; break;
                                        case 3: m_autoRun = 2; break;
                                    }
                                    break;

                                case 4: //stop
                                    switch(argIndex)
                                    {
                                        case 0: m_editOnClose = true; break;
                                        case 6:
                                        case 2: m_autoClose = true; break;
                                    }
                                    break;

                                case 5: //save
                                    switch(argIndex)
                                    {
                                        case 4: m_save = 2; break;
                                        case 5: m_save = 0; break;
                                    }
                                    break;
                            }
                        }
                        for(;charIndex < params.length() && params.at(charIndex) == ' '; charIndex++);
                        argsInProcess = (charIndex < params.length() && params.at(charIndex) == ',');
                    }while(argsInProcess);
                }break;
            }
        }
    }
}

}
