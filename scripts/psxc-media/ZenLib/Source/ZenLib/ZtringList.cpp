// ZenLib::ZtringList - More methods for vector<std::(w)string>
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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <wx/strconv.h>
#include <algorithm>
#include <functional>
#include "ZenLib/ZtringList.h"
using std::vector; //Visual C++ 6 patch
//---------------------------------------------------------------------------

namespace ZenLib
{

static Ztring ZtringList_Bidon;

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Constructors
ZtringList::ZtringList ()
{
    Separator[0]=_T(";");
    Quote=_T("\"");
    Max[0]=Error;
}

ZtringList::ZtringList(const ZtringList &Source)
{
    Separator[0]=Source.Separator[0];
    Quote=Source.Quote;

    reserve(Source.size());
    for (intu Pos=0; Pos<Source.size(); Pos++)
        push_back(Source[Pos]);
}

ZtringList::ZtringList (const Ztring &Source)
{
    Separator[0]=_T(";");
    Quote=_T("\"");
    Max[0]=Error;
    Write(Source.c_str());
}

ZtringList::ZtringList (const Char *Source)
{
    Separator[0]=_T(";");
    Quote=_T("\"");
    Max[0]=Error;
    Write(Source);
}

#ifdef _UNICODE
ZtringList::ZtringList (const char* S)
{
    Write(wxConvUTF8.cMB2WC(S).data());
}
#endif

//***************************************************************************
// Operator
//***************************************************************************

//---------------------------------------------------------------------------
// Operator ==
bool ZtringList::operator== (const ZtringList &Source) const
{
    return (Read()==Source.Read());
}

//---------------------------------------------------------------------------
// Operator !=
bool ZtringList::operator!= (const ZtringList &Source) const
{
    return (!(Read()==Source.Read()));
}

//---------------------------------------------------------------------------
// Operator +=
ZtringList &ZtringList::operator+= (const ZtringList &Source)
{
    reserve(size()+Source.size());
    for (size_type Pos=0; Pos<Source.size(); Pos++)
        push_back(Source[Pos]);

    return *this;
}

//---------------------------------------------------------------------------
// Operator =
ZtringList &ZtringList::operator= (const ZtringList &Source)
{
    clear();
    Ztring C=Separator[0];
    Ztring Q=Quote;

    Separator[0]=Source.Separator[0];
    Quote=Source.Quote;
    reserve(Source.size());
    for (size_type Pos=0; Pos<Source.size(); Pos++)
        push_back(Source[Pos]);

    Separator[0]=C;
    Quote=Q;

    return *this;
}

//---------------------------------------------------------------------------
// Operator ()
Ztring &ZtringList::operator() (size_type Pos)
{
    if (Pos>=size())
        Write(_T(""), Pos);

    return operator[](Pos);
}

//***************************************************************************
// In/Out
//***************************************************************************

//---------------------------------------------------------------------------
// Read
Ztring ZtringList::Read () const
{
    //Integrity
    if (size()==0)
        return _T("");

    Ztring Retour;
    for (size_type Pos=0; Pos<size(); Pos++)
    {
        if (operator[](Pos).find(Separator[0])==std::string::npos)
            Retour+=operator[](Pos)+Separator[0];
        else
            Retour+=Quote+operator[](Pos)+Quote+Separator[0];
    }

    //delete all useless separators at the end
    while (Retour.find(Separator[0].c_str(), Retour.size()-Separator[0].size())!=std::string::npos)
        Retour.resize(Retour.size()-Separator[0].size());

    return Retour;
}

Ztring ZtringList::Read (size_type Pos) const
{
    //Integrity
    if (Pos>=size())
        return _T("");

    return operator[](Pos);
}

//---------------------------------------------------------------------------
// Write
void ZtringList::Write(const Ztring &ToWrite)
{
    clear();

    if (!&ToWrite || !ToWrite.size())
        return;

    size_type PosC=0;
    bool Fini=false;
    Ztring C1;

    Ztring DelimiterL;
    Ztring DelimiterR;
    do
    {
        if (ToWrite.size()>PosC && Quote.size()>0 && ToWrite[PosC]==Quote[0] && ToWrite.substr(PosC, Quote.size())==Quote) //Quote found (ToWrite[PosC]==Quote[0] is here for optimization
        {
            DelimiterL=Quote;
            DelimiterR=Quote+Separator[0];
        }
        else
        {
            DelimiterL.clear();
            DelimiterR=Separator[0];
        }

        C1=ToWrite.SubString(DelimiterL, DelimiterR, PosC, Ztring_AddLastItem);
        if (DelimiterR.size()>Separator[0].size() && C1.size()==ToWrite.size()-Quote.size()-PosC) //This is the last item of the line, we must suppress the Quote at the end
            C1.resize(C1.size()-Quote.size());

        if (size()<Max[0])
            push_back(C1);
        else
        {
            //No more adding is permit, we add to the last element (with the separator)
            at(size()-1)+=Separator[0];
            at(size()-1)+=C1;
        }

        PosC+=C1.size()+DelimiterL.size()+DelimiterR.size();
        if (PosC>=ToWrite.size())
            Fini=true;
    }
    while (!Fini);

    return;
}

void ZtringList::Write(const Ztring &ToWrite, size_type Pos)
{
    if (Pos>=size())
    {
        //Reservation de ressources
        if (capacity()==0)
            reserve(1);
        while (Pos>=capacity())
            reserve(capacity()*2);

        while (Pos>size())
            push_back (_T(""));
        push_back(ToWrite);
    }
    else
        operator[](Pos)=ToWrite;

    return;
}

//***************************************************************************
// Edition
//***************************************************************************

//---------------------------------------------------------------------------
// Swap
void ZtringList::Swap (size_type Pos0_A, size_type Pos0_B)
{
    //Integrity
    size_type Pos_Max;
    if (Pos0_A<Pos0_B)
        Pos_Max=Pos0_B;
    else
        Pos_Max=Pos0_A;
    if (Pos_Max>=size())
        Write(_T(""), Pos_Max);

    operator [] (Pos0_A).swap(operator [] (Pos0_B));
}

//---------------------------------------------------------------------------
// Sort
void ZtringList::Sort(ztring_t)
{
    std::stable_sort(begin(), end());
    return;
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
// Find
Ztring::size_type ZtringList::Find (const Ztring &ToFind, size_type Pos, const Ztring &Comparator, ztring_t Options) const
{
    while (Pos<size() && !(operator[](Pos).Compare(ToFind, Comparator, Options)))
        Pos++;
    if (Pos>=size())
        return Error;
    return Pos;
}

//---------------------------------------------------------------------------
// Return the length of the longest string in the list.
Ztring::size_type ZtringList::MaxStringLength_Get ()
{
   size_type Max = 0;
   for (ZtringList::const_iterator it=begin(); it!=end(); ++it)
      if (it->size()>Max)
         Max=it->size();
   return Max;
}

//***************************************************************************
// Configuration
//***************************************************************************

//---------------------------------------------------------------------------
// Separator
void ZtringList::Separator_Set (size_type Level, const Ztring &NewSeparator)
{
    if (Level>0)
        return;
    Separator[Level]=NewSeparator;
}

//---------------------------------------------------------------------------
// Quote
void ZtringList::Quote_Set (const Ztring &NewQuote)
{
    Quote=NewQuote;
}

//---------------------------------------------------------------------------
// Separator
void ZtringList::Max_Set (size_type Level, size_type NewMax)
{
    if (Level>0 || NewMax==0)
        return;
    Max[Level]=NewMax;
}

} //namespace






