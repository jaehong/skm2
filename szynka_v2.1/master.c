#include <pthread.h>
#include <iostream>
#include <time.h>

#include <list>

#include "bus.h"
#include "glo.h"

#define MY_ADDR 'z'



pthread_t       m_thBusThread;
bool            m_bWaitForAck;
std::list<char> m_objClients;
char						m_cNextSlave;

bool            m_bClockWait; // jak true to czekamy
time_t          m_timeClockStart;

void master_start(void)
{
  pthread_create(&m_thBusThread, NULL, bus_start, NULL);
	m_objClients.push_front('c');
	m_objClients.push_front('b');
  sleep(1);
}

//! Taki semi-destruktor :)
void master_end()
{
  pthread_join(m_thBusThread, NULL);
}

void master_message_loop()
{

  printf("%d\n", FRAME_SIZE);

  struct frame *recvframe; 

  printf("+=======+=========+==========+=======================+\n");
  printf("|  TYP  | ADRESAT | ODBIORCA |                       |\n");
  printf("| RAMKI |  RAMKI  |   RAMKI  |                       |\n");
  printf("+=======+=========+==========+=======================+\n");

  while(1)
  {

    if(m_timeClockStart + MASTER_TIMEOUT <= time(NULL) ) {
      printf("+                    TIMEOUT                         +\n");
        m_bClockWait = false;
    }


    recvframe = bus_pickframe();
    if(recvframe != NULL)
    {
      switch(recvframe->f_chFlags & RESET_ID)
      {
      
        case ACK:
        {
          printf("|  ACK  |   %3c   |   %3c    |                       |\n", recvframe->f_chSenderAddr, recvframe->f_chReceiverAddr);
        }
        break;
      
        case DAT:
        {
          printf("|  DAT  |   %3c   |   %3c    |                       |\n", recvframe->f_chSenderAddr, recvframe->f_chReceiverAddr);
        }
	break;

	case FIN:
	{
          printf("|  FIN  |   %3c   |   %3c    |                       |\n", recvframe->f_chSenderAddr, recvframe->f_chReceiverAddr);
          if(recvframe->f_chReceiverAddr == MY_ADDR) {
            m_bClockWait = false;
          }
	}
        break;

        case STR:
        {
          printf("|  STR  |   MSR   |   %3c    |                       |\n", m_cNextSlave);
        }
        break;
      }
    }
    else
    {
      if(!m_bClockWait) 
      {
        m_cNextSlave = m_objClients.front();
        m_objClients.pop_front();
        m_objClients.push_back(m_cNextSlave);
        bus_putframe(MY_ADDR, m_cNextSlave, STR, "aaaaaaaaaaaaaaa"); // zmienic w przyszlosci
        m_bClockWait = true;
        m_timeClockStart = time(NULL);
        printf("|                 WYSLANO NOWA RAMKE                 |\n");
      }
     }
  }
}

int main()
{
    master_start();
    master_message_loop();
    master_end();
}
