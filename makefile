CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
TARGET = pixel2jack
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
HEADERS = ./include/pixelartconverter.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
ifeq ($(OS),Windows_NT)
	del /Q $(TARGET).exe 2>nul
else
	rm -f $(TARGET)
endif