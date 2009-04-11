/*
    Plik: crc.h

    Plik zawiera deklaracj� funkcji crc16 obliczaj�c� crc 16 bitowe.
*/


/*
    Funkcja oblicza CRC 16 bitowe. Wykorzystuje tablic� crc_table (wyznaczonej dla danego polinominal).
    Je�li chcemy oblicza� crc w kilku krokach, mo�emy wywo�a� funkcj� wielokrotnie przekazyj�c jej
    kolejne ci�gi bajt�w z kt�rych chcemy policzy� crc oraz crc z poprzedniego kroku. Pocz�tkowe crc
    powinno by� 0.

    Parametry: data - wskazanie na bufor zawieraj�cy dane z kt�rych liczymy CRC,
               size - wielko�� bufora data,
               crc - crc z poprzedniego wywo�ania lub 0.
    Wynik: obliczone crc.
 */
unsigned short crc16( const unsigned char * data, unsigned long size, unsigned short crc = 0 );

