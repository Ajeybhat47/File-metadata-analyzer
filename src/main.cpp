#include "FileMetaDataAnalyzer.h"
#include <iostream>
#include <iomanip>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::filesystem::path filePath = argv[1];
    CustomMap<std::string, std::string> metadata;

    // Determine file type based on file signature
    FileType fileType = determineFileType<poppler::document, std::ifstream, JPEGHeader, PNGHeader, BMPHeader, ZIPHeader, WAVHeader>(filePath);

    // Analyze metadata based on file type
    switch (fileType) {
        case FileType::PDF:
            try {
                metadata = FileMetaDataAnalyzer<poppler::document>::analyzeMetadata(filePath);
                std::cout << "PDF Metadata:" << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error analyzing PDF metadata: " << e.what() << std::endl;
                return 1;
            }
            break;
        case FileType::TXT:
            metadata = FileMetaDataAnalyzer<std::ifstream>::analyzeMetadata(filePath);
            std::cout << "TXT Metadata:" << std::endl;
            break;
        case FileType::JPEG:
            metadata = FileMetaDataAnalyzer<JPEGHeader>::analyzeMetadata(filePath);
            std::cout << "JPEG Metadata:" << std::endl;
            break;
        case FileType::PNG:
            metadata = FileMetaDataAnalyzer<PNGHeader>::analyzeMetadata(filePath);
            std::cout << "PNG Metadata:" << std::endl;
            break;
        case FileType::BMP:
            metadata = FileMetaDataAnalyzer<BMPHeader>::analyzeMetadata(filePath);
            std::cout << "BMP Metadata:" << std::endl;
            break;
        case FileType::ZIP:
            metadata = FileMetaDataAnalyzer<ZIPHeader>::analyzeMetadata(filePath);
            std::cout << "ZIP Metadata:" << std::endl;
            break;
        case FileType::WAV:
            metadata = FileMetaDataAnalyzer<WAVHeader>::analyzeMetadata(filePath);
            std::cout << "WAV Metadata:" << std::endl;
            break;
        default:
            std::cerr << "Unsupported file format." << std::endl;
            return 1;
    }

    // Print the extracted metadata
    for (const auto& [key, value] : metadata) {
        std::cout << std::left << std::setw(20) << key << ": " << value << std::endl;
    }
    std::cout << std::endl;

    return 0;
}