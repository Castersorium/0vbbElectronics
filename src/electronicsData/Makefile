CXX := g++
DEBUG_FLAGS := -g
RELEASE_FLAGS := -O2 # Could be adjust

ROOT_CONFIG = `root-config --cflags --glibs`

INCLUDE_PATH := ./include
SOURCE_PATH := .
TARGET_PATH := ../../build/electronicsData

SOURCE := $(wildcard $(SOURCE_PATH)/saveVAGraph.cpp)
TARGET := $(TARGET_PATH)/saveVAGraph.exe

# Default
CXXFLAGS := -std=c++17 -Wall $(RELEASE_FLAGS)

$(TARGET): $(SOURCE) | $(TARGET_PATH) # "|" makes sure target path is valid
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH)/ $(SOURCE) $(ROOT_CONFIG) -o $(TARGET) 

$(TARGET_PATH):
	mkdir -p $(TARGET_PATH)

# For debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
