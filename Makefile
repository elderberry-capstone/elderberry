OPTSLIVE := -O3 
OPTSDEV  := -g
OPTSPROF := -O3 -pg
OPTS     := -ffast-math $(call cc-option,-flto -fwhole-program)
WARNINGS := -Werror -Wall -Wextra -Wmissing-prototypes -Wwrite-strings -Wno-missing-field-initializers -Wno-unused-parameter
CFLAGS   := -MD -std=gnu99 $(OPTS) $(WARNINGS) -fno-strict-aliasing $(shell pkg-config --cflags libusb-1.0)
LDLIBS   := -lrt $(shell pkg-config --libs libusb-1.0)
.DEFAULT_GOAL := all

MAINMIML ?= Main.miml
MIMLMK   ?= Miml.mk

include $(MIMLMK)


all: CFLAGS += $(OPTSLIVE)
all: fc


debug: CFLAGS  += $(OPTSDEV)
debug: LDFLAGS += $(OPTSDEV)
debug: fc


prof: CFLAGS  += $(OPTSPROF)
prof: LDFLAGS += $(OPTSPROF)
prof: fc


fc: $(OBJECTS)


miml:
	codeGen.py -m $(MIML)		#generates Miml.mk


clean:
	rm -f *.o *.d fc core
	# rm $(MIMLMK)

