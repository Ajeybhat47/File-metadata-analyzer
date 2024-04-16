#include "FileMetaDataAnalyzer.h"
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <zip.h>
#include <type_traits>

/**
 * @brief Helper function to analyze the metadata of a file based on its type.
 *
 * This function uses template specialization to handle the metadata extraction for each supported file type.
 *
 * @tparam T The file header type.
 * @param filePath The path to the file.
 * @return A `CustomMap` containing the extracted metadata.
 */
template <typename T>
CustomMap<std::string, std::string> analyzeMetadataHelper(const std::filesystem::path& filePath) {
    CustomMap<std::string, std::string> metadata;

    if constexpr (std::is_same_v<T, poppler::document>) {
        // PDF metadata extraction logic
        poppler::document* doc = poppler::document::load_from_file(filePath.string());
        if (!doc || doc->is_locked()) {
            delete doc;
            return metadata;
        }

        metadata["Title"] = std::string(doc->get_title().begin(), doc->get_title().end());
        metadata["Author"] = std::string(doc->get_author().begin(), doc->get_author().end());
        metadata["Subject"] = std::string(doc->get_subject().begin(), doc->get_subject().end());
        metadata["Keywords"] = std::string(doc->get_keywords().begin(), doc->get_keywords().end());
        metadata["Creator"] = std::string(doc->get_creator().begin(), doc->get_creator().end());
        metadata["Producer"] = std::string(doc->get_producer().begin(), doc->get_producer().end());
        metadata["CreationDate"] = doc->get_creation_date();
        metadata["ModificationDate"] = doc->get_modification_date();
        metadata["FileType"] = "PDF";
        delete doc;
    } else if constexpr (std::is_same_v<T, std::ifstream>) {
        // TXT metadata extraction logic
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return metadata;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();

        std::string line;
        std::getline(file, line);
        if (!line.empty()) {
            metadata["Title"] = line;
        }

        std::getline(file, line);
        if (!line.empty()) {
            metadata["Author"] = line;
        }

        // Extract other TXT metadata...

        metadata["FileName"] = filePath.filename().string();
        metadata["FileSize"] = std::to_string(fileContent.size()) + " bytes";
        metadata["FileType"] = "TXT";
        file.close();
    } else if constexpr (std::is_same_v<T, JPEGHeader>) {
        // JPEG metadata extraction logic
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return metadata;
        }

        JPEGHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(JPEGHeader));
        file.close();

        metadata["FileType"] = "JPEG";
        metadata["Marker"] = std::to_string(header.marker);
        metadata["Length"] = std::to_string(header.length);
        metadata["Identifier"] = std::string(reinterpret_cast<char*>(header.identifier), 5);
        metadata["Version"] = std::to_string(header.version);
        metadata["Units"] = std::to_string(static_cast<int>(header.units));
        metadata["XDensity"] = std::to_string(header.xDensity);
        metadata["YDensity"] = std::to_string(header.yDensity);
        metadata["ThumbnailWidth"] = std::to_string(static_cast<int>(header.thumbWidth));
        metadata["ThumbnailHeight"] = std::to_string(static_cast<int>(header.thumbHeight));
    } else if constexpr (std::is_same_v<T, PNGHeader>) {
        // PNG metadata extraction logic
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return metadata;
        }

        PNGHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(PNGHeader));
        file.close();

        metadata["FileType"] = "PNG";
        metadata["Signature"] = std::string(reinterpret_cast<char*>(header.signature), 8);
        metadata["Width"] = std::to_string(header.width);
        metadata["Height"] = std::to_string(header.height);
    } else if constexpr (std::is_same_v<T, BMPHeader>) {
        // BMP metadata extraction logic
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return metadata;
        }

        BMPHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
        file.close();

        metadata["FileType"] = "BMP";
        metadata["Signature"] = std::string(header.signature, 2);
        metadata["FileSize"] = std::to_string(header.fileSize);
    } else if constexpr (std::is_same_v<T, ZIPHeader>) {
        // ZIP metadata extraction logic
        int error;
        zip_t* zip = zip_open(filePath.string().c_str(), 0, &error);
        if (!zip) {
            // Handle the error code in `error`
            return metadata;
        }

        // Get the overall ZIP file information
        zip_stat_t zipStat;
        if (zip_stat(zip, "", 0, &zipStat) == 0) {
            metadata["FileType"] = "ZIP";
            metadata["FileSize"] = std::to_string(zipStat.size) + " bytes";
        }

        // Get the ZIP archive comment
        int commentLength;
        const char* comment = zip_get_archive_comment(zip, &commentLength, 0);
        if (commentLength > 0) {
            metadata["Comment"] = std::string(comment, commentLength);
        }

        // Get the first file in the ZIP archive
        zip_file_t* file = zip_fopen_index(zip, 0, 0);
        if (file) {
            ZIPHeader header;
            zip_fread(file, &header, sizeof(ZIPHeader));

            // Extract metadata from the ZIP header
            metadata["FileName"] = std::string(reinterpret_cast<char*>(header.fileNameLength + 1), header.fileNameLength);
            metadata["CompressedSize"] = std::to_string(header.compressedSize) + " bytes";
            metadata["CompressionMethod"] = std::to_string(header.compressionMethod);
            metadata["LastModificationTime"] = std::to_string(header.lastModTime);
            metadata["LastModificationDate"] = std::to_string(header.lastModDate);
            metadata["CRC32"] = std::to_string(header.crc32);
            metadata["UncompressedSize"] = std::to_string(header.uncompressedSize) + " bytes";

            zip_fclose(file);
        }

        zip_close(zip);
    } else if constexpr (std::is_same_v<T, WAVHeader>) {
        //WAV metadata extraction logic
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return metadata;
        }

        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
        file.close();

        metadata["FileType"] = "WAV";
        metadata["RIFFTag"] = std::string(header.riffTag, 4);
        metadata["RIFFSize"] = std::to_string(header.riffSize);
        metadata["WAVETag"] = std::string(header.waveTag, 4);
        metadata["FMTTag"] = std::string(header.fmtTag, 4);
        metadata["FMTSize"] = std::to_string(header.fmtSize);
        metadata["AudioFormat"] = std::to_string(header.audioFormat);
        metadata["NumChannels"] = std::to_string(header.numChannels);
        metadata["SampleRate"] = std::to_string(header.sampleRate);
        metadata["ByteRate"] = std::to_string(header.byteRate);
        metadata["BlockAlign"] = std::to_string(header.blockAlign);
        metadata["BitsPerSample"] = std::to_string(header.bitsPerSample);
        metadata["DataTag"] = std::string(header.dataTag, 4);
        metadata["DataSize"] = std::to_string(header.dataSize);
    }

    return metadata;
}

