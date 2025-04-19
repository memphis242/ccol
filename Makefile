.POSIX:

################################# The Prelude ##################################

# Set the OS-specific tool cmds / executable extensions
ifeq ($(OS),Windows_NT)

  TARGET_EXTENSION = exe
  STATIC_LIB_EXTENSION = lib

  ifeq ($(shell uname -s),) # not in a bash-like shell
    CLEANUP = del /F /Q
    MKDIR = mkdir
  else # in a bash-like shell, like msys
    CLEANUP = rm -f
    MKDIR = mkdir -p
  endif

else

  TARGET_EXTENSION = out
  STATIC_LIB_EXTENSION = a
  CLEANUP = rm -f
  MKDIR = mkdir -p

endif

# Relevant directories
PATH_UNITY        = Unity/src/
PATH_SRC          = src/
PATH_INC          = inc/
PATH_TEST_FILES   = test/
PATH_BUILD        = build/
PATH_OBJECT_FILES = $(PATH_BUILD)objs/
PATH_RESULTS      = $(PATH_BUILD)results/
PATH_PROFILE      = $(PATH_BUILD)profile/
PATH_BENCHMARK	   = benchmark/
PATH_SCRIPTS      = scripts/
BUILD_DIRS        = $(PATH_BUILD) $(PATH_OBJECT_FILES)

# Lists of files
# The pattern employed here is to generate lists of files which shall then be
# used as pre-requisities in downstream rules.
COLORIZE_CPPCHECK_SCRIPT = $(PATH_SCRIPTS)colorize_cppcheck.py
COLORIZE_UNITY_SCRIPT = $(PATH_SCRIPTS)colorize_unity.py

UNITY_SRC_FILES = $(wildcard $(PATH_UNITY)*.c)
UNITY_HDR_FILES = $(wildcard $(PATH_UNITY)*.h)

BUILD_TYPE ?= LIB
DS ?= ALL

ifeq ($(DS), ALL)
  SRC_FILES = $(wildcard $(PATH_SRC)*.c)
  HDR_FILES += $(wildcard $(PATH_INC)*.h)
  SRC_TEST_FILES += $(wildcard $(PATH_TEST_FILES)*.c)
  LIB_FILE = $(PATH_BUILD)liblwcol.$(STATIC_LIB_EXTENSION)
else
  SRC_FILES += $(PATH_SRC)$(DS).c
  HDR_FILES += $(PATH_INC)$(DS).c
  SRC_TEST_FILES += $(PATH_TEST_FILES)$(DS).c
  LIB_FILE = $(PATH_BUILD)lib$(DS).$(STATIC_LIB_EXTENSION)
endif

ifeq ($(BUILD_TYPE), TEST)

  BUILD_DIRS += $(PATH_RESULTS)

  SRC_FILES += $(UNITY_SRC_FILES)
  HDR_FILES += $(UNITY_HDR_FILES)
  RESULTS = $(patsubst %.c, $(PATH_RESULTS)%.txt, $(notdir $(SRC_TEST_FILES)))

else ifeq ($(BUILD_TYPE), PROFILE)

  BUILD_DIRS += $(PATH_PROFILE)

endif

# List of all object files we're expecting
OBJ_FILES = $(patsubst %.c,$(PATH_OBJECT_FILES)%.o, $(notdir $(SRC_FILES)))

# Compiler setup
CROSS	= 
CC = $(CROSS)gcc
COMPILER_WARNING_FLAGS = -Wall -Wextra \
								 -Wconversion -Wdouble-promotion -Wnull-dereference \
								 -Wwrite-strings -Wformat=2 -Wcast-align=strict \
								 -Wswitch-enum -Wpedantic -Wfloat-equal \
								 -Wdeprecated-declarations -Wmissing-prototypes -Wparentheses \
								 -Wreturn-type -Wlogical-op -Wstrict-aliasing \
								 -Wuninitialized -Wmaybe-uninitialized -Wshadow
COMPILER_SANITIZERS = -fsanitize=bool -fsanitize=undefined -fsanitize-trap
COMPILER_OPTIMIZATION_LEVEL_DEBUG = -Og -g3
COMPILER_OPTIMIZATION_LEVEL_SPEED = -O3
COMPILER_OPTIMIZATION_LEVEL_SPACE = -Os
COMPILER_STANDARD = -std=c99
INCLUDE_PATHS = -I. -I$(PATH_INC) -I$(PATH_UNITY)
COMMON_DEFINES =
DIAGNOSTIC_FLAGS = -fdiagnostics-color
COMPILER_STATIC_ANALYZER = -fanalyzer

