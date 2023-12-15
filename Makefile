CXX := g++
DEBUG_FLAGS := -g
RELEASE_FLAGS := -O2 # Could be adjust

# Default
CXXFLAGS := -std=c++17 -Wall $(RELEASE_FLAGS)

RT_CONFIG := `root-config --cflags --glibs`

INCLUDE_PATH := ./include
SOURCE_PATH := ./src/plotRTrecording
TARGET_PATH := ./build/plotRTrecording

SOURCE := $(wildcard $(SOURCE_PATH)/*.cpp)
TARGET := $(TARGET_PATH)/plotRTrecording.exe

$(TARGET): $(SOURCE) | $(TARGET_PATH) # "|" makes sure target path is valid
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH)/ $(SOURCE) $(RT_CONFIG) -o $(TARGET) $(CFLAGS)

$(TARGET_PATH):
	mkdir -p $(TARGET_PATH)

# For debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

.PHONY: clean
clean:
	rm -rf $(TARGET)
