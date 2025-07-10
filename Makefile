.POSIX:

################################# The Prelude ##################################

.PHONY: release release-vec libvector
.PHONY: debug debug-vec
.PHONY: test-vec test-all
.PHONY: test-vec-arena

test-vec:
	@$(MAKE) _test BUILD_TYPE=TEST DS=vector > /dev/null
	cat $(RESULTS) | python $(COLORIZE_UNITY_SCRIPT)

test-vec-arena:
	@$(MAKE) _test BUILD_TYPE=TEST-ARENA DS=vector > /dev/null
# TODO: Start up both the server test arena .c file and the client Python script

test-all:
	@$(MAKE) --always-make test-vec > /dev/null
	cat $(RESULTS) | python $(COLORIZE_UNITY_SCRIPT)

release:
	@$(MAKE) lib BUILD_TYPE=RELEASE DS=ALL

libvector: release-vec
release-vec:
	@$(MAKE) lib BUILD_TYPE=RELEASE DS=vector

debug:
	@$(MAKE) lib BUILD_TYPE=DEBUG DS=ALL

debug-vec:
	@$(MAKE) lib BUILD_TYPE=DEBUG DS=vector

CLEANUP = rm -f
MKDIR = mkdir -p
TARGET_EXTENSION=exe
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

# Relevant paths
PATH_UNITY        = Unity/src/
PATH_SRC          = src/
PATH_INC          = inc/
PATH_CFG          = cfg/
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
UNITY_OBJ_FILES = $(patsubst %.c, $(PATH_OBJECT_FILES)%.o, $(notdir $(UNITY_SRC_FILES)))

COLLECTION_LIB_NAME = ccol

BUILD_TYPE ?= RELEASE
DS ?= ALL

ifeq ($(DS), ALL)
  SRC_FILES = $(wildcard $(PATH_SRC)*.c)
  HDR_FILES = $(wildcard $(PATH_INC)*.h) $(wildcard $(PATH_CFG)$(DS)_cfg.h)
  SRC_TEST_FILES = $(wildcard $(PATH_TEST_FILES)*.c)
  LIB_FILE = $(PATH_BUILD)lib$(COLLECTION_LIB_NAME).$(STATIC_LIB_EXTENSION)
  DS_OBJ_FILES = $(patsubst %.c, $(PATH_OBJECT_FILES)%.o, $(notdir $(SRC_FILES)))
else
  SRC_FILES = $(PATH_SRC)$(DS).c
  HDR_FILES = $(PATH_INC)$(DS).h $(wildcard $(PATH_CFG)$(DS)_cfg.h)
  SRC_TEST_FILES = $(PATH_TEST_FILES)test_$(DS).c
  LIB_FILE = $(PATH_BUILD)lib$(DS).$(STATIC_LIB_EXTENSION)
  DS_OBJ_FILES = $(PATH_OBJECT_FILES)$(DS).o
endif
TEST_EXECUTABLES = $(patsubst %.c, $(PATH_BUILD)%.$(TARGET_EXTENSION), $(notdir $(SRC_TEST_FILES)))
LIB_LIST_FILE = $(patsubst %.$(STATIC_LIB_EXTENSION), $(PATH_BUILD)%.lst, $(notdir $(LIB_FILE)))
TEST_LIST_FILE = $(patsubst %.$(TARGET_EXTENSION), $(PATH_BUILD)%.lst, $(notdir $(TEST_EXECUTABLES)))
TEST_OBJ_FILES = $(patsubst %.c, $(PATH_OBJECT_FILES)%.o, $(notdir $(SRC_TEST_FILES)))
RESULTS = $(patsubst %.c, $(PATH_RESULTS)%.txt, $(notdir $(SRC_TEST_FILES)))

ifeq ($(BUILD_TYPE), TEST)
  BUILD_DIRS += $(PATH_RESULTS)
else ifeq ($(BUILD_TYPE), PROFILE)
  BUILD_DIRS += $(PATH_PROFILE)
endif

# List of all object files we're expecting for the data structures
OBJ_FILES = $(patsubst %.c,$(PATH_OBJECT_FILES)%.o, $(notdir $(SRC_FILES)))

# Compiler setup
CROSS	= 
CC = $(CROSS)gcc

