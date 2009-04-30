/*
    Plik: crc.h

    Plik zawiera deklaracj�funkcji crc16 obliczajc crc 16 bitowe.
*/


/*
    Funkcja oblicza CRC 16 bitowe. Wykorzystuje tablic�crc_table (wyznaczonej dla danego polinominal).
    Jeli chcemy oblicza�crc w kilku krokach, moemy wywoa�funkcj�wielokrotnie przekazyjc jej
    kolejne cigi bajt� z kt�ych chcemy policzy�crc oraz crc z poprzedniego kroku. Pocztkowe crc 
    powinno by�0.

    Parametry: data - wskazanie na bufor zawierajcy dane z kt�ych liczymy CRC,
               size - wielko�bufora data,
               crc - crc z poprzedniego wywoania lub 0.
    Wynik: obliczone crc.
 */
unsigned short crc16( const unsigned char * data, unsigned long size, unsigned short crc);
