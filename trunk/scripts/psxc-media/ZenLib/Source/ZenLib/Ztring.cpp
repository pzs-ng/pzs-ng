// ZenLib::Ztring - std::(w)string is better
// Copyright (C) 2002-2006 Jerome Martinez, Zen@MediaArea.net
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// More methods for std::(w)string
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "ZenLib/Ztring.h"
#include "ZenLib/Utils.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <wx/strconv.h>
#include <wx/datetime.h>
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace ZenLib
{

//---------------------------------------------------------------------------
typedef basic_stringstream<Char> tstringStream;
//---------------------------------------------------------------------------

//***************************************************************************
// Operators
//***************************************************************************

Char &Ztring::operator() (size_type Pos)
{
    if (Pos>size())
        resize(Pos);
    return operator[] (Pos);
}

//***************************************************************************
// Conversions
//***************************************************************************

Ztring& Ztring::From_Unicode (const wchar_t* S)
{
    if (S==NULL)
        return *this;

    #ifdef _UNICODE
        assign(S);
    #else
        size_type OK=wxConvLocal.WC2MB(NULL, S, 0);
        if (OK!=0 && OK!=Error)
            assign(wxConvLocal.cWC2MB(S));
    #endif
    return *this;
}

Ztring& Ztring::From_Unicode (const wchar_t *S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length==Error)
        Length=wcslen(S+Start);
    wchar_t* Temp=new wchar_t[Length+1];
    wcsncpy (Temp, S+Start, Length);
    Temp[Length]='\0';

    From_Unicode(Temp);
    delete[] Temp;
    return *this;
}

Ztring& Ztring::From_UTF8 (const char* S)
{
    if (S==NULL)
        return *this;

    size_type OK=wxConvUTF8.MB2WC(NULL, S, 0);
    if (OK!=0 && OK!=Error)
        #ifdef _UNICODE
            assign(wxConvUTF8.cMB2WC(S).data());
        #else
            assign(wxConvLocal.cWC2MB(wxConvUTF8.cMB2WC(S)));
        #endif
    return *this;
}

Ztring& Ztring::From_UTF8 (const char* S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length==Error)
        Length=strlen(S+Start);
    char* Temp=new char[Length+1];
    strncpy (Temp, S+Start, Length);
    Temp[Length]='\0';

    From_UTF8(Temp);
    delete[] Temp;
    return *this;
}

Ztring& Ztring::From_UTF16 (const char* S)
{
    if (S==NULL)
        return *this;

    if (S[0]==(char)0xFF && S[0]==(char)0xFE)
        return From_UTF16LE(S+2);
    else if (S[0]==(char)0xFE && S[0]==(char)0xFF)
        return From_UTF16BE(S+2);
    else
        return From_UTF16LE(S); //Not sure, default
}

Ztring& Ztring::From_UTF16 (const char* S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length<2)
        return *this;

    if (S[0]==(char)0xFF && S[0]==(char)0xFE)
        return From_UTF16LE(S+2, Start, Length-2);
    else if (S[0]==(char)0xFE && S[0]==(char)0xFF)
        return From_UTF16BE(S+2, Start, Length-2);
    else if (S[0]==(char)0x00 && S[0]==(char)0x00)
    {
        clear(); //No begin!
        return *this;
    }
    else
        return From_UTF16LE(S+2, Start, Length-2); //Not sure, default
}

Ztring& Ztring::From_UTF16BE (const char* S)
{
    if (S==NULL)
        return *this;

    wxMBConvUTF16BE wxConvUTF16BE;
    size_type OK=wxConvUTF16BE.MB2WC(NULL, S, 0);
    if (OK!=0 && OK!=Error)
        #ifdef _UNICODE
            assign(wxConvUTF16BE.cMB2WC(S).data());
        #else
            assign(wxConvLocal.cWC2MB(wxConvUTF16BE.cMB2WC(S)));
        #endif
    return *this;
}

Ztring& Ztring::From_UTF16BE (const char* S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length==Error)
    {
        Length=0;
        while(S[Length]!=0x0000)
            Length++;
    }

    char* Temp=new char[Length+2];
    memcpy (Temp, S+Start, Length);
    Temp[Length+0]=0x00;
    Temp[Length+1]=0x00;
    From_UTF16BE(Temp);
    delete[] Temp;
    return *this;
}

