#pragma once

#include <vector>
#include <string>


class AudioFileAdapter {
public:
    virtual ~AudioFileAdapter() = default;

    [[nodiscard]] virtual bool load(const std::string& filePath) = 0;
 
    [[nodiscard]] virtual bool save(const std::string& filePath, 
                                    const std::vector<float>& samples) = 0;

    [[nodiscard]] virtual const std::vector<float>& getSamples() const = 0;

    [[nodiscard]] virtual float getDuration() const noexcept = 0;

    [[nodiscard]] virtual int getSampleRate() const noexcept = 0;

    [[nodiscard]] virtual int getChannels() const noexcept = 0;
};
