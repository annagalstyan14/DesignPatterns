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
#include <memory>
#include <QFutureWatcher>

// Forward declarations
class AudioEngine;
class AudioClip;
class TransportBar;
class WaveformWidget;
class EffectsPanel;
class CaptionPanel;
class CaptionParser;
class ILogger;
class CommandHistory;
class IEffect;

/**
 * @brief Main application window
 * 
 * Manages the overall layout and coordinates all components:
 * - Menu bar with File, Edit, Effects, Help menus
 * - Transport bar for playback controls
 * - Waveform display
 * - Effects panel
 * - Caption panel
 * 
 * Design patterns:
 * - Mediator: Coordinates between all components
 * - Observer: Qt signals/slots for event handling
 */
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
    
    // Captions
    void onImportCaptions();
    void onExportCaptions();
    
    // Playback finished
    void onPlaybackFinished();

    void onRefreshAudioDevice();
    
    // State updates
    void onAudioLoaded();
    void updateWindowTitle();

    // Effects slots (NEW)
    void onAddEffect(const QString& effectType);
    void onRemoveEffect(size_t index);
    void onParamChanged(size_t index, const QString& param, float oldValue, float newValue);

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
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

private slots:
    void onPreviewTimerTimeout();
    void onPreviewComputationFinished();

private:
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

    // NEW: Effect chain management
    std::vector<std::shared_ptr<IEffect>> effectChain_;
    void syncEffectsPanel();
};

#endif // MAIN_WINDOW_H