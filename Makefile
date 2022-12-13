# C Compiler
CC=gcc
CCLIBS=-lc -lcrypto

# Executable Name
LBITs=$(shell getconf LONG_BIT)
ifeq ($(LBITs), 64)
	EXEC=tam.x64
else
	EXEC=tam.x86
endif

# File Sets
LIBOBJ=src/lib-src/decrypt.o src/lib-src/encrypt.o src/lib-src/func.o src/lib-src/readwrite.o
TAMOBJ=src/tam-src/main.o src/tam-src/tam.o

all: release
release: lib exec
debug: clean lib-dbg exec-dbg

# Library Release Compile Rules
lib: $(LIBOBJ) src/libtarim.h
	@echo "=> $(CC) -fPIC -shared $(LIBOBJ) -o libtarim.so $(CCLIBS)"
	$(shell $(CC) -fPIC -shared $(LIBOBJ) -o libtarim.so $(CCLIBS))
	@echo "(release) Library Compiled!"
	@echo ""

# Library Debug Compile Rules
lib-dbg: src/libtarim.h
	@echo "=> $(CC) -g -fPIC -shared src/lib-src/*.c -o libtarim.so $(CCLIBS)"
	$(shell $(CC) -g -fPIC -shared src/lib-src/*.c -o libtarim.so $(CCLIBS))
	@echo "(debug) Library Compiled!"
	@echo ""

# Executable Release Compile Rules
exec: $(TAMOBJ) src/libtarim.h src/tam-src/tam.h lib
	@echo "=> $(CC) $(TAMOBJ) -o $(EXEC) -L. -ltarim"
	$(shell $(CC) $(TAMOBJ) -o $(EXEC) -L. -ltarim)
	@echo "TAM (release) Compiled!"
	@echo "-----------------------"
	@echo ""

# Executable Debug Compile Rules
exec-dbg: src/libtarim.h src/tam-src/tam.h lib-dbg
	@echo "=> $(CC) -g src/tam-src/*.c -o $(EXEC) -L. -ltarim"
	$(shell $(CC) -g src/tam-src/*.c -o $(EXEC) -L. -ltarim)
	@echo "TAM (debug) compiled!"
	@echo "---------------------"
	@echo ""

# Library Object Rules
%.o: %.c
	@echo "-> $(CC) -c $^ -o $@"
	$(shell $(CC) -c $^ -o $@)

# Run Executable if Library not installed
.PHONY: run
run:
	@echo "Running Executable..."
	$(shell LD_LIBRARY_PATH="$(shell pwd):$LD_LIBRARY_PATH" ./$(EXEC))

# Clean Rules
clean:
	$(shell rm src/lib-src/*.o)
	$(shell rm libtarim.so libtarim-dbg.so)
	$(shell rm src/tam-src/*.o)
	$(shell rm $(EXEC))
	@echo "Project Cleaned!"
