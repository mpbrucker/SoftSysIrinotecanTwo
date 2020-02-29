pcm : pcm.o
		cc -o pcm pcm.o -lasound
keyInput : keyInput.o
		cc -o keyInput keyInput.o -lcurses


pcm.o : pcm.c
		cc -c pcm.c
keyInput.o : keyInput.c
		cc -c keyInput.c
clean :
		rm pcm keyInput pcm.o keyInput.o