XLIB := $(shell pkg-config --cflags --libs x11)
GLIB := $(shell pkg-config --libs --cflags glib-2.0)

Irinotecan : Irinotecan.o pcm.o keyInput.o hashset.o hashset_itr.o
	cc -o Irinotecan Irinotecan.o pcm.o keyInput.o hashset.o hashset_itr.o -lasound -lm -lpthread $(XLIB)
pcm : pcm.o
		cc -o pcm pcm.o hashset.o hashset_itr.o -lasound -lm
keyInput : keyInput.o
		cc -o keyInput keyInput.o $(XLIB) -lpthread


hashset.o : hashset.c
	cc -c hashset.c
hashset_itr.o : hashset_itr.c
	cc -c hashset_itr.c
Irinotecan.o : Irinotecan.c
	cc -c Irinotecan.c
pcm.o : pcm.c
		cc -c pcm.c
keyInput.o : keyInput.c
		cc -c keyInput.c
clean :
		rm pcm keyInput Irinotecan pcm.o keyInput.o Irinotecan.o hashset.o hashset_itr.o