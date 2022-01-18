#include "inteldeviceinfoitem.h"

#define STYLENAME "org.ukui.style"

IntelDeviceInfoItem::IntelDeviceInfoItem(QWidget *parent, bluetoothdevice * dev):
    QFrame(parent),
    _MDev(dev)
{
    this->setMinimumSize(580,64);
    this->setMaximumSize(1800,64);
    //this->setObjectName(_MDev? _MDev.data()->address(): "null");
    this->setObjectName(_MDev? _MDev->getDevAddress() : "null");

    m_str_dev_connecting = tr("Connecting");
    m_str_dev_disconnecting = tr("Disconnecting");
    m_str_dev_connected = tr("Connected");
    m_str_dev_ununited = tr("Not Connected");
    m_str_dev_conn_fail = tr("Connect fail");
    m_str_dev_disconn_fail = tr("Disconnect fail");

    InitMemberVariables();
    setDeviceConnectSignals();
}

IntelDeviceInfoItem::~IntelDeviceInfoItem()
{

}

void IntelDeviceInfoItem::TimedRestoreConnectionErrorDisplay()
{
    qDebug() << Q_FUNC_INFO << __LINE__;
    //错误信息显示超时后，显示错误操作后的设备状态
    QTimer::singleShot(8000,this,[=]{
        if (_MDev->isPaired())
        {
            _DevStatus = DEVSTATUS::Paired;

            if (_MDev->isConnected())
                _DevStatus = DEVSTATUS::Connected;

        } else {
            _DevStatus = DEVSTATUS::NoPaired;
        }
        update();
    });
}


void IntelDeviceInfoItem::InitMemberVariables()
{
    if (_MDev->isPaired()) {

        _DevStatus = DEVSTATUS::Paired;

        if (_MDev->isConnected()) {
            _DevStatus = DEVSTATUS::Connected;
        }
    } else {
        _DevStatus = DEVSTATUS::NoPaired;
    }

    if(QGSettings::isSchemaInstalled(STYLENAME)){
        item_gsettings = new QGSettings(STYLENAME);

        connect(item_gsettings,&QGSettings::changed,this,&IntelDeviceInfoItem::GSettingsChanges);

        if(item_gsettings->get("style-name").toString() == "ukui-black" ||
           item_gsettings->get("style-name").toString() == "ukui-dark")
            _themeIsBlack = true;
        else
            _themeIsBlack = false;

        _fontFamily = item_gsettings->get("style-name").toString();
        _fontSize = item_gsettings->get("system-font-size").toString().toInt();
    }

    devName  = _MDev->getDevName();
    _MStatus = Status::Nomal;
    _clicked = false;
    _pressFlag = false;
    _connDevTimeOutFlag = false;

    _iconTimer = new QTimer(this);
    _iconTimer->setInterval(110);
    connect(_iconTimer,&QTimer::timeout,this,[=]{
//        if (iconFlag == 0)
//            iconFlag = 7;
//        iconFlag--;
        update();
    });

    _devConnTimer = new QTimer(this);
    _devConnTimer->setInterval(30000);
    connect(_devConnTimer,&QTimer::timeout,this,[=]{
        if(BlueToothMain::m_device_pin_flag)
        {
            _devConnTimer->stop();
            _devConnTimer->start();
            return;
        }

        emit devConnectionComplete();
        _devConnTimer->stop();
        _iconTimer->stop();
        _clicked = false;
        _connDevTimeOutFlag = true ;

        qDebug() << Q_FUNC_INFO << "current dev status:" << _DevStatus ;
        if (DEVSTATUS::Connecting == _DevStatus) {
            _DevStatus = DEVSTATUS::ConnectFailed;
        } else if (DEVSTATUS::DisConnecting == _DevStatus) {
            _DevStatus = DEVSTATUS::DisConnectFailed;
        }
        update();
        TimedRestoreConnectionErrorDisplay();
    });

    dev_Menu = new QMenu(this);
    connect(dev_Menu,&QMenu::triggered,this,&IntelDeviceInfoItem::MenuSignalDeviceFunction);

}

