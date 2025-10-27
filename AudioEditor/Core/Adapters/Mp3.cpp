#include "Mp3.h"
#include "../Logger.h"
#include <mpg123.h>
#include <lame/lame.h>
#include <vector>
#include <stdexcept>
#include <cstdio> // For FILE, fopen, fclose, fwrite

Mp3Adapter::Mp3Adapter() {
    if (mpg123_init() != MPG123_OK) {
        Logger::getInstance().log("Error: Failed to initialize MPG123");
        throw std::runtime_error("MPG123 initialization failed");
    }
}

Mp3Adapter::~Mp3Adapter() {
    mpg123_exit();
}

bool Mp3Adapter::load(const std::string& filePath) {
    Logger::getInstance().log("Loading MP3 file: " + filePath);
    mpg123_handle* mh = mpg123_new(NULL, NULL);
    if (!mh || mpg123_open(mh, filePath.c_str()) != MPG123_OK) {
        Logger::getInstance().log("Error: Failed to open MP3 file: " + filePath);
        if (mh) mpg123_delete(mh);
        return false;
    }

    long rate;
    int ch, encoding;
    if (mpg123_getformat(mh, &rate, &ch, &encoding) != MPG123_OK) {
        Logger::getInstance().log("Error: Failed to get MP3 format: " + filePath);
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }

    if (encoding != MPG123_ENC_SIGNED_16) {
        Logger::getInstance().log("Error: Unsupported MP3 encoding: " + filePath);
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }

    inputRate = rate; // Store the detected rate
    channels = ch;    // Store the detected channel count

    size_t buffer_size = mpg123_outblock(mh);
    std::vector<unsigned char> buffer(buffer_size);
    std::vector<float> samples;
    size_t done;
    int err;

    while ((err = mpg123_read(mh, buffer.data(), buffer_size, &done)) == MPG123_OK || err == MPG123_DONE) {
        if (done <= 0) break;
        for (size_t i = 0; i < done; i += 2 * channels) {
            short sample = (buffer[i + 1] << 8) | buffer[i];
            samples.push_back(static_cast<float>(sample) / 32767.0f);
            if (channels == 2 && i + 2 < done) {
                short right = (buffer[i + 3] << 8) | buffer[i + 2];
                samples.push_back(static_cast<float>(right) / 32767.0f);
            }
        }
        if (err == MPG123_DONE) break;
    }

    mpg123_close(mh);
    mpg123_delete(mh);

    if (samples.empty()) {
        Logger::getInstance().log("Error: No samples decoded from: " + filePath);
        return false;
    }

    samples_ = std::move(samples);
    duration_ = static_cast<float>(samples_.size()) / (inputRate * channels);
    Logger::getInstance().log("Loaded " + std::to_string(samples_.size()) + " samples from " + filePath);
    return true;
}

bool Mp3Adapter::save(const std::string& filePath, const std::vector<float, std::allocator<float>>& processedSamples) {
    Logger::getInstance().log("Saving MP3 file: " + filePath);
    if (processedSamples.empty()) {
        Logger::getInstance().log("Error: Empty samples vector in save");
        return false;
    }

    size_t numSamples = processedSamples.size();
    if (numSamples % channels != 0) {
        Logger::getInstance().log("Error: Sample count not divisible by channel count: " + std::to_string(numSamples) + " / " + std::to_string(channels));
        return false;
    }

    // Convert float samples to 16-bit PCM
    std::vector<short> pcmData(numSamples);
    for (size_t i = 0; i < numSamples; ++i) {
        float sample = std::clamp(processedSamples[i], -1.0f, 1.0f);
        pcmData[i] = static_cast<short>(sample * 32767.0f);
    }

    lame_t lame = lame_init();
    if (!lame) {
        Logger::getInstance().log("Error: LAME init failed");
        return false;
    }
    
    lame_set_in_samplerate(lame, inputRate);
    lame_set_out_samplerate(lame, inputRate);
    lame_set_num_channels(lame, channels);
    lame_set_quality(lame, 2); // Higher quality (0-9, lower is better)
    
    if (lame_init_params(lame) < 0) {
        Logger::getInstance().log("Error: LAME initialization failed");
        lame_close(lame);
        return false;
    }

    FILE* fp = fopen(filePath.c_str(), "wb");
    if (!fp) {
        Logger::getInstance().log("Error: Could not open output file: " + filePath);
        lame_close(lame);
        return false;
    }

    // Calculate number of frames (for stereo, this is numSamples / 2)
    int numFrames = static_cast<int>(numSamples / channels);
    
    // Encode in chunks to avoid buffer overflow
    const int CHUNK_SIZE = 4096; // Process 4096 frames at a time
    unsigned char mp3buffer[16384];
    size_t pcmOffset = 0;
    
    Logger::getInstance().log("Encoding " + std::to_string(numFrames) + " frames (" + 
                             std::to_string(numSamples) + " samples) to MP3");

    while (pcmOffset < numSamples) {
        int framesToEncode = std::min(CHUNK_SIZE, static_cast<int>((numSamples - pcmOffset) / channels));
        
        int mp3bytes;
        if (channels == 2) {
            mp3bytes = lame_encode_buffer_interleaved(
                lame,
                pcmData.data() + pcmOffset,
                framesToEncode,
                mp3buffer,
                sizeof(mp3buffer)
            );
        } else {
            // Mono encoding
            mp3bytes = lame_encode_buffer(
                lame,
                pcmData.data() + pcmOffset,
                nullptr,
                framesToEncode,
                mp3buffer,
                sizeof(mp3buffer)
            );
        }
        
        if (mp3bytes < 0) {
            Logger::getInstance().log("Error: LAME encoding failed at offset " + 
                                    std::to_string(pcmOffset) + 
                                    " (return code: " + std::to_string(mp3bytes) + ")");
            fclose(fp);
            lame_close(lame);
            return false;
        }
        
        if (mp3bytes > 0) {
            fwrite(mp3buffer, 1, mp3bytes, fp);
        }
        
        pcmOffset += framesToEncode * channels;
    }

    // Flush remaining data
    int mp3bytes = lame_encode_flush(lame, mp3buffer, sizeof(mp3buffer));
    if (mp3bytes > 0) {
        fwrite(mp3buffer, 1, mp3bytes, fp);
    }

    fclose(fp);
    lame_close(lame);
    
    Logger::getInstance().log("Saved " + std::to_string(numSamples) + " samples to " + filePath);
    return true;
}

std::vector<float> Mp3Adapter::getSamples() const {
    return samples_;
}

float Mp3Adapter::getDuration() const {
    return duration_;
}