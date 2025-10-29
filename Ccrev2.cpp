#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

extern struct VARS GVPtr,GRPtr;
#if Z80
void IncOp(struct OP_DEF *);
void DecOp(struct OP_DEF *);
#endif

int Ccc::subShift(uint8_t m, int T, long VType, uint8_t VSize, long RType, union STR_LONG *VCost, union STR_LONG *RCost, struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3) {
  char AS[16],BS[16],MyBuf[16],MyBuf1[16];
  int i,n;

  if((RType & VARTYPE_IS_POINTER) || (VType & VARTYPE_IS_POINTER))
    PROCError(2111,NULL);    // messaggio da cambiare...
  if(m) {
#if ARCHI
    _tcscpy(AS,"ASL");
#elif Z80
    _tcscpy(AS,"sla");
#elif I8086
    _tcscpy(AS,"shl");// finire
#elif MC68000
    _tcscpy(AS,"lsl");
#elif I8051
    _tcscpy(AS,"sal");
#elif MICROCHIP
    _tcscpy(AS,"RLF");
#endif
    }
  else {
    if(VType & VARTYPE_UNSIGNED)
#if ARCHI
      _tcscpy(AS,"LSR");
#elif Z80
      _tcscpy(AS,"srl");
#elif I8086
      _tcscpy(AS,"shr");
#elif MC68000
      _tcscpy(AS,"lsr");
#elif I8051
      _tcscpy(AS,"shr");
#elif MICROCHIP
      _tcscpy(AS,"RRF");
#endif
    else 
#if ARCHI
      _tcscpy(AS,"ASR");
#elif Z80
      _tcscpy(AS,"sra");
#elif I8086
      _tcscpy(AS,"sar");
#elif MC68000
      _tcscpy(AS,"asr");
#elif I8051
      _tcscpy(AS,"sar");
#elif MICROCHIP
      _tcscpy(AS,"RRF");
#endif
    }
#if MC68000
	switch(VSize) {
		case 1:
			_tcscat(AS,".b");
			break;
		case 2:
			_tcscat(AS,".w");
			break;
		case 4:
			_tcscat(AS,".l");
			break;
		}
#endif
	switch(VSize) {
	  case 1:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          _tcscpy(MyBuf,AS);
          _tcscat(MyBuf,Dr1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr,NULL,MyBuf);
#elif Z80
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#elif I8086
//          if(CPU86<2) {

          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#elif MC68000
//          if(CPU86<2) {

          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#elif MICROCHIP
//          if(CPUPIC<2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#endif
          break;
        case 1:                        
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          _tcscpy(MyBuf1,AS);
          _tcscat(MyBuf1," ");
          _tcscat(MyBuf1,Dr);
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,MyBuf,NULL,MyBuf1);
#elif Z80
          i=VCost->l;
myVShift1:
          if(i<8) {
            while(i--)
              PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,0);
            // manca signed/unsigned
            }
#elif I8086
          i=VCost->l;
myVShift1:          
          if(i<8) {
						if(CPU86<2) {
							while(i--)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
							}
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,0);
            // manca signed/unsigned
            }
#elif MC68000
          i=VCost->l;
myVShift1:          
					if(i<=8)
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,i & 7,OPDEF_MODE_REGISTRO,Regs->D    -1);
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,i,OPDEF_MODE_REGISTRO,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
						}
					// VERIFICARE il registro di lavoro arriva ciucco per il 68000 e in altri casi non l'abbiamo usato SISTEMARE
#elif MICROCHIP
          i=VCost->l;
myVShift1:
          if(i<8) {
            while(i--)
              PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,0);
            // manca signed/unsigned
            }
#endif
          break;
        case 2:                    
#if ARCHI
      		sprintf(MyBuf,"%s#%s",AS,RCost);
      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80 || I8086 || MC68000
          i=RCost->l;
          goto myVShift1;
#elif MICROCHIP
          i=RCost->l;
          goto myVShift1;
#endif
      		break;
    		default:
      		break;
    		}
    	break;
	  case 2:
#if Z80
      if(m)
        _tcscpy(BS,"rl");
      else
        _tcscpy(BS,"rr");
#endif
      u[1]=u[0];         // preparo il byte alto in u[1] che non è usato
      switch(T) {
        case -1:
        case VARTYPE_PLAIN_INT:
#if ARCHI
          _tcscpy(MyBuf,AS);
          _tcscat(MyBuf,Dr1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);   // uso solo byte basso
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          if(m) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
			      IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
			      IncOp(&u[1]);
  	        PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
            } 
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#elif I8086
          if(CPU86<2) {
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);   //
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          }
	        else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Regs->DSl);
	          }  
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D   -1);
//          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);   // uso solo byte basso
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          if(m) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
			      IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
			      IncOp(&u[1]);
  	        PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
            } 
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMP,"GOTO",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#endif
          break;
        case 1:                        
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          _tcscpy(MyBuf1,AS);
          _tcscat(MyBuf1," ");
          _tcscat(MyBuf1,Dr);
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,MyBuf,NULL,MyBuf1);
#elif Z80
          i=VCost->l;
myVShift:
          if(i<16) {
myVShift_:
            if(i<8) {
              while(i--) {
			          if(m) {
			            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
						      IncOp(&u[1]);
			            PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
			            }
			          else {
			            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
						      IncOp(&u[1]);
			  	        PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
			            } 
                }
              }
            else {
              if(m) {
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[0].mode,&u[0].s,u[0].ofs);
                // non va su (hl)...
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              else {  
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              i-=8;
              goto myVShift_;
              }    
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO8,0);
            // manca signed/unsigned
            }
#elif I8086            
          i=VCost->l;
myVShift:
          if(CPU86<2) {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,4,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,4,0);
//            PROCOper(LINE_TYPE_ISTRUZIONE,movString,"cl",i);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,"cl");
            }
          else {  
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,4,0);
//            PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,i);
            }
#elif MC68000
          i=VCost->l;
myVShift:
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO,Regs->D);
					if(i<=8)
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,i & 7,OPDEF_MODE_REGISTRO,Regs->D    -1);
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,i,OPDEF_MODE_REGISTRO,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
						}
					// VERIFICARE il registro di lavoro arriva ciucco per il 68000 e in altri casi non l'abbiamo usato SISTEMARE


#elif MICROCHIP
          i=VCost->l;
myVShift:
          if(i<16) {
myVShift_:
            if(i<8) {
              while(i--) {
			          if(m) {
			            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
						      IncOp(&u[1]);
			            PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
			            }
			          else {
			            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
						      IncOp(&u[1]);
			  	        PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
			            } 
                }
              }
            else {
              if(m) {
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[0].mode,&u[0].s,u[0].ofs);
                // non va su (hl)...
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              else {  
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              i-=8;
              goto myVShift_;
              }    
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO,0);
            // manca signed/unsigned
            }
#endif
          break;
        case 2:                    
#if ARCHI
      		sprintf(MyBuf,"%s#%s",AS,RCost);
      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80 || I8086 || MC68000
          i=RCost->l;
          goto myVShift;
