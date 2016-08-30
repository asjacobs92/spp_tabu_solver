# Compiler settings
CXX=g++
CXXFLAGS=-Wall -Iinclude

# Lint settings
LINT=python ../cpplint.py
LINTFILTER=--filter=-readability/streams,-runtime/threadsafe_fn

# source files
SRC = src/main.cpp

MKDIR_P = @mkdir -p

OUT_DIR = bin

.PHONY: directories

# build
all: directories txt2dat

directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

txt2dat:
	@echo .
	@echo Linting source files...
	$(LINT) $(LINTFILTER) $(SRC)
	@echo .
	@echo .
	@echo .
	@echo Compiling...
	$(CXX) -o bin/$@ $(SRC) $(CXXFLAGS)
	@echo .
