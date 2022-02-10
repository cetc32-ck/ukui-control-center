#include "timezonechooser.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QTimeZone>
#include <QCompleter>
#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QtCore/qmath.h>
#include <QDialog>
#include <QGSettings>
#include <ukcc/widgets/imageutil.h>

const QString kcnBj = "北京";
const QString kenBj = "Asia/Beijing";

extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);


TimeZoneChooser::TimeZoneChooser(QWidget *parent) : QDialog(parent)
{
    m_map = new TimezoneMap(this);
//    m_map->show();
    m_zoneinfo = new ZoneInfo;
    m_searchInput = new QLineEdit(this);
    m_cancelBtn = new QPushButton(tr("Cancel"));
    m_confirmBtn = new QPushButton(tr("Confirm"));

    this->setObjectName("MapFrame");
    this->setWindowTitle(tr("Change Timezone"));
    this->installEventFilter(this);

    //m_searchInput->setStyleSheet("background-color:palette(windowtext)");
    m_searchInput->setFocusPolicy(Qt::ClickFocus);
    m_searchInput->setFixedSize(400,36);
    m_searchInput->installEventFilter(this);
    m_searchInput->setFocusPolicy(Qt::ClickFocus);
    m_searchInput->setContextMenuPolicy(Qt::NoContextMenu);
     QHBoxLayout* SearchLayout = new QHBoxLayout(m_searchInput);

    m_queryWid=new QWidget(m_searchInput);

    QHBoxLayout* queryWidLayout = new QHBoxLayout(m_queryWid);
    queryWidLayout->setContentsMargins(0, 0, 0, 0);
    queryWidLayout->setAlignment(Qt::AlignJustify);
    queryWidLayout->setSpacing(0);

    QIcon searchIcon = QIcon::fromTheme("edit-find-symbolic");
    m_queryIcon = new QLabel(this);
    m_queryIcon->setPixmap(searchIcon.pixmap(searchIcon.actualSize(QSize(16, 16))));
    m_queryIcon->setProperty("useIconHighlightEffect",0x02);

    m_queryText = new QLabel(this);
    m_queryText->setText(tr("Search Timezone"));
    m_queryText->setStyleSheet("background:transparent;color:#626c6e;");
    m_queryText->adjustSize();

//    queryWidLayout->addStretch();
    queryWidLayout->addWidget(m_queryIcon);
    queryWidLayout->addWidget(m_queryText);
//    queryWidLayout->addStretch();

    m_animation= new QPropertyAnimation(m_queryWid, "geometry", this);
    m_animation->setDuration(100);

    SearchLayout->addStretch();
    SearchLayout->addWidget(m_queryWid);
    SearchLayout->addStretch();


    connect(m_animation,&QPropertyAnimation::finished,this,&TimeZoneChooser::animationFinishedSlot);


    initSize();

    QHBoxLayout *btnlayout = new QHBoxLayout;
    btnlayout->addStretch();
    btnlayout->addWidget(m_cancelBtn);
    btnlayout->addSpacing(5);
    btnlayout->addWidget(m_confirmBtn);
    btnlayout->addSpacing(36);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);

    QLabel *mTipLabel = new QLabel(this);
    mTipLabel->setText(tr("To select a time zone, please click where near you on the map and select a city from the nearest city"));
    mTipLabel->setStyleSheet("background:transparent;color:#626c6e;");
    mTipLabel->setAlignment(Qt::AlignHCenter);

    layout->addWidget(m_searchInput, 0, Qt::AlignHCenter);
    layout->addWidget(mTipLabel,Qt::AlignHCenter);
    layout->addSpacing(32);
    layout->addWidget(m_map, 0, Qt::AlignHCenter);
    layout->addSpacing(32);
    layout->addLayout(btnlayout);
    layout->addSpacing(32);

    setLayout(layout);

    connect(m_confirmBtn, &QPushButton::clicked,[this]{
        QString timezone = m_map->getTimezone();
        emit this->confirmed(timezone);
    });

    connect(m_cancelBtn, &QPushButton::clicked, this, [this]{
        hide();
        emit this->cancelled();
    });



    connect(m_map, &TimezoneMap::timezoneSelected, this, [this]{
        if (m_searchInput->hasFocus() || !m_searchInput->text().isEmpty()) {
            m_searchInput->setText("");
            m_searchInput->setFocus();
            m_searchInput->clearFocus();
        }
    });

    connect(m_searchInput, &QLineEdit::editingFinished, [this]{
        QString timezone = m_searchInput->text();
        timezone = m_zoneCompletion.value(timezone,timezone);
        m_map->setTimezone(timezone);
    });

    QTimer::singleShot(0, [this] {
        QStringList completions;
//        completions << kenBj;
//        completions << kcnBj;
//        m_zoneCompletion[kcnBj] = kenBj;
        for (QString timezone : QTimeZone::availableTimeZoneIds()) {
//            if ("Asia/Shanghai" == timezone) {
//                continue;
//            }
            completions << timezone;

            const QString locale = QLocale::system().name();
            QString localizedTimezone = m_zoneinfo->getLocalTimezoneName(timezone, locale);
            completions << localizedTimezone;

            m_zoneCompletion[localizedTimezone] = timezone;
        }

        QCompleter *completer = new QCompleter(completions, m_searchInput);
        completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);

        m_searchInput->setCompleter(completer);

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)
        connect(completer, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),
                [=](const QString &text){
#else
        //鼠标点击后直接页面跳转(https://doc.qt.io/qt-5/qcompleter.html#activated-1)
        connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
                [=](const QString &text) {
#endif
            Q_UNUSED(text);
            QString timezone = m_searchInput->text();
            timezone = m_zoneCompletion.value(timezone,timezone);
            m_map->setTimezone(timezone);
        });

        m_popup = completer->popup();
        m_popup->installEventFilter(this);
    });
}

