#include "keyInput.h"
#include <X11/Xlib.h> // Every Xlib program must include this
#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>   // So we got the profile for 10 seconds
#include <stdio.h>    // Printing
#include <stdlib.h>
#include <signal.h>


#define NIL (0)

int currkey = -1;
static volatile int running = 1;

void killKeyCapture(){
    running = 0;
}

void * keyCapture() {

    signal(SIGINT, killKeyCapture);

    //Xlib Attempt
    Display * dpy = XOpenDisplay(NIL);
    assert(dpy);

    int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
    int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
    
    // Create the window

    Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
                    200, 100, 0, blackColor, blackColor);

    // We want to get MapNotify events
    XSelectInput(dpy, w, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    // "Map" the window (that is, make it appear on the screen)
    XMapWindow(dpy, w);
    
    // Create a "Graphics Context"
    GC gc = XCreateGC(dpy, w, 0, NIL);

    // Tell the GC we draw using the white color
    XSetForeground(dpy, gc, whiteColor);

    // Wait for the MapNotify event from the server as a response to our XMapWindow call (?)
    for(;;) {
        XEvent e;
        XNextEvent(dpy, &e);
        //printf("%i\n", e.type);
        if (e.type == MapNotify)
            break;
    }

    // Draw a line
    XDrawLine(dpy, w, gc, 10, 60, 180, 20);

    // Send the "DrawLine" request to the server
    XFlush(dpy);

    // Turn AutoRepeat Off
    XAutoRepeatOff(dpy);

    // Signal Catch program kill
    

    // Read incoming key events
    while(running) {
        XEvent e;
        XNextEvent(dpy, &e);
        if (e.type == KeyPress){
            setKey(e.xkey.keycode);
            printf("pressed key: %u\n", e.xkey.keycode);
        }
        if (e.type == KeyRelease){
            releaseKey(e.xkey.keycode);
            printf("released key: %u\n", e.xkey.keycode);
        }
        //printf("CK: %i\n", currkey);
    }
    XAutoRepeatOn(dpy);
    XCloseDisplay(dpy);
    printf("HERE PCM");
    return NULL;

    //Ncurses Attempt
    /*initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    notimeout(stdscr, TRUE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    int ch;
    while (1){
        if ((ch = getch()) == ERR){
            // User hasn't hit a key yet
        }else{
            // User has hit a key
            waddch(stdscr, ch | A_UNDERLINE | COLOR_PAIR(3));
            wrefresh(stdscr);
        }
    }


    endwin();*/
}