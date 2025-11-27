#include "MainWindow.h"
#include "AudioEngine.h"
#include "TransportBar.h"
#include "WaveformWidget.h"
#include "EffectsPanel.h"
#include "CaptionPanel.h"
#include "CaptionParser.h"
#include "../Core/AudioClip.h"
#include "../Core/Commands/CommandHistory.h"
#include "../Core/Logging/CompositeLogger.h"
#include "../Core/Logging/ConsoleLogger.h"
#include "../Core/Logging/FileLogger.h"
#include "../Core/EffectFactory.h"
#include "../Core/Effects/Reverb.h"
#include "../Core/Effects/Speed.h"
#include "../Core/Effects/Volume.h"
#include "../Core/Commands/ApplyEffect.h"
#include <QApplication>
#include <QScreen>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QtConcurrent/QtConcurrentRun>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , audioClip_(nullptr)
    , audioEngine_(nullptr)
    , commandHistory_(nullptr)
    , captionParser_(nullptr)
    , centralWidget_(nullptr)
    , transportBar_(nullptr)
    , waveformWidget_(nullptr)
    , effectsPanel_(nullptr)
    , captionPanel_(nullptr)
    , previewDebounceTimer_(nullptr)
    , hasUnsavedChanges_(false)
{
    // Setup logger
    auto compositeLogger = std::make_shared<CompositeLogger>();
    compositeLogger->addLogger(std::make_shared<ConsoleLogger>());
    compositeLogger->addLogger(std::make_shared<FileLogger>("audioeditor.log"));
    logger_ = compositeLogger;
    
    logger_->log("Application starting...");
    
    // Register effects with factory
    EffectFactory::registerEffect("Reverb", [](std::shared_ptr<ILogger> log) {
        return std::make_shared<Reverb>(log);
    });
    EffectFactory::registerEffect("Speed", [](std::shared_ptr<ILogger> log) {
        return std::make_shared<SpeedChangeEffect>(1.0f, log);
    });
    EffectFactory::registerEffect("Volume", [](std::shared_ptr<ILogger> log) {
        return std::make_shared<VolumeEffect>(1.0f, log);
    });
    
    // Initialize components
    audioEngine_ = new AudioEngine(this);
    commandHistory_ = new CommandHistory(logger_);
    captionParser_ = new CaptionParser(logger_);
    
    // Debounce timer for smooth slider preview
    previewDebounceTimer_ = new QTimer(this);
    previewDebounceTimer_->setSingleShot(true);
    previewDebounceTimer_->setInterval(150);
    connect(previewDebounceTimer_, &QTimer::timeout, this, &MainWindow::onPreviewTimerTimeout);

    previewWatcher_ = new QFutureWatcher<std::vector<float>>(this);
    connect(previewWatcher_, &QFutureWatcher<std::vector<float>>::finished,
            this, &MainWindow::onPreviewComputationFinished);
    
    // Setup UI
    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    applyTheme();
    
    // Window properties
    setWindowTitle("Audio Editor");
    setMinimumSize(1000, 600);
    
    // Center on screen
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - 1200) / 2;
    int y = (screenGeometry.height() - 700) / 2;
    setGeometry(x, y, 1200, 700);
    
    // Enable drag and drop
    setAcceptDrops(true);
    
    // Initial UI state
    updateUIState();
    
    logger_->log("Application ready");
}

MainWindow::~MainWindow() {
    logger_->log("Application closing...");
    delete commandHistory_;
    delete captionParser_;
}

