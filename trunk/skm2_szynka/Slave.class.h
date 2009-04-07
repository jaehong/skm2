#ifndef SLAVE_CLASS_H_INCLUDED
#define SLAVE_CLASS_H_INCLUDED

#include "Types.h"

namespace SKM2
{

class Slave
{
public:
  Slave(uint8_t id);

  //! Odczytuje z bufora zadana liczbe znakow
  uint8_t* Read(int bytes);
private:
  uint8_t  mDeviceId;
  uint8_t  mReadBuffer[1024];
};

}
#endif // SLAVE_CLASS_H_INCLUDED