void TimeZoneChooser::setTitle(QString title) {
    this->setWindowTitle(title);
}

void TimeZoneChooser::setMarkedTimeZoneSlot(QString timezone) {
    m_map->setTimezone(timezone);
}

void TimeZoneChooser::keyRealeaseEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Cancel)) {
        hide();
        emit this->cancelled();
    }
}

bool TimeZoneChooser::eventFilter(QObject* obj, QEvent *event) {
    if (obj == m_popup && event->type() == QEvent::Move) {
        QMoveEvent* move = static_cast<QMoveEvent*>(event);
        QPoint desPos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height() + 2));

        if(move->pos() != desPos) {
            m_popup->move(desPos);
        }
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);
        QWidget *searchParentWid = static_cast<QWidget*>(m_searchInput->parent());
        QPoint  searchPoint      = searchParentWid->mapFromGlobal(mEvent->globalPos());
        if (!m_searchInput->geometry().contains(searchPoint)) {
                if (m_isSearching == true) {
                    m_searchInput->setFocus();
                    m_searchInput->clearFocus();
                }
        }
    }

    if (obj == m_searchInput) {
        if (event->type() == QEvent::FocusIn) {
            if (m_searchInput->text().isEmpty()) {

                m_queryWid->layout()->removeWidget(m_queryText);
                m_queryText->setParent(nullptr);
                m_animation->stop();
                m_animation->setStartValue(QRect(m_queryWid->x(),0,
                                               m_queryIcon->width()+5,(m_searchInput->height()+36)/2));

                m_animation->setEndValue(QRect(0, 0, m_queryIcon->width() + 5,(m_searchInput->height()+36)/2));
                m_animation->setEasingCurve(QEasingCurve::OutQuad);
                m_animation->start();
                m_searchInput->setTextMargins(20, 1, 0, 1);
            }
            m_isSearching = true;
        } else if (event->type() == QEvent::FocusOut) {
            m_searchKeyWords.clear();
            if (m_searchInput->text().isEmpty()) {
                if (m_isSearching) {
                    m_queryText->adjustSize();
                    m_animation->setStartValue(QRect(0, 0,
                                                     m_queryIcon->width()+5,(m_searchInput->height()+36)/2));
                    m_animation->setEndValue(QRect((m_searchInput->width() - (m_queryIcon->width()+m_queryText->width()+10))/2,0,
                                                   m_queryIcon->width()+m_queryText->width()+30,(m_searchInput->height()+36)/2));
                    m_animation->setEasingCurve(QEasingCurve::InQuad);
                    m_animation->start();
                }
            }
            m_isSearching=false;
        }
    }
    return QObject::eventFilter(obj, event);
}

//获取适合屏幕的地图大小
QSize TimeZoneChooser::getFitSize(){
    // const QDesktopWidget *desktop = QApplication::desktop();
    // const QRect primaryRect = desktop->availableGeometry(desktop->primaryScreen());

    // double width = primaryRect.width() - 360/* dcc */ - 20 * 2;
    // double height = primaryRect.height() - 70/* dock */ - 20 * 2;

    return QSize(960, 602);
}


void TimeZoneChooser::initSize(){

    double MapPixWidth = 900.0;
    double MapPixHeight = 500.0;
    double MapPictureWidth = 978.0;
    double MapPictureHeight = 500.0;

    const QSize fitSize = getFitSize();
   setFixedSize(fitSize.width(), fitSize.height());

    const float mapWidth = qMin(MapPixWidth, fitSize.width() - 20 * 2.0);
    //搜索时区36,取消 确定按钮36, mTipLabel 36
    const float mapHeight = qMin(MapPixHeight, fitSize.height() - 36.0 * 3 - 32.0 * 3);
    const double widthScale = MapPictureWidth / mapWidth;
    const double heightScale = MapPictureHeight / mapHeight;
    const double scale = qMax(widthScale, heightScale);

    m_map->setFixedSize(MapPictureWidth / scale, MapPictureHeight / scale);

    m_cancelBtn->setFixedHeight(36);
    m_confirmBtn->setFixedHeight(36);
    m_cancelBtn->setFixedWidth(120);
    m_confirmBtn->setFixedWidth(120);
}


void TimeZoneChooser::animationFinishedSlot()
{
    if (m_isSearching) {
        m_queryWid->layout()->removeWidget(m_queryText);
        m_queryText->setParent(nullptr);
        m_searchInput->setTextMargins(20, 1, 0, 1);
        if(!m_searchKeyWords.isEmpty()) {
            m_searchInput->setText(m_searchKeyWords);
            m_searchKeyWords.clear();
        }
    } else {
        m_queryWid->layout()->addWidget(m_queryText);
    }
}

void TimeZoneChooser::hide()
{
    m_searchInput->setText("");
    m_searchInput->setFocus();
    m_searchInput->clearFocus();
    QDialog::hide();
    return;
}

void TimeZoneChooser::closeEvent(QCloseEvent *e)
{
    hide();
    emit cancelled();
}
