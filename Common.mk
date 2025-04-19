.POSIX:
CROSS	= 
CC = $(CROSS)gcc
COMPILER_WARNING_FLAGS = -Wall -Wextra -Wconversion -Wdouble-promotion -Wnull-dereference -Wwrite-strings -Wformat=2 -Wcast-align=strict -Wswitch-enum -Wpedantic
# COMPILER_SANITIZERS =
COMPILER_SANITIZERS = -fsanitize=bool -fsanitize=undefined -fsanitize-trap
COMPILER_OPTIMIZATION_LEVEL = -Og -g3
COMPILER_OPTIMIZATION_LEVEL_SPEED = -O3
COMPILER_OPTIMIZATION_LEVEL_SPACE = -Os
CFLAGS = $(COMPILER_WARNING_FLAGS) $(COMPILER_SANITIZERS) $(COMPILER_OPTIMIZATION_LEVEL)
CFLAGS_FAST = $(COMPILER_WARNING_FLAGS) $(COMPILER_OPTIMIZATION_LEVEL_SPEED)
CFLAGS_SMALL = $(COMPILER_WARNING_FLAGS) $(COMPILER_OPTIMIZATION_LEVEL_SPACE)
LDFLAGS = 

# Ensure SOURCE_FILE is defined in the subfolder Makefile
ifndef SOURCE_FILE
$(error SOURCE_FILE is not defined. Please define it in the subfolder Makefile.)
endif

# Rules for building the executable, object file, and listing file

# Run it, baby!
.PHONY: run
run: $(SOURCE_FILE).exe $(SOURCE_FILE).lst	# Don't actually need the .lst file but want to force the disassembly generation
	@echo
	@echo "----------------------------------------"
	@echo "Running $<..."
	@echo
	./$(SOURCE_FILE).exe

# Produces an object dump that includes the disassembly of the executable
$(SOURCE_FILE).lst: $(SOURCE_FILE).exe
	@echo
	@echo "----------------------------------------"
	@echo "Disassembly of $<..."
	@echo
	objdump -D $< > $@

# Build the executable
$(SOURCE_FILE).exe: $(SOURCE_FILE).o
	@echo
	@echo "----------------------------------------"
	@echo "Linking $< into executable..."
	@echo
	$(CC) $(LDFLAGS) $< -o $@

# Build the object file and run static analysis against it
$(SOURCE_FILE).o: $(SOURCE_FILE).c
	@echo
	@echo "----------------------------------------"
	@echo "Compiling $<..."
	@echo
	$(CC) -c $(CFLAGS) $< -o $@
	@echo
	@echo "----------------------------------------"
	@echo "Running static analysis on $<..."
	@echo
	cppcheck --template='{severity}: {file}:{line}: {message}' $< 2>&1 | tee cppcheck.log | python ../../../colorize_cppcheck.py

# Clean rule to remove generated files
.PHONY: clean
clean:
	rm -f *.exe *.o *.lst
