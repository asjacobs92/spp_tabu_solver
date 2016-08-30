# Compiler settings
CXX=g++
CXXFLAGS=-Wall -Iinclude

# Lint settings
LINT=python tools/cpplint.py
LINTFILTER=--filter=-readability/streams,-runtime/threadsafe_fn

# source files
SRC = src/main.cpp src/set_partitioner.cpp
c = include/set_partitioner.h

MKDIR_P = @mkdir -p

OUT_DIR = bin

.PHONY: directories

# build
all: directories spp_tabu_solver

directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

spp_tabu_solver:
	@echo .
	@echo Linting source files...
	$(LINT) $(LINTFILTER) $(SRC) $(INCLUDE)
	@echo .
	@echo .
	@echo .
	@echo Compiling...
	$(CXX) -o bin/$@ $(SRC) $(CXXFLAGS)
	@echo .
