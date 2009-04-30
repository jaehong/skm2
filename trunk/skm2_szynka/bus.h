//! Opakowanie najnizeszej warstwy programu w uzyteczne funkcje
/*!
	\created: 2009-04-21
	\author: pejotr, mietek
*/

#ifndef BUS_H_
#define BUS_H_

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "glo.h"
#include "libser.h"
#include "reader.h"
#include "frame.h"
#include "writer.h"

//pthread_t     b_thReaderThread;

struct frame* bus_pickframe();
void*         bus_start(void*);
void          bus_stop();
void          bus_putframe(char ,char ,char ,char* );
int 	      getLibserFd();
//void*         slave_run(void*);
void setFd(int fd); //dla celow testowych -> slave2
void setFrameNumber(bool num, char* flags); //ustawia numer ramki
void setFrameFlag(int, char*); //ustawia flage ramki


#endif /* SLAVE_H_ */
