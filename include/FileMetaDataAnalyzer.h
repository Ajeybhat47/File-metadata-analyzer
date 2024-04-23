#ifndef FILE_METADATA_ANALYZER_H
#define FILE_METADATA_ANALYZER_H

#include <filesystem>
#include <poppler/cpp/poppler-document.h>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <concepts>
#include "CustomMap.h"

//Enumeration representing the supported file types.
enum class FileType {
    PDF,
    TXT,
    JPEG,
    PNG,
    BMP,
    GIF,
    ZIP,
    WAV,
    UNKNOWN
};


struct BasicMetadata {
    std::string fileName;
    std::string fileSize;
    std::string fileType;
    std::string creationTime;
    std::string lastModified;
    std::string lastAccess;
};

//Structure representing the header of a JPEG file.
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

//Structure representing the header of a PNG file.
struct PNGHeader {
    uint8_t  signature[8];
    uint32_t width;
    uint32_t height;
    uint8_t  bitDepth;
    uint8_t  colorType;
    uint8_t  compression;
    uint8_t  filter;
    uint8_t  interlace;
};

//Structure representing the header of a BMP file.
// Structure representing the header of a BMP file.
struct BMPHeader {
    char     signature[2];
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};

//Structure representing the header of a ZIP file.
struct ZIPHeader {
    uint32_t signature;     
    uint16_t versionMadeBy; // Version made by (high byte version, low byte host system)
    uint16_t versionNeeded;  // Version needed to extract (minimum)
    uint16_t flags;          // General purpose bit flag
    uint16_t compressionMethod; // Compression method
    uint16_t lastModTime;    
    uint16_t lastModDate;    
    uint32_t crc32;         
    uint32_t compressedSize; 
    uint32_t uncompressedSize; 
    uint16_t fileNameLength; 
    uint16_t extraFieldLength; 
};


//Structure representing the header of a WAV file.
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

//structure representing gif header
struct GIFHeader {
    char signature[3];
    char version[3];
};

//extending the gif header/metadata
struct LogicalScreenDescriptor {
    unsigned short width;
    unsigned short height;
    unsigned char packedFields;
    unsigned char backgroundColorIndex;
    unsigned char pixelAspectRatio;
};

//File signature constants for different file types.
inline constexpr uint8_t JPEGSignature[] = {0xFF, 0xD8, 0xFF};
inline constexpr uint8_t PNGSignature[] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
inline constexpr uint8_t BMPSignature[] = {'B', 'M'};
inline constexpr uint8_t PDFSignature[] = {'%', 'P', 'D', 'F'};
inline constexpr uint8_t ZIPSignature[] = {0x50, 0x4B, 0x03, 0x04};
inline constexpr char WAVSignature[] = {'R', 'I', 'F', 'F'};
inline constexpr uint8_t GIFSignature[] = {0x47, 0x49, 0x46}; // "GIF" in ASCII



/**
    Concept to ensure the template parameter T is a valid file header type.
    The concept checks if T is one of the supported file header types: `poppler::document`, `std::ifstream`, `JPEGHeader`, `PNGHeader`, `BMPHeader`, `ZIPHeader`, or `WAVHeader`.
 */
template <typename T>
concept FileHeader = std::is_same_v<T, poppler::document> ||
                     std::is_same_v<T, std::ifstream> ||
                     std::is_same_v<T, JPEGHeader> ||
                     std::is_same_v<T, PNGHeader> ||
                     std::is_same_v<T, BMPHeader> ||
                     std::is_same_v<T, ZIPHeader> ||
                     std::is_same_v<T, WAVHeader> ||
                     std::is_same_v<T, GIFHeader> ||
                     std::is_same_v<T, LogicalScreenDescriptor> ||
                     std::is_same_v<T, BasicMetadata>;


/**
    Determines the file type of the given file path.
 This function uses a fold expression to check the file signature and return the corresponding `FileType`.
 @tparam T The supported file header types.
 @param filePath The path to the file.
 @return The determined file type.
 */
template <typename... T>
    requires (sizeof...(T) > 0)
FileType determineFileType(const std::filesystem::path& filePath);


/**
 Analyzes the metadata of the file at the given path.
 This is a helper function that is called by the `FileMetaDataAnalyzer` class. It extracts the metadata based on the file type.
 @tparam T The file header type.
 @param filePath The path to the file.
 @return A `CustomMap` containing the extracted metadata.
 */
template <typename T>
CustomMap<std::string, std::string> analyzeMetadataHelper(const std::filesystem::path& filePath);



/**
A class that analyzes the metadata of files.
This class uses the `analyzeMetadataHelper()` function to extract the metadata for the supported file types.
@tparam T The supported file header types.
 */
template <FileHeader... T>
class FileMetaDataAnalyzer {
public:
    /**
     Analyzes the metadata of the file at the given path.
     @param filePath The path to the file.
     @return A `CustomMap` containing the extracted metadata.
     */
    static CustomMap<std::string, std::string> analyzeMetadata(const std::filesystem::path& filePath) {
        CustomMap<std::string, std::string> metadata;
        ((void)mergeMap(metadata, analyzeMetadataHelper<T>(filePath)), ...);
        return metadata;
    }

private:
    CustomMap<std::string, std::string> metadata;

    // Declaration of the mergeMap function
    static void mergeMap(CustomMap<std::string, std::string>& dest, const CustomMap<std::string, std::string>& src) {
        for (const auto& [key, value] : src) {
            dest[key] = value;
        }
    }

    // Friend declaration for the analyzeMetadataHelper function template
    template <typename U>
    friend CustomMap<std::string, std::string> analyzeMetadataHelper(const std::filesystem::path& filePath);
};

#endif