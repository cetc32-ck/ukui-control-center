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
#ifndef POWER_H
#define POWER_H

#include <QObject>
#include <QtPlugin>
#include <QStyledItemDelegate>
#include <QGSettings>
#include <QDBusInterface>
#include <QDBusReply>
#include <QLabel>

#include "shell/interface.h"
#include "SwitchButton/switchbutton.h"

namespace Ui {
class Power;
}

class Power : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kycc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    explicit Power();
    ~Power();

public:
    QString get_plugin_name() Q_DECL_OVERRIDE;
    int get_plugin_type() Q_DECL_OVERRIDE;
    QWidget * get_plugin_ui() Q_DECL_OVERRIDE;
    void plugin_delay_control() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;

public:
    void initSearText();
    void setupComponent();
    void setupConnect();
    void initModeStatus();
    void initCustomPlanStatus();
    void resetCustomPlanStatus();
    void initPowerOtherStatus();
    void isPowerSupply();
    void initStyleSheet();
    void refreshUI();
    void writeConf(int is_saving);
    void initConnection();
private:
    Ui::Power *ui;

private:
    QWidget * pluginWidget;
    SwitchButton * allowdarkenBtn;

private:
    bool isConterCenterChecked;
    bool mFirstLoad;
    QGSettings * settings;
    QGSettings * m_qsettings;
    QGSettings * m_centerSettings;
    QString pluginName;
    QString checkedStatus = "custdom";
    int pluginType;

    QStringList sleepStringList;
    QStringList closeStringList;
    QStringList closeLidStringList;
    QStringList darkenStringList;
    QStringList lidStringList;
    QStringList buttonStringList;
    QStringList iconShowList;
    QStringList suspendList;
    QLabel * btIconLabel;
    QLabel * acIconLabel;

private:
    bool settingsCreate;
};

#endif // POWER_H
