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
#include <QtWidgets>

#include "display.h"
#include "ui_display.h"

#include <KF5/KScreen/kscreen/getconfigoperation.h>
#include <KF5/KScreen/kscreen/output.h>
#include <QDebug>

DisplaySet::DisplaySet() : mFirstLoad(true)
{
    pluginName = tr("Displays");
    pluginType = DEVICES;
}

DisplaySet::~DisplaySet(){

}

QWidget *DisplaySet::get_plugin_ui(){
    if (mFirstLoad) {
        mFirstLoad = false;
        pluginWidget = new DisplayWidget;
        pluginWidget->setAttribute(Qt::WA_StyledBackground,true);
        QObject::connect(new KScreen::GetConfigOperation(), &KScreen::GetConfigOperation::finished,
                         [&](KScreen::ConfigOperation *op) {
            pluginWidget->setConfig(qobject_cast<KScreen::GetConfigOperation*>(op)->config());
        });
    }
    return pluginWidget;
}

QString DisplaySet::get_plugin_name(){
    return pluginName;
}

int DisplaySet::get_plugin_type(){
    return pluginType;
}

void DisplaySet::plugin_delay_control(){

}

const QString DisplaySet::name() const {

    return QStringLiteral("display");
}

