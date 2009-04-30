#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

#include "libser.h"


/*
    Funkcja otwira port szeregowy. Port jest otwierany z w³¹czonym sprzêtowym sterowaniem 
    przep³ywem (RTS/CTS) w celu poprawnego sterowania w³¹czaniem nadajnika konwertera RS232 -> RS485.

    Parametry: device - nazwa urz¹dzenia - portu szeregowego (ttyS0 dla COM1, ttyS1 dla COM2, ...),
               speed - szybkoœæ komunikacji (B9600, B115200, ...).
    Wynik: >= 0 - uchwyt do pliku (port szeregowy),
           < 0  - b³¹d.
*/
int libser_open( const char * device, int speed ) 
{
	int fd;
	struct termios tio;
	if ((fd = open(device, O_RDWR | O_EXCL | O_SYNC)) < 0)
		return -1;
    libser_setrts(fd, RTS_OFF); // z TEST
    cfmakeraw(&tio);
	tio.c_cflag = CS8 | CREAD | CLOCAL | CRTSCTS;
	cfsetispeed( & tio, speed );
	cfsetospeed( & tio, speed );
	if( tcflush( fd, TCIFLUSH ) < 0 )
		return -2;
	if( tcsetattr( fd, TCSADRAIN, &tio ) < 0 )
		return -3;
	libser_flush( fd );
	return fd;
}

/*
    Funkcja zamyka port szeregowy.

    Parametry: fd - uchwyt do pliku (zwrócony przez funkcjê libser_open).
    Wynik: brak.
*/
void libser_close( int fd ) 
{
    libser_setrts(fd, RTS_OFF); // z TEST
	close( fd );
}

int libser_setrts( int fd, int onoff )
{
	int serflag = TIOCM_RTS;
	return ioctl( fd, onoff ? TIOCMBIS : TIOCMBIC, & serflag );
}

int serial_isempty( int fd ) 
{
    int status;
    if( ioctl( fd, TIOCSERGETLSR, & status, sizeof( status ) ) < 0 )
	    return -1;				
    if( status == TIOCSER_TEMT )
	    return 1;
    return 0;
}

/*
    Funkcja odczytuje podan¹ liczbê bajtów z portu szeregowego.
    Odczytana liczba bajtów mo¿e byæ mniejsza od ¿¹danej (timeout).

    Parametry: fd - uchwyt do pliku (zwrócony przez funkcjê libser_open).
               buff - wskazanie na bufor, gdzie zostan¹ zapisane odczytane bajty,
               len - liczba bajtów do odczytania,
               tv - struktura okreœlaj¹ca timeout.
   Wynik: liczba odczytanych bajtów.
*/
int libser_read( int fd, char * buff, size_t len, struct timeval * tv ) 
{
	int cnt;
	fd_set rfds;
	FD_ZERO( & rfds );
	FD_SET( fd, & rfds );
	for( cnt = 0; cnt < len; cnt++ ) 
    {
    	struct timeval _tv;
    	_tv.tv_sec	= tv->tv_sec;
    	_tv.tv_usec	= tv->tv_usec;

		if( select( fd + 1, & rfds, NULL, NULL, & _tv ) <= 0 )
			break;
		if( read( fd, & buff[ cnt ], 1 ) < 0 )
			break;
	}
	return cnt;
}


/*
    Funkcja opró¿nia bufor wejœciowy.

    Parametry: fd - uchwyt do pliku (zwrócony przez funkcjê libser_open).
    Wynik: brak.
*/
void libser_flush( int fd )
{
	char c;
	struct timeval tv;
    do 
    {
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
    }
    while( libser_read( fd, & c, 1, & tv ) > 0 );
}

/*
    Funkcja zapisuje podan¹ liczbê bajtów do portu szeregowego.

    Parametry: fd - uchwyt do pliku (zwrócony przez funkcjê libser_open).
               buff - wskazanie na bufor zawieraj¹cy dane do wys³ania,
               count - liczba bajtów do wys³ania.
    Wynik: liczba wys³anych bajtów.
*/
int libser_write( int fd, const void * buff, size_t count ) 
{
/*
	ssize_t cnt;
   	cnt=write( fd, buff, count );
    return cnt;
*/
    // z TEST
	ssize_t cnt;
	libser_setrts(fd, RTS_ON);
    cnt=write(fd, buff, count);
    while(!serial_isempty(fd));
	libser_setrts(fd, RTS_OFF);		
	return cnt;
}


