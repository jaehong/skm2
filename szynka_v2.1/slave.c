#include <pthread.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <map>

#include "bus.h"
#include "glo.h"
#include <list>
#include<map>


using namespace std;

//! Watek obslugi szyny
/*!
		Watek odpowiedzailny za uruchomienie Czytacza szyny
 */
pthread_t s_thBusThread;
char      s_chAddr;
char      s_chRecv;
list<struct inframe> s_list;
map<char, bool> s_ackmap;
FILE * file;

bool      s_bAckWait;
time_t    s_timeClockStart;


void slave_start(void)
{
  pthread_create(&s_thBusThread, NULL, bus_start, NULL);
  sleep(1);
}

void slave_end()
{
  pthread_join(s_thBusThread, NULL);
}

void slave_sendfile(char*, char);

void slave_message_loop()
{
  map<char, bool> frame_numbers; //przechowuje numery kolejnych ramek w zaleznosci od wysylajcego
  struct frame *recvframe; 

	//printf("%d", FRAME_SIZE);
	

  printf("+=======+=========+==========+=======================+\n");
  printf("|  TYP  | ADRESAT | ODBIORCA |                       |\n");
  printf("| RAMKI |  RAMKI  |   RAMKI  |                       |\n");
  printf("+=======+=========+==========+=======================+\n");

  while(1)
  {
  
    if(s_bAckWait && s_timeClockStart + SLAVE_TIMEOUT <= time(NULL) ) {
        printf("+                    TIMEOUT                         +\n");
        s_bAckWait = false;
    }
  
    recvframe = bus_pickframe();
    if(recvframe != NULL) 
    {
      switch(recvframe->f_chFlags & RESET_ID)
      {
        case ACK:
        {
            //s_ackmap.insert(pair<char, bool>
            //s_ackmap[recvframe->f_chReceiverAddr] =! s_ackmap[recvframe->f_chReceiverAddr];
                        
            s_bAckWait = false;
            if(recvframe->f_chReceiverAddr != s_chAddr)
            {
              bus_putframe( s_chAddr, MASTER_ADDR, FIN, "aaa");
            }
            
            
            if(recvframe->f_chSenderAddr != s_chAddr && recvframe->f_chReceiverAddr == s_chAddr)
            {
              s_list.pop_front();
            }
            
        }
        break;

	      // pakiet z danymi
        case DAT:
        {
          printf("|  DAT  |   %3c   |   %3c    |                       |\n", recvframe->f_chSenderAddr, recvframe->f_chReceiverAddr);
        	if(recvframe->f_chReceiverAddr == s_chAddr)
					{
						switch(recvframe->f_chFlags & RCK)
						{
							case RCK:
							{
								printf("|       |         |          | WYMAGA POTWIERDZENIA  |\n");
              	if( (file=fopen(&s_chRecv, "a")) == NULL)
								{
		              printf("Nie udalo sie otworzyc pliku!\n");
              	}
              	fwrite(recvframe->f_chData, 1, strlen(recvframe->f_chData), file);
              	fclose(file);             
              	bus_putframe(s_chAddr, recvframe->f_chSenderAddr, ACK, "...");
							}
							break;

							case 0:
							{
								printf("|       |         |          | RAMKA TYPU BROADCAST  |\n");
							}
							break;
						}
          }
        }
        break;
                    
        // zezwolenie na wysylanie
        case STR:
        {
	        printf("|  STR  |   %3c   |   %3c    |                       |\n", recvframe->f_chSenderAddr, recvframe->f_chReceiverAddr);
          if(recvframe->f_chReceiverAddr == s_chAddr) 
          {
            frame_numbers[recvframe->f_chSenderAddr]=false;
            if(!s_list.empty())
            {
							//printf("BEDE WYSYLAC\n");
              bus_putframe( s_chAddr, s_chRecv, s_list.front().f_chFlags, s_list.front().f_chData);
              s_timeClockStart = time(NULL);
              s_bAckWait = true;
            }
						else
						{
							printf("|  FIN  |   %3c   |   %3c    | KONIEC TRANSMISJI :)  |\n", s_chAddr, MASTER_ADDR);
            	bus_putframe( s_chAddr, MASTER_ADDR, FIN, "aaa");				
						}
          }
        }
        break;
      }
    }
    else 
    {
      usleep(500);
    }
    delete recvframe;	
  }
}

void slave_setaddress(char a)
{
	s_chAddr = a;
}


void slave_sendfile(char* name)
{
  char flag;
  FILE* file;
	bool num=false;

  bool number=false;
  struct inframe toSend;
  if( (file=fopen(name, "r")) ==NULL){
		printf("Nie udalo sie otworzyc pliku %s!\n", name);
		return;
  }

  while(!feof(file))
  {
    fread(toSend.f_chData, 1, FRAME_BUFFER_SIZE, file);    
    ///setFrameNumber(number, &flag);
    number=!number; 
		toSend.f_chFlags = number ? NUM1 : NUM0;
    toSend.f_chFlags |= DAT | RCK;
    s_list.push_back(toSend);
        
    for(int i = 0; i < FRAME_BUFFER_SIZE; ++i) 
    {
      toSend.f_chData[i] = '\0';
    }
	}
  fclose(file);
}





int main(int argc, char **argv)
{
//char c;
//setFrameFlag(DAT, &c);


//printf("%d\n", argc);
if(argc != 4 ) {
	printf("Usage: ./slave my_addr\n");
	exit(1);
}
	
s_chAddr = *argv[1];
s_chRecv = *argv[2];


        slave_sendfile(argv[3]);
	slave_start();
	slave_message_loop();
	slave_end();
	return 0;
}


