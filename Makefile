SOURCE_DIR := src
BUILD_DIR  := build

IGNORE_WARNINGS := -Wno-comment                        \
                   -Wno-cast-function-type             \
                   -Wno-unused-parameter               \
                   -Wno-unused-variable                \
                   -Wno-unused-function

LIB_INCLUDES :=
LIB_SOURCES  :=

CXX        := x86_64-w64-mingw32-g++
CXXFLAGS   := -std=c++17 -mwindows -fpermissive \
              -Wall -Wextra $(IGNORE_WARNINGS)

CXXDEFINES := UNICODE _UNICODE

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

BUILD_TARGET_DIR := $(BUILD_DIR)/$(BUILD_TARGET)
TARGET           := $(BUILD_TARGET_DIR)/$(notdir $(CURDIR)).exe

SOURCES         := $(shell find $(SOURCE_DIR) $(LIB_SOURCES) -type f -name "*.cpp")
HEADERS         := $(shell find $(SOURCE_DIR) $(LIB_SOURCES) -type f -name "*.h")
RC_FILE         := $(shell find $(SOURCE_DIR) $(LIB_SOURCES) -type f -name "*.rc")
RC_OBJECT       := $(RC_FILE:%.rc=$(BUILD_TARGET_DIR)/%.o)
OBJECTS         := $(SOURCES:%.cpp=$(BUILD_TARGET_DIR)/%.o) $(RC_OBJECT)
DEFINES         := $(foreach define,$(CXXDEFINES),-D$(define))
INCLUDES        := $(foreach include,$(LIB_INCLUDES),-I$(include))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

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
