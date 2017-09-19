/*
Copyright (C) 2017 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#ifndef UWINDOWS_H
#define UWINDOWS_H

typedef struct window {
    uint32_t index;
    uint32_t data[256];
    struct window* next;

} window_t;

window_t* window_create();
void window_free(window_t*);
void window_line(window_t*, uint32_t, uint32_t, uint32_t, uint32_t);
void window_text(window_t*, char*, uint32_t, uint32_t);

#endif
