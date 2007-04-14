// File__Base_Inform - Base for other files
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
//
// Give common methods for all file types
// Init and Finalize part
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#include <MediaInfo/Setup.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <time.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <ZenLib/Utils.h>
#include "MediaInfo/File__Base.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void PlayTime_PlayTime123   (const Ztring &Value, ZtringListList &List); //Internal function
void FileSize_FileSize123   (const Ztring &Value, ZtringListList &List); //Internal function
void Kilo_Kilo123           (const Ztring &Value, stream_t StreamKind, ZtringListList &List); //Internal function
void Value_Value123         (const Ztring &Value, stream_t StreamKind, ZtringListList &List); //Internal function
//---------------------------------------------------------------------------

//***************************************************************************
// Init
//***************************************************************************

//---------------------------------------------------------------------------
void File__Base::General_Fill()
{
    //Coherancy
    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);

    //FileName and FileSize
    //if (CompleteFileName.size()>0 && (General[0](_T("CompleteName")).empty() || General[0](_T("FileSize")).empty())) //Do it even if already, some merges can modify it
    if (CompleteFileName.size()>0)
    {
        //FileName
        General[0](_T("CompleteName"))=CompleteFileName;
        wxFileName FN; FN=CompleteFileName.c_str();
        General[0](_T("FolderName"))=FN.GetPath().c_str();
        General[0](_T("FileName"))=FN.GetName().c_str();
        General[0](_T("FileExtension"))=Ztring(FN.GetExt().c_str()).MakeLowerCase();

        //FileSize
        int64u FileSize=0;
        #ifdef __BORLANDC__ //Awful hack to be able to have FileSize>4 GiB
        HANDLE Handle=CreateFile(CompleteFileName.c_str(), 0, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (Handle!=INVALID_HANDLE_VALUE)
        {
            DWORD High; DWORD Low=GetFileSize(Handle,&High);
            FileSize=0x100000000ULL*High+Low;
            CloseHandle(Handle); Handle=NULL;
        }
        else
        {
        #endif
            wxFile FB;
            if (FB.Open(CompleteFileName.c_str()))
            {
                FileSize=FB.Length();
                FB.Close();
            }
        #ifdef __BORLANDC__ //Awful hack to be able to have FileSize>4 GiB
        }
        #endif
        if (FileSize)
            General[0](_T("FileSize")).From_Number(FileSize);
    }
}

//***************************************************************************
// Finalize
//***************************************************************************

//---------------------------------------------------------------------------
void File__Base::Finalize()
{
    //Format specific
    Open_Buffer_Finalize();

    //For each kind of Stream
    for (size_t Pos=0; Pos<General.size();  Pos++) Finalize_General(Pos);
    for (size_t Pos=0; Pos<Video.size();    Pos++) Finalize_Video(Pos);
    for (size_t Pos=0; Pos<Audio.size();    Pos++) Finalize_Audio(Pos);
    for (size_t Pos=0; Pos<Text.size();     Pos++) Finalize_Text(Pos);
    for (size_t Pos=0; Pos<Chapters.size(); Pos++) Finalize_Chapters(Pos);
    for (size_t Pos=0; Pos<Image.size();    Pos++) Finalize_Image(Pos);
    for (size_t Pos=0; Pos<Menu.size();     Pos++) Finalize_Menu(Pos);

    //For all streams (Generic)
    for (size_t StreamKind=Stream_General; StreamKind<Stream_Max; StreamKind++)
        Finalize__All((stream_t)StreamKind);
}

//---------------------------------------------------------------------------
void File__Base::Finalize__All(stream_t StreamKind)
{
    Ztring Z1, Z2; //For Codec_List
    for (size_t Pos=0; Pos<(*Stream[StreamKind]).size(); Pos++)
        Finalize__All(StreamKind, Pos, Z1, Z2);

    //Codec_List
    if (StreamKind!=Stream_General && !Z1.empty())
    {
        Z1.resize(Z1.size()-3); //Delete extra " / "
        Z2.resize(Z2.size()-3); //Delete extra " / "
        Ztring Z3=Get((stream_t)StreamKind, 0, _T("StreamKind"), Info_Measure);
        General[0](Z3+_T("_Codec_List"))=Z1;
        General[0](Z3+_T("_Language_List"))=Z2;
    }
}