void IntelDeviceInfoItem::MenuSignalDeviceFunction(QAction *action)
{
    if(NULL == _MDev)
        return;
    qDebug() << Q_FUNC_INFO << action->text() ;
    if(action->text() == tr("send file"))
    {
        qDebug() << Q_FUNC_INFO << "To :" << _MDev->getDevName() << "send file" << __LINE__;
        emit devSendFiles(_MDev->getDevAddress());
        //Send_files_by_address(action->statusTip());
    }
    else if(action->text() == tr("remove"))
    {
        DevRemoveDialog *mesgBox = new DevRemoveDialog();
        mesgBox->setModal(true);
        mesgBox->setDialogText(_MDev->getDevName());

        connect(mesgBox,&DevRemoveDialog::accepted,this,[=]{
           qDebug() << Q_FUNC_INFO << "To :" << _MDev->getDevName()  << "Remove" << __LINE__;
           emit devRemove(_MDev->getDevAddress());
        });

        mesgBox->exec();
    }
}


void IntelDeviceInfoItem::setDeviceCurrentStatus()
{
    if (NULL != _MDev)
    {
        if (_connDevTimeOutFlag)
        {
            _DevStatus = DEVSTATUS::Error;
            QTimer::singleShot(8000,this,[=]{
                _connDevTimeOutFlag = false ;
                update();
            });
        }
        else
        {
            if(_MDev->isPaired())
            {
                _DevStatus = DEVSTATUS::Paired;
                if (_MDev->isConnected())
                    _DevStatus = DEVSTATUS::Connected;
            }
            else
            {
                _DevStatus = DEVSTATUS::None;
            }
        }
    }
}


void IntelDeviceInfoItem::GSettingsChanges(const QString &key)
{
    qDebug() << Q_FUNC_INFO << key;
    if("styleName" == key){
        if(item_gsettings->get("style-name").toString() == "ukui-black" ||
           item_gsettings->get("style-name").toString() == "ukui-dark")
            _themeIsBlack = true;
        else
            _themeIsBlack = false;
    } else if ("systemFont" == key) {
        _fontFamily = item_gsettings->get("style-name").toString();
    } else if ("systemFontSize" == key) {
        _fontSize = item_gsettings->get("system-font-size").toString().toInt();
    }
}

void IntelDeviceInfoItem::setDeviceConnectSignals()
{
    if (_MDev)
    {
        connect(_MDev,&bluetoothdevice::nameChanged,this,[=](const QString &name)
        {
            devName = name ;
            update();
        });

        connect(_MDev,&bluetoothdevice::typeChanged,this,[=](bluetoothdevice::DEVICE_TYPE type)
        {
            update();
        });

        connect(_MDev,&bluetoothdevice::pairedChanged,this,[=](bool paired)
        {
            qDebug() << Q_FUNC_INFO << "pairedChanged" << __LINE__;
            BlueToothMain::m_device_pin_flag = false;
            if(_devConnTimer->isActive())
                _devConnTimer->stop();
            if (_iconTimer->isActive())
                _iconTimer->stop();
            if (paired)
            {
                qDebug() << Q_FUNC_INFO << "pairedChanged" << __LINE__;
                emit devPaired(_MDev->getDevAddress());
            }
            _clicked = false;

            if (paired)
            {
                _DevStatus = DEVSTATUS::Paired;
                emit devPaired(_MDev->getDevAddress());

                if (_MDev->isConnected())
                    _DevStatus = DEVSTATUS::Connected;

            } else {
                _DevStatus = DEVSTATUS::NoPaired;
            }

            update();
        });

        connect(_MDev,&bluetoothdevice::connectedChanged,this,[=](bool connected)
        {
            BlueToothMain::m_device_pin_flag = false;
            if(_devConnTimer->isActive())
                _devConnTimer->stop();
            if (_iconTimer->isActive())
                _iconTimer->stop();
            _clicked = false;

            if (_MDev->isPaired() && connected) {
                _DevStatus = DEVSTATUS::Connected;
            } else if (!_MDev->isPaired() && connected) {
                _DevStatus = DEVSTATUS::Connecting;
            } else if (_MDev->isPaired() && !connected){
                _DevStatus = DEVSTATUS::Paired;
            } else {
                _DevStatus = DEVSTATUS::ConnectFailed;
            }

            emit devConnectionComplete();
            update();
            TimedRestoreConnectionErrorDisplay();
        });

        connect(_MDev,&bluetoothdevice::errorInfoRefresh,this,[=](int errorId , QString errorText)
        {
            qDebug () << Q_FUNC_INFO << "error:" << errorId << errorText << __LINE__;
            if (errorId)
            {
                if(_devConnTimer->isActive())
                    _devConnTimer->stop();
                if (_iconTimer->isActive())
                    _iconTimer->stop();
                _clicked = false;

                qDebug () << Q_FUNC_INFO << "error:" << errorId << errorText << __LINE__;
                //BlueToothMain::m_device_pin_flag = false;
                _DevStatus = DEVSTATUS::ConnectFailed;
                emit devConnectionComplete();
                update();
                TimedRestoreConnectionErrorDisplay();
            }
        });
    }
}

