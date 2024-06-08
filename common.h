#ifndef LIBOPENPMD_COMMON_H
#define LIBOPENPMD_COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLENGTH_UNKNOWN_BUFFER 256
#define LENGTH_BUFFER 4				//change this value to change the threshold of the number of notes before printing a length command (MUST BE GREATER THAN 1!!!!)
#define UNUSED(x) (void)(x)

#include "opna.h"

typedef enum {
  OPNA
} enum_pmdChip;

typedef struct {
  unsigned char  version;
  void*          trackPointers;
  unsigned short fmInstrumentPointer;
} struct_pmdHeader;

unsigned char fileReadBytes (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned char*  readResults
);

unsigned char fileReadShorts (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned short* readResults
);

unsigned char pmdReadVersion (
  FILE*           readFile,
  unsigned char*  pmdFileVersion
);

unsigned char pmdReadTrackPointers (
  FILE*           readFile,
  enum_pmdChip    readChip,
  void*           pmdFileTrackPointers
);

unsigned char pmdReadFmInstrumentPointer (
  FILE*           readFile,
  unsigned short* pmdFileFmInstrumentPointer
);

static char FMExtendFlag;
static short FMExtendAddr[3];

static char PPZExtendFlag;
static short PPZExtendAddr[8];

char	buff1[65535];
char	buff2[65535];

unsigned char ZENLEN;

int	inc1;
int	inc2;
int			inc2arr[LENGTH_BUFFER];
unsigned short pmdNoteArr[LENGTH_BUFFER];

unsigned char lenBuff[LENGTH_BUFFER];
int	lenFlag;
unsigned char	lenDefault[1];
int	InsertLen;
unsigned char	PortBuff[LENGTH_BUFFER];
int PortFlag; //hotfix for octave and pmdRead bugs because portamento misaligns the buffers


int		octaveFlag;
unsigned char	octaveBuff[LENGTH_BUFFER + 1];

int		RhythmFlag;
int		DT2Flag = 0;
long	InsPoint;
unsigned short	RhythmBuff[LENGTH_BUFFER + 1];

#endif /* LIBOPENPMD_COMMON_H */
