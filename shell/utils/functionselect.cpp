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
#include "functionselect.h"
#include "../interface.h"

#include <QDebug>

QList<QList<FuncInfo>> FunctionSelect::funcinfoList;
QStack<RecordFunc> FunctionSelect::recordFuncStack;

//FuncInfo FunctionSelect::displayStruct;


FunctionSelect::FunctionSelect()
{
}

FunctionSelect::~FunctionSelect()
{
}

void FunctionSelect::initValueSystem() {
    QList<FuncInfo> systemList;
    for (int i = 0; i < TOTALSYSFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = SYSTEM;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        systemList.append(funcStruct);
    }
    systemList[DISPLAY].nameString = QString("Display");
    systemList[DISPLAY].namei18nString =  QObject::tr("Display");
    systemList[AUDIO].nameString = QString("Audio");
    systemList[AUDIO].namei18nString = QObject::tr("Audio");
    systemList[POWER].nameString = QString("Power");
    systemList[POWER].namei18nString = QObject::tr("Power");
    systemList[NOTICE].nameString = QString("Notice");
    systemList[NOTICE].namei18nString = QObject::tr("Notice");
    systemList[NOTICE].mainShow = false;
    systemList[VINO].nameString = QString("Vino");
    systemList[VINO].namei18nString = QObject::tr("Vino");
    systemList[VINO].mainShow = false;
    systemList[PROJECTION].nameString = QString("Projection");
    systemList[PROJECTION].namei18nString = QObject::tr("Projection");
    systemList[PROJECTION].mainShow = false;
    systemList[ABOUT].nameString = QString("About");
    systemList[ABOUT].namei18nString = QObject::tr("About");
    systemList[EXPERIENCEPLAN].nameString = QString("Experienceplan");
    systemList[EXPERIENCEPLAN].namei18nString = QObject::tr("Experienceplan");
    systemList[EXPERIENCEPLAN].mainShow = false;

    funcinfoList.append(systemList);
}

void FunctionSelect::initValueDevice() {
    QList<FuncInfo> devicesList;
    for (int i = 0; i < TOTALDEVICESFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = DEVICES;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        devicesList.append(funcStruct);
    }
    devicesList[BLUETOOTH].nameString = QString("Bluetooth");
    devicesList[BLUETOOTH].namei18nString = QObject::tr("Bluetooth");
    devicesList[PRINTER].nameString = QString("Printer");
    devicesList[PRINTER].namei18nString = QObject::tr("Printer");
    devicesList[PRINTER].mainShow = false;    
    devicesList[MOUSE].nameString = QString("Mouse");
    devicesList[MOUSE].namei18nString = QObject::tr("Mouse");
    devicesList[TOUCHPAD].nameString = QString("Touchpad");
    devicesList[TOUCHPAD].namei18nString = QObject::tr("Touchpad");
    devicesList[TOUCHSCREEN].nameString = QString("TouchScreen");
    devicesList[TOUCHSCREEN].namei18nString =  QObject::tr("TouchScreen");
    devicesList[TOUCHSCREEN].mainShow = false;
    devicesList[KEYBOARD].nameString = QString("Keyboard");
    devicesList[KEYBOARD].namei18nString = QObject::tr("Keyboard");
    devicesList[SHORTCUT].nameString = QString("Shortcut");
    devicesList[SHORTCUT].namei18nString = QObject::tr("Shortcut");
    devicesList[SHORTCUT].mainShow = false;
    
    funcinfoList.append(devicesList);
}

void FunctionSelect::initValueNetwork() {
    QList<FuncInfo> networkList;
    for (int i = 0; i < TOTALNETFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = NETWORK;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        networkList.append(funcStruct);
    }
    networkList[WIREDCONNECT].nameString = QString("WiredConnect");
    networkList[WIREDCONNECT].namei18nString = QObject::tr("WiredConnect");
    networkList[WLANCONNECT].nameString = QString("Wlanconnect");
    networkList[WLANCONNECT].namei18nString = QObject::tr("WlanConnect");
    networkList[VPN].nameString = QString("Vpn");
    networkList[VPN].namei18nString = QObject::tr("Vpn");
    networkList[PROXY].nameString = QString("Proxy");
    networkList[PROXY].namei18nString = QObject::tr("Proxy");
    networkList[PROXY].mainShow = false;
    
    funcinfoList.append(networkList);
}

void FunctionSelect::initValuePersonal() {
    QList<FuncInfo> personalizedList;
    for (int i = 0; i < TOTALPERSFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = PERSONALIZED;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        personalizedList.append(funcStruct);
    }
    personalizedList[THEME].nameString = QString("Theme");
    personalizedList[THEME].namei18nString = QObject::tr("Theme");
    personalizedList[BACKGROUND].nameString = QString("Background");
    personalizedList[BACKGROUND].namei18nString = QObject::tr("Background");
    personalizedList[SCREENLOCK].nameString = QString("Screenlock");
    personalizedList[SCREENLOCK].namei18nString = QObject::tr("Screenlock");
    personalizedList[SCREENLOCK].mainShow = false;
    personalizedList[SCREENSAVER].nameString = QString("Screensaver");
    personalizedList[SCREENSAVER].namei18nString = QObject::tr("Screensaver");
    personalizedList[FONTS].nameString = QString("Fonts");
    personalizedList[FONTS].namei18nString = QObject::tr("Fonts");
    personalizedList[DESKTOP].nameString = QString("Desktop");
    personalizedList[DESKTOP].namei18nString = QObject::tr("Desktop");
    personalizedList[DESKTOP].mainShow = false;

    funcinfoList.append(personalizedList); 
}