void MainWindow::setupUI() {
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget_);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Content area (waveform + side panels)
    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    // --- Waveform (center, expandable) ---
    waveformWidget_ = new WaveformWidget(this);
    
    // --- Right panel with effects and captions ---
    QWidget* rightPanel = new QWidget(this);
    rightPanel->setFixedWidth(300);
    rightPanel->setStyleSheet("background-color: #252525;");
    
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    // Effects panel
    effectsPanel_ = new EffectsPanel(logger_, this);
    
    // Caption panel
    captionPanel_ = new CaptionPanel(logger_, this);
    
    // Splitter for effects/captions
    QSplitter* rightSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter->addWidget(effectsPanel_);
    rightSplitter->addWidget(captionPanel_);
    rightSplitter->setSizes({300, 300});
    rightSplitter->setStyleSheet(R"(
        QSplitter::handle {
            background-color: #3d3d3d;
            height: 2px;
        }
    )");
    
    rightLayout->addWidget(rightSplitter);
    
    // Add to content layout
    contentLayout->addWidget(waveformWidget_, 1);  // Stretch
    contentLayout->addWidget(rightPanel);
    
    // --- Transport bar (bottom) ---
    transportBar_ = new TransportBar(this);
    
    // Assemble main layout
    mainLayout->addLayout(contentLayout, 1);  // Stretch
    mainLayout->addWidget(transportBar_);
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // --- File Menu ---
    fileMenu_ = menuBar->addMenu("&File");
    
    newAction_ = fileMenu_->addAction("&New Project");
    newAction_->setShortcut(QKeySequence::New);
    connect(newAction_, &QAction::triggered, this, &MainWindow::onNewProject);
    
    openAction_ = fileMenu_->addAction("&Open Audio...");
    openAction_->setShortcut(QKeySequence::Open);
    connect(openAction_, &QAction::triggered, this, &MainWindow::onOpenAudio);
    
    fileMenu_->addSeparator();
    
    saveAction_ = fileMenu_->addAction("&Save");
    saveAction_->setShortcut(QKeySequence::Save);
    saveAction_->setEnabled(false);
    connect(saveAction_, &QAction::triggered, this, &MainWindow::onSaveAudio);
    
    exportAction_ = fileMenu_->addAction("&Export As...");
    exportAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_E));
    exportAction_->setEnabled(false);
    connect(exportAction_, &QAction::triggered, this, &MainWindow::onExportAudio);
    
    fileMenu_->addSeparator();
    
    importCaptionsAction_ = fileMenu_->addAction("Import &Captions...");
    connect(importCaptionsAction_, &QAction::triggered, this, &MainWindow::onImportCaptions);
    
    exportCaptionsAction_ = fileMenu_->addAction("Export Captions...");
    exportCaptionsAction_->setEnabled(false);
    connect(exportCaptionsAction_, &QAction::triggered, this, &MainWindow::onExportCaptions);
    
    fileMenu_->addSeparator();
    
    exitAction_ = fileMenu_->addAction("E&xit");
    exitAction_->setShortcut(QKeySequence::Quit);
    connect(exitAction_, &QAction::triggered, this, &MainWindow::onExit);
    
    // --- Edit Menu ---
    editMenu_ = menuBar->addMenu("&Edit");
    
    undoAction_ = new QAction("Undo", this);
    undoAction_->setShortcut(QKeySequence::Undo);  // Ctrl+Z
    undoAction_->setEnabled(false);
    editMenu_->addAction(undoAction_);

    redoAction_ = new QAction("Redo", this);
    redoAction_->setShortcut(QKeySequence::Redo);  // Ctrl+Shift+Z or Ctrl+Y
    redoAction_->setEnabled(false);
    editMenu_->addAction(redoAction_);

    
    
    // --- Help Menu ---
    helpMenu_ = menuBar->addMenu("&Help");
    
    aboutAction_ = helpMenu_->addAction("&About");
    connect(aboutAction_, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About Audio Editor",
            "<h2>Audio Editor</h2>"
            "<p>Version 1.0</p>"
            "<p>A simple audio effects application.</p>"
            "<p>Supports MP3 and WAV files with effects like "
            "Reverb, Speed, and Volume.</p>"
        );
    });
    
    // Style menu bar
    menuBar->setStyleSheet(R"(
        QMenuBar {
            background-color: #1e1e1e;
            color: #e0e0e0;
            padding: 5px;
        }
        QMenuBar::item {
            padding: 5px 10px;
            border-radius: 4px;
        }
        QMenuBar::item:selected {
            background-color: #3d3d3d;
        }
        QMenu {
            background-color: #2d2d2d;
            color: #e0e0e0;
            border: 1px solid #3d3d3d;
            padding: 5px;
        }
        QMenu::item {
            padding: 8px 30px 8px 20px;
            border-radius: 4px;
        }
        QMenu::item:selected {
            background-color: #00bcd4;
            color: #000000;
        }
        QMenu::separator {
            height: 1px;
            background-color: #3d3d3d;
            margin: 5px 10px;
        }
    )");
}

void MainWindow::setupStatusBar() {
    QStatusBar* status = new QStatusBar(this);
    setStatusBar(status);
    
    status->setStyleSheet(R"(
        QStatusBar {
            background-color: #1e1e1e;
            color: #808080;
            border-top: 1px solid #3d3d3d;
            padding: 5px;
        }
    )");
    
    status->showMessage("Ready");
}

