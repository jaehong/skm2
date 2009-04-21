/*
 * Reader.class.h
 *
 *  Created on: 2009-04-21
 *      Author: pejotr
 */

#ifndef READER_CLASS_H_
#define READER_CLASS_H_

#include <pthread.h>

namespace SKM2
{

class Reader
{
public:
	Reader(char*);
	Reader(char*, std::ostream&);

	void Go();

	//! Zatrzymanie wątku klienta
	void Stop() {
		m_bRun = false;
	}

	//! Wypisywanie na ekran wszystkie co zostanie odczytane
	void Debug(bool flag) {
		m_bDebug = flag;
	}

	//! Funktor
	void operator()() {
		Go();
	}

private:
	std::ostream& m_sOutput;
	volatile bool m_bRun;
	bool          m_bDebug;

};

}

#endif /* READER_CLASS_H_ */
