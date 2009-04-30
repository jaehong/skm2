//! Czytacz szyny, najnizsza warstwa programu.
/*!
	Czytacz szyny uruchomiony jest w oddzielnym watku,
	wszystkie odczytane dane umieszcza w swoim wewnetrznym buforze.

	\created: 2009-04-21
	\author: pejotr
*/

#include <stdio.h>
#include "reader.h"
#include "glo.h"
#include "frame.h"


char          r_chBuffer[BUFFER_SIZE]; 	//!< Wewnetrzny bufor czytacz
int           r_iBytesRead;							//!< Ilosc dostepntych danych w buforze
volatile bool r_bRun;										//!< Poki TRUE program dziala
volatile bool r_bPause;									//!< Wstrzymuje odczyt z szyny

bool          r_bDebug;			//!< Jesli ustawiona na TRUE wyswietlane sa dodatkowe informacje
int           r_iLibserFd; 	//!< Deskryptor szyny


pthread_mutex_t r_mtBuffer, 		//!< Synchronizacja dostepu do bufora
								r_mtBytesRead;	//!< Ochrona ilczby odczytanych bajtow

PRIVATE void reader_log(char* data);

//!Uruchamia watek szyny, przeprowadza koniczene inicjalizacje
void reader_start(int libserFd, int bytes=0)
{
	r_bRun = true;
	r_iLibserFd = libserFd;
	r_iBytesRead = bytes;

	pthread_mutex_init(&r_mtBuffer, NULL);
	pthread_mutex_init(&r_mtBytesRead, NULL);
}

//! Zatrzymanie watku czytajacego
void reader_stop()
{
	r_bRun = false;
}

//! Przepisuje zadana ilosc bajtow z bufora Czytacza do destBuffer
/*!
		Kopiowana jest maksymalna mozliwa ilosc bajtow. Tzn jesli w buforze
		jest wystarczajaca danych kopiowana jest podana jako argument ilosc
		bajtow, przeciwnym przypadku kopiowane sa wszystkie dostepne dane

		\return ilosc skoopiowanych bajtow
 */
int reader_write(char* destBuffer, int bytes)
{
	pthread_mutex_lock(&r_mtBuffer);
		// skopiuj zadana ilosc danych do dostarczonego bufora
		int copycnt = MIN(bytes, r_iBytesRead);
		memcpy(destBuffer, r_chBuffer, copycnt);

		// Przesun bufor readera w lewo o odczyta liczbe bajtow
		memcpy(r_chBuffer, r_chBuffer + copycnt, BUFFER_SIZE-copycnt-1);
		r_iBytesRead -= copycnt;
		fflush(stdout);
	pthread_mutex_unlock(&r_mtBuffer);
	return copycnt;
}

//! Glowna funckcja czytacza, Odpalana w osobnym watku
/*!
		Czytacz czyta z szyny dane po 1 bajcie. Jesli oczytany znak jest
		hipotetycznym poczatkiem ramki jest kopiowany do wewnetrznego bufora
		a nastepnie z szyny odczytywana jest ilosc bajtow rowna rozmiarowi
		ramki, zapisywana bezpo¶rednio w zrzutowanej strukturze, a nastepnie
		w buforze tuz za prawdopodobnym rozpoczeciem ramki. Nie ma w tym miejscu
		zadnej kontroli czy faktycznie odczytany bajt rowny co do wartosci znacznikowi
		poczatku ramki, faktycznie jest poczatkiem ramki. Zajmuje sie tym warstwa
		wyzsza
 */
void* reader_read(void*)
{
	struct timeval tv;
	struct frame fr;
	char c;

	while(r_bRun) {

		if(r_iBytesRead == BUFFER_SIZE) {
			continue;
		}

		tv.tv_sec = 2; tv.tv_usec	= 0;
		if( libser_read( r_iLibserFd, &c, 1, &tv ) != 1) {
			//printf(".");
		} else {
			if( c == BEG_MARK )
			{
				pthread_mutex_lock(&r_mtBuffer);
					///printf("@@ RAMKA ZNALEZIONA @@ %d\n", r_iBytesRead);
					r_chBuffer[r_iBytesRead] = BEG_MARK;
					tv.tv_sec = 20; tv.tv_usec = 0;
					libser_read(r_iLibserFd, (char *)&fr, FRAME_SIZE, &tv);
					memcpy(r_chBuffer + r_iBytesRead + 1, (char*)&fr, FRAME_SIZE);
					r_iBytesRead += FRAME_SIZE + 1;
				pthread_mutex_unlock(&r_mtBuffer);
			}
		}
		fflush(stdout); // !!!
		usleep(100);
	}
}


//! Wypisuje dodatkowe komunikaty
PRIVATE void reader_log(char* data)
{
	if(r_bDebug) {
		printf("%s", data);
	}
}
