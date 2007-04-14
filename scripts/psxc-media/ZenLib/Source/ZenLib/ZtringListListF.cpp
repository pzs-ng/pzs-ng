// ZenLib::ZtringListListF - ZtringListList with file load/save
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
//
// ZtringListList with file load/save
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <wx/wxprec.h>
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/strconv.h>
#include <wx/utils.h>
#include "ZenLib/ZtringListListF.h"
//---------------------------------------------------------------------------

namespace ZenLib
{

//---------------------------------------------------------------------------
#define READ_SIZE 512*1024
//---------------------------------------------------------------------------

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Constructors
void ZtringListListF::ZtringListListF_Common ()
{
    Backup_Nb_Max=0;
    Backup_Nb=0;
    Sauvegarde=true;
    #ifdef _UNICODE
        Local=false;
    #else
        Local=true;
    #endif
}

ZtringListListF::ZtringListListF ()
:ZtringListList ()
{
    ZtringListListF_Common();
}

ZtringListListF::ZtringListListF (const ZtringListList &Source)
:ZtringListList (Source)
{
    ZtringListListF_Common();
}

ZtringListListF::ZtringListListF (const Ztring &Source)
:ZtringListList (Source)
{
    ZtringListListF_Common();
}

ZtringListListF::ZtringListListF (const Char *Source)
:ZtringListList (Source)
{
    ZtringListListF_Common();
}

#ifdef _UNICODE
ZtringListListF::ZtringListListF (const char* Source)
:ZtringListList (Source)
{
    ZtringListListF_Common();
}
#endif

//***************************************************************************
// File management
//***************************************************************************

//---------------------------------------------------------------------------
// Load
size_t ZtringListListF::Load (const Ztring &NewFileName)
{
    clear();
    if (!NewFileName.empty())
        Name=NewFileName;

    size_t I1=Error;

    if (Name.find(_T(".csv"))!=Error)
        I1=CSV_Charger();
    if (Name.find(_T(".cfg"))!=Error)
        I1=CFG_Charger();
    if (I1!=Error)
    {
        Backup_Nb=0; //mettre ici le code pour trouver le nb de backup
        return Backup_Nb;
    }
    else
        return Error;
}

//---------------------------------------------------------------------------
// Load CSV
size_t ZtringListListF::CSV_Charger ()
{
    //Read file
    char *Buffer=NULL;
    unsigned long BytesCount;
    #ifdef _WIN32 //Awful hack to be able to read files with Unicode in their name
    HANDLE Handle=CreateFile(Name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (Handle!=(HANDLE)-1)
    {
        DWORD Length_High;DWORD Length_Low=GetFileSize(Handle, &Length_High);
        Buffer=new char[Length_Low+1];
        ReadFile(Handle, Buffer, Length_Low, &BytesCount, NULL);
        CloseHandle(Handle);
    }
    else
    {
    #endif
        if (wxFileName::FileExists(Name.c_str()))
        {
            wxFile F;
            if (!F.Open(Name.c_str()))
            {
                delete[] Buffer;
                return Error;
            }
            Buffer=new char[F.Length()+1];
            BytesCount=F.Read(Buffer, F.Length());
            F.Close();
        }
        else
        {
            delete[] Buffer;
            return Error;
        }
    #ifdef _WIN32 //Awful hack to be able to read files with Unicode in their name
    }
    #endif
    if (BytesCount==Error)
    {
        delete[] Buffer;
        return Error;
    }
    Buffer[BytesCount]='\0';

    //Convert file in UTF-8 or Local
    Ztring File;
    if (!Local)
    {
        //UTF-8
        File.From_UTF8(Buffer, 0, BytesCount);
        #ifdef _DEBUG
        if (File.size()==0)
             File.From_Local(Buffer, 0, BytesCount);
        #endif //_DEBUG
    }
    if (File.size()==0)
        //Local of UTF-8 failed
        File.From_Local(Buffer, 0, BytesCount);

    //Separators
    if (Separator[0]==_T("(Default)"))
        Separator[0]=wxTextFile::GetEOL();
    Ztring SeparatorT=Separator[1];
    Separator[1]=_T(";");

    //Writing
    Write(File);

    //Separators
    Separator[1]=SeparatorT;

    delete[] Buffer;
    return 1;
}

//---------------------------------------------------------------------------
// Chargement CFG
size_t ZtringListListF::CFG_Charger ()
{
    char*  Buffer=NULL;
    size_t Buffer_Size=0;
    //WxWidgets is not able to handle files with Unicode in their name, or with FileSize>4 GiB, we use Win32 API
    #ifdef _WIN32
    HANDLE Handle=CreateFile(Name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (Handle==INVALID_HANDLE_VALUE)
    {
        //Sometime the file is locked for few millisiconds, we try again
        Sleep(3000);
        Handle=CreateFile(Name.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    }
    if (Handle!=INVALID_HANDLE_VALUE)
    {
        //FileSize
        int64u FileSize;
        DWORD High;DWORD Low=GetFileSize(Handle,&High);
        FileSize=0x100000000ULL*High+Low;
        int Read_Size=FileSize<READ_SIZE?FileSize:READ_SIZE;
        Buffer=new char[Read_Size];
        //Buffer
        ReadFile(Handle, Buffer, Read_Size, (DWORD*)&Buffer_Size, NULL);
        //Close
        CloseHandle(Handle); Handle=NULL;
    }
    //WxWidgets version
    #else //_WIN32
    wxFile FB;
    if (FB.Open(Name.c_str()))
    {
        //FileSize
        int64u FileSize=FB.Length();
        int Read_Size=FileSize<READ_SIZE?FileSize:READ_SIZE;
        Buffer=new char[Read_Size];
        //Begin
        Buffer_Size=FB.Read(Buffer, Read_Size);
        //Close
        FB.Close();
    }
    #endif //_WIN32
    //File is not openable
    if (Buffer==NULL || Buffer_Size==0)
    {
        delete[] Buffer;
        return Error;
    }

    //Convert File --> ZtringList
    ZtringList List;
    List.Separator_Set(0, _T("\r\n"));
    Ztring Z1;
    Z1.From_UTF8(Buffer, Buffer_Size);
    List.Write(Z1);

    Ztring SeparatorT=Separator[1];
    Separator[1]=_T(";");

    Ztring Propriete, Valeur, Commentaire;

    for (size_t Pos=0; Pos<List.size(); Pos++)
    {
        Ztring &Lu=List(Pos);
        if (Lu.find(_T("="))>0)
        {
            //Obtention du Name
            Propriete=Lu.SubString(_T(""), _T("="));
            NettoyerEspaces(Propriete);
            //Obtention de la valeur
            Valeur=Lu.SubString(_T("="), _T(";"), 0, Ztring_AddLastItem);
            NettoyerEspaces(Valeur);
        }
        //Obtention du commentaire
        Commentaire=Lu.SubString(_T(";"), _T(""), 0, Ztring_AddLastItem);
        NettoyerEspaces(Commentaire);
        //Ecriture
        push_back((Propriete+_T(";")+Valeur+_T(";")+Commentaire).c_str()); //Visual C++ 6 is old...
    }
    Separator[1]=SeparatorT;
    return 1;
}

//---------------------------------------------------------------------------
// Sauvegarde globale
size_t ZtringListListF::Save (const Ztring &FileName)
{
    //Gestion de l'annulation de la sauvegarde
    if (!Sauvegarde)
        return Backup_Nb;

    if (FileName!=_T(""))
        Name=FileName;

    //Gestion des backups
    Backup_Nb=0;
    size_t I2;
    if (Backup_Nb_Max>0)
    {
        //TODO : not tested
        for (size_t I1=Backup_Nb_Max-1; I1>0; I1--)
        {
            Ztring Z1=Name+_T(".sav"); Z1+=Ztring::ToZtring((intu)I1);
            Ztring Z2=Name+_T(".sav"); Z2+=Ztring::ToZtring((intu)I1+1);
            wxRemoveFile (Z2.c_str());
            I2=wxRenameFile(Z1.c_str(), Z2.c_str());
            if (I2 && !Backup_Nb)
                Backup_Nb=I2;
        }
        Ztring Z1=Name+_T(".sav0");
        wxRemoveFile(Z1.c_str());
        wxRenameFile(Name.c_str(), Z1.c_str());
        Backup_Nb++;
    }

    I2=Error;
    if (Name.find(_T(".csv"))!=Error)
        I2=CSV_Sauvegarder();
    if (Name.find(_T(".cfg"))!=Error)
        I2=CFG_Sauvegarder();

    if (I2>0)
    {
        return Backup_Nb;
    }
    else
        return 0;
}

//---------------------------------------------------------------------------
// Sauvegarde CSV
size_t ZtringListListF::CSV_Sauvegarder ()
{
    //Sauvegarde
    wxRemoveFile(Name.c_str());
    wxFile F;
    if (!F.Open(Name.c_str(), wxFile::write))
        return Error;

    if (Separator[0]==_T("(Default)"))
        Separator[0]=wxTextFile::GetEOL();

    F.Write(Read().c_str());

    return 1;
}

//---------------------------------------------------------------------------
// Sauvegarde CFG
size_t ZtringListListF::CFG_Sauvegarder ()
{
    wxFile F;
    if (!F.Create(Name.c_str(), true))
        return Error;

    Ztring ToWrite;
    Ztring Propriete, Valeur, Commentaire;

    ;
    for (size_t Pos=0; Pos<size(); Pos++)
    {
        Propriete=Read(Pos, 0);
        Valeur=Read(Pos, 1);
        Commentaire=Read(Pos, 2);
        if (Propriete!=_T(""))
        {
            ToWrite+=Propriete+_T(" = ");
            if (Valeur!=_T(""))
                ToWrite+=Valeur+_T(" ");
        }
        if (Commentaire!=_T(""))
            ToWrite+=_T("; ")+Commentaire;
        ToWrite+=_T("\r\n");
    }
    F.Write(ToWrite.c_str());

    return 1;
}

//---------------------------------------------------------------------------
// Annulation
size_t ZtringListListF::Cancel ()
{
    Ztring Z1=Name+_T(".sav0"); //Visual C++ 6 patch
    wxRemoveFile(Name.c_str());
    wxRenameFile(Z1.c_str(), Name.c_str());
    for (size_t I1=1; I1<=Backup_Nb; I1++)
    {
        Ztring Z2=Name+_T(".sav"); Z2+=Ztring::ToZtring((intu)I1); //Visual C++ 6 patch
        Ztring Z3=Name+_T(".sav"); Z3+=Ztring::ToZtring((intu)I1-1); //Visual C++ 6 patch
        wxRemoveFile(Z3.c_str());
        wxRenameFile(Z2.c_str(), Z3.c_str());
    }
    Write(_T(""));
    return CSV_Charger();
}

//***************************************************************************
// Divers
//***************************************************************************

//---------------------------------------------------------------------------
// Nettoyage
size_t ZtringListListF::NettoyerEspaces (Ztring &ANettoyer)
{
    size_t Debut=0;
    while (Debut<ANettoyer.size() && ANettoyer[Debut]==_T(' '))
        Debut++;
    size_t Fin=ANettoyer.size()-1;
    while (Fin!=(size_t)-2 && ANettoyer[Fin]==_T(' '))
        Fin--;
    if (Fin>=Debut)
        ANettoyer=ANettoyer.substr(Debut, Fin-Debut+1);
    else
        ANettoyer=_T("");
    return 1;
}

//---------------------------------------------------------------------------
// Backup
void ZtringListListF::Backup_Set (bool NewSave)
{
    Sauvegarde=NewSave;
    Save();
}

void ZtringListListF::Backup_Count_Set (size_t NewCount)
{
    Backup_Nb_Max=NewCount;
}

//---------------------------------------------------------------------------
// Local
void ZtringListListF::Local_Set (bool NewLocal)
{
    Local=NewLocal;
}

} //Namespace

