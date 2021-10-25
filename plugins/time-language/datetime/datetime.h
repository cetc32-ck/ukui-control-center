/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>

#include "shell/interface.h"
#include "changtime.h"
#include "shell/utils/mthread.h"

#include <QWidget>
#include <QLabel>
#include <QAbstractButton>
#include <QThread>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusReply>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QTimer>
#include <QSettings>
#include <QGSettings>
#include <QComboBox>

#include "timeBtn.h"
#include "Label/fixlabel.h"
#include "worldMap/timezonechooser.h"
#include "worldMap/zoneinfo.h"
#include "SwitchButton/switchbutton.h"
#include "HoverWidget/hoverwidget.h"
#include "AddBtn/addbtn.h"

/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif

extern "C" {
#include <glib.h>
#include <gio/gio.h>
}

#define TZ_DATA_FILE "/usr/share/zoneinfo/zoneUtc"
#define DEFAULT_TZ "Asia/Shanghai"

namespace Ui {
class DateTime;
}

class DateTime : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ukcc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    DateTime();
    ~DateTime();

    QString plugini18nName()   Q_DECL_OVERRIDE;
    int pluginTypes()       Q_DECL_OVERRIDE;
    QWidget * pluginUi()   Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;
    bool isShowOnHomePage() const Q_DECL_OVERRIDE;
    QIcon icon() const          Q_DECL_OVERRIDE;
    bool isEnable() const       Q_DECL_OVERRIDE;

    void initTitleLabel();
    void initUI();
    void initNtp();
    void initTimeShow();
    void initComponent();
    void initStatus();
    void connectToServer();
    bool fileIsExits(const QString& filepath);
    static void syncRTC();
    void setNtpFrame(bool visiable);
    void addTimezone(const QString& timezone);
    void newTimeshow(const QString& timezone);
    void initSetTime();
    void setTime();
    bool getSyncStatus();

public:
    QLabel       *syncNetworkRetLabel = nullptr;
    SwitchButton *syncTimeBtn         = nullptr; //网络时间同步按钮

private:
    Ui::DateTime *ui;
    QString      localizedTimezone;
    QString      pluginName;
    int          pluginType;
    QWidget      *pluginWidget;

    QGSettings     *m_formatsettings      = nullptr;

    QDBusInterface *m_datetimeiface       = nullptr;
    QDBusInterface *m_datetimeiproperties = nullptr;
    QDBusInterface *m_cloudInterface;

    QMap<QString, int> tzindexMapEn;
    QMap<QString, int> tzindexMapCN;

    SwitchButton *m_formTimeBtn           = nullptr;
    QLabel       *m_formTimeLabel         = nullptr;
    QTimer       *m_itimer                = nullptr;

    QLabel *syncNetworkLabel              = nullptr;

    TimeZoneChooser *m_timezone;
    ZoneInfo* m_zoneinfo;

    QDateTime current;
    bool mFirstLoad;

    int ntpComboxPreId;
    QComboBox *ntpCombox = nullptr;
    bool  changeZoneFlag = false;
    QStringList timezonesList;
    QString localZone;
Q_SIGNALS:
    void changed();

private slots:
    void datetimeUpdateSlot();
    void changetimeSlot();
    void changezoneSlot(QString zone);
    void timeFormatClickedSlot(bool, bool);
    void synctimeFormatSlot(bool status,bool outChange);
    QDBusMessage rsyncWithNetworkSlot(bool status);
    void keyChangedSlot(const QString &key);
    bool setNtpAddr(QString address);

private:
    void changezoneSlot(int flag = 0);
    void loadHour();
    void setCurrentTime();
    void initConnect();

public:
    static QString getLocalTimezoneName(QString timezone, QString locale);
    QString getTimeAndWeek(const QDateTime timeZone);
};


class CGetSyncRes : public QThread{
    Q_OBJECT
public:
    CGetSyncRes(DateTime *dataTimeUI,QString successMSG,QString failMSG);
    ~CGetSyncRes();
protected:
    void run()override;
private:
    DateTime *dataTimeUI;
    QString successMSG;
    QString failMSG;
    bool changeLableFlag;
};

class CSyncTime : public QThread{
    Q_OBJECT
public:
    CSyncTime(DateTime *dataTimeUI,QString successMSG,QString failMSG);
    ~CSyncTime();
protected:
    void run()override;
private:
    DateTime *dataTimeUI;
    QString successMSG;
    QString failMSG;
};

#endif // DATETIME_H
