# C Compiler
CC=gcc
CCLIBS=-lc -lcrypto

# Detect Operating System Between Mac and Linux
os=$(shell uname)

# Executable Name
LBITs=$(shell getconf LONG_BIT)

ifeq ($(LBITs), 64)
	EXEC=tam.x64
	LIB=libtarim.x64.so
	LINK=ltarim.x64

else
	EXEC=tam.x86
	LIB=libtarim.x86.so
	LINK=ltarim.x86
endif

# File Sets
LIBOBJ=src/lib-src/decrypt.o src/lib-src/encrypt.o src/lib-src/func.o src/lib-src/readwrite.o
TAMOBJ=src/tam-src/main.o src/tam-src/tam.o

all: release
release: lib exec
debug: lib-dbg exec-dbg

# Object Rules
%.o: %.c
	@echo "-> $(CC) -c -fPIC $^ -o $@"
	$(shell $(CC) -c -fPIC $^ -o $@)

# Library Release Compile Rules
lib: $(LIBOBJ) src/libtarim.h
	@echo "=> $(CC) -fPIC -shared $(LIBOBJ) -o $(LIB) $(CCLIBS)"
	$(shell $(CC) -fPIC -shared $(LIBOBJ) -o $(LIB) $(CCLIBS))
	@echo ""
	@echo "+-----------------------------+"
	@echo "| Library (release) Compiled! |"
	@echo "+-----------------------------+"
	@echo ""

# Library Debug Compile Rules
lib-dbg: src/libtarim.h
	@echo "=> $(CC) -g -fPIC -shared src/lib-src/*.c -o $(LIB) $(CCLIBS)"
	$(shell $(CC) -g -fPIC -shared src/lib-src/*.c -o $(LIB) $(CCLIBS))
	@echo ""
	@echo "+---------------------------+"
	@echo "| Library (debug) Compiled! |"
	@echo "+---------------------------+"
	@echo ""

# Executable Release Compile Rules
exec: src/tam-src/main.c src/tam-src/tam.c src/libtarim.h src/tam-src/tam.h lib
	@echo "-> $(CC) -c src/tam-src/main.c -o src/tam-src/main.o"
	$(shell $(CC) -c src/tam-src/main.c -o src/tam-src/main.o)
	@echo "-> $(CC) -c src/tam-src/tam.c -o src/tam-src/tam.o"
	$(shell $(CC) -c src/tam-src/tam.c -o src/tam-src/tam.o)
	@echo "=> $(CC) $(TAMOBJ) -o $(EXEC) -L. -$(LINK)"
	$(shell $(CC) $(TAMOBJ) -o $(EXEC) -L. -$(LINK))
	@echo ""
	@echo "+=========================+"
	@echo "| TAM (release) Compiled! |"
	@echo "+=========================+"
	@echo ""

# Executable Debug Compile Rules
exec-dbg: src/tam-src/main.c src/tam-src/tam.c src/libtarim.h src/tam-src/tam.h lib-dbg
	@echo "-> $(CC) -c -g src/tam-src/main.c -o src/tam-src/main.o"
	$(shell $(CC) -c -g src/tam-src/main.c -o src/tam-src/main.o)
	@echo "-> $(CC) -c -g src/tam-src/tam.c -o src/tam-src/tam.o"
	$(shell $(CC) -c -g src/tam-src/tam.c -o src/tam-src/tam.o)
	@echo "=> $(CC) -g $(TAMOBJ) -o $(EXEC) -L. -$(LINK)"
	$(shell $(CC) -g $(TAMOBJ) -o $(EXEC) -L. -$(LINK))
	@echo ""
	@echo "+=======================+"
	@echo "| TAM (debug) compiled! |"
	@echo "+=======================+"
	@echo ""

# Run Executable if Library not installed
.PHONY: run
run:
	@echo ""
	@echo "Running Executable using command locally:"
	@echo "LD_LIBRARY_PATH='$(shell pwd):$LD_LIBRARY_PATH' ./$(EXEC)"
	@echo ""
	@echo "Run [make install] to install to the system"

# Install Rules
.PHONY:
install:
	@echo ""
	$(shell sudo cp $(EXEC) /usr/local/bin)
	$(shell sudo cp $(LIB) /usr/local/lib)
	@echo "Exporting Library Path for this terminal session..."
	$(shell export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib)
	@echo ""
	@echo "Remember to add following lines to .bashrc/.zshrc file to avoid providing library location everytime:"
	@echo "    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib"
	@echo ""
	@echo "+===========================+"
	@echo "| TAM & LIBTARIM Installed! |"
	@echo "+===========================+"
	@echo ""

# Uninstall Rules
.PHONY: uninstall
uninstall:
	@echo ""
	$(shell sudo rm /usr/local/bin/$(EXEC))
	$(shell sudo rm /usr/local/lib/$(LIB))
	@echo "Remember to remove following lines from .bashrc/.zshrc file:"
	@echo "    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib"
	@echo ""
	@echo "+=============================+"
	@echo "| TAM & LIBTARIM Uninstalled! |"
	@echo "+=============================+"
	@echo ""

# Clean Rules
clean:
	$(shell rm src/lib-src/*.o)
	$(shell rm $(LIB))
	$(shell rm src/tam-src/*.o)
	$(shell rm $(EXEC))
	@echo ""
	@echo "+------------------+"
	@echo "| Project Cleaned! |"
	@echo "+------------------+"
	@echo ""
