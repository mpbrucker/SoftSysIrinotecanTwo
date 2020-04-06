XLIB := $(shell pkg-config --cflags --libs x11)

Irinotecan : Irinotecan.o pcm.o keyInput.o
	cc -o Irinotecan Irinotecan.o pcm.o keyInput.o -lasound -lm -lpthread $(XLIB)
pcm : pcm.o
		cc -o pcm pcm.o -lasound -lm
keyInput : keyInput.o
		cc -o keyInput keyInput.o $(XLIB) -lpthread


Irinotecan.o : Irinotecan.c
	cc -c Irinotecan.c
pcm.o : pcm.c
		cc -c pcm.c
keyInput.o : keyInput.c
		cc -c keyInput.c
clean :
		rm pcm keyInput Irinotecan pcm.o keyInput.o Irinotecan.o