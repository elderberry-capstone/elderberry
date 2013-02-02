GCC := gcc
GWARNINGS := -Wall
GDEBUG := -g -O0

CIN := profiler.c
COUT := profile
CFLAGS := -lnsl -lresolv -lrt -pthread $(WARNINGS)

SC := gcc
SWARNINGS := -Wall
SFLAGS := -lnsl -lresolv -lrt -lHX -pthread -std=gnu99 $(WARNINGS)
SIN := server_listener.c
SOUT := listener
	
all:
	make client;
	make server;
	
debug:
	make debugclient;
	make debugserver;
	
client:
	$(GCC) $(CIN) -o $(COUT) $(CFLAGS)
	
server:
	$(GCC) $(SIN) -o $(SOUT) $(SFLAGS)
	
debugclient:
	$(GCC) $(CIN) -o $(COUT) $(CFLAGS) $(GDEBUG)
	
debugserver:
	$(GCC) $(SIN) -o $(SOUT) $(SFLAGS) $(GDEBUG)


clean:
	make cleanclient;
	make cleanserver;
	
cleanclient:
	rm $(COUT)

cleanserver:
	rm $(SOUT)
