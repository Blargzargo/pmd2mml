#include "common.h"

unsigned char fileReadBytes (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned char*  readResults
) {
  unsigned char   buffer[MAXLENGTH_UNKNOWN_BUFFER];
  size_t          readActualCount;
  unsigned char   result;
  unsigned char   i;

  readActualCount = fread (buffer, 1, readCount, readFile);
  result          = (readCount == readActualCount);

  if (result) {
    for (i = 0; i < readCount; i++) {
      readResults[i] = buffer[i];
    };
  };

  return result;
}

unsigned char fileReadShorts (
  FILE*           readFile,
  unsigned short  readCount,
  unsigned short* readResults
) {
  unsigned short  buffer[MAXLENGTH_UNKNOWN_BUFFER/2];
  size_t          readActualCount;
  unsigned char   result;
  unsigned char   i;

  readActualCount = fread (buffer, 2, readCount, readFile);
  result          = (readCount == readActualCount);

  if (result) {
    for (i = 0; i < readCount; i++) {
      readResults[i] = buffer[i];
    };
  };

  return result;
}

unsigned char pmdReadVersion (
  FILE*           readFile,
  unsigned char*  pmdFileVersion
) {
  unsigned char   buffer;
  unsigned char   result;

  result = fileReadBytes (readFile, 1, &buffer);

  if (result) {
    (*pmdFileVersion) = buffer;
  };

  return result;
}

unsigned char pmdReadTrackPointers (
  FILE*           readFile,
  enum_pmdChip    readChip,
  void*           pmdFileTrackPointers
) {
  unsigned char  result;

  switch (readChip) {
    case OPNA:
      result = pmdReadOpnaTrackPointers (
        readFile, pmdFileTrackPointers
      );
      break;
    default:
      /*
       * TODO
       * nicely propagate and log the error,
       * then abort somehow?
       */
      result = 1;
      break;
  };

  return result;
}

unsigned char pmdReadFmInstrumentPointer (
  FILE*           readFile,
  unsigned short* pmdFileFmInstrumentPointer
) {
  unsigned short  buffer;
  unsigned char   result;

  result = fileReadShorts (readFile, 1, &buffer);

  if (result) {
    (*pmdFileFmInstrumentPointer) = buffer;
  }

  return result;
}