#elif MICROCHIP
          i=RCost->l;
          goto myVShift;
#endif
      		break;
    		default:
      		break;
    		}
	    break;
	  case 4:
#if Z80
      u[2]=u[0];         // preparo i byte alto in u[2,3] non usati
      IncOp(&u[2]);
      u[3]=u[1];         // preparo i byte alto
      IncOp(&u[3]);
#elif MICROCHIP
      u[2]=u[0];         // preparo i byte alto in u[2,3] non usati
      IncOp(&u[2]);
      u[3]=u[1];         // preparo i byte alto
      IncOp(&u[3]);
#endif
      switch(T) {
        case -1:
        case VARTYPE_PLAIN_INT:
#if ARCHI
          _tcscpy(MyBuf,AS);
          _tcscat(MyBuf,Dr1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);                 // uso byte basso e basta
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          if(m) {
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[3].mode,&u[3].s,u[3].ofs);
	          }
	        else {
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
	          }  
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#elif I8086
          if(CPU86<2) {
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);                 // uso byte basso e basta
//	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,"cl",Dr1);
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,"cl");
//	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,Dr1,"cl");
	          }
	        else {
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Regs->DSl);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr1,Regs->DSl);
	          }  
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
//						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,i,OPDEF_MODE_REGISTRO,Regs->D    -1);
#elif MICROCHIP
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);                 // uso byte basso e basta
          PROCOutLab(FNGetLabel(MyBuf,2));
          PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
          PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
          if(m) {
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[3].mode,&u[3].s,u[3].ofs);
	          }
	        else {
	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
	          }  
          PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO_HIGH8,3);
          FNGetLabel(MyBuf1,2);
          PROCOper(LINE_TYPE_JUMP,"GOTO",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
          PROCOutLab(MyBuf1);
#endif
          break;
        case 1:                        
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          _tcscpy(MyBuf1,AS);
          _tcscat(MyBuf1," ");
          _tcscat(MyBuf1,Dr);
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,MyBuf,NULL,MyBuf1);
#elif Z80
          i=VCost->l;
myVShift4:
          if(i<32) {
myVShift4_:
            if(i<8) {
              while(i--) {
			          if(m) {
				          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[3].mode,&u[3].s,u[3].ofs);
				          }
				        else {
				          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
				          }  
                }
              }
            else {
              if(m) {
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[2].mode,&u[2].s,u[2].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,u[0].mode,&u[0].s,u[0].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,0);
                }
              else {  
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,u[2].mode,&u[2].s,u[2].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[3].mode,&u[3].s,u[3].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_IMMEDIATO8,0);
                }
              i-=8;
              goto myVShift4_;
              }    
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_IMMEDIATO8,0);
            // manca signed/unsigned
            }
#elif I8086
          i=VCost->l;
myVShift4:
          if(CPU86<2) {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,u[2].mode,&u[2].s,u[2].ofs);
//            PROCOper(LINE_TYPE_ISTRUZIONE,movString,"cl",i);
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,"cl");
//	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,Dr1,"cl");
            }
          else {  
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
//            PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,i);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,Dr1,i);
            }
#elif MC68000
          i=VCost->l;
myVShift4:
					if(i<=8)
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,i,OPDEF_MODE_REGISTRO,Regs->D    -1);
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,i,OPDEF_MODE_REGISTRO,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
						}
					// VERIFICARE il registro di lavoro arriva ciucco per il 68000 e in altri casi non l'abbiamo usato SISTEMARE
#elif MICROCHIP
          i=VCost->l;
myVShift4:
          if(i<32) {
myVShift4_:
            if(i<8) {
              while(i--) {
			          if(m) {
				          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[3].mode,&u[3].s,u[3].ofs);
				          }
				        else {
				          PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[1].mode,&u[1].s,u[1].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[2].mode,&u[2].s,u[2].ofs);
				          PROCOper(LINE_TYPE_ISTRUZIONE,BS,u[0].mode,&u[0].s,u[0].ofs);
				          }  
                }
              }
            else {
              if(m) {
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[2].mode,&u[2].s,u[2].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,u[0].mode,&u[0].s,u[0].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              else {  
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,u[2].mode,&u[2].s,u[2].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,u[1].mode,&u[1].s,u[1].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,u[3].mode,&u[3].s,u[3].ofs);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              i-=8;
              goto myVShift4_;
              }    
            }
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,OPDEF_MODE_IMMEDIATO8,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_IMMEDIATO8,0);
            // manca signed/unsigned
            }
#endif
          break;
        case 2:                    
      		sprintf(MyBuf,"%s#%s",AS,RCost);
#if ARCHI
      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80 || I8086 || MC68000
          i=RCost->l;
          goto myVShift4;
#elif MICROCHIP
          i=RCost->l;
          goto myVShift4;
#endif
      		break;
    		default:
      		break;
    		}
	    break;
    }	
		                  
  return 1;
  }  
                         
int Ccc::subAdd(int m, int T, int VQ, struct VARS *VVar, long *VType, int *VSize,
								long RType, int RSize, union STR_LONG *VCost, union STR_LONG *RCost, 
								struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3,int m1) {
  char AS[16],myBuf[16],T1S[16];                                        // m1 per auto-assign
  int i,OldSize;
  char *p,*p1;
  long l;


  OldSize=*VSize;
  if(m) {
    if((RType & VARTYPE_IS_POINTER) && (*VType & VARTYPE_IS_POINTER))     // lecito sommare int o int e ptr..
      PROCError(2111,NULL);
    }                                     // lecito sottrarre ptr.. o ptr e int
  *T1S=0;
  if(*VType & VARTYPE_IS_POINTER) {
#if ARCHI
    if(T==2) {
      sprintf(T1S,"ASL #%d",log((double)*VSize)/log(2.0));
      }
    else {
      RCost->l=RCost->l *(*VSize);
      }
#elif Z80 || I8086 || MC68000 || MICROCHIP
    RCost->l *= FNGetMemSize(*VType,*VSize,2);    // sarebbe da agg. per struct/array
#endif
    *VSize=PTR_SIZE;
    }
  if(RType & VARTYPE_IS_POINTER) {
    if(RSize>1) {
#if ARCHI
      if(T==1) {
        sprintf(T1S,"ASL #%d",log(RSize)/log(2));
        }
      else {
        VCost->l=VCost->l *RSize;
        }
#elif Z80 || I8086 || MC68000 || MICROCHIP
      VCost->l *= FNGetMemSize(RType,RSize,2);   // manca struct/array
#endif
      }         
    *VSize=PTR_SIZE;
    *VType=RType;
    OldSize=RSize;           // tutto questo se il primo è cost.
    }
#if ARCHI
  _tcscpy(AS,m ? "ADD" : "SUB");
#elif Z80 || I8086
  _tcscpy(AS,m ? "add" : "sub");
#elif MC68000
  _tcscpy(AS,m ? "add" : "sub");
	switch(*VSize) {
		case 1:
		  _tcscat(AS,".b");
			break;
		case 2:
		  _tcscat(AS,".w");
			break;
		case 4:
		  _tcscat(AS,".l");
			break;
		}
#elif MICROCHIP
  _tcscpy(AS,m ? "ADDWF" : "SUBWF");
#endif
	switch(*VSize) {
	  case 1:
  		switch(T) {
        case -1:
#if Z80
				  OpA(AS,&u[0],&u[1]);
				  break;
#elif MICROCHIP
				  OpA(AS,&u[0],&u[1]);
				  break;

					// #else prosegue??
#endif				  
    		case 0:
#if ARCHI
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
#elif Z80
          if(Regs->D==0) {
        		if(*AS == 's') {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
          		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
        		  }
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
        		}
          else
 					  OpA(AS,&u[0],&u[1]);
#elif I8086
       		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
#elif MC68000
       		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
#elif MICROCHIP
          if(Regs->D==0) {
        		if(*AS == 's') {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
          		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
        		  }
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
        		}
          else
 					  OpA(AS,&u[0],&u[1]);
#endif
      		break;
    		case 1:                                
			    i=VCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(m)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,i,&u[0]);
#elif I8086
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
#elif MC68000
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(m)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,i,&u[0]);
#endif
      		break;
    		case 2:
			    i=RCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(m)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,&u[0],i);
