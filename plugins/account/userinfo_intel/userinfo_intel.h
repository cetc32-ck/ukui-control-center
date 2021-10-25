﻿/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
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
#ifndef USERINFO_INTEL_H
#define USERINFO_INTEL_H

#include <QObject>
#include <QtPlugin>

//#include <QToolButton>
//#include <QMenu>
//#include <QAction>
#include <QSignalMapper>
#include <QMouseEvent>
#include <QSettings>
#include <QProcess>

#include "shell/interface.h"

#include "qtdbus/systemdbusdispatcher.h"
#include "qtdbus/userdispatcher.h"

#include "changegroupinteldialog.h"
#include "changepwdinteldialog.h"
#include "changephoneinteldialog.h"

#include "changepininteldialog.h"

#include "changefaceinteldialog.h"
#include "changetypeinteldialog.h"
#include "changevalidinteldialog.h"
#include "deluserinteldialog.h"
#include "createuserinteldialog.h"
#include "HoverWidget/hoverwidget.h"
#include "picturetowhite.h"
#include "messageboxpowerintel.h"

#ifdef ENABLEPQ
extern "C" {
#include <pwquality.h>
}
#endif

enum {
    STANDARDUSER,
    ADMINISTRATOR,
    ROOT
};

typedef struct _UserInfomation {
    QString objpath;
    QString username;
    QString realname;
    QString iconfile;
    QString passwd;
    int accounttype;
    int passwdtype;
    bool current;
    bool logined;
    bool autologin;
    bool noPwdLogin;
    qint64 uid;
}UserInfomation;

typedef struct _PwdQualityOption {

    int diff_ok;
    int min_length;
    int dig_credit;
    int up_credit;
    int low_credit;
    int oth_credit;
    int min_class;
    int max_repeat;
    int max_class_repeat;
    int max_sequence;
    int gecos_check;
    int dict_check;
    int user_check;
    int enforcing;
    int retry_times;
    int enforce_for_root;
    int local_users_only;
    int palindrome;
    int no_similar_check;
    char *bad_words;
    char *dict_path;

}PwdQualityOption;

namespace Ui {
class UserInfoIntel;
}

class QDBusInterface;
class SwitchButton;

class UserInfoIntel : public QObject, CommonInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ukcc.CommonInterface")
    Q_INTERFACES(CommonInterface)

public:
    UserInfoIntel();
    ~UserInfoIntel();

    QString plugini18nName() Q_DECL_OVERRIDE;
    int pluginTypes() Q_DECL_OVERRIDE;
    QWidget *pluginUi() Q_DECL_OVERRIDE;
    const QString name() const  Q_DECL_OVERRIDE;
    bool isShowOnHomePage() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    bool isEnable() const Q_DECL_OVERRIDE;

public:
    void initSearchText();
    void initComponent();
    void initAllUserStatus();

    void _acquireAllUsersInfo();
    UserInfomation _acquireUserInfo(QString objpath);
    QString _accountTypeIntToString(int type);
    void _buildWidgetForItem(UserInfomation user);
    void _resetListWidgetHeigh();

    void _refreshUserInfoUI();

    void showCreateUserDialog();
    void createUser(QString username, QString pwd, QString pin, int atype);
    void createUserDone(QString objpath);

    void showDeleteUserDialog(QString username);
    void deleteUser(bool removefile, QString username);
    void deleteUserDone(QString objpath);

    void showChangePwdDialog(QString username);
    void showChangePhoDialog(QString username);
    void changeUserPwd(QString pwd, QString username);

    void showChangeTypeDialog(QString username);
    void changeUserType(int atype, QString username);

    void showChangeFaceDialog(QString username);
    void changeUserFace(QString facefile, QString username);

    void showChangeValidDialog(QString username);

    void showChangeGroupDialog();

    void get_all_users();
    UserInfomation init_user_info(QString objpath);
    void setup_otherusers_ui();
    void build_item_with_widget(UserInfomation user);
    void ui_component_init();

    QString accounttype_enum_to_string(int id);
    QString login_status_bool_to_string(bool status);


    void readCurrentPwdConf();
    QStringList getUsersList();

    QPixmap PixmapToRound(const QString &src, int radius);

    void setChangeFaceShadow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::UserInfoIntel *ui;
    bool mFirstLoad;
    QString pluginName;
    QString oldName;
    int pluginType;
    QWidget * pluginWidget;
    HoverWidget *addWgt;

    UserInfomation current_user;
    bool isDelCurrentUser = false;

private:
    SwitchButton * nopwdSwitchBtn;
    SwitchButton * autoLoginSwitchBtn;

    SystemDbusDispatcher * sysdispatcher;
    QSettings * autoSettings = nullptr;
    QGSettings  *m_pgsettings;
    bool enter = false;

    PictureToWhite *m_pPictureToWhite;


private:
    bool getAutomaticLogin(QString username);

private:
    QMap<QString, UserInfomation> allUserInfoMap;
    QMap<QString, QListWidgetItem *> otherUserItemMap;

    int adminnum;

    QString _newUserPwd;

//    QMap<QString, QToolButton *> otherbtnMap;
    QMap<QString, QListWidgetItem *> otherItemMap;

    QSignalMapper * pwdSignalMapper;
    QSignalMapper * faceSignalMapper;
    QSignalMapper * typeSignalMapper;
    QSignalMapper * delSignalMapper;

    QSize faceSize;
    QSize itemSize;
    QSize btnSize;


    QString pwdcreate;
    QString my_name;

    QDBusInterface * sysinterface;

    bool enablePwdQuality;

#ifdef ENABLEPQ
    pwquality_settings_t * pwdconf;
#endif

    PwdQualityOption pwdOption;

    QString pwdMsg;

private slots:
//    void show_change_pwd_dialog_slot(QString username);
//    void change_pwd_slot(QString pwd, QString username);
//    void change_pwd_done_slot();

//    void show_change_face_dialog_slot(QString username);
//    void change_face_slot(QString facefile, QString username);
//    void change_face_done_slot();

//    void show_change_accounttype_dialog_slot(QString username);
//    void change_accounttype_slot(int atype, QString username, bool status);
//    void change_accounttype_done_slot();

//    void show_del_user_dialog_slot(QString username);
    void delete_user_slot(bool removefile, QString username);
//    void delete_user_done_slot(QString objpath);

//    void show_create_user_dialog_slot();
//    void create_user_slot(QString username, QString pwd, QString pin, int atype, bool autologin);
//    void create_user_done_slot(QString objpath);
};

#endif // USERINFO_INTEL_H
