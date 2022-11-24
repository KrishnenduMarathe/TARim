# C Compiler
CC=gcc
CCFLAGS=
CCLIBS=-lc -lcrypto

# Executable Name
LBITs=$(shell getconf LONG_BIT)
ifeq ($(LBITs), 64)
	EXEC=tam.x64
else
	EXEC=tam.x86
endif

# Object Sets
LIBOBJ=src/lib-src/crypt.o src/lib-src/func.o src/lib-src/readwrite.o
TAMOBJ=

all: lib $(EXEC)

# Library Compile Rules
lib: $(LIBOBJ) src/libtarim.h
	@echo "=> $(CC) $(CCFLAGS) -fPIC -shared $(LIBOBJ) -o libtarim.so $(CCLIBS)"
	$(shell $(CC) $(CCFLAGS) -fPIC -shared $(LIBOBJ) -o libtarim.so $(CCLIBS))
	@echo "Library Compiled!"

# Executable Compile Rules
$(EXEC): $(TAMOBJ) src/libtarim.h
	@echo "Dummy Rule"

# Object Rules
%.o: %.c
	@echo "-> $(CC) -c $^ -o $@"
	$(shell $(CC) -c $^ -o $@)

# Run Executable if Library not installed
.PHONY: run
run: libtarim
	@echo "Running Executable..."
	$(shell LD_LIBRARY_PATH="$(shell pwd):$LD_LIBRARY_PATH" ./$(EXEC))

# Clean Rules
clean:
	$(shell rm src/lib-src/*.o)
	$(shell rm libtarim.so)
	$(shell rm src/tam-src/*.o)
	$(shell rm $(EXEC))
	@echo "Project Cleaned!"