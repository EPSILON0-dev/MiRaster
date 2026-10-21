CC      := cc
CFLAGS  := -Wall -Wextra -Werror $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs) -lm

SRCS := $(wildcard platform/sdl2/*.c) $(wildcard raster/*.c)

INC  := -Iinclude -I.

DEBUG_FLAGS    := -g -O0 -fsanitize=address -fno-omit-frame-pointer
RELEASE_FLAGS  := -Ofast -march=native

BUILD_DEBUG   := build/debug
BUILD_RELEASE := build/release

DEBUG_OBJS   := $(addprefix $(BUILD_DEBUG)/,$(SRCS:.c=.o))
DEBUG_DEPS   := $(DEBUG_OBJS:.o=.d)
RELEASE_OBJS := $(addprefix $(BUILD_RELEASE)/,$(SRCS:.c=.o))
RELEASE_DEPS := $(RELEASE_OBJS:.o=.d)

.PHONY: all debug release clean

all: debug

debug: $(BUILD_DEBUG)/miraster
$(BUILD_DEBUG)/miraster: $(DEBUG_OBJS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(LDFLAGS) -fsanitize=address -o $@ $^

release: $(BUILD_RELEASE)/miraster
$(BUILD_RELEASE)/miraster: $(RELEASE_OBJS)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DEBUG)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(INC) -MMD -MP -c $< -o $@

$(BUILD_RELEASE)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(INC) -MMD -MP -c $< -o $@

clean:
	rm -rf build $(BUILD_DEBUG) $(BUILD_RELEASE)

-include $(DEBUG_DEPS) $(RELEASE_DEPS)