//---------------------------------------------------------------------------
void File__Base::Finalize__All(stream_t StreamKind, size_t Pos, Ztring &Z1, Ztring &Z2)
{
    if (StreamKind!=Stream_General)
    {
        //Codec
        if (!(*Stream[StreamKind])[Pos](_T("Codec")).empty())
        {
            const Ztring &C1=Config.Codec_Get((*Stream[StreamKind])[Pos](_T("Codec")));
            if (C1.empty())
            {
                if ((*Stream[StreamKind])[Pos](_T("Codec/String")).empty()) (*Stream[StreamKind])[Pos](_T("Codec/String"))=(*Stream[StreamKind])[Pos](_T("Codec"));
            }
            else
            {
                if ((*Stream[StreamKind])[Pos](_T("Codec/String")).empty()) (*Stream[StreamKind])[Pos](_T("Codec/String"))=C1;
                if ((*Stream[StreamKind])[Pos](_T("Codec/Family")).empty()) (*Stream[StreamKind])[Pos](_T("Codec/Family"))=Config.Codec_Get((*Stream[StreamKind])[Pos](_T("Codec")), InfoCodec_KindofCodec);
                if ((*Stream[StreamKind])[Pos](_T("Codec/Info"  )).empty()) (*Stream[StreamKind])[Pos](_T("Codec/Info"  ))=Config.Codec_Get((*Stream[StreamKind])[Pos](_T("Codec")), InfoCodec_Description, (stream_t)StreamKind);
                if ((*Stream[StreamKind])[Pos](_T("Codec/Url"   )).empty()) (*Stream[StreamKind])[Pos](_T("Codec/Url"   ))=Config.Codec_Get((*Stream[StreamKind])[Pos](_T("Codec")), InfoCodec_Url);
            }
        }
        //Special cases
        if ((*Stream[StreamKind])[Pos](_T("Channel(s)")).empty()
         &&((*Stream[StreamKind])[Pos](_T("Codec"))==_T("samr")
         || (*Stream[StreamKind])[Pos](_T("Codec"))==_T("sawb")
         || (*Stream[StreamKind])[Pos](_T("Codec"))==_T("7A21")
         || (*Stream[StreamKind])[Pos](_T("Codec"))==_T("7A22"))
            )
            (*Stream[StreamKind])[Pos](_T("Channel(s)")).From_Number(1); //AMR is always with 1 channel

        //Language
        //-Find 2-digit language
        (*Stream[StreamKind])[Pos](_T("Language")).MakeLowerCase();
        if ((*Stream[StreamKind])[Pos](_T("Language")).size()==3 && (*Stream[StreamKind])[Pos](_T("Language"))==_T("und"))
            (*Stream[StreamKind])[Pos](_T("Language")).clear();
        if ((*Stream[StreamKind])[Pos](_T("Language")).size()==3 && !Config.Iso639_Get((*Stream[StreamKind])[Pos](_T("Language"))).empty())
           (*Stream[StreamKind])[Pos](_T("Language"))=Config.Iso639_Get((*Stream[StreamKind])[Pos](_T("Language")));
        //-Translate
        Ztring Temp=_T("Language_"); Temp+=(*Stream[StreamKind])[Pos](_T("Language"));
        const Ztring& Z3=Config.Language_Get(Temp);
        if (!Z3.empty())
            (*Stream[StreamKind])[Pos](_T("Language/String"))=Z3;
        else
            (*Stream[StreamKind])[Pos](_T("Language/String"))=(*Stream[StreamKind])[Pos](_T("Language"));

        //Codec_List
        Z1+=(*Stream[StreamKind])[Pos](_T("Codec/String"))+_T(" / ");
        Z2+=(*Stream[StreamKind])[Pos](_T("Language/String"))+_T(" / ");

        //BitRate
        if (!(*Stream[StreamKind])[Pos](_T("StreamSize")).empty() && !(*Stream[StreamKind])[Pos](_T("PlayTime")).empty())
        {
            int64u PlayTime=(*Stream[StreamKind])[Pos](_T("PlayTime")).To_int64u();
            int64u StreamSize=(*Stream[StreamKind])[Pos](_T("StreamSize")).To_int64u();
            if (PlayTime>0 && StreamSize>0)
                (*Stream[StreamKind])[Pos](_T("BitRate")).From_Number(StreamSize*8*1000/PlayTime);
        }
    }

    //Counts
    (*Stream[StreamKind])[Pos](_T("Count")).From_Number((*Stream[StreamKind])[Pos].size());
    (*Stream[StreamKind])[Pos](_T("StreamCount")).From_Number((*Stream[StreamKind]).size());
    (*Stream[StreamKind])[Pos](_T("StreamKindID")).From_Number(Pos);

    //Strings
    size_t List_Measure_Pos=Error;
    do
    {
        List_Measure_Pos=Config.Info_Get(StreamKind).Find(_T(""), Info_Measure, List_Measure_Pos+1, _T("!="));
        if (List_Measure_Pos!=Error)
        {
            const Ztring &List_Measure_Value=Config.Info_Get(StreamKind).Read(List_Measure_Pos, Info_Measure);
            const Ztring &List_Name_Value=Config.Info_Get(StreamKind).Read(List_Measure_Pos, Info_Name);
                 if (List_Measure_Value==_T(" byte"))
                FileSize_FileSize123(List_Name_Value, (*Stream[StreamKind])[Pos]);
            else if (List_Measure_Value==_T(" bps") || List_Measure_Value==_T(" Hz"))
                Kilo_Kilo123(List_Name_Value, StreamKind, (*Stream[StreamKind])[Pos]);
            else if (List_Measure_Value==_T(" ms"))
                PlayTime_PlayTime123(List_Name_Value, (*Stream[StreamKind])[Pos]);
            else
                Value_Value123(List_Name_Value, StreamKind, (*Stream[StreamKind])[Pos]);
        }

    }
    while (List_Measure_Pos!=Error);
}