#elif I8086
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
#elif MC68000
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(m)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,&u[0],i);
#endif
      		break;
    		}
      break;
    case 2:
  		switch(T) {
        case -1:
#if Z80
          goto caseT0;
          break;
#endif
    		case 0:
#if ARCHI
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
#elif Z80
											      
          if(VQ==3 && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,*(int *)VVar->label,OPDEF_MODE_REGISTRO,Regs->D);
        		goto dontChgV;
        		}
          else {
	          if(Regs->D==1) {
	        		if(*AS == 's') {
	        		  *(AS+1)='b';
	        		  *(AS+2)='c';
	          		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	        		  }
							if((*VType & 15) && !(RType & 15)) {
		  				  i=OldSize;
		  				  while(i--)
    	        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
								}
							else	
	        		  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
	        		}
	          else {
caseT0:	          
							if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
//							printf("\asommo ptr %d %d, %d %d",u[0].mode,u[0].s.n,u[1].mode,u[1].s.n);
		  				  i=OldSize;
		  				  u[2]=u[0];
		  				  u[3]=u[1];
		  				  while(i--) {
			  				  u[0]=u[2];
			  				  u[1]=u[3];
									Op2A(AS,&u[0],&u[1]);
									}
								}
							else	
							  Op2A(AS,&u[0],&u[1]);
	         		}
	         	}	
#elif I8086
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
  				  i=OldSize;
  				  while(i >>= 1)
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sla",OPDEF_MODE_REGISTRO,0);
//						  PROCOper(LINE_TYPE_ISTRUZIONE,"sla",Dr,NULL);
						}
          if(VQ==3 && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
       		  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
//        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Regs[VVar],Dr);
        		goto dontChgV;
        		}
          else
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#elif MC68000
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
  				  i=OldSize;
				    PROCOper(LINE_TYPE_ISTRUZIONE,"lsl.l",OPDEF_MODE_IMMEDIATO,log(i)/log(2),OPDEF_MODE_REGISTRO,Regs->D);
//						  PROCOper(LINE_TYPE_ISTRUZIONE,"sla",Dr,NULL);
						}
          if(VQ==3 && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
       		  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO32,1);
//        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Regs[VVar],Dr);
        		goto dontChgV;
        		}
          else
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO,Regs->D+1);
#elif MICROCHIP
											      
          if(VQ==3 && VVar->classe == 4) {    // ERRORE! al massimo è OK per +=
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,*(int *)VVar->label,OPDEF_MODE_REGISTRO,Regs->D);
        		goto dontChgV;
        		}
          else {
	          if(Regs->D==1) {
	        		if(*AS == 'S') {
	        		  *(AS+1)='U';
	        		  *(AS+2)='B';
	          		PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
	        		  }
							if((*VType & 15) && !(RType & 15)) {
		  				  i=OldSize;
		  				  while(i--)
    	        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
								}
							else	
	        		  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
	        		}
	          else {
caseT0:	          
							if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
//							printf("\asommo ptr %d %d, %d %d",u[0].mode,u[0].s.n,u[1].mode,u[1].s.n);
		  				  i=OldSize;
		  				  u[2]=u[0];
		  				  u[3]=u[1];
		  				  while(i--) {
			  				  u[0]=u[2];
			  				  u[1]=u[3];
									Op2A(AS,&u[0],&u[1]);
									}
								}
							else	
							  Op2A(AS,&u[0],&u[1]);
	         		}
	         	}	
#endif
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
      		i=VCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
				  Op2A(AS,i,&u[0],m1);
#elif I8086
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
				  Op2A(AS,i,&u[0],m1);
#endif
      		break;
    		case 2:
      		i=RCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
				  Op2A(AS,&u[0],i,m1);
#elif I8086
myAdd:      		
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MC68000
myAdd:      		
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO16,0);
#elif MICROCHIP
				  Op2A(AS,&u[0],i,m1);
#endif
      		break;
    		}
      break;
    case 4:
  		switch(T) {
        case -1:
    		case 0:
#if ARCHI
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
#elif Z80
					OpA(AS,&u[0],&u[2]);
      		if(*AS == 'a')
      		  *(AS+2)='c';
      		else {
      		  *(AS+1)='b';
      		  *(AS+2)='c';
      		  }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
					OpA(AS,&u[0],&u[2]);
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					// se la variabile 2 da sommare o cmq usare era short e la dest è int, fa casino! problema di cast direi... 2025
					OpA(AS,&u[1],&u[3]);
			    IncOp(&u[1]);
			    IncOp(&u[3]);
					OpA(AS,&u[1],&u[3]);
#elif I8086
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
      		if(*AS == 'a')
      		  *(AS+2)='c';
      		else {
      		  *(AS+1)='b';
      		  *(AS+2)='b';
      		  }
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
#elif MC68000
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,&u2->s,0,OPDEF_MODE_REGISTRO32,&u->s,0);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,/*Regs->D,u[1].mode,&u[1].s,u[1].ofs,*/Regs->D,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
					OpA(AS,&u[0],&u[2]);
      		if(*AS == 'a')
      		  *(AS+2)='c';
      		else {
      		  *(AS+1)='b';
      		  *(AS+2)='c';
      		  }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
					OpA(AS,&u[0],&u[2]);
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					OpA(AS,&u[1],&u[3]);
			    IncOp(&u[1]);
			    IncOp(&u[3]);
					OpA(AS,&u[1],&u[3]);
#endif
      		break;
    		case 1:                                
      		l=VCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
      		i=(int)l;
      		if(i & 0xff) {
						OpA(AS,i,&u[0]);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,i >> 8,&u[0]);
					if(u[1].mode & CONDIZ_MINORE)
				    IncOp(&u[1]);
					OpA(AS,*(((char *)&l)+2),&u[1]);
			    IncOp(&u[1]);
					OpA(AS,*(((char *)&l)+3),&u[1]);
