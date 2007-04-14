// ZenLib::bitStream - Read bit per bit
// Copyright (C) 2006-2006 Jerome Martinez, Zen@MediaArea.net
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
// Read a stream bit per bit
// Can read up to 32 bits at once
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef ZenBitStreamH
#define ZenBitStreamH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <ZenLib/Conf.h>
//---------------------------------------------------------------------------

namespace ZenLib
{

#ifndef MIN
    #define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

class BitStream
{
public:
    BitStream ()                                                                {Attach(NULL, 0);};
    BitStream (const int8u* Buffer_, size_t Size_)                              {Attach(Buffer_, Size_);};
   ~BitStream ()                                                                {};

    void Attach(const int8u* Buffer_, size_t Size_)
    {
        Buffer=Buffer_;
        Buffer_Size=Buffer_Size_Init=Size_*8; //Size is in bits
        LastByte_Size=0;
        BufferUnderRun=false;
        BookMark=false;
    };

    int32u Get (int8u HowMany)
    {
        int32u ToReturn;
        static const int32u Mask[33]={
          0x00000000,
          0x00000001, 0x00000003, 0x00000007, 0x0000000f,
          0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
          0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
          0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
          0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
          0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
          0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
          0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
        };

        if (HowMany==0 || HowMany>32)
            return 0;          
        if ((size_t)HowMany>Buffer_Size+LastByte_Size)
        {
            Buffer_Size=0;
            LastByte_Size=0;
            BufferUnderRun=true;
            return 0;
        }

        if (HowMany<=LastByte_Size)
        {
            LastByte_Size-=HowMany;
            ToReturn=LastByte>>LastByte_Size;
        }
        else
        {
            int32u NewBits=HowMany-LastByte_Size;
            if (NewBits==32)
                ToReturn=0;
            else
                ToReturn=LastByte<<NewBits;
            switch ((NewBits-1)/8)
            {
                case 3 :    NewBits-=8;
                            ToReturn|=*Buffer<<NewBits;
                            Buffer++;
                            Buffer_Size-=8;
                case 2 :    NewBits-=8;
                            ToReturn|=*Buffer<<NewBits;
                            Buffer++;
                            Buffer_Size-=8;
                case 1 :    NewBits-=8;
                            ToReturn|=*Buffer<<NewBits;
                            Buffer++;
                            Buffer_Size-=8;
                case 0 :
                            LastByte=*Buffer;
                            Buffer++;
            }
            LastByte_Size=MIN(8, Buffer_Size)-NewBits;
            Buffer_Size -=MIN(8, Buffer_Size);
            ToReturn|=(LastByte>>LastByte_Size)&Mask[NewBits];
        }
        return ToReturn&Mask[HowMany];
    };

    void Skip (int8u HowMany)
    {
        if (HowMany==0 || HowMany>32)
            return;
        if ((size_t)HowMany>Buffer_Size+LastByte_Size)
        {
            Buffer_Size=0;
            LastByte_Size=0;
            BufferUnderRun=true;
            return;
        }

        if (HowMany<=LastByte_Size)
            LastByte_Size-=HowMany;
        else
        {
            int32u NewBits=HowMany-LastByte_Size;
            switch ((NewBits-1)/8)
            {
                case 3 :    NewBits-=8;
                            Buffer++;
                            Buffer_Size-=8;
                case 2 :    NewBits-=8;
                            Buffer++;
                            Buffer_Size-=8;
                case 1 :    NewBits-=8;
                            Buffer++;
                            Buffer_Size-=8;
                case 0 :
                            LastByte=*Buffer;
                            Buffer++;
            }
            LastByte_Size=MIN(8, Buffer_Size)-NewBits;
            Buffer_Size -=MIN(8, Buffer_Size);
        }
    };

    int32u Peek(int8u HowMany)
    {
        BookMarkPos(true);
        int32u ToReturn=Get(HowMany);
        BookMarkPos(false);
        return ToReturn;
    }

    void BookMarkPos(bool ToSet)
    {
        if (ToSet)
        {
            BookMark=1;
            Buffer_BookMark=Buffer;
            Buffer_Size_BookMark=Buffer_Size;
            LastByte_BookMark=LastByte;
            LastByte_Size_BookMark=LastByte_Size;
            BufferUnderRun_BookMark=BufferUnderRun;
        }
        else
        {
            BookMark=0;
            Buffer=Buffer_BookMark;
            Buffer_Size=Buffer_Size_BookMark;
            LastByte=LastByte_BookMark;
            LastByte_Size=LastByte_Size_BookMark;
            BufferUnderRun=BufferUnderRun_BookMark;
        }
    };

    int Remain () //How many bits remain?
    {
        return Buffer_Size+LastByte_Size;
    };

    void Byte_Align()
    {
        Get(LastByte_Size);
    };

    int32u Offset_Get()
    {
        if (BufferUnderRun)
            return Error;
        return (Buffer_Size_Init-Buffer_Size)/8;
    };

private :
    const int8u*    Buffer;
    size_t          Buffer_Size;
    size_t          Buffer_Size_Init;
    int8u           LastByte;
    int8u           LastByte_Size;
    bool            BufferUnderRun;

    bool            BookMark;
    const int8u*    Buffer_BookMark;
    size_t          Buffer_Size_BookMark;
    int8u           LastByte_BookMark;
    int8u           LastByte_Size_BookMark;
    bool            BufferUnderRun_BookMark;
};

} //namespace ZenLib
#endif
