CC=g++
CFLAGS=-O3 -march=native -std=c++17 -Wall -pedantic


yac_sim: yac_sim.o
	$(CC) $(CFLAGS) -o $@ $^

yac_sim.o: yac_sim.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

cleanall: clean
	rm -rf GPATH GTAGS GRTAGS

clean:
	rm -rf yac_sim yac_sim.o *~
