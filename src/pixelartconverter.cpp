/**
 * Pixel Art Converter
 *
 * Implementation fo pixel art to Jack language converter
 *
 * Part of pixel2jack
 */

#include "pixelartconverter.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

//pixel methods
bool Pixel::isBlack() const
{
    return r == 0;
}

bool Pixel::isWhite() const
{
    return r == 255;
}

//rectangle methods
std::string Rectangle::toJackCode(int indent) const
{
    std::string spaces(indent, '\t');
    std::stringstream ss;
    ss << spaces << "do Screen.setColor(" << (filled ? "true" : "false") << ");\n";
    ss << spaces << "do Screen.drawRectangle("
       << x << ", " << y << ", "
       << (x + width - 1) << ", " << (y + height - 1) << ");\n";
    return ss.str();
}

//pixel art converter methods
bool PixelArtConverter::loadBMP(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false;
    }

    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;

    file.read(reinterpret_cast<char *>(&fileHeader), sizeof(fileHeader));
    file.read(reinterpret_cast<char *>(&infoHeader), sizeof(infoHeader));

    if (fileHeader.fileType != 0x4D42)
    {
        std::cerr << "Error: Not a valid BMP file." << std::endl;
        return false;
    }

    if (infoHeader.bitCount != 1)
    {
        std::cerr << "Error: Only 1-bit BMP files are supported." << std::endl;
        return false;
    }

    width = infoHeader.width;
    height = infoHeader.height;
    pixels.resize(height, std::vector<Pixel>(width));

    //read the 2-color palette (BGR0)
    struct RGBQuad
    {
        uint8_t b, g, r, reserved;
    };
    RGBQuad palette[2];
    file.read(reinterpret_cast<char *>(palette), sizeof(palette));

    //calculate row size (padded to multiple of 4 bytes)
    int rowSize = ((width + 31) / 32) * 4;
    std::vector<uint8_t> rowData(rowSize);

    file.seekg(fileHeader.offsetData, std::ios::beg);

    for (int y = height - 1; y >= 0; --y)
    {
        file.read(reinterpret_cast<char *>(rowData.data()), rowSize);

        for (int x = 0; x < width; ++x)
        {
            int byteIndex = x / 8;
            int bitIndex = 7 - (x % 8);

            //extract bit
            bool bit = (rowData[byteIndex] >> bitIndex) & 1;

            //some BMPs store least significant bit as leftmost pixel
            //detect automatically by checking both bit orders on first line
            if (y == height - 1 && x == 0)
            {
                //heuristic: if leftmost pixel is wrong color, flip bit order
                bool msbBit = (rowData[0] >> 7) & 1;
                bool lsbBit = rowData[0] & 1;
                if (msbBit == bit && lsbBit != bit)
                    bitIndex = (x % 8); //LSB-first
            }

            RGBQuad color = palette[bit];
            pixels[y][x] = Pixel{color.b, color.g, color.r};
        }
    }

    return true;
}

bool PixelArtConverter::isPixelFilled(int x, int y) const
{
    return pixels[y][x].isBlack();
}

std::vector<Rectangle> PixelArtConverter::optimizeToRectangles()
{
    std::vector<Rectangle> rectangles;
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (!visited[y][x] && isPixelFilled(x, y))
            {
                Rectangle rect = findLargestRectangle(x, y, visited);
                rectangles.push_back(rect);
            }
        }
    }
    return rectangles;
}

Rectangle PixelArtConverter::findLargestRectangle(int startX, int startY,
                                                  std::vector<std::vector<bool>> &visited)
{
    int maxWidth = 0;
    while (startX + maxWidth < width &&
           !visited[startY][startX + maxWidth] &&
           isPixelFilled(startX + maxWidth, startY))
    {
        maxWidth++;
    }

    int maxHeight = 0;
    bool canExtend = true;
    while (canExtend && (startY + maxHeight) < height)
    {
        for (int x = 0; x < maxWidth; x++)
        {
            if (visited[startY + maxHeight][startX + x] ||
                !isPixelFilled(startX + x, startY + maxHeight))
            {
                canExtend = false;
                break;
            }
        }
        if (canExtend)
            maxHeight++;
    }

    //mark visited
    for (int y = 0; y < maxHeight; y++)
    {
        for (int x = 0; x < maxWidth; x++)
        {
            visited[startY + y][startX + x] = true;
        }
    }

    return Rectangle{startX, startY, maxWidth, maxHeight, true};
}