#elif I8086
      		i=(int)l;
      		if(i) {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,i);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='b';
        		  }
        		}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D+1,l >> 16);
#elif MC68000
      		i=(int)l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
      		i=(int)l;
      		if(i & 0xff) {
						OpA(AS,i,&u[0]);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,i >> 8,&u[0]);
					if(u[1].mode & CONDIZ_MINORE)
				    IncOp(&u[1]);
					OpA(AS,*(((char *)&l)+2),&u[1]);
			    IncOp(&u[1]);
					OpA(AS,*(((char *)&l)+3),&u[1]);
#endif
      		break;
    		case 2:
      		l=RCost->l;
#if ARCHI
      		*MyBuf='#';
      		_tcscpy(MyBuf+1,VCost->s);
      		PROCOper(AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,MyBuf,T1S);
#elif Z80
      		i=(int)l;
      		if(i & 0xff) {
						OpA(AS,&u[0],i);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,&u[0],i >> 8);
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					OpA(AS,&u[1],*(((char *)&l)+2));
			    IncOp(&u[1]);
					OpA(AS,&u[1],*(((char *)&l)+3));
#elif I8086
      		i=(int)l;
      		if(i) {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,0,i);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='b';
        		  }
        		}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,0,l >> 16);
#elif MC68000
      		i=(int)l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,l,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
      		i=(int)l;
      		if(i & 0xff) {
						OpA(AS,&u[0],i);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,&u[0],i >> 8);
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					OpA(AS,&u[1],*(((char *)&l)+2));
			    IncOp(&u[1]);
					OpA(AS,&u[1],*(((char *)&l)+3));
#endif
      		break;
    		}
      break;
    }  
  VQ=1;
dontChgV:  
  *VSize=OldSize;
  if(!m) {                                    // sottraggo ptr e diventa int...
    if((RType & VARTYPE_IS_POINTER) && (*VType & VARTYPE_IS_POINTER)) {
      *VType=VARTYPE_PLAIN_INT;
      *VSize=PTR_SIZE;
      }
    }
  return VQ;
  }
  
int Ccc::subMul(char m, int T, int VQ, struct VARS *VVar, long VType, uint8_t VSize, int RQ, long RType, uint8_t RSize, union STR_LONG *VCost, union STR_LONG *RCost, struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3) {
  char myBuf[16],*AS;
  int i;
  struct VARS *v;
             
  if(m == '*') {
#if ARCHI
    PROCOper(LINE_TYPE_ISTRUZIONE,"MUL",Dr,Dr1,Dr,NULL);
#elif Z80
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(T) {
		      case 0:
		        break;  
		      case 1:
		        break;  
		      case 2:
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost);
		        if(!i)
		          Regs->Dec(2);
		        break;  
		      }
        if(Regs->D>1) {
          Regs->Save();
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
          if(VSize>1) {
				    IncOp(&u[0]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
				    DecOp(&u[0]);
            }
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,1,u[1].mode,&u[1].s,u[1].ofs);
          if(RSize>1) {
				    IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,u[1].mode,&u[1].s,u[1].ofs);
				    DecOp(&u[1]);
            }
          }
        if(VSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_IMMEDIATO8,0);
        if(RSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,OPDEF_MODE_IMMEDIATO8,0);
        PROCOper(LINE_TYPE_ISTRUZIONE,"rst",OPDEF_MODE_IMMEDIATO,0x28);
        if(Regs->D>1) {
				  PROCOper(LINE_TYPE_ISTRUZIONE,"exeeee",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,3,0);//SISTEMARE
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_FRAMEPOINTER,0);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//			    IncOp(&u[0]);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,0,0);
          Regs->Get();
          }
        break;
		  case 4:
		    switch(T) {
		      case 0:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;
		      case 1:
		        break;  
		      case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_lmul",0);
  	    if(!v)
    		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
        if(Regs->D>0) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_HIGH8,0);
			    IncOp(&u[0]);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_LOW8,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_HIGH8,0);
			    IncOp(&u[1]);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,0);
          Regs->Get();
          }
        break;
      }
#elif I8086
  AS="mul";
	switch(VSize) {
	  case 1:
  		switch(T) {
        case -1:
    		case 0:
         	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,NULL);
      		break;
    		case 1:                                
    		  i=VCost->l;
myMul1:    		  
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		case 2:
    		  i=RCost->l;
    		  goto myMul1;
      		break;
    		}
      break;
    case 2:
  		switch(T) {
        case -1:
    		case 0:
         	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,0);
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
myMul2:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		case 2:
    		  i=RCost->l;
					goto myMul2;
      		break;
    		}
      break;
    case 4:
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+3);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+2);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
      v=FNCercaVar("_lmul",0);
 	    if(!v)
   		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
      for(i=0; i<3; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
      if(Regs->D>0) {
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->DS,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D1S,"dx");
        Regs->Get();
        }
      break;
    }  
#elif MC68000
  AS=VType & VARTYPE_UNSIGNED ? "mulu" : "muls";
	switch(VSize) {
	  case 1:
  		switch(T) {
        case -1:
    		case 0:
//			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,3,u[0].mode,&u[0].s,u[0].ofs);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D    -1,OPDEF_MODE_REGISTRO,Regs->D);
      		break;
    		case 1:                                
    		  i=VCost->l;
myMul1:    		  
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D    -1,OPDEF_MODE_REGISTRO,Regs->D);
      		break;
    		case 2:
    		  i=RCost->l;
    		  goto myMul1;
      		break;
    		}
      break;
    case 2:
  		switch(T) {
        case -1:
    		case 0:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D    -1,OPDEF_MODE_REGISTRO16,Regs->D);
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
myMul2:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,i,OPDEF_MODE_REGISTRO16,Regs->D);
      		break;
    		case 2:
    		  i=RCost->l;
					goto myMul2;
      		break;
    		}
      break;
    case 4:
  		switch(T) {
        case -1:
    		case 0:
					if(VQ==VALUE_IS_COSTANTE) {			// VERIFICARE!! co'è T esattamente? non indica costante...
	    		  i=VCost->l;
		  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						}
					else {
	  				PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D   ,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	  				PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D   -1,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						}
myMul4_:
					v=FNCercaVar("_lmul",0);
 					if(!v)
   					v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     			PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"addq"/*pushString*/,OPDEF_MODE_IMMEDIATO,8,OPDEF_MODE_STACKPOINTER,0);
					if(Regs->D>0) {
		//boh qua				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
		//        Regs->Get();
						}
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
myMul4:
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
					goto myMul4_;
      		break;
    		case 2:
    		  i=RCost->l;
					goto myMul4;
      		break;
    		}
      break;
    case 8:		// boh nel caso!
// finire
      break;
    }  