bool IntelDeviceInfoItem::mouseEventIntargetAera(QPoint p)
{
    QRect *targte = new QRect(this->width()-55,14,36,36);
    if (targte->contains(p)) {
        return true;
    } else {
        return false;
    }
}

QRect IntelDeviceInfoItem::getStatusTextRect(QRect rect)
{
    if (_MDev && _MDev->isPaired()) {
        return QRect(this->width()-226,16,150,30);
    } else {
        return QRect(this->width()-120,16,105,30);
    }
}

QRect IntelDeviceInfoItem::getLoadIconRect()
{
    if (_MDev &&_MDev->isPaired()) {
        return QRect(this->width()-98,19,20,20);
    } else {
        return QRect(this->width()-35,19,20,20);
    }
}

QRect IntelDeviceInfoItem::getFontPixelQPoint(QString str)
{
    QFont font;
    font.setFamily(_fontFamily);
    font.setPointSize(_fontSize);
    QFontMetrics fm(font);

    return fm.boundingRect(str);
}

void IntelDeviceInfoItem::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (!BlueToothMain::m_device_operating)
        _MStatus = Status::Hover;
    else
        _MStatus = Status::Nomal;
    update();
}

void IntelDeviceInfoItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    _MStatus = Status::Nomal;
    update();
}

void IntelDeviceInfoItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (mouseEventIntargetAera(event->pos())) {
            _pressBtnFlag = true;
        } else {
            if (!BlueToothMain::m_device_operating)
            {
                _pressFlag = true;
                _MStatus = Status::Check;
            }
        }
    }
    update();
}

