/*
Copyright (C) 2017 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#ifndef SYSCALLS_H
#define SYSCALLS_H

void debug( const char *str );
void exit( int status );
int yield();
int run( const char *cmd );
int wait();
int open( const char *path, int mode, int flags );
int read( int fd, void *data, int length );
int write( int fd, void *data, int length );
int lseek( int fd, int offset, int whence );
int close( int fd );
int w_color( int wd, int r, int g, int b );
int w_rect( int wd, int x, int y, int w, int h );
int w_clear( int wd, int x, int y, int w, int h );
int w_line( int wd, int x, int y, int w, int h );
int w_char( int wd, int x, int y, char c );
int w_string( int wd, int x, int y, char *s );

#endif