COMPILER_WARNING_FLAGS = \
    -Wall -Wextra -Wpedantic -pedantic-errors \
    -Wconversion -Wdouble-promotion -Wnull-dereference \
    -Wwrite-strings -Wformat=2 -Wformat-overflow=2 \
    -Wformat-signedness -Wuseless-cast -Wstrict-prototypes \
    -Wcast-align=strict -Wimplicit-fallthrough=3 -Wswitch-default \
    -Wswitch-enum -Wfloat-equal -Wuse-after-free=2 \
    -Wdeprecated-declarations -Wmissing-prototypes -Wparentheses \
    -Wreturn-type -Wlogical-op -Wstrict-aliasing \
    -Wuninitialized -Wmaybe-uninitialized -Wshadow \
    -Wduplicated-cond -Wduplicated-branches \
    -Walloc-zero -Walloc-size

# Includes some -Wno-... flags for warnings that I'd normally want for my lib
# src but **not** for my test file, which intentionally has all sorts of
# naughty shenanigans going on
COMPILER_WARNINGS_TEST_BUILD = \
    -Wall -Wextra -Wpedantic -pedantic-errors \
    -Wconversion -Wdouble-promotion -Wnull-dereference \
    -Wwrite-strings -Wformat=2 -Wformat-overflow=2 \
    -Wformat-signedness \
    -Wcast-align=strict -Wimplicit-fallthrough=3 -Wswitch-default \
    -Wswitch-enum -Wfloat-equal -Wuse-after-free=2 \
    -Wdeprecated-declarations -Wmissing-prototypes -Wparentheses \
    -Wreturn-type -Wlogical-op -Wstrict-aliasing \
    -Wuninitialized -Wmaybe-uninitialized -Wshadow \
    -Wsuggest-attribute=const \
    -Walloc-zero -Walloc-size \
    -Wno-analyzer-use-of-uninitialized-value -Wno-uninitialized \
    -Wno-maybe-uninitialized

# Consider -Wmismatched-dealloc
COMPILER_SANITIZERS = \
    -fsanitize=undefined -fsanitize-trap \
    -fsanitize=enum  -fsanitize=bool -fsanitize=bounds

LIB_OPTIMIZATION_FLAGS ?=
ifdef LTO
LIB_OPTIMIZATION_FLAGS = -ffunction-sections -fdata-sections # Facilitates link-time optimization (LDO)
endif
COMPILER_OPTIMIZATION_LEVEL_DEBUG = -Og -g3
COMPILER_OPTIMIZATION_LEVEL_SPEED = -O3 $(LIB_OPTIMIZATION_FLAGS)
COMPILER_OPTIMIZATION_LEVEL_SPACE = -Os $(LIB_OPTIMIZATION_FLAGS)
COMPILER_STANDARD = -std=c99
INCLUDE_PATHS = -I. -I$(PATH_INC) -I$(PATH_UNITY) -I$(PATH_CFG)
COMMON_DEFINES =
DIAGNOSTIC_FLAGS = -fdiagnostics-color
COMPILER_STATIC_ANALYZER = -fanalyzer

# Compile up the compiler flags
CFLAGS = $(INCLUDE_PATHS) $(COMMON_DEFINES) \
			$(DIAGNOSTIC_FLAGS) $(COMPILER_WARNING_FLAGS) $(COMPILER_STATIC_ANALYZER) \
			$(COMPILER_STANDARD) $(COMPILER_OPTIMIZATION_LEVEL_SPEED)

CFLAGS_TEST = \
         -DTEST $(COMMON_DEFINES) \
         $(INCLUDE_PATHS) \
         $(DIAGNOSTIC_FLAGS) $(COMPILER_WARNINGS_TEST_BUILD) \
         $(COMPILER_STATIC_ANALYZER) $(COMPILER_STANDARD) \
         $(COMPILER_SANITIZERS) $(COMPILER_OPTIMIZATION_LEVEL_DEBUG)

ifeq ($(BUILD_TYPE), RELEASE)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_SPEED)

else ifeq ($(BUILD_TYPE), BENCHMARK)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_SPEED)

else ifeq ($(BUILD_TYPE), PROFILE)
CFLAGS += -DNDEBUG $(COMPILER_OPTIMIZATION_LEVEL_DEBUG) -pg
LDFLAGS += -pg

else
CFLAGS += $(COMPILER_SANITIZERS) $(COMPILER_OPTIMIZATION_LEVEL_DEBUG)
endif

# Compile up linker flags
LDFLAGS += $(DIAGNOSTIC_FLAGS)


############################# The Rules & Recipes ##############################