void IntelDeviceInfoItem::MouseClickedDevFunc()
{
    qDebug () << Q_FUNC_INFO << __LINE__;
    if (!_MDev->isPaired())
        return;

    dev_Menu->clear();
    QPoint currentWPoint = this->pos();
    QPoint sreenPoint = QWidget::mapFromGlobal(currentWPoint);
    //qDebug () <<Q_FUNC_INFO << "allPoint--x--y:" << sreenPoint.x() << sreenPoint.y() <<__LINE__;

    QPalette palette;
    if(_themeIsBlack)
    {
        palette.setBrush(QPalette::Base,QColor(Qt::black));
        palette.setBrush(QPalette::Active, QPalette::Text,QColor(Qt::white));
    }
    else
    {
        palette.setBrush(QPalette::Base,QColor(Qt::white));
        palette.setBrush(QPalette::Active, QPalette::Text,QColor(Qt::black));
    }

    dev_Menu->setProperty("setIconHighlightEffectDefaultColor", dev_Menu->palette().color(QPalette::Active, QPalette::Base));
    dev_Menu->setPalette(palette);
    dev_Menu->setMinimumWidth(180);

//    if (-1 != _MDev.data()->uuids().indexOf(BluezQt::Services::ObexObjectPush)) {
//        QAction *sendfile = new QAction(dev_Menu);
//        sendfile->setText(tr("Send files"));
//        dev_Menu->addAction(sendfile);
//        dev_Menu->addSeparator();
//    }
    if (bluetoothdevice::DEVICE_TYPE::phone == _MDev->getDevType() ||
        bluetoothdevice::DEVICE_TYPE::computer == _MDev->getDevType())
    {
        QAction *sendfile = new QAction(dev_Menu);
        sendfile->setText(tr("send file"));
        dev_Menu->addAction(sendfile);
        dev_Menu->addSeparator();
    }

    QAction *remove = new QAction(dev_Menu);
    remove->setText(tr("remove"));
    dev_Menu->addAction(remove);

    //qDebug () << this->x() << this->y() << "======x ======y";
    dev_Menu->move(qAbs(sreenPoint.x())+this->width()-200,qAbs(sreenPoint.y())+this->y()+40);
    dev_Menu->exec();

}

void IntelDeviceInfoItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << Q_FUNC_INFO << mouseEventIntargetAera(event->pos());
        if (mouseEventIntargetAera(event->pos()) && _pressBtnFlag) {
            MouseClickedDevFunc();
            _pressBtnFlag = false;
        } else {
            if (!BlueToothMain::m_device_operating)
            {
                BlueToothMain::m_device_operating = true;
                MouseClickedFunc();
                _pressFlag = false;
                _MStatus = Status::Hover;
            }
        }
    }
    update();

}

void IntelDeviceInfoItem::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseEventIntargetAera(event->pos())) {
        _inBtn = true;
        this->update();
    } else {
        if (_inBtn) {
            _inBtn = false;
            this->update();
        }
    }

}

void IntelDeviceInfoItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(Qt::transparent));

    DrawBackground(painter);
    DrawStatusIcon(painter);
    DrawText(painter);

    if (_iconTimer->isActive())
        DrawLoadingIcon(painter);
    else
        DrawStatusText(painter);

    if (_MDev->isPaired())
    {
        this->setMouseTracking(true);
        DrawFuncBtn(painter);
    }
}

/************************************************
 * @brief  getPainterBrushColor
 * @param  null
 * @return QColor
*************************************************/
QColor IntelDeviceInfoItem::getPainterBrushColor()
{
//    QColor color;
//    switch (_MStatus) {
//    case Status::Nomal:
//        color = QColor(Qt::white);//("#EBEBEB");
//        break;
//    case Status::Hover:
//        color = QColor("#D7D7D7");
//        break;
//    default:
//        color = QColor(Qt::white);//("#EBEBEB");
//        break;
//    }
//    return color;

    QColor color;
    switch (_MStatus) {
    case Status::Nomal:

        if(_themeIsBlack)
            //color = QColor("#1F2022");//("#EBEBEB");
            color = QColor(Qt::black);//("#EBEBEB");
        else
            color = QColor(Qt::white);//("#EBEBEB");

        //color = QColor(Qt::black);//("#EBEBEB");

        break;
    case Status::Hover:
        if(_themeIsBlack)
            color = _inBtn?QColor(Qt::black):QColor("#37373B");
        else
            color = _inBtn?QColor(Qt::white):QColor("#D1D1D1");
        break;
    case Status::Check:
        if(_themeIsBlack)
            color = QColor("#4B4B4F");
        else
            color = QColor("#D9D9D9");
        break;
    default:
        if(_themeIsBlack)
            color = QColor("#1F2022");//("#EBEBEB");
        else
            color = QColor(Qt::white);//("#EBEBEB");
        //color = QColor(Qt::white);//("#EBEBEB");
        break;
    }
    if (_MStatus == Status::Hover || _MStatus == Status::Check || _themeIsBlack)
        color.setAlpha(50);
    return color;
}

