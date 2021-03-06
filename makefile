cc=mpicc

all: mpi_solved1 mpi_solved2 mpi_solved3 mpi_solved4 mpi_solved5 mpi_solved6 mpi_solved7 ssort

mpi_solved1: mpi_solved1.c
	$(cc) mpi_solved1.c -o mpi_solved1

mpi_solved2: mpi_solved2.c
	$(cc) mpi_solved2.c -o mpi_solved2

mpi_solved3: mpi_solved3.c
	$(cc) mpi_solved3.c -o mpi_solved3

mpi_solved4: mpi_solved4.c
	$(cc) mpi_solved4.c -o mpi_solved4

mpi_solved5: mpi_solved5.c
	$(cc) mpi_solved5.c -o mpi_solved5

mpi_solved6: mpi_solved6.c
	$(cc) mpi_solved6.c -o mpi_solved6

mpi_solved7: mpi_solved7.c
	$(cc) mpi_solved7.c -o mpi_solved7

ssort: ssort.c
	$(cc) ssort.c -o ssort

clean:
	rm mpi_solved1 mpi_solved2 mpi_solved3 mpi_solved4 mpi_solved5 mpi_solved6 mpi_solved7 ssort