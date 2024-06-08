#include <stdio.h>
#include <string.h>

#include "common.h"
#include "common.c"
#include "opna.c"
#include "opna.h"

#define DEBUG_PMD_FILE "RAVETUNE.M2"

/*
 * dummy, might be populated when there's a use case?
 */
unsigned char pmdReadSequenceNoChannel (FILE* pmdFile) {
  /*
   * pmdFile has to be carried through for function signature reasons
   * manually "mark" it as unused like this so compiler is happy
   */
  UNUSED (pmdFile);
  return 1;
}

unsigned char pmdReadSequenceFM (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  argBytes[4];
  unsigned short argShorts[1];
  unsigned char  resultCmd, resultArg;
  const char*    notes[] = {"c", "c+", "d", "d+", "e", "f", "f+", "g", "g+", "a", "a+", "b"};
  
  unsigned char  bitCounter;
  const char*    rhythmSourceSampleBitMap[] = {
    "b",
    "s",
    "c",
    "h",
    "t",
    "i"
  };
  
  const char*	rhythmSourceSamplePan[] = {"l","m","r"};

  //printf ("0x%04X: ", (unsigned short) ftell (pmdFile));

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    switch (cmd[0]) {
      case 0xFF:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "@%hu", //cmd[0], argBytes[0], 
		  argBytes[0]); //command 0x%02X   0x%02X                  - Instrument Definition: 
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFE:
        resultArg = fileReadBytes (pmdFile, 3, argBytes);
        if (resultArg) {		
          switch (argBytes[1]) {
            case 0xB1: //FE 02X B1 02X
              if (argBytes[2] < 0x80) {
              	inc2 += sprintf (buff2+inc2, "q%hu-%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X      - Sound Cut Setting 2: 
			    //cmd[0], argBytes[0], argBytes[1], argBytes[2],
			    argBytes[0], (argBytes[2] + argBytes[0]) & 0xFF
			  	);
			  	
			  } else if (argBytes[0] != argBytes[0] - (argBytes[2] & 0x7F)){
			  	inc2 += sprintf (buff2+inc2, "q%hu-%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X      - Sound Cut Setting 2: 
			    //cmd[0], argBytes[0], argBytes[1], argBytes[2],
			    argBytes[0], argBytes[0] - (argBytes[2] & 0x7F)
			    );
			    
			  } else {
			  	inc2 += sprintf (buff2+inc2, "q%hu-%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X      - Sound Cut Setting 2: 
			    //cmd[0], argBytes[0], argBytes[1], argBytes[2],
			    argBytes[0], argBytes[0] ^ 0x80
			    );
			  }
			  break; 			  
			default: //FE 02X 02X 02X
			  inc2 += sprintf (buff2+inc2, "q%hu", //command 0x%02X   0x%02X                  - Sound Cut Setting 2: 
                //cmd[0], argBytes[0],
                argBytes[0]
              );
			  fseek (pmdFile, -2, SEEK_CUR);
			  break;
		  };          
        } else {
          resultCmd = resultArg;
          fseek (pmdFile, -2, SEEK_CUR);
        };
        break;
      case 0xFD:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "V%hu", //command 0x%02X   0x%02X                  - Volume Setting 2: 
		  //cmd[0], argBytes[0], 
		  argBytes[0]); 
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          switch (argBytes[0]) {
            case 0xFF:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
                inc2 += sprintf (buff2+inc2, "t%hu", //command 0x%02X   0x%02X 0x%02X             - Tempo Setting 1: 
                  //cmd[0], argBytes[0], argBytes[1],
                  argBytes[1]
                );
              } else {
                resultCmd = resultArg;
              };
              break;
            case 0xFE:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
              	if (argBytes[1] < 0x80) {
              	  inc2 += sprintf (buff2+inc2, "T+%hd", //command 0x%02X   0x%02X 0x%02X             - add Tempo Setting 2 (TimerB): 
                  //cmd[0], argBytes[0], argBytes[1],
                  (signed char) argBytes[1]
                  );
				} else {
				  inc2 += sprintf (buff2+inc2, "T%hd", //command 0x%02X   0x%02X 0x%02X             - add Tempo Setting 2 (TimerB): 
                  //cmd[0], argBytes[0], argBytes[1],
                  (signed char) argBytes[1]
                  );
				}
                
              } else {
                resultCmd = resultArg;
              };
              break;
            case 0xFD:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              if (resultArg) {
                if (argBytes[1] < 0x80) {
              	  inc2 += sprintf (buff2+inc2, "t+%hd", //command 0x%02X   0x%02X 0x%02X             - add Tempo Setting 1: 
                  //cmd[0], argBytes[0], argBytes[1],
                  (signed char) argBytes[1]
                  );
				} else {
				  inc2 += sprintf (buff2+inc2, "t%hd", //command 0x%02X   0x%02X 0x%02X             - add Tempo Setting 1: 
                  //cmd[0], argBytes[0], argBytes[1],
                  (signed char) argBytes[1]
                  );
				}
              } else {
                resultCmd = resultArg;
              };
              break;
            default:
              inc2 += sprintf (buff2+inc2, "T%hu", //command 0x%02X   0x%02X                  - Tempo Setting 2 (TimerB): 
                //cmd[0], argBytes[0],
                argBytes[0]
              );
              break;
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xFB:
        inc2 += sprintf (buff2+inc2, "&"//, cmd[0]
		); //command 0x%02X                         - Tie: 
        break;
      case 0xFA:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "D%hd", //command 0x%02X 0x%04X                  - Detune Setting: 
            //cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF9:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "[", //command 0x%02X 0x%04X                  - start loop, stop at 0x%04X: 
            //cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF8:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          resultArg = fileReadShorts (pmdFile, 1, argShorts);
          if (resultArg) {
            inc2 += sprintf (buff2+inc2, "]%hu", //command 0x%02X   0x%02X 0x%02X 0x%04X      - stop loop count %hu, %hu full loop(s), start at 0x%04X: 
              //cmd[0], argBytes[0], argBytes[1], argShorts[0],
              argBytes[0]
			  //, argBytes[1], argShorts[0]
            );
          } else {
            resultCmd = resultArg;
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF7:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, ":"
		  //, //command 0x%02X 0x%04X                  - exit loop, jump to 0x%04X: 
            //cmd[0], argShorts[0],
            //argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF6:
        inc2 += sprintf (buff2+inc2, "L" //command 0x%02X                         - Master Loop Start: 
		//, cmd[0]
		); 
        break;
      case 0xF5:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "_%hd", //command 0x%02X   0x%02X                  - Absolute Transposition: 
		  //cmd[0], argBytes[0], 
		  (signed char) argBytes[0]
		  );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF4:
        inc2 += sprintf (buff2+inc2, ")" //command 0x%02X                         - Relative Volume Up: 
		//, cmd[0]
		);
        break;
      case 0xF3:
        inc2 += sprintf (buff2+inc2, "(" //command 0x%02X                         - Relative Volume Down: 
		//, cmd[0]
		); 
        break;
      case 0xF2:
        resultArg = fileReadBytes (pmdFile, 4, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MA%hu,%hu,%hd,%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X 0x%02X - Software LFO A delay, speed, depthA, depthB: 
            //cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3],
            argBytes[0], argBytes[1], (signed char) argBytes[2], argBytes[3]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF1:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "*A%hu", //command 0x%02X   0x%02X                  - Software LFO A Switch: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xF0:
        resultArg = fileReadBytes (pmdFile, 4, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "E%hu,%hd,%hu,%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X 0x%02X - SSG Software Envelope AL, DD, SR, RR: 
            //cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3],
            argBytes[0], (signed char) argBytes[1], argBytes[2], argBytes[3]
          );
        };
        break;
      case 0xEF:
      	resultArg = fileReadBytes (pmdFile, 2, argBytes);
      	if (resultArg) {
      	  inc2 += sprintf (buff2+inc2, "y$%02X,%hu", //command 0x%02X   0x%02X 0x%02X             - FM Chip Register Write: 
			//cmd[0], argBytes[0], argBytes[1],
			argBytes[0], argBytes[1]
		  );
		} else {
		  resultCmd = resultArg;
		};
		break;
	  case 0xEE:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "w%hu", //command 0x%02X   0x%02X                  - Noise Frequency: 
           // cmd[0], argBytes[0], 
		   argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xED:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          switch (argBytes[0]) {
		    case 0x00:
          	  inc2 += sprintf (buff2+inc2, "P0" //command 0x%02X   0x%02X                  - Noise Output (no): 
              //cmd[0], argBytes[0]
              );
              break;
            case 0x07:
              inc2 += sprintf (buff2+inc2, "P1" //command 0x%02X   0x%02X                  - Noise Output (Tone): 
              //cmd[0], argBytes[0]
              );
              break;
            case 0x38:
              inc2 += sprintf (buff2+inc2, "P2" //command 0x%02X   0x%02X                  - Noise Output (Noise): 
              //cmd[0], argBytes[0]
              );
              break;
            case 0x3F:
              inc2 += sprintf (buff2+inc2, "P3" //command 0x%02X   0x%02X                  - Noise Output (Tone + Noise): 
              //cmd[0], argBytes[0]
              );
              break;
            default:
        	  inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X                  - Noise Output (UNK): P%hu\n", 
              cmd[0], argBytes[0], argBytes[0]
              );
              break;
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xEC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "p%hu", //command 0x%02X   0x%02X                  - Pan Setting 1: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xEB:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes);
      	if (argBytes[0] < 0x40) {
      		/*inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X                  - play ",
              cmd[0], argBytes[0]
            );*/
      	  for (bitCounter=0; bitCounter<6; ++bitCounter) {
      	    if ((argBytes[0] >> bitCounter) & 0x01) {
      	  	  inc2 += sprintf (buff2+inc2, "\\%s", rhythmSourceSampleBitMap[bitCounter]);
			}
		  }
		  //inc2 += sprintf (buff2+inc2, "\n");
		} else if (argBytes[0] >= 0x80 && argBytes[0] < 0xC0) {
      		/*inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X                  - stop ",
              cmd[0], argBytes[0]
            );*/
      	  for (bitCounter=0; bitCounter<6; ++bitCounter) {
      	    if ((argBytes[0] >> bitCounter) & 0x01) {
      	  	  inc2 += sprintf (buff2+inc2, "\\%sp", rhythmSourceSampleBitMap[bitCounter]);
			}
		  }
		  //inc2 += sprintf (buff2+inc2, "\n");
		} else if (argBytes[0] >= 0x40 || argBytes[0] >= 0xC0) {
			inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X                  - UNKNOWN DRUM\n",
              cmd[0], argBytes[0]
            );
		} else {
			resultCmd = resultArg;
		}
		break;
	  case 0xEA:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (argBytes[0] >= 0x20 && argBytes[0] < 0xE0) {
          inc2 += sprintf (buff2+inc2, "\\v%s%hu", //command 0x%02X   0x%02X                  - Rhythm Individual Volume: 
            //cmd[0], argBytes[0],
            rhythmSourceSampleBitMap[(argBytes[0] >> 5) - 1], argBytes[0] % 32
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE9:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (argBytes[0] >= 0x20 && argBytes[0] < 0xE0 && ((argBytes[0] & 0x1C) == 0) && ((argBytes[0] & 0x03) != 0) ) {
          inc2 += sprintf (buff2+inc2, "\\%s%s", //command 0x%02X   0x%02X                  - Rhythm Individual Panning: 
            //cmd[0], argBytes[0],
            rhythmSourceSamplePan[(argBytes[0] & 3) - 1], rhythmSourceSampleBitMap[(argBytes[0] >> 5) - 1]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE8:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "\\V%hu", //command 0x%02X   0x%02X                  - Rhythm Master Volume: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE7:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "__%hd", //command 0x%02X   0x%02X                  - Relative Transposition: 
            //cmd[0], argBytes[0], 
			(signed char) argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE6:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          if (argBytes[0] < 0x80) {
          	inc2 += sprintf (buff2+inc2, "\\V+%hd", //command 0x%02X   0x%02X                  - add Rhythm Master Volume: 
            //cmd[0], argBytes[0], 
			(signed char) argBytes[0]
            );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "\\V%hd", //command 0x%02X   0x%02X                  - add Rhythm Master Volume: 
            //cmd[0], argBytes[0], 
			(signed char) argBytes[0]
            );
		  }
          
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE5:
      	resultArg = fileReadBytes (pmdFile, 2, argBytes);
      	if (resultArg) {
      	  /*inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X 0x%02X             - add Rhythm Individual Volume: ",
			cmd[0], argBytes[0], argBytes[1]
		  );*/
		  if (argBytes[1] < 0x80) {
		  	for (bitCounter=0; bitCounter<6; ++bitCounter) {
      	      if ((argBytes[0] >> bitCounter) & 0x01) {
      	  	    inc2 += sprintf (buff2+inc2, "\\v%s+%hd", rhythmSourceSampleBitMap[bitCounter], (signed char) argBytes[1]);
			  }
		    }
		  } else {
		  	for (bitCounter=0; bitCounter<6; ++bitCounter) {
      	      if ((argBytes[0] >> bitCounter) & 0x01) {
      	  	    inc2 += sprintf (buff2+inc2, "\\v%s%hd", rhythmSourceSampleBitMap[bitCounter], (signed char) argBytes[1]);
			  }
		    }
		  }
      	        	  
		} else {
		  resultCmd = resultArg;
		};
		break;
	  case 0xE4:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "#D%hu", //command 0x%02X   0x%02X                  - Hardware LFO Delay: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE3:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, ")%%%hu", //command 0x%02X   0x%02X                  - Fine Relative Volume Change: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE2:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "(%%%hu", //command 0x%02X   0x%02X                  - Fine Relative Volume Change: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE1:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "H%hu,%hu", //command 0x%02X   0x%02X                  - Hardware LFO PMS/AMS: 
            //cmd[0], argBytes[0],
            (argBytes[0] & 0x07), (argBytes[0] >> 4)
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xE0:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "#%hu,%hu", //command 0x%02X   0x%02X                  - Hardware LFO Switch/Depth: 
            //cmd[0], argBytes[0],
            (argBytes[0] >> 3), (argBytes[0] & 0x07)
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDF: //easier to convert 0xDF to Z command rather than C command, since C command does weird compiler magic
        resultArg = fileReadBytes (pmdFile, 1, argBytes);//^old comment, I implemented ZENLEN anyway
        if (resultArg) {
        	ZENLEN = argBytes[0];
        	int d = 24 * ZENLEN / 96;
        	lenDefault[0] = (unsigned char) d;
          inc2 += sprintf (buff2+inc2, "C%hu", //command 0x%02X   0x%02X                  - Zenlen: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDE:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, ")^%%%hu", //command 0x%02X   0x%02X                  - Relative Volume Accent: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDD:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "(^%%%hu", //command 0x%02X   0x%02X                  - Relative Volume Accent: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "~%hu", //command 0x%02X   0x%02X                  - Status1: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDB:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          if (argBytes[0] < 0x80) {
          	inc2 += sprintf (buff2+inc2, "~+%hd", //command 0x%02X   0x%02X                  - Status1: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
            );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "~%hd", //command 0x%02X   0x%02X                  - Status1: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
            );
		  }
          
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xDA:
        resultArg = fileReadBytes (pmdFile, 3, argBytes);
        if (resultArg) {
          unsigned char octave1[10];
          unsigned char octave2[10];
          inc2order(pmdFile, 5);
          PortBuff[LENGTH_BUFFER - 1] = 0x01;
          snprintf (octave1, 10, "%s", octaveProcessing(&argBytes[0]));
          snprintf (octave2, 10, "%s", octaveProcessing(&argBytes[1]));
          inc2 += sprintf (buff2+inc2, "{%s%s%s%s}%s", //command 0x%02X   0x%02X 0x%02X   0x%02X      - Portamento: 
            //cmd[0], argBytes[0], argBytes[1], argBytes[2],
            octave1, notes[argBytes[0] & 0x0F], octave2, notes[argBytes[1] & 0x0F], lengthProcessing(&argBytes[2])
          );
          if (lenFlag == LENGTH_BUFFER - 1) {
          	insertLen(pmdFile);
		  }
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD9:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "#w%hu", //command 0x%02X   0x%02X                  - Hardware LFO Waveform: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD8:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (argBytes[0] < 0x80) {
          inc2 += sprintf (buff2+inc2, "#a%hu", //command 0x%02X   0x%02X                  - Hardware LFO AMD: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
    	} else if (argBytes[0] >= 0x80 && argBytes[0] < 0xC0) {
    	  inc2 += sprintf (buff2+inc2, "#p%hu", //command 0x%02X   0x%02X                  - Hardware LFO PMD: 
            //cmd[0], argBytes[0],
            (argBytes[0] - 128)
          );
		} else if (argBytes[0] >= 0xC0) {
		  inc2 += sprintf (buff2+inc2, "#p%hd", //command 0x%02X   0x%02X                  - Hardware LFO PMD: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD7:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "#f%hu", //command 0x%02X   0x%02X                  - Hardware LFO Frequency: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD6:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MDA%hu,%hd", //command 0x%02X   0x%02X 0x%02X             - Software LFO A Temporal Change: 
            //cmd[0], argBytes[0], argBytes[1],
            argBytes[0], (signed char) argBytes[1]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD5:
        resultArg = fileReadShorts (pmdFile, 1, argShorts);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "DD%hd", //command 0x%02X 0x%04X                  - Detune Setting: 
            //cmd[0], argShorts[0],
            argShorts[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
	  case 0xD4:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "n%hu", //command 0x%02X   0x%02X                  - SSG SFX: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD3:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "N%hu", //command 0x%02X   0x%02X                  - FM SFX: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD2:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "F%hu", //command 0x%02X   0x%02X                  - Fade Out: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD1:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "command 0x%02X   0x%02X                  - UNUSED (jump1)\n",
            cmd[0], argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xD0:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          if (argBytes[0] < 0x80) {
          	inc2 += sprintf (buff2+inc2, "w+%hd", //command 0x%02X   0x%02X                  - Noise Frequency: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
            );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "w%hd", //command 0x%02X   0x%02X                  - Noise Frequency: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
            );
		  }
          
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCF:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "s%hu", //command 0x%02X   0x%02X                  - Slot Solo: 
            //cmd[0], argBytes[0],
            (argBytes[0] >> 4)
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCE:
        resultArg = fileReadShorts (pmdFile, 3, argShorts);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, ",%hd,%hd,%hd", //command 0x%02X   0x%04X                  - Instrument: 
            //cmd[0], argShorts[0],
            argShorts[0], argShorts[1], argShorts[2]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCD:
        resultArg = fileReadBytes (pmdFile, 5, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "E%hu,%hu,%hu,%hu,%hu,%hu", //command 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X- SSG Env AR,DR,SR,RR,SL,AL: 
            //cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3], argBytes[4],
            argBytes[0], argBytes[1], argBytes[2], (argBytes[3] & 0x0F), (argBytes[3] >> 4), argBytes[4]
          );
        };
        break;
      case 0xCC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "DX%hu", //command 0x%02X   0x%02X                  - SSG Pitch Interval Correction (Detune Extend): 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCB:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MWA%hu", //command 0x%02X   0x%02X                  - Software LFO A Waveform: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xCA:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MXA%hu", //command 0x%02X   0x%02X                  - Software LFO A Speed Setting: 
           // cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xC9:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "EX%hu", //command 0x%02X   0x%02X                  - Envelope Extend: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xC8:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes);
      	if (resultArg) {
      	  resultArg = fileReadShorts (pmdFile, 1, argShorts);
      	  if (resultArg) {	
      	    inc2 += sprintf (buff2+inc2, "sd%hu,%hd", //command 0x%02X   0x%02X 0x%04X           - Slot Detune: 
			  //cmd[0], argBytes[0], argShorts[0],
			  argBytes[0], argShorts[0]
		    );
		  } else {
		    resultCmd = resultArg;
		  };
		} else {
		  resultCmd = resultArg;
		};
		break;
	  case 0xC7:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes);
      	if (resultArg) {
      	  resultArg = fileReadShorts (pmdFile, 1, argShorts);
      	  if (argShorts[0] < 0x8000) {
      	  	inc2 += sprintf (buff2+inc2, "sdd%hu,+%hd", //command 0x%02X   0x%02X 0x%04X           - Slot Relative Detune: 
			//cmd[0], argBytes[0], argShorts[0],
			argBytes[0], argShorts[0]
		    );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "sdd%hu,%hd", //command 0x%02X   0x%02X 0x%04X           - Slot Relative Detune: 
			//cmd[0], argBytes[0], argShorts[0],
			argBytes[0], argShorts[0]
		    );
		  }
      	  
		} else {
		  resultCmd = resultArg;
		};
		break;
      case 0xC6:
      	resultArg = fileReadShorts (pmdFile, 3, argShorts);
      	if (resultArg) {
      	  /*inc2 += sprintf (buff2+inc2, "command 0x%02X 0x%04X 0x%04X 0x%04X    - FM3Extend, FMx1 start: 0x%04X, FMx2 start: 0x%04X, FMx3 start: 0x%04X\n",
      	    cmd[0], argShorts[0], argShorts[1], argShorts[2],
      	    argShorts[0], argShorts[1], argShorts[2]
      	  );*/
      	  for (FMExtendFlag = 1; (argShorts[FMExtendFlag - 1] != 0x0000) && (FMExtendFlag <= 3); FMExtendFlag++) {
      	  	FMExtendAddr[FMExtendFlag - 1] = argShorts[FMExtendFlag - 1];
      	  	//inc2 += sprintf (buff2+inc2, "0x%04X\n", FMExtendAddr[FMExtendFlag - 1]);
			}
			--FMExtendFlag;
		} else {
		  resultCmd = resultArg;
		};
      	break;
      case 0xC5:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MMA%hu", //command 0x%02X   0x%02X                  - Software LFO A Slot: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xC4:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (argBytes[0] % 32 == 0) {
          inc2 += sprintf (buff2+inc2, "Q%hu", //command 0x%02X   0x%02X                  - Sound Cut Setting 1: 
            //cmd[0], argBytes[0],
             ((~argBytes[0] & 0xFF) >> 5) + 1
          );
        } else if (argBytes[0] == 0xFF) {
          inc2 += sprintf (buff2+inc2, "Q0" //command 0x%02X   0x%02X                  - Sound Cut Setting 1: 
            //cmd[0], argBytes[0]
          );
    	} else {
    	  inc2 += sprintf (buff2+inc2, "Q%%%hu", //command 0x%02X   0x%02X                  - Sound Cut Setting 1: 
            //cmd[0], argBytes[0],
            (~argBytes[0]) & 0xFF
          );
        };
        break;
      case 0xC3:
      	resultArg = fileReadBytes (pmdFile, 2, argBytes);
      	if (resultArg) {
      	  if (argBytes[0] < 0x80) {
      	  	inc2 += sprintf (buff2+inc2, "px+%hd,%hu", //command 0x%02X   0x%02X 0x%02X             - Pan Setting 2: 
			//cmd[0], argBytes[0], argBytes[1],
			(signed char) argBytes[0], argBytes[1]
		    );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "px%hd,%hu", //command 0x%02X   0x%02X 0x%02X             - Pan Setting 2: 
			//cmd[0], argBytes[0], argBytes[1],
			(signed char) argBytes[0], argBytes[1]
		    );
		  }
      	  
		} else {
		  resultCmd = resultArg;
		};
		break;
	  case 0xC2:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MA%hu", //command 0x%02X   0x%02X                  - Software LFO A Delay: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xC1:
          inc2 += sprintf (buff2+inc2, "&&"//, cmd[0]
		  ); //command 0x%02X                           - Slur: 
        break;
      case 0xC0:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          switch (argBytes[0]) {
            case 0xFF:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "DF%hu", //command 0x%02X   0x%02X 0x%02X             - Absolute FM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
			case 0xFE:
			  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
			  if (argBytes[1] < 0x80) {
			  	inc2 += sprintf (buff2+inc2, "DF+%hd", //command 0x%02X   0x%02X 0x%02X             - Relative FM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  } else {
			  	inc2 += sprintf (buff2+inc2, "DF%hd", //command 0x%02X   0x%02X 0x%02X             - Relative FM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  }
              break;
            case 0xFD:
        	  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "DS%hu", //command 0x%02X   0x%02X 0x%02X             - Absolute SSG Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
			case 0xFC:
			  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
			  if (argBytes[1] < 0x80) {
			  	inc2 += sprintf (buff2+inc2, "DS+%hd", //command 0x%02X   0x%02X 0x%02X             - Relative SSG Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  } else {
			  	inc2 += sprintf (buff2+inc2, "DS%hd", //command 0x%02X   0x%02X 0x%02X             - Relative SSG Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  }
              break;
            case 0xFB:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "DP%hu", //command 0x%02X   0x%02X 0x%02X             - Absolute PCM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
			case 0xFA:
			  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
			  if (argBytes[1] < 0x80) {
			  	inc2 += sprintf (buff2+inc2, "DP+%hd", //command 0x%02X   0x%02X 0x%02X             - Relative PCM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  } else {
			  	inc2 += sprintf (buff2+inc2, "DP%hd", //command 0x%02X   0x%02X 0x%02X             - Relative PCM Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  }
              break;
            case 0xF9:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "DR%hu", //command 0x%02X   0x%02X 0x%02X             - Absolute Rhythm Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
			case 0xF8:
			  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
			  if (argBytes[1] < 0x80) {
			  	inc2 += sprintf (buff2+inc2, "DR+%hd", //command 0x%02X   0x%02X 0x%02X             - Relative Rhythm Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  } else {
			  	inc2 += sprintf (buff2+inc2, "DR%hd", //command 0x%02X   0x%02X 0x%02X             - Relative Rhythm Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  }
              break;
            case 0xF7:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "A%hu", //command 0x%02X   0x%02X 0x%02X             - PCM Method Selection: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
            case 0xF6:
              resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
              inc2 += sprintf (buff2+inc2, "DZ%hu", //command 0x%02X   0x%02X 0x%02X             - Absolute PPZ Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                argBytes[1]
              );
              break;
			case 0xF5:
			  resultArg = fileReadBytes (pmdFile, 1, &argBytes[1]);
			  if (argBytes[1] < 0x80) {
			  	inc2 += sprintf (buff2+inc2, "DZ+%hd", //command 0x%02X   0x%02X 0x%02X             - Relative PPZ Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  } else {
			  	inc2 += sprintf (buff2+inc2, "DZ%hd", //command 0x%02X   0x%02X 0x%02X             - Relative PPZ Volume Down Setting: 
                //cmd[0], argBytes[0], argBytes[1],
                (signed char) argBytes[1]
                );
			  }
              break;
            default:
              inc2 += sprintf (buff2+inc2, "m%hu", //command 0x%02X                           - Channel Mask: 
                //cmd[0], argBytes[0],
                argBytes[0]
              );
          };
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBF:
        resultArg = fileReadBytes (pmdFile, 4, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MB%hu,%hu,%hd,%hu", //command 0x%02X   0x%02X 0x%02X   0x%02X 0x%02X - Software LFO B delay, speed, depthA, depthB: 
            //cmd[0], argBytes[0], argBytes[1], argBytes[2], argBytes[3],
            argBytes[0], argBytes[1], (signed char) argBytes[2], argBytes[3]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBE:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "*B%hu", //command 0x%02X   0x%02X                  - Software LFO B Switch: 
            //cmd[0], argBytes[0], 
			argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBD:
        resultArg = fileReadBytes (pmdFile, 2, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MDB%hu,%hd", //command 0x%02X   0x%02X 0x%02X             - Software LFO B Temporal Change: 
            //cmd[0], argBytes[0], argBytes[1],
            argBytes[0], (signed char) argBytes[1]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBC:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MWB%hu", //command 0x%02X   0x%02X                  - Software LFO B Waveform: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBB:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MXB%hu", //command 0x%02X   0x%02X                  - Software LFO B Speed Setting: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xBA:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MMB%hu", //command 0x%02X   0x%02X                  - Software LFO B Slot: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xB9:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "MB%hu", //command 0x%02X   0x%02X                  - Software LFO B Delay: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xB8:
      	resultArg = fileReadBytes (pmdFile, 2, argBytes);
      	if (argBytes[0] < 0x10) {
      	  inc2 += sprintf (buff2+inc2, "O%hu,%hu", //command 0x%02X   0x%02X 0x%02X             - FM TL Setting: 
		//	cmd[0], argBytes[0], argBytes[1],
			argBytes[0], argBytes[1]
		  );
		} else if (argBytes[0] >= 0xF0) {
		  if (argBytes[1] < 0x80) {
		  	inc2 += sprintf (buff2+inc2, "O%hu,+%hd", //command 0x%02X   0x%02X 0x%02X             - FM TL Setting: 
			//cmd[0], argBytes[0], argBytes[1],
			argBytes[0] - 0xF0, (signed char) argBytes[1]
		    );
		  } else {
		  	inc2 += sprintf (buff2+inc2, "O%hu,%hd", //command 0x%02X   0x%02X 0x%02X             - FM TL Setting: 
			//cmd[0], argBytes[0], argBytes[1],
			argBytes[0] - 0xF0, (signed char) argBytes[1]
		    );
		  }
		  
		} else {
		  resultCmd = resultArg;
		};
		break;
      case 0xB7:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, ",%hu", //command 0x%02X   0x%02X                  - Software LFO AB Temporal Change Timer: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xB6:
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (argBytes[0] < 0x80) {
          inc2 += sprintf (buff2+inc2, "FB%hu", //command 0x%02X   0x%02X                  - FM FB Setting: 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
    	} else if (argBytes[0] >= 0x80 && argBytes[0] < 0xC0) {
    	  inc2 += sprintf (buff2+inc2, "FB+%hu", //command 0x%02X   0x%02X                  - FM FB Setting: 
            //cmd[0], argBytes[0],
            (argBytes[0] - 128)
          );
		} else if (argBytes[0] >= 0xC0) {
		  inc2 += sprintf (buff2+inc2, "FB%hd", //command 0x%02X   0x%02X                  - FM FB Setting: 
            //cmd[0], argBytes[0],
            (signed char) argBytes[0]
          );
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0xB5:
      	resultArg = fileReadBytes (pmdFile, 2, argBytes);
      	if (resultArg) {
      	  inc2 += sprintf (buff2+inc2, "sk%hu,%hu", //command 0x%02X   0x%02X 0x%02X             - Slot Key-On Delay: 
			//cmd[0], argBytes[0], argBytes[1],
			argBytes[0], argBytes[1]
		  );
		} else {
		  resultCmd = resultArg;
		};
		break;
	  case 0xB4:
      	resultArg = fileReadShorts (pmdFile, 8, argShorts);
      	if (resultArg) {
      	  /*inc2 += sprintf (buff2+inc2, "command 0x%02X 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X\n\t\t\t\t\t     - PPZExtend\n",
      	    cmd[0], argShorts[0], argShorts[1], argShorts[2], argShorts[3],
      	    argShorts[4], argShorts[5], argShorts[6], argShorts[7]
      	  );*/
      	  for (PPZExtendFlag = 1; (argShorts[PPZExtendFlag - 1] != 0x0000) && (PPZExtendFlag <= 8); PPZExtendFlag++) {
      	  	PPZExtendAddr[PPZExtendFlag - 1] = argShorts[PPZExtendFlag - 1];
      	  	//inc2 += sprintf (buff2+inc2, "0x%04X\n", PPZExtendAddr[PPZExtendFlag - 1]);
			}
		} else {
		  resultCmd = resultArg;
		};
      	break;
      case 0xB3:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes); 
      	if (resultArg) {
      	  inc2 += sprintf (buff2+inc2, "q,%hu", //command 0x%02X   0x%02X                  - Sound Cut Setting 2 (num3): 
            //cmd[0], argBytes[0],
            argBytes[0]
          );
		} else {
          resultCmd = resultArg;
    	};
    	break;
      case 0xB2:
      	resultArg = fileReadBytes (pmdFile, 1, argBytes);
      	if (resultArg) {
      	  inc2 += sprintf (buff2+inc2, "_M%hd", //command 0x%02X   0x%02X                  - set Master Modulation: 
			//cmd[0], argBytes[0],
			(signed char) argBytes[0]
		  );
		} else {
		  resultCmd = resultArg;
		};
		break;
      case 0x80:
        inc2 += sprintf (buff2+inc2, "\n"//, cmd[0]
		); //command 0x%02X                         - Master Loop End
        break;
      case 0x00: case 0x10: case 0x20: case 0x30: case 0x40: case 0x50: case 0x60: case 0x70:
      case 0x01: case 0x11: case 0x21: case 0x31: case 0x41: case 0x51: case 0x61: case 0x71:
      case 0x02: case 0x12: case 0x22: case 0x32: case 0x42: case 0x52: case 0x62: case 0x72:
      case 0x03: case 0x13: case 0x23: case 0x33: case 0x43: case 0x53: case 0x63: case 0x73:
      case 0x04: case 0x14: case 0x24: case 0x34: case 0x44: case 0x54: case 0x64: case 0x74:
      case 0x05: case 0x15: case 0x25: case 0x35: case 0x45: case 0x55: case 0x65: case 0x75:
      case 0x06: case 0x16: case 0x26: case 0x36: case 0x46: case 0x56: case 0x66: case 0x76:
      case 0x07: case 0x17: case 0x27: case 0x37: case 0x47: case 0x57: case 0x67: case 0x77:
      case 0x08: case 0x18: case 0x28: case 0x38: case 0x48: case 0x58: case 0x68: case 0x78:
      case 0x09: case 0x19: case 0x29: case 0x39: case 0x49: case 0x59: case 0x69: case 0x79:
      case 0x0A: case 0x1A: case 0x2A: case 0x3A: case 0x4A: case 0x5A: case 0x6A: case 0x7A:
      case 0x0B: case 0x1B: case 0x2B: case 0x3B: case 0x4B: case 0x5B: case 0x6B: case 0x7B:
        inc2order(pmdFile, 2);
        PortBuff[LENGTH_BUFFER - 1] = 0x00;
		resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          inc2 += sprintf (buff2+inc2, "%s%s%s", //command 0x%02X   0x%02X                  - play note: %s, octave: %hu, length: %hu\n
            //cmd[0], argBytes[0],
            //notes[cmd[0] & 0x0F], cmd[0] >> 4, argBytes[0]
            octaveProcessing(&cmd[0]), notes[cmd[0] & 0x0F], lengthProcessing(&argBytes[0])
          );
          if (lenFlag == LENGTH_BUFFER - 1) {
          	insertLen(pmdFile);
		  }
        } else {
          resultCmd = resultArg;
        };
        break;
      case 0x0F: case 0x1F: case 0x2F: case 0x3F: case 0x4F: case 0x5F: case 0x6F: case 0x7F:
      	inc2order(pmdFile, 2);
      	PortBuff[LENGTH_BUFFER - 1] = 0x00;
        resultArg = fileReadBytes (pmdFile, 1, argBytes);
        if (resultArg) {
          
          inc2 += sprintf (buff2+inc2, "r%s", //command 0x%02X   0x%02X                  - play rest,                length: %hu\n
            //cmd[0], argBytes[0],
            lengthProcessing(&argBytes[0])
          );
          	unsigned char octaveRest[1] = { 0xF0 };
          	octaveProcessing(octaveRest);
          if (lenFlag == LENGTH_BUFFER - 1) {
          	insertLen(pmdFile);
		  }
        } else {
          resultCmd = resultArg;
        };
        break;
      default:
        inc2 += sprintf (buff2+inc2, "command 0x%02X                         - UNKNOWN COMMAND\n", cmd[0]);
        break;
    };
  };
  return resultCmd;
}

unsigned char pmdReadSequenceRhythm (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  resultCmd;

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    /*
     * 00-7F: subroutines
     * 80-FF: sequence commands
     *
     * TODO
     * verify behaviour
     */
    if (cmd[0] < 0x80) {
      //printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      inc2 += sprintf (buff2+inc2, "R%hu", //command 0x%02X                         - execute Rhythm pattern: 
        //cmd[0], 
		cmd[0]
      );
    } else {
      fseek (pmdFile, -1, SEEK_CUR);
      resultCmd = pmdReadSequenceFM (pmdFile);
    };
  };

  return resultCmd;
}

unsigned char pmdReadRhythmSubroutineCommand (FILE* pmdFile) {
  unsigned char  cmd[1];
  unsigned char  arg[2];
  unsigned char  resultCmd;
  unsigned char  resultArg;

  unsigned char  bitCounter;
  const char*    rhythmSampleBitMap[] = {
    "Bass",
    "Snare Drum 1",
    "Low Tom",
    "Middle Tom",
    "High Tom",
    "Rim Shot",
    "Snare Drum 2",
    "Hi-Hat Close",
    "Hi-Hat Open",
    "Crash Cymbal",
    "Ride Cymbal",
    "PPS #1",
    "PPS #2",
    "PPS #3"
  };
  unsigned short  rhythmInstrumentNum;

  resultCmd = fileReadBytes (pmdFile, 1, cmd);

  if (resultCmd) {
    if (cmd[0] == 0x0F) {
    	inc2order(pmdFile, 2);
      resultArg = fileReadBytes (pmdFile, 1, arg);
      if (resultArg) {
      	
        //printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 2);
        inc2 += sprintf (buff2+inc2, "r%s", //command 0x%02X   0x%02X                  - play rest,                length: 
          //cmd[0], arg[0], 
		  lengthProcessing(&arg[0])
        );
        for (int r = 0; r < LENGTH_BUFFER; r++) {
        	RhythmBuff[r] = RhythmBuff[r+1];
		}
		RhythmBuff[LENGTH_BUFFER] = '\0';
        if (lenFlag == LENGTH_BUFFER - 1) {
        	RhythmBuff[LENGTH_BUFFER] = RhythmBuff[0];
          	insertLen(pmdFile);
		  }
      } else {
        resultCmd = resultArg;
      }
    } else if (cmd[0] >= 0x80 && cmd[0] < 0xC0) {
    	inc2order(pmdFile, 2);
      resultArg = fileReadBytes (pmdFile, 2, arg);
      if (resultArg) {
        //printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 3);
       /* printf ("command 0x%02X   0x%02X 0x%02X             - play ", //(command * 0x100 + bb) & 0x3FFF, length: %hu\n",
          cmd[0], arg[0], arg[1]
        );*/
        rhythmInstrumentNum = 0;
        for (bitCounter=0; bitCounter<8; ++bitCounter) {
          if ((arg[0] >> bitCounter) & 0x01) {
            rhythmInstrumentNum = rhythmInstrumentNum + (((arg[0] >> bitCounter) & 0x01) << bitCounter);
            //printf ("%u\n", rhythmInstrumentNum);
          }
        }
        for (bitCounter=0; bitCounter<6; ++bitCounter) {
          if ((cmd[0] >> bitCounter) & 0x01) {
            //printf ("[%s]", rhythmSampleBitMap[bitCounter+8]);
            rhythmInstrumentNum = rhythmInstrumentNum + (((((cmd[0] - 0x80) >> bitCounter) & 0x01) << bitCounter) * 256);
            //printf ("%u\n", rhythmInstrumentNum);
          };
        };
        for (int r = 0; r < LENGTH_BUFFER; r++) {
        	RhythmBuff[r] = RhythmBuff[r+1];
		}
		RhythmBuff[LENGTH_BUFFER] = rhythmInstrumentNum;
        
        if (RhythmBuff[LENGTH_BUFFER] != RhythmBuff[LENGTH_BUFFER - 1]) {
        	inc2 += sprintf (buff2+inc2, "@%u", rhythmInstrumentNum);
		}
        inc2 += sprintf (buff2+inc2, "c%s", lengthProcessing(&arg[1]));
        if (lenFlag == LENGTH_BUFFER - 1) {
        	RhythmBuff[LENGTH_BUFFER] = RhythmBuff[0];
         	insertLen(pmdFile);
		  }
      } else {
        resultCmd = resultArg;
      }
    } else if (cmd[0] == 0xFF) {
      //printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      printf ("\n"
	  //, cmd[0]
	  ); //command 0x%02X                         - Subroutine Return
    } else if (cmd[0] > 0x00 && cmd[0] < 0x80) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 1);
      printf ("command 0x%02X                         - UNKNOWN COMMAND\n", cmd[0]);
      resultCmd = 1;
    } else {
      fseek (pmdFile, -1, SEEK_CUR);
      resultCmd = pmdReadSequenceFM (pmdFile);
    };
  };

  return resultCmd;
}

unsigned char pmdReadInstrumentSectionGetOpNumber (unsigned char i) {
  switch (i%4) {
    case 0:
      return 1;
      break;
    case 1:
      return 3;
      break;
    case 2:
      return 2;
      break;
    case 3:
      return 4;
      break;
    default:
      return 255;
      break;
  };
}
/*
unsigned char pmdReadInstrumentSection (FILE* pmdFile) {
  unsigned char  dataID;
  unsigned char  dataValue;
  unsigned char  i;
  unsigned char  result;

  result = fileReadBytes (pmdFile, 1, &dataID);

  if (result) {
    printf ("Instrument ID: %hu\n", dataID);
    for (i=0; i<25; ++i) {
      result = fileReadBytes (pmdFile, 1, &dataValue);
      if (result) {
        printf ("Instrument Data[%02hu]: %hu%hu%hu%hu%hu%hu%hu%hu\n", i,
          (dataValue >> 7) & 0x01,
          (dataValue >> 6) & 0x01,
          (dataValue >> 5) & 0x01,
          (dataValue >> 4) & 0x01,
          (dataValue >> 3) & 0x01,
          (dataValue >> 2) & 0x01,
          (dataValue >> 1) & 0x01,
          (dataValue >> 0) & 0x01
        );
        switch (i/4) {
          case 0:
            printf ("OP[%hu]  - DT: %hu, ML: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              (dataValue >> 4) & 0x07,
              dataValue & 0x0F
            );
            break;
          case 1:
            printf ("OP[%hu]  - TL: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue & 0x7F
            );
            break;
          case 2:
            printf ("OP[%hu]  - KS: %hu, AR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              (dataValue >> 6) & 0x03,
              dataValue & 0x1F
            );
            break;
          case 3:
            printf ("OP[%hu]  - AM: %hu, DR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue >> 7,
              dataValue & 0x1F
            );
            break;
          case 4:
            printf ("OP[%hu]  - SR: %hu, DT2: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue & 0x1F,
              (dataValue >> 6) & 0x03
            );
            break;
          case 5:
            printf ("OP[%hu]  - SL: %hu, RR: %hu\n",
              pmdReadInstrumentSectionGetOpNumber (i),
              dataValue >> 4,
              dataValue & 0x0F
            );
            break;
          case 6:
            printf ("Common - FB: %hu, AL: %hu\n",
              (dataValue >> 3) & 0x07,
              dataValue & 0x07
            );
            break;
        };
      } else {
        break;
      };
    };
  };

  return result;
}
*/

unsigned char pmdReadInstrumentSection (FILE* pmdFile) {
  unsigned char  dataID[1];
  unsigned char  dataValue[25];
  unsigned char  i;
  unsigned char  result;
  const char	 slotOrder[] = {1,3,2,4};
  static int	 insNum;

  printf ("\n0x%X\n", ftell (pmdFile));
  result = fileReadBytes (pmdFile, 1, dataID);
  

  if (result) {
  	if (DT2Flag == 0) {
  	inc1 += sprintf (buff1+inc1, ";INS ALG  FB \n");
    inc1 += sprintf (buff1+inc1, "@%03hu ", dataID[0]); //Instrument ID
    result = fileReadBytes (pmdFile, 25, &dataValue[0]);
      if (result) {
        inc1 += sprintf (buff1+inc1, "%03hu %03hu\n", dataValue[24] & 0x07, (dataValue[24] >> 3) & 0x07); //ALG FB
        inc1 += sprintf (buff1+inc1, ";AR  DR  SR  RR  SL  TL  KS  ML DT1 AMS\n");
        for (i=0; i<4; ++i) {
          inc1 += sprintf (buff1+inc1, "%03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu\n",
			dataValue[8 + slotOrder[i] - 1] & 0x1F, //AR
			dataValue[12 + slotOrder[i] - 1] & 0x1F, //DR
			dataValue[16 + slotOrder[i] - 1] & 0x1F, //SR
			dataValue[20 + slotOrder[i] - 1] & 0x0F, //RR
			dataValue[20 + slotOrder[i] - 1] >> 4, //SL
			dataValue[4 + slotOrder[i] - 1] & 0x7F, //TL
			(dataValue[8 + slotOrder[i] - 1] >> 6) & 0x03, //KS
			dataValue[0 + slotOrder[i] - 1] & 0x0F, //ML
			(dataValue[0 + slotOrder[i] - 1] >> 4) & 0x07, //DT1
			dataValue[12 + slotOrder[i] - 1] >> 7 //AMS
		  );
		  if ((dataValue[16 + slotOrder[i] - 1] >> 6) & 0x03 > 0) {
				DT2Flag = 1;
			}
		};
		inc1 += sprintf (buff1+inc1, "\n");
		
      } else {
        result = 0;
      }	
      if (DT2Flag == 1) {
      	printf ("DT2 detected");
      	for (int i = 0; i <= inc1; i++) {
      		buff1[i] = '\0';
	  	}
	  	inc1 = 0;
	  	fseek (pmdFile, InsPoint - ftell (pmdFile) + 1, SEEK_CUR);	
	  	printf ("\nBack to 0x%X\n", ftell (pmdFile));
	  }  
	}
  	if (DT2Flag == 1) {
  		if (InsPoint == ftell (pmdFile) - 1) {
  			fseek (pmdFile, -1, SEEK_CUR);
  			result = fileReadBytes (pmdFile, 1, dataID);
		  }
  	inc1 += sprintf (buff1+inc1, ";INS ALG  FB \n");
    inc1 += sprintf (buff1+inc1, "@%03hu ", dataID[0]); //Instrument ID
    result = fileReadBytes (pmdFile, 25, &dataValue[0]);
      if (result) {
        inc1 += sprintf (buff1+inc1, "%03hu %03hu\n", dataValue[24] & 0x07, (dataValue[24] >> 3) & 0x07); //ALG FB
        inc1 += sprintf (buff1+inc1, ";AR  DR  SR  RR  SL  TL  KS  ML DT1 DT2 AMS\n");
        for (i=0; i<4; ++i) {
          inc1 += sprintf (buff1+inc1, "%03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu %03hu\n",
			dataValue[8 + slotOrder[i] - 1] & 0x1F, //AR
			dataValue[12 + slotOrder[i] - 1] & 0x1F, //DR
			dataValue[16 + slotOrder[i] - 1] & 0x1F, //SR
			dataValue[20 + slotOrder[i] - 1] & 0x0F, //RR
			dataValue[20 + slotOrder[i] - 1] >> 4, //SL
			dataValue[4 + slotOrder[i] - 1] & 0x7F, //TL
			(dataValue[8 + slotOrder[i] - 1] >> 6) & 0x03, //KS
			dataValue[0 + slotOrder[i] - 1] & 0x0F, //ML
			(dataValue[0 + slotOrder[i] - 1] >> 4) & 0x07, //DT1
			(dataValue[16 + slotOrder[i] - 1] >> 6) & 0x03, //DT2
			dataValue[12 + slotOrder[i] - 1] >> 7 //AMS
		  );
		};
		inc1 += sprintf (buff1+inc1, "\n");
      } else {
        result = 0;
      }
	}
  };

  return result;
}

unsigned char pmdReadSequence (FILE* pmdFile, unsigned char pmdSection) {
  unsigned char (* pmdReadSequenceTypes[5]) (FILE* x);

  pmdReadSequenceTypes[0] = pmdReadSequenceNoChannel;
  pmdReadSequenceTypes[1] = pmdReadSequenceFM;
  pmdReadSequenceTypes[2] = pmdReadSequenceFM;
  pmdReadSequenceTypes[3] = pmdReadSequenceFM;
  pmdReadSequenceTypes[4] = pmdReadSequenceRhythm;

  return (*pmdReadSequenceTypes[pmdSection]) (pmdFile);
}

/*
 * TODO
 * check if section exists before attempting to parse it
 * (compare ftell against the instrumentPointer?)
 */
unsigned char pmdReadRhythmData (FILE* pmdFile) {
  unsigned short lookupTable[MAXLENGTH_UNKNOWN_BUFFER];
  unsigned char  result;
  unsigned char  resultReturnRead;
  unsigned char  bufferReturnRead;
  unsigned short extraDataPointer;
  unsigned char  extraDataFlags[2];
  unsigned short i;
  unsigned char  lookupTableSize = 0;
  unsigned short SongLoopLength[2];
  
  RhythmFlag = 1;

  printf ("Look-up Table\n-------------\n");
  for (i=0; i<MAXLENGTH_UNKNOWN_BUFFER; ++i) {
    result = fileReadShorts (pmdFile, 1, &lookupTable[i]);
    printf ("R%hu -> 0x%04X\n", i, lookupTable[i]);
    if (ftell (pmdFile) > (long int) lookupTable[0] + 1) {
      result = 0;
    };
    if (!result || ftell (pmdFile) == (long int) lookupTable[0] + 1) {
      lookupTableSize = (unsigned char) i;
      break;
    };
  }

  if (result) {
    for (i=0; i<lookupTableSize; ++i) {
    	inc2 += sprintf (buff2+inc2, "\nR%d\t", i);
    	printf ("\nR%d\t", i);
    	TrackReset();
      /*
       * TODO
       * try parsing the subroutine section with the lookupTable
       */
      while (ftell (pmdFile) < lookupTable[i+1])  {
        pmdReadRhythmSubroutineCommand (pmdFile);
      };
      if (ftell (pmdFile) == lookupTable[i+1]) {
        resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
        if (resultReturnRead && bufferReturnRead == 0xFF) {
          fseek (pmdFile, -1, SEEK_CUR);
          pmdReadRhythmSubroutineCommand (pmdFile);
        } else {
          printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
          result = 0;
          break;
        };
      } else {
        printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
        result = 0;
        break;
      };
    };
    resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
    inc2 += sprintf (buff2+inc2, "\nR%d\t", i);
    TrackReset();
    while (resultReturnRead && bufferReturnRead != 0xFF)  {
      fseek (pmdFile, -1, SEEK_CUR);
      pmdReadRhythmSubroutineCommand (pmdFile);
      resultReturnRead = fileReadBytes (pmdFile, 1, &bufferReturnRead);
    };
    if (bufferReturnRead == 0xFF) {
      fseek (pmdFile, -1, SEEK_CUR);
      pmdReadRhythmSubroutineCommand (pmdFile);
    } else {
      printf ("Subroutine #%hu does not end in Subroutine Return command.\n", i);
      result = 0;
    };
  };
	/*
  if (result) {
    resultReturnRead = fileReadShorts (pmdFile, 2, SongLoopLength);
      printf("Song Length: %u\n", (unsigned int) (SongLoopLength[1]*65536) + SongLoopLength[0]);
      if (!resultReturnRead) {
        result = 0;
  	  };
  };
  if (result) {
    resultReturnRead = fileReadShorts (pmdFile, 2, SongLoopLength);
      printf("Loop Length: %u\n", (unsigned int) (SongLoopLength[1]*65536) + SongLoopLength[0]);
      if (!resultReturnRead) {
        result = 0;
  	  };
  };

  if (result) {
    resultReturnRead = fileReadShorts (pmdFile, 1, &extraDataPointer);
    if (!resultReturnRead) {
      result = 0;
    };
  };

  if (result) {
    resultReturnRead = fileReadBytes (pmdFile, 2, extraDataFlags);
    if (resultReturnRead) {
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 4);
      printf ("Metadata Address: 0x%04X\n", extraDataPointer);
      printf ("0x%04X: ", (unsigned short) ftell (pmdFile) - 2);
      printf ("MC.EXE Version:    0x%02X\n", extraDataFlags[0]);
      if (extraDataFlags[1] != 0xFE) {
        result = 0;
      };
    };
  }; */

  return result;
}

unsigned char pmdLoad (char* pmdFilename) {
  struct_pmdHeader pmdHeader;
  struct_pmdOpnaTrackPointers opnaTrackPointers;
  unsigned short   i, j;
  unsigned char    bufferReturnRead;
  unsigned short   extraDataPointer;
  unsigned short   extraDataTable[256];
  unsigned char    extraDataBuffer[256];
  unsigned char    extraDataFlags[2];
  unsigned short   junk;
  unsigned char    pmdReadSuccess = 0;
  FILE*            fp = NULL;
  unsigned char    pmdSection = 0;
  unsigned short   SongLoopLength[2];
  unsigned char	   x;
  unsigned char*   FMxName[] = {"X", "Y", "Z"};
  unsigned char*   PPZName[] = {"a", "b", "c", "d", "e", "f", "g", "h"};

  /*
   * TODO
   * load based on version / whatever instead of always OPNA
   * see PMDMML_EN.MAN:
   * 1.1.3. Correspondence Between Channel Notation and Sound Sources
   */
  pmdHeader.trackPointers = (void*) &opnaTrackPointers;

  fp = fopen (pmdFilename, "rb");

  /*
   * TODO
   * this is really ugly, clean up
   * (array of function pointers in ANSI C?)
   */
  if (fp != NULL) {
    while (1) {
      pmdReadSuccess = pmdReadVersion (fp, &pmdHeader.version);
      if (!pmdReadSuccess) break;

      pmdReadSuccess = pmdReadTrackPointers (fp, OPNA, pmdHeader.trackPointers);
      if (!pmdReadSuccess) break;
      /*
      pmdReadSuccess = pmdReadRhythmSubroutineTablePointer (fp, &pmdHeader.rhythmSubroutineTablePointer);
      if (!pmdReadSuccess) break;
      */

      pmdReadSuccess = pmdReadFmInstrumentPointer (fp, &pmdHeader.fmInstrumentPointer);
      break;
    }
    if (!pmdReadSuccess) {
      printf ("Failed to read header data.\n");
    };
  } else {
    printf ("Failed to open file '%s'.\n", pmdFilename);
    pmdReadSuccess = 0;
  };

  while (1) {
    if (!pmdReadSuccess) {
      break;
    };

    printf ("HEADER\n------\n");
    TrackReset();
    printf ("VERSION ID\t0x%02x\n", pmdHeader.version);
    for (i=0; i<=5; ++i) {
      printf ("FM%d    SEQUENCE\t0x%04hX\n", i+1, opnaTrackPointers.FM[i]);
    };
    for (i=i; i<=8; ++i) {
      printf ("SSG%d   SEQUENCE\t0x%04hX\n", i-5, ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[i-6]);
    };
    printf ("ADPCM  SEQUENCE\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM);
    printf ("Rhythm SEQUENCE\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm);
    printf ("Rhythm DATA\t0x%04hX\n", ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer);
    printf ("FM INSTRUMENTS\t0x%04hX\n", pmdHeader.fmInstrumentPointer);

    /*
    fseek (fp, pmdHeader.trackPointers.FM[0] + 1, SEEK_SET);
    */
    ++pmdSection;
    inc2 += sprintf (buff2+inc2, "A\t");
    ZENLEN = 0x60;
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[1] + 1) { //Read up to B
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[1] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nB\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[2] + 1) { //Read up to C
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[2] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nC\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[3] + 1) { //Read up to D
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[3] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nD\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[4] + 1) { //Read up to E
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[4] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nE\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[5] + 1) { //Read up to F
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->FM[5] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nF\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[0] + 1) { //Read up to G
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[0] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    ++pmdSection;
    inc2 += sprintf (buff2+inc2, "\nG\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[1] + 1) { //Read up to H
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[1] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nH\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[2] + 1) { //Read up to I
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->SSG[2] + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };
    
    inc2 += sprintf (buff2+inc2, "\nI\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM + 1) { //Read up to J
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->ADPCM + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    ++pmdSection;
    inc2 += sprintf (buff2+inc2, "\nJ\t");
    TrackReset();
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm + 1) { //Read up to K
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->Rhythm + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

    /*
     * TODO
     * CLEAN THIS MESS UP AAAAAAAAAA
     * move all this Rhythm stuff into a more general function
     */
    ++pmdSection; 
    inc2 += sprintf (buff2+inc2, "\nK\t");
    TrackReset();
    if (FMExtendFlag != 0 || PPZExtendFlag != 0) { //If FMExtend or PPZExtend is present
    	//FMExtend
    	//printf("pass\n");
    	//printf("0x%04X	0x%08X\n", ftell (fp), FMExtendAddr[0]);
    	//pmdReadSuccess = 0;
    	//break;
    	
    	while ((ftell (fp) < FMExtendAddr[0] + 1) && FMExtendFlag != 0) { //Read up to X (Read K)
    		pmdReadSequence (fp, pmdSection);
		}
		
		if (FMExtendFlag != 0) { //The previous while will do nothing if there is no FMExtendFlag, so this will only disable Rhythm reading if there is FMExtend to read
			pmdSection = 3;
		}
		
		for (x = 1; FMExtendAddr[x] != 0x0000 && x <= 2; x++) { //Read up to Y or Z (Read X and maybe Y)
			inc2 += sprintf (buff2+inc2, "\n%s\t", FMxName[x-1]);
			TrackReset();
			while (ftell (fp) < FMExtendAddr[x] + 1) {
				pmdReadSequence (fp, pmdSection);
			}
		}
		if (FMExtendAddr[x-1] != 0x0000){
			inc2 += sprintf (buff2+inc2, "\n%s\t", FMxName[x-1]);
			TrackReset();
		}
		//PPZExtend
		while ((ftell (fp) < PPZExtendAddr[0] + 1) && PPZExtendFlag != 0) { //Read up to a (Read K or Z). It will read as K if there is no FMExtend
    		pmdReadSequence (fp, pmdSection);
		}
		
		if (PPZExtendFlag != 0) { //The previous while will do nothing if there is no PPZExtendFlag, so this will only disable Rhythm reading if there is FMExtend to read
			pmdSection = 3;
		}
		
		for (x = 1; PPZExtendAddr[x] != 0x0000 && x <= 7; x++) { //Read up to last PPZ channel
			inc2 += sprintf (buff2+inc2, "\n%s\t", PPZName[x-1]);
			TrackReset();
			while (ftell (fp) < PPZExtendAddr[x] + 1) {
				pmdReadSequence (fp, pmdSection);
			}
		}
		if (PPZExtendAddr[x-1] != 0x0000){
			inc2 += sprintf (buff2+inc2, "\n%s\t", PPZName[x-1]);
			TrackReset();
		}
	}
	
    while (ftell (fp) < ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer + 1) { //Read up to Lookup Table (Read last FM3x or last PPZ or K)
      pmdReadSequence (fp, pmdSection);
    };
    if (ftell (fp) != ((struct_pmdOpnaTrackPointers*) pmdHeader.trackPointers)->RhythmPatternTablePointer + 1) {
      printf ("Parsed data does not align with specified sequence size.\n");
      pmdReadSuccess = 0;
      break;
    };

	pmdSection = 4;
    if (ftell (fp) + 12 < pmdHeader.fmInstrumentPointer) {
      pmdReadSuccess = pmdReadRhythmData (fp);
      if (!pmdReadSuccess) {
        break;
      };
    } else {
      printf ("No Rhythm subroutines found.\n");
    };      
      if (pmdReadSuccess) {
      	if (pmdHeader.fmInstrumentPointer - ftell (fp) == 11) { //Possibly only present in 4.7/4.8
      		pmdReadSuccess = fileReadShorts (fp, 2, SongLoopLength);
      		printf("Song Length: %u\n", (unsigned int) (SongLoopLength[1]*65536) + SongLoopLength[0]);
      		
      		if (!pmdReadSuccess) {
          		pmdReadSuccess = 0;
  	    	};
  	    	
      		if (pmdReadSuccess) {
    			pmdReadSuccess = fileReadShorts (fp, 2, SongLoopLength);
      			printf("Loop Length: %u\n", (unsigned int) (SongLoopLength[1]*65536) + SongLoopLength[0]);
      			
      			if (!pmdReadSuccess) {
      	  			pmdReadSuccess = 0;
  	    		};
  	  		};
	  	}
	  }
      if (pmdHeader.fmInstrumentPointer - ftell (fp) == 3) {
      	if (pmdReadSuccess) {
        pmdReadSuccess = fileReadShorts (fp, 1, &extraDataPointer);
      };

      if (pmdReadSuccess) {
        pmdReadSuccess = fileReadBytes (fp, 2, extraDataFlags);
        if (pmdReadSuccess) {
          printf ("0x%04X: ", (unsigned short) ftell (fp) - 4);
          printf ("Metadata Address: 0x%04X\n", extraDataPointer);
          printf ("0x%04X: ", (unsigned short) ftell (fp) - 2);
          printf ("MC.EXE Version:    0x%02X\n", extraDataFlags[0]);
          if (extraDataFlags[1] != 0xFE) {
            pmdReadSuccess = 0;
          };
        };
      };
	  } else {
	  	printf ("Unknown MC.EXE Version.\n"); //Possibly Pre-4.3
	  }


    if (pmdReadSuccess) {
      InsPoint = ftell (fp);
      printf ("\n0x%X\n", InsPoint);
      pmdReadSuccess = fileReadShorts (fp, 1, &junk);
      while (pmdReadSuccess && junk != 0xFF00) {
        fseek (fp, -2, SEEK_CUR);
        pmdReadSuccess = pmdReadInstrumentSection (fp);
        if (!pmdReadSuccess) break;
        pmdReadSuccess = fileReadShorts (fp, 1, &junk);
        if (!pmdReadSuccess) break;
      };
    };
    FILE *fMML;
      char mmlFilename[strlen(pmdFilename) - strlen(strrchr(pmdFilename, '.')) + 5];
      const char* MMLExt[] = {".MML"};
      for (i = 0; i < (strlen(pmdFilename) - strlen(strrchr(pmdFilename, '.'))); i++) {
      	mmlFilename[i] = pmdFilename[i];
      	mmlFilename[i+1] = '\0';
	  }
	  fMML = fopen(strcat(mmlFilename, *MMLExt), "rb");
	if (fMML != NULL) {
	  	mmlFilename[0] = '2';
	}
    if (pmdReadSuccess) {
    	fMML = fopen(mmlFilename, "w");
	} 	
		  
      	
    if (getc(fp) == EOF) { //Possibly Pre-4.0i
		printf ("No Metadata!\n");
		if (FMExtendFlag != 0){
      	fprintf (fMML, "#FM3Extend      ");
      	for (i = 0; i < FMExtendFlag; i++) {
      	  fprintf (fMML, "%s", FMxName[i]);
		  }
		fprintf (fMML, "\n");
	  }
	  if (PPZExtendFlag != 0){
      	fprintf (fMML, "#PPZExtend      ");
      	for (i = 0; i < PPZExtendFlag - 1; i++) {
      	  fprintf (fMML, "%s", PPZName[i]);
		  }
		fprintf (fMML, "\n");
	  }
	  if (DT2Flag == 1) {
	  	fprintf (fMML, "#DT2Flag        On\n");
	  }
      
      fprintf (fMML, "#Option         /v/c");
      if (pmdHeader.version == 0x00){
      	fprintf (fMML, "\n");
      	printf ("OPN\n");
	  } else if (pmdHeader.version == 0x01) {
	  	fprintf (fMML, "/m\n");
	  	printf ("OPM\n");
	  } else if (pmdHeader.version == 0x02) {
	  	fprintf (fMML, "/l");
	  	printf ("OPL\n");
	  } else {
	  	fprintf (fMML, "\n");
	  	printf ("Unknown Chip Header Info");
	  }
      
      fprintf (fMML, "\n%s", buff1); //print Instruments to MML
      fprintf (fMML, "%s", buff2); //print Channels to MML
      fclose (fp);
      fclose (fMML);
      for (int i = 0; i <= inc1; i++) {
      	buff1[i] = '\0';
	  }
	  
	  for (int i = 0; i <= inc2; i++) {
      	buff2[i] = '\0';
	  }
	  inc1 = 0;
	  inc2 = 0;
	  TrackReset();
      pmdReadSuccess = 0;
	}

    if (pmdReadSuccess) {
      fseek (fp, extraDataPointer+1, SEEK_SET);
      /*
       * TODO
       * pmdReadExtraData (fp);
       */
      for (i=0; i<256; ++i) {
        pmdReadSuccess = fileReadShorts (fp, 1, &extraDataTable[i]);
        if (!pmdReadSuccess) break;
        if (extraDataTable[i] == 0x0000) break;
      };
    };

    if (pmdReadSuccess) {
      
      i=0;
      while (extraDataTable[i] != 0x0000) {
        fseek (fp, extraDataTable[i]+1, SEEK_SET);
        j=0;
        pmdReadSuccess = fileReadBytes (fp, 1, &extraDataBuffer[j]);
        if (!pmdReadSuccess) break;
        while (extraDataBuffer[j++] != 0x00) {
          pmdReadSuccess = fileReadBytes (fp, 1, &extraDataBuffer[j]);
          if (!pmdReadSuccess) break;
        };
        if (*extraDataBuffer == 0x00) {
        	++i;       	
		} else {
			if (extraDataFlags[0] == 0x48) {
			  switch (i) {
          		case 0:
            	  	fprintf (fMML, "#PPZFile        %s\n", extraDataBuffer);
           		 	break;
          		case 1:
            		fprintf (fMML, "#PPSFile        %s\n", extraDataBuffer);
            		break;
          		case 2:
            		fprintf (fMML, "#PCMFile        %s\n", extraDataBuffer);
            		break;
          		case 3:
            		fprintf (fMML, "#Title          %s\n", extraDataBuffer);
            		break;
          		case 4:
           			fprintf (fMML, "#Composer       %s\n", extraDataBuffer);
            		break;
          		case 5:
            		fprintf (fMML, "#Arranger       %s\n", extraDataBuffer);
            		break;
          		default:
            		fprintf (fMML, "#Memo           %s\n", extraDataBuffer);
            		break;
        	  };
        	  ++i;
			} else {
			  switch (i) {
          		case 0:
            		fprintf (fMML, "#PPSFile        %s\n", extraDataBuffer);
            		break;
          		case 1:
            		fprintf (fMML, "#PCMFile        %s\n", extraDataBuffer);
            		break;
          		case 2:
            		fprintf (fMML, "#Title          %s\n", extraDataBuffer);
            		break;
          		case 3:
           			fprintf (fMML, "#Composer       %s\n", extraDataBuffer);
            		break;
          		case 4:
            		fprintf (fMML, "#Arranger       %s\n", extraDataBuffer);
            		break;
          		default:
            		fprintf (fMML, "#Memo           %s\n", extraDataBuffer);
            		break;
        	  };
        	  ++i;
			}
        
      };
  	  };
      
      if (FMExtendFlag != 0){
      	fprintf (fMML, "#FM3Extend      ");
      	for (i = 0; i < FMExtendFlag; i++) {
      	  fprintf (fMML, "%s", FMxName[i]);
		  }
		fprintf (fMML, "\n");
	  }
	  if (PPZExtendFlag != 0){
      	fprintf (fMML, "#PPZExtend      ");
      	for (i = 0; i < PPZExtendFlag - 1; i++) {
      	  fprintf (fMML, "%s", PPZName[i]);
		  }
		fprintf (fMML, "\n");
	  }
	  
      if (DT2Flag == 1) {
	  	fprintf (fMML, "#DT2Flag        On\n");
	  }
      fprintf (fMML, "#Option         /v/c");
      if (pmdHeader.version == 0x00){
      	fprintf (fMML, "\n");
      	printf ("OPN\n");
	  } else if (pmdHeader.version == 0x01) {
	  	fprintf (fMML, "/m\n");
	  	printf ("OPM\n");
	  } else if (pmdHeader.version == 0x02) {
	  	fprintf (fMML, "/l");
	  	printf ("OPL\n");
	  } else {
	  	fprintf (fMML, "\n");
	  	printf ("Unknown Chip Header Info");
	  }
      fprintf (fMML, "#Filename       T_%s\n", pmdFilename);
      
      fprintf (fMML, "\n%s", buff1); //print Instruments to MML
      fprintf (fMML, "%s", buff2); //print Channels to MML
      fclose(fMML);
      for (int i = 0; i <= inc1; i++) {
      	buff1[i] = '\0';
	  }

	  for (int i = 0; i <= inc2; i++) {
      	buff2[i] = '\0';
	  }
	  inc1=0;
	  inc2=0;
	  TrackReset();
    };

    break;
  }
  if (!pmdReadSuccess) {
    printf ("Error occurred.\n");
  };

  if (fp != NULL) {
    fclose (fp);
  };

  return !pmdReadSuccess;
  getchar();
}

unsigned char pmdLoadDebug (void) {
  return pmdLoad (DEBUG_PMD_FILE);
}

int main (int argc, char* argv[]) {
  if (argc >= 2) {
  	for (int i = 1; i < argc; i++) {
		printf ("%s\n", argv[i]);
		pmdLoad (argv[i]);
	}
  } else {
    printf ("This program decompiles OPNA PMD files into PMDMML files. \n\n");
    printf ("75%% of the code came from libopenpmd, credit to OPNA2608 on GitHub. \n");
    printf ("The sloppiest 25%% was written by Blargzargo.\n");
    return 0;
  };
}
