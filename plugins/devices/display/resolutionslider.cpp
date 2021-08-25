#include "resolutionslider.h"
#include "utils.h"

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QComboBox>
#include <QFile>
#include <QStyledItemDelegate>

//#include <KLocalizedString>

#include <KF5/KScreen/kscreen/output.h>

static bool sizeLessThan(const QSize &sizeA, const QSize &sizeB)
{

    return sizeA.width()  <  sizeB.width() ;
}

ResolutionSlider::ResolutionSlider(const KScreen::OutputPtr &output, QWidget *parent)
    : QWidget(parent)
    , mOutput(output)
{
    connect(output.data(), &KScreen::Output::currentModeIdChanged,
            this, &ResolutionSlider::slotOutputModeChanged);
#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 0)

#else
    connect(output.data(), &KScreen::Output::modesChanged,
            this, &ResolutionSlider::init);
#endif

    init();
}

ResolutionSlider::~ResolutionSlider()
{
}

void ResolutionSlider::init()
{
    this->setMinimumSize(402,48);
    this->setMaximumSize(1677215, 48);
    mModes.clear();
    Q_FOREACH (const KScreen::ModePtr &mode, mOutput->modes()) {
        if (mModes.contains(mode->size())) {
            continue;
        }
        mModes << mode->size();
    }
    std::sort(mModes.begin(), mModes.end(), sizeLessThan);

    delete layout();
    delete mSmallestLabel;
    mSmallestLabel = nullptr;
    delete mBiggestLabel;
    mBiggestLabel = nullptr;
    delete mCurrentLabel;
    mCurrentLabel = nullptr;
    delete mSlider;
    mSlider = nullptr;
    delete mComboBox;
    mComboBox = nullptr;

    QGridLayout *layout = new QGridLayout(this);
    int margin = layout->margin();
    // Avoid double margins
    layout->setContentsMargins(0, 0, 0, 0);
    if (!mModes.empty()) {
        std::reverse(mModes.begin(), mModes.end());
        mComboBox = new QComboBox();
        mComboBox->setMinimumSize(402,48);
        mComboBox->setMaximumSize(1677215, 48);
//        mComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//        mComboBox->setEditable(false);
//        mComboBox->setStyleSheet(qss);
//        mComboBox->setItemDelegate(itemDelege);
//        mComboBox->setMaxVisibleItems(5);

        int currentModeIndex = -1;
        int preferredModeIndex = -1;
        Q_FOREACH (const QSize &size, mModes) {
            if (size.width() < 1024) {
                continue;
            }
            if (size == mModes[0]) {
                mComboBox->addItem(Utils::sizeToString(size) + tr(" (Recommended)"));
            } else {
                mComboBox->addItem(Utils::sizeToString(size));
            }

            if (mOutput->currentMode() && (mOutput->currentMode()->size() == size)) {
                currentModeIndex = mComboBox->count() - 1;
            } else if (mOutput->preferredMode() && (mOutput->preferredMode()->size() == size)) {
                preferredModeIndex = mComboBox->count() - 1;
            }
            //根据需求，当前版本只保留分辨率第一项,如需设置分辨率，删去这个break;
            break;
        }
        mComboBox->blockSignals(true);
        if (currentModeIndex != -1) {
            mComboBox->setCurrentIndex(currentModeIndex);
        } else if (preferredModeIndex != -1) {
            mComboBox->setCurrentIndex(preferredModeIndex);
        }
        mComboBox->blockSignals(false);

        layout->addWidget(mComboBox, 0, 0, 1, 1);
        connect(mComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &ResolutionSlider::slotValueChanged, Qt::UniqueConnection);
        Q_EMIT resolutionChanged(mModes.at(mComboBox->currentIndex()));
    } else {
        mCurrentLabel = new QLabel(this);
        mCurrentLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(mCurrentLabel, 1, 0, 1, 3);

        if (mModes.isEmpty()) {
            mCurrentLabel->setText(tr("No available resolutions"));
        } else if (mModes.count() == 1) {
            mCurrentLabel->setText(Utils::sizeToString(mModes.first()));
        } else {
            // No double margins left and right, but they are needed on top and bottom
            layout->setContentsMargins(0, margin, 0, margin);
            mSlider = new QSlider(Qt::Horizontal, this);
            mSlider->setTickInterval(1);
            mSlider->setTickPosition(QSlider::TicksBelow);
            mSlider->setSingleStep(1);
            mSlider->setPageStep(1);
            mSlider->setMinimum(0);
            mSlider->setMaximum(mModes.size() - 1);
            mSlider->setSingleStep(1);
            if (mOutput->currentMode()) {
                mSlider->setValue(mModes.indexOf(mOutput->currentMode()->size()));
            } else if (mOutput->preferredMode()) {
                mSlider->setValue(mModes.indexOf(mOutput->preferredMode()->size()));
            } else {
                mSlider->setValue(mSlider->maximum());
            }
            layout->addWidget(mSlider, 0, 1);
            connect(mSlider, &QSlider::valueChanged,
                    this, &ResolutionSlider::slotValueChanged);

            mSmallestLabel = new QLabel(this);
            mSmallestLabel->setText(Utils::sizeToString(mModes.first()));
            layout->addWidget(mSmallestLabel, 0, 0);
            mBiggestLabel = new QLabel(this);
            mBiggestLabel->setText(Utils::sizeToString(mModes.last()));
            layout->addWidget(mBiggestLabel, 0, 2);

            const auto size = mModes.at(mSlider->value());
            mCurrentLabel->setText(Utils::sizeToString(size));
            Q_EMIT resolutionChanged(size);
        }
    }
}

QSize ResolutionSlider::currentResolution() const
{
    if (mModes.isEmpty()) {
        return QSize();
    }

    if (mModes.size() < 2) {
        return mModes.first();
    }

    if (mSlider) {
        return mModes.at(mSlider->value());
    } else {
        const int i = mComboBox->currentIndex();
        return i > -1 ? mModes.at(i) : QSize();
    }
}

QSize ResolutionSlider::getMaxResolution() const {
    if (mModes.isEmpty()) {
        return QSize();
    }
    return mModes.first();
}

void ResolutionSlider::slotOutputModeChanged()
{
    if (!mOutput->currentMode()) {
        return;
    }

    if (mSlider) {
        mSlider->blockSignals(true);
        mSlider->setValue(mModes.indexOf(mOutput->currentMode()->size()));
        mSlider->blockSignals(false);
    } else if (mComboBox) {
        mComboBox->blockSignals(true);
        mComboBox->setCurrentIndex(mModes.indexOf(mOutput->currentMode()->size()));
        mComboBox->blockSignals(false);
    }
}

void ResolutionSlider::slotValueChanged(int value)
{
    const QSize &size = mModes.at(value);
    if (mCurrentLabel) {
        mCurrentLabel->setText(Utils::sizeToString(size));
    }

    Q_EMIT resolutionChanged(size);
}