#elif MICROCHIP
		switch(VSize) {
		  case 1:
			  AS="MULLW";
  			switch(T) {
					case -1:
    			case 0:
         		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,NULL);
      			break;
    			case 1:                                
    				i=VCost->l;
myMul1:    		  
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      			break;
    			case 2:
    				i=RCost->l;
    				goto myMul1;
      			break;
    			}
				break;
		  case 2:
		    switch(T) {
		      case 0:
		        break;  
		      case 1:
		        break;  
		      case 2:
		        i=Regs->Inc(2);
   	        PROCUseCost(RQ,RType,RSize,RCost,0);		// FINIRE
		        if(!i)
		          Regs->Dec(2);
		        break;  
		      }
        if(Regs->D>1) {
          Regs->Save();
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
          if(VSize>1) {
				    IncOp(&u[0]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
				    DecOp(&u[0]);
            }
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,1,u[1].mode,&u[1].s,u[1].ofs);
          if(RSize>1) {
				    IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,u[1].mode,&u[1].s,u[1].ofs);
				    DecOp(&u[1]);
            }
          }
        if(VSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_IMMEDIATO8,0);
        if(RSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,OPDEF_MODE_IMMEDIATO8,0);
        PROCOper(LINE_TYPE_ISTRUZIONE,"rst",OPDEF_MODE_IMMEDIATO,0x28);
        if(Regs->D>1) {
				  PROCOper(LINE_TYPE_ISTRUZIONE,"exrrrrrrrr",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,3,0);//SISTEMARE!
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_FRAMEPOINTER,0);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//			    IncOp(&u[0]);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,0,0);
          Regs->Get();
          }
        break;
		  case 4:
		    switch(T) {
		      case 0:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;
		      case 1:
		        break;  
		      case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,0);		// FINIRE
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_lmul",0);
  	    if(!v)
    		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
        if(Regs->D>0) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_HIGH8,0);
			    IncOp(&u[0]);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_LOW8,0);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_HIGH8,0);
			    IncOp(&u[1]);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,0);
          Regs->Get();
          }
        break;
      }
#endif
    }
  else {		// 
	// OTTIMIZZARE se divisore multiplo di 2 ?? qua o in ottimizzazioni?
#if ARCHI
    if(Regs->D>0) {
      Regs->Save();
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV R0, ",Dr,NULL);
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV R1,",Dr1,NULL);
      }
	  v=FNCercaVar("__IntDiv",0);
  	if(!v)
    	v=PROCAllocVar("__IntDiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
    PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
//    PROCOper(LINE_TYPE_CALL,"BL __IntDiv",NULL,NULL);
#elif Z80
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(T) {
		      case 0:
		        break;  
		      case 1:
		        break;  
		      case 2:
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost);
		        if(!i)
		          Regs->Dec(2);
		        break;  
		      }
        if(Regs->D>1) {
          Regs->Save();
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
          if(VSize>1) {
  			    IncOp(&u[0]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
            }
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,1,u[1].mode,&u[1].s,u[1].ofs);
          if(RSize>1) {
				    IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,u[1].mode,&u[1].s,u[1].ofs);
				    }
          }
        if(VSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_IMMEDIATO8,0);
        if(RSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,OPDEF_MODE_IMMEDIATO8,0);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,VType & VARTYPE_UNSIGNED ? CONDIZ_MINORE : 0);
        PROCOper(LINE_TYPE_ISTRUZIONE,"rst",OPDEF_MODE_IMMEDIATO,0x30);		// routine divisione, SPECIFICA per SKYNET!
        break;
		  case 4:
		    switch(T) {
		      case 0:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;
		      case 1:
		        break;  
		      case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_ldiv",0);
  	    if(!v)
    		  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
				if(Regs->D>0) {
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"ax");
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"dx");
					Regs->Get();
					}
        break;
      }
#elif I8086
  AS="div";
	switch(VSize) {
	  case 1:
  		switch(T) {
        case -1:
    		case 0:
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
//					PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr1);
      		break;
    		case 1:
    		  i=VCost->l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		case 2:
    		  i=RCost->l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		}
      break;
    case 2:
  		switch(T) {
        case -1:
    		case 0:
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
//         	PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr1);
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		case 2:
    		  i=RCost->l;
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
      		break;
    		}
      break;
    case 4:
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,Regs->D,OPDEF_MODE_REGISTRO);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,Regs->D+1,OPDEF_MODE_REGISTRO,NULL);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,Regs->D+2,OPDEF_MODE_REGISTRO,NULL);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,Regs->D+3,OPDEF_MODE_REGISTRO,NULL);
      v=FNCercaVar("_ldiv",0);
      if(!v)
    	  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
      UseLMul=TRUE;
      for(i=0; i<4; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
      if(Regs->D>0) {
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D+1,"dx");
        Regs->Get();
        }
      break;
    }  
#elif MC68000
  AS=VType & VARTYPE_UNSIGNED ? "divu" : "divs";

	// OTTIMIZZARE se divisore multiplo di 2
	if(RSize==1)
		PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);

	switch(VSize) {
	  case 1:
  		switch(T) {
        case -1:
    		case 0:
					PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,3,u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		case 1:
    		  i=VCost->l;
myDiv1:    		  
					PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D   -1,OPDEF_MODE_REGISTRO,Regs->D);
      		break;
    		case 2:
    		  i=RCost->l;
					goto myDiv1;
      		break;
    		}
      break;
    case 2:
  		switch(T) {
        case -1:
    		case 0:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D    -1,OPDEF_MODE_REGISTRO16,Regs->D);
//         	PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr1);
      		break;
    		case 1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
myDiv2:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,i,OPDEF_MODE_REGISTRO16,Regs->D);
      		break;
    		case 2:
    		  i=RCost->l;
					goto myDiv2;
      		break;
    		}
      break;
    case 4:
  		switch(T) {
        case -1:
    		case 0:
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D   ,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D   -1,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
myDiv4_:
					v=FNCercaVar("_div",0);
 					if(!v)
   					v=PROCAllocVar("_div",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     			PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"addq"/*pushString*/,OPDEF_MODE_IMMEDIATO,8,OPDEF_MODE_STACKPOINTER,0);
					if(Regs->D>0) {
		//boh qua				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
		//        Regs->Get();
						}
      		break;
    		case 1:
    		  i=VCost->l;
myDiv4:
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
					goto myDiv4_;
      		break;
    		case 2:
    		  i=RCost->l;
					goto myDiv4;
      		break;
    		}
      break;
    case 8:		// boh nel caso!
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,0,u[0].mode,&u[0].s,u[0].ofs);
// finire

      v=FNCercaVar("_ldiv",0);
      if(!v)
    	  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
      UseLMul=TRUE;
      for(i=0; i<4; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
      if(Regs->D>0) {
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D+1,"dx");
        Regs->Get();
        }
			break;
    }  
