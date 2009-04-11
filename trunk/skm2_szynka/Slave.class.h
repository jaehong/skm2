#ifndef SLAVE_CLASS_H_INCLUDED
#define SLAVE_CLASS_H_INCLUDED

#include "Types.h"
#include "crc.h"
#include"Frame.h"

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

private:
  uint8_t  mDeviceId;
  uint8_t  mReadBuffer[1024];
  uint32_t fd;			// uchwyt zwrocony przez libser_open
  bool mTransmisionSet; //czy transmisja nawizana
  uint16_t mReadBuffBytesOccupied; //ilosc bajtow danych zapisanych aktualnie w mReadBuffer

};

}
#endif // SLAVE_CLASS_H_INCLUDED
