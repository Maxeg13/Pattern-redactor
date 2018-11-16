//modified from http://blablacode.ru/programmirovanie/392
#ifndef SERIAL_DLL
#define SERIAL_DLL
#include "stdafx.h"
#include "targetver.h"
#include <Windows.h>
class Serial
{
public:
    HANDLE hSerial;
void  InitCOM(const wchar_t* );
char  ReadCOM(bool& );
void close();
void write(char c);
};
#endif // SERIAL_H
