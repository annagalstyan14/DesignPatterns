#ifndef EFFECT_WIDGET_H
#define EFFECT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QTimer>
#include <QMap>
#include <memory>
#include <map>

// Forward declarations
class IEffect;
class ILogger;

/**
 * @brief Widget for a single audio effect with parameter controls
 * 
 * Displays effect name, enable checkbox, parameter sliders, and remove button.
 */
class EffectWidget : public QFrame {
    Q_OBJECT

public:
    EffectWidget(const QString& effectType, std::shared_ptr<ILogger> logger, 
                 QWidget* parent = nullptr);
    ~EffectWidget() = default;

    // Get the effect type name
    QString getEffectType() const;
    
    // Check if effect is enabled
    bool isEffectEnabled() const;
    
    // Create the actual effect with current parameters
    std::shared_ptr<IEffect> createEffect() const;
    
    // Get/set parameter state for undo/redo
    QMap<QString, int> getParameterState() const;
    void setParameterState(const QMap<QString, int>& state);
    int getPreviousSliderValue(const QString& paramKey) const;

signals:
    void removeRequested(EffectWidget* widget);
    void parameterChanged();

private slots:
    void onRemoveClicked();
    void onSliderChanged(int value);
    void onSliderPressed();
    void onSliderReleased();
    void onEnabledToggled(bool enabled);

private:
    void setupUI();
    void setupReverbControls();
    void setupSpeedControls();
    void setupVolumeControls();
    
    void addSlider(const QString& name, const QString& paramKey,
                   int min, int max, int defaultValue, const QString& suffix = "");
    void scheduleParameterChange();
    void setSliderDragging(const QString& paramKey, bool dragging);

    QString effectType_;
    std::shared_ptr<ILogger> logger_;
    
    // UI Components
    QVBoxLayout* mainLayout_;
    QCheckBox* enableCheckbox_;
    QPushButton* removeButton_;
    QWidget* parametersWidget_;
    QVBoxLayout* parametersLayout_;
    
    // Parameter sliders and their current values
    struct SliderData {
        QSlider* slider;
        QLabel* valueLabel;
        QString suffix;
        bool isDragging = false;
        int previousValue = 0;
    };
    std::map<QString, SliderData> sliders_;

    QTimer* parameterDebounceTimer_;
};

#endif // EFFECT_WIDGET_H