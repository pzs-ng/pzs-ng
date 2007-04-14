// File_Wm - Info for Windows Media files
// Copyright (C) 2002-2007 Jerome Martinez, Zen@MediaArea.net
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation condition
#include <MediaInfo/Setup.h>
#if defined(MEDIAINFO_WM_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <ZenLib/ZtringListList.h>
#include <ZenLib/Utils.h>
#include "MediaInfo/Multiple/File_Wm.h"
#include "MediaInfo/MediaInfo_Config.h"
#ifdef WIN32
    #include <windows.h>
#endif
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Wm::Read_Buffer_Continue()
{
    if (Buffer_Size<128)
        return;

    size_t Offset=0; //Offset debut
    int32u* BufferInt;

    //Format
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size-128 && !(BufferInt[0]==0x75B22630 && BufferInt[1]==0x11CF668E && BufferInt[2]==0xAA00D9A6 && BufferInt[3]==0x6CCE6200))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }
    if (Offset>Buffer_Size-128)
    {
        File_Offset=File_Size;
        return;
    }

    Stream_Prepare(Stream_General);
    General[0](_T("Format"))=_T("WM");

    //Play time
    Offset=0;
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size-4 && !(BufferInt[0]==0x8CABDCA1 && BufferInt[1]==0x11CFA947 && BufferInt[2]==0xC000E48E && BufferInt[3]==0x6553200C))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }
    if (Offset<=Buffer_Size-4)
    {
        General[0](_T("PlayTime")).From_Number(LittleEndian2int64u((char*)Buffer+Offset+64)/10000-LittleEndian2int64u((char*)Buffer+Offset+80));
    }

    //Description Title
    Offset=0;
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size-4 && !(BufferInt[0]==0x75B22633 && BufferInt[1]==0x11CF668E && BufferInt[2]==0xAA00D9A6 && BufferInt[3]==0x6CCE6200))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }
    if (Offset<=Buffer_Size-4)
    {
        Offset+=24;
        int Taille0=Buffer[Offset+0]+Buffer[Offset+1]*0x100;
        int Taille1=Buffer[Offset+2]+Buffer[Offset+3]*0x100;
        int Taille2=Buffer[Offset+4]+Buffer[Offset+5]*0x100;
        int Taille3=Buffer[Offset+6]+Buffer[Offset+7]*0x100;

        Offset+=10;

        General[0](_T("Title")).From_Unicode((wchar_t*)(Buffer+Offset), 0, Taille0);
        General[0](_T("Performer")).From_Unicode((wchar_t*)(Buffer+Offset+Taille0), 0, Taille1);
        General[0](_T("Copyright")).From_Unicode((wchar_t*)(Buffer+Offset+Taille0+Taille1), 0, Taille2);
        General[0](_T("Comment")).From_Unicode((wchar_t*)(Buffer+Offset+Taille0+Taille1+Taille2), 0, Taille3);
    }

    //Info audio
    Offset=0;
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size/sizeof(int32u)-4 && !(BufferInt[0]==0xF8699E40 && BufferInt[1]==0x11CF5B4D && BufferInt[2]==0x8000FDA8 && BufferInt[3]==0x2B445C5F))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }
    if (Offset<=Buffer_Size-4)
    {
        size_t Audio_Count=Stream_Prepare(Stream_Audio);
        Offset+=54;
        Audio[Audio_Count](_T("Codec")).From_Number(Buffer[Offset]+Buffer[Offset+1]*0x100, 16);
        Audio[Audio_Count](_T("Channel(s)")).From_Number(Buffer[Offset+2]); //2 octets
        BufferInt=(int32u*)(Buffer+Offset);
        Audio[Audio_Count](_T("SamplingRate")).From_Number(BufferInt[1]);
        Audio[Audio_Count](_T("BitRate")).From_Number(BufferInt[2]*8);
    }

    //Info video
    Offset=0;
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size-4 && !(BufferInt[0]==0xBC19EFC0 && BufferInt[1]==0x11CF5B4D && BufferInt[2]==0x8000FDA8 && BufferInt[3]==0x2B445C5F))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }
    if (Offset<=Buffer_Size-4)
    {
        Offset+=54;
        Offset+=15;
        BufferInt=(int32u*)(Buffer+Offset);
        if (BufferInt[0]>10 && BufferInt[0]<100000 && BufferInt[1]>10 && BufferInt[1]<100000) //Detection method is poor, so we need to test coherancy...
        {
            size_t Video_Count=Stream_Prepare(Stream_Video);
            Video[Video_Count](_T("Width")).From_Number(BufferInt[0]);
            Video[Video_Count](_T("Height")).From_Number(BufferInt[1]);
            Video[Video_Count](_T("Codec")).From_Local((char*)Buffer+Offset+12, 4);
        }

    }

    //Extended
    Offset=0;
    BufferInt=(int32u*)Buffer;
    while (Offset<=Buffer_Size-4 && !(BufferInt[0]==0xD2D0A440 && BufferInt[1]==0x11D2E307 && BufferInt[2]==0xA000F097 && BufferInt[3]==0x50A85EC9))
    {
        Offset++;
        BufferInt=(int32u*)(Buffer+Offset);
    }

    if (Offset<=Buffer_Size-4)
    {
        Offset+=24;
        int Nb=Buffer[Offset]+Buffer[Offset+1]*0x100;
        Offset+=2;
        for (int Pos=0; Pos<Nb; Pos++)
        {
            int TailleNom=Buffer[Offset]+Buffer[Offset+1]*0x100;
            Offset+=2;
            Ztring Nom; Nom.From_Unicode((wchar_t*)(Buffer+Offset), 0, TailleNom/2);
            Offset+=TailleNom;
            int Type=Buffer[Offset]+Buffer[Offset+1]*0x100;
            Offset+=2;//2 octets du type
            int TailleValeur=Buffer[Offset]+Buffer[Offset+1]*0x100;
            Offset+=2;
            //Gestion des type
            Ztring Valeur;
            if (Type==0) //Unicode
                Valeur.From_Unicode((wchar_t*)(Buffer+Offset), 0, TailleValeur/2);
            else if (Type==1) //Byte
                Valeur.From_Local((char*)(Buffer+Offset), 0, TailleValeur/2);
            else if (Type==2)//Bool
                Valeur.From_Number((int)Buffer[Offset]);
            else if (Type==3)//DWord
                Valeur.From_Number(LittleEndian2int32u(Buffer+Offset));
            else if (Type==4)//QWord
                Valeur.From_Number(LittleEndian2int64u(Buffer+Offset));
            else if (Type==5)//Word
                Valeur.From_Number(LittleEndian2int16u(Buffer+Offset));
            //Gestion des noms
                 if (Nom==_T("WM/AlbumTitle"))
                Fill(Stream_General, 0, "Album", Valeur);
            else if (Nom==_T("WM/AlbumArtist"))
            {
                Fill(Stream_General, 0, "Performer", ""); //Clear last value, like Author (Header)
                Fill(Stream_General, 0, "Performer", Valeur);
            }
            else if (Nom==_T("WM/ArtistSortOrder"))
                Fill(Stream_General, 0, "Performer/Sort", Valeur);
            else if (Nom==_T("WM/AuthorURL"))
                Fill(Stream_General, 0, "Performer/Url", Valeur);
            else if (Nom==_T("WM/BeatsPerMinute"))
                Fill(Stream_General, 0, "BPM", Valeur);
            else if (Nom==_T("WM/Binary"))
                Fill(Stream_General, 0, "Cover", "Y");
            else if (Nom==_T("WM/Comments"))
            {
                Fill(Stream_General, 0, "Comment", ""); //Clear last value, like Comment (Header)
                Fill(Stream_General, 0, "Comment", Valeur);
            }
            else if (Nom==_T("WM/Composer"))
                Fill(Stream_General, 0, "Composer", Valeur);
            else if (Nom==_T("WM/Conductor"))
                Fill(Stream_General, 0, "Conductor", Valeur);
            else if (Nom==_T("WM/EncodedBy"))
                Fill(Stream_General, 0, "EncodedBy", Valeur);
            else if (Nom==_T("WM/EncoderSettings"))
                Fill(Stream_General, 0, "Encoded_Library_Settings", Valeur);
            else if (Nom==_T("WM/EncodingTime"))
            {
                #ifdef _WIN32
                int64u I1=Valeur.To_int64u();
                FILETIME FileTime;
                FileTime.dwLowDateTime=(int32u)(I1&0xFFFFFFFF);
                FileTime.dwHighDateTime=(int32u)((I1&0xFFFFFFFF00000000LL)/0x100000000LL);
                SYSTEMTIME SystemTime;
                if (FileTimeToSystemTime(&FileTime, &SystemTime))
                {
                    Ztring Z1, Z2;
                    Z1+=_T("UTC ");
                    Z1+=Ztring::ToZtring(SystemTime.wYear);
                    Z1+=_T("-");
                    Z2=Ztring::ToZtring(SystemTime.wMonth);
                    if (Z2.size()<2) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Z1+=_T("-");
                    Z2=Ztring::ToZtring(SystemTime.wDay);
                    if (Z2.size()<2) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Z1+=_T(" ");
                    Z2=Ztring::ToZtring(SystemTime.wHour);
                    if (Z2.size()<2) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Z1+=_T(":");
                    Z2=Ztring::ToZtring(SystemTime.wMinute);
                    if (Z2.size()<2) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Z1+=_T(":");
                    Z2=Ztring::ToZtring(SystemTime.wSecond);
                    if (Z2.size()<2) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Z1+=_T(".");
                    Z2=Ztring::ToZtring(SystemTime.wMilliseconds);
                    if (Z2.size()<2) Z2=Ztring(_T("00"))+Z2;
                    if (Z2.size()<3) Z2=Ztring(_T("0"))+Z2;
                    Z1+=Z2;
                    Fill(Stream_General, 0, "Encoded_Date", Z1);
                }
                #endif //_WIN32
            }
            else if (Nom==_T("WM/Genre"))
                            {
                Fill(Stream_General, 0, "Genre", ""); //Clear last value, like Comment (Header)
                Fill(Stream_General, 0, "Genre", Valeur);
            }
            else if (Nom==_T("WM/GenreID"))
            {
                if (Get(Stream_General, 0, _T("Genre")).empty())
                    Fill(Stream_General, 0, "Genre", Valeur);
            }
            else if (Nom==_T("WM/MediaPrimaryClassID")) {}
            else if (Nom==_T("WM/MCDI")) {}
            else if (Nom==_T("WM/ModifiedBy"))
                Fill(Stream_General, 0, "RemixedBy", Valeur);
            else if (Nom==_T("WM/OriginalAlbumTitle"))
                Fill(Stream_General, 0, "Original/Album", Valeur);
            else if (Nom==_T("WM/Picture"))
                Fill(Stream_General, 0, "Cover", "Y");
            else if (Nom==_T("WM/Provider"))
                Fill(Stream_General, 0, "Provider", Valeur);
            else if (Nom==_T("WM/Publisher"))
                Fill(Stream_General, 0, "Publisher", Valeur);
            else if (Nom==_T("WM/RadioStationName"))
                Fill(Stream_General, 0, "RadioStation", Valeur);
            else if (Nom==_T("WM/RadioStationOwner"))
                Fill(Stream_General, 0, "RadioStation/Owner", Valeur);
            else if (Nom==_T("WM/TrackNumber"))
            {
                Fill(Stream_General, 0, "Track/Position", ""); //Clear last value, like WM/Track
                Fill(Stream_General, 0, "Track/Position", Valeur);
            }
            else if (Nom==_T("WM/Track"))
            {
                if (Get(Stream_General, 0, _T("Track/Position")).empty())
                    Fill(Stream_General, 0, "Track/Position", Valeur.To_int32u()+1);
            }
            else if (Nom==_T("WM/UniqueFileIdentifier"))
            {
                if (Valeur.empty() || Valeur[0]!=_T(';')) //Test if there is only the separator
                {
                    Valeur.FindAndReplace(_T(";"), Config.TagSeparator_Get());
                    Fill(Stream_General, 0, "UniqueID", Valeur);
                }
            }
            else if (Nom==_T("WM/Writer"))
                Fill(Stream_General, 0, "Lyristic", Valeur);
            else if (Nom==_T("WM/Year"))
                Fill(Stream_General, 0, "Recorded_Date", Valeur);
            else if (Nom==_T("IsVBR")) {}
            else if (Nom==_T("WMFSDKVersion")) {}
            else if (Nom==_T("WMFSDKNeeded")) {}
            else
            {
                General[0].push_back(Nom);
                General[0](Nom, Info_Name_Text)=Nom;
                General[0](Nom)=Valeur;
            }
            Offset+=TailleValeur;

        }
    }

    //No need of more
    File_Offset=File_Size;
}

