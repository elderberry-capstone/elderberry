#OPTS := -O3 -ffast-math $(call cc-option,-flto -fwhole-program)
#WARNINGS := -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings -Wno-missing-field-initializers -Wno-unused-parameter
#CFLAGS := -g -MD -std=gnu99 $(OPTS) $(WARNINGS) -fno-strict-aliasing $(shell pkg-config --cflags glib-2.0 libusb-1.0)
#LDLIBS := -lrt $(shell pkg-config --libs glib-2.0 libusb-1.0)

CC := gcc
WARNINGS := -Wall
CFLAGS := -lnsl -lresolv -lrt -pthread $(WARNINGS)
CDEBUG := -g -O0 $(CFLAGS)
OUT := profile

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
profiler:
	$(CC) -o $(OUT) $(CFLAGS)

profilerg:
	$(CC) -o $(OUT) $(DEBUG)

clean:
	rm -f *.o fc hwemu1 fc profile testrun