# Compile up the compiler flags
CFLAGS = $(INCLUDE_PATHS) $(COMMON_DEFINES) $(DIAGNOSTIC_FLAGS) $(COMPILER_WARNING_FLAGS) $(COMPILER_STATIC_ANALYZER) $(COMPILER_STANDARD)

ifeq ($(BUILD_TYPE), RELEASE)
$(info CFLAGS for release)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_SPEED)

else ifeq ($(BUILD_TYPE), TEST)
$(info CFLAGS for testing)
CFLAGS += -DTEST $(COMPILER_SANITIZERS) $(COMPILER_OPTIMIZATION_LEVEL_DEBUG)

else ifeq ($(BUILD_TYPE), BENCHMARK)
$(info CFLAGS for benchmarking)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_SPEED)

else ifeq ($(BUILD_TYPE), PROFILE)
$(info CFLAGS for profiling)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_DEBUG) -pg
LDFLAGS += -pg

else
$(info CFLAGS for debugging)
CFLAGS += $(COMPILER_SANITIZERS) $(COMPILER_OPTIMIZATION_LEVEL_DEBUG)
endif

# Compile up linker flags
LDFLAGS += $(DIAGNOSTIC_FLAGS)


############################# The Rules & Recipes ##############################

######################### Lib Rules ########################
.PHONY: lib
# Build the static library files
lib: $(BUILD_DIRS) $(LIB_FILE)
	@echo
	@echo "----------------------------------------"
	@echo "Library built!"
	@echo "----------------------------------------"

# Build the library of the DS in question
#$(PATH_BUILD)%.$(STATIC_LIB_EXTENSION) : $(OBJ_FILES)
$(PATH_BUILD)libvector.$(STATIC_LIB_EXTENSION): $(OBJ_FILES)
	@echo
	@echo "----------------------------------------"
	@echo "Constructing static library of DSA: $<..."
	@echo
	ar rcs $@ $<


######################## Test Rules ########################
.PHONY: test
test: $(BUILD_DIRS) $(TEST_FILE_NAME).exe $(TEST_FILE_NAME).lst	# Don't actually need the .lst file but want to force the disassembly generation
	@echo
	@echo "----------------------------------------"
	@echo "Running the test..."
	@echo
	./$(TEST_FILE_NAME).exe

# Produces an object dump that includes the disassembly of the executable
$(TEST_FILE_NAME).lst: $(TEST_FILE_NAME).exe
	@echo
	@echo "----------------------------------------"
	@echo "Disassembly of $<..."
	@echo
	objdump -D $< > $@

# Build the executable
$(TEST_FILE_NAME).exe: $(TEST_FILE_NAME).o lib$(SRC_FILES).a
	@echo
	@echo "----------------------------------------"
	@echo "Linking $< and the DSA static lib into executable..."
	@echo
	$(CC) $(LDFLAGS) $< -L. -l$(SRC_FILES) -o $@

# Build the object file and run static analysis against it
$(TEST_FILE_NAME).o: $(TEST_FILE_NAME).c
	@echo
	@echo "----------------------------------------"
	@echo "Compiling the test file: $<..."
	@echo
	$(CC) -c $(CFLAGS) $< -o $@
	@echo
	@echo "----------------------------------------"
	@echo "Running static analysis on $<..."
	@echo
	cppcheck $<

######################### Generic ##########################

# Compile the DSA source file into an object file
$(PATH_OBJECT_FILES)%.o : $(PATH_SRC)%.c $(PATH_INC)%.h
	@echo
	@echo "----------------------------------------"
	@echo "Compiling the DSA source file: $<..."
	@echo
	$(CC) -c $(CFLAGS) $< -o $@
	@echo
	@echo "----------------------------------------"
	@echo "Running static analysis on $<..."
	@echo
	cppcheck --template='{severity}: {file}:{line}: {message}' $< 2>&1 | tee $(PATH_BUILD)cppcheck.log | python $(COLORIZE_CPPCHECK_SCRIPT)

# Make the directories if they don't already exist
$(PATH_RESULTS):
	$(MKDIR) $@

$(PATH_OBJECT_FILES):
	$(MKDIR) $@

$(PATH_BUILD):
	$(MKDIR) $@

$(PATH_PROFILE):
	$(MKDIR) $@

# Clean rule to remove generated files
.PHONY: clean
clean:
	$(CLEANUP) $(PATH_OBJECT_FILES)*.o
	$(CLEANUP) $(PATH_BUILD)*.$(TARGET_EXTENSION)
	$(CLEANUP) $(PATH_RESULTS)*.txt
	$(CLEANUP) $(PATH_BUILD)*.lst

.PRECIOUS: $(PATH_BUILD)%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATH_BUILD)Test%.o
.PRECIOUS: $(PATH_RESULTS)%.txt
.PRECIOUS: $(PATH_RESULTS)%.lst
