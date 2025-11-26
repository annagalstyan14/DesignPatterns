#include "EffectWidget.h"
#include "../Core/Effects/IEffect.h"
#include "../Core/Effects/Echo.h"
#include "../Core/Effects/Reverb.h"
#include "../Core/Effects/Speed.h"
#include "../Core/Effects/Volume.h"
#include "../Core/Logging/ILogger.h"
#include <QHBoxLayout>

EffectWidget::EffectWidget(const QString& effectType, 
                           std::shared_ptr<ILogger> logger,
                           QWidget* parent)
    : QFrame(parent)
    , effectType_(effectType)
    , logger_(logger)
    , mainLayout_(nullptr)
    , enableCheckbox_(nullptr)
    , removeButton_(nullptr)
    , parametersWidget_(nullptr)
    , parametersLayout_(nullptr)
{
    setupUI();
}

void EffectWidget::setupUI() {
    // Frame styling
    setFrameShape(QFrame::StyledPanel);
    setStyleSheet(R"(
        EffectWidget {
            background-color: #2d2d2d;
            border: 1px solid #3d3d3d;
            border-radius: 6px;
        }
        EffectWidget:hover {
            border-color: #555555;
        }
    )");

    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(10, 8, 10, 10);
    mainLayout_->setSpacing(8);

    // --- Header Row ---
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(8);

    // Enable checkbox with effect name
    enableCheckbox_ = new QCheckBox(effectType_, this);
    enableCheckbox_->setChecked(true);
    enableCheckbox_->setStyleSheet(R"(
        QCheckBox {
            color: #e0e0e0;
            font-weight: bold;
            font-size: 13px;
            spacing: 8px;
        }
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 3px;
            border: 2px solid #555555;
            background-color: #2d2d2d;
        }
        QCheckBox::indicator:checked {
            background-color: #00bcd4;
            border-color: #00bcd4;
        }
        QCheckBox::indicator:hover {
            border-color: #00bcd4;
        }
    )");
    connect(enableCheckbox_, &QCheckBox::toggled, 
            this, &EffectWidget::onEnabledToggled);

    // Remove button
    removeButton_ = new QPushButton("×", this);
    removeButton_->setFixedSize(24, 24);
    removeButton_->setToolTip("Remove effect");
    removeButton_->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #808080;
            border: none;
            font-size: 18px;
            font-weight: bold;
        }
        QPushButton:hover {
            color: #ff5252;
        }
    )");
    connect(removeButton_, &QPushButton::clicked, 
            this, &EffectWidget::onRemoveClicked);

    headerLayout->addWidget(enableCheckbox_);
    headerLayout->addStretch();
    headerLayout->addWidget(removeButton_);
    mainLayout_->addLayout(headerLayout);

    // --- Parameters Container ---
    parametersWidget_ = new QWidget(this);
    parametersLayout_ = new QVBoxLayout(parametersWidget_);
    parametersLayout_->setContentsMargins(0, 0, 0, 0);
    parametersLayout_->setSpacing(6);
    mainLayout_->addWidget(parametersWidget_);

    // Setup effect-specific controls
    if (effectType_ == "Echo") {
        setupEchoControls();
    } else if (effectType_ == "Reverb") {
        setupReverbControls();
    } else if (effectType_ == "Speed") {
        setupSpeedControls();
    } else if (effectType_ == "Volume") {
        setupVolumeControls();
    }
}

void EffectWidget::addSlider(const QString& name, const QString& paramKey,
                              int min, int max, int defaultValue, 
                              const QString& suffix) {
    QHBoxLayout* rowLayout = new QHBoxLayout();
    rowLayout->setSpacing(10);

    // Parameter name label
    QLabel* nameLabel = new QLabel(name, this);
    nameLabel->setFixedWidth(70);
    nameLabel->setStyleSheet("color: #a0a0a0; font-size: 11px;");

    // Slider
    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(defaultValue);
    slider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 4px;
            background: #3d3d3d;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #00bcd4;
            width: 12px;
            height: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::handle:horizontal:hover {
            background: #00e5ff;
        }
        QSlider::sub-page:horizontal {
            background: #00bcd4;
            border-radius: 2px;
        }
    )");

    // Value label
    QLabel* valueLabel = new QLabel(QString::number(defaultValue) + suffix, this);
    valueLabel->setFixedWidth(45);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    valueLabel->setStyleSheet("color: #e0e0e0; font-size: 11px;");

    // Store slider data
    sliders_[paramKey] = {slider, valueLabel, suffix};

    // Connect slider
    connect(slider, &QSlider::valueChanged, this, [this, paramKey](int value) {
        auto& data = sliders_[paramKey];
        data.valueLabel->setText(QString::number(value) + data.suffix);
        emit parameterChanged();
    });

    rowLayout->addWidget(nameLabel);
    rowLayout->addWidget(slider, 1);
    rowLayout->addWidget(valueLabel);
    parametersLayout_->addLayout(rowLayout);
}

void EffectWidget::setupEchoControls() {
    addSlider("Intensity", "intensity", 0, 100, 50, "%");
}

void EffectWidget::setupReverbControls() {
    addSlider("Intensity", "intensity", 0, 100, 50, "%");
}

void EffectWidget::setupSpeedControls() {
    // Speed: 50 = 0.5x, 100 = 1.0x, 200 = 2.0x
    addSlider("Speed", "speed", 50, 200, 100, "%");
}

void EffectWidget::setupVolumeControls() {
    // Volume: 0 = 0%, 100 = 100%, 200 = 200%
    addSlider("Gain", "gain", 0, 200, 100, "%");
}

void EffectWidget::onRemoveClicked() {
    emit removeRequested(this);
}

void EffectWidget::onSliderChanged(int value) {
    Q_UNUSED(value);
    emit parameterChanged();
}

void EffectWidget::onEnabledToggled(bool enabled) {
    // Dim parameters when disabled
    parametersWidget_->setEnabled(enabled);
    
    if (enabled) {
        parametersWidget_->setStyleSheet("");
    } else {
        parametersWidget_->setStyleSheet("QWidget { color: #555555; }");
    }
    
    emit parameterChanged();
}

QString EffectWidget::getEffectType() const {
    return effectType_;
}

bool EffectWidget::isEffectEnabled() const {
    return enableCheckbox_->isChecked();
}

std::shared_ptr<IEffect> EffectWidget::createEffect() const {
    if (!isEffectEnabled()) {
        return nullptr;
    }

    if (effectType_ == "Echo") {
        auto echo = std::make_shared<Echo>(logger_);
        if (sliders_.count("intensity")) {
            echo->setIntensity(sliders_.at("intensity").slider->value());
        }
        return echo;
        
    } else if (effectType_ == "Reverb") {
        auto reverb = std::make_shared<Reverb>(logger_);
        if (sliders_.count("intensity")) {
            reverb->setIntensity(sliders_.at("intensity").slider->value());
        }
        return reverb;
        
    } else if (effectType_ == "Speed") {
        float speedFactor = 1.0f;
        if (sliders_.count("speed")) {
            speedFactor = sliders_.at("speed").slider->value() / 100.0f;
        }
        return std::make_shared<SpeedChangeEffect>(speedFactor, logger_);
        
    } else if (effectType_ == "Volume") {
        float gain = 1.0f;
        if (sliders_.count("gain")) {
            gain = sliders_.at("gain").slider->value() / 100.0f;
        }
        return std::make_shared<VolumeEffect>(gain, logger_);
    }

    return nullptr;
}