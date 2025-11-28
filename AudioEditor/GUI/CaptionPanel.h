#ifndef CAPTION_PANEL_H
#define CAPTION_PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include <memory>

// Forward declaration
class ILogger;

/**
 * @brief Struct representing a single caption/subtitle
 */
struct Caption {
    double startTime;   // Start time in seconds
    double endTime;     // End time in seconds
    QString text;       // Caption text
};

/**
 * @brief Panel for displaying and managing captions/subtitles
 * 
 * Features:
 * - Import SRT files
 * - Display captions in a table
 * - Highlight current caption during playback
 * - Click caption to seek
 * - Export captions to TXT
 * 
 * Design pattern: Observer (Qt signals/slots)
 */
class CaptionPanel : public QWidget {
    Q_OBJECT

public:
    explicit CaptionPanel(std::shared_ptr<ILogger> logger, QWidget* parent = nullptr);
    ~CaptionPanel() = default;

    // Set captions directly
    void setCaptions(const std::vector<Caption>& captions);
    
    // Get current captions
    std::vector<Caption> getCaptions() const;
    
    // Clear all captions
    void clearCaptions();
    
    // Update highlight based on current playback time (in milliseconds)
    void setCurrentTime(qint64 positionMs);
    
    // Enable/disable the panel
    void setEnabled(bool enabled);
    void setSpeedFactor(float factor) { speedFactor_ = factor; }

signals:
    // Emitted when user clicks Import button
    void importRequested();
    
    // Emitted when user clicks Export button
    void exportRequested();
    
    // Emitted when user clicks a caption row (time in ms)
    void seekRequested(qint64 positionMs);

private slots:
    void onImportClicked();
    void onExportClicked();
    void onCaptionClicked(int row, int column);
    void onCaptionDoubleClicked(int row, int column);

private:
    void setupUI();
    void updateTable();
    void highlightRow(int row);
    QString formatTime(double seconds);

    std::shared_ptr<ILogger> logger_;
    
    // Captions data
    std::vector<Caption> captions_;
    int currentHighlightedRow_;
    float speedFactor_ = 1.0f;  // Add this line
    
    // UI Components
    QVBoxLayout* mainLayout_;
    QLabel* headerLabel_;
    QTableWidget* captionTable_;
    QPushButton* importButton_;
    QPushButton* exportButton_;
    QLabel* noCaptionsLabel_;
    QLabel* currentCaptionLabel_;  // Shows current caption text prominently
};

#endif // CAPTION_PANEL_H