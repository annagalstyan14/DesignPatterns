#ifndef EFFECTS_PANEL_H
#define EFFECTS_PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMap>
#include <QString>
#include <vector>
#include <memory>

class EffectWidget;
class IEffect;
class ILogger;

struct EffectState {
    QString effectType;
    bool enabled;
    QMap<QString, int> parameters;
};

struct EffectsPanelState {
    bool effectsEnabled;
    std::vector<EffectState> effects;
};

class EffectsPanel : public QWidget {
    Q_OBJECT

public:
    explicit EffectsPanel(std::shared_ptr<ILogger> logger, QWidget* parent = nullptr);
    ~EffectsPanel() = default;

    std::vector<std::shared_ptr<IEffect>> getEffects() const;
    std::vector<std::shared_ptr<IEffect>> getEffectsForExport() const;
    bool areEffectsEnabled() const { return effectsEnabled_; }
    void clearEffects();
    void setEnabled(bool enabled);
    void restoreState(const EffectsPanelState& state);
    EffectsPanelState saveState() const;

signals:
    void effectsChanged();
    void compareToggled(bool effectsEnabled);
    void applyEffectsRequested();
    void stateChangeCompleted(const EffectsPanelState& oldState, const EffectsPanelState& newState);

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

    QPushButton* compareButton_;
    QPushButton* applyButton_;
    bool effectsEnabled_ = true;

    std::shared_ptr<ILogger> logger_;
    std::vector<std::shared_ptr<IEffect>> effects_;
    
    QVBoxLayout* mainLayout_;
    QComboBox* effectTypeCombo_;
    QPushButton* addButton_;
    QScrollArea* scrollArea_;
    QWidget* effectsContainer_;
    QVBoxLayout* effectsLayout_;
    QLabel* noEffectsLabel_;
    
    std::vector<EffectWidget*> effectWidgets_;

    EffectsPanelState lastSavedState_;
    bool isRestoringState_ = false;
};

#endif // EFFECTS_PANEL_H