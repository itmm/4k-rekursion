.PHONY = all clean

all: ./rekursion
	./rekursion

clean:
	rm -f ./rekursion

rekursion.cpp: rekursion.md
	mdp $^
	./limit.sh $^

CXXFLAGS += -Wall -std=c++17
