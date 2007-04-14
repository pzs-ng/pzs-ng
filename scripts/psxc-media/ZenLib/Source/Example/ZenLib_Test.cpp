#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include "ZenLib/Conf.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/ZtringListList.h"
#include "ZenLib/MemoryDebug.h"

using namespace ZenLib;
using namespace std;

int main(char argc, char ** argv)
{
    //Ztring and Unicode
    //-How To Use
    Ztring Z1, Z2;
    Z1+=Z2.From_Unicode(L"Unicode - \xE0\x01FC\r\n");
    Z1+=Z2.From_Local("No Unicode - abc\r\n");
    Z1+=Z2.From_Number(0x7FFFFFFFFFFFFFFFULL);
    //Displaying
    #if defined(_UNICODE)
        wprintf (L"%s\r\n", Z1.c_str());
    #else
        printf ("%s\r\n", Z1.c_str());
    #endif

    //ZtringLisList
    ZtringListList ZLL;
    ZLL=L"11;12;13\r\n21;22;23\r\n31;32;33";
    wprintf (L"\r\nZtringListList\r\n%s\r\n", ZLL.Read().c_str());
    wprintf (L"%s\r\n", ZLL.Read(1).c_str());
    wprintf (L"%s\r\n", ZLL.Read(1, 1).c_str());




    //Debug Mode
    char* A=new char[50]; //Search for a file Debug_MemoryLeak.txt

    system("PAUSE");

}
//---------------------------------------------------------------------------

