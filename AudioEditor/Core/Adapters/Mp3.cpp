#include "Mp3.h"
#include <mpg123.h>
#include <lame/lame.h>
#include <cstring>

// ADD CONSTRUCTOR
Mp3Adapter::Mp3Adapter(std::shared_ptr<ILogger> logger) 
    : logger_(logger) {
}

bool Mp3Adapter::load(const std::string& filePath) {
    logger_->log("Loading MP3 file from: " + filePath);
    
    mpg123_handle* mh = mpg123_new(NULL, NULL);
    if (!mh) {
        logger_->log("Failed to create mpg123 handle");
        return false;
    }
    
    if (mpg123_open(mh, filePath.c_str()) != MPG123_OK) {
        logger_->log("Failed to open MP3 file: " + filePath);
        mpg123_delete(mh);
        return false;
    }
    
    int encoding;
    if (mpg123_getformat(mh, &sampleRate, &channels, &encoding) != MPG123_OK) {
        logger_->log("Failed to get MP3 format");
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }
    
    off_t length = mpg123_length(mh);
    if (length == MPG123_ERR) {
        logger_->log("Failed to get MP3 length");
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }
    
    duration = static_cast<float>(length) / sampleRate;
    
    samples.clear();
    unsigned char* buffer = new unsigned char[4096];
    size_t done;
    int err;
    
    while ((err = mpg123_read(mh, buffer, 4096, &done)) == MPG123_OK) {
        for (size_t i = 0; i < done; i += 2) {
            if (i + 1 < done) {
                short sample = (buffer[i + 1] << 8) | buffer[i];
                samples.push_back(static_cast<float>(sample) / 32768.0f);
            }
        }
    }
    
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);
    
    logger_->log("Loaded MP3: " + filePath + " (" + std::to_string(samples.size()) + " samples, " + std::to_string(duration) + "s, " + std::to_string(channels) + " channels)");
    return true;
}

std::vector<float> Mp3Adapter::getSamples() const { return samples; }
float Mp3Adapter::getDuration() const { return duration; }

bool Mp3Adapter::save(const std::string& filePath, const std::vector<float>& processedSamples) {
    logger_->log("Saving MP3: " + filePath);
    
    // Log sample statistics
    float maxSample = 0.0f;
    double sumSquares = 0.0;
    for (const auto& sample : processedSamples) {
        maxSample = std::max(maxSample, std::abs(sample));
        sumSquares += sample * sample;
    }
    double rms = std::sqrt(sumSquares / processedSamples.size());
    logger_->log("Saving samples - Max sample: " + std::to_string(maxSample) + ", RMS: " + std::to_string(rms));
    
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, sampleRate);
    lame_set_num_channels(lame, channels);
    lame_set_quality(lame, 2);
    if (lame_init_params(lame) < 0) {
        logger_->log("Failed to initialize LAME");
        return false;
    }

    FILE* file = fopen(filePath.c_str(), "wb");
    if (!file) {
        logger_->log("Failed to open output file: " + filePath);
        lame_close(lame);
        return false;
    }

    std::vector<short> intSamples(processedSamples.size());
    for (size_t i = 0; i < processedSamples.size(); ++i) {
        intSamples[i] = static_cast<short>(processedSamples[i] * 32767.0f);
    }

    int mp3Size;
    std::vector<unsigned char> mp3Buffer(7200 + processedSamples.size() / channels * 1.25);
    if (channels == 2) {
        mp3Size = lame_encode_buffer_interleaved(lame, intSamples.data(),
                                                processedSamples.size() / channels, mp3Buffer.data(), mp3Buffer.size());
    } else {
        mp3Size = lame_encode_buffer(lame, intSamples.data(), nullptr,
                                     processedSamples.size(), mp3Buffer.data(), mp3Buffer.size());
    }
    if (mp3Size < 0) {
        logger_->log("LAME encoding failed");
        fclose(file);
        lame_close(lame);
        return false;
    }
    fwrite(mp3Buffer.data(), 1, mp3Size, file);

    mp3Size = lame_encode_flush(lame, mp3Buffer.data(), mp3Buffer.size());
    fwrite(mp3Buffer.data(), 1, mp3Size, file);

    fclose(file);
    lame_close(lame);
    logger_->log("Successfully saved MP3: " + filePath);
    return true;
}