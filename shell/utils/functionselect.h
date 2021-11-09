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
#ifndef FUNCTIONSELECT_H
#define FUNCTIONSELECT_H

#include <QDir>
#include <QList>
#include <QStack>
#include <QObject>
#include <QPluginLoader>
#include <QCoreApplication>

typedef struct _FuncInfo
{
    int type;
    int index;
    bool mainShow;
    QString nameString;
    QString namei18nString;
}FuncInfo;

typedef struct _RecordFunc
{
    int type;
    QString namei18nString;

}RecordFunc;

Q_DECLARE_METATYPE(_RecordFunc)
Q_DECLARE_METATYPE(_FuncInfo)

class FunctionSelect
{
public:
    explicit FunctionSelect();
    ~FunctionSelect();

public:
    static QList<QList<FuncInfo>> funcinfoListHomePage;
    static QList<QList<FuncInfo>> funcinfoList;
    static QStack<RecordFunc> recordFuncStack;
    static QList<QString> systemPluginName;
    static QList<QString> devicePluginName;
    static QList<QString> networkPluginName;
    static QList<QString> personalPluginName;
    static QList<QString> accountPluginName;
    static QList<QString> datePluginName;
    static QList<QString> updatePluginName;
    static QList<QString> securityPluginName;
    static QList<QString> appPluginName;
    static QList<QString> searchPluginName;

    static void initValue();
    static void initPluginName();
    static void pushRecordValue(int type, QString name);
    static void popRecordValue();

    static void loadHomeModule();
    static void loadModule(QList<FuncInfo> &systemList, QString name, QString i18nName, int type, bool isShow, QList<QString> pluginName);
};

#endif // FUNCTIONSELECT_H