Ztring& Ztring::From_UTF16LE (const char* S)
{
    if (S==NULL)
        return *this;

    wxMBConvUTF16LE wxConvUTF16LE;
    size_type OK=wxConvUTF16LE.MB2WC(NULL, S, 0);
    if (OK!=0 && OK!=Error)
        #ifdef _UNICODE
            assign(wxConvUTF16LE.cMB2WC(S).data());
        #else
            assign(wxConvLocal.cWC2MB(wxConvUTF16LE.cMB2WC(S)));
        #endif
    return *this;
}

Ztring& Ztring::From_UTF16LE (const char* S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length==Error)
    {
        Length=0;
        while(S[Length]!=0x0000)
            Length+=2;
    }

    char* Temp=new char[Length+2];
    memcpy (Temp, S+Start, Length);
    Temp[Length+0]=0x00;
    Temp[Length+1]=0x00;
    From_UTF16LE(Temp);
    delete[] Temp;
    return *this;
}

Ztring& Ztring::From_Local (const char* S)
{
    if (S==NULL)
        return *this;

    #ifdef _UNICODE
        //Added by epoximator because of memory leak detection in wxWidgets 2.6.3
        #ifdef _WIN32
            #define MAX_STRING_SIZE 4096
            wchar_t WideString[MAX_STRING_SIZE+1];
            size_t OK=mbstowcs(WideString, S, MAX_STRING_SIZE);
            if (OK==MAX_STRING_SIZE)
                WideString[MAX_STRING_SIZE]=L'\0'; //In this case, \0 is not added to string, must do it
            if(OK!=0 && OK!=Error)
                assign(WideString);
        #else //_WIN32
            size_type OK=wxConvLocal.MB2WC(NULL, S, 0);
            if (OK!=0 && OK!=Error)
                assign(wxConvLocal.cMB2WC(S).data());
        #endif //_WIN32
    #else
        assign(S);
    #endif
    return *this;
}

Ztring& Ztring::From_Local (const char* S, size_type Start, size_type Length)
{
    if (S==NULL)
        return *this;

    if (Length==Error)
        Length=strlen(S+Start);
    #ifdef _UNICODE
        char* Temp=new char[Length+1];
        strncpy (Temp, S+Start, Length);
        Temp[Length]='\0';
        From_Local(Temp);
        delete[] Temp;
    #else
        assign(S, Start, Length);
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int8s I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _itot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << (int)I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int8u I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _ultot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << (unsigned int)I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int16s I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _itot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << (int)I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int16u I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _ultot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << (unsigned int)I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int32s I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _itot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int32u I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[33];
        _ultot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << I;
        assign(SS.str());
    #endif
    return *this;
}

#if (MAXTYPE_INT>=64)
Ztring& Ztring::From_Number (const int64s I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[65];
        _i64tot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << I;
        assign(SS.str());
    #endif
    return *this;
}

Ztring& Ztring::From_Number (const int64u I, int8u Radix)
{
    #ifdef _WIN32
        Char* C1=new Char[65];
        _ui64tot (I, C1, Radix);
        assign (C1);
        delete[] C1;
    #else
        tstringStream SS;
        SS << setbase(Radix) << I;
        assign(SS.str());
    #endif
    return *this;
}
#endif

