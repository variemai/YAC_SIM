CPP=g++
CPPFLAGS=-std=c++14 -Wall -pedantic

yac_sim: yac_sim.o
	$(CPP) $(CPPFLAGS) -o yac_sim yac_sim.o

yac_sim.o:
	$(CPP) $(CPPFLAGS) -c yac_sim.cpp
clean:
	rm -rf yac_sim yac_sim.o
