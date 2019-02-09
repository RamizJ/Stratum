#ifndef HYPERBASE_H
#define HYPERBASE_H

#include "stdata_global.h"

#include <memory>
#include <QMap>
#include <QSize>

#include "StDataTypes.h"

namespace StData {

class STDATASHARED_EXPORT HyperBase
{
public:
    enum HyperDataType {HDT_HyperKey = 10, HDT_HyperSpace = 11};

    HyperBase();
    HyperBase(const HyperBase& other);
    HyperBase(const QMap<int, HyperDataItem*>& hyperData);

    SpaceHyperKeyPtr getSpaceHyperKey();
    SpaceItemHyperKeyPtr getSpaceItemHyperKey();

    bool hasHyperKeyData() const;

    void operator = (const HyperBase& other);

private:
    QMap<int, HyperDataItem*> m_hyperData;
};

/*--------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT SpaceHyperKey
{
public:
    enum SpaceWindowFlag
    {
        SWF_AUTOSCROLL = 1, SWF_HSCROLL = 2, SWF_VSCROLL = 4, SWF_SPACESIZE = 8,
        SWF_NORESIZE = 16, SWF_DIALOG = 32, SWF_POPUP = 64, SWF_MDI = 128,
        SWF_MODAL = 256, SWF_AUTOORG = 512, SWF_MAXIMIZE = 1024, SWF_MINIMIZE = 2048
    };

    SpaceHyperKey();
    SpaceHyperKey(const SpaceHyperKey& other);
    SpaceHyperKey(QByteArray data);

    int spaceWindoweFlags() const { return m_spaceWindowFlags; }

    bool isAutoOrg() const;
//    bool isCorrectPos() const;
    bool isSpaceSize() const;
    bool isNoResize() const;

    bool isAutoScroll() const;
    bool isHScrollEnabled() const;
    bool isVScrollEnabled() const;

    Qt::WindowFlags windowFlags() const;
    Qt::WindowState windowState() const;
    QSize windowSize() const { return m_windowSize; }

    bool isValid() const { return m_isValid; }

    void operator = (const SpaceHyperKey& other);

private:
    bool m_isValid;

    int m_spaceWindowFlags;
    QSize m_windowSize;
};


/*--------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT SpaceItemHyperKey
{
public:
    enum {Target = 1, WinName, Object, Mode, Effect, Time, Disabled, Add};
    enum OpenMode {OM_RunApp = 1, OM_OpenProject};

public:
    SpaceItemHyperKey();
    SpaceItemHyperKey(const SpaceItemHyperKey& other);
    SpaceItemHyperKey(const QByteArray& data);

    void operator =(const SpaceItemHyperKey& other);

    QString target() const { return m_target; }
    void setTarget(const QString& target) {m_target = target;}

    QString windowName() const { return m_windowName; }
    void setWindowName(const QString& windowName) {m_windowName = windowName;}

    QString object() const { return m_object; }
    void setObject(const QString& obj) {m_object = obj;}

    QString params() const { return m_params; }
    void setParams(const QString& params) {m_params = params;}

    quint32 openMode() const { return m_openMode; }
    void setOpenMode(qint32 openMode) {m_openMode = openMode;}

    QString effect() const { return m_effect; }
    void setEffect(const QString& effect) {m_effect = effect;}

    qint16 time() const { return m_time; }
    void setTime(qint16 time) {m_time = time;}

    bool disabled() const { return m_disabled; }
    void setDisabled(bool disabled) {m_disabled = disabled;}

    void load(const QByteArray& hyperKeyData);

private:
    QString m_target;
    QString m_windowName;
    QString m_object;
    QString m_params;
    quint32 m_openMode;
    QString m_effect;
    qint16 m_time;
    bool m_disabled;
};

/*--------------------------------------------------------------------------------*/
class STDATASHARED_EXPORT HyperKeyParams
{
public:
    HyperKeyParams(const QString& params);

    QString state() const { return m_state; }
    void setState(const QString& state) { m_state = state; }

    QString defScheme() const { return m_defScheme; }
    void setDefScheme(const QString& defScheme) { m_defScheme = defScheme; }

    bool isReadOnly() const { return m_isReadOnly; }
    void setIsReadOnly(bool isReadOnly) { m_isReadOnly = isReadOnly; }

    bool autoRun() const { return m_autoRun; }
    void setAutoRun(bool autoRun) { m_autoRun = autoRun; }

    bool autoEdit() const { return m_autoEdit; }
    void setAutoEdit(bool autoEdit) { m_autoEdit = autoEdit; }

    bool autoClose() const { return m_autoClose; }
    void setAutoClose(bool autoClose) { m_autoClose = autoClose; }

    bool editOnClose() const { return m_editOnClose; }
    void setEditOnClose(bool editOnClose) { m_editOnClose = editOnClose; }

    bool save() const { return m_save; }
    void setSave(bool save) { m_save = save; }

    bool hideWindows() const { return m_hideWindows; }
    void setHideWindows(bool hideWindows) { m_hideWindows = hideWindows; }

private:
    QString m_state;
    QString m_defScheme;

    bool m_isReadOnly;
    qint8 m_autoRun;     // (1 - авто запуск, 2 - нет , 0 - по умолчанию)
    bool m_autoEdit;    // показ схемы при загрузке проекта
    bool m_autoClose;   // авто закрытие по остановке проекта
    bool m_editOnClose; // показ схемы при остановке проекта
    qint8 m_save;        // запись при остановке: (нельзя записться, 1 - можно но только специально, 2 - запрос)
    bool m_hideWindows; // прятать окна всех проектов
};

}

#endif // HYPERBASE_H
