#OPTS := -O3 -ffast-math $(call cc-option,-flto -fwhole-program)
#WARNINGS := -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings -Wno-missing-field-initializers -Wno-unused-parameter
#CFLAGS := -g -MD -std=gnu99 $(OPTS) $(WARNINGS) -fno-strict-aliasing $(shell pkg-config --cflags glib-2.0 libusb-1.0)
#LDLIBS := -lrt $(shell pkg-config --libs glib-2.0 libusb-1.0)

CC := gcc 
WARNINGS := -Wall
CFLAGS := -g -std=gnu99  $(pkg-config --cflags libusb-1.0) $(WARNINGS) -pg
LIBS := 


all: fc

<<<<<<< Updated upstream
fc: fcfutils.o fcfmain.o  module_testLoggerScreen.o module_mouse_clark.o module_mouse_clark2.o module_mouse_jm.o module_mouse_jm2.o module_profile.o module_gps.o module_theo-imu.o utils_libusb-1.0.o -lusb-1.0
=======
fc: fcfutils.o fcfmain.o module_testIMU.o module_testLoggerDisk.o module_testLoggerScreen.o module_mouse.o module_mouse3.o utils_libusb-1.0.o -lusb-1.0
>>>>>>> Stashed changes

%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS) 


clean:
	rm -f *.o fc hwemu1