int PixelArtConverter::countBlackPixels() const
{
    int count = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (isPixelFilled(x, y))
                count++;
        }
    }
    return count;
}

//utility functions
std::string extractClassName(const std::string &filename)
{
    //find the last slash or backslash
    size_t lastSlash = filename.find_last_of("/\\");
    std::string baseName = (lastSlash != std::string::npos) ? filename.substr(lastSlash + 1) : filename;

    //remove the file extension
    size_t dotPos = baseName.find_last_of(".");
    if (dotPos != std::string::npos)
    {
        baseName = baseName.substr(0, dotPos);
    }

    //capitalize the first letter
    if (!baseName.empty())
    {
        baseName[0] = toupper(baseName[0]);
    }

    //remove invalid characters
    for (char &c : baseName)
    {
        if (!isalnum(c) && c != '_')
        {
            c = '_';
        }
    }
    return baseName;
}

void generateJackFile(const std::string &className,
                      const PixelArtConverter &converter,
                      const std::vector<Rectangle> &rectangles)
{
    std::string outputFilename = className + ".jack";
    std::ofstream outFile(outputFilename);

    if (!outFile)
    {
        std::cerr << "Error: Cannot create output file " << outputFilename << std::endl;
        return;
    }

    //write Jack class
    outFile << "/**\n";
    outFile << " * " << className << " - Generated pixel art class\n";
    outFile << " * Image size: " << converter.getWidth() << "x" << converter.getHeight() << " pixels\n";
    outFile << " * Black pixels: " << converter.countBlackPixels() << "\n";
    outFile << " * Optimized to " << rectangles.size() << " rectangles\n";
    outFile << " */\n";
    outFile << "class " << className << " {\n";
    outFile << "\n";

    //draw function with offset parameters
    outFile << "\t/**\n";
    outFile << "\t * Draws the pixel art at the specified offset\n";
    outFile << "\t * offsetX: X coordinate of top-left corner\n";
    outFile << "\t * offsetY: Y coordinate of top-left corner\n";
    outFile << "\t */\n";
    outFile << "\tfunction void draw(int offsetX, int offsetY) {\n";

    for (const auto &rect : rectangles)
    {
        outFile << "\t\tdo Screen.setColor(" << (rect.filled ? "true" : "false") << ");\n";
        outFile << "\t\tdo Screen.drawRectangle(";
        outFile << "offsetX + " << rect.x << ", ";
        outFile << "offsetY + " << rect.y << ", ";
        outFile << "offsetX + " << (rect.x + rect.width - 1) << ", ";
        outFile << "offsetY + " << (rect.y + rect.height - 1) << ");\n";
    }

    outFile << "\t\treturn;\n";
    outFile << "\t}\n";
    outFile << "\n";

    //simple draw function at origin
    outFile << "\t/**\n";
    outFile << "\t * Draws the pixel art at origin (0, 0)\n";
    outFile << "\t */\n";
    outFile << "\tfunction void drawAtOrigin() {\n";
    outFile << "\t\tdo " << className << ".draw(0, 0);\n";
    outFile << "\t\treturn;\n";
    outFile << "\t}\n";
    outFile << "\n";

    //get dimensions functions
    outFile << "\t/**\n";
    outFile << "\t * Returns the width of the pixel art\n";
    outFile << "\t */\n";
    outFile << "\tfunction int getWidth() {\n";
    outFile << "\t\treturn " << converter.getWidth() << ";\n";
    outFile << "\t}\n";
    outFile << "\n";

    outFile << "\t/**\n";
    outFile << "\t * Returns the height of the pixel art\n";
    outFile << "\t */\n";
    outFile << "\tfunction int getHeight() {\n";
    outFile << "\t\treturn " << converter.getHeight() << ";\n";
    outFile << "\t}\n";

    outFile << "}\n";

    outFile.close();

    std::cout << "Generated: " << outputFilename << std::endl;
    std::cout << "Class: " << className << std::endl;
    std::cout << "Size: " << converter.getWidth() << "x" << converter.getHeight() << " pixels" << std::endl;
    std::cout << "Rectangles: " << rectangles.size() << std::endl;
    std::cout << "\nUsage in Jack code:" << std::endl;
    std::cout << "  do " << className << ".draw(x, y);  // Draw at position x, y" << std::endl;
    std::cout << "  do " << className << ".drawAtOrigin();  // Draw at 0, 0" << std::endl;
}