######################### Lib Rules ########################
# Build the static library files
.PHONY: collection
collection: $(BUILD_DIRS) $(LIB_FILE) $(LIB_LIST_FILE)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[32mCollection Library \033[35m$(LIB_FILE) \033[32;1mbuilt\033[0m!"
	@echo "----------------------------------------"

.PHONY: lib
# Build the static library files
lib: $(BUILD_DIRS) $(LIB_FILE) $(LIB_LIST_FILE)
	@echo
	@echo "----------------------------------------"
	@echo -e "Library \033[35m$(LIB_FILE) \033[32;1mbuilt\033[0m!"
	@echo "----------------------------------------"

$(LIB_FILE): $(DS_OBJ_FILES) $(BUILD_DIRS) 
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mConstructing\033[0m the static library: $@..."
	@echo
	ar rcs $@ $<

######################## Test Rules ########################
_test: $(BUILD_DIRS) $(TEST_EXECUTABLES) $(LIB_FILE) $(TEST_LIST_FILE) $(RESULTS)
	@echo
	@echo -e "\033[36mAll tests completed!\033[0m"
	@echo

# Write the test results to a result .txt file
$(PATH_RESULTS)%.txt: $(PATH_BUILD)%.$(TARGET_EXTENSION) $(COLORIZE_UNITY_SCRIPT)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mRunning\033[0m $<..."
	@echo
	-./$< 2>&1 | tee $@ | python $(COLORIZE_UNITY_SCRIPT)

$(PATH_BUILD)%.$(TARGET_EXTENSION): $(TEST_OBJ_FILES) $(UNITY_OBJ_FILES) $(LIB_FILE)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mLinking\033[0m $(TEST_OBJ_FILES), $(UNITY_OBJ_FILES), and the collection static lib $(LIB_FILE) into an executable..."
	@echo
	$(CC) $(LDFLAGS) $(TEST_OBJ_FILES) $(UNITY_OBJ_FILES) -L$(PATH_BUILD) -l$(basename $(notdir $(LIB_FILE))) -o $@

$(PATH_OBJECT_FILES)%.o: $(PATH_TEST_FILES)%.c $(COLORIZE_CPPCHECK_SCRIPT)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mCompiling\033[0m the test file: $<..."
	@echo
	$(CC) -c $(CFLAGS_TEST) $< -o $@
	@echo

$(PATH_OBJECT_FILES)%.o: $(PATH_UNITY)%.c $(PATH_UNITY)%.h
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mCompiling\033[0m the unity file: $<..."
	@echo
	$(CC) -c $(CFLAGS_TEST) $< -o $@
	@echo

.PHONY: unity_static_analysis
unity_static_analysis: $(PATH_UNITY)unity.c $(COLORIZE_CPPCHECK_SCRIPT)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mRunning static analysis\033[0m on $<..."
	@echo
	cppcheck --template='{severity}: {file}:{line}: {message}' $< 2>&1 | tee $(PATH_BUILD)cppcheck.log | python $(COLORIZE_CPPCHECK_SCRIPT)

######################### Generic ##########################

# Compile the collection source file into an object file
$(PATH_OBJECT_FILES)%.o : $(PATH_SRC)%.c $(PATH_INC)%.h $(PATH_CFG)%_cfg.h $(COLORIZE_CPPCHECK_SCRIPT)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mCompiling\033[0m the collection source file: $<..."
	@echo
	$(CC) -c $(CFLAGS) $< -o $@
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mRunning static analysis\033[0m on $<..."
	@echo
	cppcheck --template='{severity}: {file}:{line}: {message}' $< 2>&1 | tee $(PATH_BUILD)cppcheck.log | python $(COLORIZE_CPPCHECK_SCRIPT)

$(LIB_LIST_FILE): $(LIB_FILE)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mDisassembly\033[0m of $< into $@..."
	@echo
	objdump -D $< > $@

# Produces an object dump that includes the disassembly of the executable
$(PATH_BUILD)%.lst: $(PATH_BUILD)%.$(TARGET_EXTENSION)
	@echo
	@echo "----------------------------------------"
	@echo -e "\033[36mDisassembly\033[0m of $< into $@..."
	@echo
	objdump -D $< > $@

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
	@echo
	$(CLEANUP) $(PATH_OBJECT_FILES)*.o
	$(CLEANUP) $(PATH_BUILD)*.$(TARGET_EXTENSION)
	$(CLEANUP) $(PATH_RESULTS)*.txt
	$(CLEANUP) $(PATH_BUILD)*.lst
	$(CLEANUP) $(PATH_BUILD)*.log
	$(CLEANUP) $(PATH_BUILD)*.$(STATIC_LIB_EXTENSION)
	@echo

.PRECIOUS: $(PATH_BUILD)%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATH_BUILD)Test%.o
.PRECIOUS: $(PATH_RESULTS)%.txt
.PRECIOUS: $(PATH_RESULTS)%.lst
