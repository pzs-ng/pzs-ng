// File_Mk - Info for Matroska files
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
#if defined(MEDIAINFO_MK_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mk.h"
#ifdef _WIN32
    #include <ebml/WinIOCallback.h>
#else
    #include <ebml/StdIOCallback.h>
#endif
#include <ebml/EbmlTypes.h>
#include <ebml/EbmlHead.h>
#include <ebml/EbmlVoid.h>
#include <ebml/EbmlCrc32.h>
#include <ebml/EbmlSubHead.h>
#include <ebml/EbmlStream.h>
#include <ebml/EbmlBinary.h>
#include <ebml/EbmlString.h>
#include <ebml/EbmlUnicodeString.h>
#include <ebml/EbmlContexts.h>
#include <ebml/EbmlVersion.h>

// libmatroska includes
#include <matroska/KaxConfig.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxContexts.h>
#include <matroska/KaxChapters.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxInfo.h>
#include <matroska/KaxInfoData.h>
#include <matroska/KaxTags.h>
#include <matroska/KaxTag.h>
#include <matroska/KaxTagMulti.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxClusterData.h>
#include <matroska/KaxTrackAudio.h>
#include <matroska/KaxTrackVideo.h>
#include <matroska/KaxAttachments.h>
#include <matroska/KaxAttached.h>
#include <matroska/KaxVersion.h>

