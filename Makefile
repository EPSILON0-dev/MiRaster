CC      := cc
CFLAGS  := -Wall -Wextra -Werror $(shell sdl2-config --cflags) -fPIC
LDFLAGS := $(shell sdl2-config --libs) -lm

# Library code: rasterizer core, kept backend-free.
LIB_SRCS := $(wildcard raster/*.c)
# SDL2 platform backend, built into its own shared object.
PLATFORM_SRCS := $(wildcard platform/sdl2/*.c)
# Demos: one ELF per top-level demos/*.c, each linked against the rasterizer
# and platform shared objects plus the shared demo scaffolding in demos/common/.
DEMO_SRCS   := $(wildcard demos/*.c)
COMMON_SRCS := $(wildcard demos/common/*.c)
DEMO_BINS   := $(DEMO_SRCS:.c=)

INC  := -Iinclude -I.

DEBUG_FLAGS    := -g -O0 -fsanitize=address -fno-omit-frame-pointer
RELEASE_FLAGS  := -Ofast -march=native -flto

SOFLAGS := -shared

BUILD_DEBUG   := build/debug
BUILD_RELEASE := build/release

DEBUG_LIB_OBJS      := $(addprefix $(BUILD_DEBUG)/,$(LIB_SRCS:.c=.o))
DEBUG_PLATFORM_OBJS := $(addprefix $(BUILD_DEBUG)/,$(PLATFORM_SRCS:.c=.o))
DEBUG_COMMON_OBJS   := $(addprefix $(BUILD_DEBUG)/,$(COMMON_SRCS:.c=.o))
DEBUG_DEMO_OBJS     := $(addprefix $(BUILD_DEBUG)/,$(DEMO_SRCS:.c=.o))
DEBUG_DEMO_BINS     := $(addprefix $(BUILD_DEBUG)/,$(DEMO_BINS))
DEBUG_DEPS          := $(DEBUG_LIB_OBJS:.o=.d) $(DEBUG_PLATFORM_OBJS:.o=.d) $(DEBUG_COMMON_OBJS:.o=.d) $(DEBUG_DEMO_OBJS:.o=.d)

RELEASE_LIB_OBJS      := $(addprefix $(BUILD_RELEASE)/,$(LIB_SRCS:.c=.o))
RELEASE_PLATFORM_OBJS := $(addprefix $(BUILD_RELEASE)/,$(PLATFORM_SRCS:.c=.o))
RELEASE_COMMON_OBJS   := $(addprefix $(BUILD_RELEASE)/,$(COMMON_SRCS:.c=.o))
RELEASE_DEMO_OBJS     := $(addprefix $(BUILD_RELEASE)/,$(DEMO_SRCS:.c=.o))
RELEASE_DEMO_BINS     := $(addprefix $(BUILD_RELEASE)/,$(DEMO_BINS))
RELEASE_DEPS          := $(RELEASE_LIB_OBJS:.o=.d) $(RELEASE_PLATFORM_OBJS:.o=.d) $(RELEASE_COMMON_OBJS:.o=.d) $(RELEASE_DEMO_OBJS:.o=.d)

# Never auto-delete intermediate files (compiled objects are reused between
# the 'debug' and 'release' goals or after a failed goal).
.SECONDARY:

.PHONY: all debug release clean

all: debug

debug: $(BUILD_DEBUG)/libmiraster.so $(BUILD_DEBUG)/libmiraster_sdl2.so $(DEBUG_DEMO_BINS)

release: $(BUILD_RELEASE)/libmiraster.so $(BUILD_RELEASE)/libmiraster_sdl2.so $(RELEASE_DEMO_BINS)

# Rasterizer core: no SDL2 dependency.
$(BUILD_DEBUG)/libmiraster.so: $(DEBUG_LIB_OBJS)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SOFLAGS) -Wl,-soname,libmiraster.so -o $@ $^ -lm

$(BUILD_RELEASE)/libmiraster.so: $(RELEASE_LIB_OBJS)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(SOFLAGS) -Wl,-soname,libmiraster.so -o $@ $^ -lm

# SDL2 platform backend: depends on libmiraster.so in the same build directory.
# '$$ORIGIN' rpath: loader runpaths are not transitive, so without this the
# backend could not find the rasterizer core at runtime (the executable's
# RUNPATH does not apply to the backend's own dependencies).
$(BUILD_DEBUG)/libmiraster_sdl2.so: $(DEBUG_PLATFORM_OBJS) $(BUILD_DEBUG)/libmiraster.so
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(SOFLAGS) -Wl,-soname,libmiraster_sdl2.so -Wl,-rpath,'$$ORIGIN' $(LDFLAGS) -L$(BUILD_DEBUG) -o $@ $(filter %.o,$^) -lmiraster

$(BUILD_RELEASE)/libmiraster_sdl2.so: $(RELEASE_PLATFORM_OBJS) $(BUILD_RELEASE)/libmiraster.so
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(SOFLAGS) -Wl,-soname,libmiraster_sdl2.so -Wl,-rpath,'$$ORIGIN' $(LDFLAGS) -L$(BUILD_RELEASE) -o $@ $(filter %.o,$^) -lmiraster

# Demo ELF: its own object + the demos/common objects, linked against the two
# shared libraries one directory above (build/<flavor>/). '$$ORIGIN/..' rpath
# makes the demos find them there, so they never need to be copied or installed.
$(DEBUG_DEMO_BINS): %: %.o $(DEBUG_COMMON_OBJS) $(BUILD_DEBUG)/libmiraster_sdl2.so $(BUILD_DEBUG)/libmiraster.so
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -L$(BUILD_DEBUG) -Wl,-rpath,'$$ORIGIN/..' -o $@ $(filter %.o,$^) -lmiraster_sdl2 -lmiraster -lm

$(RELEASE_DEMO_BINS): %: %.o $(RELEASE_COMMON_OBJS) $(BUILD_RELEASE)/libmiraster_sdl2.so $(BUILD_RELEASE)/libmiraster.so
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -L$(BUILD_RELEASE) -Wl,-rpath,'$$ORIGIN/..' -o $@ $(filter %.o,$^) -lmiraster_sdl2 -lmiraster -lm

$(BUILD_DEBUG)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(INC) -MMD -MP -c $< -o $@

$(BUILD_RELEASE)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(INC) -MMD -MP -c $< -o $@

clean:
	rm -rf build

-include $(DEBUG_DEPS) $(RELEASE_DEPS)
