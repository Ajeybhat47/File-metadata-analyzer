#ifndef FILE_METADATA_ANALYZER_H
#define FILE_METADATA_ANALYZER_H

#include <filesystem>
#include <poppler/cpp/poppler-document.h>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <concepts>
#include "CustomMap.h"

enum class FileType {
    PDF,
    TXT,
    JPEG,
    PNG,
    BMP,
    ZIP,
    WAV,
    UNKNOWN
};

struct JPEGHeader {
    uint16_t marker;
    uint16_t length;
    uint8_t  identifier[5];
    uint16_t version;
    uint8_t  units;
    uint16_t xDensity;
    uint16_t yDensity;
    uint8_t  thumbWidth;
    uint8_t  thumbHeight;
};

struct PNGHeader {
    uint8_t  signature[8];
    uint32_t width;
    uint32_t height;
    // Add other fields as per PNG specification...
};

struct BMPHeader {
    char     signature[2];
    uint32_t fileSize;
    // Add other fields as per BMP specification...
};

struct ZIPHeader {
    uint32_t signature;     // Always 0x04034b50
    uint16_t versionMadeBy; // Version made by (high byte version, low byte host system)
    uint16_t versionNeeded;  // Version needed to extract (minimum)
    uint16_t flags;          // General purpose bit flag
    uint16_t compressionMethod; // Compression method
    uint16_t lastModTime;    // Last mod file time (MS-DOS time)
    uint16_t lastModDate;    // Last mod file date (MS-DOS date)
    uint32_t crc32;          // CRC-32 of uncompressed data
    uint32_t compressedSize; // Compressed size
    uint32_t uncompressedSize; // Uncompressed size
    uint16_t fileNameLength;  // File name length
    uint16_t extraFieldLength; // Extra field length
};

struct WAVHeader {
    char riffTag[4];
    uint32_t riffSize;
    char waveTag[4];
    char fmtTag[4];
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char dataTag[4];
    uint32_t dataSize;
};

inline constexpr uint8_t JPEGSignature[] = {0xFF, 0xD8, 0xFF};
inline constexpr uint8_t PNGSignature[] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
inline constexpr uint8_t BMPSignature[] = {'B', 'M'};
inline constexpr uint8_t PDFSignature[] = {'%', 'P', 'D', 'F'};
inline constexpr uint8_t ZIPSignature[] = {0x50, 0x4B, 0x03, 0x04};
inline constexpr char WAVSignature[] = {'R', 'I', 'F', 'F'};


template <typename T>
concept FileHeader = std::is_same_v<T, poppler::document> ||
                     std::is_same_v<T, std::ifstream> ||
                     std::is_same_v<T, JPEGHeader> ||
                     std::is_same_v<T, PNGHeader> ||
                     std::is_same_v<T, BMPHeader> ||
                     std::is_same_v<T, ZIPHeader> ||
                     std::is_same_v<T, WAVHeader>;


template <typename... T>
    requires (sizeof...(T) > 0)
FileType determineFileType(const std::filesystem::path& filePath);

template <typename T>
CustomMap<std::string, std::string> analyzeMetadataHelper(const std::filesystem::path& filePath);

template <FileHeader... T>
class FileMetaDataAnalyzer {
public:
    static CustomMap<std::string, std::string> analyzeMetadata(const std::filesystem::path& filePath) {
        return analyzeMetadataHelper<T...>(filePath);
    }



private:
    template <typename U>
    friend CustomMap<std::string, std::string> analyzeMetadataHelper(const std::filesystem::path& filePath);
};


#endif