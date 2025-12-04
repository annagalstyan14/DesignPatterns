#ifndef CAPTION_PARSER_H
#define CAPTION_PARSER_H

#include <QString>
#include <vector>
#include <memory>

class ILogger;
struct Caption;

class CaptionParser {
public:
    explicit CaptionParser(std::shared_ptr<ILogger> logger);
    ~CaptionParser() = default;

    // Parse SRT file and return captions
    std::vector<Caption> parseSRT(const QString& filePath);
    
    // Export captions to plain text file (just the text, one per line)
    bool exportTXT(const QString& filePath, const std::vector<Caption>& captions);
    
    // Export captions back to SRT format
    bool exportSRT(const QString& filePath, const std::vector<Caption>& captions);

private:
    // Parse SRT timestamp format: "00:00:01,234" -> seconds
    double parseTimestamp(const QString& timestamp);
    
    // Format seconds to SRT timestamp: seconds -> "00:00:01,234"
    QString formatTimestamp(double seconds);

    std::shared_ptr<ILogger> logger_;
};

#endif // CAPTION_PARSER_H