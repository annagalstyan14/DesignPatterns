#include "Mp3.h"
#include "Logger.h"
#include <mpg123.h>
#include <cstring>

bool Mp3Adapter::load(const std::string& filePath) {
    Logger::getInstance().log("Loading MP3 file from: " + filePath);
    
    mpg123_handle* mh = mpg123_new(NULL, NULL);
    if (!mh) {
        Logger::getInstance().log("Failed to create mpg123 handle");
        return false;
    }
    
    if (mpg123_open(mh, filePath.c_str()) != MPG123_OK) {
        Logger::getInstance().log("Failed to open MP3 file: " + filePath);
        mpg123_delete(mh);
        return false;
    }
    
    // Get format information
    long rate;
    int channels, encoding;
    if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK) {
        Logger::getInstance().log("Failed to get MP3 format");
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }
    
    // Calculate duration
    off_t length = mpg123_length(mh);
    if (length == MPG123_ERR) {
        Logger::getInstance().log("Failed to get MP3 length");
        mpg123_close(mh);
        mpg123_delete(mh);
        return false;
    }
    
    duration = static_cast<float>(length) / rate;
    
    // Read all audio data
    samples.clear();
    unsigned char* buffer = new unsigned char[4096];
    size_t done;
    int err;
    
    while ((err = mpg123_read(mh, buffer, 4096, &done)) == MPG123_OK) {
        // Convert bytes to float samples
        for (size_t i = 0; i < done; i += 2) { // Assuming 16-bit samples
            if (i + 1 < done) {
                short sample = (buffer[i + 1] << 8) | buffer[i];
                samples.push_back(static_cast<float>(sample) / 32768.0f);
            }
        }
    }
    
    delete[] buffer;
    mpg123_close(mh);
    mpg123_delete(mh);
    
    Logger::getInstance().log("Loaded MP3: " + filePath + " (" + std::to_string(samples.size()) + " samples, " + std::to_string(duration) + "s)");
    return true;
}

std::vector<float> Mp3Adapter::getSamples() const { return samples; }
float Mp3Adapter::getDuration() const { return duration; }
bool Mp3Adapter::save(const std::string& filePath) { 
    Logger::getInstance().log("Saving MP3: " + filePath);
    return true; // Placeholder
}