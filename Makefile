CC=gcc
CXX=g++
RM=rm -rf

CXXFLAGS=-std=c++2c -Wc++26-extensions -fPIC -Wall -Wextra -pedantic
LDFLAGS=-Wall -Wextra
LDLIBS=-Llib/aoc

RELEASE_FLAGS=-O3 -march=native -flto -ffast-math -funroll-loops # beware of march native when compiling for multiple processors
RELEASE_LD_FLAGS=-flto -fuse-linker-plugin -fno-fat-lto-objects # beare -ftlo also needs some linker adjustments

DEBUG_FLAGS=-g -DDEBUG  # Debugging flags (you can add more as needed)
DEBUG_LD_FLAGS=
DEBUG_LDLIBS=-Ldebug_lib/aoc

PROJECT_DIR=.
INCLUDE_DIR=$(PROJECT_DIR)/src/include/

SRC_DIR=$(PROJECT_DIR)/src/cpp
OBJ_DIR=$(PROJECT_DIR)/obj
LIB_DIR=$(PROJECT_DIR)/lib

BIN_DIR=$(PROJECT_DIR)/bin
DEBUG_OBJ_DIR=$(PROJECT_DIR)/debug_obj
DEBUG_LIB_DIR=$(PROJECT_DIR)/debug_lib
DEBUG_BIN_DIR=$(PROJECT_DIR)/debug_bin