Ztring& Ztring::From_Number (const float32 F, int8u Precision, ztring_t Options)
{
    Char C1[100];
    #if defined (_UNICODE)
        snwprintf (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #else
        snprintf  (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #endif
    assign(C1);

    if ((Options & Ztring_NoZero && size()>0) && find(_T('.'))>0)
    {
        while (size()>0 && ((*this)[size()-1]==_T('0')))
            resize(size()-1);
        if (size()>0 && (*this)[size()-1]==_T('.'))
            resize(size()-1);
    }

    return *this;
}

Ztring& Ztring::From_Number (const float64 F, int8u Precision, ztring_t Options)
{
    Char C1[100];
    #if defined (_UNICODE)
        snwprintf (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #else
        snprintf  (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #endif
    assign(C1);

    if ((Options & Ztring_NoZero && size()>0) && find(_T('.'))>0)
    {
        while (size()>0 && ((*this)[size()-1]==_T('0')))
            resize(size()-1);
        if (size()>0 && (*this)[size()-1]==_T('.'))
            resize(size()-1);
    }

    return *this;
}

Ztring& Ztring::From_Number (const float128 F, int8u Precision, ztring_t Options)
{
    Char C1[100];
    #if defined (_UNICODE)
        snwprintf (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #else
        snprintf  (C1, 99, (Ztring(_T("%."))+Ztring::ToZtring(Precision)+_T("f")).c_str(), F);
    #endif
    assign(C1);

    if ((Options & Ztring_NoZero && size()>0) && find(_T('.'))>0)
    {
        while (size()>0 && ((*this)[size()-1]==_T('0')))
            resize(size()-1);
        if (size()>0 && (*this)[size()-1]==_T('.'))
            resize(size()-1);
    }

    return *this;
}

#if defined (MACOSX)
Ztring& Ztring::From_Number (const size_t I, int8u Radix)
{
    tstringStream SS;
    SS << setbase(Radix) << I;
    assign(SS.str());
    return *this;
}
#endif

//---------------------------------------------------------------------------
Ztring& Ztring::Duration_From_Milliseconds (const int64u Value)
{
    int8u HH=Value/1000/60/60;
    int8u MM=Value/1000/60   -((HH*60));
    int8u SS=Value/1000      -((HH*60+MM)*60);
    int8u MS=Value           -((HH*60+MM)*60+SS)*1000;
    Ztring DateT;
    Ztring Date;
    DateT.From_Number(HH); if (DateT.size()<2){DateT=Ztring(_T("0"))+DateT;}
    Date+=DateT;
    Date+=_T(":");
    DateT.From_Number(MM); if (DateT.size()<2){DateT=Ztring(_T("0"))+DateT;}
    Date+=DateT;
    Date+=_T(":");
    DateT.From_Number(SS); if (DateT.size()<2){DateT=Ztring(_T("0"))+DateT;}
    Date+=DateT;
    Date+=_T(".");
    DateT.From_Number(MS); if (DateT.size()<2){DateT=Ztring(_T("00"))+DateT;} else if (DateT.size()<3){DateT=Ztring(_T("0"))+DateT;}
    Date+=DateT;
    assign (Date.c_str());
    return *this;
}

Ztring& Ztring::Date_From_Seconds_1904 (const int64u Value)
{
    wxDateTime Date;
	Date.SetYear(1904);
	Date.SetMonth(wxDateTime::Jan);
    Date.SetDay(1);
    Date.SetHour(0);
    Date.SetMinute(0);
    Date.SetSecond(0);
    if (Value>=0x80000000)
    {
        //wxTimeSpan doesn't support unsigned int
        int64u Value2=Value;
        while (Value2>0x7FFFFFFF)
        {
            Date+=wxTimeSpan::Seconds(0x7FFFFFFF);
            Value2-=0x7FFFFFFF;
        }
        Date+=wxTimeSpan::Seconds(Value2);
    }
    else
        Date+=wxTimeSpan::Seconds(Value);

    Ztring ToReturn=_T("UTC ");
    ToReturn+=Date.FormatISODate();
    ToReturn+=_T(" ");
    ToReturn+=Date.FormatISOTime();

    assign (ToReturn.c_str());
    return *this;
}

Ztring& Ztring::Date_From_Seconds_1970 (const int32u Value)
{
    time_t Time=(time_t)Value;
    struct tm *Gmt=gmtime(&Time);
    Ztring DateT;
    Ztring Date=_T("UTC ");
    Date+=Ztring::ToZtring((Gmt->tm_year+1900));
    Date+=_T("-");
    DateT.From_Number(Gmt->tm_mon+1); if (DateT.size()<2){DateT=Ztring(_T("0"))+Ztring::ToZtring(Gmt->tm_mon+1);}
    Date+=DateT;
    Date+=_T("-");
    DateT.From_Number(Gmt->tm_mday); if (DateT.size()<2){DateT=Ztring(_T("0"))+Ztring::ToZtring(Gmt->tm_mday);}
    Date+=DateT;
    Date+=_T(" ");
    DateT.From_Number(Gmt->tm_hour); if (DateT.size()<2){DateT=Ztring(_T("0"))+Ztring::ToZtring(Gmt->tm_hour);}
    Date+=DateT;
    Date+=_T(":");
    DateT=Ztring::ToZtring(Gmt->tm_min); if (DateT.size()<2){DateT=Ztring(_T("0"))+Ztring::ToZtring(Gmt->tm_min);}
    Date+=DateT;
    Date+=_T(":");
    DateT.From_Number(Gmt->tm_sec); if (DateT.size()<2){DateT=Ztring(_T("0"))+Ztring::ToZtring(Gmt->tm_sec);}
    Date+=DateT;
    assign (Date.c_str());
    return *this;
}

Ztring& Ztring::Date_From_String (const char* Value, size_t Value_Size)
{
    Ztring ToReturn=_T("UTC ");
    wxDateTime Date;
    Ztring DateS;
    DateS.From_Local(Value, Value_Size).c_str();
    if (!DateS.empty() && DateS[DateS.size()-1]==_T('\n'))
        DateS.resize(DateS.size()-1);

    //Some strange formating : exactly 24 bytes (or 25 with 0x0A at the end) and Year is at the end
    if (DateS.size()==24 && DateS[23]>=_T('0') && DateS[23]<=_T('9') && DateS[21]>=_T('0') && DateS[21]<=_T('9') && DateS[19]==_T(' '))
        Date.ParseFormat(DateS.c_str(), _T("%a %b %d %H:%M:%S %Y"));
    //ISO date
    else if (DateS.size()==10 && (DateS[4]<_T('0') || DateS[4]>_T('9')) && (DateS[7]<_T('0') || DateS[7]>_T('9')))
    {
        DateS[4]=_T('-');
        DateS[7]=_T('-');
        ToReturn+=DateS;
    }
    //Default
    else
        Date.ParseDateTime(DateS.c_str());

    if (ToReturn.size()<5 && Date.GetYear()!=1970)
    {
        ToReturn+=Date.FormatISODate();
        ToReturn+=_T(" ");
        ToReturn+=Date.FormatISOTime();
    }
    else if (ToReturn.size()<5)
        ToReturn+=DateS;

    assign (ToReturn.c_str());
    return *this;
}

//---------------------------------------------------------------------------
std::wstring Ztring::To_Unicode () const
{
    #ifdef _UNICODE
        return c_str();
    #else
        return wxConvLocal.cMB2WC(c_str()).data();
    #endif
}

std::string Ztring::To_UTF8 () const
{
    #ifdef _UNICODE
        return wxConvUTF8.cWC2MB(c_str()).data();
    #else
        return wxConvUTF8.cWC2MB(wxConvLocal.cMB2WC(c_str())).data();
    #endif
}

std::string Ztring::To_Local () const
{
    #ifdef _UNICODE
        wxCharBuffer C=wxConvLocal.cWC2MB(c_str());
        if (C.data())
            return C.data();
        else
            return "";
    #else
        return c_str();
    #endif
}

//---------------------------------------------------------------------------
//Operateur ToInt
int32s Ztring::To_int32s (ztring_t Options) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    int32s I;
    #ifdef _WIN32
        I=_ttoi(c_str());
    #else
        tstringStream SS;
        SS << c_str();
        SS >> I;
    #endif

    //Rounded
    if (Options==Ztring_Rounded && find(_T("."))!=Error)
    {
        float128 F=To_float128();
        F-=I;
        if (F>=0.5)
            return I+1;
    }

    return I;
}

//---------------------------------------------------------------------------
//Operateur ToInt
int32u Ztring::To_int32u (ztring_t Options) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    int32u I;
    #ifdef _WIN32
        I=_ttoi64(c_str()); //TODO : I>0x7FFFFFFF - Replaced by i64 version to support, but not good
    #else
        tstringStream SS;
        SS << c_str();
        SS >> I;
    #endif

    //Rounded
    if (Options==Ztring_Rounded && find(_T("."))!=Error)
    {
        float128 F=To_float128();
        F-=I;
        if (F>=0.5)
            return I+1;
    }

    return I;
}

//---------------------------------------------------------------------------
//Operateur ToInt
#if (MAXTYPE_INT>=64)
int64s Ztring::To_int64s (ztring_t Options) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    int64s I;
    #ifdef _WIN32
        I=_ttoi64(c_str());
    #else
        tstringStream SS;
        SS << c_str();
        SS >> I;
    #endif

    //Rounded
    if (Options==Ztring_Rounded && find(_T("."))!=Error)
    {
        float128 F=To_float128();
        F-=I;
        if (F>0.5)
            return I+1;
    }

    return I;
}
#endif

//---------------------------------------------------------------------------
//Operateur ToInt
#if (MAXTYPE_INT>=64)
int64u Ztring::To_int64u (ztring_t Options) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    int64u I;
    #ifdef _WIN32
        I=_ttoi64(c_str()); //TODO : I>0x7FFFFFFFFFFFFFFF
    #else
        tstringStream SS;
        SS << c_str();
        SS >> I;
    #endif

    //Rounded
    if (Options==Ztring_Rounded && find(_T("."))!=Error)
    {
        float128 F=To_float128();
        F-=I;
        if (F>=0.5)
            return I+1;
    }

    return I;
}
#endif

//---------------------------------------------------------------------------
//Operateur ToFloat
float32 Ztring::To_float32(ztring_t) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    #if defined(_WIN32) && defined(_UNICODE)
        return (wcstod(c_str(),NULL));
    #else //MinGW32
        float32 F;
        tstringStream SS;
        SS << c_str();
        SS >> F;

        return F;
    #endif
}

