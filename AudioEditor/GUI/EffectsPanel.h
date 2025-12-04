#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <vector>
#include <memory>
#include "../Core/Commands/EffectState.h"

class EffectWidget;
class IEffect;
class ILogger;

class EffectsPanel : public QWidget {
    Q_OBJECT

public:
    explicit EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent = nullptr);
    ~EffectsPanel() override = default;
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffects() const;
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffectsForExport() const;
    [[nodiscard]] bool areEffectsEnabled() const noexcept { return effectsEnabled_; }
    void clearEffects();
    void setEnabled(bool enabled);
    void restoreState(const EffectsPanelState& state);
    [[nodiscard]] EffectsPanelState saveState() const;

signals:
    void effectsChanged();
    void compareToggled(bool effectsEnabled);
    void applyEffectsRequested();
    void stateChangeCompleted(const EffectsPanelState& oldState, 
                              const EffectsPanelState& newState);

private slots:
    void onAddEffectClicked();
    void onApplyClicked();
    void onEffectRemoved(EffectWidget* widget);
    void onEffectParameterChanged();
    void onCompareToggled(bool enabled);

private:
    void setupUI();
    void addEffectWidget(const QString& effectType);
    void addEffectWidget(const QString& effectType, const QMap<QString, int>& params);

    std::shared_ptr<ILogger> logger_;
    
    // UI Components
    QVBoxLayout* mainLayout_ = nullptr;
    QComboBox* effectTypeCombo_ = nullptr;
    QPushButton* addButton_ = nullptr;
    QPushButton* compareButton_ = nullptr;
    QScrollArea* scrollArea_ = nullptr;
    QWidget* effectsContainer_ = nullptr;
    QVBoxLayout* effectsLayout_ = nullptr;
    QLabel* noEffectsLabel_ = nullptr;
    
    // Effect widgets
    std::vector<EffectWidget*> effectWidgets_;
    
    // State
    bool effectsEnabled_ = true;
    EffectsPanelState lastSavedState_;
    bool isRestoringState_ = false;
};