CPP_SRCS=$(wildcard $(SRC_DIR)/*.cpp)

OBJ_FILES = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) # Replace .cpp
MAIN_SRC_FILES = $(SRC_DIR)/main.cpp $(SRC_DIR)/io.cpp
MAIN_OBJ_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o) # Replace .cpp
LIB_OBJ_FILES_EXCLUDING_MAIN = $(filter-out $(MAIN_OBJ_FILES), $(OBJ_FILES)) # excluding main
SHLIB_OBJ_FILES= $(filter-out $(OBJ_DIR)/aoc%.o, $(LIB_OBJ_FILES_EXCLUDING_MAIN)) # excluding main

DEBUG_OBJ_FILES = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.o) # Replace .cpp
DEBUG_DEP_FILES = $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.d) # Replace .cpp
DEBUG_MAIN_OBJ_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.o) # Replace .cpp
DEBUG_MAIN_DEP_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.cpp=$(DEBUG_OBJ_DIR)/%.d) # Replace .cpp
DEBUG_LIB_OBJ_FILES_EXCLUDING_MAIN = $(filter-out $(DEBUG_MAIN_OBJ_FILES), $(DEBUG_OBJ_FILES)) # excluding main
DEBUG_SHLIB_OBJ_FILES= $(filter-out $(DEBUG_OBJ_DIR)/aoc%.o, $(DEBUG_LIB_OBJ_FILES_EXCLUDING_MAIN)) # excluding main

TARGET=$(BIN_DIR)/main
LIBS=$(filter $(LIB_DIR)/aoc%.so, $(LIB_OBJ_FILES_EXCLUDING_MAIN:$(OBJ_DIR)/%.o=$(LIB_DIR)/%.so))
LIBS_RPATH=-Wl,-rpath=$(LIB_DIR)

DEBUG_TARGET=$(DEBUG_BIN_DIR)/main
DEBUG_LIBS=$(filter $(DEBUG_LIB_DIR)/aoc%.so, $(LIB_OBJ_FILES_EXCLUDING_MAIN:$(OBJ_DIR)/%.o=$(DEBUG_LIB_DIR)/%.so))
DEBUG_LIBS_RPATH=-Wl,-rpath=$(DEBUG_LIB_DIR)

# TESTS
tests: $(PROJECT_DIR)/test_build/simplex_test

# link
$(PROJECT_DIR)/test_build/simplex_test: $(PROJECT_DIR)/test_build/simplex_test.o $(PROJECT_DIR)/test_build/simplex.o $(PROJECT_DIR)/test_build/mymath.o

	  @mkdir -p $(PROJECT_DIR)/test_build
	  $(CXX) $(LDFLAGS) -Wl,-rpath=$(PROJECT_DIR)/test_build -Wall -Wextra -o $@ $^
-include $(wildcard $(PROJECT_DIR)/test_build/*.d)

# compile
$(PROJECT_DIR)/test_build/simplex_test.o: $(SRC_DIR)/test_simplex.cpp 
		@mkdir -p $(PROJECT_DIR)/test_build # Create object directory if it doesn't exist
		$(CXX) -std=c++2c -Wc++26-extensions -Wall -Wextra -pedantic $(DEBUG_FLAGS) -MMD -c $< -o $@
# compile
$(PROJECT_DIR)/test_build/simplex.o:$(SRC_DIR)/simplex.cpp
		@mkdir -p $(PROJECT_DIR)/test_build # Create object directory if it doesn't exist
		$(CXX) -std=c++2c -Wc++26-extensions -Wall -Wextra -pedantic $(DEBUG_FLAGS) -MMD -c $< -o $@
$(PROJECT_DIR)/test_build/mymath.o:$(SRC_DIR)/mymath.cpp
		@mkdir -p $(PROJECT_DIR)/test_build # Create object directory if it doesn't exist
		$(CXX) -std=c++2c -Wc++26-extensions -Wall -Wextra -pedantic $(DEBUG_FLAGS) -MMD -c $< -o $@


# ### RELEASE
# build all release
all: $(TARGET) $(LIBS)

# link
$(TARGET): $(MAIN_OBJ_FILES)
	  @mkdir -p $(BIN_DIR)
		$(CXX) $(LDFLAGS) $(RELEASE_LD_FLAGS) $(LIBS_RPATH) $(LDLIBS) -o $@ $^

#compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp # $(INCLUDE_DIR)/%.hpp
		@mkdir -p $(OBJ_DIR) # Create object directory if it doesn't exist
		$(CXX) $(CXXFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Build shared library from object files
$(LIB_DIR)/%.so: $(OBJ_DIR)/%.o $(SHLIB_OBJ_FILES)
		@mkdir -p $(LIB_DIR)   # Create library directory if it doesn't exist
#		$(CXX) -shared -o $@ $< 
		$(CXX) -shared -o $@ $^

# ### DEBUG
# Default debug target
# debug: CXXFLAGS += $(DEBUG_FLAGS)  moved to debug compile target directly
debug: all_debug

# Builds for debug
all_debug: $(DEBUG_TARGET) $(DEBUG_LIBS)
	#
# link
$(DEBUG_TARGET): $(DEBUG_MAIN_OBJ_FILES)
	  @mkdir -p $(DEBUG_BIN_DIR)
		$(CXX) $(LDFLAGS) $(DEBUG_LIBS_RPATH) $(DEBUG_LDLIBS) -o $@ $^
-include $(wildcard $(DEBUG_OBJ_DIR)/*.d)

#compile
$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp # $(INCLUDE_DIR)/%.hpp
		@mkdir -p $(DEBUG_OBJ_DIR) # Create object directory if it doesn't exist
		$(CXX) $(CXXFLAGS) $(DEBUG_FLAGS) -MMD -MP -c $< -o $@
-include $(wildcard $(DEBUG_OBJ_DIR)/*.d)
#-include $(DEBUG_DEP_FILES)
#-include $(DEBUG_MAIN_DEP_FILES)

# Build shared library from object files
$(DEBUG_LIB_DIR)/%.so: $(DEBUG_OBJ_DIR)/%.o $(DEBUG_SHLIB_OBJ_FILES)
		@mkdir -p $(DEBUG_LIB_DIR)   # Create library directory if it doesn't exist
#		$(CXX) -shared -o $@ $< 
		$(CXX) -shared -o $@ $^

-include $(wildcard $(DEBUG_OBJ_DIR)/*.d)


# ### GENERAL
depend: .depend

.depend: $(CPP_SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJ_DIR)/*.o $(TARGET) $(SHARED_LIB)
		$(RM) $(BIN_DIR) $(LIB_DIR)
		$(RM) $(DEBUG_OBJ_DIR)/*.o $(DEBUG_TARGET) $(DEBUG_SHARED_LIB)
		$(RM) $(DEBUG_BIN_DIR) $(DEBUG_LIB_DIR)
		$(RM) $(PROJECT_DIR)/test_build

distclean: clean
	$(RM) *~ .depend

include .depend

.PHONY: all clean distclean all_debug debug tests