//---------------------------------------------------------------------------
//Operateur ToFloat
float64 Ztring::To_float64(ztring_t) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    #if defined(_WIN32) && defined(_UNICODE)
        return (wcstod(c_str(),NULL));
    #else //MinGW32
        float32 F;
        tstringStream SS;
        SS << c_str();
        SS >> F;

        return F;
    #endif
}

//---------------------------------------------------------------------------
//Operateur ToFloat
float128 Ztring::To_float128(ztring_t) const
{
    //Integrity
    if (empty())
        return 0;

    //Conversion
    #if defined(_WIN32) && defined(_UNICODE)
        return (wcstod(c_str(),NULL));
    #else //MinGW32
        float32 F;
        tstringStream SS;
        SS << c_str();
        SS >> F;

        return F;
    #endif
}

//***************************************************************************
// Edition
//***************************************************************************

//---------------------------------------------------------------------------
// Retourne une partie de la chaine
Ztring Ztring::SubString (const tstring &Begin, const tstring &End, size_type Pos, ztring_t Options) const
{
    //Recherche Début
    size_type I_Debut=find(Begin, Pos);
    if (I_Debut==Error)
        return _T("");
    I_Debut+=Begin.size();

    //gestion fin NULL
    if (End==_T(""))
        return substr(I_Debut);

    //Recherche Fin
    size_type I_Fin=find(End, I_Debut);
    if (I_Fin==Error)
    {
        if (Options & Ztring_AddLastItem)
            return substr(I_Debut);
        else
            return _T("");
    }

    return substr(I_Debut, I_Fin-I_Debut);
}

