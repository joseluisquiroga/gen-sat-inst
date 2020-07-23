.PHONY: install clean
CC=gcc
CPP=g++
#LDFLAGS = -ggdb -static
LIBS = -lgmp
OBJS=gen.o num-gen.o
LDFLAGS = -lgmpxx -lgmp #-static  -ggdb  -L/home/tpyhala/lib

all: gsat_is_prime
gsat_is_prime: adder.o braun-multiplier.o output-format.o print_dimacs.o is_prime.o; $(CPP) $(LDFLAGS) -o gsat_is_prime adder.o braun-multiplier.o output-format.o print_dimacs.o is_prime.o
install: gsat_is_prime; install gsat_is_prime $(TARGETDIR) 
deps: ; gcc -MM *.cpp *.c >> deps
clean:; rm -f gsat_is_prime *.o *~ deps
include deps





