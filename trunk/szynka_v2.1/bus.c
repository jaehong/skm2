//! Opakowanie najnizeszej warstwy programu w uzyteczne funkcje
/*!
	\created: 2009-04-21
	\author: pejotr, mietek
*/

 // @todo (pejotr#1#): Przeniesc sprawdzanie odbiorcy ramki z slave.c do bus.c
 // @todo (pejotr#3#): Usunac aktywne oczekiwanie

#include "bus.h"
#include "crc.h"

int           b_bInit;

char          b_chBuffer[BUFFER_SIZE];
volatile bool b_bRun;
int           b_iLibserFd;
pthread_t     b_thReaderThread;
pthread_t     b_thSlaveThread;

PRIVATE bool bus_validateframe(struct frame* recFrame);
PRIVATE void bus_readheader(char*);
PRIVATE int  bus_findframe();
PRIVATE int  bus_readtobuffer(int, int);
PRIVATE struct frame bus_buildframe(char sender, char receiver, char flags, char* data);
PRIVATE int bus_resetbuffer(int offset);


void bus_putframe( char sender_addr, char dest_addr, char flags, char* data)
{
	struct frame package;
	package.f_chReceiverAddr = dest_addr;
	package.f_chSenderAddr = sender_addr;
	package.f_chDataLength = strlen(data);
	package.f_chFlags = flags;
	strcpy(package.f_chData, data);
	package.f_iCrc = crc16((const unsigned char*)&package, FRAME_SIZE - sizeof(unsigned short), 0);
	fflush(stdout);
	int send = writer_send( ((char*)&package), FRAME_SIZE, b_iLibserFd);
}

PRIVATE struct frame bus_buildframe(char sender, char receiver, char flags, char* data)
{
	struct frame toSend;
	toSend.f_chSenderAddr = sender;
	toSend.f_chReceiverAddr = receiver;
	toSend.f_chDataLength = strlen(data);
	toSend.f_chFlags = flags;
	memcpy(toSend.f_chData,data,toSend.f_chDataLength);
	toSend.f_iCrc=crc16((const unsigned char*)&toSend, FRAME_SIZE - sizeof(unsigned short), 0);;
	return toSend;

}

//! Pobiera gotowa i poprawna ramke z bufora
/*!
		Funkcja znajduje ramke sprawdza czy jest poprwna (CRC)
		i zwraca wskaznik na mijesce gdzie jest ramka. Jesli akutalnie
		w buforze nie ma zadnej ramki lub ramka nie jest poprawna
		funkcja zwraca NULL
 */
struct frame* bus_pickframe()
{
	int pos = bus_findframe();
	struct frame *newframe = new frame;

	if( pos == -1 ) {
		int counter = (int)(BUFFER_SIZE/2);
		memcpy(b_chBuffer, b_chBuffer+counter, counter);
		bus_readtobuffer(counter, counter);
		return NULL;
	}
	else
	{
		memcpy((char*)newframe, b_chBuffer + pos + 1, sizeof(struct frame));
		memcpy(b_chBuffer, b_chBuffer + pos + FRAME_SIZE, BUFFER_SIZE - FRAME_SIZE - pos);
		bus_readtobuffer(BUFFER_SIZE - FRAME_SIZE - pos, BUFFER_SIZE - pos - FRAME_SIZE);
	}

	bool valid = bus_validateframe(newframe);
	if(!valid)
	{
		printf("Ramka niepoprawna - CRC mismatch ;)\n");
		delete newframe;
		return NULL;
	}

	return newframe;
}

//! Uruchamianie szyny
/*!
    Funkcja uruchamia Czytacza
 */
void* bus_start(void*)
{
	for(int i=0; i<BUFFER_SIZE; ++i)
	{
		b_chBuffer[i]='0';
	}
	b_iLibserFd = libser_open("/dev/ttyS0", B9600);
	reader_start(b_iLibserFd, 0);
	b_bInit = true;
	reader_read(NULL);
	pthread_create(&b_thReaderThread, NULL, reader_read, NULL);
}

void bus_stop()
{
	reader_stop();
}

//! Przeszukuje bufor w poszukiwaniu znacznika poczatku
PRIVATE int bus_findframe()
{
	///printf("Szukanie ramki: ");
	int counter = (int)(BUFFER_SIZE/2);
	for(int i = 0; i < counter; ++i) {
		///printf("%c", b_chBuffer[i]);
		if(b_chBuffer[i] == BEG_MARK)
		{
			///printf("Hahaha, znalazem poczatek ramki xD \n");
			return i;
		}
	}
	///printf("\n");
	return -1;
}

//! Sprawdza poprawnosc ramki
PRIVATE bool bus_validateframe(struct frame* recvframe)
{
    unsigned short newcrc = crc16((const unsigned char*)recvframe,FRAME_SIZE - sizeof(unsigned short), 0);
		//return false;
		return (newcrc == recvframe->f_iCrc);
}

//! Wczytuje do lokalnego bufora dane odczytane z szyny
PRIVATE int bus_readtobuffer(int bytes, int offset)
{
	int readcnt = 0, read = 0, toread = bytes;
	int cnt=0;
	while(read != bytes)
	{
		readcnt = reader_write(b_chBuffer + offset + read, toread);
		///printf("Yeee, skopiowalem %d :\n", readcnt);
		read += readcnt;
		toread -= readcnt;

		// na wszelki wypadek zeby nie zapetlic sie w odbiorze, probujemy
		// odczytac 3 razy jesli nic nieodczytamy opuszczamy petle
		if(readcnt == 0 )
		{
			++cnt;
			if(cnt == 3)
			{
				bus_resetbuffer(offset + read);
				return 1;
			}
		}
		else
		{
			cnt = 0;
		}
		usleep(500);
	}
	return 1;
}

//! Wypenlnia zerami okreslony obszar bufora szyny
PRIVATE int bus_resetbuffer(int offset)
{
	for(int i = offset; i < BUFFER_SIZE; ++i)
	{
		b_chBuffer[i] = '0';
	}
}
