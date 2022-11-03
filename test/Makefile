# C COMPILER
CC=gcc
CCFLAGS=

# Test Objects
TEST=readFile.o writeFile.o

all: $(TEST)

# Object Rules
%.o: %.c
	@echo "=> $(CC) $^ -o $@"
	$(shell $(CC) $^ -o $@)

# Clean Rules
.PHONY: clean
clean:
	$(shell rm *.o)
	@echo "Project Cleaned!"