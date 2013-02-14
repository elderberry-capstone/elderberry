#OPTS := -O3 -ffast-math $(call cc-option,-flto -fwhole-program)
WARNINGS := -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings -Wno-missing-field-initializers -Wno-unused-parameter
CFLAGS := -g -MD -std=gnu99 $(OPTS) $(WARNINGS) -fno-strict-aliasing $(shell pkg-config --cflags libusb-1.0) -pg
LDLIBS := -lrt $(shell pkg-config --libs libusb-1.0)

all: fc

fc: fc.o logging.o libusb-basic.o theo-imu.o gps.o mouse.o mouse2.o socket.o virtdevsrv.o miml.o profile.o

clean:
	rm -f *.o *.d fc core
