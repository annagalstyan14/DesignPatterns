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

    // Get all effects for applying to audio
    std::vector<std::shared_ptr<IEffect>> getEffects() const;
    
    // Clear all effects
    void clearEffects();
    
    // Enable/disable the panel
    void setEnabled(bool enabled);

signals:
    // Emitted when user clicks Apply
    void applyEffectsRequested();
    
    // Emitted when any effect parameter changes
    void effectsChanged();

private slots:
    void onAddEffectClicked();
    void onApplyClicked();
    void onEffectRemoved(EffectWidget* widget);
    void onEffectParameterChanged();

private:
    void setupUI();
    void addEffectWidget(const QString& effectType);
    void updateApplyButton();

    std::shared_ptr<ILogger> logger_;
    
    // UI Components
    QVBoxLayout* mainLayout_;
    QComboBox* effectTypeCombo_;
    QPushButton* addButton_;
    QScrollArea* scrollArea_;
    QWidget* effectsContainer_;
    QVBoxLayout* effectsLayout_;
    QPushButton* applyButton_;
    QLabel* noEffectsLabel_;
    
    // Effect widgets
    std::vector<EffectWidget*> effectWidgets_;
};

#endif // EFFECTS_PANEL_H