#elif MICROCHIP
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(T) {
		      case 0:
		        break;  
		      case 1:
		        break;  
		      case 2:
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost,0);		// FINIRE
		        if(!i)
		          Regs->Dec(2);
		        break;  
		      }
        if(Regs->D>1) {
          Regs->Save();
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
          if(VSize>1) {
  			    IncOp(&u[0]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
            }
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,1,u[1].mode,&u[1].s,u[1].ofs);
          if(RSize>1) {
				    IncOp(&u[1]);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,u[1].mode,&u[1].s,u[1].ofs);
				    }
          }
        if(VSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_IMMEDIATO8,0);
        if(RSize==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,1,OPDEF_MODE_IMMEDIATO8,0);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,VType & VARTYPE_UNSIGNED ? CONDIZ_MINORE : 0);
        PROCOper(LINE_TYPE_ISTRUZIONE,"rst",OPDEF_MODE_IMMEDIATO,0x30);		// routine divisione, SPECIFICA per SKYNET!
        break;
		  case 4:
		    switch(T) {
		      case 0:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;
		      case 1:
		        break;  
		      case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,0);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_ldiv",0);
  	    if(!v)
    		  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
				if(Regs->D>0) {
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"ax");
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"dx");
					Regs->Get();
					}
        break;
      }
#endif
    if(m=='%') {
#if ARCHI
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
#elif Z80
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_HIGH8,1);
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#elif I8086
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#elif MC68000
      PROCOper(LINE_TYPE_ISTRUZIONE,"swap",OPDEF_MODE_REGISTRO32,Regs->D   -1);
#elif MICROCHIP
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_HIGH8,1);
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#endif
      }
    if(Regs->D>1) {
#if ARCHI
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",Dr,",R0");
      Regs->Get();
#elif Z80 
      if(VSize<4) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,"swap",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_REGISTRO,0);
				// ehm ;) 2025
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//		    IncOp(&u[0]);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,2,0);
        Regs->Get();
        }
#elif I8086 
      if(VSize<4) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,"swap",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_REGISTRO,0);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//		    IncOp(&u[0]);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,2,0);
        Regs->Get();
        }
#elif MC68000
      if(VSize<4) {
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"swap",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_REGISTRO,0);
// qua non direi

//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//		    IncOp(&u[0]);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,2,0);
//        Regs->Get();
        }
#elif MICROCHIP
      if(VSize<4) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_REGISTRO,0);
// finire!

//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,1,0);
//		    IncOp(&u[0]);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,2,0);
        Regs->Get();
        }
#endif
      }
    }
  return 1;
  }

int Ccc::FNIs1Bit(int t) {
  register int i,j=0,k,i1;
  
  for(i=0x0001,i1=0; i; i <<= 1,i1++) {
    if(t & i) {
      j++;
      k=i1;
      }
    }
//    printf("1 bit: %d %d\n\a",j,k);
  if(j==1)
    return k+1;
  else
    return 0;   
  }

int Ccc::subAOX(char m, int *cond, int T, int VQ, struct VARS *VVar, long VType, int VSize, long RType, int RSize, union STR_LONG *VCost, union STR_LONG *RCost, struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3) {
  char AS[16],myBuf[16];
  int i,j;
  long l;

  if((RType & VARTYPE_IS_POINTER) || (VType & VARTYPE_IS_POINTER))
    PROCError(2111,NULL);     // messaggio da cambiare
  switch(m) {
    case '&':
#if ARCHI
      _tcscpy(AS,"AND");
#elif Z80 || I8086
      _tcscpy(AS,"and");
#elif MC68000
      _tcscpy(AS,"and");
#elif MICROCHIP
      _tcscpy(AS,"ANDLW");
#endif
      break;
    case '^':
#if ARCHI
      _tcscpy(AS,"EOR");
#elif Z80 || I8086
      _tcscpy(AS,"xor");
#elif MC68000
      _tcscpy(AS,"eor");
#elif MICROCHIP
      _tcscpy(AS,"XORLW");
#endif
      break;
    case '|':
#if ARCHI
      _tcscpy(AS,"ORR");
#elif Z80 || I8086
      _tcscpy(AS,"or");
#elif MC68000
      _tcscpy(AS,"or");
#elif MICROCHIP
      _tcscpy(AS,"IORLW");
#endif
      break;
    default: 
      break;
    }
  VQ=1;  
#if MC68000
	if(T==2)		// costante
		_tcscat(AS,"i");
	switch(VSize) {
		case 1:
			_tcscat(AS,".b");
			break;
		case 2:
			_tcscat(AS,".w");
			break;
		case 4:
			_tcscat(AS,".l");
			break;
		}
#endif
	switch(VSize) {
	  case 1:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,NULL);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
					if(*cond && m=='&')
            VQ=CONDIZ_DIVERSO;      //-15;
#elif I8086
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
//          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr1);
#elif MC68000
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,3,u[0].mode,&u[0].s,u[0].ofs);
#endif
          break;
        case 1:
          i=VCost->l;
myAOX1:          
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,MyBuf,OPDEF_MODE_REGISTRO,Dr1,NULL);
#elif Z80
					if(*cond && m=='&') {
					  if(j=FNIs1Bit(i)) {
//					    itoa(j-1,MyBuf,10);
              PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO8,j-1,u[0].mode,&u[0].s,u[0].ofs);
					    }
					  else {  
//              ReadVar(VVar,0,0,0);
//              OpA(AS,Dr,i);
              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
              PROCOper(LINE_TYPE_ISTRUZIONE,"and",OPDEF_MODE_IMMEDIATO8,i);
              }
            VQ=CONDIZ_DIVERSO;
            }
					else {
					  if(m=='|' && (j=FNIs1Bit(i))) {
//					    itoa(j-1,MyBuf,10);
              PROCOper(LINE_TYPE_ISTRUZIONE,"set",OPDEF_MODE_IMMEDIATO8,j-1,u[0].mode,&u[0].s,u[0].ofs);
					    }
					  else  
              OpA(AS,&u[0],i);
            }
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
#elif MICROCHIP
					if(*cond && m=='&') {
					  if(j=FNIs1Bit(i)) {
//					    itoa(j-1,MyBuf,10);
              PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSC\nGOTO",OPDEF_MODE_IMMEDIATO,j-1,u[0].mode,&u[0].s,u[0].ofs);
					    }
					  else {  
//              ReadVar(VVar,0,0,0);
//              OpA(AS,Dr,i);
              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
              PROCOper(LINE_TYPE_ISTRUZIONE,"ANDLW",OPDEF_MODE_IMMEDIATO8,i);
              }
            VQ=CONDIZ_DIVERSO;
            }
					else {
					  if(m=='|' && (j=FNIs1Bit(i))) {
//					    itoa(j-1,MyBuf,10);
              PROCOper(LINE_TYPE_ISTRUZIONE,"BSF",OPDEF_MODE_IMMEDIATO,j-1,u[0].mode,&u[0].s,u[0].ofs);
					    }
					  else  
              OpA(AS,&u[0],i);
            }
#endif
          break;
        case 2:
          i=RCost->l;
          goto myAOX1;
          break;                        
        default:
          break;
        }
      break;
	  case 2:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,NULL);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
			    IncOp(&u[0]);
			    IncOp(&u[1]);
          OpA(AS,&u[0],&u[1]);
#elif I8086
//          PROCOper(AS,Dr,Dr);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D    -1);
					// VERIFICARE -1
#endif
          break;
        case 1:
          j=VCost->l;
