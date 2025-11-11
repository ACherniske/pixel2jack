/**
 * main.cpp
 * 
 * pixel2jack - Convert pixel art BMP images into Jack drawing code
 * 
 * Usage: pixel2jack <input.bmp>
 * 
 * Part of pixel2jack
 */

#include "pixelartconverter.h"
#include <iostream>

void printUsage(const char* programName) {
    std::cout << "pixel2jack - Pixel Art to Jack Converter\n";
    std::cout << "=========================================\n\n";
    std::cout << "Usage: " << programName << " <input.bmp>\n\n";
    std::cout << "Converts a 1-bit (black & white) BMP image to a Jack class file.\n";
    std::cout << "Black pixels are considered filled; white pixels are empty.\n\n";
    std::cout << "Output: <ImageName>.jack\n\n";
    std::cout << "Tips for best results:\n";
    std::cout << "  - Works with 1-bit (monochrome) BMPS\n";
    std::cout << "  - Use pure black (#000000) for filled pixels\n";
    std::cout << "  - Use pure white (#FFFFFF) for empty pixels\n";
    std::cout << "  - Save as uncompressed BMP format\n";
    std::cout << "For more information: https://github.com/ACherniske/pixel2jack\n";
}

void printVersion() {
    std::cout << "pixel2jack version 1.1.0\n";
    std::cout << "Pixel art to Jack converter for the Hack platform\n";
    std::cout << "Part of the Nand2Tetris ecosystem\n";
}

int main(int argc, char* argv[]) {
    //handle command line arguments
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string arg1 = argv[1];
    
    //check for help flag
    if (arg1 == "-h" || arg1 == "--help" || arg1 == "help") {
        printUsage(argv[0]);
        return 0;
    }
    
    //check for version flag
    if (arg1 == "-v" || arg1 == "--version" || arg1 == "version") {
        printVersion();
        return 0;
    }
    
    //process the BMP file
    std::string inputFile = argv[1];
    std::string className = extractClassName(inputFile);
    
    std::cout << "pixel2jack - Converting " << inputFile << "...\n";
    std::cout << "=========================================\n\n";
    
    PixelArtConverter converter;
    
    if (!converter.loadBMP(inputFile)) {
        std::cerr << "\nConversion failed.\n";
        return 1;
    }
    
    std::cout << "Image loaded successfully!\n";
    std::cout << "Optimizing rectangles...\n\n";
    
    auto rectangles = converter.optimizeToRectangles();
    
    generateJackFile(className, converter, rectangles);
    
    std::cout << "\n✓ Conversion complete!\n";
    
    return 0;
}