.PHONY: install clean
CC=gcc
CPP=g++
#LDFLAGS = -ggdb -static
LIBS = -lgmp
OBJS=cardi.o mer_twist.o
#LDFLAGS = -lgmpxx -lgmp #-static  -ggdb  -L/home/tpyhala/lib
LDFLAGS = 

all: gsat_cardi
gsat_cardi: cardi.o mer_twist.o; $(CPP) $(LDFLAGS) -o gsat_cardi cardi.o mer_twist.o 
install: gsat_cardi; install gsat_cardi $(TARGETDIR) 
deps: ; gcc -MM *.cpp >> deps
clean: ; rm -f gsat_cardi *.o *~ deps
include deps