/************************************************
 * @brief  getDevStatusColor
 * @param  null
 * @return QColor
*************************************************/
QColor IntelDeviceInfoItem::getDevStatusColor()
{
    QColor color;

    if (_MDev) {
        if (_MDev->isConnected()) {
            color = QColor("#2FB3E8");
        } else {
            color = QColor("#F4F4F4");
        }
    } else {
        color = QColor("#F4F4F4");
    }
    return color;
}

/************************************************
 * @brief  getDevTypeIcon
 * @param  null
 * @return QPixmap
*************************************************/
QPixmap IntelDeviceInfoItem::getDevTypeIcon()
{
    QPixmap icon;
    QString iconName;
    if (_MDev) {
        switch (_MDev->getDevType()) {
        case bluetoothdevice::DEVICE_TYPE::phone:
            iconName = "phone-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::computer:
            iconName = "computer-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::headset:
            iconName = "audio-headset-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::headphones:
            iconName = "audio-headphones-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::audioVideo:
            iconName = "audio-speakers-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::keyboard:
            iconName = "input-keyboard-symbolic";
            break;
        case bluetoothdevice::DEVICE_TYPE::mouse:
            iconName = "input-mouse-symbolic";
            break;
        default:
            iconName = "bluetooth-active-symbolic";
            break;
        }
    } else {
        iconName = "bluetooth-active-symbolic";
    }

    //if (_themeIsBlack && (Status::Nomal == _MStatus)) {
    if (_themeIsBlack) {
        icon = ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme(iconName).pixmap(16,16),"white");
    } else {
        icon = QIcon::fromTheme(iconName).pixmap(16,16);
    }

    return icon;
}

/************************************************
 * @brief  DrawBackground
 * @param  painter
 * @return null
*************************************************/
void IntelDeviceInfoItem::DrawBackground(QPainter &painter)
{
    painter.save();
    painter.setBrush(getPainterBrushColor());
    painter.drawRoundedRect(this->rect(),6,6,Qt::AbsoluteSize);
    painter.restore();
}

/************************************************
 * @brief  DrawStatusIcon
 * @param  painter
 * @return null
*************************************************/
void IntelDeviceInfoItem::DrawStatusIcon(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    style()->drawItemPixmap(&painter, QRect(20, 14, 35, 35), Qt::AlignCenter, getDevTypeIcon());
    painter.restore();
}

int IntelDeviceInfoItem::NameTextCoordinate_Y_offset()
{
    int show_coordinate_text_Y_offset = 0 ;
    switch (_fontSize)
    {
    case 15:
    case 14:
    case 13:
    case 12:
        show_coordinate_text_Y_offset = 0;
    case 11:
    case 10:
    default:
        show_coordinate_text_Y_offset = 0;
        break;
    }
    return show_coordinate_text_Y_offset;
}

QString IntelDeviceInfoItem::getDeviceName(QString devName)
{
    QString showRealName;
    int nCount = devName.count();
    //qDebug() << Q_FUNC_INFO << "DevName Count:" << nCount << __LINE__;

    int zhCount = 0;
    int charMCount = 0;
    for(int i = 0; i < nCount; i++)
    {
        QChar ch = devName.at(i);
        ushort uNum = ch.unicode();
        if(uNum >= 0x4E00 && uNum <= 0x9FA5)
        {
            // 这个字符是中文
            zhCount ++;
            //qDebug() << Q_FUNC_INFO << "this is zh:" << uNum << zhCount << __LINE__;
        }

        if (uNum == 0x004D || uNum == 0x006D)
        {
            // 这个字符是m/M
            charMCount++;
            //qDebug() << Q_FUNC_INFO << "this is char m/M:" << uNum << charMCount << __LINE__;
        }
    }
    if(devName.length() > (ShowNameTextNumberMax() - zhCount - charMCount/2)) /*减去中文长度 或者 m字符显示长度*/
    {
        showRealName = devName.left(ShowNameTextNumberMax()- zhCount - charMCount/2 - 3);
        showRealName.append("...");
        toptipflag = true;
        this->setToolTip(_MDev->getDevName());
    }
    else
    {
        showRealName = devName;
        toptipflag = false;
        this->setToolTip("");
    }
    //qDebug() << Q_FUNC_INFO << "showRealName:" << showRealName << __LINE__;

    return showRealName;
}

