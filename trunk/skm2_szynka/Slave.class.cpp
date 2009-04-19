#include "Slave.class.h"



using namespace SKM2;

Slave::Slave(uint8_t id): mDeviceId(id), mTransmisionSet(false), mReadBuffBytesOccupied(0)
{
	fd = libser_open("/dev/ttyS1", B9600);
}

SLave::~Slave() { libser_close(fd); }



/*
 * w tej funkcji kreci sie slave
 */
void Slave::Go()
{
	uint8_t fr_buff[256];
	bool id_num=0;

	//tutaj odpalamy watek (PutData()) ktory czyta dane z szyny i wrzuca do bufora jesli
	//jest w nim miejsce

	while(true)
	{
		ReadFrame(fr_buff);
		if(fr_buff[0]!=BEG_MARK) continue; /* trzeba sprawdzic bo ReadFrame
											* wcale nie musi wrzucic danych do fr_buff
											*/
		Frame frame;
		frame.MakeFrame(fr_buff);
		mReadBuffBytesOccupied -= frame.GetDataLength(); //ilosc nieprzeczytanych danych w mReadBuffer


		uint8_t flag = frame.GetFlags_flag();
		if(frame.GetFlags_id()!=id_num)continue;
		if(mTransmisionSet && flag==STR ) continue;
		if(!mTransmisionSet && flag!=STR) continue;

		if(mTransmisionSet && flag==FIN)//koniec transmisji
		{
			//TODO zrobic cos z danymi
			id_num=mTransmisionSet=0;
			continue;
		}

		if(mTransmisionSet && flag==DAT)//dane
		{
			//TODO zrobic cos z danymi
			//TODO wyslac potwierdzenie (w nowym watku??)
			id_num= !id_num;
			continue;
		}

		if(flag==ACK)//zalegle potwierdzenie
		{
			//TODO sprawdzic kolejke wiadomosci oczekujacych na potwierdzenie odbioru
			//porownac odbiorcow i nadawcow, usunac ramke oczekujaca na potwierdzenie
			//jesli wszystko sie zgdza, badz olac ack jesli nie pasuje
		}

	}

}


/*
 * czyta po 1 bajcie do mReadBuffer
 */

void Slave::PutData()
{
	uint16_t index;

	while(true)
	{
		//mutex.down()
		index=mReadBuffBytesOccupied+1; //wskazuje kolejna wolna komorke
		if(index>=1024) continue; //brak wolnego miejsca
		if( libser_read( fd, & mDataBuffer[index], 1, & tv ) != 1 )continue;
		++mReadBuffBytesOccupied;
		//mutex.up()
	}
}


/*
 * odczytuje podana ilosc bajtow z bufora
 * jednoczesnie usuwa te przeczytane bajty
 *
 *
 * w fr_buff przeczytana ramka
 */

void Slave::ReadFrame(uint8_t* fr_buff)
{
	//tutaj mutex.down()

	uint32_t index=FindFrame();
	if(index==-1)
	{
		//mutex.up()
		return;
	}

	Frame* frame=new Frame();
	frame->MakeFrame(&mReadBuffer[index]);
	uint32_t len_to_cut = index + frame->GetDataLength() + 7; //ale brzydko....

	strncpy(fr_buff, &mReadBuffer[index], frame->GetDataLength());//kopiuje ramke

	strncpy(mReadBuffer, &mReadBuffer[index+frame->GetDataLength()], 1024-len_to_cut );
	//! dobrze by bylo jeszczena koncu bufora wrzucic jakies znaki (np puste)
	//! zeby przypadkiem nie zdublowac ramki
	//tutaj mutex.up()

}


/*
 * @return index w buforze oznaczajacy poczatek ramki
 * lub -1 jesli brak poprawnej ramki
 */

int16_t Slave::FindFrame() //przeglada bufor bez jego modyfikacji
{
	uint16_t length;
	uint8_t* ptr;
	uint16_t crc_new;

	for(int i=0; i<1024; ++i)
	{
		ptr = &mReadBuffer[i];

		if(*ptr==BEG_MARK)
		{
			ptr+=3*sizeof(char); //wskazuje na mDataLength
			length=*ptr;
			crc_new = crc16(ptr,length);

			ptr+=(length+1)*sizeof(char); //wskazuje na mCrc
			if(crc_new==*ptr)return i; //znalezlismy ramke
		}
	}

	return -1;
}
