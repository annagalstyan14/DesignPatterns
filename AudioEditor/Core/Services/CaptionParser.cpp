#include "CaptionParser.h"
#include "CaptionPanel.h"  // For Caption struct
#include "../Core/ILogger.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

CaptionParser::CaptionParser(std::shared_ptr<ILogger> logger)
    : logger_(logger)
{
}

std::vector<Caption> CaptionParser::parseSRT(const QString& filePath) {
    std::vector<Caption> captions;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (logger_) {
            logger_->error("Failed to open SRT file: " + filePath.toStdString());
        }
        return captions;
    }
    
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    // SRT format:
    // 1
    // 00:00:01,234 --> 00:00:04,567
    // Caption text line 1
    // Caption text line 2 (optional)
    // 
    // 2
    // ...
    
    enum class State { Index, Timestamp, Text };
    State state = State::Index;
    
    Caption currentCaption;
    QStringList textLines;
    
    // Regex for timestamp line
    QRegularExpression timestampRegex(
        R"((\d{2}:\d{2}:\d{2}[,\.]\d{3})\s*-->\s*(\d{2}:\d{2}:\d{2}[,\.]\d{3}))"
    );
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        switch (state) {
            case State::Index:
                // Skip index number, look for timestamp
                if (line.isEmpty()) {
                    continue;
                }
                // Check if this line is actually a timestamp (some SRT files skip index)
                if (timestampRegex.match(line).hasMatch()) {
                    // This is a timestamp line, process it
                    auto match = timestampRegex.match(line);
                    currentCaption.startTime = parseTimestamp(match.captured(1));
                    currentCaption.endTime = parseTimestamp(match.captured(2));
                    textLines.clear();
                    state = State::Text;
                } else {
                    // Assume it's an index, next line should be timestamp
                    state = State::Timestamp;
                }
                break;
                
            case State::Timestamp: {
                auto match = timestampRegex.match(line);
                if (match.hasMatch()) {
                    currentCaption.startTime = parseTimestamp(match.captured(1));
                    currentCaption.endTime = parseTimestamp(match.captured(2));
                    textLines.clear();
                    state = State::Text;
                } else {
                    // Invalid format, try to recover
                    if (logger_) {
                        logger_->warning("Invalid SRT timestamp: " + line.toStdString());
                    }
                    state = State::Index;
                }
                break;
            }
                
            case State::Text:
                if (line.isEmpty()) {
                    // End of this caption
                    if (!textLines.isEmpty()) {
                        currentCaption.text = textLines.join(" ");
                        captions.push_back(currentCaption);
                    }
                    state = State::Index;
                } else {
                    // Add text line
                    textLines.append(line);
                }
                break;
        }
    }
    
    // Don't forget last caption if file doesn't end with blank line
    if (state == State::Text && !textLines.isEmpty()) {
        currentCaption.text = textLines.join(" ");
        captions.push_back(currentCaption);
    }
    
    file.close();
    
    if (logger_) {
        logger_->log("Parsed " + std::to_string(captions.size()) + 
                     " captions from: " + filePath.toStdString());
    }
    
    return captions;
}

double CaptionParser::parseTimestamp(const QString& timestamp) {
    // Format: "00:00:01,234" or "00:00:01.234"
    // Returns seconds as double
    
    QString ts = timestamp;
    ts.replace(',', '.');  // Normalize separator
    
    QStringList parts = ts.split(':');
    if (parts.size() != 3) {
        return 0.0;
    }
    
    double hours = parts[0].toDouble();
    double minutes = parts[1].toDouble();
    double seconds = parts[2].toDouble();
    
    return hours * 3600.0 + minutes * 60.0 + seconds;
}

QString CaptionParser::formatTimestamp(double seconds) {
    int totalMs = static_cast<int>(seconds * 1000);
    
    int hours = totalMs / 3600000;
    totalMs %= 3600000;
    
    int minutes = totalMs / 60000;
    totalMs %= 60000;
    
    int secs = totalMs / 1000;
    int ms = totalMs % 1000;
    
    return QString("%1:%2:%3,%4")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0'));
}

bool CaptionParser::exportTXT(const QString& filePath, 
                               const std::vector<Caption>& captions) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (logger_) {
            logger_->error("Failed to create TXT file: " + filePath.toStdString());
        }
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    for (const Caption& cap : captions) {
        out << cap.text << "\n";
    }
    
    file.close();
    
    if (logger_) {
        logger_->log("Exported " + std::to_string(captions.size()) + 
                     " captions to: " + filePath.toStdString());
    }
    
    return true;
}

bool CaptionParser::exportSRT(const QString& filePath,
                               const std::vector<Caption>& captions) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (logger_) {
            logger_->error("Failed to create SRT file: " + filePath.toStdString());
        }
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    for (size_t i = 0; i < captions.size(); ++i) {
        const Caption& cap = captions[i];
        
        // Index
        out << (i + 1) << "\n";
        
        // Timestamps
        out << formatTimestamp(cap.startTime) << " --> " 
            << formatTimestamp(cap.endTime) << "\n";
        
        // Text
        out << cap.text << "\n";
        
        // Blank line separator
        out << "\n";
    }
    
    file.close();
    
    if (logger_) {
        logger_->log("Exported " + std::to_string(captions.size()) + 
                     " captions to: " + filePath.toStdString());
    }
    
    return true;
}
