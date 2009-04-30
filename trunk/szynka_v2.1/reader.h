//! Czytacz szyny, najnizsza warstwa programu.
/*!
	Czytacz szyny uruchomiony jest w oddzielnym watku,
	wszystkie odczytane dane umieszcza w swoim wewnetrznym buforze.

	\created: 2009-04-21
	\author: pejotr
*/

#ifndef READER_H_
#define READER_H_

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#include "libser.h"

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

void  reader_start(int libserFd, int );
void  reader_stop();
void  reader_pause();
int   reader_write(char* destBuffer, int bytes);
void* reader_read(void*);



#endif /* READER_H_ */
