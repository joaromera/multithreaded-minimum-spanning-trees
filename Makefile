.PHONY: all clean

CC=g++
CPP_STANDARD=c++11
CFLAGS=-c -pthread -std=$(CPP_STANDARD)
LFLAGS=-pthread -std=$(CPP_STANDARD)

OBJS=grafo.o

all: TP1

TP1: $(OBJS) tp1_grafo.cpp
	$(CC) $(LFLAGS) $(OBJS) $^ -o TP1

grafo.o: grafo.h grafo.cpp
	$(CC) $(CFLAGS) grafo.cpp -o grafo.o

clean:
	rm $(OBJS)
	rm TP1

# Usar esto para debugear vars de entorno en el makefile.
.PHONY: print-makefile-vars
print-makefile-vars:
	@echo -e "CC           = $(CC)"
	@echo -e "CPP_STANDARD = $(CPP_STANDARD)"
	@echo -e "CFLAGS       = $(CFLAGS)"
	@echo -e "LFLAGS       = $(LFLAGS)"
	@echo -e "OBJS         = $(OBJS)"
	@echo -e "input        = $(input)"
	@echo -e "threads      = $(threads)"

test: TP1
	./TP1 $(input)

testParaleloVacio: TP1
	./TP1 test/vacio.txt paralelo $(threads)

testParaleloTrivial: TP1
	./TP1 test/trivial.txt paralelo $(threads)

testParaleloSimple: TP1
	./TP1 test/simple.txt paralelo $(threads)

testParaleloArbol1000: TP1
	./TP1 test/arbol1000.txt paralelo $(threads)

testParaleloRandom1000: TP1
	./TP1 test/random1000.txt paralelo $(threads)

testParaleloCompleto1000: TP1
	./TP1 test/completo1000.txt paralelo $(threads)

testParaleloCompleto2000: TP1
	./TP1 test/completo2000.txt paralelo $(threads)

testSecuencialVacio: TP1
	./TP1 test/vacio.txt secuencial

testSecuencialTrivial: TP1
	./TP1 test/trivial.txt secuencial

testSecuencialSimple: TP1
	./TP1 test/simple.txt secuencial

testSecuencialArbol1000: TP1
	./TP1 test/arbol1000.txt secuencial

testSecuencialRandom1000: TP1
	./TP1 test/random1000.txt secuencial

testSecuencialCompleto1000: TP1
	./TP1 test/completo1000.txt secuencial

testSecuencialCompleto2000: TP1
	./TP1 test/completo2000.txt secuencial
