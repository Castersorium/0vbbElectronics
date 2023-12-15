CFLAGS = -Wall -g
RT_CONFIG = `root-config --cflags --glibs`

#INCLUDE_PATH := ./include
SOURCE_PATH := .
TARGET_PATH := ../../build/electronicsData

SOURCE := $(wildcard $(SOURCE_PATH)/saveVAGraph.cpp)
TARGET := $(TARGET_PATH)/saveVAGraph.exe

$(TARGET): $(SOURCE) | $(TARGET_PATH) # "|" makes sure target path is valid
	g++ -std=c++17 -I$(INCLUDE_PATH)/ $(SOURCE) $(RT_CONFIG) -o $(TARGET) $(CFLAGS)

$(TARGET_PATH):
	mkdir -p $(TARGET_PATH)

.PHONY: clean
clean:
	rm -rf $(TARGET)
