#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <QObject>
#include <QAudioSink>
#include <QAudioFormat>
#include <QBuffer>
#include <QTimer>
#include <QByteArray>
#include <memory>
#include <vector>

// Forward declaration
class AudioClip;

/**
 * @brief Playback state enumeration
 */
enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

/**
 * @brief Audio playback engine using Qt6 QAudioSink
 * 
 * This class handles real-time audio playback with play/pause/stop/seek
 * functionality. It emits signals for UI synchronization.
 * 
 * Design patterns used:
 * - Observer: Qt signals notify UI of state changes
 * - Dependency Injection: AudioClip is set externally
 */
class AudioEngine : public QObject {
    Q_OBJECT

public:
    explicit AudioEngine(QObject* parent = nullptr);
    ~AudioEngine();

    // Clip management
    void setAudioClip(std::shared_ptr<AudioClip> clip);
    std::shared_ptr<AudioClip> getAudioClip() const;

    // Playback control
    void play();
    void pause();
    void stop();
    void seek(qint64 positionMs);

    // Getters
    PlaybackState getState() const;
    qint64 getPositionMs() const;
    qint64 getDurationMs() const;
    float getVolume() const;

    // Setters
    void setVolume(float volume);  // 0.0 to 1.0

signals:
    void positionChanged(qint64 positionMs);
    void stateChanged(PlaybackState state);
    void durationChanged(qint64 durationMs);
    void audioFinished();

private slots:
    void onAudioStateChanged(QAudio::State state);
    void updatePosition();

private:
    void setupAudio();
    void convertSamplesToBytes();
    void applyVolume(QByteArray& buffer);

    std::shared_ptr<AudioClip> audioClip_;
    
    QAudioSink* audioSink_;
    QBuffer* audioBuffer_;
    QByteArray audioData_;
    QTimer* positionTimer_;
    
    PlaybackState state_;
    float volume_;
    int sampleRate_;
    int channels_;
    qint64 pausedPosition_;  // Bytes position when paused
};

#endif // AUDIO_ENGINE_H