int IntelDeviceInfoItem::ShowNameTextNumberMax()
{
    float display_coefficient = (this->width() - 300)/500.00;
    //qDebug() << Q_FUNC_INFO << "display coefficient:" << display_coefficient << __LINE__;
    int max_text_number = 0;
    switch (_fontSize)
    {
    case 10:
        max_text_number += 6 ;
    case 11:
        max_text_number += 5 ;
    case 12:
        max_text_number += 4 ;
    case 13:
        max_text_number += 3 ;
    case 14:
        max_text_number += 2 ;
    case 15:
    default:
        max_text_number += 45 ;
        break;
    }

    int showMaxNameLength = max_text_number*display_coefficient;
    //qDebug() << Q_FUNC_INFO << "The max length :" <<  showMaxNameLength << __LINE__;

    return (45 > showMaxNameLength?45: showMaxNameLength);

}

/************************************************
 * @brief  DrawText
 * @param  painter
 * @return null
*************************************************/
void IntelDeviceInfoItem::DrawText(QPainter &painter)
{
    painter.save();
    if(_themeIsBlack)
        painter.setPen(QColor(Qt::white));
    else
        painter.setPen(QColor(Qt::black));

    //qDebug()<< Q_FUNC_INFO << "=================================" << this->width() << __LINE__;
    //qDebug()<< Q_FUNC_INFO << "=================================" << _fontSize << __LINE__;
    //qDebug()<< Q_FUNC_INFO << "=================================" << _MDev->getDevName().length() << __LINE__;

    //painter.drawText(70,20,this->width()-300,30,Qt::AlignLeft|Qt::AlignVCenter,_MDev? _MDev->getDevName(): QString("Example"));
    painter.drawText(70,16,this->width()-300,30,Qt::AlignLeft|Qt::AlignVCenter,_MDev?getDeviceName( _MDev->getDevName() ): QString("Example"));
    painter.restore();
}

void IntelDeviceInfoItem::DrawStatusText(QPainter &painter)
{
    //setDeviceCurrentStatus();
    painter.save();
    if(_themeIsBlack)
        painter.setPen(QColor(Qt::white));
    else
        painter.setPen(QColor(Qt::black));

//    if(Status::Hover == _MStatus)
//        painter.setPen(QColor(Qt::black));
//    else
//        painter.setPen(QColor(Qt::white));

    QString str;

    switch (_DevStatus) {
    case DEVSTATUS::Paired:
        str = m_str_dev_ununited;
        painter.setPen(QColor("#818181"));
        break;
    case DEVSTATUS::Connected:
        str = m_str_dev_connected;
        break;
    case DEVSTATUS::Connecting:
        str = m_str_dev_connecting;
        break;
    case DEVSTATUS::DisConnecting:
        str = m_str_dev_disconnecting;
        break;
    case DEVSTATUS::DisConnectFailed:
        str = m_str_dev_disconn_fail;
        break;
    case DEVSTATUS::ConnectFailed:
        str = m_str_dev_conn_fail;
        break;
    default:
        break;
    }

    QRect rect = getFontPixelQPoint(str);

    painter.drawText(getStatusTextRect(rect),Qt::AlignRight,str);

    painter.restore();
}