/**
 * @brief Determines the file type based on the file signature.
 *
 * This function uses a fold expression to check the file signature and return the corresponding `FileType`.
 *
 * @tparam T The supported file header types.
 * @param filePath The path to the file.
 * @return The determined file type.
 */
template <typename... T>
    requires (sizeof...(T) > 0)
FileType determineFileType(const std::filesystem::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return FileType::UNKNOWN;
    }

    uint8_t signature[8];
    file.read(reinterpret_cast<char*>(signature), sizeof(signature));

    // Fold expression to check the file signature
    return (
        [&]() -> FileType {
            return (
                (std::memcmp(signature, JPEGSignature, sizeof(JPEGSignature)) == 0) ? FileType::JPEG :
                (std::memcmp(signature, PNGSignature, sizeof(PNGSignature)) == 0) ? FileType::PNG :
                (std::memcmp(signature, BMPSignature, sizeof(BMPSignature)) == 0) ? FileType::BMP :
                (signature[0] == '%' && signature[1] == 'P' && signature[2] == 'D' && signature[3] == 'F') ? FileType::PDF :
                (std::memcmp(signature, ZIPSignature, sizeof(ZIPSignature)) == 0) ? FileType::ZIP :
                (std::memcmp(signature, WAVSignature, sizeof(WAVSignature)) == 0) ? FileType::WAV :
                FileType::TXT
            );
        }()
    );
}

// Explicit template instantiations for the supported file header types
template FileType determineFileType<poppler::document, std::ifstream, JPEGHeader, PNGHeader, BMPHeader, ZIPHeader, WAVHeader>(const std::filesystem::path& filePath);

// Explicit template instantiations for the FileMetaDataAnalyzer class
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<poppler::document>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<std::ifstream>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<JPEGHeader>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<PNGHeader>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<BMPHeader>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<ZIPHeader>::analyzeMetadata(const std::filesystem::path& filePath);
template CustomMap<std::string, std::string> FileMetaDataAnalyzer<WAVHeader>::analyzeMetadata(const std::filesystem::path& filePath);