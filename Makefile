all: sim

sim: List.o PCB.o main.o
	gcc List.o PCB.o main.o -o sim

List.o: List.c List.h
	gcc -c List.c

PCB.o: PCB.c PCB.h List.h
	gcc -c PCB.c

main.o: main.c List.h PCB.h
	gcc -c main.c

clean:
	rm -f sim *.o