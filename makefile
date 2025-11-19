ifeq ($(DEBUG), 1)
BUILD_TARGET := debug
else
BUILD_TARGET := release
endif

APP_NAME     := PowerTray
APP_FILENAME := $(APP_NAME).exe

SOURCE_DIR   := src
RESOURCE_DIR := res
LIB_DIR      := lib
BUILD_DIR    := build

BUILD_TARGET_DIR := $(BUILD_DIR)/$(BUILD_TARGET)
BUILD_OBJ_DIR    := $(BUILD_TARGET_DIR)/obj
TARGET           := $(BUILD_TARGET_DIR)/bin/$(APP_FILENAME)

SOURCES := $(shell find $(SOURCE_DIR) -type f \( -name "*.cpp" -o -name "*.rc" \) )
OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_OBJ_DIR)/%.o,$(SOURCES))
OBJECTS := $(patsubst $(SOURCE_DIR)/%.rc,$(BUILD_OBJ_DIR)/%.rc.o,$(OBJECTS))

################################################################################

CXXFLAGS := -std=c++20 -fpermissive \
            -MMD -MP \
            -Wall -Wextra \
            -Wno-cast-function-type \
            -Wno-unused-parameter \
            -Wno-unused-variable \
            -Wno-unused-function

DEFINES := -DUNICODE -D_UNICODE \
           -D_APP_NAME=$(APP_NAME) \
           -D_APP_FILENAME=$(APP_FILENAME)

INCLUDES := -I$(SOURCE_DIR) \
            -I$(RESOURCE_DIR) \
            -Iinclude \
            -I$(LIB_DIR)/argparse/include

LDFLAGS     := -static -static-libgcc -static-libstdc++ -mwindows \
               -lmsvcrt -lcomdlg32 -lgdi32 -luser32 -lshell32 -lpowrprof

ifeq ($(DEBUG), 1)
CXXFLAGS     += -g -O0
CXXDEFINES   += -DDEBUG
else
CXXFLAGS     += -O3 -flto -ffunction-sections -fdata-sections
CXXDEFINES   += -DRELEASE
LDFLAGS      += -s -Wl,--gc-sections
endif

################################################################################

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $^ $(LDFLAGS)
ifneq ($(DEBUG), 1)
ifneq (, $(shell which upx))
	upx $@
endif
endif

$(BUILD_OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_OBJ_DIR)/%.rc.o: $(SOURCE_DIR)/%.rc
	@mkdir -p $(dir $@)
	windres $(DEFINES) $(INCLUDES) $< $@

clean:
	rm -rf $(BUILD_DIR)

-include $(OBJECTS:.o=.d)