void MainWindow::setupConnections() {
    // --- TransportBar <-> AudioEngine ---
    connect(effectsPanel_, &EffectsPanel::applyRequested, this, &MainWindow::onApplyEffects);
    connect(transportBar_, &TransportBar::playClicked,
            audioEngine_, &AudioEngine::play);
    connect(transportBar_, &TransportBar::pauseClicked,
            audioEngine_, &AudioEngine::pause);
    connect(transportBar_, &TransportBar::stopClicked,
            audioEngine_, &AudioEngine::stop);
    connect(transportBar_, &TransportBar::seekRequested,
            audioEngine_, &AudioEngine::seek);
    connect(transportBar_, &TransportBar::volumeChanged,
            audioEngine_, &AudioEngine::setVolume);
    
    connect(audioEngine_, &AudioEngine::positionChanged,
            transportBar_, &TransportBar::setPosition);
    connect(audioEngine_, &AudioEngine::durationChanged,
            transportBar_, &TransportBar::setDuration);
    connect(audioEngine_, &AudioEngine::stateChanged,
            transportBar_, &TransportBar::setState);
    connect(audioEngine_, &AudioEngine::audioFinished,
            this, &MainWindow::onPlaybackFinished);
    
    // --- WaveformWidget <-> AudioEngine ---
    connect(waveformWidget_, &WaveformWidget::seekRequested,
            audioEngine_, &AudioEngine::seek);
    connect(audioEngine_, &AudioEngine::positionChanged,
            waveformWidget_, &WaveformWidget::setPlayheadPosition);
    
    // --- CaptionPanel <-> AudioEngine ---
    connect(captionPanel_, &CaptionPanel::seekRequested,
            audioEngine_, &AudioEngine::seek);
    connect(audioEngine_, &AudioEngine::positionChanged,
            captionPanel_, &CaptionPanel::setCurrentTime);
    
    // --- CaptionPanel buttons ---
    connect(captionPanel_, &CaptionPanel::importRequested,
            this, &MainWindow::onImportCaptions);
    connect(captionPanel_, &CaptionPanel::exportRequested,
            this, &MainWindow::onExportCaptions);
    
    // --- EffectsPanel ---
    connect(effectsPanel_, &EffectsPanel::effectsChanged,
        this, &MainWindow::updatePreview);

// Compare toggle - switch between original and effected audio
connect(effectsPanel_, &EffectsPanel::compareToggled,
        this, [this](bool enabled) {
            Q_UNUSED(enabled);
            onPreviewTimerTimeout();  // Immediately update (no debounce for toggle)
        });
}

void MainWindow::applyTheme() {
    // Global application style
    setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QWidget {
            background-color: #1e1e1e;
            color: #e0e0e0;
        }
        QToolTip {
            background-color: #2d2d2d;
            color: #e0e0e0;
            border: 1px solid #3d3d3d;
            padding: 5px;
            border-radius: 4px;
        }
    )");
}

void MainWindow::onNewProject() {
    if (!confirmUnsavedChanges()) {
        return;
    }

    cancelPendingPreview();
    
    // Stop playback
    audioEngine_->stop();
    
    // Clear everything
    audioClip_.reset();
    audioEngine_->setAudioClip(nullptr);
    waveformWidget_->clear();
    effectsPanel_->clearEffects();
    captionPanel_->clearCaptions();
    commandHistory_->clear();
    
    currentFilePath_.clear();
    hasUnsavedChanges_ = false;
    
    updateUIState();
    updateWindowTitle();
    
    statusBar()->showMessage("New project created", 3000);
    logger_->log("New project created");
}

void MainWindow::onOpenAudio() {
    if (!confirmUnsavedChanges()) {
        return;
    }
    
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Audio File",
        QString(),
        "Audio Files (*.mp3 *.wav);;MP3 Files (*.mp3);;WAV Files (*.wav);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        loadAudioFile(filePath);
    }
}

