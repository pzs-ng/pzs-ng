/* This file and avinfo.c is more or less a part of avinfo.
 * (c) George Shunklin 2002,  gs@anime.org.ru
 * home page: http://anime.org.ru/soft/avinfo/
 */

#include <stdio.h>
#include <stdlib.h>

#define DWORD unsigned int
#define FOURCC unsigned int

typedef struct {
  DWORD  dwMicroSecPerFrame;
  DWORD  dwMaxBytesPerSec;
  DWORD  dwReserved1;
  DWORD  dwFlags;
  DWORD  dwTotalFrames;
  DWORD  dwInitialFrames;
  DWORD  dwStreams;
  DWORD  dwSuggestedBufferSize;
  DWORD  dwWidth;
  DWORD  dwHeight;
  DWORD  dwScale;
  DWORD  dwRate;
  DWORD  dwStart;
  DWORD  dwLength;
} MainAVIHeader; /* AVI header */

typedef struct {
  FOURCC  fccType;
  FOURCC  fccHandler;
  DWORD   dwFlags;
  DWORD   dwReserved1;
  DWORD   dwInitialFrames;
  DWORD   dwScale;
  DWORD   dwRate;
  DWORD   dwStart;
  DWORD   dwLength;
  DWORD   dwSuggestedBufferSize;
  DWORD   dwQuality;
  DWORD   dwSampleSize;
} AVIStreamHeader; /* stream header */

typedef struct {
  AVIStreamHeader *Data_h;
  DWORD DataSize_h;
  void *Data_f;
  DWORD DataSize_f;
  void *Data_d;
  DWORD DataSize_d;
  void *Next;
} mem_chunk;

int ReadAVIHeader(FILE* riff, MainAVIHeader* avihdr, mem_chunk ** str_data);
int AviSeek(FILE* file, FOURCC ID);