char * lengthProcessing(unsigned char Length[]) {
	static char 	newLength[11];
	unsigned char	oldLength;
	
	oldLength = Length[0];
	lenFlag = 0;
	
	for (int i = 0; i < LENGTH_BUFFER - 1; i++) { //shift array elements left
		lenBuff[i] = lenBuff[i+1];
		//printf ("%u ", lenBuff[i]);
	}
	lenBuff[LENGTH_BUFFER - 1] = Length[0]; //add new length to array
	//printf ("%u ", lenBuff[LENGTH_BUFFER - 1]);
	
	
	int k = 0;
	for (int i = 0; i < LENGTH_BUFFER - 1; i++) { // compare array elements
		if (lenBuff[i] == lenBuff[i+1]) { //next element is equal
			lenFlag++;
			k++;
		} else if ((lenBuff[i] * 2 == lenBuff[i+1] * 3) || (lenBuff[i] * 3 == lenBuff[i+1] * 2)) { //next element is 1.5 times bigger or 1.5 times smaller (checking for dotted notes)
			lenFlag++;
		} else { //no obvious relation between note lengths
			lenFlag = 0;
		}
	}
	//printf("\nlenFlag: %d", lenFlag);
	//printf("\nk: %d", k);
	if (k == LENGTH_BUFFER - 1 && lenDefault[0] == lenBuff[LENGTH_BUFFER - 1]) { // Do NOT repeat len commands!!
		lenFlag = 0;
	//	printf("\nLine 135");
	} else if (lenFlag != LENGTH_BUFFER - 1) { //false if not all comparisons were true
		lenFlag = 0;
		//printf("\nLine 138");
	} else if (lenBuff[LENGTH_BUFFER - 1] == 0) { //Check is done to not erroneously print l%0
		lenFlag = 0;
		//printf("\nLine 141");
	} else if (k == LENGTH_BUFFER - 1 && (ZENLEN % (lenBuff[LENGTH_BUFFER - 1] * 2 / 3) == 0 && (lenBuff[LENGTH_BUFFER - 1] * 2) % 3 != 0)){
	 	lenFlag = 0;
	 	//printf("\nLine 144");
	} else { //Finally, setting lenDefault...
		if (k == LENGTH_BUFFER - 1) { //if a length completely fills the buffer, it gets priority...
			if (ZENLEN % (lenBuff[LENGTH_BUFFER - 1] * 2 / 3) == 0 && (lenBuff[LENGTH_BUFFER - 1] * 2) % 3 != 0) { //...unless it's a dotted note, Default will NEVER be dotted (personal preference)
				lenDefault[0] = lenBuff[LENGTH_BUFFER - 1] * 2 / 3;
				//printf("\nLine 149");
			} else {
				lenDefault[0] = lenBuff[LENGTH_BUFFER - 1];	//You may print now, go on
				//printf("\nLine 152");
			}
			
		} else { //You can only get to this point if lenBuff consists entirely of values with a 1.5 times relation to eachother (more likely than you think unfortunately)
			int min = 0; //I had to create min/max variables for reasons that will be explained later
			int max = 0;
			for (int i = 0; i < LENGTH_BUFFER - 1; i++) { //min/max
				if (lenBuff[min] < lenBuff[i+1]) {
					//empty
				} else if (lenBuff[min] > lenBuff[i+1]) {
					min = i + 1;
				} else {
					//404 not found
				}
				if (lenBuff[max] < lenBuff[i+1]) {
					max = i + 1;
				} else if (lenBuff[max] > lenBuff[i+1]) {
					//nothing here but us chickens
				} else {
					//Wait, where did everyone go?
				}
			}
			//printf("\nmin: %d", min);
			//printf("\nmax: %d", max);
			if (lenBuff[min] * 3 != lenBuff[max] * 2) { //mistakes were made, this can actually happen if the array contains weird values like [8,12,18,27]
				lenFlag = 0;
				//printf("\nLine 178");
			} else if (lenBuff[min] == lenDefault[0]) { //If the min is already the Default, ignore
				lenFlag = 0;
				//printf("\nLine 181");
			} else if (ZENLEN % lenBuff[min] == 0) {
				lenDefault[0] = lenBuff[min];
				//printf("\nLine 184");
			} else {
				lenDefault[0] = lenBuff[min];
				//printf("\nI am not really sure how you got here...\n");
			}	
		}
		/*
		if (k == LENGTH_BUFFER - 1 && lenDefault[0] ==) {
			lenDefault[0] = lenBuff[LENGTH_BUFFER - 1];
			printf("\nStatus1");
		} else {
			lenDefault[0] = lenBuff[j];
			printf("\nStatus2");
		}*/
		
		//printf ("length set: l%%%d\n", lenDefault[0]);
	}
	
	if (oldLength == 0) {
		oldLength = 1;
	}
	
	if ((oldLength == lenDefault[0]) && lenFlag != LENGTH_BUFFER - 1 && InsertLen != 1) {
		snprintf(newLength, 11, "");
		
	} else if ((oldLength * 2 == lenDefault[0] * 3) && lenFlag != LENGTH_BUFFER - 1 && InsertLen != 1) {
		snprintf(newLength, 11, ".");
		
	} else if (ZENLEN % oldLength == 0) {
		snprintf(newLength, 11, "%d", ZENLEN / oldLength);
		
	} else if (ZENLEN % (oldLength * 2 / 3) == 0 && (oldLength * 2) % 3 == 0) {
		snprintf(newLength, 11, "%d.", ZENLEN / (oldLength * 2 / 3));
		
	} else {
		snprintf(newLength, 11, "%%%d", oldLength);
	}
	
	return newLength;
}