//---------------------------------------------------------------------------
void File__Base::Finalize_General(size_t)
{
    //Integrity
    if (0>=General.size())
        return;

    //General, bases
    General_Fill(); //Not always done by File_*
    if (!Config.Format_Get(General[0](_T("Format")), InfoFormat_Name).empty())
    {
        Fill(Stream_General, 0, "Format/String", Config.Format_Get(General[0](_T("Format")), InfoFormat_LongName));
        Fill(Stream_General, 0, "Format/Family", Config.Format_Get(General[0](_T("Format")), InfoFormat_Family));
        Fill(Stream_General, 0, "Format/Info", Config.Format_Get(General[0](_T("Format")), InfoFormat_Info));
        Fill(Stream_General, 0, "Format/Url", Config.Format_Get(General[0](_T("Format")), InfoFormat_Url));
        Fill(Stream_General, 0, "Format/Extensions", Config.Format_Get(General[0](_T("Format")), InfoFormat_Extensions));
    }

    //Corelation
    //-PlayTime if General not filled
    if (General[0](_T("PlayTime")).empty())
    {
        int64u PlayTime=0;
        //For all streams (Generic)
        for (size_t StreamKind=Stream_Video; StreamKind<Stream_Max; StreamKind++)
            for (size_t Pos=0; Pos<(*Stream[StreamKind]).size(); Pos++)
            {
                if (!(*Stream[StreamKind])[Pos](_T("PlayTime")).empty())
                {
                    int64u PlayTime_Stream=(*Stream[StreamKind])[Pos](_T("PlayTime")).To_int64u();
                    if (PlayTime_Stream>PlayTime)
                        PlayTime=PlayTime_Stream;
                }
            }

        //Filling
        if (PlayTime>0)
            General[0](_T("PlayTime")).From_Number(PlayTime);
    }
    //-Stream size if all stream sizes are OK
    if (General[0](_T("StreamSize")).empty())
    {
        int64u StreamSize_Total=0;
        bool IsOK=true;
        //For all streams (Generic)
        for (size_t StreamKind=Stream_Video; StreamKind<Stream_Max; StreamKind++)
        {
            if (StreamKind!=Stream_Chapters && StreamKind!=Stream_Menu) //They have no big size, we never calculate them
                for (size_t Pos=0; Pos<(*Stream[StreamKind]).size(); Pos++)
                {
                    int64u StreamSize=(*Stream[StreamKind])[Pos](_T("StreamSize")).To_int64u();
                    if (StreamSize>0)
                        StreamSize_Total+=StreamSize;
                    else if (IsOK)
                        IsOK=false; //StreamSize not available for 1 stream, we can't calculate
                }
        }

        //Filling
        if (IsOK && StreamSize_Total<File_Size)
            General[0](_T("StreamSize")).From_Number(File_Size-StreamSize_Total);
    }

    //-BitRate if we have one Audio stream with bitrate
    if (General[0](_T("PlayTime")).empty() && General[0](_T("BitRate")).empty() && Video.empty() && Audio.size()==1 && Audio[0](_T("BitRate")).To_int64u()!=0)
        General[0](_T("BitRate"))=Audio[0](_T("BitRate"));
    //-BitRate if PlayTime
    if (General[0](_T("BitRate")).empty() && General[0](_T("PlayTime")).To_int64u()!=0)
        General[0](_T("BitRate")).From_Number(General[0](_T("FileSize")).To_int64u()*8*1000/General[0](_T("PlayTime")).To_int64u());
    //-PlayTime if BitRate
    if (General[0](_T("PlayTime")).empty() && General[0](_T("BitRate")).To_int64u()!=0)
        General[0](_T("PlayTime")).From_Number(General[0](_T("FileSize")).To_int64u()*8*1000/General[0](_T("BitRate")).To_int64u());
    //-Video bitrate if we have all audio bitrates and overal bitrate
    if (Video.size()==1 && General[0](_T("BitRate")).size()>4 && Video[0](_T("BitRate")).empty()) //BitRate is > 10 000, to avoid strange behavior
    {
        int32s VideoBitRate=General[0](_T("BitRate")).To_int32s()-5000; //5000 bps because of a "classic" format overhead
        bool VideobitRateIsValid=true;
        for (size_t Pos=0; Pos<Audio.size(); Pos++)
        {
            int32s AudioBitRate=Audio[Pos](_T("BitRate")).To_int32s();
            if (AudioBitRate>0)
                VideoBitRate-=AudioBitRate-2000; //5000 bps because of a "classic" stream overhead
            else
                VideobitRateIsValid=false;
        }
        if (VideobitRateIsValid && VideoBitRate>=10000) //to avoid strange behavior
            Video[0](_T("BitRate")).From_Number((float)VideoBitRate*0.98); //Default container overhead=2%
    }

    //Counts
    General[0](_T("Count")).From_Number(General[0].size());
    General[0](_T("VideoCount")).From_Number(Video.size());
    General[0](_T("AudioCount")).From_Number(Audio.size());
    General[0](_T("TextCount")).From_Number(Text.size());
    General[0](_T("ChaptersCount")).From_Number(Chapters.size());

    //Format
    General[0](_T("Codec"))=General[0](_T("Format"));
    General[0](_T("Codec/String"))=General[0](_T("Format/String"));
    General[0](_T("Codec/Info"))=General[0](_T("Format/Info"));
    General[0](_T("Codec/Family"))=General[0](_T("Format/Family"));
    General[0](_T("Codec/Url"))=General[0](_T("Format/Url"));
    General[0](_T("Codec/Extensions"))=General[0](_T("Format/Extensions"));
    General[0](_T("Codec_Settings"))=General[0](_T("Format_Settings"));
    General[0](_T("Codec_Settings_Automatic"))=General[0](_T("Format_Settings_Automatic"));

    //Tags
    Finalize_Tags();
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Video(size_t Pos)
{
    //Integrity
    if (Pos>=Video.size())
        return;

    //FrameRate
    if (!Video[Pos](_T("FrameRate")).empty())
    {
        Ztring Z1; Z1.From_Number(Video[Pos](_T("FrameRate")).To_float32(), 3, Ztring_NoZero);
        Video[Pos](_T("FrameRate/String"))=Z1+Config.Language_Get(_T(" fps"));
    }
    //Interlacement
    if (!Video[Pos](_T("Interlacement")).empty())
    {
        Ztring &Z1=Video[Pos](_T("Interlacement"));
        if (Z1.size()==3)
            Video[Pos](_T("Interlacement/String"))=Config.Language_Get(Ztring(_T("Interlaced_"))+Z1);
        else
            Video[Pos](_T("Interlacement/String"))=Config.Language_Get(Z1);
        if (Video[Pos](_T("Interlacement/String")).empty())
            Video[Pos](_T("Interlacement/String"))=Z1;
    }
    //Aspect Ratio
    if (Video[Pos](_T("AspectRatio")).empty())
    {
        float F1=Video[Pos](_T("Height")).To_float32();
        float F2=Video[Pos](_T("Width")).To_float32();
        if (F1 && F2)
            Video[Pos](_T("AspectRatio")).From_Number(F2/F1);
    }
    if (!Video[Pos](_T("AspectRatio")).empty())
    {
        float F1=Video[Pos](_T("AspectRatio")).To_float32();
        Ztring C1;
             if (0);
        else if (F1>1.23 && F1<1.27) C1=_T("5/4");
        else if (F1>1.30 && F1<1.37) C1=_T("4/3");
        else if (F1>1.70 && F1<2.85) C1=_T("16/9");
        else if (F1>2.10 && F1<2.22) C1=_T("2.2");
        else if (F1>2.23 && F1<2.30) C1=_T("2.25");
        else if (F1>2.30 && F1<2.40) C1=_T("2.35");
        else              C1.From_Number(F1);
        Video[Pos](_T("AspectRatio/String"))=C1;
    }
    //Standard
    if (Video[Pos](_T("Standard")).empty() && Video[Pos](_T("Width"))==_T("720"))
    {
             if (Video[Pos](_T("Height"))==_T("576"))
            Video[Pos](_T("Standard"))=_T("PAL");
        else if (Video[Pos](_T("Height"))==_T("480"))
            Video[Pos](_T("Standard"))=_T("NTSC");
    }
    //Bits/(Pixel*Frame)
    if (!Video[Pos](_T("BitRate")).empty())
    {
        float F1=(float)Video[Pos](_T("Width")).To_int32s()*(float)Video[Pos](_T("Height")).To_int32s()*Video[Pos](_T("FrameRate")).To_float32();
        if (F1)
            Video[Pos](_T("Bits-(Pixel*Frame)")).From_Number(Video[Pos](_T("BitRate")).To_float32()/F1);
    }
    //FrameCount
    if (Video[Pos](_T("FrameCount")).empty())
    {
        int64s PlayTime=Video[Pos](_T("PlayTime")).To_int64s();
        if (PlayTime==0)
            PlayTime=General[0](_T("PlayTime")).To_int64s();
        float FrameRate=Video[Pos](_T("FrameRate")).To_float32();
        if (PlayTime && FrameRate)
           Video[Pos](_T("FrameCount")).From_Number(int64u(((float64)PlayTime)/1000*FrameRate));
    }
    //Playtime
    if (Video[Pos](_T("PlayTime")).empty())
    {
        int64u PlayTime=Video[Pos](_T("PlayTime")).To_int64u();
        int64u FrameRate=Video[Pos](_T("FrameRate")).To_int64u();
        if (PlayTime==0 && FrameRate!=0)
            PlayTime=Video[Pos](_T("FrameCount")).To_int64u()*1000/FrameRate;
        if (PlayTime)
           Video[Pos](_T("PlayTime")).From_Number(PlayTime);
    }

    if (!Video[Pos](_T("Encoded_Library")).empty())
    {
        Ztring Encoder=Video[Pos](_T("Encoded_Library"));
        const Ztring& Name=Config.Encoder_Get(Video[Pos](_T("Encoded_Library")));
        if (!Name.empty())
        {
            Video[Pos](_T("Encoded_Library/String"))=Name;
            Video[Pos](_T("Encoded_Library/Date"))=Config.Encoder_Get(Video[Pos](_T("Encoded_Library"), InfoEncoder_Date));
        }
        else
            Video[Pos](_T("Encoded_Library/String"))=Video[Pos](_T("Encoded_Library"));
    }
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Audio(size_t Pos)
{
    //Integrity
    if (Pos>=Audio.size())
        return;

    //SamplingCount
    if (Audio[Pos](_T("SamplingCount")).empty())
    {
        int64s PlayTime=Audio[Pos](_T("PlayTime")).To_int64s();
        if (PlayTime==0)
            PlayTime=General[0](_T("PlayTime")).To_int64s();
        float SamplingRate=Audio[Pos](_T("SamplingRate")).To_float32();
        if (PlayTime && SamplingRate)
           Audio[Pos](_T("SamplingCount")).From_Number(int64u(((float64)PlayTime)/1000*SamplingRate));
    }
    //Playtime
    if (General[0](_T("PlayTime")).empty())
    {
        int64s PlayTime=Audio[Pos](_T("PlayTime")).To_int64s();
        if (PlayTime==0 && Audio[Pos](_T("SamplingRate")).To_int64s()!=0)
            PlayTime=Audio[Pos](_T("SamplingCount")).To_int64s()*1000/Audio[Pos](_T("SamplingRate")).To_int64s();
        if (PlayTime)
           General[0](_T("PlayTime")).From_Number(PlayTime);
    }
    //Delay/Video0
    if (Video.size()>0 && !Audio[Pos](_T("Delay")).empty())
    {
        Audio[Pos](_T("Video0_Delay")).From_Number(Audio[Pos](_T("Delay")).To_int32s()-Video[0](_T("Delay")).To_int32s());
    }
    //CBR/VBR
    if (Audio[Pos](_T("BitRate_Mode")).empty() && !Audio[Pos](_T("Codec")).empty())
    {
        Ztring Z1=Config.Codec_Get(Audio[Pos](_T("Codec")), InfoCodec_BitRate_Mode);
        if (!Z1.empty())
            Audio[Pos](_T("BitRate_Mode"))=Z1;
    }
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Text(size_t)
{
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Chapters(size_t)
{
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Image(size_t)
{
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Menu(size_t)
{
}

//---------------------------------------------------------------------------
void File__Base::Finalize_Tags()
{
    //Integrity
    if (1>=General.size())
        return;

    //-Movie/Album
    if (!General[0](_T("Title")).empty() && General[0](_T("Movie")).empty() && General[0](_T("Track")).empty())
    {
        if (Video.size()!=0)
            General[0](_T("Movie"))=General[0](_T("Title"));
        else
            General[0](_T("Track"))=General[0](_T("Title"));
    }
    if (!General[0](_T("Title/More")).empty() && General[0](_T("Movie/More")).empty() && General[0](_T("Track/More")).empty())
    {
        if (Video.size()!=0)
            General[0](_T("Movie/More"))=General[0](_T("Title/More"));
        else
            General[0](_T("Track/More"))=General[0](_T("Title/More"));
    }
    if (!General[0](_T("Title/Url")).empty() && General[0](_T("Movie/Url")).empty() && General[0](_T("Track/Url")).empty())
    {
        if (Video.size()!=0)
            General[0](_T("Movie/Url"))=General[0](_T("Title/Url"));
        else
            General[0](_T("Track/Url"))=General[0](_T("Title/Url"));
    }
    //-Title
    if (General[0](_T("Title")).empty() && !General[0](_T("Movie")).empty())
        General[0](_T("Title"))=General[0](_T("Movie"));
    if (General[0](_T("Title")).empty() && !General[0](_T("Track")).empty())
        General[0](_T("Title"))=General[0](_T("Track"));
    if (General[0](_T("Title/More")).empty() && !General[0](_T("Movie/More")).empty())
        General[0](_T("Title/More"))=General[0](_T("Movie/More"));
    if (General[0](_T("Title/More")).empty() && !General[0](_T("Track/More")).empty())
        General[0](_T("Title/More"))=General[0](_T("Track/More"));
    if (General[0](_T("Title/Url")).empty() && !General[0](_T("Movie/Url")).empty())
        General[0](_T("Title/Url"))=General[0](_T("Movie/Url"));
    if (General[0](_T("Title/Url")).empty() && !General[0](_T("Track/Url")).empty())
        General[0](_T("Title/Url"))=General[0](_T("Track/Url"));

    //-Genre
    if (!General[0](_T("Genre")).empty() && General[0](_T("Genre")).size()<4 && General[0](_T("Genre"))[0]>=_T('0') && General[0](_T("Genre"))[0]<=_T('9'))
    {
        Ztring Genre;
        if (General[0](_T("Genre")).size()==1) Genre=Ztring(_T("Genre_00"))+General[0](_T("Genre"));
        if (General[0](_T("Genre")).size()==2) Genre=Ztring(_T("Genre_0" ))+General[0](_T("Genre"));
        if (General[0](_T("Genre")).size()==3) Genre=Ztring(_T("Genre_"  ))+General[0](_T("Genre"));
        General[0](_T("Genre"))=Config.Language_Get(Genre);
    }
}

//***************************************************************************
// Internal Functions
//***************************************************************************

//---------------------------------------------------------------------------
//PlayTime
void PlayTime_PlayTime123(const Ztring &Value, ZtringListList &List)
{
    if (List.Find(Value+_T("/String"))==Error || List(Value).empty())
        return;

    int32s HH, MM, SS, MS;
    Ztring PlayTimeString1, PlayTimeString2, PlayTimeString3;
    bool Negative=false;
    MS=List(Value).To_int32s(); //en ms
    if (MS<0)
    {
        Negative=true;
        MS=-MS;
    }
    if (MS==0)
        return;
    HH=MS/1000/60/60; //h
    if (HH>0)
    {
        PlayTimeString1+=Ztring::ToZtring(HH)+Config.Language_Get(_T("h"));
        PlayTimeString2+=Ztring::ToZtring(HH)+Config.Language_Get(_T("h"));
        if (HH<10)
            PlayTimeString3+=Ztring(_T("0"))+Ztring::ToZtring(HH)+_T(":");
        else
            PlayTimeString3+=Ztring::ToZtring(HH)+_T(":");
        MS-=HH*60*60*1000;
    }
    if (HH==0)
    {
        PlayTimeString3+=_T("00:");
    }

    MM=MS/1000/60; //mn
    if (MM>0)
    {
        if (PlayTimeString1.size()>0)
            PlayTimeString1+=_T(" ");
        PlayTimeString1+=Ztring::ToZtring(MM)+Config.Language_Get(_T("mn"));
        if (PlayTimeString2.size()<5)
        {
            if (PlayTimeString2.size()>0)
                PlayTimeString2+=_T(" ");
            PlayTimeString2+=Ztring::ToZtring(MM)+Config.Language_Get(_T("mn"));
        }
        if (MM<10)
            PlayTimeString3+=Ztring(_T("0"))+Ztring::ToZtring(MM)+_T(":");
        else
            PlayTimeString3+=Ztring::ToZtring(MM)+_T(":");
        MS-=MM*60*1000;
    }
    if (MM==0)
    {
        PlayTimeString3+=_T("00:");
    }

    SS=MS/1000; //s
    if (SS>0)
    {
        if (PlayTimeString1.size()>0)
            PlayTimeString1+=_T(" ");
        PlayTimeString1+=Ztring::ToZtring(SS)+Config.Language_Get(_T("s"));
        if (PlayTimeString2.size()<5)
        {
            if (PlayTimeString2.size()>0)
                PlayTimeString2+=_T(" ");
            PlayTimeString2+=Ztring::ToZtring(SS)+Config.Language_Get(_T("s"));
        }
        else if (PlayTimeString2.size()==0)
            PlayTimeString2+=Ztring::ToZtring(SS)+Config.Language_Get(_T("s"));
        if (SS<10)
            PlayTimeString3+=Ztring(_T("0"))+Ztring::ToZtring(SS)+_T(".");
        else
            PlayTimeString3+=Ztring::ToZtring(SS)+_T(".");
        MS-=SS*1000;
    }
    if (SS==0)
    {
        PlayTimeString3+=_T("00.");
    }

    if (MS>0) //ms
    {
        if (PlayTimeString1.size()>0)
            PlayTimeString1+=_T(" ");
        PlayTimeString1+=Ztring::ToZtring(MS)+Config.Language_Get(_T("ms"));
        if (PlayTimeString2.size()<5)
        {
            if (PlayTimeString2.size()>0)
                PlayTimeString2+=_T(" ");
            PlayTimeString2+=Ztring::ToZtring(MS)+Config.Language_Get(_T("ms"));
        }
        if (MS<10)
            PlayTimeString3+=Ztring(_T("00"))+Ztring::ToZtring(MS);
        else if (MS<100)
            PlayTimeString3+=Ztring(_T("0"))+Ztring::ToZtring(MS);
        else
            PlayTimeString3+=Ztring::ToZtring(MS);
    }
    if (MS==0)
    {
        PlayTimeString3+=_T("000");
    }

    if (Negative)
    {
        PlayTimeString1=Ztring(_T("-"))+PlayTimeString1;
        PlayTimeString2=Ztring(_T("-"))+PlayTimeString2;
        PlayTimeString3=Ztring(_T("-"))+PlayTimeString3;
    }

    List(Value+_T("/String"))=PlayTimeString2;
    List(Value+_T("/String1"))=PlayTimeString1;
    List(Value+_T("/String2"))=PlayTimeString2;
    List(Value+_T("/String3"))=PlayTimeString3;
}

//---------------------------------------------------------------------------
//FileSize
void FileSize_FileSize123(const Ztring &Value, ZtringListList &List)
{
    if (List.Find(Value+_T("/String"))==Error || List(Value).empty())
        return;

    float F1=(float)List(Value).To_int64s(); //Video C++ 6 patch, should be int64u
    //--Bytes, KiB, MiB or GiB...
    int32u Pow3=0;
    while(F1>=1024)
    {
        F1/=1024;
        Pow3++;
    }
    //--Count of digits
    int32u I2, I3, I4;
         if (F1>=100)
    {
        I2=0;
        I3=0;
        I4=1;
    }
    else if (F1>=10)
    {
        I2=0;
        I3=1;
        I4=2;
    }
    else //if (F1>=1)
    {
        I2=1;
        I3=2;
        I4=3;
    }
    Ztring Measure;
    switch (Pow3)
    {
        case  0 : Measure=Config.Language_Get(Ztring::ToZtring(F1, 0), _T(" Byte")); Measure.FindAndReplace(Ztring::ToZtring(F1, 0), _T("")); break; //This is only to have measure with multiple plurals
        case  1 : Measure=Config.Language_Get(_T(" KiB")); break;
        case  2 : Measure=Config.Language_Get(_T(" MiB")); break;
        case  3 : Measure=Config.Language_Get(_T(" GiB")); break;
        default : Measure=Config.Language_Get(_T(" ????Bytes"));
    }
    List(Value+_T("/String1"))=Ztring::ToZtring(F1, 0)+Measure;
    List(Value+_T("/String2"))=Ztring::ToZtring(F1, I2)+Measure;
    List(Value+_T("/String3"))=Ztring::ToZtring(F1, I3)+Measure;
    List(Value+_T("/String4"))=Ztring::ToZtring(F1, I4)+Measure;
    List(Value+_T("/String"))=List(Value+_T("/String3"));
}

//---------------------------------------------------------------------------
//FileSize
void Kilo_Kilo123(const Ztring &Value, stream_t StreamKind, ZtringListList &List)
{
    if (List.Find(Value+_T("/String"))==Error || List(Value).empty())
        return;

    int32u BitRate=List(Value).To_int32u();
    if (BitRate>10000000)
    {
        Ztring Measure=Config.Info_Get(StreamKind).Read(Value, Info_Measure);
        Measure.insert(1, _T("M"));
        List(Value+_T("/String"))=Ztring::ToZtring(float32_int32s((float)BitRate/1000000))+Config.Language_Get(Measure);
    }
    else if (BitRate>10000)
    {
        Ztring Measure=Config.Info_Get(StreamKind).Read(Value, Info_Measure);
        Measure.insert(1, _T("K"));
        List(Value+_T("/String"))=Ztring::ToZtring(float32_int32s((float)BitRate/1000))+Config.Language_Get(Measure);
    }
    else if (BitRate>0)
        List(Value+_T("/String"))=Ztring::ToZtring(BitRate)+Config.Language_Get(Config.Info_Get(StreamKind).Read(Value, Info_Measure));
}

//---------------------------------------------------------------------------
//Value --> Value with measure
void Value_Value123(const Ztring &Value, stream_t StreamKind, ZtringListList &List)
{
    //String wanted?
    if (List.Find(Value+_T("/String"))==Error || List(Value).empty())
        return;

    //String needed?
    if (List(Value).empty())
        return;

    //Filling
    List(Value+_T("/String"))=Config.Language_Get(List(Value), Config.Info_Get(StreamKind).Read(Value, Info_Measure));
}

} //NameSpace

