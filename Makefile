CFLAGS=-g -fsanitize=address,leak -c
CFLAGS_OCOMPILE=-fsanitize=address,leak
CC = g++

all: processor assembler disassembler

processor: processor.o processor.h stack.o stack.h commands.h
	$(CC) $(CFLAGS_OCOMPILE) processor.o stack.o -o processor

assembler: assembler.o assembler.h commands.h
	$(CC) $(CFLAGS_OCOMPILE) assembler.o -o assembler

disassembler: disassembler.o commands.h disassembler.h
	$(CC) $(CFLAGS_OCOMPILE) disassembler.o -o disassembler

processor.o: processor.cpp processor.h commands.h
	$(CC) $(CFLAGS) processor.cpp

stack.o: stack.cpp stack.h
	$(CC) $(CFLAGS) stack.cpp

assembler.o: assembler.cpp assembler.h commands.h
	$(CC) $(CFLAGS) assembler.cpp

disassembler.o: disassembler.cpp commands.h disassembler.h
	$(CC) $(CFLAGS) disassembler.cpp

clean:
	rm -rf *.o stack