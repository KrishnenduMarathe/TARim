# C COMPILER
CC=gcc
CCFLAGS=-g -lcrypto

# Test Objects
TEST=aes.o aria.o camellia.o readFile.o writeFile.o

all: $(TEST)

# Object Rules
%.o: %.c
	@echo "=> $(CC) $^ -o $@ $(CCFLAGS)"
	$(shell $(CC) $^ -o $@ $(CCFLAGS))

# Clean Rules
.PHONY: clean
clean:
	$(shell rm *.o)
	@echo "Test Cleaned!"