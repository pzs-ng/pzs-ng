// ZenLib::Utils - Very small utilities
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
// ZenUtils
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Integer and float manipulation
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef ZenUtilsH
#define ZenUtilsH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <ZenLib/Conf.h>
//---------------------------------------------------------------------------

namespace ZenLib
{

//***************************************************************************
// Integer transformations
//***************************************************************************

//---------------------------------------------------------------------------
//Little Endians
int8s  LittleEndian2int8s  (const char* List);
int8u  LittleEndian2int8u  (const char* List);
int16s LittleEndian2int16s (const char* List);
int16u LittleEndian2int16u (const char* List);
int32s LittleEndian2int32s (const char* List);
int32u LittleEndian2int32u (const char* List);
#if (MAXTYPE_INT >= 64)
int64s LittleEndian2int64s (const char* List);
int64u LittleEndian2int64u (const char* List);
#endif
#ifndef __BORLANDC__
inline int8s  LittleEndian2int8s  (const int8u* List) {return LittleEndian2int8s  ((const char*)List);}
inline int8u  LittleEndian2int8u  (const int8u* List) {return LittleEndian2int8u  ((const char*)List);}
inline int16s LittleEndian2int16s (const int8u* List) {return LittleEndian2int16s ((const char*)List);}
inline int16u LittleEndian2int16u (const int8u* List) {return LittleEndian2int16u ((const char*)List);}
inline int32s LittleEndian2int32s (const int8u* List) {return LittleEndian2int32s ((const char*)List);}
inline int32u LittleEndian2int32u (const int8u* List) {return LittleEndian2int32u ((const char*)List);}
#if (MAXTYPE_INT >= 64)
inline int64s LittleEndian2int64s (const int8u* List) {return LittleEndian2int64s ((const char*)List);}
inline int64u LittleEndian2int64u (const int8u* List) {return LittleEndian2int64u ((const char*)List);}
#endif
#endif //__BORLANDC__
//---------------------------------------------------------------------------
//Big Endians
int8s  BigEndian2int8s     (const char* List);
int8u  BigEndian2int8u     (const char* List);
int16s BigEndian2int16s    (const char* List);
int16u BigEndian2int16u    (const char* List);
int32s BigEndian2int32s    (const char* List);
int32u BigEndian2int32u    (const char* List);
#if (MAXTYPE_INT >= 64)
int64s BigEndian2int64s    (const char* List);
int64u BigEndian2int64u    (const char* List);
#endif
#ifndef __BORLANDC__
inline int8s  BigEndian2int8s     (const int8u* List) {return BigEndian2int8s     ((const char*)List);}
inline int8u  BigEndian2int8u     (const int8u* List) {return BigEndian2int8u     ((const char*)List);}
inline int16s BigEndian2int16s    (const int8u* List) {return BigEndian2int16s    ((const char*)List);}
inline int16u BigEndian2int16u    (const int8u* List) {return BigEndian2int16u    ((const char*)List);}
inline int32s BigEndian2int32s    (const int8u* List) {return BigEndian2int32s    ((const char*)List);}
inline int32u BigEndian2int32u    (const int8u* List) {return BigEndian2int32u    ((const char*)List);}
#if (MAXTYPE_INT >= 64)
inline int64s BigEndian2int64s    (const int8u* List) {return BigEndian2int64s    ((const char*)List);}
inline int64u BigEndian2int64u    (const int8u* List) {return BigEndian2int64u    ((const char*)List);}
#endif
#endif //__BORLANDC__

//---------------------------------------------------------------------------
// int32 - int64
#if (MAXTYPE_INT >= 64)
    int64s int32s_int64s (                int32s  High, int32u  Low);
    int64u int32u_int64u (                int32u  High, int32u  Low);
    void   int32s_int64s (int64s &BigInt, int32s  High, int32u  Low);
    void   int32u_int64u (int64s &BigInt, int32u  High, int32u  Low);
    void   int64s_int32s (int64s  BigInt, int32s &High, int32u &Low);
    void   int64u_int32u (int64u  BigInt, int32u &High, int32u &Low);
#endif

//---------------------------------------------------------------------------
// Floats and ints
int32s float32_int32s  (float32 F, bool Rounded=true);
int64s float32_int64s  (float32 F, bool Rounded=true);
int32s float64_int32s  (float64 F, bool Rounded=true);
int64s float64_int64s  (float64 F, bool Rounded=true);

// These functions are used because MSVC6 isn't able to convert an unsigned int64 to a floating-point value, and I couldn't think of a cleaner way to handle it.
#if defined(_MSC_VER) && _MSC_VER<=1200
    inline float32 int64u_float32 (int64u v) {return static_cast<float32>(static_cast<int64s>(v>>1))*2.0f + static_cast<float32>(static_cast<int64s>(v & 1));}
    inline float64 int64u_float64 (int64u v) {return static_cast<float64>(static_cast<int64s>(v>>1))*2.0f + static_cast<float32>(static_cast<int64s>(v & 1));}
#else
    #if defined(_MSC_VER)
	    #pragma warning( disable : 4244 )
	#endif
    inline float32 int64u_float32 (int64u v) {return v;}
    inline float64 int64u_float64 (int64u v) {return v;}
    #if defined(_MSC_VER)
        #pragma warning( default : 4244 )
	#endif
#endif // defined(_MSC_VER) && _MSC_VER<=1200

//---------------------------------------------------------------------------
// CC (often used in all containers to identify a stream
#if (MAXTYPE_INT >= 64)
inline int64u CC8(const char*  C) {return C[7]<<0 | C[6]<<8 | C[5]<<16 | C[4]<<24 | ((int64u)C[3])<<32 | ((int64u)C[2])<<40 | ((int64u)C[1])<<48 | ((int64u)C[0])<<56;}
inline int64u CC7(const char*  C) {return C[6]<<0 | C[5]<<8 | C[4]<<16 | C[3]<<24 | ((int64u)C[2])<<32 | ((int64u)C[1])<<40 | ((int64u)C[0])<<48 | ((int64u)   0)<<56;}
inline int64u CC6(const char*  C) {return C[5]<<0 | C[4]<<8 | C[3]<<16 | C[2]<<24 | ((int64u)C[1])<<32 | ((int64u)C[0])<<40 | ((int64u)   0)<<48 | ((int64u)   0)<<56;}
inline int64u CC5(const char*  C) {return C[4]<<0 | C[3]<<8 | C[2]<<16 | C[1]<<24 | ((int64u)C[0])<<32 | ((int64u)   0)<<40 | ((int64u)   0)<<48 | ((int64u)   0)<<56;}
#endif
inline int32u CC4(const char*  C) {return C[3]<<0 | C[2]<<8 | C[1]<<16 | C[0]<<24;}
inline int32u CC3(const char*  C) {return C[2]<<0 | C[1]<<8 | C[0]<<16 |    0<<24;}
inline int16u CC2(const char*  C) {return C[1]<<0 | C[0]<<8;}
inline int8u  CC1(const char*  C) {return C[0]<<0;}
#ifndef __BORLANDC__
#if (MAXTYPE_INT >= 64)
inline int64u CC8(const int8u* C) {return C[7]<<0 | C[6]<<8 | C[5]<<16 | C[4]<<24 | ((int64u)C[3])<<32 | ((int64u)C[2])<<40 | ((int64u)C[1])<<48 | ((int64u)C[0])<<56;}
inline int64u CC7(const int8u* C) {return C[6]<<0 | C[5]<<8 | C[4]<<16 | C[3]<<24 | ((int64u)C[2])<<32 | ((int64u)C[1])<<40 | ((int64u)C[0])<<48 | ((int64u)   0)<<56;}
inline int64u CC6(const int8u* C) {return C[5]<<0 | C[4]<<8 | C[3]<<16 | C[2]<<24 | ((int64u)C[1])<<32 | ((int64u)C[0])<<40 | ((int64u)   0)<<48 | ((int64u)   0)<<56;}
inline int64u CC5(const int8u* C) {return C[4]<<0 | C[3]<<8 | C[2]<<16 | C[1]<<24 | ((int64u)C[0])<<32 | ((int64u)   0)<<40 | ((int64u)   0)<<48 | ((int64u)   0)<<56;}
#endif
inline int32u CC4(const int8u* C) {return C[3]<<0 | C[2]<<8 | C[1]<<16 | C[0]<<24;}
inline int32u CC3(const int8u* C) {return C[2]<<0 | C[1]<<8 | C[0]<<16 |    0<<24;}
inline int16u CC2(const int8u* C) {return C[1]<<0 | C[0]<<8;}
inline int8u  CC1(const int8u* C) {return C[0]<<0;}
#endif // __BORLANDC__

} //namespace ZenLib
#endif
