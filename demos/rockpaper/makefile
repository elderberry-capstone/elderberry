CC := gcc 
WARNINGS := -Wall
CFLAGS := -g -std=c99  $(pkg-config --cflags libusb-1.0) $(WARNINGS)
LIBS := 


all: fc

fc: fc.o fcfutils.o fcfmain.o game.o testLoggerDisk.o testLoggerScreen.o mouse.o mouse2.o -lusb-1.0

%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS) 


clean:
	rm -f *.o fc hwemu1
