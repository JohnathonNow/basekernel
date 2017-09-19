/*
Copyright (C) 2017 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "uwindows.h"
#include "kwindows.h"
#include "kmalloc.h"
#include "string.h"


window_t* window_create() {
    window_t* t = kmalloc(sizeof(window_t)); //Can't use kmalloc
    t.index = t.next = 0;
    return t;
}

void window_free(window_t* t) {
    kfree(t);
}

void window_line(window_t*, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
    t.data[t.index++] = LINE;
    t.data[t.index++] = x0;
    t.data[t.index++] = y0;
    t.data[t.index++] = x1;
    t.data[t.index++] = y1;
}

void window_text(window_t*, char* c, uint32_t x, uint32_t y) {
    t.data[t.index++] = TEXT;
    t.data[t.index++] = x;
    t.data[t.index++] = y;
    char* d = (char*)(t.data + t.index);
	while(*c) {
		*d++ = *c++;
	}
	*d++ = 0;
    t.index += 1+d-(char*)(t.data + t.index);
}
#endif