void FunctionSelect::initValueAccount() {
    QList<FuncInfo> accountList;
    for (int i = 0; i < TOTALACCOUNTFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = ACCOUNT;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        accountList.append(funcStruct);
    }
    accountList[USERINFO].nameString = QString("Userinfo");
    accountList[USERINFO].namei18nString = QObject::tr("User Info");
    accountList[NETWORKACCOUNT].nameString = QString("Cloud Account");
    accountList[NETWORKACCOUNT].namei18nString = QObject::tr("Cloud Account");
    accountList[BIOMETRICS].nameString = QString("Biometrics");
    accountList[BIOMETRICS].namei18nString = QObject::tr("Biometrics");
     accountList[BIOMETRICS].mainShow = false;

    funcinfoList.append(accountList);
}

void FunctionSelect::initValueDatetime() {
    QList<FuncInfo> datetimeList;
    for (int i = 0; i < TOTALDTFUNC; i++){
        FuncInfo funcStruct;
        funcStruct.type = DATETIME;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        datetimeList.append(funcStruct);
    }
    datetimeList[DAT].nameString = QString("Date");
    datetimeList[DAT].namei18nString = QObject::tr("Date");
    datetimeList[AREA].nameString = QString("Area");
    datetimeList[AREA].namei18nString = QObject::tr("Area");

    funcinfoList.append(datetimeList);
}

void FunctionSelect::initValueUpdate() {
    QList<FuncInfo> updatesList;
    for (int i = 0; i < TOTALUPDATE; i++){
        FuncInfo funcStruct;
        funcStruct.type = UPDATE;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        updatesList.append(funcStruct);
    }
    updatesList[BACKUP].nameString = QString("Backup");
    updatesList[BACKUP].namei18nString = QObject::tr("Backup");
    updatesList[UPDATES].nameString = QString("Update");
    updatesList[UPDATES].namei18nString = QObject::tr("Update");
    updatesList[UPGRADE].nameString = QString("Upgrade");
    updatesList[UPGRADE].namei18nString = QObject::tr("Upgrade");

    funcinfoList.append(updatesList);
}

void FunctionSelect::initValueSecurity() {
    QList<FuncInfo> securityList;
    for (int i = 0; i < TOTALSECURITY; i++){
        FuncInfo funcStruct;
        funcStruct.type = SECURITY;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        securityList.append(funcStruct);
    }
    securityList[SECURITYCENTER].nameString = QString("SecurityCenter");
    securityList[SECURITYCENTER].namei18nString = QObject::tr("Security Center");

    funcinfoList.append(securityList);
}

void FunctionSelect::initValueApp() { 
    QList<FuncInfo> appList;
    for (int i = 0; i < TOTALAPP; i++){
        FuncInfo funcStruct;
        funcStruct.type = APPLICATION;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        appList.append(funcStruct);
    }
    appList[DEFAULTAPP].nameString = QString("Defaultapp");
    appList[DEFAULTAPP].namei18nString =  QObject::tr("Default App");
    appList[AUTOBOOT].nameString = QString("Autoboot");
    appList[AUTOBOOT].namei18nString = QObject::tr("Auto Boot");

    funcinfoList.append(appList);
}

void FunctionSelect::initValueSearch() { 
    QList<FuncInfo> searchList;
    for (int i = 0; i < TOTALAPP; i++){
        FuncInfo funcStruct;
        funcStruct.type = SEARCH_F;
        funcStruct.index = i;
        funcStruct.mainShow = true;
        searchList.append(funcStruct);
    }
    searchList[SEARCH].nameString = QString("Search");
    searchList[SEARCH].namei18nString = QObject::tr("Search");

    funcinfoList.append(searchList);
}

void FunctionSelect::initValue(){
    initValueSystem();
    initValueDevice();
    initValueNetwork();
    initValuePersonal();
    initValueAccount();
    initValueDatetime();
    initValueUpdate();
    initValueSecurity();
    initValueApp();
    initValueSearch();
}

void FunctionSelect::pushRecordValue(int type, QString name){
    RecordFunc reFuncStruct;
    reFuncStruct.type = type;
    reFuncStruct.namei18nString = name;
    if (recordFuncStack.length() < 1)
        recordFuncStack.push(reFuncStruct);
    else if (QString::compare(recordFuncStack.last().namei18nString, name) != 0){
//        qDebug() << recordFuncStack.last().namei18nString << name;
        recordFuncStack.push(reFuncStruct);
    }
}

void FunctionSelect::popRecordValue() {
    if (!recordFuncStack.isEmpty()) {
        recordFuncStack.pop();
    }
}
