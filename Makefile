.PHONY: all clean

SDIR = src
ODIR = bin

CC=g++
CPP_STANDARD=c++11
CFLAGS=-c -pthread -std=$(CPP_STANDARD) -ggdb
LFLAGS=-pthread -std=$(CPP_STANDARD) -ggdb

HEADERS = $(wildcard ${SDIR}/*.h) $(wildcard ${SDIR}/*.hpp)
OBJS = $(patsubst ${SDIR}/%.cpp,${ODIR}/%.o,$(wildcard ${SDIR}/*.cpp))

all: TP1

${ODIR}/%.o: ${SDIR}/%.cpp
	@mkdir -p ${ODIR}
	$(CC) $(CFLAGS) -c -o $@ $<

TP1: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o TP1

clean:
	rm -rf ${ODIR} TP1

# Usar esto para debugear vars de entorno en el makefile.
.PHONY: print-makefile-vars
print-makefile-vars:
	@echo -e "ODIR         = $(ODIR)"
	@echo -e "SDIR         = $(SDIR)"
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

testSecuencialCompleto2000: TP1
	./TP1 test/completo2000.txt secuencial

testk3: TP1
	./TP1 test/correctitud/k3.txt paralelo 2
	./TP1 test/correctitud/k3.txt paralelo 4
	./TP1 test/correctitud/k3.txt paralelo 8
	./TP1 test/correctitud/k3.txt paralelo 16

testk4: TP1
	./TP1 test/correctitud/k4.txt paralelo 2
	./TP1 test/correctitud/k4.txt paralelo 4
	./TP1 test/correctitud/k4.txt paralelo 8
	./TP1 test/correctitud/k4.txt paralelo 16

testOtroTest: TP1
	./TP1 test/correctitud/otro_test.txt paralelo 2
	./TP1 test/correctitud/otro_test.txt paralelo 4
	./TP1 test/correctitud/otro_test.txt paralelo 8
	./TP1 test/correctitud/otro_test.txt paralelo 16
