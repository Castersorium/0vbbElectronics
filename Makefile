CXX := g++
DEBUG_FLAGS := -g
#RELEASE_FLAGS := -O2 # Could be adjust

# Default
CXXFLAGS := -std=c++17 -Wall $(RELEASE_FLAGS)
ROOT_CONFIG = `root-config --cflags --glibs`
INCLUDE_PATH := ./include

# Define path

## dataConverter
SOURCE_PATH_DATACVT := ./src/dataConverter
TARGET_PATH_DATACVT := ./build/dataConverter
SOURCE_DATACVT := $(wildcard $(SOURCE_PATH_DATACVT)/*.cpp)
TARGET_DATACVT := $(TARGET_PATH_DATACVT)/dataConverter.exe

## plotRTrecording
SOURCE_PATH_PLOTRT := ./src/plotRTrecording
TARGET_PATH_PLOTRT := ./build/plotRTrecording
SOURCE_PLOTRT := $(wildcard $(SOURCE_PATH_PLOTRT)/*.cpp)
TARGET_PLOTRT := $(TARGET_PATH_PLOTRT)/plotRTrecording.exe

# Define all: new Program add here
all: dataConverter plotRTrecording

# Compile

## dataConverter
dataConverter: $(SOURCE_DATACVT) | $(TARGET_PATH_DATACVT) # "|" makes sure target path is valid
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH)/ $(SOURCE_DATACVT) $(ROOT_CONFIG) -o $(TARGET_DATACVT) 

$(TARGET_PATH_DATACVT):
	mkdir -p $(TARGET_PATH_DATACVT)
	
## plotRTrecording
plotRTrecording: $(SOURCE_PLOTRT) | $(TARGET_PATH_PLOTRT) # "|" makes sure target path is valid
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH)/ $(SOURCE_PLOTRT) $(ROOT_CONFIG) -o $(TARGET_PLOTRT) 

$(TARGET_PATH_PLOTRT):
	mkdir -p $(TARGET_PATH_PLOTRT)

# For debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET_DATACVT)
debug: $(TARGET_PLOTRT)

# For clean
.PHONY: clean
clean:
	rm -rf $(TARGET_DATACVT)
	rm -rf $(TARGET_PLOTRT)
clean_DATACVT:
	rm -rf $(TARGET_DATACVT)
clean_PLOTRT:
	rm -rf $(TARGET_PLOTRT)