//---------------------------------------------------------------------------
//FindAndReplace
Ztring::size_type Ztring::FindAndReplace (const ZenLib::tstring &ToFind, const ZenLib::tstring &ReplaceBy, size_type Pos, ZenLib::ztring_t Options)
{
   size_type Count=0;
   size_type Middle=Pos;
   while (!(Count==1 && !(Options&Ztring_Recursive)) && (Middle=find(ToFind, Middle))!=npos)
   {
      replace(Middle, ToFind.length(), ReplaceBy);
      Middle += ReplaceBy.length();
      Count++;
   }

    return Count;
}

//---------------------------------------------------------------------------
//Mise en minuscules
Ztring &Ztring::MakeLowerCase()
{
    transform(begin(), end(), begin(), (int(*)(int))tolower); //(int(*)(int)) is a patch for unix
    return *this;
}

//---------------------------------------------------------------------------
// Mise en majuscules
Ztring &Ztring::MakeUpperCase()
{
    transform(begin(), end(), begin(), (int(*)(int))toupper); //(int(*)(int)) is a patch for unix
    return *this;
}

//---------------------------------------------------------------------------
// Remove leading whitespaces from a string
Ztring &Ztring::TrimLeft(Char ToTrim)
{
    size_type First=0;
    while (operator[](First)!=ToTrim)
        First++;
    assign (c_str()+First);
    return *this;
}

