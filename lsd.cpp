#include <iostream>
#include <fstream>

inline constexpr uint8_t GIFSignature[] = {0x47, 0x49, 0x46};

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

int main() {
    const char* filename = "samples/1.gif";
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    // Check GIF signature
    uint8_t fileSignature[3];
    file.read(reinterpret_cast<char*>(fileSignature), sizeof(fileSignature));
    if (fileSignature[0] != GIFSignature[0] || fileSignature[1] != GIFSignature[1] || fileSignature[2] != GIFSignature[2]) {
        std::cerr << "Invalid GIF signature." << std::endl;
        file.close();
        return 1;
    }

    // Skip over GIF header
    file.seekg(6, std::ios::cur);

    // Read Logical Screen Descriptor
    LogicalScreenDescriptor lsd;
    if (!readLogicalScreenDescriptor(file, lsd)) {
        file.close();
        return 1;
    }

    // Print Logical Screen Descriptor
    std::cout << "Width: " << lsd.width << std::endl;
    std::cout << "Height: " << lsd.height << std::endl;
    std::cout << "Packed Fields: " << static_cast<unsigned>(lsd.packedFields) << std::endl;
    std::cout << "Background Color Index: " << static_cast<unsigned>(lsd.backgroundColorIndex) << std::endl;
    std::cout << "Pixel Aspect Ratio: " << static_cast<unsigned>(lsd.pixelAspectRatio) << std::endl;

    file.close();
    return 0;
}