#include <ZenLib/Utils.h>
#include "wx/strconv.h"
using namespace ZenLib;
using namespace libebml;
using namespace libmatroska;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
//To clarify the code
#define BEGIN \
UpperElementLevel=0; \
Level++; \
ElementLevel[Level]=Stream.FindNextElement(ElementLevel[Level-1]->Generic().Context, UpperElementLevel, 0xFFFFFFFFFFFFFFFFLL, 0); \
while (!ShouldStop && ElementLevel[Level]!=NULL && UpperElementLevel<=0 && File.getFilePointer()<ElementLevel[Level-1]->GetElementPosition()+ElementLevel[Level-1]->HeadSize()+ElementLevel[Level-1]->GetSize()) \
{ \
        if(0){ \

//-For each Ebml Atom (and this is a value, not an atom with other atoms in it)
#define EBML(_KAX) \
    } \
    else if (EbmlId(*ElementLevel[Level])==E##_KAX::ClassInfos.GlobalId) \
    { \
        FLUSH(); \
        E##_KAX* Kax=static_cast<E##_KAX*>(ElementLevel[Level]); \
        Kax->ReadData(Stream.I_O()); \
        if (Config.Details_Get()) \
            Details_Add_Element(Level+1, #_KAX); \
        FLUSH(); \

//-For each Matroska Atom (and this is a value, not an atom with other atoms in it)
#define KAX(_KAX) \
    } \
    else if (EbmlId(*ElementLevel[Level])==Kax##_KAX::ClassInfos.GlobalId) \
    { \
        FLUSH(); \
        Kax##_KAX* Kax=static_cast<Kax##_KAX*>(ElementLevel[Level]); \
        Kax->ReadData(Stream.I_O()); \
        if (Config.Details_Get()) \
            Details_Add_Element(Level+1, #_KAX); \
        FLUSH(); \

//-For each Matroska Atom (and this is an Atom with other atoms in it, not a value)
#define SUB(_KAX) \
    } \
    else if (EbmlId(*ElementLevel[Level])==Kax##_KAX::ClassInfos.GlobalId) \
    { \
        FLUSH(); \
        if (Config.Details_Get()) \
            Details_Add_Element(Level+1, #_KAX); \

//-End to find an atom
#define END \
    } \
    if (UpperElementLevel>0) \
    { \
        delete ElementLevel[Level]; \
        ElementLevel[Level]=ElementLevel[Level+1]; \
        UpperElementLevel--; \
    } \
    else \
    { \
        ElementLevel[Level]->SkipData(Stream, ElementLevel[Level]->Generic().Context); \
        delete ElementLevel[Level]; ElementLevel[Level]=NULL; \
        if (!ShouldStop && File.getFilePointer()<ElementLevel[Level-1]->GetElementPosition()+ElementLevel[Level-1]->HeadSize()+ElementLevel[Level-1]->GetSize()) \
            ElementLevel[Level]=Stream.FindNextElement(ElementLevel[Level-1]->Generic().Context, UpperElementLevel, (int32u)-1, 0); \
    } \
} \
Level--; \

#define DETAIL(_DETAIL) \
    if (Config.Details_Get()) \
    { \
        Details_Add_Info(Error, "Value", _DETAIL); \
    } \

#define DETAIL_FILL(_NAME, _DETAIL) \
    DETAIL(_DETAIL) \
    Fill(_NAME, _DETAIL);\

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef struct //Microsoft Video structure (video codec private)
{
   int32s  Size;
   int32s  Width;
   int32s  Height;
   int16u  Planes;
   int16u  BitCount;
   int32u  Compression;
   int32u  SizeImage;
   int32s  XPelsPerMeter;
   int32s  YPelsPerMeter;
   int32u  ClrUsed;
   int32u  ClrImportant;
} Mk_VideoInfo_MS;

typedef struct //Microsoft Audio structure (audio codec private)
{
    int16u FormatTag;
    int16u Channels;
    int32u SamplesPerSec;
    int32u AvgBytesPerSec;
    int16u BlockAlign;
    int16u BitsPerSample;
    int16u Size;
} Mk_AudioInfo_MS;
//---------------------------------------------------------------------------

//***************************************************************************
// Format
//***************************************************************************

void File_Mk::Read_File()
{
    EbmlElement* ElementLevel[9]; //Must be here because we need of them in catch(...)

    try {
    #ifdef _WIN32
        WinIOCallback File(CompleteFileName.c_str(), MODE_READ);
    #else
        if (wxConvCurrent->cWX2MB(CompleteFileName.c_str()).data()==NULL)
            return;
        StdIOCallback File(wxConvCurrent->cWX2MB(CompleteFileName.c_str()), MODE_READ); //TODO : Unicode FileName
    #endif
    EbmlStream Stream(File);

    bool ShouldStop=false;
    int Level=0;
    int UpperElementLevel=0;
    int64u TimecodeScale=0;
    double Duration=0;
    for (size_t Pos=0; Pos<9; Pos++) ElementLevel[Pos]=NULL;

    //Test if the file is Matroska
    bool IsMatroska=false;
    ElementLevel[0]=Stream.FindNextID(EbmlHead::ClassInfos, 0xFFFFFFFFFFFFFFFFLL);
    if (ElementLevel[0] && EbmlId(*ElementLevel[0])==EbmlHead::ClassInfos.GlobalId && File.getFilePointer()<ElementLevel[0]->GetElementPosition()+ElementLevel[0]->HeadSize()+ElementLevel[0]->GetSize())
    {
        if (Config.Details_Get())
            Details_Add_Element(Level+1, "EbmlHead");
        BEGIN
        EBML(DocType)
            DETAIL(*Kax)
            if (std::string(*Kax)=="matroska")
                IsMatroska=true;
        END
    }
    if (ElementLevel[0])
    {
        ElementLevel[0]->SkipData(Stream, ElementLevel[0]->Generic().Context);
        delete ElementLevel[0]; ElementLevel[0]=NULL;
    }

    if (!IsMatroska)
        return;

    Stream_Prepare(Stream_General);
    Fill("Format", "Matroska");

    //Segment
    ElementLevel[0]=Stream.FindNextID(KaxSegment::ClassInfos, 0xFFFFFFFFFFFFFFFFLL);
    if (ElementLevel[0] && EbmlId(*ElementLevel[0])==KaxSegment::ClassInfos.GlobalId)
    {
        BEGIN
        SUB(Info)
            BEGIN
            KAX(TimecodeScale)
                DETAIL(uint64(*Kax))
                if (Duration!=0)
                {
                    Fill("PlayTime", Duration * uint64(*Kax) / 1000000.0);
                    TimecodeScale=0;
                }
                else
                    TimecodeScale=uint64(*Kax);
            KAX(Duration)
                DETAIL(double(*Kax))
                if (TimecodeScale!=0)
                {
                    Fill("PlayTime", double(*Kax)* int64u_float64(TimecodeScale) / 1000000.0);
                    TimecodeScale=0;
                }
                else
                    Duration=double(*Kax);
            KAX(DateUTC)
                DETAIL(Kax->GetEpochDate())
                Fill("Encoded_Date", Ztring().Date_From_Seconds_1970(Kax->GetEpochDate()));
            KAX(Title)
                DETAIL(UTFstring(*Kax).c_str())
                Fill("Title", UTFstring(*Kax).c_str());
            KAX(MuxingApp)
                DETAIL(UTFstring(*Kax).c_str())
                Fill("Encoded_Library", UTFstring(*Kax).c_str());
            KAX(WritingApp)
                DETAIL(UTFstring(*Kax).c_str())
                Fill("Encoded_Application", UTFstring(*Kax).c_str());
            END
            if (Duration!=0 && TimecodeScale==0)
                Fill("PlayTime", Duration);
            else
                TimecodeScale=1000000; //Default value
        SUB(Cluster)
            ShouldStop=true;
        SUB(Tracks)
            BEGIN
            SUB(TrackEntry)
                Stream_Prepare(Stream_Max);
                int32u TrackDefaultDuration=0;
                std::string Codec;
                Ztring CodecPrivate_Codec_4CC;
                Ztring CodecPrivate_Codec_2CC;
                int32u CodecPrivate_Width=0;
                int32u CodecPrivate_Height=0;
                int32u CodecPrivate_Resolution=0;
                BEGIN
                KAX(TrackNumber)
                    DETAIL((uint32)(*Kax))
                    Fill("ID", (uint32)(*Kax));
                KAX(TrackUID)
                    DETAIL((uint32)(*Kax))
                    Fill("UniqueID", (uint32)(*Kax));
                KAX(TrackType)
                    DETAIL((uint8)(*Kax))
                    switch(uint8(*Kax))
                    {
                        case track_video:    Stream_Prepare(Stream_Video); break;
                        case track_audio:    Stream_Prepare(Stream_Audio); break;
                        case track_subtitle: Stream_Prepare(Stream_Text ); break;
                        default:             ;
                    }
                KAX(TrackDefaultDuration)
                    DETAIL((uint32)(*Kax))
                    TrackDefaultDuration=(uint32)(*Kax);
                KAX(TrackName)
                    DETAIL(UTFstring(*Kax).c_str())
                    Fill("Title", UTFstring(*Kax));
                KAX(TrackLanguage)
                    DETAIL(std::string(*Kax))
                    Fill("Language", std::string(*Kax));
                KAX(CodecID)
                    DETAIL(std::string(*Kax))
                    Codec=std::string(*Kax);
                    Fill("Codec", std::string(*Kax));
                KAX(CodecPrivate)
                    //MS Windows video codecs are in private element of codec
                         if (Kax->GetSize()==sizeof(Mk_VideoInfo_MS))
                    {
                        Mk_VideoInfo_MS *VideoInfo=(Mk_VideoInfo_MS*)Kax->GetBuffer();

                        CodecPrivate_Codec_4CC.From_Local((char*)&(VideoInfo->Compression), 0, 4);
                        CodecPrivate_Width=VideoInfo->Width;
                        CodecPrivate_Height=VideoInfo->Height;
                        CodecPrivate_Resolution=VideoInfo->BitCount;
                    }
                    //MS Windows audio codecs are in private element of codec
                    else if (Kax->GetSize()==sizeof(Mk_AudioInfo_MS))
                    {
                        Mk_AudioInfo_MS *AudioInfo=(Mk_AudioInfo_MS*)Kax->GetBuffer();
                        CodecPrivate_Codec_2CC.From_Number(AudioInfo->FormatTag, 16).MakeUpperCase();
                    }
                KAX(CodecName)
                    DETAIL(UTFstring(*Kax).c_str())
                    Fill("Codec/String", UTFstring(*Kax).c_str());
                SUB(TrackVideo)
                    double DisplayWidth=0;
                    double DisplayHeight=0;
                    BEGIN
                    KAX(VideoPixelWidth)
                        DETAIL((uint16)(*Kax))
                        Fill("Width", (uint16)(*Kax));
                    KAX(VideoPixelHeight)
                        DETAIL((uint16)(*Kax))
                        Fill("Height", (uint16)(*Kax));
                    KAX(VideoFrameRate)
                        DETAIL((double)(*Kax))
                        Fill("FrameRate", (double)(*Kax), 3);
                    KAX(VideoDisplayWidth)
                        DETAIL((uint16)(*Kax))
                        DisplayWidth=(uint16)(*Kax);
                    KAX(VideoDisplayHeight)
                        DETAIL((uint16)(*Kax))
                        DisplayHeight=(uint16)(*Kax);
                    END
                    if (DisplayWidth && DisplayHeight)
                        Fill("AspectRatio", ((float)DisplayWidth)/DisplayHeight);
                SUB(TrackAudio)
                    BEGIN
                    KAX(AudioSamplingFreq)
                        DETAIL((double)(*Kax))
                        Fill("SamplingRate", (double)(*Kax), 0);
                    KAX(AudioOutputSamplingFreq)
                        DETAIL((double)(*Kax))
                        Fill("SamplingRate", (double)(*Kax), 0);
                    KAX(AudioChannels)
                        DETAIL((uint16)(*Kax))
                        Fill("Channel(s)", (uint16)(*Kax));
                    KAX(AudioBitDepth)
                        DETAIL((uint16)(*Kax))
                        Fill("Resolution", (uint16)(*Kax));
                    END
                END
                //MS Windows video codecs are in private element of codec
                     if (Codec=="V_MS/VFW/FOURCC" && !CodecPrivate_Codec_4CC.empty())
                {
                    Fill("Codec", "");      Fill("Codec",       CodecPrivate_Codec_4CC);
                    Fill("Width", "");      Fill("Width",       CodecPrivate_Width);
                    Fill("Height", "");     Fill("Height",      CodecPrivate_Height);
                    Fill("Resolution", ""); Fill("Resolution",  CodecPrivate_Resolution);
                }
                //MS Windows audio codecs are in private element of codec
                else if (Codec=="A_MS/ACM" && !CodecPrivate_Codec_4CC.empty())
                {
                    Fill("Codec", "");      Fill("Codec",       CodecPrivate_Codec_2CC);
                }
                //FrameRate
                if (TrackDefaultDuration && StreamKind_Last==Stream_Video)
                    Fill("FrameRate", 1000000000.0/TrackDefaultDuration);
            END
        SUB(Cues)
        SUB(Attachments)
            BEGIN
            SUB(Attached)
                Ztring CoverName;
                BEGIN
                KAX(FileName)
                    if (CoverName.empty()) CoverName=UTFstring(*Kax).c_str(); //FileName is not a priority
                KAX(FileDescription)
                    CoverName=UTFstring(*Kax).c_str();
                END
                if (!CoverName.empty())
                {
                    if (!General[0](_T("Cover")).empty())
                        General[0](_T("Cover"))+=_T(" / ");
                    General[0](_T("Cover"))+=CoverName;
                }
            END
        SUB(Chapters)
            BEGIN
            SUB(EditionEntry)
                size_t Chapters_Pos=Stream_Prepare(Stream_Chapters);
                size_t Pos=0;
                BEGIN
                KAX(EditionUID)
                    Chapters[Chapters_Pos](_T("UniqueID")).From_Number((uint32)(*Kax));
                SUB(ChapterAtom)
                    Pos++;
                    BEGIN
                    KAX(ChapterTimeStart)
                        if (TimecodeScale==0)
                            TimecodeScale=1000000; //Default Value
                        Chapters[Chapters_Pos](Ztring::ToZtring(Pos)).Duration_From_Milliseconds((uint64)(*Kax)/int64u(TimecodeScale)); Chapters[Chapters_Pos](Ztring::ToZtring(Pos))+=_T(" ");
                    SUB(ChapterDisplay)
                        BEGIN
                        KAX(ChapterString)
                            Chapters[Chapters_Pos](Ztring::ToZtring(Pos))+=UTFstring(*Kax).c_str();
                        KAX(ChapterLanguage)
                            Chapters[Chapters_Pos](_T("Language")).From_Local(std::string(*Kax).c_str());
                        END
                    END
                END
            END
        SUB(Tags)
            BEGIN
            SUB(Tag)
                BEGIN
                SUB(TagSimple)
                    Ztring Name=_T("XXX");
                    BEGIN
                    KAX(TagName)
                        Name=UTFstring(*Kax).c_str();
                    KAX(TagString)
                        General[0](Name)=UTFstring(*Kax).c_str();
                    END
                END
            END
        END
    }
    //TODO : if multiple segments
    delete ElementLevel[0]; ElementLevel[0]=NULL;

    return;

    }
    catch (...)
    {
        for (size_t Pos=0; Pos<9; Pos++)
            delete ElementLevel[Pos];
        return;
    }
}

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mk::HowTo(stream_t StreamKind)
{
    switch (StreamKind)
    {
        case (Stream_General) :
            Fill_HowTo("Format", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("PlayTime", "R");
            Fill_HowTo("Domain", "");
            Fill_HowTo("Collection", "");
            Fill_HowTo("Season", "");
            Fill_HowTo("Movie", "");
            Fill_HowTo("Movie/More", "");
            Fill_HowTo("Album", "");
            Fill_HowTo("Comic", "");
            Fill_HowTo("Comic/Position_Total", "");
            Fill_HowTo("Part", "");
            Fill_HowTo("Part/Position", "");
            Fill_HowTo("Part/Position_Total", "");
            Fill_HowTo("Track", "");
            Fill_HowTo("Track/More", "");
            Fill_HowTo("Track/Sort", "");
            Fill_HowTo("Track/Position", "");
            Fill_HowTo("Track/Position_Total", "");
            Fill_HowTo("Chapter", "");
            Fill_HowTo("SubTrack", "");
            Fill_HowTo("Original/Album", "");
            Fill_HowTo("Original/Movie", "");
            Fill_HowTo("Original/Part", "");
            Fill_HowTo("Original/Track", "");
            Fill_HowTo("Performer", "");
            Fill_HowTo("Artist", "");
            Fill_HowTo("Performer/Sort", "");
            Fill_HowTo("Performer/Url", "");
            Fill_HowTo("Original/Performer", "");
            Fill_HowTo("Accompaniment", "");
            Fill_HowTo("Composer", "");
            Fill_HowTo("Composer/Nationality", "");
            Fill_HowTo("Arranger", "");
            Fill_HowTo("Lyricist", "");
            Fill_HowTo("Original/Lyricist", "");
            Fill_HowTo("Conductor", "");
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
            Fill_HowTo("EncodedBy", "");
            Fill_HowTo("RemixedBy", "");
            Fill_HowTo("ProductionStudio", "");
            Fill_HowTo("ThanksTo", "");
            Fill_HowTo("Publisher", "");
            Fill_HowTo("Publisher/URL", "");
            Fill_HowTo("Label", "");
            Fill_HowTo("Genre", "");
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
            Fill_HowTo("Recorded_Date", "");
            Fill_HowTo("Encoded_Date", "");
            Fill_HowTo("Tagged_Date", "");
            Fill_HowTo("Written_Date", "");
            Fill_HowTo("Mastered_Date", "");
            Fill_HowTo("Recorded_Location", "");
            Fill_HowTo("Written_Location", "");
            Fill_HowTo("Archival_Location", "");
            Fill_HowTo("Comment", "");
            Fill_HowTo("Rating ", "");
            Fill_HowTo("Encoded_Application", "");
            Fill_HowTo("Encoded_Application/Url", "");
            Fill_HowTo("Encoded_Library", "");
            Fill_HowTo("Encoded_Library_Settings", "");
            Fill_HowTo("BPM", "");
            Fill_HowTo("ISRC", "");
            Fill_HowTo("ISBN", "");
            Fill_HowTo("BarCode", "");
            Fill_HowTo("LCCN", "");
            Fill_HowTo("CatalogNumber", "");
            Fill_HowTo("LabelCode", "");
            Fill_HowTo("Copyright", "");
            Fill_HowTo("Copyright/Url", "");
            Fill_HowTo("Producer_Copyright", "");
            Fill_HowTo("TermsOfUse", "");
            Fill_HowTo("Cover", "");
            Fill_HowTo("Cover_Datas", "");
            break;
        case (Stream_Video) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("FrameRate", "R");
            Fill_HowTo("FrameCount", "R");
            Fill_HowTo("Width", "R");
            Fill_HowTo("Height", "R");
            Fill_HowTo("AspectRatio", "R");
            Fill_HowTo("BitRate", "R");
            break;
        case (Stream_Audio) :
            Fill_HowTo("Codec", "R");
            Fill_HowTo("BitRate", "R");
            Fill_HowTo("Channel(s)", "R");
            Fill_HowTo("SamplingRate", "R");
            Fill_HowTo("Resolution", "R");
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

#endif //MEDIAINFO_MK_YES






