//---------------------------------------------------------------------------
void File_Wm::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("UniqueID", "R|WM/UniqueFileIdentifier");
            Fill_HowTo("Format", "");
            Fill_HowTo("BitRate", "");
            Fill_HowTo("PlayTime", "");
            Fill_HowTo("Domain", "");
            Fill_HowTo("Collection", "");
            Fill_HowTo("Season", "");
            Fill_HowTo("Movie", "R|Title");
            Fill_HowTo("Movie/More", "");
            Fill_HowTo("Album", "R|WM/AlbumTitle");
            Fill_HowTo("Album/Sort", "");
            Fill_HowTo("Comic", "");
            Fill_HowTo("Comic/Position_Total", "");
            Fill_HowTo("Part", "");
            Fill_HowTo("Part/Position", "");
            Fill_HowTo("Part/Position_Total", "");
            Fill_HowTo("Track", "R|Title");
            Fill_HowTo("Track/More", "");
            Fill_HowTo("Track/Sort", "");
            Fill_HowTo("Track/Position", "R|WM/TrackNumber or WM/Track");
            Fill_HowTo("Track/Position_Total", "");
            Fill_HowTo("Chapter", "");
            Fill_HowTo("SubTrack", "");
            Fill_HowTo("Original/Album", "R|WM/OriginalAlbumTitle");
            Fill_HowTo("Original/Movie", "");
            Fill_HowTo("Original/Part", "");
            Fill_HowTo("Original/Track", "");
            Fill_HowTo("Performer", "R|WM/AlbumArtist or Author");
            Fill_HowTo("Performer/Sort", "R|WM/ArtistSortOrder");
            Fill_HowTo("Performer/Url", "R|WM/AuthorURL");
            Fill_HowTo("Original/Performer", "");
            Fill_HowTo("Accompaniment", "R|WM/AlbumArtist");
            Fill_HowTo("Composer", "R|WM/Composer");
            Fill_HowTo("Composer/Nationality", "");
            Fill_HowTo("Arranger", "");
            Fill_HowTo("Lyricist", "R|WM/Writer");
            Fill_HowTo("Original/Lyricist", "");
            Fill_HowTo("Conductor", "R|WM/Conductor");
            Fill_HowTo("Director", "");
            Fill_HowTo("AssistantDirector", "");
            Fill_HowTo("DirectorOfPhotography", "");
            Fill_HowTo("SoundEngineer", "");
            Fill_HowTo("ArtDirector", "");
            Fill_HowTo("ProductionDesigner", "");
            Fill_HowTo("Choregrapher", "");
            Fill_HowTo("CostumeDesigner", "");
            Fill_HowTo("Actor", "");
            Fill_HowTo("Actor_Character", "");
            Fill_HowTo("WrittenBy", "");
            Fill_HowTo("ScreenplayBy", "");
            Fill_HowTo("EditedBy", "");
            Fill_HowTo("Producer", "");
            Fill_HowTo("CoProducer", "");
            Fill_HowTo("ExecutiveProducer", "");
            Fill_HowTo("DistributedBy", "");
            Fill_HowTo("MasteredBy", "");
            Fill_HowTo("EncodedBy", "R|WM/EncodedBy");
            Fill_HowTo("RemixedBy", "R|WM/ModifiedBy");
            Fill_HowTo("ProductionStudio", "");
            Fill_HowTo("ThanksTo", "");
            Fill_HowTo("Publisher", "R|WM/Publisher");
            Fill_HowTo("Publisher/URL", "");
            Fill_HowTo("Label", "");
            Fill_HowTo("Genre", "R|WM/Genre WM/GenreID");
            Fill_HowTo("Mood", "");
            Fill_HowTo("ContentType", "");
            Fill_HowTo("Subject", "");
            Fill_HowTo("Description", "");
            Fill_HowTo("Keywords", "");
            Fill_HowTo("Summary", "");
            Fill_HowTo("Synopsys", "");
            Fill_HowTo("Period", "");
            Fill_HowTo("LawRating", "");
            Fill_HowTo("ICRA", "");
            Fill_HowTo("Released_Date", "");
            Fill_HowTo("Original/Released_Date", "");
            Fill_HowTo("Recorded_Date", "R|WM/Year");
            Fill_HowTo("Encoded_Date", "R|WM/EncodingTime");
            Fill_HowTo("Tagged_Date", "");
            Fill_HowTo("Written_Date", "");
            Fill_HowTo("Mastered_Date", "");
            Fill_HowTo("Recorded_Location", "");
            Fill_HowTo("Written_Location", "");
            Fill_HowTo("Archival_Location", "");
            Fill_HowTo("Comment", "R|WM/Comments or Comment");
            Fill_HowTo("Rating ", "");
            Fill_HowTo("Encoded_Application", "");
            Fill_HowTo("Encoded_Application/Url", "");
            Fill_HowTo("Encoded_Library", "");
            Fill_HowTo("Encoded_Library_Settings", "R|WM/EncoderSettings");
            Fill_HowTo("BPM", "R|WM/BeatsPerMinute");
            Fill_HowTo("ISRC", "");
            Fill_HowTo("ISBN", "");
            Fill_HowTo("BarCode", "");
            Fill_HowTo("LCCN", "");
            Fill_HowTo("CatalogNumber", "");
            Fill_HowTo("LabelCode", "");
            Fill_HowTo("Copyright", "R|Copyright");
            Fill_HowTo("Copyright/Url", "");
            Fill_HowTo("Producer_Copyright", "");
            Fill_HowTo("TermsOfUse", "");
            Fill_HowTo("Cover", "R|WM/Picture or WM/Binary");
            Fill_HowTo("Cover_Datas", "N|WM/Picture or WM/Binary");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R|Overall BitRate - Audio BitRates");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("SamplingRate", "R");
            break;
        case (Stream_Text) :
            break;
        case (Stream_Chapters) :
            break;
        case (Stream_Image) :
            break;
        case (Stream_Menu) :
            break;
        case (Stream_Max) :
            break;
    }
}

} //NameSpace

#endif //MEDIAINFO_WM_YES