void MainWindow::loadAudioFile(const QString& filePath) {
    logger_->log("Loading audio file: " + filePath.toStdString());
    
    cancelPendingPreview();
    
    // Stop current playback
    audioEngine_->stop();
    
    // Show loading status
    statusBar()->showMessage("Loading " + filePath + "...");
    QApplication::processEvents();
    
    // Create and load audio clip
    audioClip_ = std::make_shared<AudioClip>(filePath.toStdString(), logger_);
    
    if (!audioClip_->load()) {
        QMessageBox::critical(this, "Error", 
            "Failed to load audio file:\n" + filePath);
        audioClip_.reset();
        statusBar()->showMessage("Failed to load file", 3000);
        return;
    }
    
    // Set audio in engine
    audioEngine_->setAudioClip(audioClip_);
    
    // Update waveform
    waveformWidget_->setSamples(
        audioClip_->getSamples(),
        44100,  // Default sample rate
        2       // Default stereo
    );
    
    // Clear effects and captions
    effectsPanel_->clearEffects();
    captionPanel_->clearCaptions();
    commandHistory_->clear();
    
    // Update state
    currentFilePath_ = filePath;
    hasUnsavedChanges_ = false;
    
    updateUIState();
    updateWindowTitle();
    onAudioLoaded();
    
    statusBar()->showMessage("Loaded: " + filePath, 5000);
    logger_->log("Audio loaded successfully: " + filePath.toStdString());
}

void MainWindow::onSaveAudio() {
    if (!audioClip_) {
        return;
    }
    
    QString savePath = currentFilePath_;
    
    // If original was loaded, ask for new name to avoid overwriting
    if (!savePath.isEmpty()) {
        QFileInfo fileInfo(savePath);
        QString defaultName = fileInfo.baseName() + "_edited." + fileInfo.suffix();
        QString defaultPath = fileInfo.absolutePath() + "/" + defaultName;
        
        savePath = QFileDialog::getSaveFileName(
            this,
            "Save Audio File",
            defaultPath,
            "MP3 Files (*.mp3);;WAV Files (*.wav)"
        );
    } else {
        savePath = QFileDialog::getSaveFileName(
            this,
            "Save Audio File",
            "output.mp3",
            "MP3 Files (*.mp3);;WAV Files (*.wav)"
        );
    }
    
    ApplyEffectsForExport();

    if (savePath.isEmpty()) {
        return;
    }
    
    statusBar()->showMessage("Saving...");
    QApplication::processEvents();
    
    if (audioClip_->save(savePath.toStdString())) {
        hasUnsavedChanges_ = false;
        updateWindowTitle();
        statusBar()->showMessage("Saved: " + savePath, 5000);
        logger_->log("Audio saved: " + savePath.toStdString());
    } else {
        QMessageBox::critical(this, "Error", "Failed to save audio file.");
        statusBar()->showMessage("Save failed", 3000);
    }
}

void MainWindow::onExportAudio() {
    if (!audioClip_) {
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Audio",
        "exported_audio.mp3",
        "MP3 Files (*.mp3);;WAV Files (*.wav)"
    );
    
    ApplyEffectsForExport();

    if (filePath.isEmpty()) {
        return;
    }
    
    statusBar()->showMessage("Exporting...");
    QApplication::processEvents();
    
    if (audioClip_->save(filePath.toStdString())) {
        statusBar()->showMessage("Exported: " + filePath, 5000);
        logger_->log("Audio exported: " + filePath.toStdString());
        
        QMessageBox::information(this, "Export Complete",
            "Audio exported successfully to:\n" + filePath);
    } else {
        QMessageBox::critical(this, "Error", "Failed to export audio file.");
        statusBar()->showMessage("Export failed", 3000);
    }
}

void MainWindow::ApplyEffectsForExport() {
    if (!audioClip_) {
        return;
    }

    auto exportEffects = effectsPanel_->getEffectsForExport();

    if (exportEffects.empty() || !effectsPanel_->areEffectsEnabled()) {
        audioClip_->clearEffects();
        audioEngine_->setAudioClip(audioClip_);
        waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);
        logger_->log("No export effects applied (effects disabled or none configured).");
        return;
    }

    audioClip_->clearEffects();
    for (const auto& effect : exportEffects) {
        audioClip_->addEffect(effect);
    }

    audioClip_->applyEffects();

    audioEngine_->setAudioClip(audioClip_);
    waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);

    logger_->log("Applied " + std::to_string(exportEffects.size()) + " effect(s) for export.");
}

void MainWindow::onExit() {
    close();
}

void MainWindow::onUndo()
{
    if (commandHistory_->canUndo()) {
        commandHistory_->undo();

        audioEngine_->revertToOriginal();
        waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);
        isPreviewMode_ = false;

        hasUnsavedChanges_ = commandHistory_->canUndo();
        undoAction_->setEnabled(commandHistory_->canUndo());
        redoAction_->setEnabled(commandHistory_->canRedo());

        updateWindowTitle();
        statusBar()->showMessage("Undo", 1500);
    }
}

