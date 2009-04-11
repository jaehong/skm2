#include "Frame.h"

/*
 * uzywana do tworzenia ramki do wyslania
 */
Frame::Frame(uint16_t length)
{
	mData= new uint8_t[length];
	mDataLength=length;
}

Frame::Frame(){}

Frame::~Frame()
{
	delete [] mData;
}

void Frame::MakeFrame(uint8_t buff[])
{
	uint8_t* ptr=buff;

	mBegining=*ptr;
	mSenderAddr=*++ptr;
	mReceiverAddr=*++ptr;
	mDataLength=(uint16_t)*++ptr;
	mFlags=*ptr;
	for(int i=0; i<mDataLength; ++i)
		mData[i]=*++ptr;
	mCrc=(uint16_t)*++ptr;
}

void Frame::SetDataLength(uint16_t l)
{
		if(l<=249){
			mDataLength=l;
			return;
		}
		mDataLength=0; //z dupy troche...
}

void Frame::SetFlag_flag(uint8_t f)
{
		mFlags = mFlags & RESET_FLAG; //->0000y000
		mFlags = mFlags | f;
}

void Frame::SetFlag_id(uint8_t i)
{
		mFlags = mFlags & RESET_ID; //->yyyy0000
		mFlags = mFlags | i;
}
