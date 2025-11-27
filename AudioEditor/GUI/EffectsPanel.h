#ifndef EFFECTS_PANEL_H
#define EFFECTS_PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <vector>
#include <memory>

// Forward declarations
class EffectWidget;
class IEffect;
class ILogger;

/**
 * @brief Panel containing audio effect controls
 * 
 * Allows user to:
 * - Add effects from dropdown
 * - Remove effects
 * - Adjust effect parameters with sliders
 * - Apply effects to audio
 * 
 * Design patterns:
 * - Observer: Emits signals when effects change
 * - Factory: Uses EffectFactory to create effects
 */
class EffectsPanel : public QWidget {
    Q_OBJECT

public:
    explicit EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent = nullptr);
    ~EffectsPanel() = default;

    // Get all effects for preview (returns empty if effects disabled)
    std::vector<std::shared_ptr<IEffect>> getEffects() const;

    // Get effects regardless of toggle state (for saving)
    std::vector<std::shared_ptr<IEffect>> getEffectsForExport() const;

    // Check if effects are enabled
    bool areEffectsEnabled() const { return effectsEnabled_; }
    
    // Clear all effects
    void clearEffects();
    
    // Enable/disable the panel
    void setEnabled(bool enabled);

    // NEW: Add method to sync widgets from external chain (for undo/redo)
    void syncFromChain(const std::vector<std::shared_ptr<IEffect>>& chain);

signals:
    // Emitted when effects list or parameters change (for preview)
    void effectsChanged();
    
    // Emitted when user wants to compare (toggle effects on/off)
    void compareToggled(bool effectsEnabled);

    // NEW: Signals for commands (bubble to MainWindow)
    void addEffectRequested(const QString& effectType);
    void removeEffectRequested(size_t index);
    void paramChangedRequested(size_t index, const QString& param, float oldValue, float newValue);

private slots:
    void onAddEffectClicked();
    void onEffectRemoved(EffectWidget* widget);
    void onEffectParameterChanged();
    void onCompareToggled(bool enabled);

private:
    void setupUI();
    void addEffectWidget(const QString& effectType);

    QPushButton* compareButton_;
    bool effectsEnabled_ = true;

    std::shared_ptr<ILogger> logger_;
    
    // UI Components
    QVBoxLayout* mainLayout_;
    QComboBox* effectTypeCombo_;
    QPushButton* addButton_;
    QScrollArea* scrollArea_;
    QWidget* effectsContainer_;
    QVBoxLayout* effectsLayout_;
    QLabel* noEffectsLabel_;
    
    // Effect widgets
    std::vector<EffectWidget*> effectWidgets_;
};

#endif // EFFECTS_PANEL_H