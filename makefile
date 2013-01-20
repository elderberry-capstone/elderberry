#OPTS := -O3 -ffast-math $(call cc-option,-flto -fwhole-program)
#WARNINGS := -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings -Wno-missing-field-initializers -Wno-unused-parameter
#CFLAGS := -g -MD -std=gnu99 $(OPTS) $(WARNINGS) -fno-strict-aliasing $(shell pkg-config --cflags glib-2.0 libusb-1.0)
#LDLIBS := -lrt $(shell pkg-config --libs glib-2.0 libusb-1.0)

CC := g++
WARNINGS := -Wall
CFLAGS := -g $(WARNINGS)


all: fc

fc: fc.o fcfutils.o fcfmain.o testIMU.o testLoggerDisk.o testLoggerScreen.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	


clean:
	rm -f *.o fc hwemu1
