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
#ifndef CHANGEPWDINTELDIALOG_H
#define CHANGEPWDINTELDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProcess>

#ifdef ENABLEPQ
extern "C" {

#include <pwquality.h>

}

#define PWCONF "/etc/security/pwquality.conf"
#define RFLAG 0x1
#define CFLAG 0x2

#endif

namespace Ui {
class ChangePwdIntelDialog;
}

class ChangePwdIntelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwdIntelDialog(QWidget *parent = 0);
    ~ChangePwdIntelDialog();

public:
    void initPwdChecked();
    void setupComponent();
    void setupConnect();

    void refreshConfirmBtnStatus();

    void setFace(QString iconfile);
    void setUsername(QString username);
    void setPwdType(QString type);
    void setAccountType(QString text);

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *w,QEvent *e);

private slots:
    void pwdLegalityCheck(QString pwd);
    //标准输出
    void on_readyReadStandardOutput();
    //标准错误输出
    void on_readyReadStandardError();
    //QProcess程序启动输出展示
    void on_started();
    //QProcess程序写入展示
    void on_byteWritten();
private:
    Ui::ChangePwdIntelDialog *ui;
    QString nameTip;
    QString pwdTip;
    QString pwdTip_2;
    QString pwdSureTip;
    QHBoxLayout * oldPwdLineEditHLayout;
    QHBoxLayout * pwdLineEditHLayout;
    QHBoxLayout * pwdsureLineEditHLayout;
    QPushButton * showOldPwdBtn;
    QPushButton * showPwdBtn;
    QPushButton * showSurePwdBtn;

    bool enablePwdQuality;
    bool isOldPwdVisibel;
    bool isNewPwdVisibel;
    bool isSurePwdVisibel;

    QProcess *cmd;
    int process;

#ifdef ENABLEPQ
    pwquality_settings_t *settings;
#endif

Q_SIGNALS:
    void passwd_send(QString pwd, QString username);
};

#endif // CHANGEPWDINTELDIALOG_H