/*
 * 								Frame structure:
 *
 * ==============================================================================================
 * | mBegining | mSenderAddr | mReceiverAddr | mDataLength | mFlags   |   mData   |    mCrc 	|
 * 0		   7			15				23				31	  39			39-1991	  55-2007
 * ==============================================================================================
 *
 * where:
 *
 * 				=======================
 * mFlags ->	| Flag | Id | Padding |
 * 				0	   4	5		  7
 * 				=======================
 *
 */





#ifndef FRAME_H_
#define FRAME_H_

#include "Types.h"

#define FIN 0x10 //00010000B
#define DAT 0x20 //00100000B
#define STR 0x40 //01000000B
#define ACK 0x80 //10000000B
#define RESET_ID 0xF0 //11110000B
#define RESET_FLAG 0x8 //00001000B

#define BEG_MARK 0x33 //???!!!!

class Frame {
public:
	Frame(uint16_t length);
	virtual ~Frame();

	void MakeFrame(uint8_t buff[]); //uzywane przy odbieraniu ramki

	int8_t GetBegining(){ return mBegining; }
	int8_t GetSender(){ return mSenderAddr; }
	int8_t GetReceiver(){ return mReceiverAddr; }
	uint16_t GetDataLength(){ return mDataLength; }
	uint8_t GetFlags(){ return mFlags; }
	uint8_t GetFlags_flag(){ return mFlags & RESET_ID;}
	bool GetFlags_id(){ return ( (mFlags & RESET_FLAG)==RESET_FLAG ); } //return 1 <=> mId=1
	uint8_t* GetData(){ return mData; }
	uint16_t GetCrc(){ return mCrc; }

	void SetBegining(int8_t b){ mBegining = b; }
	void SetSenderAddr(int8_t s){ mSenderAddr = s; }
	void SetReceiverAddr(int8_t r){ mReceiverAddr = r; }
	void SetDataLength(uint16_t l);
	void SetFlag_flag(uint8_t f);
	void SetFlag_id(uint8_t i);
	void SerFlags(uint8_t f){ mFlags = f; }
	void SetCrc(uint16_t c){ mCrc = c; }


private:
	int8_t mBegining;
	int8_t mSenderAddr;
	int8_t mReceiverAddr;
	uint16_t mDataLength; //ma zajmowac 8 bitow a nie 16, wiec trzeba sprawdzac zakres
	int8_t mFlags;
	uint8_t* mData;
	uint16_t mCrc;

};

#endif /* FRAME_H_ */
