CFLAGS=-g -fsanitize=address,leak -c
CC = g++

all: processor assembler disassembler

processor: processor.o processor.h stack.o stack.h commands.h
	$(CC) -fsanitize=address,leak processor.o stack.o -o processor

assembler: assembler.o assembler.h commands.h
	$(CC) -fsanitize=address,leak assembler.o -o assembler

disassembler: disassembler.o commands.h disassembler.h
	$(CC) -fsanitize=address,leak disassembler.o -o disassembler

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