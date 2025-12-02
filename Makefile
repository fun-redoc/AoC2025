CC=gcc
CXX=g++
RM=rm -rf
CXXFLAGS=-std=c++23 -fPIC -Wall -pedantic
DEBUG_FLAGS = -g         # Debugging flags (you can add more as needed)
LDFLAGS=-Wall
LDLIBS=-Llib/aoc
DEBUG_LDLIBS=-Ldebug_lib/aoc

PROJECT_DIR=.
INCLUDE_DIR=$(PROJECT_DIR)/src/include/
SRC_DIR=$(PROJECT_DIR)/src/cpp
OBJ_DIR=$(PROJECT_DIR)/obj
DEBUG_OBJ_DIR=$(PROJECT_DIR)/debug_obj
LIB_DIR=$(PROJECT_DIR)/lib
DEBUG_LIB_DIR=$(PROJECT_DIR)/debug_lib
BIN_DIR=$(PROJECT_DIR)/bin
DEBUG_BIN_DIR=$(PROJECT_DIR)/debug_bin

CPP_SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) # Replace .cpp
DEBUG_OBJ_FILES = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.o) # Replace .cpp
MAIN_SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/io.cpp
MAIN_OBJ_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) # Replace .cpp
DEBUG_MAIN_OBJ_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.o) # Replace .cpp
LIB_OBJ_FILES_EXCLUDING_MAIN = $(filter-out $(MAIN_OBJ_FILES), $(OBJ_FILES)) # excluding main
DEBUG_LIB_OBJ_FILES_EXCLUDING_MAIN = $(filter-out $(DEBUG_MAIN_OBJ_FILES), $(DEBUG_OBJ_FILES)) # excluding main
SHLIB_OBJ_FILES= $(filter-out $(OBJ_DIR)/aoc%.o, $(LIB_OBJ_FILES_EXCLUDING_MAIN)) # excluding main
DEBUG_SHLIB_OBJ_FILES= $(filter-out $(DEBUG_OBJ_DIR)/aoc%.o, $(DEBUG_LIB_OBJ_FILES_EXCLUDING_MAIN)) # excluding main

TARGET=$(BIN_DIR)/main
DEBUG_TARGET=$(DEBUG_BIN_DIR)/main
LIBS=$(filter $(LIB_DIR)/aoc%.so, $(LIB_OBJ_FILES_EXCLUDING_MAIN:$(OBJ_DIR)/%.o=$(LIB_DIR)/%.so))
DEBUG_LIBS=$(filter $(DEBUG_LIB_DIR)/aoc%.so, $(LIB_OBJ_FILES_EXCLUDING_MAIN:$(OBJ_DIR)/%.o=$(DEBUG_LIB_DIR)/%.so))
LIBS_RPATH=-Wl,rpath=$(LIB_DIR)
DEBUG_LIBS_RPATH=-Wl,-rpath=$(DEBUG_LIB_DIR)
all: $(TARGET) $(LIBS)


# link
$(TARGET): $(MAIN_OBJ_FILES)
	  @mkdir -p $(BIN_DIR)
		$(CXX) $(LDFLAGS) $(LIBS_RPATH) $(LDLIBS) -o $@ $^

#compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
		@mkdir -p $(OBJ_DIR) # Create object directory if it doesn't exist
		$(CXX) $(CXXFLAGS) -c $< -o $@

# Build shared library from object files
$(LIB_DIR)/%.so: $(OBJ_DIR)/%.o $(SHLIB_OBJ_FILES)
		@mkdir -p $(LIB_DIR)   # Create library directory if it doesn't exist
#		$(CXX) -shared -o $@ $< 
		$(CXX) -shared -o $@ $^

# ### DEBUG
# Default debug target
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all_debug

# Builds for debug
all_debug: $(DEBUG_TARGET) $(DEBUG_LIBS)
	#
# link
$(DEBUG_TARGET): $(DEBUG_MAIN_OBJ_FILES)
	  @mkdir -p $(DEBUG_BIN_DIR)
		$(CXX) $(LDFLAGS) $(DEBUG_LIBS_RPATH) $(DEBUG_LDLIBS) -o $@ $^

#compile
$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
		@mkdir -p $(DEBUG_OBJ_DIR) # Create object directory if it doesn't exist
		$(CXX) $(CXXFLAGS) -c $< -o $@

# Build shared library from object files
$(DEBUG_LIB_DIR)/%.so: $(DEBUG_OBJ_DIR)/%.o $(DEBUG_SHLIB_OBJ_FILES)
		@mkdir -p $(DEBUG_LIB_DIR)   # Create library directory if it doesn't exist
#		$(CXX) -shared -o $@ $< 
		$(CXX) -shared -o $@ $^

depend: .depend

.depend: $(CPP_SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJ_DIR)/*.o $(TARGET) $(SHARED_LIB)
		$(RM) $(BIN_DIR) $(LIB_DIR)
		$(RM) $(DEBUG_OBJ_DIR)/*.o $(DEBUG_TARGET) $(DEBUG_SHARED_LIB)
		$(RM) $(DEBUG_BIN_DIR) $(DEBUG_LIB_DIR)

distclean: clean
	$(RM) *~ .depend

include .depend

.PHONY: all clean distclean all_debug debug