myAOX:    		                    
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,MyBuf,OPDEF_MODE_REGISTRO,Dr1,NULL);
#elif Z80
				  if(m=='&') {
				    if(i=FNIs1Bit(~j)) {
					    if(i<=8) {
//  					    itoa(i-1,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"res",OPDEF_MODE_IMMEDIATO8,i-1,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            else {  
//  					    itoa(i-9,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"res",OPDEF_MODE_IMMEDIATO8,i-9,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            }
	          else {
		          i=j & 0xff;
		          if(i != 0xff)
		            OpA(AS,&u[0],i);
					    IncOp(&u[0]);
		          i=(j >> 8) & 0xff;
		          if(i != 0xff)
		            OpA(AS,&u[0],i);
		          }  
  			    }
				  else {
				    if(m=='|' && (i=FNIs1Bit(j))) {
					    if(i<=8) {
//  					    itoa(i-1,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"set",OPDEF_MODE_IMMEDIATO8,i-1,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            else {  
//  					    itoa(i-9,MyBuf,10);
						    IncOp(&u[0]);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"set",OPDEF_MODE_IMMEDIATO8,i-9,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            }
	          else {
		          i=j & 0xff;
		          if(i)
		            OpA(AS,&u[0],i);
					    IncOp(&u[0]);
		          i=j >> 8;
		          if(i)
		            OpA(AS,&u[0],i);
		          }  
	          }  
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,j);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,j,OPDEF_MODE_REGISTRO16,Regs->D);
#elif MICROCHIP
				  if(m=='&') {
				    if(i=FNIs1Bit(~j)) {
					    if(i<=8) {
//  					    itoa(i-1,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"BCF",OPDEF_MODE_IMMEDIATO,i-1,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            else {  
//  					    itoa(i-9,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"BCF",OPDEF_MODE_IMMEDIATO,i-9,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            }
	          else {
		          i=j & 0xff;
		          if(i != 0xff)
		            OpA(AS,&u[0],i);
					    IncOp(&u[0]);
		          i=(j >> 8) & 0xff;
		          if(i != 0xff)
		            OpA(AS,&u[0],i);
		          }  
  			    }
				  else {
				    if(m=='|' && (i=FNIs1Bit(j))) {
					    if(i<=8) {
//  					    itoa(i-1,MyBuf,10);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"BSF",OPDEF_MODE_IMMEDIATO,i-1,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            else {  
//  					    itoa(i-9,MyBuf,10);
						    IncOp(&u[0]);
	              PROCOper(LINE_TYPE_ISTRUZIONE,"BSF",OPDEF_MODE_IMMEDIATO,i-9,u[0].mode,&u[0].s,u[0].ofs);
	              }
	            }
	          else {
		          i=j & 0xff;
		          if(i)
		            OpA(AS,&u[0],i);
					    IncOp(&u[0]);
		          i=j >> 8;
		          if(i)
		            OpA(AS,&u[0],i);
		          }  
	          }  
#endif
          break;
        case 2:
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,RCost->s);
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr,MyBuf,NULL);
#elif Z80 || I8086 || MC68000 || MICROCHIP
          j=RCost->l;
          goto myAOX;
#endif
          break;                        
        default:
          break;
        }
      break;
	  case 4:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr,OPDEF_MODE_REGISTRO,Dr1,NULL);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
			    IncOp(&u[0]);
			    IncOp(&u[1]);
          OpA(AS,&u[0],&u[1]);
#elif I8086
//          PROCOper(AS,Dr,Dr2);
//          PROCOper(AS,Dr1,Dr3);
#elif MC68000
//          PROCOper(AS,Dr,Dr2);
//          PROCOper(AS,Dr1,Dr3);
#endif
          break;
        case 1:
          l=VCost->l;
myAOXl:    		                    
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Dr,MyBuf,OPDEF_MODE_REGISTRO,Dr1,NULL);
#elif Z80 || MICROCHIP
          i=l & 0xff;
          if((i!=0xff && m=='&') || (i && m!='&')) {
            OpA(AS,&u[0],i);
            }
			    IncOp(&u[0]);
          i=*(((char *)&l)+1) & 0xff;
          if((i!=0xff && m=='&') || (i && m!='&')) {
            OpA(AS,&u[0],i);
            }
          i=*(((char *)&l)+2) & 0xff;
          if((i!=0xff && m=='&') || (i && m!='&')) {
            OpA(AS,&u[1],i);
            }
			    IncOp(&u[1]);
          i=*(((char *)&l)+3) & 0xff;
          if((i!=0xff && m=='&') || (i && m!='&')) {
            OpA(AS,&u[1],i);
            }
#elif I8086
          i=l & 0xffff;
          if((i!=0xffff && m=='&') || (i && m!='&')) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
            }
          i=(l >> 16) & 0xffff;
          if((i!=0xffff && m=='&') || (i && m!='&')) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
            }
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,l,OPDEF_MODE_REGISTRO,Regs->D    -1);
#endif
          break;
        case 2:
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,RCost->s);
	        PROCOper(AS,Dr,Dr,MyBuf,NULL);
#elif Z80 || I8086 || MC68000 || MICROCHIP
          l=RCost->l;
          goto myAOXl;
#endif
          break;                        
        default:
          break;
        }
	    break;
    }
  return VQ;
  }


enum Ccc::OPERANDO_CONDIZIONALE Ccc::subCMP(const char *TS, int cond, int T, int VQ, struct VARS *VVar, long VType, uint8_t VSize, long RType, uint8_t RSize, union STR_LONG *VCost, union STR_LONG *RCost, 
																			 struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3) {
  char AS[16],myBuf[16];
  int i;
	enum OPERANDO_CONDIZIONALE RQ;
  long l;
               
  switch(*TS) {
    case '<':
			if(VType & VARTYPE_UNSIGNED) {		// FARE! 2025
				if(*(TS+1) == '=')
					RQ=CONDIZ_MINORE_UGUALE;   //-13;
				else
					RQ=CONDIZ_MINORE; 	//-10;
				}
			else {
				if(*(TS+1) == '=')
					RQ=CONDIZ_MINORE_UGUALE;   //-13;
				else
					RQ=CONDIZ_MINORE; 	//-10;
				}
      break;
    case '>':
			if(VType & VARTYPE_UNSIGNED) {		// FARE! 2025
				if(*(TS+1) == '=')
					RQ=CONDIZ_MAGGIORE_UGUALE;   //-11;
				else
					RQ=CONDIZ_MAGGIORE;   //-12;
				}
			else {
				if(*(TS+1) == '=')
					RQ=CONDIZ_MAGGIORE_UGUALE;   //-11;
				else
					RQ=CONDIZ_MAGGIORE;   //-12;
				}
      break;
    case '=': 
      RQ=CONDIZ_UGUALE;     //-14;
      break;
    case '!':
      RQ=CONDIZ_DIVERSO;     //-15;
      break;
    default:
      break;
    }
  VSize=FNGetMemSize(VType,VSize,0);
#if ARCHI
      _tcscpy(AS,"CMP");
#elif Z80 || I8086
      _tcscpy(AS,"cmp");
#elif MC68000
      _tcscpy(AS,"cmp");
#elif MICROCHIP
      _tcscpy(AS,"XORLW");		// cmq. v.
#endif
#if MC68000
	if(T==2)		// costante
		_tcscat(AS,"i");
	switch(VSize) {
		case 1:
			_tcscat(AS,".b");
			break;
		case 2:
			_tcscat(AS,".w");
			break;
		case 4:
			_tcscat(AS,".l");
			break;
		}
#endif
	switch(VSize) {
	  case 1:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].D,OPDEF_MODE_REGISTRO,Regs[1].D);
