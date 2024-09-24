objects = myfunc.o iHS_soft.o
c = gcc -ggdb -w -O2
libs1=-lm -lz -lpthread
libs2=-lm -lz -lpthread -lgsl -lgslcblas
soft=HaploSweep

$(soft) : $(objects)
	$(c) -o $(soft) $(objects) $(libs1)

myfunc.o : myfunc.h myfunc.c
	$(c) -c myfunc.c
iHS_soft.o : iHS_soft.h iHS_soft.c
	$(c) -c iHS_soft.c

clean :
	rm -f $(soft) $(objects)
