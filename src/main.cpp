#include "FileMetaDataAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

/**
 Prints the metadata key-value pairs in a formatted way.
 * @tparam KeyType The type of the keys.
 * @tparam ValueType The type of the values.
 * @param metadata The metadata to be printed.
 */

template <typename KeyType, typename ValueType>
void printMetadata(const CustomMap<KeyType, ValueType>& metadata) {
    for (const auto& [key, value] : metadata) {
        std::cout << std::left << std::setw(20) << key << ": " << value << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::filesystem::path filePath = argv[1];
    CustomMap<std::string, std::string> metadata;

    // Determine file type based on file signature
    FileType fileType = determineFileType<poppler::document, std::ifstream, JPEGHeader, PNGHeader, BMPHeader, ZIPHeader, WAVHeader,GIFHeader>(filePath);

    // Analyze metadata based on file type
    switch (fileType) {
        case FileType::PDF:
            try {
                metadata = FileMetaDataAnalyzer<BasicMetadata,poppler::document>::analyzeMetadata(filePath);
                std::cout << "PDF Metadata:" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error analyzing PDF metadata: " << e.what() << std::endl;
                return 1;
            }
            break;
        case FileType::TXT:
            metadata = FileMetaDataAnalyzer<BasicMetadata,std::ifstream>::analyzeMetadata(filePath);
            std::cout << "TXT Metadata:" << std::endl;
            break;
        case FileType::JPEG:
            metadata = FileMetaDataAnalyzer<BasicMetadata,JPEGHeader>::analyzeMetadata(filePath);
            std::cout << "JPEG Metadata:" << std::endl;
            break;
        case FileType::PNG:
            metadata = FileMetaDataAnalyzer<BasicMetadata,PNGHeader>::analyzeMetadata(filePath);
            std::cout << "PNG Metadata:" << std::endl;
            break;
        case FileType::BMP:
            metadata = FileMetaDataAnalyzer<BasicMetadata,BMPHeader>::analyzeMetadata(filePath);
            std::cout << "BMP Metadata:" << std::endl;
            break;
        case FileType::ZIP:
            metadata = FileMetaDataAnalyzer<BasicMetadata,ZIPHeader>::analyzeMetadata(filePath);
            std::cout << "ZIP Metadata:" << std::endl;
            break;
        case FileType::WAV:
            metadata = FileMetaDataAnalyzer<BasicMetadata,WAVHeader>::analyzeMetadata(filePath);
            std::cout << "WAV Metadata:" << std::endl;
            break;

        case FileType::GIF:
            metadata = FileMetaDataAnalyzer<BasicMetadata,GIFHeader,LogicalScreenDescriptor>::analyzeMetadata(filePath);
            std::cout << "GIF Metadata:" << std::endl;
            break;
        default:
            std::cerr << "Unsupported file format." << std::endl;
            return 1;
    }

    // Print the extracted metadata 
    printMetadata(metadata);

    return 0;
}