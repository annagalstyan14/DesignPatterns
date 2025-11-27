#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <vector>
#include <memory>
#include "../Core/Commands/EffectStateCommand.h"  // For EffectsPanelState

class EffectWidget;
class IEffect;
class ILogger;

/**
 * @brief Panel for managing audio effects
 * 
 * Allows adding, removing, and configuring effects.
 * Emits signals when effects change for preview updates.
 * 
 * Design Pattern: Observer (Qt signals/slots)
 */
class EffectsPanel : public QWidget {
    Q_OBJECT

public:
    explicit EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent = nullptr);
    ~EffectsPanel() override = default;

    /**
     * @brief Get all currently enabled effects
     */
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffects() const;
    
    /**
     * @brief Get all effects regardless of enabled state (for export)
     */
    [[nodiscard]] std::vector<std::shared_ptr<IEffect>> getEffectsForExport() const;
    
    /**
     * @brief Check if effects are globally enabled (compare toggle)
     */
    [[nodiscard]] bool areEffectsEnabled() const noexcept { return effectsEnabled_; }
    
    /**
     * @brief Clear all effects
     */
    void clearEffects();
    
    /**
     * @brief Enable/disable the panel
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Restore state from snapshot (for undo/redo)
     */
    void restoreState(const EffectsPanelState& state);
    
    /**
     * @brief Save current state to snapshot
     */
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
