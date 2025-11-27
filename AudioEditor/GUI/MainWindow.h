#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>
#include <QFileInfo>
#include <memory>
#include <QFutureWatcher>
#include "EffectsPanel.h"  // Added this include for EffectsPanelState
// Forward declarations
class AudioEngine;
class AudioClip;
class TransportBar;
class WaveformWidget;
// class EffectsPanel;  // No longer needed as full include is added
class CaptionPanel;
class CaptionParser;
class ILogger;
class CommandHistory;
class IEffect;
class MainWindow : public QMainWindow {
Q_OBJECT
public:
explicit MainWindow(QWidget* parent = nullptr);
~MainWindow();
protected:
void closeEvent(QCloseEvent* event) override;
void dragEnterEvent(QDragEnterEvent* event) override;
void dropEvent(QDropEvent* event) override;
private slots:
// File menu actions
void onNewProject();
void onOpenAudio();
void onSaveAudio();
void onExportAudio();
void onExit();
// Edit menu actions
void onUndo();
void onRedo();
// Playback
void onTogglePlayPause();
// Captions
void onImportCaptions();
void onExportCaptions();
// Playback finished
void onPlaybackFinished();
void onRefreshAudioDevice();
// State updates
void onAudioLoaded();
void updateWindowTitle();
// Effects
void onApplyEffects();
void onPreviewTimerTimeout();
void onPreviewComputationFinished();
void onEffectStateChanged(const EffectsPanelState& oldState, const EffectsPanelState& newState);
private:
void setupUI();
void setupMenuBar();
void setupStatusBar();
void setupConnections();
void setupShortcuts();
void applyTheme();
void loadAudioFile(const QString& filePath);
void updateUIState();
bool confirmUnsavedChanges();
// Core components
std::shared_ptr<ILogger> logger_;
std::shared_ptr<AudioClip> audioClip_;
AudioEngine* audioEngine_;
CommandHistory* commandHistory_;
CaptionParser* captionParser_;
// Original samples for undo
std::vector<float> originalSamples_;
// UI Widgets
QWidget* centralWidget_;
TransportBar* transportBar_;
WaveformWidget* waveformWidget_;
EffectsPanel* effectsPanel_;
CaptionPanel* captionPanel_;
// Menus
QMenu* fileMenu_;
QMenu* editMenu_;
QMenu* effectsMenu_;
QMenu* helpMenu_;
// Actions
QAction* newAction_;
QAction* openAction_;
QAction* saveAction_;
QAction* exportAction_;
QAction* exitAction_;
QAction* undoAction_;
QAction* redoAction_;
QAction* importCaptionsAction_;
QAction* exportCaptionsAction_;
QAction* aboutAction_;
QTimer* previewDebounceTimer_;
QFutureWatcher<std::vector<float>>* previewWatcher_ = nullptr;
bool previewComputationQueued_ = false;
std::vector<std::shared_ptr<IEffect>> queuedEffects_;
bool discardPreviewResult_ = false;
// State
QString currentFilePath_;
bool hasUnsavedChanges_;
bool isPreviewMode_ = false;
void updatePreview();
void startPreviewComputation(const std::vector<std::shared_ptr<IEffect>>& effects);
void cancelPendingPreview();
};
#endif // MAIN_WINDOW_H