CFLAGS=-g -fsanitize=address,leak -c

all: processor assembler disassembler

processor: processor.o
	$(CC) -fsanitize=address,leak processor.o -o processor

assembler: assembler.o assembler.h
	$(CC) -fsanitize=address,leak assembler.o -o assembler

disassembler: disassembler.o
	$(CC) -fsanitize=address,leak disassembler.o -o disassembler

processor.o: processor.cpp
	$(CC) $(CFLAGS) processor.cpp

assembler.o: assembler.cpp assembler.h
	$(CC) $(CFLAGS) assembler.cpp

disassembler.o: disassembler.cpp
	$(CC) $(CFLAGS) disassembler.cpp

clean:
	rm -rf *.o stack