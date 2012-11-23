# License
#
# Copyright (c) 2012 Sébastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)
#

### Options: ###

# C++ compiler 
CXX = g++

# flags for C++
CXXFLAGS ?= -Wall -Wextra -pedantic -Wformat-security -Winit-self -Wswitch-default -Wswitch-enum -Wfloat-equal -Wundef -Wshadow -Wcast-qual -Wconversion -Wlogical-op -Winline -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn

# [Debug,Release]
BUILD ?= Debug

### Conditionally set variables: ###

ifeq ($(BUILD),Debug)
BUILD_FLAGS = -g3 -rdynamic -fstack-protector-all -fno-inline -O0 -DDEBUG -D_DEBUG
endif
ifeq ($(BUILD),Release)
BUILD_FLAGS = -O2
endif
ifeq ($(BUILD),Debug)
LINK_FLAGS = -g3 -rdynamic
endif
ifeq ($(BUILD),Release)
LINK_FLAGS =
endif

### Variables: ###

CPPDEPS = -MT $@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP

OPENGL_CXXFLAGS = $(BUILD_FLAGS) $(CXXFLAGS)
OPENGL_OBJECTS =  \
	$(BUILD)/opengl_main.o \
	
	
### Targets: ###

all: $(BUILD) $(BUILD)/opengl

clean: 
	rm -f $(BUILD)/*.o
	rm -f $(BUILD)/*.d
	rm -f $(BUILD)/opengl

$(BUILD): $(BUILD)/
	mkdir -p $(BUILD)

$(BUILD)/opengl: $(OPENGL_OBJECTS)
	$(CXX) -o $@ $(OPENGL_OBJECTS) $(LINK_FLAGS) -lglut


$(BUILD)/opengl_main.o: src/main.cpp
	$(CXX) -c -o $@ $(OPENGL_CXXFLAGS) $(CPPDEPS) $<


.PHONY: all clean


# Dependencies tracking:
-include $(BUILD)/*.d


