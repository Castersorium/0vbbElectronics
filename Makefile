CFLAGS = -Wall -g
RT_CONFIG = `root-config --cflags --glibs`

INCLUDE_PATH := ./include
SOURCE_PATH := ./src/plotRTrecording
TARGET_PATH := ./build/plotRTrecording

SOURCE := $(wildcard $(SOURCE_PATH)/*.cpp)
TARGET := $(TARGET_PATH)/plotRTrecording.exe

$(TARGET): $(SOURCE) | $(TARGET_PATH) # "|" makes sure target path is valid
	g++ -std=c++17 -I$(INCLUDE_PATH)/ $(SOURCE) $(RT_CONFIG) -o $(TARGET) $(CFLAGS)

$(TARGET_PATH):
	mkdir -p $(TARGET_PATH)

.PHONY: clean
clean:
	rm -rf $(TARGET)
