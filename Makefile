.PHONY = all clean

APP = ./rekursion
SOURCE = ${APP:=.cpp}

all: ${APP}
	${APP}

clean:
	rm -f ${APP} ${SOURCE}

${SOURCE}: README.md
	mdp $^
	./limit.sh $^

CXXFLAGS += -Wall -std=c++17
