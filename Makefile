CXX         := x86_64-w64-mingw32-g++
CXXDEFINES  := UNICODE _UNICODE
CXXINCLUDES :=
CXXFLAGS    := -Wall -Wno-unused-variable -Wno-unused-function -O2 -mwindows
LDFLAGS     := -static -static-libgcc -static-libstdc++ \
               -lmsvcrt -lmsvcrt                        \
               -lcomdlg32 -lgdi32 -luser32 -lshell32 -lpowrprof

ifeq ($(DEBUG), 1)
CXXFLAGS     += -g
CXXDEFINES   += DEBUG
BUILD_TARGET := debug
else
CXXFLAGS     += -s -flto -ffunction-sections -fdata-sections -Wl,--gc-sections
CXXDEFINES   += RELEASE
BUILD_TARGET := release
endif

SOURCE_DIR       := src
BUILD_DIR        := build
BUILD_TARGET_DIR := $(BUILD_DIR)/$(BUILD_TARGET)

HEADERS  := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES  := $(wildcard $(SOURCE_DIR)/*.cpp)
RC_FILE   := $(wildcard $(SOURCE_DIR)/*.rc)
RC_OBJECT := $(RC_FILE:%.rc=$(BUILD_TARGET_DIR)/%.o)
OBJECTS   := $(SOURCES:%.cpp=$(BUILD_TARGET_DIR)/%.o) $(RC_OBJECT)
DEFINES  := $(foreach define,$(CXXDEFINES),-D$(define))
INCLUDES := $(foreach include,$(CXXINCLUDES),-I$(include))

TARGET := $(BUILD_TARGET_DIR)/$(notdir $(CURDIR)).exe

.PHONY: all debug release clean

all: $(TARGET)

debug:   ; @$(MAKE) DEBUG=1
release: ; @$(MAKE) DEBUG=0

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_TARGET_DIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(RC_OBJECT): $(RC_FILE)
	@mkdir -p $(BUILD_TARGET_DIR)
	@mkdir -p $(shell dirname $@)
	windres $(RC_FILE) $(RC_OBJECT)

clean:
	rm -rf $(BUILD_DIR)
