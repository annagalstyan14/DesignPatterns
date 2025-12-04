#ifndef WAVEFORM_WIDGET_H
#define WAVEFORM_WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <vector>

class WaveformWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaveformWidget(QWidget* parent = nullptr);
    ~WaveformWidget() = default;

    // Set audio data - call this when loading a file
    void setSamples(const std::vector<float>& samples, int sampleRate, int channels);
    
    // Clear the waveform
    void clear();

    // Playhead position (in milliseconds)
    void setPlayheadPosition(qint64 positionMs);
    
    // Zoom control (1.0 = fit all, higher = zoomed in)
    void setZoom(float zoom);
    float getZoom() const;
    
    // Get duration
    qint64 getDurationMs() const;

signals:
    // Emitted when user clicks to seek
    void seekRequested(qint64 positionMs);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void computePeaks();
    void renderWaveform();
    int positionToX(qint64 positionMs) const;
    qint64 xToPosition(int x) const;

    // Audio data
    std::vector<float> samples_;
    int sampleRate_;
    int channels_;
    qint64 durationMs_;

    // Computed peaks for drawing (min/max per pixel column)
    struct Peak {
        float min;
        float max;
    };
    std::vector<Peak> peaks_;

    // Cached waveform image
    QPixmap waveformCache_;
    bool cacheValid_;
    float displayScale_ = 1.0f;

    // View state
    float zoom_;           // 1.0 = fit all, 2.0 = 2x zoom, etc.
    qint64 scrollOffsetMs_; // Horizontal scroll position

    // Playhead
    qint64 playheadPositionMs_;

    // Mouse interaction
    bool isDragging_;
    int lastMouseX_;

    // Colors
    QColor backgroundColor_;
    QColor waveformColor_;
    QColor waveformPeakColor_;
    QColor playheadColor_;
    QColor centerLineColor_;
};

#endif // WAVEFORM_WIDGET_H