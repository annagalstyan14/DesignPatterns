#include "EffectsPanel.h"
#include "EffectWidget.h"
#include "../Core/Effects/Reverb.h"
#include "../Core/Effects/Speed.h"
#include "../Core/Effects/Volume.h"
#include <QHBoxLayout>

EffectsPanel::EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent)
    : QWidget(parent), logger_(logger) {
    setupUI();
}

void EffectsPanel::setupUI() {
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(10, 10, 10, 10);
    mainLayout_->setSpacing(10);

    // Header or other UI...

    // Add effect
    QHBoxLayout* addLayout = new QHBoxLayout();
    effectTypeCombo_ = new QComboBox(this);
    effectTypeCombo_->addItems({"Reverb", "Speed", "Volume"});
    addButton_ = new QPushButton("Add Effect", this);
    connect(addButton_, &QPushButton::clicked, this, &EffectsPanel::onAddEffectClicked);
    addLayout->addWidget(effectTypeCombo_);
    addLayout->addWidget(addButton_);
    mainLayout_->addLayout(addLayout);

    // Scroll area for effects
    scrollArea_ = new QScrollArea(this);
    effectsContainer_ = new QWidget();
    effectsLayout_ = new QVBoxLayout(effectsContainer_);
    effectsLayout_->setSpacing(8);
    scrollArea_->setWidget(effectsContainer_);
    scrollArea_->setWidgetResizable(true);
    mainLayout_->addWidget(scrollArea_);

    // No effects label
    noEffectsLabel_ = new QLabel("No effects added", this);
    mainLayout_->addWidget(noEffectsLabel_);

    // Compare button
    compareButton_ = new QPushButton("Compare On/Off", this);
    compareButton_->setCheckable(true);
    compareButton_->setChecked(true);
    connect(compareButton_, &QPushButton::toggled, this, &EffectsPanel::onCompareToggled);
    mainLayout_->addWidget(compareButton_);
}

void EffectsPanel::onAddEffectClicked() {
    QString type = effectTypeCombo_->currentText();
    emit addEffectRequested(type);  // Bubble to MainWindow for command
}

void EffectsPanel::addEffectWidget(const QString& effectType) {
    EffectWidget* widget = new EffectWidget(effectType, logger_, this);
    connect(widget, &EffectWidget::removeRequested, this, &EffectsPanel::onEffectRemoved);
    connect(widget, &EffectWidget::parameterChanged, this, &EffectsPanel::onEffectParameterChanged);
    // NEW: Connect param changed for undo (but actual command in widget on release)
    effectsLayout_->addWidget(widget);
    effectWidgets_.push_back(widget);
    noEffectsLabel_->setVisible(false);
    emit effectsChanged();
}

void EffectsPanel::onEffectRemoved(EffectWidget* widget) {
    size_t index = std::find(effectWidgets_.begin(), effectWidgets_.end(), widget) - effectWidgets_.begin();
    emit removeEffectRequested(index);  // Bubble for command
    widget->deleteLater();
    effectWidgets_.erase(effectWidgets_.begin() + index);
    if (effectWidgets_.empty()) noEffectsLabel_->setVisible(true);
    emit effectsChanged();
}

void EffectsPanel::onEffectParameterChanged() {
    emit effectsChanged();
}

void EffectsPanel::onCompareToggled(bool enabled) {
    effectsEnabled_ = enabled;
    emit compareToggled(enabled);
    emit effectsChanged();
}

std::vector<std::shared_ptr<IEffect>> EffectsPanel::getEffects() const {
    if (!effectsEnabled_) return {};
    std::vector<std::shared_ptr<IEffect>> effects;
    for (auto widget : effectWidgets_) {
        auto effect = widget->createEffect();
        if (effect) effects.push_back(effect);
    }
    return effects;
}

std::vector<std::shared_ptr<IEffect>> EffectsPanel::getEffectsForExport() const {
    std::vector<std::shared_ptr<IEffect>> effects;
    for (auto widget : effectWidgets_) {
        auto effect = widget->createEffect();
        if (effect) effects.push_back(effect);
    }
    return effects;
}

void EffectsPanel::clearEffects() {
    for (auto widget : effectWidgets_) {
        widget->deleteLater();
    }
    effectWidgets_.clear();
    noEffectsLabel_->setVisible(true);
}

void EffectsPanel::setEnabled(bool enabled) {
    addButton_->setEnabled(enabled);
    compareButton_->setEnabled(enabled);
    for (auto widget : effectWidgets_) {
        widget->setEnabled(enabled);
    }
}

void EffectsPanel::syncFromChain(const std::vector<std::shared_ptr<IEffect>>& chain) {
    clearEffects();
    for (auto effect : chain) {
        QString type;
        if (dynamic_cast<Reverb*>(effect.get())) type = "Reverb";
        else if (dynamic_cast<SpeedChangeEffect*>(effect.get())) type = "Speed";
        else if (dynamic_cast<VolumeEffect*>(effect.get())) type = "Volume";
        addEffectWidget(type);
        // Sync sliders from effect params
        auto widget = effectWidgets_.back();
        // Example for Reverb
        if (type == "Reverb") {
            auto reverb = std::dynamic_pointer_cast<Reverb>(effect);
            widget->sliders_["intensity"].slider->setValue(static_cast<int>(reverb->wetMix_ * 200.0f));  // Adjust scaling
            // Similar for other params
        }
        // Repeat for Speed and Volume
        if (type == "Speed") {
            auto speed = std::dynamic_pointer_cast<SpeedChangeEffect>(effect);
            widget->sliders_["speed"].slider->setValue(static_cast<int>(speed->getSpeedFactor() * 100.0f));
        }
        if (type == "Volume") {
            auto volume = std::dynamic_pointer_cast<VolumeEffect>(effect);
            widget->sliders_["gain"].slider->setValue(static_cast<int>(volume->getGain() * 100.0f));
        }
    }
}