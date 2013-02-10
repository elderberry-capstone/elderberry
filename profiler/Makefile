CC:=g++
IN:=profile.cpp
TESTIN:=gettfds.cpp
TESTOUT:=gtfds
OUT:=profile
OPTIONS:= -Wall
DEBUG:=-g
INCLUDE:= -I /usr/lib/
LINK:= -lboost_program_options

all:
	$(CC) -o $(OUT) $(IN) $(INCLUDE) $(LINK) $(OPTIONS)
	
debug:
	$(CC) -o $(OUT) $(IN) $(INCLUDE) $(LINK) $(OPTIONS) $(DEBUG)

test:
	$(CC) -o $(TESTOUT) $(TESTIN)