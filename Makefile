SOURCE_DIR := src
BUILD_DIR  := build

IGNORE_WARNINGS := -Wno-comment                        \
                   -Wno-unused-parameter               \
                   -Wno-unused-variable                \
                   -Wno-unused-function

LIB_INCLUDES :=
LIB_SOURCES  :=

CXXFLAGS   := -std=c++17 -mwindows -fpermissive \
              -Wall -Wextra $(IGNORE_WARNINGS)

CXXDEFINES := UNICODE _UNICODE _APP_NAME="\"$(notdir $(CURDIR))\"" _APP_FILE_NAME="\"$(notdir $(CURDIR)).exe\""

LDFLAGS    := -static -static-libgcc -static-libstdc++                  \
              -lmsvcrt -lcomdlg32 -lgdi32 -luser32 -lshell32 -lpowrprof

ifeq ($(DEBUG), 1)
CXXFLAGS     += -g -O0
CXXDEFINES   += DEBUG
BUILD_TARGET := debug
else
CXXFLAGS     += -O2 -s -flto -ffunction-sections -fdata-sections -Wl,--gc-sections
CXXDEFINES   += RELEASE
BUILD_TARGET := release
endif

BUILD_TARGET_DIR := $(BUILD_DIR)/$(ARCH)/$(BUILD_TARGET)
TARGET           := $(BUILD_TARGET_DIR)/$(notdir $(CURDIR)).exe

SOURCES         := $(shell find $(SOURCE_DIR) $(LIB_SOURCES) -type f -name "*.cpp")
RC_FILE         := $(shell find $(SOURCE_DIR) $(LIB_SOURCES) -type f -name "*.rc")
RC_OBJECT       := $(RC_FILE:%.rc=$(BUILD_TARGET_DIR)/%.o)
OBJECTS         := $(SOURCES:%.cpp=$(BUILD_TARGET_DIR)/%.o) $(RC_OBJECT)
DEFINES         := $(foreach define,$(CXXDEFINES),-D$(define))
INCLUDES        := $(foreach include,$(LIB_INCLUDES),-I$(include))

.PHONY: all mingw32 mingw64 clean

all: mingw64
build: $(TARGET)
mingw32: ; $(MAKE) build ARCH=x86 CXX=i686-w64-mingw32-g++
mingw64: ; $(MAKE) build ARCH=x64 CXX=x86_64-w64-mingw32-g++

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_TARGET_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(RC_OBJECT): $(RC_FILE)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	windres $(DEFINES) $(INCLUDES) $< $@

clean:
	rm -rf $(BUILD_DIR)
