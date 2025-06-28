TARGET_NAME := PowerTray

################################################################################

CXX     := x86_64-w64-mingw32-g++
WINDRES := windres

MD5     := md5sum

MKDIR   := mkdir
CP      := cp
RM      := rm
TOUCH   := touch

################################################################################

CXXFLAGS := -std=c++17 -fpermissive -MMD -MP \
            -Wall -Wextra \
            -Wno-cast-function-type \
            -Wno-unused-variable

CXXDEFINES  := -DUNICODE -D_UNICODE
CXXINCLUDES := -Iinclude

LDFLAGS := -static -static-libgcc -static-libstdc++ -mwindows \
           -lmsvcrt -lcomdlg32 -lgdi32 -luser32 -lshell32 -lpowrprof

ifeq ($(DEBUG), 1)
CXXFLAGS     += -g -O0
CXXDEFINES   += -DDEBUG
BUILD_TARGET := debug
else
CXXFLAGS     += -O2 -flto -ffunction-sections -fdata-sections
LDFLAGS      += -s -Wl,--gc-sections
CXXDEFINES   += -DRELEASE
BUILD_TARGET := release
endif

################################################################################

RES_DIR   := res
SRC_DIR   := src
BUILD_DIR := build/$(BUILD_TARGET)

TARGET := $(BUILD_DIR)/$(TARGET_NAME).exe

SRCS  := $(shell find $(SRC_DIR) -type f -name "*.cpp")
RCS   := $(shell find $(RES_DIR) -type f -name "*.rc")
OBJS  := $(SRCS:%.cpp=$(BUILD_DIR)/%.o) $(RCS:%.rc=$(BUILD_DIR)/%.rc.o)
DEPS  := $(OBJS:.o=.d)
RES   := $(shell find $(RES_DIR) -type f)

RES_STAMP     := $(BUILD_DIR)/$(RES_DIR)/timestamp
RES_HASH      := $(BUILD_DIR)/$(RES_DIR)/current.md5
RES_HASH_TEMP := $(BUILD_DIR)/$(RES_DIR)/temp.md5

################################################################################

.PHONY: all clean $(TARGET) $(DEPS)

all: _res $(TARGET)

_res: $(RES)
	@$(MKDIR) -p $(BUILD_DIR)
	@$(MKDIR) -p $(dir $(RES_STAMP))
	@$(MD5) $(RES) | sort > $(RES_HASH_TEMP)
	@if [ ! -f $(RES_HASH) ] || [ "$$($(MD5) $(RES_HASH) | awk '{print $$1}')" != "$$($(MD5) $(RES_HASH_TEMP) | awk '{print $$1}')" ]; then \
		$(CP) $(RES_HASH_TEMP) $(RES_HASH); \
		$(TOUCH) $(RES_STAMP); \
	fi

$(TARGET): $(OBJS) $(DEPS)
	@$(MKDIR) -p $(BUILD_DIR)
	@$(MKDIR) -p $(shell dirname $@)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR) -p $(BUILD_DIR)
	@$(MKDIR) -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) $(CXXDEFINES) $(CXXINCLUDES) -c $< -o $@

$(BUILD_DIR)/%.rc.o: $(RCS) $(RES_STAMP)
	@$(MKDIR) -p $(BUILD_DIR)
	@$(MKDIR) -p $(shell dirname $@)
	$(WINDRES) $(CXXDEFINES) $(CXXINCLUDES) $< $@

clean:
	$(RM) -rf $(BUILD_DIR)

-include $(DEPS)