void MainWindow::onRedo()
{
    if (commandHistory_->canRedo()) {
        commandHistory_->redo();

        audioEngine_->revertToOriginal();
        waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);
        isPreviewMode_ = false;

        hasUnsavedChanges_ = commandHistory_->canUndo();  // still unsaved if we can undo
        undoAction_->setEnabled(commandHistory_->canUndo());
        redoAction_->setEnabled(commandHistory_->canRedo());

        updateWindowTitle();
        statusBar()->showMessage("Redo", 1500);
    }
}

void MainWindow::onImportCaptions() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Import Captions",
        QString(),
        "SubRip Files (*.srt);;All Files (*)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    std::vector<Caption> captions = captionParser_->parseSRT(filePath);
    
    if (captions.empty()) {
        QMessageBox::warning(this, "Import Failed",
            "No captions found in the file.\n"
            "Please make sure it's a valid SRT file.");
        return;
    }
    
    captionPanel_->setCaptions(captions);
    exportCaptionsAction_->setEnabled(true);
    
    statusBar()->showMessage("Imported " + QString::number(captions.size()) + " captions", 3000);
}

void MainWindow::onExportCaptions() {
    std::vector<Caption> captions = captionPanel_->getCaptions();
    
    if (captions.empty()) {
        QMessageBox::warning(this, "No Captions",
            "There are no captions to export.");
        return;
    }
    
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Captions",
        "captions.txt",
        "Text Files (*.txt);;SubRip Files (*.srt)"
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    bool success = false;
    
    if (filePath.endsWith(".srt", Qt::CaseInsensitive)) {
        success = captionParser_->exportSRT(filePath, captions);
    } else {
        success = captionParser_->exportTXT(filePath, captions);
    }
    
    if (success) {
        statusBar()->showMessage("Captions exported: " + filePath, 3000);
        QMessageBox::information(this, "Export Complete",
            "Captions exported successfully.");
    } else {
        QMessageBox::critical(this, "Export Failed",
            "Failed to export captions.");
    }
}

void MainWindow::onPlaybackFinished() {
    statusBar()->showMessage("Playback finished", 2000);
}

void MainWindow::onAudioLoaded() {
    statusBar()->showMessage("Ready", 2000);
}

void MainWindow::updateUIState() {
    bool hasAudio = (audioClip_ != nullptr);
    
    // Menu actions
    saveAction_->setEnabled(hasAudio);
    exportAction_->setEnabled(hasAudio);
    
    // Undo/Redo
    undoAction_->setEnabled(commandHistory_->canUndo());
    redoAction_->setEnabled(commandHistory_->canRedo());

    
    // Panels
    effectsPanel_->setEnabled(hasAudio);
}

void MainWindow::updateWindowTitle() {
    QString title = "Audio Editor";
    
    if (!currentFilePath_.isEmpty()) {
        QFileInfo fileInfo(currentFilePath_);
        title += " - " + fileInfo.fileName();
    }
    
    if (hasUnsavedChanges_) {
        title += " *";
    }
    
    setWindowTitle(title);
}