char * octaveProcessing(unsigned char octave[]) {
	static char 	newOctave[10];
	if (octaveFlag == 1) {
		octaveBuff[LENGTH_BUFFER] = octaveBuff[0];
		octaveFlag = 0;
	}
	
	for (int i = 0; i < LENGTH_BUFFER; i++) {
		octaveBuff[i] = octaveBuff[i+1];
		//printf ("%hu ", octaveBuff[i]);
	}
	
	if (octave[0] == 0xFF) {
		octaveBuff[LENGTH_BUFFER - 1] = octave[0];
	}
	if (octaveBuff[LENGTH_BUFFER - 1] == 0xF0) {
		octaveBuff[LENGTH_BUFFER - 1] = octaveBuff[LENGTH_BUFFER - 2];
	}
	if (octave[0] != 0xF0) {
		octaveBuff[LENGTH_BUFFER] = (octave[0] >> 4) + 0x01;
	}
	
	//printf ("%hu\n", octaveBuff[LENGTH_BUFFER]);
	
	
	if (octaveBuff[LENGTH_BUFFER - 1] >= 0x01 && octaveBuff[LENGTH_BUFFER - 1] <= 0x08) {
	//									Comment this out if you prefer o4c>>>>c instead of o4co8c
		if ((octaveBuff[LENGTH_BUFFER] - octaveBuff[LENGTH_BUFFER - 1]) / 2 == 0 && PortFlag == 0) {
			if (octaveBuff[LENGTH_BUFFER] - octaveBuff[LENGTH_BUFFER - 1] == 1) {
				snprintf (newOctave, 10, ">");
			} else if (octaveBuff[LENGTH_BUFFER] - octaveBuff[LENGTH_BUFFER - 1] == 0) {
				snprintf (newOctave, 10, "");
			} else {
				snprintf (newOctave, 10, "<");
			}
		} else {
			snprintf(newOctave, 10, "o%hu", octaveBuff[LENGTH_BUFFER]);
			PortFlag = 0;
		}
	/*									Un-comment this code if you prefer o4c>>>>c instead of o4co8c, most melodies don't change 2 octaves
										Also, I should really use more switch cases instead of this if/else spaghetti >.<
		if (octaveBuff[LENGTH_BUFFER] > octaveBuff[LENGTH_BUFFER - 1]) {
			for (int i = 0; octaveBuff[LENGTH_BUFFER] - octaveBuff[LENGTH_BUFFER - 1] - i > 0; i++){
				snprintf(&newOctave[i], 10-i, ">");
			}			
			
		} else if (octaveBuff[LENGTH_BUFFER] == octaveBuff[LENGTH_BUFFER - 1]) {
			snprintf(newOctave, 10, "");
			
		} else { //(octaveBuff[LENGTH_BUFFER] < octaveBuff[LENGTH_BUFFER - 1])
			for (int i = 0; octaveBuff[LENGTH_BUFFER - 1] - octaveBuff[LENGTH_BUFFER] - i > 0; i++){
				snprintf(&newOctave[i], 10-i, "<");
			}
		} */
		
	} else { //no octaveBuff[LENGTH_BUFFER - 1], or greater than 1 octave change (I personally think >>> looks ugly lmao)
		snprintf(newOctave, 10, "o%hu", octaveBuff[LENGTH_BUFFER]);
		PortFlag = 0;
	}
	return newOctave;
}

char * TrackReset(){
	unsigned char octaveReset[1] = { 0xFF };
	unsigned char lengthReset[1] = { '\0' };

    lengthProcessing(lengthReset);
    octaveProcessing(octaveReset);
    lenFlag = 0;
    if (RhythmFlag != 1) {
    	lenDefault[0] = 0x18;
	} else {
		lenDefault[0] = 0x00;
		RhythmBuff[LENGTH_BUFFER] = 0x00;
	}
    
}

char * inc2order(FILE* pmdFile, int Offset) {
	for (int i = 0; i < LENGTH_BUFFER - 1; i++) {
		inc2arr[i] = inc2arr[i+1];
		pmdNoteArr[i] = pmdNoteArr[i+1];
		PortBuff[i] = PortBuff[i+1];
	}
	inc2arr[LENGTH_BUFFER - 1] = inc2;
	pmdNoteArr[LENGTH_BUFFER - 1] = (unsigned short) ftell (pmdFile) - Offset + 1;
	//printf("inc2arr: %d %d %d %d", inc2arr[LENGTH_BUFFER - 4], inc2arr[LENGTH_BUFFER - 3], inc2arr[LENGTH_BUFFER - 2], inc2arr[LENGTH_BUFFER - 1]);
	//printf("pmdNoteArr: %x %x %x %x\n", pmdNoteArr[LENGTH_BUFFER - 4], pmdNoteArr[LENGTH_BUFFER - 3], pmdNoteArr[LENGTH_BUFFER - 2], pmdNoteArr[LENGTH_BUFFER - 1]);
}

char * insertLen(FILE* pmdFile) {
	inc2 = inc2arr[0];
	pmdNoteArr[LENGTH_BUFFER - 1] = pmdNoteArr[0];
	PortFlag = 0;
	for (int i = 0; i < LENGTH_BUFFER - 1; i++) {
		if (PortBuff[i] == 0x01) {
			PortFlag++;
		}
	}
	fflush(pmdFile);
	fseek (pmdFile, (long) pmdNoteArr[LENGTH_BUFFER - 1], SEEK_SET);
	InsertLen = 1;
	octaveFlag = 1;
	static char		lenCommand[12];
	inc2 += sprintf (buff2+inc2, "l%s", lengthProcessing(&lenDefault[0]));
	InsertLen = 0;
	return lenCommand;
}


