/* This file and avinfo.h is more or less a part of avinfo.
 * (c) George Shunklin 2002,  gs@anime.org.ru
 * home page: http://anime.org.ru/soft/avinfo/
 */

#include "avinfo.h"
#include "video_info.h"

/* Seek an AVI file until a requested chunk is found. return a Size of chunk,
 * or zero if no requsted chunk found.
 */
int AviSeek(FILE* file, FOURCC ID) {
  struct {
    DWORD ID;
    DWORD Size;
  } chunk; /* reference view of chunk */

  if(!fread(&chunk, sizeof(chunk), 1, file))
    return(0);
  while(chunk.ID!= ID) {
    if(!fseek(file, chunk.Size, SEEK_CUR))
      return(0);
    if(!fread(&chunk, sizeof(chunk), 1, file))
      return(0);
  }

  return(chunk.Size);
}




int ReadAVIHeader(FILE *riff, MainAVIHeader *avihdr, mem_chunk **str_data) {
  int counter, SizeCount;
  DWORD Data, Size, riffType;
  mem_chunk* current = NULL;
  mem_chunk* temp;
  *str_data = NULL;

  if(!AviSeek(riff, 0x46464952))
    return(-1);
  if(!fread(&riffType, 1, sizeof(riffType), riff))
    return(-2);
  if(riffType != 0x20495641)
    return(-3); /* riff, but not AVI (e.g. WAV, RMI) */
  if(!AviSeek(riff, 0x5453494C))
    return(-4);
  if(!fread(&riffType, sizeof(riffType), 1, riff))
    return(-5);
  while(riffType != 0x6C726468) {  /* if not a hrdl type,skip all LIST chunk */
    if(!AviSeek(riff, 0x5453494C))
      return(-6);
    if(!fread(&riffType, sizeof(riffType), 1, riff))
      return(-7);
  }
  if(!fread(&riffType, sizeof(riffType), 1, riff))
    return(-8);
  if(riffType != 0x68697661)
    return(-9);
  if(!fread(&Size, sizeof(Size), 1, riff))
    return(-10);
  if(Size != sizeof(MainAVIHeader))
    return(-11); /* size of avi header doesn't match */
  if(!fread(avihdr, Size, 1, riff))
    return(-12); /* <--Reading header */

  for(counter = avihdr->dwStreams; counter; counter--) {
    do {
      SizeCount = AviSeek(riff, 0x5453494C);
      if(!SizeCount)
        return(-13);
      if(!fread(&Data, sizeof(Data), 1, riff))
        return(-14);
    }
    while(Data != 0x6C727473);

    temp = (mem_chunk*)malloc(sizeof(mem_chunk));
    if(!temp)
      return(-15);
    if(current)
      current->Next = (void*)temp;
    else
      current = temp;
    temp->Next = NULL;
    if(!*str_data)
      *str_data = current;
    if(!fread(&Data, 1, sizeof(Data), riff))
      return(-16);
    if(!fread(&Size, 1, sizeof(Size), riff))
      return(-17);
    if(Data == 0x68727473) {
      if(!(temp->Data_h = (void*)malloc(Size)))
        return(-18);
      if(!fread(temp->Data_h, 1, Size, riff))
        return(-19);
      temp->DataSize_h = Size;
      SizeCount = SizeCount - Size - 12;
      if(SizeCount > 0) {
        if(!fread(&Data, 1, sizeof(Data), riff))
          return(-20);
        if(!fread(&Size, 1, sizeof(Size), riff))
          return(-21);
      }
    }
    if(Data == 0x66727473) {
      if(!(temp->Data_f = (void*)malloc(Size)))
        return(-22);
      if(!fread(temp->Data_f, 1,Size, riff))
        return(-23);
      temp->DataSize_f = Size;
      SizeCount = SizeCount - Size - 8;
      if(SizeCount > 0) {
        if(!fread(&Data, 1, sizeof(Data), riff))
          return(-24);
        if(!fread(&Size, 1, sizeof(Size), riff))
          return(-25);
        SizeCount = SizeCount - 8;
      }
    }
    if(Data == 0x64727473) {
      if(!(temp->Data_d = (void*)malloc(Size)))
        return(-26);
      if(!fread(temp->Data_d,1, Size, riff))
        return(-27);
      temp->DataSize_d = Size;
    }
    if(SizeCount > 0)
      fseek(riff, SizeCount, SEEK_CUR);
  }

  return(0);
}




int get_avi_info(FILE* fp, struct program_info *program, unsigned long file_size) {
  MainAVIHeader avih;
  mem_chunk *temp;

  rewind(fp);
  if(ReadAVIHeader(fp, &avih, &temp))
    return(-1);
  program->program_avg_bit_rate = file_size / (avih.dwTotalFrames * avih.dwMicroSecPerFrame / 1000) * 8000;

  return(0);
}