bool MainWindow::confirmUnsavedChanges() {
    if (!hasUnsavedChanges_) {
        return true;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Unsaved Changes",
        "You have unsaved changes. Do you want to continue and discard them?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    return (reply == QMessageBox::Yes);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (confirmUnsavedChanges()) {
        audioEngine_->stop();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (filePath.endsWith(".mp3", Qt::CaseInsensitive) ||
                filePath.endsWith(".wav", Qt::CaseInsensitive) ||
                filePath.endsWith(".srt", Qt::CaseInsensitive)) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void MainWindow::dropEvent(QDropEvent* event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }
    
    QString filePath = urls.first().toLocalFile();
    
    if (filePath.endsWith(".srt", Qt::CaseInsensitive)) {
        // Import captions
        std::vector<Caption> captions = captionParser_->parseSRT(filePath);
        if (!captions.empty()) {
            captionPanel_->setCaptions(captions);
            exportCaptionsAction_->setEnabled(true);
            statusBar()->showMessage("Imported " + QString::number(captions.size()) + " captions", 3000);
        }
    } else if (filePath.endsWith(".mp3", Qt::CaseInsensitive) ||
               filePath.endsWith(".wav", Qt::CaseInsensitive)) {
        // Load audio
        if (confirmUnsavedChanges()) {
            loadAudioFile(filePath);
        }
    }
}

void MainWindow::onRefreshAudioDevice() {
    if (audioClip_) {
        qint64 pos = audioEngine_->getPositionMs();
        bool wasPlaying = (audioEngine_->getState() == PlaybackState::Playing);
        
        audioEngine_->stop();
        audioEngine_->setAudioClip(audioClip_);  // This recreates the audio sink
        audioEngine_->seek(pos);
        
        if (wasPlaying) {
            audioEngine_->play();
        }
        
        statusBar()->showMessage("Audio device refreshed", 2000);
    }
}

void MainWindow::updatePreview() {
    // Just restart the timer - actual preview happens when timer fires
    // This prevents lag when dragging sliders
    previewDebounceTimer_->start();
}

void MainWindow::onPreviewTimerTimeout() {
    // Safety check - must have audio loaded
    if (!audioClip_ || audioClip_->getSamples().empty()) {
        return;
    }
    
    auto effects = effectsPanel_->getEffects();
    startPreviewComputation(effects);
}

void MainWindow::startPreviewComputation(const std::vector<std::shared_ptr<IEffect>>& effects) {
    if (!audioClip_) {
        return;
    }

    if (effects.empty()) {
        cancelPendingPreview();
        audioEngine_->revertToOriginal();
        waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);
        isPreviewMode_ = false;
        statusBar()->showMessage("Original audio", 1500);
        return;
    }

    if (previewWatcher_->isRunning()) {
        queuedEffects_ = effects;
        previewComputationQueued_ = true;
        return;
    }

    discardPreviewResult_ = false;
    statusBar()->showMessage("Rendering preview...");

    auto baseSamples = audioClip_->getSamples();
    std::vector<std::shared_ptr<IEffect>> effectCopies = effects;

    auto future = QtConcurrent::run([baseSamples = std::move(baseSamples), effectCopies]() mutable {
        auto processed = baseSamples;
        for (const auto& effect : effectCopies) {
            if (!effect) {
                continue;
            }

        if (auto* reverb = dynamic_cast<Reverb*>(effect.get())) {
                reverb->reset();
            }

            if (auto* speed = dynamic_cast<SpeedChangeEffect*>(effect.get())) {
                float factor = speed->getSpeedFactor();
                size_t newSize = static_cast<size_t>(processed.size() / factor);
                if (newSize > processed.size()) {
                    processed.resize(newSize);
                }
            }

            size_t newSize = effect->apply(processed.data(), processed.size());
            processed.resize(newSize);
        }

        return processed;
    });

    previewWatcher_->setFuture(future);
}

void MainWindow::onPreviewComputationFinished() {
    if (!previewWatcher_) {
        return;
    }

    if (discardPreviewResult_) {
        discardPreviewResult_ = false;
    } else {
        std::vector<float> processed = previewWatcher_->result();
        audioEngine_->previewWithSamples(processed);
        waveformWidget_->setSamples(processed, 44100, 2);
        isPreviewMode_ = true;
        statusBar()->showMessage("Preview ready", 1000);
    }

    if (previewComputationQueued_ && !queuedEffects_.empty()) {
        previewComputationQueued_ = false;
        auto pending = queuedEffects_;
        queuedEffects_.clear();
        startPreviewComputation(pending);
    } else {
        queuedEffects_.clear();
    }
}

void MainWindow::cancelPendingPreview() {
    if (!previewWatcher_) {
        return;
    }

    if (previewWatcher_->isRunning()) {
        discardPreviewResult_ = true;
        previewWatcher_->cancel();
    }

    previewComputationQueued_ = false;
    queuedEffects_.clear();
}

void MainWindow::onApplyEffects()
{
    if (!audioClip_ || audioClip_->getSamples().empty()) {
        statusBar()->showMessage("No audio loaded", 2000);
        return;
    }

    auto effects = effectsPanel_->getEffectsForExport();
    if (effects.empty()) {
        statusBar()->showMessage("No active effects to apply", 2000);
        return;
    }

    // Create the command
    auto command = std::make_shared<ApplyEffectCommand>(audioClip_, effects, logger_);

    // CORRECT METHOD (from your CommandHistory.h):
    commandHistory_->executeCommand(command);

    // Clear effects panel
    effectsPanel_->clearEffects();

    // Update preview/audio
    audioEngine_->revertToOriginal();
    waveformWidget_->setSamples(audioClip_->getSamples(), 44100, 2);
    isPreviewMode_ = false;

    // Unsaved changes = we have something to undo
    hasUnsavedChanges_ = commandHistory_->canUndo();

    updateUIState();
    updateWindowTitle();
    statusBar()->showMessage("Effects applied permanently", 3000);
}