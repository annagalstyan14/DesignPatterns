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
#include <memory>

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
    
    // Effects
    void onApplyEffects();
    
    // Captions
    void onImportCaptions();
    void onExportCaptions();
    
    // Playback finished
    void onPlaybackFinished();
    
    // State updates
    void onAudioLoaded();
    void updateWindowTitle();

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
    
    // State
    QString currentFilePath_;
    bool hasUnsavedChanges_;
};

#endif // MAIN_WINDOW_H