void IntelDeviceInfoItem::DrawFuncBtn(QPainter &painter)
{
    painter.save();

    if (_inBtn) {
        painter.setPen(QColor("#D1CFCF"));
        painter.setBrush(QColor("#D1CFCF"));
    }

    painter.drawRoundRect(this->width()-55,10,36,36,30,30);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);


    if (_themeIsBlack) {
        if (_inBtn)
            style()->drawItemPixmap(&painter,QRect(this->width()-48,19,20,20), Qt::AlignCenter,
                                    ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme("image-loading-symbolic").pixmap(20),"black"));
        else
            style()->drawItemPixmap(&painter, QRect(this->width()-48,19,20,20), Qt::AlignCenter,
                                    ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme("image-loading-symbolic").pixmap(20),"white"));
    } else
        style()->drawItemPixmap(&painter, QRect(this->width()-48,19,20,20), Qt::AlignCenter,
                                ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme("image-loading-symbolic").pixmap(20),"black"));

    painter.restore();
}

void IntelDeviceInfoItem::DrawLoadingIcon(QPainter &painter)
{
    painter.save();
    if (iconFlag >= 7)
        iconFlag = 0;
    if (_themeIsBlack)
        style()->drawItemPixmap(&painter,
                                getLoadIconRect(),
                                Qt::AlignCenter,
                                ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(iconFlag)).pixmap(20),"white"));
    else
        style()->drawItemPixmap(&painter,
                                getLoadIconRect(),
                                Qt::AlignCenter,
                                ImageUtil::drawSymbolicColoredPixmap(QIcon::fromTheme("ukui-loading-"+QString::number(iconFlag)).pixmap(20),"default"));

    painter.restore();
    iconFlag++;

}

/************************************************
 * @brief  MouseClickedFunc
 * @param  null
 * @return null
*************************************************/
void IntelDeviceInfoItem::MouseClickedFunc()
{
    qDebug() << Q_FUNC_INFO ;
    _clicked = true;
    _pressFlag = false;

    if (_MDev) {
        if (_MDev->isConnected()) {
            //_MDev->disconnectFromDevice();
            emit devDisconnect(_MDev->getDevAddress());
            _DevStatus = DEVSTATUS::DisConnecting;
        } else {
            //DevConnectFunc();
            emit devConnect(_MDev->getDevAddress());
            _DevStatus = DEVSTATUS::Connecting;
        }
    }

    //_iconTimer loading
    if (!_iconTimer->isActive())
    {
        _iconTimer->start();
        _devConnTimer->start();
    }
}

/************************************************
 * @brief  该函数意义是当设备为音频设备时，断开已经连接的音频设备
 * @param  null
 * @return null
*************************************************/
void IntelDeviceInfoItem::DevConnectFunc()
{
    if (_MDev)
    {
//        if (_MDev->getDevType() == devAudioVideo ||
//            _MDev->getDevType() == BluezQt::Device::Headphones ||
//            _MDev->getDevType() == BluezQt::Device::Headset    ) {

//            BluezQt::AdapterPtr MDevAdapter = _MDev.data()->adapter();

//            for (auto dev : MDevAdapter.data()->devices()) {

//                if (dev.data()->isConnected() && dev.data()->isPaired()) {
//                    if (dev.data()->type() == BluezQt::Device::AudioVideo ||
//                        dev.data()->type() == BluezQt::Device::Headphones ||
//                        dev.data()->type() == BluezQt::Device::Headset    ) {

//                        BluezQt::PendingCall *pending = dev.data()->disconnectFromDevice();
//                        connect(pending,&BluezQt::PendingCall::finished,this,[&](BluezQt::PendingCall *call){
//                            if (!call->error()) {
//                                _MDev.data()->connectToDevice();
//                            }
//                        });
//                        //如果有正在连接的音频设备，做完以上流程后，直接退出函数体
//                        //因为流程解决定了只有一个音频设备连接，而不需要判断后面是否有音频设备连接；提高代码执行效率
//                        return;
//                    }
//                }
//            }
//            //当没有任何音频设备连接，最后再连接目标设备
//            _MDev.data()->connectToDevice();
//        } else {
//            _MDev.data()->connectToDevice();
//        }
    }
}
