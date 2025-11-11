#ifndef PIXELARTCONVERTER_H
#define PIXELARTCONVERTER_H

#include <string>
#include <vector>
#include <cstdint>

// BMP file format structures
#pragma pack(push, 1) // Ensure no padding
struct BMPFileHeader
{
    uint16_t fileType{0x4D42}; // File type, must be 'BM'
    uint32_t fileSize{0};      // Size of the file in bytes
    uint16_t reserved1{0};     // Reserved, must be 0
    uint16_t reserved2{0};     // Reserved, must be 0
    uint32_t offsetData{0};    // Offset to start of pixel data
};

struct BMPInfoHeader
{
    uint32_t size{0};            // Size of this header
    int32_t width{0};            // Width of the bitmap in pixels
    int32_t height{0};           // Height of the bitmap in pixels
    uint16_t planes{1};          // Number of color planes, must be 1
    uint16_t bitCount{0};        // Number of bits per pixel
    uint32_t compression{0};     // Compression type
    uint32_t sizeImage{0};       // Size of image data in bytes
    int32_t xPixelsPerMeter{0};  // Horizontal resolution in pixels per meter
    int32_t yPixelsPerMeter{0};  // Vertical resolution in pixels per meter
    uint32_t colorsUsed{0};      // Number of colors in the color palette
    uint32_t colorsImportant{0}; // Number of important colors used
};
#pragma pack(pop) // Ensure no padding

/**
 * Represents a single pixel with RGB values.
 */

struct Pixel
{
    uint8_t b, g, r;      // BMP uses BGR format
    bool isBlack() const; // filled
    bool isWhite() const; // empty
};

/**
 * Represents a rectangular region fo roptimized drawing
 */

struct Rectangle
{
    int x, y;
    int width, height;
    bool filled;

    std::string toJackCode(int indeent = 2) const;
};

/**
 * Main converter class for pixel art to Jack transformation
 */

class PixelArtConverter
{
private:
    std::vector<std::vector<Pixel>> pixels;
    int width;
    int height;

    Rectangle findLargestRectangle(int startX, int startY,
                                   std::vector<std::vector<bool>> &visited);

public:
    bool loadBMP(const std::string &filename);
    bool isPixelFilled(int x, int y) const;
    std::vector<Rectangle> optimizeToRectangles();
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int countBlackPixels() const;
};

/**
 * Extract clean class name filename
 * Removes path and extension
 * @param filename Input filename
 * @return Clean class name
 */

std::string extractClassName(const std::string &filename);

/**
 * Generate Jack code
 * @param className Name of the Jack class
 * @param converter Converter instance with loaded image
 * @param rectangles Optimized rectangles
 */

void generateJackFile(const std::string &className,
                      const PixelArtConverter &converter,
                      const std::vector<Rectangle> &rectangles);

#endif // PIXELARTCONVERTER_H