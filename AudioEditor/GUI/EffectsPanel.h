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

class EffectWidget;
class IEffect;
class ILogger;

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

signals:
    void effectsChanged();
    void compareToggled(bool effectsEnabled);
    void applyEffectsRequested();

private slots:
    void onAddEffectClicked();
    void onApplyClicked();
    void onEffectRemoved(EffectWidget* widget);
    void onEffectParameterChanged();
    void onCompareToggled(bool enabled);

private:
    void setupUI();
    void addEffectWidget(const QString& effectType);

    QPushButton* compareButton_;
    QPushButton* applyButton_;
    bool effectsEnabled_ = true;

    std::shared_ptr<ILogger> logger_;
    
    QVBoxLayout* mainLayout_;
    QComboBox* effectTypeCombo_;
    QPushButton* addButton_;
    QScrollArea* scrollArea_;
    QWidget* effectsContainer_;
    QVBoxLayout* effectsLayout_;
    QLabel* noEffectsLabel_;
    
    std::vector<EffectWidget*> effectWidgets_;
};

#endif // EFFECTS_PANEL_H