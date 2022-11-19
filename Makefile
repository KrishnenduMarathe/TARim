# C COMPILER
CC=gcc
CCFLAGS=-g

# Test Rules
.PHONY: test
test:
	$(shell cd test)
	$(shell mske)
	$(shell cd ..)