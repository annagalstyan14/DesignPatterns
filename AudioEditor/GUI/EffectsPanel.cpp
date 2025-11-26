#include "EffectsPanel.h"
#include "EffectWidget.h"
#include "../Core/Effects/IEffect.h"
#include "../Core/EffectFactory.h"
#include "../Core/Logging/ILogger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>

EffectsPanel::EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent)
    : QWidget(parent)
    , logger_(logger)
    , mainLayout_(nullptr)
    , effectTypeCombo_(nullptr)
    , addButton_(nullptr)
    , scrollArea_(nullptr)
    , effectsContainer_(nullptr)
    , effectsLayout_(nullptr)
    , applyButton_(nullptr)
    , noEffectsLabel_(nullptr)
{
    setupUI();
}

void EffectsPanel::setupUI() {
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(10, 10, 10, 10);
    mainLayout_->setSpacing(10);

    // --- Header ---
    QLabel* headerLabel = new QLabel("Effects", this);
    headerLabel->setStyleSheet(R"(
        QLabel {
            color: #00bcd4;
            font-size: 16px;
            font-weight: bold;
            padding: 5px 0;
        }
    )");
    mainLayout_->addWidget(headerLabel);

    // --- Add Effect Row ---
    QHBoxLayout* addLayout = new QHBoxLayout();
    addLayout->setSpacing(8);

    effectTypeCombo_ = new QComboBox(this);
    effectTypeCombo_->addItem("Echo");
    effectTypeCombo_->addItem("Reverb");
    effectTypeCombo_->addItem("Speed");
    effectTypeCombo_->addItem("Volume");
    effectTypeCombo_->setStyleSheet(R"(
        QComboBox {
            background-color: #3d3d3d;
            color: #e0e0e0;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 6px 10px;
            min-width: 100px;
        }
        QComboBox:hover {
            border-color: #00bcd4;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #808080;
            margin-right: 5px;
        }
        QComboBox QAbstractItemView {
            background-color: #3d3d3d;
            color: #e0e0e0;
            selection-background-color: #00bcd4;
            selection-color: #000000;
            border: 1px solid #555555;
        }
    )");

    addButton_ = new QPushButton("+ Add", this);
    addButton_->setStyleSheet(R"(
        QPushButton {
            background-color: #4caf50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 15px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #66bb6a;
        }
        QPushButton:pressed {
            background-color: #388e3c;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #808080;
        }
    )");
    connect(addButton_, &QPushButton::clicked, this, &EffectsPanel::onAddEffectClicked);

    addLayout->addWidget(effectTypeCombo_, 1);
    addLayout->addWidget(addButton_);
    mainLayout_->addLayout(addLayout);

    // --- Separator ---
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: #3d3d3d;");
    separator->setFixedHeight(1);
    mainLayout_->addWidget(separator);

    // --- Scroll Area for Effects ---
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea_->setStyleSheet(R"(
        QScrollArea {
            background-color: transparent;
            border: none;
        }
        QScrollBar:vertical {
            background-color: #2d2d2d;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background-color: #555555;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #666666;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");

    effectsContainer_ = new QWidget();
    effectsContainer_->setStyleSheet("background-color: transparent;");
    
    effectsLayout_ = new QVBoxLayout(effectsContainer_);
    effectsLayout_->setContentsMargins(0, 0, 0, 0);
    effectsLayout_->setSpacing(8);
    effectsLayout_->setAlignment(Qt::AlignTop);

    // No effects label
    noEffectsLabel_ = new QLabel("No effects added.\nSelect an effect and click '+ Add'.", this);
    noEffectsLabel_->setAlignment(Qt::AlignCenter);
    noEffectsLabel_->setStyleSheet(R"(
        QLabel {
            color: #808080;
            font-size: 12px;
            padding: 30px;
        }
    )");
    effectsLayout_->addWidget(noEffectsLabel_);

    scrollArea_->setWidget(effectsContainer_);
    mainLayout_->addWidget(scrollArea_, 1);

    // --- Apply Button ---
    applyButton_ = new QPushButton("Apply Effects", this);
    applyButton_->setEnabled(false);
    applyButton_->setStyleSheet(R"(
        QPushButton {
            background-color: #00bcd4;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #00acc1;
        }
        QPushButton:pressed {
            background-color: #0097a7;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #808080;
        }
    )");
    connect(applyButton_, &QPushButton::clicked, this, &EffectsPanel::onApplyClicked);
    mainLayout_->addWidget(applyButton_);

    // Panel background
    setStyleSheet(R"(
        EffectsPanel {
            background-color: #252525;
            border-left: 1px solid #3d3d3d;
        }
    )");
    
    setMinimumWidth(250);
    setMaximumWidth(350);
}

void EffectsPanel::onAddEffectClicked() {
    QString effectType = effectTypeCombo_->currentText();
    addEffectWidget(effectType);
    
    if (logger_) {
        logger_->log("Added effect: " + effectType.toStdString());
    }
}

void EffectsPanel::addEffectWidget(const QString& effectType) {
    // Hide "no effects" label
    noEffectsLabel_->hide();
    
    // Create effect widget
    EffectWidget* effectWidget = new EffectWidget(effectType, logger_, this);
    
    // Connect signals
    connect(effectWidget, &EffectWidget::removeRequested,
            this, &EffectsPanel::onEffectRemoved);
    connect(effectWidget, &EffectWidget::parameterChanged,
            this, &EffectsPanel::onEffectParameterChanged);
    
    // Add to layout and list
    effectsLayout_->addWidget(effectWidget);
    effectWidgets_.push_back(effectWidget);
    
    updateApplyButton();
    emit effectsChanged();
}

void EffectsPanel::onEffectRemoved(EffectWidget* widget) {
    // Remove from list
    auto it = std::find(effectWidgets_.begin(), effectWidgets_.end(), widget);
    if (it != effectWidgets_.end()) {
        effectWidgets_.erase(it);
    }
    
    // Remove from layout and delete
    effectsLayout_->removeWidget(widget);
    widget->deleteLater();
    
    // Show "no effects" label if empty
    if (effectWidgets_.empty()) {
        noEffectsLabel_->show();
    }
    
    updateApplyButton();
    emit effectsChanged();
    
    if (logger_) {
        logger_->log("Effect removed");
    }
}

void EffectsPanel::onEffectParameterChanged() {
    emit effectsChanged();
}

void EffectsPanel::onApplyClicked() {
    emit applyEffectsRequested();
}

std::vector<std::shared_ptr<IEffect>> EffectsPanel::getEffects() const {
    std::vector<std::shared_ptr<IEffect>> effects;
    
    for (EffectWidget* widget : effectWidgets_) {
        if (widget->isEffectEnabled()) {
            auto effect = widget->createEffect();
            if (effect) {
                effects.push_back(effect);
            }
        }
    }
    
    return effects;
}

void EffectsPanel::clearEffects() {
    // Remove all effect widgets
    for (EffectWidget* widget : effectWidgets_) {
        effectsLayout_->removeWidget(widget);
        widget->deleteLater();
    }
    effectWidgets_.clear();
    
    noEffectsLabel_->show();
    updateApplyButton();
}

void EffectsPanel::setEnabled(bool enabled) {
    effectTypeCombo_->setEnabled(enabled);
    addButton_->setEnabled(enabled);
    
    for (EffectWidget* widget : effectWidgets_) {
        widget->setEnabled(enabled);
    }
    
    updateApplyButton();
}

void EffectsPanel::updateApplyButton() {
    bool hasEffects = !effectWidgets_.empty();
    applyButton_->setEnabled(hasEffects);
}