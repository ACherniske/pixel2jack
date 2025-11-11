# pixel2jack

Convert pixel art into optimized Jack drawing code for the Hack platform

## Overview

pixel2jack is a command-line tool that converts 1-bit BMP images into optimized Jack language code for drawing pixel art on the Hack computer platform. It reads monochrome bitmap files and generates efficient rectangle-based drawing commands, minimizing the number of draw calls needed to render the image.

## Features

- **1-bit BMP Support**: Reads monochrome bitmap files with black and white pixels
- **Rectangle Optimization**: Converts pixel data into optimized rectangular regions to minimize drawing operations
- **Jack Code Generation**: Outputs clean, readable Jack class files with drawing functions
- **Cross-platform**: Works on Windows, macOS, and Linux
- **Command-line Interface**: Simple usage with helpful error messages and usage instructions

## Installation

### Prerequisites

- C++ compiler with C++17 support (g++, clang++, or MSVC)
- Make or compatible build system

### Building from Source

```bash
# Clone or download the project
cd pixel2jack

# Build using make
make

# Or build manually
g++ -std=c++17 -Wall -Wextra -I./include -o pixel2jack src/*.cpp
```

## Usage

### Basic Usage

```bash
# Convert a BMP file to Jack code
./pixel2jack image.bmp

# This generates image.jack in the current directory
```

### Command Line Options

```bash
# Show help
./pixel2jack -h
./pixel2jack --help
./pixel2jack help

# Show version information
./pixel2jack -v
./pixel2jack --version
./pixel2jack version
```

### Input Requirements

- **File format**: 1-bit (monochrome) BMP files only
- **Colors**: Pure black (#000000) for filled pixels, pure white (#FFFFFF) for empty pixels
- **Compression**: Uncompressed BMP format recommended
- **Size**: Any reasonable dimensions (limited by target platform memory)

### Output

The tool generates a Jack class file with the following structure:

```jack
/**
 * ImageName - Generated pixel art class
 * Image size: WIDTHxHEIGHT pixels
 * Black pixels: COUNT
 * Optimized to N rectangles
 */
class ImageName {
    /** Draws the pixel art at the specified offset */
    function void draw(int offsetX, int offsetY) { /* ... */ }
    
    /** Draws the pixel art at origin (0, 0) */
    function void drawAtOrigin() { /* ... */ }
    
    /** Returns the width of the pixel art */
    function int getWidth() { /* ... */ }
    
    /** Returns the height of the pixel art */
    function int getHeight() { /* ... */ }
}
```