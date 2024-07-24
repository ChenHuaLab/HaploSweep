objects = myfunction.o myutil.o iHS_soft.o
c = gcc -ggdb -w -O2
libs1=-lm -lz -lbz2 -lpthread
libs2=-lm -lz -lbz2 -lpthread -lgsl -lgslcblas
soft=HaploSweep

$(soft) : $(objects)
	$(c) -o $(soft) $(objects) $(libs1)
	cp $(soft) ../bin/

myfunction.o : myfunction.h myfunction.c
	$(c) -c myfunction.c
myutil.o : myutil.h myutil.c
	$(c) -c myutil.c
iHS_soft.o : iHS_soft.h iHS_soft.c
	$(c) -c iHS_soft.c

clean :
	rm -f $(soft) $(objects)