#elif Z80
          OpA("cp",&u[0],&u[1]);
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D);
#endif
          break;
        case 1:
          i=VCost->l;
#if ARCHI
          if(i>=0) {            
            *MyBuf='#';
            _tcscpy(MyBuf+1,VCost->s);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].D,MyBuf);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].D,-VCost->l);
  		      }
#elif Z80
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("cp",i,&u[0]);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,0,i);
#elif MICROCHIP
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("XORLW",i,&u[0]);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#endif
          break;
        case 2:
          i=RCost->l;
#if ARCHI
          if(i>=0) {            
            *MyBuf='#';
            _tcscpy(MyBuf+1,VCost->s);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].D,MyBuf);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].D,-VCost->l);
  		      }
#elif Z80
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("cp",&u[0],i);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_HIGH8,3);
#elif MC68000
//          PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.w",OPDEF_MODE_REGISTRO_HIGH8,3);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO,Regs->D    -1);
#elif MICROCHIP
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("XORLW",&u[0],i);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#endif
          break;
        default:
          break;
        }
      break;
	  case 2:
      switch(T) {
        case -1:
        case 0:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,OPDEF_MODE_REGISTRO,Regs[1].Dr);
#elif Z80
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(RQ & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[1].mode,&u[1].s,u[1].ofs);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	}
#elif I8086
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",Regs->D,Dr);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[1].mode,&u[1].s,u[1].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(RQ & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[1].mode,&u[1].s,u[1].ofs);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[1].mode,&u[1].s,u[1].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	}
#endif
          break;
        case 1:
          i=VCost->l;
#if ARCHI
          if(i>=0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,VCost->l);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].Dr,-VCost->l);
  		      }
#elif Z80
					if(!i) {
					  if(RQ >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) | 4);		// RQ= RQ + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[0].mode,&u[0].s,u[0].ofs);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i >> 8);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOutLab(myBuf);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    }
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif I8086
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif MC68000
//      		PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.b",OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO_HIGH8,3);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D    -1);
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif MICROCHIP
					if(!i) {
					  if(RQ >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) | 4);		// RQ= RQ + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[0].mode,&u[0].s,u[0].ofs);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[0].mode,&u[0].s,u[0].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i >> 8);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[0].mode,&u[0].s,u[0].ofs);
					    PROCOutLab(myBuf);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    }
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#endif
          break;
        case 2:
          i=RCost->l;
#if ARCHI
          if(i>=0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,VCost->l);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].Dr,-VCost->l);
  		      }
#elif Z80
					if(!i) {
					  if(RQ >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ^ 1) | 4);		//  RQ= (RQ ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,i);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,i >> 8);
					    PROCOutLab(myBuf);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i >> 8);
				    	}
				    }
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif I8086
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif MC68000
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO16,0);
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#elif MICROCHIP
					if(!i) {
					  if(RQ >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ^1) | 4);//						  RQ= (RQ ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,i);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,i);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,i >> 8);
					    PROCOutLab(myBuf);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i >> 8);
				    	}
				    }
          /*
          PROCOper(movString,Dr1,itoa(i,MyBuf,10));
          PROCOper("or",Regs->Accu,NULL);
          PROCOper("sbc",Dr,Dr1);
          */
#endif
          break;
        default:
          break;
        }
      break;
	  case 4:
#if ARCHI
      switch(T) {
        case -1:
        case 0:
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,Regs[1].Dr);
          break;
        case 1:
          i=VCost->l;
          if(i>0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,VCost->l);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].Dr,RCost.l);
  		      }
          break;
        case 2:
          if(RCost.l>0) {
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs[0].Dr,RCost.l);
            }
          else {
        		PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO,Regs[0].Dr,-RCost.l);
            }
          break;
        default:
          break;
        }
      break;
#elif Z80
      switch(T) {
        case -1:
        case 0:
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(RQ & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
          break;
        case 1:
          l=VCost->l;
myCPl:		                        
					if(!l) {
					  if(RQ >= CONDIZ_UGUALE) {
				    	RQ = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+3);
				    	RQ = (enum OPERANDO_CONDIZIONALE)(((int)RQ ) | 4);		// RQ= RQ + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[2].mode,&u[2].s,u[2].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,l & 0xff);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
				    	}
				    else {	
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO8,l & 0xff);
					    }
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    }
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    }
				    IncOp(&u[1]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    }
				    }
          break;
        case 2:
          l=RCost->l;
          goto myCPl;
          break;
        default:
          break;
        }
      break;
#elif I8086
      switch(T) {
        case -1:
        case 0:
				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",Regs->D,Dr);
//			    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",Regs->D1S,Dr2);
          break;
        case 1:
          l=VCost->l;
myCPl:		                        
			    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*((WORD *)&l));
			    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((WORD *)&l)+2));
//				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",Regs->D,(int)l);
//			    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",Regs->D1S,*((int *)(&l)+1));
          break;
        case 2:
          l=RCost->l;
          goto myCPl;
          break;
        default:
          break;
        }
      break;
#elif MC68000
      switch(T) {
        case -1:
        case 0:
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,3,u[0].mode,&u[0].s,u[0].ofs);
          break;
        case 1:
          l=VCost->l;
myCPl:		                        
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,3,OPDEF_MODE_IMMEDIATO32,l);
          break;
        case 2:
          l=RCost->l;
          goto myCPl;
          break;
        default:
          break;
        }
      break;
#elif MICROCHIP
      switch(T) {
        case -1:
        case 0:
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(RQ & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[1].mode,&u[1].s,u[1].ofs);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(RQ & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
          break;
        case 1:
          l=VCost->l;
myCPl:		                        
					if(!l) {
					  if(RQ >= CONDIZ_UGUALE) {
				    	RQ = (enum OPERANDO_CONDIZIONALE )1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
					  else if(RQ==CONDIZ_MINORE || RQ==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+3);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ^1) | 4);//						  RQ= (RQ ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO8,0);
					    }  
					  }
					else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[2].mode,&u[2].s,u[2].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(RQ & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,l & 0xff);
				    	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) ^2);
				    	}
				    else {	
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,l & 0xff);
					    }
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,*(((char *)&l)+1));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    }
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,*(((char *)&l)+2));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    }
				    IncOp(&u[1]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(RQ & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    }
				    }
          break;
        case 2:
          l=RCost->l;
          goto myCPl;
          break;
        default:
          break;
        }
      break;
#endif
    }
  if(VType & VARTYPE_UNSIGNED)
   	RQ = (enum OPERANDO_CONDIZIONALE )(((int)RQ ) | 0x20);
//    RQ |= 0x20;  
  return RQ;
  }

