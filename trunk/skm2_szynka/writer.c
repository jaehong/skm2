//! Pisarz szyny, najnizsza warstwa programu.
/*!
	\created: 2009-04-21
	\author: pejotr, mietek
*/
#include "writer.h"

char w_chSenderAddr;

int  writer_send( char* toSend, int size, int fd)
{
	int count;
	char d =  BEG_MARK;

	libser_flush( fd);

	if(libser_write( fd, &d, 1) != 1 )
		printf( "." );
	usleep(1000);


	if( ( count =  libser_write(fd, toSend, FRAME_SIZE)) != FRAME_SIZE ) {
		printf(".");
	}

	return count;
}

