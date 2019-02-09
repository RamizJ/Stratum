/*
  file types
*/
#include <gwin2d3d\filetype.h>

#define FILE_PRJ      9501
#define FILE_CLS      9502
#define FILE_MAT      9503
#define FILE_STT      9504
#define FILE_RAR      9505
#define FILE_URL      9506

#define PACK_SIGNATURE  0x4C53
#define CLASS_SIGNATURE 0x4253
#define PRJ_SIGNATURE   0x6849



UINT GetFileType(char*,int);

