//! Definicje zwiazane bezposrednia z ramka danych
/*!
	\created: 2009-04-21
	\author: pejotr
*/

#ifndef FRAME_H_
#define FRAME_H_

#define RCK 0x8        //00001000B
#define FIN 0x10       //00010000B
#define DAT 0x20       //00100000B
#define STR 0x40       //01000000B
#define ACK 0x80       //10000000B
#define RESET_ID 0xF0  //11110000B
#define RESET_FLAG 0x8 //00000100B


#define NUM1 0x4			 //00000100B
#define NUM0 0x0			 //00000000B


#define BEG_MARK 0x5E //BEG_MARK== '^'
#define FRAME_SIZE      sizeof(struct frame)
#define IN_FRAME_SIZE   sizeof(struct inframe)
#define FRAME_BUFFER_SIZE 248

struct frame
{
	char f_chSenderAddr;
	char f_chReceiverAddr;
	unsigned short f_chDataLength;
	char f_chFlags;
	char f_chData[FRAME_BUFFER_SIZE];
	unsigned short f_iCrc;
};


struct inframe
{
  char f_chReceiverAddr;
  char f_chFlags;
  char f_chData[FRAME_BUFFER_SIZE];
};

#endif
