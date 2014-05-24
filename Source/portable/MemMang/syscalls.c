/*
 * Provide the implementation for functions used in libc.a
 */
#include <sys/types.h>

extern unsigned long _ebss;

// actually _sbrk will not be used in this program, it is provided just to
// make sure that the whole program can be linked.
// typedef char * caddr_t
caddr_t _sbrk(int incr) {
    static unsigned char *heap = NULL;
    static unsigned long __HEAP_START = (unsigned long)&_ebss;
    static unsigned long __HEAP_END = (unsigned long)0x2002C000 \
                                      - (unsigned long)0x00010000;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *)&__HEAP_START;
    }
    prev_heap = heap;
    if (heap + incr > __HEAP_END) {
        /* heap reaches end */
        return (caddr_t)0;
    }

    heap += incr;
    return (caddr_t)prev_heap;
}

// provide definition for _write(), _read(), _close(), _read() to make
// linker silent
// I don't think any of functions used in this Demo actually calls any
// functions below. But it looks like if their definitions are not provided,
// the program cannot be linked....

int _write(int file, char *ptr, int len) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

int _close(int file) {
    return 0;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}
