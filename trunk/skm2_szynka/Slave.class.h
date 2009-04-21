#ifndef SLAVE_CLASS_H_INCLUDED
#define SLAVE_CLASS_H_INCLUDED

#include <pthread.h>

#include "Reader.class.h"
#include "Types.h"
#include "crc.h"
#include "Frame.h"

namespace SKM2
{

class Slave
{
public:

  Slave(uint8_t id);
  ~Slave();
  void ReadFrame(uint8_t* fr_buff);
  int16_t FindFrame();
  void Go();
  void PutData();

  void operator()() { Go(); };

private:
  uint8_t  mDeviceId;
  uint8_t  mReadBuffer[1024];
  uint32_t fd;			// uchwyt zwrocony przez libser_open
  bool mTransmisionSet; //czy transmisja nawizana
  uint16_t mReadBuffBytesOccupied; //ilosc bajtow danych zapisanych aktualnie w mReadBuffer

  pthread_mutex_t m_mtBufferMutex;
  Reader          m_clReader;


};

}
#endif // SLAVE_CLASS_H_INCLUDED
