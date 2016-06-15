.RECIPEPREFIX != ps # spaces instead of tabs

# Project

TARGET = test

INCLUDE_DIR = .
SRC_DIR     = .

# Dependencies

LIBS += -lboost_unit_test_framework

# Compiler flags

CXX = g++
CXX_FLAGS = -I../include -g -DDEBUG -DBOOST_TEST_DYN_LINK -O2 -std=c++11 -MMD -MP
#CXX_FLAGS += -DALGORITHM_OUTPUT # to debug the algorithm

BUILD_DIR = .

OBJ_DIR = .obj

# Files

HEADERS = $(shell find $(INCLUDE_DIR) -name '*.h')
SOURCES = $(shell find $(SRC_DIR) -name '*.cc')
# Here, we map CC and CU files to object and dependency files.
# "src/foo/bar/file.cc" -> "$(OBJ_DIR)/foo/bar/file.o/d"
OBJECTS = $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.o, $(SOURCES))
DEPS    = $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.d, $(SOURCES))

# Rules

$(TARGET): $(OBJECTS)
    $(CXX) $(OBJECTS) $(LIBS) -o $(BUILD_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
    @mkdir -p $(dir $@)
    $(CXX) $(CXX_FLAGS) -o $@ -c $<

clean:
    @rm -rf .obj

-include $(DEPS) # put this at the very end for proper dependency tracking
