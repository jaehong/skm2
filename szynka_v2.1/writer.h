#ifndef WRITER_H_
#define WRITER_H_

#include "libser.h"
#include "crc.h"
#include "glo.h"
#include "frame.h"
#include "bus.h"

int  writer_send(char*, int size, int fd);


#endif /* WRITER_H_ */