//---------------------------------------------------------------------------
// Remove trailing whitespaces from a string
Ztring &Ztring::TrimRight(Char ToTrim)
{
    size_type Last=size();
    while (operator[](Last)!=ToTrim)
        Last--;
    assign (c_str(), Last);
    return *this;
}

//---------------------------------------------------------------------------
// Remove leading and trailing whitespaces from a string
Ztring &Ztring::Trim(Char ToTrim)
{
    TrimLeft(ToTrim);
    TrimRight(ToTrim);
    return *this;
}

//---------------------------------------------------------------------------
// Quotes a string
Ztring &Ztring::Quote(Char ToTrim)
{
    assign(tstring(1, ToTrim)+c_str()+ToTrim);
    return *this;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
//Count
Ztring::size_type Ztring::Count (const Ztring &ToCount, ztring_t) const
{
    size_type Count=0;
    for (size_type Pos=0; Pos<=size(); Pos++)
        if (find(ToCount, Pos)!=npos)
        {
            Count++;
            Pos+=ToCount.size()-1; //-1 because the loop will add 1
        }
    return Count;
}

//---------------------------------------------------------------------------
//Compare
bool Ztring::Compare (const Ztring &ToCompare, const Ztring &Comparator, ztring_t Options) const
{
    //Integers management
    if (size() && ToCompare.size() && at(0)>=_T('0') && at(0)<=_T('9') && at(size()-1)>=_T('0') && at(size()-1)<=_T('9') && ToCompare[0]>=_T('0') && ToCompare[0]<=_T('9') && ToCompare[ToCompare.size()-1]>=_T('0') && ToCompare[ToCompare.size()-1]<=_T('9'))
    {
        #if (MAXTYPE_INT>=64)
            int64s Left=To_int64s();
            int64s Right=ToCompare.To_int64s();
        #else
            int32s Left=To_int32s();
            int32s Right=ToCompare.To_int32s();
        #endif
        if (Comparator==_T("==")) return (Left==Right);
        if (Comparator==_T("<"))  return (Left< Right);
        if (Comparator==_T("<=")) return (Left<=Right);
        if (Comparator==_T(">=")) return (Left>=Right);
        if (Comparator==_T(">"))  return (Left> Right);
        if (Comparator==_T("!=")) return (Left!=Right);
        if (Comparator==_T("<>")) return (Left!=Right);
        return false;
    }

    //Case sensitive option
    if (!(Options & Ztring_CaseSensitive))
    {
        //Need to copy strings and make it lowercase
        Ztring Left (c_str());
        Ztring Right (ToCompare.c_str());
        Left.MakeLowerCase();
        Right.MakeLowerCase();

        //string comparasion
        if (Comparator==_T("==")) return (Left==Right);
        if (Comparator==_T("IN")) {if (Left.find(Right)!=Error) return true; else return false;}
        if (Comparator==_T("<"))  return (Left< Right);
        if (Comparator==_T("<=")) return (Left<=Right);
        if (Comparator==_T(">=")) return (Left>=Right);
        if (Comparator==_T(">"))  return (Left> Right);
        if (Comparator==_T("!=")) return (Left!=Right);
        if (Comparator==_T("<>")) return (Left!=Right);
        return false;
    }
    else
    {
        //string comparasion
        if (Comparator==_T("==")) return (*this==ToCompare);
        if (Comparator==_T("IN")) {if (this->find(ToCompare)!=Error) return true; else return false;}
        if (Comparator==_T("<"))  return (*this< ToCompare);
        if (Comparator==_T("<=")) return (*this<=ToCompare);
        if (Comparator==_T(">=")) return (*this>=ToCompare);
        if (Comparator==_T(">"))  return (*this> ToCompare);
        if (Comparator==_T("!=")) return (*this!=ToCompare);
        if (Comparator==_T("<>")) return (*this!=ToCompare);
        return false;
    }
}

} //namespace

