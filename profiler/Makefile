INCLUDE=fc.c fcfmain.c fcfutils.h gps.h libusb-basic.h logging.h miml.h mouse2.h mouse.h socket.h testIMU.h testLoggerDisk.h testLoggerScreen.h virtdevsrv.h theo-imu.h
all:
	gcc fcfutils.h profile.c `pkg-config --libs --cflags libusb-1.0`
