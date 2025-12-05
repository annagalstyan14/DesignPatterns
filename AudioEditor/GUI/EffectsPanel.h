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
    
    EffectsPanel(const EffectsPanel&) = delete;
    EffectsPanel& operator=(const EffectsPanel&) = delete;
    
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffects() const;
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffectsForExport() const;
    [[nodiscard]] bool areEffectsEnabled() const noexcept { return effectsEnabled_; }
    [[nodiscard]] size_t effectCount() const noexcept { return effectWidgets_.size(); }
    
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
    
    QVBoxLayout* mainLayout_;
    QComboBox* effectTypeCombo_;
    QPushButton* addButton_;
    QPushButton* compareButton_;
    QScrollArea* scrollArea_;
    QWidget* effectsContainer_;
    QVBoxLayout* effectsLayout_;
    QLabel* noEffectsLabel_;
    
    std::vector<EffectWidget*> effectWidgets_;
    
    bool effectsEnabled_;
    EffectsPanelState lastSavedState_;
    bool isRestoringState_;
};
