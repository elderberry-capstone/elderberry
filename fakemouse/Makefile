CC:=g++
IN:=fakemouse.cpp
OUT:=fakemouse
LINK:=`pkg-config --libs libusb-1.0`
CC11:=-std=c++11
all:
	$(CC) -o $(OUT) $(IN) $(LINK) $(CC11)
clean:
	rm *.o $(OUT)
debug:
	$(CC) -o $(OUT) $(IN) $(LINK) -g $(CC11)
