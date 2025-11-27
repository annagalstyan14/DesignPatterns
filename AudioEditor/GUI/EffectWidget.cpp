#include "EffectWidget.h"
#include "../Core/Effects/IEffect.h"
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
    , parameterDebounceTimer_(new QTimer(this))
{
    parameterDebounceTimer_->setSingleShot(true);
    parameterDebounceTimer_->setInterval(80);
    connect(parameterDebounceTimer_, &QTimer::timeout, this, [this]() {
        emit parameterChanged();
    });

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
            border-radiu...(truncated as per original)...
    )");
    connect(enableCheckbox_, &QCheckBox::toggled, this, &EffectWidget::onEnabledToggled);
    headerLayout->addWidget(enableCheckbox_);

    // Remove button
    removeButton_ = new QPushButton("Remove", this);
    connect(removeButton_, &QPushButton::clicked, this, &EffectWidget::onRemoveClicked);
    headerLayout->addStretch();
    headerLayout->addWidget(removeButton_);

    mainLayout_->addLayout(headerLayout);

    // Parameters container
    parametersWidget_ = new QWidget(this);
    parametersLayout_ = new QVBoxLayout(parametersWidget_);
    parametersLayout_->setContentsMargins(0, 0, 0, 0);
    parametersLayout_->setSpacing(6);
    mainLayout_->addWidget(parametersWidget_);

    // Setup specific controls
    if (effectType_ == "Reverb") {
        setupReverbControls();
    } else if (effectType_ == "Speed") {
        setupSpeedControls();
    } else if (effectType_ == "Volume") {
        setupVolumeControls();
    }
}

void EffectWidget::setupReverbControls() {
    addSlider("Intensity", "intensity", 0, 100, 50, "%");
    // Add more if needed
}

void EffectWidget::setupSpeedControls() {
    addSlider("Speed", "speed", 50, 200, 100, "%");
}

void EffectWidget::setupVolumeControls() {
    addSlider("Gain", "gain", 0, 400, 100, "%");
}

void EffectWidget::addSlider(const QString& name, const QString& paramKey,
                             int min, int max, int defaultValue, const QString& suffix) {
    QHBoxLayout* sliderLayout = new QHBoxLayout();
    sliderLayout->setSpacing(8);

    QLabel* label = new QLabel(name + ":", this);
    label->setStyleSheet("color: #e0e0e0; font-size: 12px;");
    sliderLayout->addWidget(label);

    QSlider* slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);
    slider->setValue(defaultValue);
    slider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            background: #3d3d3d;
            height: 4px;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #00bcd4;
            width: 14px;
            height: 14px;
            border-radius: 7px;
        }
    )");
    sliderLayout->addWidget(slider);

    QLabel* valueLabel = new QLabel(QString::number(defaultValue) + suffix, this);
    valueLabel->setStyleSheet("color: #00bcd4; font-size: 12px; min-width: 40px;");
    sliderLayout->addWidget(valueLabel);

    sliders_[paramKey] = {slider, valueLabel, suffix};

    connect(slider, &QSlider::valueChanged, this, &EffectWidget::onSliderChanged);
    connect(slider, &QSlider::sliderPressed, this, &EffectWidget::onSliderPressed);
    connect(slider, &QSlider::sliderReleased, this, &EffectWidget::onSliderReleased);
    parametersLayout_->addLayout(sliderLayout);
}

void EffectWidget::onRemoveClicked() {
    emit removeRequested(this);
}

void EffectWidget::onSliderChanged(int value) {
    auto paramKey = sender()->property("paramKey").toString();  // Set property in addSlider if needed
    if (paramKey.isEmpty()) {
        // Find key
        for (auto& entry : sliders_) {
            if (entry.second.slider == sender()) {
                paramKey = entry.first;
                break;
            }
        }
    }
    sliders_[paramKey].valueLabel->setText(QString::number(value) + sliders_[paramKey].suffix);
}

void EffectWidget::onSliderPressed() {
    auto paramKey = sender()->property("paramKey").toString();
    if (paramKey.isEmpty()) {
        for (auto& entry : sliders_) {
            if (entry.second.slider == sender()) {
                paramKey = entry.first;
                break;
            }
        }
    }
    sliders_[paramKey].previousValue = sliders_[paramKey].slider->value();
    setSliderDragging(paramKey, true);
}

void EffectWidget::onSliderReleased() {
    auto paramKey = sender()->property("paramKey").toString();
    if (paramKey.isEmpty()) {
        for (auto& entry : sliders_) {
            if (entry.second.slider == sender()) {
                paramKey = entry.first;
                break;
            }
        }
    }
    setSliderDragging(paramKey, false);
    int newValue = sliders_[paramKey].slider->value();
    int oldValue = sliders_[paramKey].previousValue;
    if (newValue != oldValue) {
        // Emit for command (scale to float, e.g., /100.0f)
        emit parameterChanged();  // Immediate preview
        emit paramChangedRequested(effectIndex_, paramKey, oldValue / 100.0f, newValue / 100.0f);  // Bubble for undo, assume effectIndex_ member set by EffectsPanel
    }
}

void EffectWidget::onEnabledToggled(bool enabled) {
    // Dim parameters when disabled
    parametersWidget_->setEnabled(enabled);
    
    if (enabled) {
        parametersWidget_->setStyleSheet("");
    } else {
        parametersWidget_->setStyleSheet("QWidget { color: #555555; }");
    }
    
    parameterDebounceTimer_->stop();
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

    if (effectType_ == "Reverb") {
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

void EffectWidget::scheduleParameterChange() {
    // If any slider is currently being dragged, wait for release
    for (const auto& entry : sliders_) {
        if (entry.second.isDragging) {
            return;
        }
    }
    parameterDebounceTimer_->start();
}

void EffectWidget::setSliderDragging(const QString& paramKey, bool dragging) {
    auto it = sliders_.find(paramKey);
    if (it == sliders_.end()) {
        return;
    }
    it->second.isDragging = dragging;
}