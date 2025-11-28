#pragma once

#include <QString>
#include <QMap>
#include <vector>

/**
 * @brief State snapshot for a single effect
 * 
 * Used for undo/redo support. Stored separately to avoid
 * circular dependencies between Core and GUI layers.
 */
struct EffectState {
    QString effectType;
    bool enabled = true;
    QMap<QString, int> parameters;
};

/**
 * @brief Complete state snapshot of the effects panel
 */
struct EffectsPanelState {
    bool effectsEnabled = true;
    std::vector<EffectState> effects;
};
