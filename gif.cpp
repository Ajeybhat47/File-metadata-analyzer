#include <iostream>
#include <fstream>
#include <vector>

inline constexpr uint8_t GIFSignature[] = {0x47, 0x49, 0x46};
// Struct to hold GIF header information
struct GifHeader {
    char signature[3];
    char version[3];
};

// Struct to hold GIF Logical Screen Descriptor
struct LogicalScreenDescriptor {
    unsigned short width;
    unsigned short height;
    unsigned char packedFields;
    unsigned char backgroundColorIndex;
    unsigned char pixelAspectRatio;
};

// Function to read GIF Logical Screen Descriptor
bool readLogicalScreenDescriptor(std::ifstream& file, LogicalScreenDescriptor& lsd) {
    // Read Logical Screen Descriptor
    file.read(reinterpret_cast<char*>(&lsd), sizeof(LogicalScreenDescriptor));

    // Check for read errors
    if (!file) {
        std::cerr << "Error reading Logical Screen Descriptor." << std::endl;
        return false;
    }

    return true;
}

// Function to print GIF Logical Screen Descriptor
void printLogicalScreenDescriptor(const LogicalScreenDescriptor& lsd) {
    std::cout << "Width: " << lsd.width << std::endl;
    std::cout << "Height: " << lsd.height << std::endl;
    std::cout << "Packed Fields: " << static_cast<unsigned>(lsd.packedFields) << std::endl;
    std::cout << "Background Color Index: " << static_cast<unsigned>(lsd.backgroundColorIndex) << std::endl;
    std::cout << "Pixel Aspect Ratio: " << static_cast<unsigned>(lsd.pixelAspectRatio) << std::endl;
}

// Function to extract and print GIF metadata
void printGifMetadata(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Check GIF signature
    uint8_t fileSignature[3];
    file.read(reinterpret_cast<char*>(fileSignature), sizeof(fileSignature));
    if (fileSignature[0] != GIFSignature[0] || fileSignature[1] != GIFSignature[1] || fileSignature[2] != GIFSignature[2]) {
        std::cerr << "Invalid GIF signature." << std::endl;
        file.close();
        return;
    }

    // Read GIF header
    GifHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(GifHeader));

    // Print header information
    std::cout << "GIF Signature: " << header.signature[0] << header.signature[1] << header.signature[2] << std::endl;
    std::cout << "GIF Version: " << header.version[0] << header.version[1] << header.version[2] << std::endl;

    // Read Logical Screen Descriptor
    LogicalScreenDescriptor lsd;
    if (!readLogicalScreenDescriptor(file, lsd)) {
        file.close();
        return;
    }

    // Print Logical Screen Descriptor
    printLogicalScreenDescriptor(lsd);

    // Check for Global Color Table
    bool hasGlobalColorTable = (lsd.packedFields & 0x80) != 0;
    if (hasGlobalColorTable) {
        std::cout << "Global Color Table is present" << std::endl;
        // Skip over the Global Color Table
        int numColors = 2 << (lsd.packedFields & 0x07);
        file.seekg(numColors * 3, std::ios::cur);
    }

    // Check for Graphics Control Extension and other extensions...
    // Skip over extensions...

    file.close();
}

int main() {
    const char* filename = "samples/1.gif";
    printGifMetadata(filename);
    return 0;
}
