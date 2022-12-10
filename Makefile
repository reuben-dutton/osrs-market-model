.PHONY: clean default all

CXXFLAGS=-std=c++11 -Wall -O2 -pg -fopenmp
CXX=mpic++
HEADER=-I./shared -I./object

# The first rule in the Makefile is the default target that is made if 'make' is invoked with
# no parameters.  'all' is a dummy target that will make everything
default : all

all: osrsmodel

osrsmodel: object/main.o object/skills.o object/items.o object/market.o object/player.o object/activity.o object/strategies.o
	$(CXX) $(HEADER) $(CXXFLAGS) $^ -o $@

object/main.o: main.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/skills.o: shared/skills.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/items.o: shared/items.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/market.o: shared/market.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/player.o: shared/player.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/activity.o: shared/activity.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@

object/strategies.o: shared/strategies.cpp
	$(CXX) $(HEADER) $(CXXFLAGS) -c $^ -o $@


clean:
	rm -f osrsmodel ./object/*.o