CC:=g++
IN:=fakemouse.cpp
OUT:=fakemouse
LINK:=`pkg-config --libs libusb-1.0`
all:
	$(CC) -o $(OUT) $(IN) $(LINK)
clean:
	rm *.o $(OUT)
debug:
	$(CC) -o $(OUT) $(IN) $(LINK) -g
