#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

//extern struct VARS GVPtr,GRPtr;

#if Z80
void IncOp(struct OP_DEF *);
void DecOp(struct OP_DEF *);
#endif

int Ccc::subShift(uint8_t m, int Mode, int8_t VQ, struct VARS *VVar, O_TYPE VType, O_SIZE VSize, O_TYPE RType, union STR_LONG *VCost, union STR_LONG *RCost, 
									struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3,bool bAutoAssign) {
  char AS[16],BS[16],MyBuf[16],MyBuf1[16];
  int i;

  if((RType & VARTYPE_IS_POINTER) || (VType & VARTYPE_IS_POINTER))
    PROCError(2111,NULL);    // messaggio da cambiare...
  if(VType & VARTYPE_FLOAT)
    PROCError(2297,NULL);    // 
  if(RType & VARTYPE_FLOAT) {
    PROCWarn(2297,NULL);    // convertire a int... lo faccio da fuori!

		}
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
#if I8086
	if(bAutoAssign) {		// SERVE flag per non far aggiungere queste in PROCObj
		switch(VSize) {
			case 1:
				_tcscat(AS," BYTE PTR ");
				break;
			case 2:
				_tcscat(AS," WORD PTR ");
				break;
			case 4:
				_tcscat(AS," DWORD PTR ");
				break;
			}
		}
#endif
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
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
          _tcscpy(MyBuf,AS);
//          _tcscat(MyBuf,Dr1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D,
						OPDEF_MODE_SHIFTR,Regs->D);
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
//          if(CPU86<1) {

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
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_REGISTRO8,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_REGISTRO8,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
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
        case MODE_IS_CONSTANT1:                        
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
#if ARCHI
          *MyBuf='#';
          _tcscpy(MyBuf+1,VCost->s);
          _tcscpy(MyBuf1,AS);
          _tcscat(MyBuf1," ");
//          _tcscat(MyBuf1,Dr);
          _tcscpy(MyBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_COSTANTE,VCost->l,OPDEF_MODE_SHIFTR,Regs->D);
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
						if(CPU86<1) {
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
					if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(VCost->l))) {		// :)
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),
							OPDEF_MODE_REGISTRO8,Regs->D+1);
						break; 
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
					PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),OPDEF_MODE_REGISTRO8,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
					VQ=VALUE_IS_EXPR;
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
        case MODE_IS_CONSTANT2:                    
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
#if ARCHI
//      		sprintf(MyBuf,"%s #%s",AS,RCost);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_COSTANTE,RCost->l,OPDEF_MODE_SHIFT,RCost->l);			// verificare tutto...
//      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
#elif Z80 || I8086
          i=RCost->l;
          goto myVShift1;
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							// (in teoria si potrebbe ottimizzare 1
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,Regs->D);
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO8,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO,Regs->D);
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_REGISTRO8,Regs->D);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_REGISTRO8,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO8,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
										}
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0) {		// 0... non so bene perché, se espressione forse
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						else {
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						VQ=VALUE_IS_EXPR;
						}
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
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
          _tcscpy(MyBuf,AS);
//          _tcscat(MyBuf,Dr1);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_SHIFTR,m ? Regs->D : (-Regs->D));			// finire....

#elif Z80
		      u[1]=u[0];         // preparo il byte alto in u[1] che non è usato
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
          if(CPU86<1) {
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,3,u[1].mode,&u[1].s,u[1].ofs);   //
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_LOW8,3);
	          }
	        else {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
	          }  
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
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
        case MODE_IS_CONSTANT1:                        
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
#if ARCHI
//          *MyBuf='#';
//          _tcscpy(MyBuf+1,VCost->s);
//          _tcscpy(MyBuf1,AS);
//          _tcscat(MyBuf1," ");
//          _tcscat(MyBuf1,Dr);
//          _tcscpy(MyBuf+1,VCost->s);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,MyBuf,NULL,MyBuf1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_SHIFT,VCost->l);
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
          if(CPU86<1) {
						if(i==1)
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,i);
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,3,OPDEF_MODE_IMMEDIATO8,i);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO_LOW8,3);
							}
            }
          else {  
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,i);
            }
#elif MC68000
					if(Optimize & OPTIMIZE_CONST && !LOWORD(VCost->l)) {
						PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D+1);
						break;
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
					if(VCost->l <= 255)
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOWORD(VCost->l),OPDEF_MODE_REGISTRO16,Regs->D);
					else if(!HIWORD(VCost->l))
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),OPDEF_MODE_REGISTRO16,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
					VQ=VALUE_IS_EXPR;
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
        case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
#if ARCHI
//      		sprintf(MyBuf,"%s#%s",AS,RCost);
//      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_SHIFT,RCost->l);
#elif Z80 || I8086
          i=RCost->l;
          goto myVShift;
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							// (in teoria si potrebbe ottimizzare 1
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO16,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									if(LOBYTE(LOWORD(RCost->l)) == 1)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0);
									else {
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO16,Regs->D);
										if(LOBYTE(LOWORD(RCost->l)) <= 8)
											PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
												OPDEF_MODE_REGISTRO8,Regs->D+1);
											PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
											}
										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
										}
									break;
								case CLASSE_AUTO:
									if(LOBYTE(LOWORD(RCost->l)) == 1)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									else {
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO16,Regs->D);
										if(LOBYTE(LOWORD(RCost->l)) <= 8)
											PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
												OPDEF_MODE_REGISTRO16,Regs->D);
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
												OPDEF_MODE_REGISTRO16,Regs->D+1);
											PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
											}
										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
										}
									break;
								case CLASSE_REGISTER:
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO16,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
										}
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0) {		// 0... non so bene perché, se espressione forse
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO16,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						else {
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						VQ=VALUE_IS_EXPR;
						}
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
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
          _tcscpy(MyBuf,AS);
//          _tcscat(MyBuf,Dr1);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_SHIFTR,Regs->D+1);
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
          if(CPU86<1) {
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
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D+1);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
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
        case MODE_IS_CONSTANT1:                        
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
#if ARCHI
//          *MyBuf='#';
//          _tcscpy(MyBuf+1,VCost->s);
//          _tcscpy(MyBuf1,AS);
//          _tcscat(MyBuf1," ");
//          _tcscat(MyBuf1,Dr);
//          _tcscpy(MyBuf+1,VCost->s);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,MyBuf,NULL,MyBuf1);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_IMMEDIATO16,VCost->l,OPDEF_MODE_SHIFTR,Regs->D);
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
          if(CPU86<1) {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[0],&u[2]);
//            PROCOper(LINE_TYPE_ISTRUZIONE,movString,"cl",i);
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[3]);
				    PROCOper(LINE_TYPE_ISTRUZIONE,BS,&u[1]);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,"cl");
//	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,Dr1,"cl");
            }
          else {  
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[3]);
				    PROCOper(LINE_TYPE_ISTRUZIONE,BS,&u[1]);
//            PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,i);
//	          PROCOper(LINE_TYPE_ISTRUZIONE,BS,Dr1,i);
            }
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
					if(VCost->l <= 255)
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),OPDEF_MODE_REGISTRO8,Regs->D);
					else if(!HIWORD(VCost->l))
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),OPDEF_MODE_REGISTRO16,Regs->D);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,VCost->l,OPDEF_MODE_REGISTRO32,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
					VQ=VALUE_IS_EXPR;
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
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[3],&u[1]);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[1],&u[2]);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[2],&u[0]);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO,0);
                }
              else {  
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[0],&u[2]);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[2],&u[1]);
                PROCOper(LINE_TYPE_ISTRUZIONE,movString,&u[1],&u[3]);
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
        case MODE_IS_CONSTANT2:                    
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
#if ARCHI
//      		sprintf(MyBuf,"%s #%s",AS,RCost->s);
//      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,Dr,Dr,NULL,MyBuf);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_SHIFT,RCost->l);
#elif Z80 || I8086
          i=RCost->l;
          goto myVShift4;
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							// (in teoria si potrebbe ottimizzare 1
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO32,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.d",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,Regs->D);
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO16,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.d",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO,Regs->D);
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO16,Regs->D);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO16,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									if(LOBYTE(LOWORD(RCost->l)) <= 8)
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
									else {
										PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(RCost->l)),
											OPDEF_MODE_REGISTRO16,Regs->D+1);
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
										}
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0) {		// 0... non so bene perché, se espressione forse
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D+1);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						else {
							if(LOBYTE(LOWORD(RCost->l)) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO8,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
								}
							}
						VQ=VALUE_IS_EXPR;
						}
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
		                  
  if(bAutoAssign && OutSource)
    _tcscpy(LastOut->rem,VVar->name);

  return 1;
  }  
        
int Ccc::subAdd(bool isAdd, int Mode, int8_t VQ, struct VARS *VVar, O_TYPE *VType, O_SIZE *VSize,
								int8_t RQ,O_TYPE RType, O_SIZE RSize, union STR_LONG *VCost, union STR_LONG *RCost, 
								struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3,bool bAutoAssign) {
  char AS[16],myBuf[16],T1S[16];                                        // m1 per auto-assign
  int i;
	O_SIZE OldSize;
  long l;

	// l'operazione è quasi simmetrica a meno del segno -, per cui si potrebbero unire COSTANTE1 e COSTANTE2 (come in subAOX

  OldSize=*VSize;
  if(1  /*m*/) {
    if((RType & VARTYPE_IS_POINTER) && (*VType & VARTYPE_IS_POINTER)) {    // lecito sommare int o int e ptr.. MA ANCHE SOTTRARRE!! 2025
			O_SIZE s1,s2;
			if((RType & ~VARTYPE_ARRAY) != ((*VType) & ~VARTYPE_ARRAY))		// altrimenti devono essere puntatori identici
				PROCWarn(4047);		// in certi casi errore!

			// se array OCCHIO che mancano le dim... finire
			s1=FNGetMemSize(*VType,*VSize,0,0);
			s2=FNGetMemSize(RType,RSize,0,0);
			if(s1 != s2)		// !
				PROCWarn(4047);
			}
    }                                     // lecito sottrarre ptr.. o ptr e int
  *T1S=0;
  if(*VType & 0xf /*VARTYPE_POINTER*/) {
	  if(*VType & 0xe /*VARTYPE_2POINTER*/) {
			if(RQ==VALUE_IS_COSTANTE) {
#if ARCHI
/*				if(Mode==2) {
					sprintf(T1S,"ASL #%d",log((double)*VSize)/log(2.0));
					}
				else {
					RCost->l=RCost->l *(*VSize);
					}*/
		    RCost->l *= FNGetMemSize(*VType,*VSize,0/*dim*/,2);    // sarebbe da agg. per struct/array
#elif Z80 || I8086 || MC68000 || MICROCHIP
		    RCost->l *= FNGetMemSize(*VType,*VSize,0/*dim*/,2);    // sarebbe da agg. per struct/array
#endif
				}
			}
    *VSize=getPtrSize(*VType);
    }
  else if(RType & VARTYPE_IS_POINTER) {		// verificare "else
    if(RSize>1) {
#if ARCHI
      /*if(Mode==1) {
        sprintf(T1S,"ASL #%d",log(RSize)/log(2));
        }
      else {
        VCost->l=VCost->l *RSize;
        }*/
      VCost->l *= FNGetMemSize(RType,RSize,0/*dim*/,2);   // manca struct/array
#elif Z80 || I8086 || MC68000 || MICROCHIP
      VCost->l *= FNGetMemSize(RType,RSize,0/*dim*/,2);   // manca struct/array
#endif
      }         
    *VSize=getPtrSize(RType);
    *VType=RType;
    OldSize=RSize;           // tutto questo se il primo è cost.
    }
#if ARCHI
  _tcscpy(AS,isAdd ? "ADD" : "SUB");
#elif Z80
  _tcscpy(AS,isAdd ? "add" : "sub");
#elif I8086
  _tcscpy(AS,isAdd ? "add" : "sub");
	if(bAutoAssign) {		// SERVE flag per non far aggiungere queste in PROCObj
		switch(*VSize) {
			case 1:
				_tcscat(AS," BYTE PTR ");
				break;
			case 2:
				_tcscat(AS," WORD PTR ");
				break;
			case 4:
				_tcscat(AS," DWORD PTR ");
				break;
			}
		}
#elif MC68000
  _tcscpy(AS,isAdd ? "add" : "sub");
	if(RQ & VALUE_IS_COSTANTE)
	  _tcscat(AS,"i");
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
  _tcscpy(AS,isAdd ? "ADDWF" : "SUBWF");
#endif

	switch(*VSize) {
	  case 1:
  		switch(Mode) {
        case MODE_IS_OTHER:
#if Z80
				  OpA(AS,&u[0],&u[1]);
				  break;
#elif MICROCHIP
				  OpA(AS,&u[0],&u[1]);
				  break;

					// #else prosegue??
#endif				  
    		case MODE_IS_VARIABLE:
#if ARCHI
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_SHIFTR,Regs->D+1);
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
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"mov",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,
										OPDEF_MODE_REGISTRO8,Regs->D);
									break;
								case CLASSE_AUTO:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"mov",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),
										OPDEF_MODE_REGISTRO8,Regs->D);
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[0],&u[1]);
						VQ=VALUE_IS_EXPR;
						}

#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
//       		PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
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
    		case MODE_IS_CONSTANT1:                                
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
			    i=VCost->l;
#if ARCHI
//      		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1,
						OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l))  /*,T1S*/);
#elif Z80
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(isAdd)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,i,&u[0]);
#elif I8086
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
					VQ=VALUE_IS_EXPR;
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),
						OPDEF_MODE_REGISTRO8,Regs->D);
					// o si potrebbe fare NEG dopo, se SUB...
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO8,Regs->D);
					VQ=VALUE_IS_EXPR;
//			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
#elif MICROCHIP
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(isAdd)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,i,&u[0]);
#endif
      		break;
    		case MODE_IS_CONSTANT2:		// in AutoAssign arriva sempre solo questo!
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
			    i=RCost->l;
#if ARCHI
//      		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1,
//						myBuf,T1S);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_SHIFT,VCost->l);
#elif Z80
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(isAdd)
						    PROCOper(LINE_TYPE_ISTRUZIONE,incString,u[0].mode,&u[0].s,u[0].ofs);
						  else  
						    PROCOper(LINE_TYPE_ISTRUZIONE,decString,u[0].mode,&u[0].s,u[0].ofs);
						  }  
					  }
					else  
	 					OpA(AS,&u[0],i);
#elif I8086
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
								OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
						if(!isAdd) {		// se sub da costante...
	//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
							VQ=VALUE_IS_EXPR;
							}
						else {
							if(!LOBYTE(LOWORD(RCost->l)))
								;
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
							}
						VQ=VALUE_IS_EXPR;
						}
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
						if(!isAdd) {		// se sub da costante...
	//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							VQ=VALUE_IS_EXPR;
							}
						else {
							if(!LOBYTE(LOWORD(RCost->l)))
								;
							else if(LOBYTE(LOWORD(RCost->l))<=8)
								PROCOper(LINE_TYPE_ISTRUZIONE,"addq",OPDEF_MODE_IMMEDIATO8,
									LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							}
						VQ=VALUE_IS_EXPR;
						}
#elif MICROCHIP
//					printf("\aecco Dr %s e DSl %s\n",Dr,Regs->DSl);
					if(i>=1 && i<=3) {
					  while(i--) {
						  if(isAdd)
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
  		switch(Mode) {
        case MODE_IS_OTHER:
#if Z80
          goto caseT0;
          break;
#endif
    		case MODE_IS_VARIABLE:
#if ARCHI
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
#elif Z80
											      
          if(VQ==VALUE_IS_VARIABILE && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO,Regs->D);
        		goto dontChgV;
        		}
          else {
	          if(Regs->D==1) {
	        		if(*AS == 's') {
	        		  *(AS+1)='b';
	        		  *(AS+2)='c';
	          		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	        		  }
							if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
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
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sla",OPDEF_MODE_REGISTRO16,Regs->D);
						}

					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"mov",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO16,Regs->D);
									break;
								case CLASSE_AUTO:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"mov",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO16,Regs->D);
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[0],&u[1]);
						VQ=VALUE_IS_EXPR;
						}


/*
          if(VQ==VALUE_IS_VARIABILE && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
       		  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
						VQ=VALUE_IS_EXPR;
        		goto dontChgV;
        		}
          else
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,u[1].mode,&u[1].s,u[1].ofs);
					VQ=VALUE_IS_EXPR;*/

#elif MC68000
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {// LEGARE a MemoryModel?!
  				  i=OldSize;
						if(i>1)
							PROCOper(LINE_TYPE_ISTRUZIONE,"lsl.l",OPDEF_MODE_IMMEDIATO,(uint8_t)(log(i)/log(2)),OPDEF_MODE_REGISTRO16,Regs->D);
						}
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
//									if(RQ!=VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
#elif MICROCHIP
											      
          if(VQ==VALUE_IS_VARIABILE && VVar->classe == CLASSE_REGISTER) {    // ERRORE! al massimo è OK per +=
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO,Regs->D);
        		goto dontChgV;
        		}
          else {
	          if(Regs->D==1) {
	        		if(*AS == 'S') {
	        		  *(AS+1)='U';
	        		  *(AS+2)='B';
	          		PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
	        		  }
							if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {
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
    		case MODE_IS_CONSTANT1:    // se il 1° è cost, uso gli stessi registri
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
      		i=VCost->l;
#if ARCHI
 //     		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1,
						OPDEF_MODE_IMMEDIATO16,LOWORD(i) /*,OPDEF_MODE_SHIFT,VCost->l*/);
#elif Z80
				  Op2A(AS,i,&u[0],bAutoAssign);
#elif I8086
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,LOWORD(i));
					VQ=VALUE_IS_EXPR;
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1);
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),OPDEF_MODE_REGISTRO16,Regs->D);
					// o si potrebbe fare NEG dopo, se SUB...
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
					VQ=VALUE_IS_EXPR;
#elif MICROCHIP
				  Op2A(AS,i,&u[0],bAutoAssign);
#endif
      		break;
    		case MODE_IS_CONSTANT2:		// in AutoAssign arriva sempre solo questo!
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
      		l=RCost->l;
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {// LEGARE a MemoryModel?!
  				  i=OldSize;
						if(i>1)
					    l*=i;
						}
#if ARCHI
//      		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1,
						OPDEF_MODE_IMMEDIATO16,LOWORD(l)/*,OPDEF_MODE_SHIFT,VCost->l*/);
#elif Z80
				  Op2A(AS,&u[0],l,bAutoAssign);
#elif I8086
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->P,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_IMMEDIATO16,LOWORD(l));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),OPDEF_MODE_IMMEDIATO16,LOWORD(l));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
						if(!isAdd) {		// se sub da costante...
	//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
							VQ=VALUE_IS_EXPR;
							}
						else {
							if(!LOWORD(l))
								;
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
							}
						}
					VQ=VALUE_IS_EXPR;
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
						if(!isAdd) {		// se sub da costante...
	//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),OPDEF_MODE_REGISTRO16,0);
							VQ=VALUE_IS_EXPR;
							}
						else {
							if(!LOWORD(l))
								;
							else if(LOWORD(l) <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,"addq",OPDEF_MODE_IMMEDIATO16,LOWORD(l),u[0].mode,&u[0].s,u[0].ofs);
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l),u[0].mode,&u[0].s,u[0].ofs);
							}
						}
					VQ=VALUE_IS_EXPR;
#elif MICROCHIP
				  Op2A(AS,&u[0],i,bAutoAssign);
#endif
      		break;
    		}
      break;

    case 4:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
#if ARCHI
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,T1S);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
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
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
      		if(*AS == 'a')
      		  *(AS+2)='c';
      		else {
      		  *(AS+1)='b';
      		  *(AS+2)='b';
      		  }
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
					VQ=VALUE_IS_EXPR;
#elif MC68000
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {// LEGARE a MemoryModel?!
  				  i=OldSize;
						if(i>1)
					    PROCOper(LINE_TYPE_ISTRUZIONE,"lsl.l",OPDEF_MODE_IMMEDIATO,(uint8_t)(log(i)/log(2)),OPDEF_MODE_REGISTRO32,Regs->D);
						}
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
//									if(RQ != VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
//									if(RQ != VALUE_IS_D0)
//										PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// (0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
						VQ=VALUE_IS_EXPR;
						}
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
    		case MODE_IS_CONSTANT1:                                
					if(Optimize & OPTIMIZE_CONST && !VCost->l)		// :)
						break; 
      		l=VCost->l;
#if ARCHI
//      		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1,
						OPDEF_MODE_IMMEDIATO32,VCost->l /*,OPDEF_MODE_SHIFT,VCost->l*/);
#elif Z80
      		if(LOBYTE(LOWORD(l))) {
						OpA(AS,LOBYTE(LOWORD(l)),&u[0]);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,HIBYTE(LOWORD(l)),&u[0]);
					if(u[1].mode & CONDIZ_MINORE)
				    IncOp(&u[1]);
					OpA(AS,LOBYTE(HIWORD(l)),&u[1]);
			    IncOp(&u[1]);
					OpA(AS,HIBYTE(HIWORD(l)),&u[1]);
#elif I8086
      		i=(int)l;
      		if(i) {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='b';
        		  }
        		}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,VCost->l,OPDEF_MODE_REGISTRO32,Regs->D);
					// o si potrebbe fare NEG dopo, se SUB...
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
					VQ=VALUE_IS_EXPR;
#elif MICROCHIP
      		if(LOBYTE(LOWORD(l))) {
						OpA(AS,LOBYTE(LOWORD(l)),&u[0]);
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,HIBYTE(LOWORD(l)),&u[0]);
					if(u[1].mode & CONDIZ_MINORE)
				    IncOp(&u[1]);
					OpA(AS,LOBYTE(HIWORD(l)),&u[1]);
			    IncOp(&u[1]);
					OpA(AS,HIBYTE(HIWORD(l)),&u[1]);
#endif
      		break;
    		case MODE_IS_CONSTANT2:		// in AutoAssign arriva sempre solo questo!
					if(Optimize & OPTIMIZE_CONST && !RCost->l) {		// :)
						VQ=VALUE_IS_EXPR;		// xché cmq ho già letto la var#1
						break;
						}
      		l=RCost->l;
					if((*VType & VARTYPE_IS_POINTER) && !(RType & VARTYPE_IS_POINTER)) {// LEGARE a MemoryModel?!
  				  i=OldSize;
						if(i>1)
					    l*=i;
						}
#if ARCHI
//      		*myBuf='#';
//      		_tcscpy(myBuf+1,VCost->s);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1,
						OPDEF_MODE_IMMEDIATO32,RCost->l /*,OPDEF_MODE_SHIFT,VCost->l*/);
#elif Z80
      		i=(int)l;
      		if(LOBYTE(LOWORD(l))) {
						OpA(AS,&u[0],LOBYTE(LOWORD(l)));
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,&u[0],HIBYTE(LOWORD(l)));
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					OpA(AS,&u[1],LOBYTE(HIWORD(l)));
			    IncOp(&u[1]);
					OpA(AS,&u[1],HIBYTE(HIWORD(l)));
#elif I8086
      		if(i) {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='b';
        		  }
        		}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,HIWORD(l));
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						if(!isAdd) {		// se sub da costante...
	//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,0);
							VQ=VALUE_IS_EXPR;
							}
						else {
							if(!l)
								;
							else if(l <= 8)
								PROCOper(LINE_TYPE_ISTRUZIONE,"addq",OPDEF_MODE_IMMEDIATO32,l,u[0].mode,&u[0].s,u[0].ofs);
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,u[0].mode,&u[0].s,u[0].ofs);
							}
						}
					VQ=VALUE_IS_EXPR;
#elif MICROCHIP
      		if(LOBYTE(LOWORD(l))) {
						OpA(AS,&u[0],LOBYTE(LOWORD(l)));
        		if(*AS == 'a')
        		  *(AS+2)='c';
        		else {
        		  *(AS+1)='b';
        		  *(AS+2)='c';
        		  }
        		}
			    IncOp(&u[0]);
					OpA(AS,&u[0],HIBYTE(LOWORD(l)));
					if(u[0].mode & CONDIZ_MINORE)
				    IncOp(&u[0]);
					OpA(AS,&u[1],LOBYTE(HIWORD(l)));
			    IncOp(&u[1]);
					OpA(AS,&u[1],HIBYTE(HIWORD(l)));
#endif
      		break;
    		}
      break;
    }  
//  VQ=VALUE_IS_EXPR;		// sistemare o lasciare com'è nei vari casi! a seconda che valore e flag siano già in D0 ecc oppure no

dontChgV:  
  *VSize=OldSize;
  if(!isAdd) {                                    // sottraggo ptr e diventa int...
    if((RType & VARTYPE_IS_POINTER) && (*VType & VARTYPE_IS_POINTER)) {
			i=FNGetMemSize(*VType,*VSize,0/*dim*/,2);
      *VType=VARTYPE_PLAIN_INT;
      *VSize=PTR_SIZE;
			if(i>1) {
#if ARCHI
//				fare
#elif Z80
  			while(i >>= 1)
					PROCOper(LINE_TYPE_ISTRUZIONE,"sar",OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
				if(CPU86<1) {
  				while(i >>= 1)
						PROCOper(LINE_TYPE_ISTRUZIONE,"sar",OPDEF_MODE_REGISTRO,Regs->D);
					}
				else
					PROCOper(LINE_TYPE_ISTRUZIONE,"sar",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,(uint8_t)(log(i)/log(2)));
#elif MC68000
//				PROCOper(LINE_TYPE_ISTRUZIONE,"asr.l",OPDEF_MODE_IMMEDIATO,(uint8_t)(log(i)/log(2)),u[0].mode,&u[0].s,u[0].ofs);
				// in effetti ci va divisione! in caso di agggregati ecc
				if((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM)
				// (e andrebbe fatta con long, almeno in certi casi
					PROCOper(LINE_TYPE_ISTRUZIONE,"divs",OPDEF_MODE_IMMEDIATO,i,u[0].mode,&u[0].s,u[0].ofs);
				else
					PROCOper(LINE_TYPE_ISTRUZIONE,"divs",OPDEF_MODE_IMMEDIATO,i,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
				u[2]=u[0];
				u[3]=u[1];
				while(i--) {
					u[0]=u[2];
					u[1]=u[3];
					Op2A("RLC",&u[0],&u[1]);		// verificare!
					}
#endif
	      }
      }
    }

  if(bAutoAssign && OutSource)
    _tcscpy(LastOut->rem,VVar->name);

  return VQ;
  }
  
int Ccc::subMul(char m, int Mode, int8_t VQ, struct VARS *VVar, O_TYPE VType, O_SIZE VSize, int8_t RQ, uint32_t RType, O_SIZE RSize, union STR_LONG *VCost, union STR_LONG *RCost, 
								struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3,bool bAutoAssign) {
  char myBuf[16],*AS;
  int i;
  struct VARS *v;
             
  if(m=='*') {
#if ARCHI
		switch(VSize) {
		  case 1:
		    PROCOper(LINE_TYPE_ISTRUZIONE,"MUL",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
				break;
		  case 2:
		    PROCOper(LINE_TYPE_ISTRUZIONE,"MUL",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
				break;
		  case 4:
		    PROCOper(LINE_TYPE_ISTRUZIONE,"MUL",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
				break;
			}
#elif Z80
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        break;  
		      case MODE_IS_CONSTANT1:
						// v. sotto
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
							break; 
						else if(!RCost->l)		// :) fare, mettere 0
							; 
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost,FALSE);
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
				if(Optimize & OPTIMIZE_CONST) {
					if((Mode == MODE_IS_CONSTANT1 && VCost->l == 1) || (Mode == MODE_IS_CONSTANT2 && RCost->l == 1))
						break;
					}
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
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        break;
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if((Mode == MODE_IS_CONSTANT1 && VCost->l == 1) || (Mode == MODE_IS_CONSTANT2 && RCost->l == 1))
							// FINIRE!
							break;
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,FALSE);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        break;  
	        }
				if((Mode == MODE_IS_CONSTANT1 && VCost->l == 1) || (Mode == MODE_IS_CONSTANT2 && RCost->l == 1))
					break;
	      v=FNCercaVar("_lmul",0);
  	    if(!v)
    		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
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
		}
#elif I8086
  AS="mul";
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
	switch(VSize) {
	  case 1:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
         	PROCOper(LINE_TYPE_ISTRUZIONE,"mul BYTE PTR "/*AS*/,u[1].mode,&u[1].s,u[1].ofs);
      		break;
    		case MODE_IS_CONSTANT1:                                
    		  i=VCost->l;
myMul1:    		  
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						}
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
						break; 
					else if(!RCost->l)		// :) fare, mettere 0
						; 
    		  i=RCost->l;
    		  goto myMul1;
      		break;
    		}
      break;
    case 2:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
         	PROCOper(LINE_TYPE_ISTRUZIONE,"mul WORD PTR "/*AS*/,u[1].mode,&u[1].s,u[1].ofs);
      		break;
    		case MODE_IS_CONSTANT1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
myMul2:
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO,LOWORD(i));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOWORD(i));
						}
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
						break; 
					else if(!RCost->l)		// :) fare, mettere 0
						; 
    		  i=RCost->l;
					goto myMul2;
      		break;
    		}
      break;
    case 4:
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+3);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+2);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
      v=FNCercaVar("_lmul",0);
 	    if(!v)
   		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
      for(i=0; i<3; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
      if(Regs->D>0) {
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,u[0].mode,&u[0].s,u[0].ofs);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,u[0].mode,&u[0].s,u[0].ofs);
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->DS,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D1S,"dx");
        Regs->Get();
        }
      break;
    }  
	}
#elif MC68000
  if(VType & VARTYPE_FLOAT) {
		switch(VSize) {
			case 4:
				break;
			case 8:
				break;
			}
		}
	else {
  AS=VType & VARTYPE_UNSIGNED ? "mulu" : "muls";
			// n.b. MUL moltiplica 2 word in una long

	if(RSize==1 && Mode != MODE_IS_CONSTANT2)
		PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",&u[1]);

	switch(VSize) {
	  case 1:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					if(VType & VARTYPE_UNSIGNED)
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,u[0].mode,&u[0].s,u[0].ofs);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",u[0].mode,&u[0].s,u[0].ofs);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
      		break;
    		case MODE_IS_CONSTANT1:                                
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOBYTE(LOWORD(VCost->l)))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,Regs->D);
						if(LOBYTE(LOWORD(VCost->l)) == 1) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(VCost->l)),u[0].mode,&u[0].s,u[0].ofs);
							}
						}
					else {
						if(!LOBYTE(LOWORD(VCost->l)))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,Regs->D+1);
						else if(LOBYTE(LOWORD(VCost->l)) < 256)
							PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),u[1].mode,&u[1].s,u[1].ofs);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(VCost->l)),u[0].mode,&u[0].s,u[0].ofs);
						}
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(RCost->l))
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO16,Regs->D);
						if(RCost->l <= 1)
							break;
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		}
      break;
    case 2:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
      		break;
    		case MODE_IS_CONSTANT1:    // se il 1° è cost, uso gli stessi registri
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(VCost->l))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D);
						if(LOBYTE(LOWORD(VCost->l)) == 1) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),u[0].mode,&u[0].s,u[0].ofs);
							}
						}
					else {
						if(!LOWORD(VCost->l))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D);
						else if(VCost->l < 256)
							PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),u[0].mode,&u[0].s,u[0].ofs);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(VCost->l),u[0].mode,&u[0].s,u[0].ofs);
						}
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(RCost->l))
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D);
						if(RCost->l <= 1)
							break;
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		}
      break;
    case 4:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					if(Regs->D>1) {		// forse finire
  					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,Regs->D+1);
//  					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO32,1);
						}
myMul4_:
					v=FNCercaVar("_lmul",0);
 					if(!v)
   					v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     			PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
					if(Regs->D>0) {
		//boh qua				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
		//        Regs->Get();
						}
      		break;
    		case MODE_IS_CONSTANT1:    // se il 1° è cost, uso gli stessi registri
					if(Optimize & OPTIMIZE_CONST) {
						if(!VCost->l) {
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,0);
							break;
							}
						if(VCost->l == 1) {
							break;
							}
						}
//	  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO32,0);
	        if(!VCost->l)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,1);
	        else if(VCost->l < 256)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,VCost->l,OPDEF_MODE_REGISTRO32,1);
			    else
			      PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,VCost->l,OPDEF_MODE_REGISTRO32,1);
//	  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,i,OPDEF_MODE_REGISTRO32,1);
					goto myMul4_;
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {
						if(!RCost->l) {
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,0);
							break;
							}
						if(RCost->l == 1) {
							break;
							}
						}
	        if(!RCost->l)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,1);
	        else if(RCost->l < 256)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,RCost->l,OPDEF_MODE_REGISTRO32,1);
			    else
			      PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,RCost->l,OPDEF_MODE_REGISTRO32,1);
					goto myMul4_;
      		break;
    		}
      break;
    case 8:		// boh nel caso!
// finire
      break;
    }  
	}
#elif MICROCHIP
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
		switch(VSize) {
		  case 1:
			  AS="MULLW";
  			switch(Mode) {
					case MODE_IS_OTHER:
    			case MODE_IS_VARIABLE:
         		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,0,NULL);
      			break;
    			case MODE_IS_CONSTANT1:                                
    				i=VCost->l;
myMul1:    		  
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,0,i);
      			break;
    			case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
							break; 
						else if(!RCost->l)		// :) fare, mettere 0
							; 
    				i=RCost->l;
    				goto myMul1;
      			break;
    			}
				break;
		  case 2:
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        break;  
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
							break; 
						else if(!RCost->l)		// :) fare, mettere 0
							; 
		        i=Regs->Inc(2);
   	        PROCUseCost(RQ,RType,RSize,RCost,0,FALSE);		// FINIRE
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
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
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
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST && RCost->l == 1)		// :)
							break; 
						else if(!RCost->l)		// :) fare, mettere 0
							; 
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,0,FALSE);		// FINIRE
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_lmul",0);
  	    if(!v)
    		  v=PROCAllocVar("_lmul",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
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
		}
#endif
    }		// mul
  else {		// div,mod 
	// (OTTIMIZZARE se divisore multiplo di 2 ?? qua o in ottimizzazioni?
#if ARCHI
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
    if(Regs->D>0) {
      Regs->Save();
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO,Regs->D+1);
      }
	  v=FNCercaVar("__IntDiv",0);
  	if(!v)
    	v=PROCAllocVar("__IntDiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
    PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
//    PROCOper(LINE_TYPE_CALL,"BL __IntDiv",NULL,NULL);
		}
#elif Z80
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        break;  
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost,FALSE);
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
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
				    IncOp(&u[3]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[3].mode,&u[3].s,u[3].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
				    IncOp(&u[2]);
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u[2].mode,&u[2].s,u[2].ofs);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        break;
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,FALSE);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_ldiv",0);
  	    if(!v)
    		  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
				if(Regs->D>0) {
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"ax");
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"dx");
					Regs->Get();
					}
        break;
      }
		}
#elif I8086
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
  AS="div";
	switch(VSize) {
	  case 1:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1);
      		break;
    		case MODE_IS_CONSTANT1:
    		  i=VCost->l;
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						}
      		break;
    		case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
    		  i=RCost->l;
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D+1);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(i)));
						}
      		break;
    		}
      break;
    case 2:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1);
      		break;
    		case MODE_IS_CONSTANT1:    // se il 1° è cost, uso gli stessi registri
    		  i=VCost->l;
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,LOWORD(i));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(i));
						}
      		break;
    		case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
    		  i=RCost->l;
					if(CPU86<1) {
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,LOWORD(i));
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(i));
						}
      		break;
    		}
      break;
    case 4:
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+2);
      PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+3);
      v=FNCercaVar("_ldiv",0);
      if(!v)
    	  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
      UseLMul=TRUE;
      for(i=0; i<4; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
      if(Regs->D>0) {
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D+1,"dx");
        Regs->Get();
        }
      break;
    }  
		}
#elif MC68000
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
  AS=VType & VARTYPE_UNSIGNED ? "divu" : "divs";
			// n.b. DIV usa sempre long come dividendo e short come divisore, ergo i cast

	// (OTTIMIZZARE se divisore multiplo di 2
	if(RSize==1 && Mode != MODE_IS_CONSTANT2)
		if(VType & VARTYPE_UNSIGNED)
			PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,u[1].mode,&u[1].s,u[1].ofs);
		else
			PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",u[1].mode,&u[1].s,u[1].ofs);

	switch(VSize) {
	  case 1:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					if(VType & VARTYPE_UNSIGNED) {
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x000000ff,u[0].mode,&u[0].s,u[0].ofs);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",u[0].mode,&u[0].s,u[0].ofs);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",u[0].mode,&u[0].s,u[0].ofs);
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
      		break;
    		case MODE_IS_CONSTANT1:                                
					if(Optimize & OPTIMIZE_CONST) {		// vale sia per / che %
						if(!LOBYTE(LOWORD(VCost->l)) || (LOBYTE(LOWORD(VCost->l))==1 && m=='%')) {
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,Regs->D);
							break;
							}
						}
			    PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
          if(!LOBYTE(LOWORD(VCost->l)))
			      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,Regs->D);
	        else if(LOBYTE(LOWORD(VCost->l)) < 256)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(VCost->l)),
							OPDEF_MODE_REGISTRO8,Regs->D);
//					PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(RCost->l))
				      PROCError(2058);		// mah sì
						if(RCost->l == 1) {
							if(m=='%')
					      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,0 /*Regs->D*/);	// andrebbe poi tolto lo swap sotto...
							else
								;
							break;
							}
						}
					if(VType & VARTYPE_UNSIGNED) {
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x000000ff,u[0].mode,&u[0].s,u[0].ofs);
						}
					else {
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",u[0].mode,&u[0].s,u[0].ofs);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",u[0].mode,&u[0].s,u[0].ofs);
						}
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(RCost->l)),u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		}
      break;
    case 2:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					if(VType & VARTYPE_UNSIGNED)
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x0000ffff,u[0].mode,&u[0].s,u[0].ofs);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",u[0].mode,&u[0].s,u[0].ofs);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
      		break;
    		case MODE_IS_CONSTANT1:    // 
					if(Optimize & OPTIMIZE_CONST) {		// vale sia per / che %
						if(!LOWORD(VCost->l) || (LOWORD(VCost->l)==1 && m=='%')) {
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO8,Regs->D);
							break;
							}
						}
			    PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1);
          if(!LOWORD(VCost->l))
			      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D);
	        else if(LOWORD(VCost->l) < 256)
			      PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO16,LOBYTE(LOWORD(VCost->l)),OPDEF_MODE_REGISTRO16,Regs->D);
//					PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(RCost->l))
				      PROCError(2058);		// mah sì
						if(RCost->l == 1) {
							if(m=='%')
					      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,0 /*Regs->D*/);	// andrebbe poi tolto lo swap sotto...
							else
								;
							break;
							}
						}
					if(VType & VARTYPE_UNSIGNED)
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x0000ffff,u[0].mode,&u[0].s,u[0].ofs);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",u[0].mode,&u[0].s,u[0].ofs);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),u[0].mode,&u[0].s,u[0].ofs);
      		break;
    		}
      break;
    case 4:
  		switch(Mode) {
        case MODE_IS_OTHER:
    		case MODE_IS_VARIABLE:
					if(Regs->D>1) {		// forse finire
//	  				PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO32,0);
	  				PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO32,Regs->D+1);
						}
myDiv4_:
					v=FNCercaVar("_ldiv",0);
 					if(!v)
   					v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
     			PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
					if(Regs->D>0) {
		//boh qua				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,u[1].mode,&u[1].s,u[1].ofs);
		//        Regs->Get();
						}
      		break;
    		case MODE_IS_CONSTANT1:
					i=VCost->l;
					if(Optimize & OPTIMIZE_CONST) {
						if(!VCost->l || (VCost->l==1 && m=='%')) {
				      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,0);
							break;
							}
						}
myDiv4:
//	  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO32,0);
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_IMMEDIATO32,i,OPDEF_MODE_REGISTRO32,Regs->D);		// dovrebbe essere 1 (minimo
					goto myDiv4_;
      		break;
    		case MODE_IS_CONSTANT2:
					i=RCost->l;
					if(Optimize & OPTIMIZE_CONST) {
						if(!LOWORD(RCost->l))
				      PROCError(2058);		// mah sì
						if(RCost->l == 1) {
							if(m=='%')
					      PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,0 /*Regs->D*/);	// andrebbe poi tolto lo swap sotto...
							else
								;
							break;
							}
						}
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
      PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
      UseLMul=TRUE;
      for(i=0; i<4; i++)
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO32,3);
      if(Regs->D>0) {
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D,"ax");
//        PROCOper(LINE_TYPE_ISTRUZIONE,movString,3,Regs->D+1,"dx");
        Regs->Get();
        }
			break;
    }  
		}
#elif MICROCHIP
	  if(VType & VARTYPE_FLOAT) {
			switch(VSize) {
			  case 4:
					break;
				case 8:
					break;
				}
			}
		else {
		switch(VSize) {
		  case 1:
		  case 2:
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
		        break;  
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
		        i=Regs->Inc(2);
            PROCUseCost(RQ,RType,RSize,RCost,0,FALSE);		// FINIRE
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
		    switch(Mode) {
		      case MODE_IS_VARIABLE:
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
		      case MODE_IS_CONSTANT1:
		        break;  
		      case MODE_IS_CONSTANT2:
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo div?					if(RCost->l == 1)		// :)
//						break; 
//					else if(!RCost->l)		// :) fare, dare errore?
//						; 
						}
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
            PROCUseCost(RQ,RType,RSize,RCost,0,FALSE);
		        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
		        break;  
	        }
	      v=FNCercaVar("_ldiv",0);
  	    if(!v)
    		  v=PROCAllocVar("_ldiv",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v->label,0);
        for(i=0; i<4; i++)
          PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
				if(Regs->D>0) {
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"ax");
//					PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"dx");
					Regs->Get();
					}
        break;
      }
		}
#endif
    if(m=='%') {
			if(VType & VARTYPE_FLOAT) {
				PROCError(2297,NULL);    // direi
				}
#if ARCHI
      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
#elif Z80
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_HIGH8,1);
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#elif I8086
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#elif MC68000
      if(VSize<4)
	      PROCOper(LINE_TYPE_ISTRUZIONE,"swap",u[0].mode,&u[0].s,u[0].ofs);
			else
	      PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,1,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_HIGH8,1);
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,1);
#endif
      }
    if(Regs->D>1) {
#if ARCHI
      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,0,
				OPDEF_MODE_REGISTRO32,Regs->D);
      Regs->Get();
#elif Z80 
      if(VSize<4) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_REGISTRO,0);
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

  if(bAutoAssign && OutSource)
    _tcscpy(LastOut->rem,VVar->name);

  return 1;
  }

uint8_t Ccc::FNIs1Bit(uint32_t t) {
  register uint32_t i,j=0;
	uint8_t k,i1;
  
  for(i=0x00000001,i1=0; i; i <<= 1,i1++) {
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

uint8_t Ccc::FNIsPower2(uint32_t t) {
  register uint32_t i,k,j=0;
	uint8_t i1;
  
  for(i=0x80000000,i1=31,k=0x7fffffff; i; i >>= 1,i1--) {
    if(t & i) {
	    if(!(t & k))
				return i1;
			else
				return 0;
      }
		k &= k >> 1;
    }
   return 0;   
  }

O_SIZE Ccc::getPtrSize(O_TYPE t) {
	O_SIZE s;

#if ARCHI
	if(((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) || (t & VARTYPE_FAR))
		s=PTR_SIZE;
	else
		s=4;
#elif Z80
	if(((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) || (t & VARTYPE_FAR))
		s=4;			// mah
	else
		s=PTR_SIZE;
#elif I8086
	if(((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) || (t & VARTYPE_FAR))
		s=4;
	else
		s=2;
#elif MC68000
	if(((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) || (t & VARTYPE_FAR))
		s=PTR_SIZE;
	else
		s=2;
#elif MICROCHIP
	if((MemoryModel & 0xf)==MEMORY_MODEL_LARGE || (t & VARTYPE_FAR))
		s=4;
	else if((MemoryModel & 0xf)==MEMORY_MODEL_MEDIUM)
		s=2;
	else
		s=PTR_SIZE;			// sarebbe 1 byte su PIC16, volendo..
#endif

	return s;
  }

int Ccc::subAOX(char m, int16_t *cond, int Mode, int8_t VQ, struct VARS *VVar, O_TYPE VType, O_SIZE VSize, 
								int8_t RQ, O_TYPE RType, O_SIZE RSize, union STR_LONG *VCost, union STR_LONG *RCost, struct OP_DEF *u, 
								struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3,bool bAutoAssign) {
  char AS[16],myBuf[16];
  int i,j;
  long l;

  if((RType & VARTYPE_IS_POINTER) || (VType & VARTYPE_IS_POINTER))
    PROCError(2111,NULL);     // messaggio da cambiare
  if(VType & VARTYPE_FLOAT) {
    PROCError(2297,NULL);    // direi
		}
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

//???  VQ=1;  era così pre-68000 ... verificare altre cPU ora

#if MC68000
	if(Mode==2)		// costante
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
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
//          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,NULL);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D,
						OPDEF_MODE_REGISTRO8,Regs->D);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
					if(*cond && m=='&')
            VQ=CONDIZ_DIVERSO;      //-15;
#elif I8086
			    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
//          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr1);
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
					  VQ=VALUE_IS_EXPR;
						}
#endif
          break;
        case MODE_IS_CONSTANT1:
          i=VCost->l;
myAOX1:          
#if ARCHI
//          *myBuf='#';
//          _tcscpy(myBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,
						/*myBuf,*/OPDEF_MODE_REGISTRO8,Regs->D+1);
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
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,0,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)));
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(i))) {	// :)
								if(m=='&')
									PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
								else 
									goto done1;
								}
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)),OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(i))) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
										else 
											goto done1;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(i))) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
										else 
											goto done1;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)),OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(i))) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
										else 
											goto done1;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)),OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(Optimize & OPTIMIZE_CONST && !LOBYTE(LOWORD(i))) {	// :)
							if(m=='&')
								PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",u[0].mode,&u[0].s,u[0].ofs);
							else 
								goto done1;
							}
						else {
							if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
								PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(i)),u[0].mode,&u[0].s,u[0].ofs);
							}
done1:
						VQ=VALUE_IS_EXPR;
						}
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
        case MODE_IS_CONSTANT2: 
#if ARCHI
          i=RCost->l;
          goto myAOX1;
#elif Z80
          i=RCost->l;
          goto myAOX1;
#elif I8086
          i=RCost->l;
          goto myAOX1;
#elif MC68000
          i=RCost->l;
          goto myAOX1;
#elif MICROCHIP
          i=RCost->l;
          goto myAOX1;
#endif
          break;                        
        default:
          break;
        }
      break;
	  case 2:
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
//          PROCOper(LINE_TYPE_ISTRUZIONE,AS,Dr,Dr,Dr1,NULL);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
			    IncOp(&u[0]);
			    IncOp(&u[1]);
          OpA(AS,&u[0],&u[1]);
#elif I8086
//          PROCOper(AS,Dr,Dr);
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(/*VQ==VALUE_IS_0 ||*/ VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
					  VQ=VALUE_IS_EXPR;
						}
#endif
          break;
        case MODE_IS_CONSTANT1:
          j=VCost->l;
myAOX:    		                    
#if ARCHI
//          *myBuf='#';
//          _tcscpy(myBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,
						/*myBuf,*/OPDEF_MODE_REGISTRO16,Regs->D+1);
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
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,LOWORD(j));
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(j),OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									if(Optimize & OPTIMIZE_CONST && !LOWORD(j)) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
										else 
											goto done2;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(j),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									if(Optimize & OPTIMIZE_CONST && !LOWORD(j)) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
										else 
											goto done2;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(j),OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									if(Optimize & OPTIMIZE_CONST && !LOWORD(j)) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
										else 
											goto done2;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(j),OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						}
					else {
						if(Optimize & OPTIMIZE_CONST && !LOWORD(j)) {	// :)
							if(m=='&')
								PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",u[0].mode,&u[0].s,u[0].ofs);
							else 
								goto done2;
							}
						else {
							if(/*VQ==VALUE_IS_0 ||*/ VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
								PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
		//						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,LOWORD(j),u[0].mode,&u[0].s,u[0].ofs);
							}
done2:
					  VQ=VALUE_IS_EXPR;
						}
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
        case MODE_IS_CONSTANT2:
#if ARCHI
//          *myBuf='#';
//          _tcscpy(myBuf+1,RCost->s);
	        PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D,
						OPDEF_MODE_IMMEDIATO16,RCost->l);
#elif Z80 || I8086 || MICROCHIP
          j=RCost->l;
          goto myAOX;
#elif MC68000 
          j=RCost->l;
          goto myAOX;
#endif
          break;                        
        default:
          break;
        }
      break;
	  case 4:
      switch(Mode) {
        case MODE_IS_OTHER:
        case MODE_IS_VARIABLE:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D,
						OPDEF_MODE_REGISTRO32,Regs->D+1);
#elif Z80 || MICROCHIP
          OpA(AS,&u[0],&u[1]);
			    IncOp(&u[0]);
			    IncOp(&u[1]);
          OpA(AS,&u[0],&u[1]);
#elif I8086
//          PROCOper(AS,Dr,Dr2);
//          PROCOper(AS,Dr1,Dr3);
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
//							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(/*VQ==VALUE_IS_0 ||*/ VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
					  VQ=VALUE_IS_EXPR;
						}
#endif
          break;
        case MODE_IS_CONSTANT1:
          l=VCost->l;
myAOXl:    		                    
#if ARCHI
//          *myBuf='#';
//          _tcscpy(myBuf+1,VCost->s);
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,
						/*myBuf,*/OPDEF_MODE_REGISTRO32,Regs->D+1);
#elif Z80 || MICROCHIP
          i=LOBYTE(LOWORD(l));
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
          i=LOWORD(l);
          if((i!=0xffff && m=='&') || (i && m!='&')) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,0,i);
            }
          i=HIWORD(l);
          if((i!=0xffff && m=='&') || (i && m!='&')) {
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,0,i);
            }
#elif MC68000
					if(bAutoAssign) {
						if(VQ==VALUE_IS_D0) {
							if(Optimize & OPTIMIZE_CONST && !l) {	// :)
								if(m=='&')
									PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
								else 
									goto done4;
								}
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else if(VQ==VALUE_IS_VARIABILE) {
							switch(VVar->classe) {
								case CLASSE_EXTERN:
								case CLASSE_GLOBAL:
								case CLASSE_STATIC:
									if(Optimize & OPTIMIZE_CONST && !l) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
										else 
											goto done4;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
									break;
								case CLASSE_AUTO:
									if(Optimize & OPTIMIZE_CONST && !l) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
										else 
											goto done4;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
									break;
								case CLASSE_REGISTER:
									if(Optimize & OPTIMIZE_CONST && !l) {	// :)
										if(m=='&')
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
										else 
											goto done4;
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label));
									break;
								}
							}
						else
							;
						VQ=VALUE_IS_EXPR;
						}
					else {
						if(Optimize & OPTIMIZE_CONST && !l) {	// :)
							if(m=='&')
								PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",u[0].mode,&u[0].s,u[0].ofs);
							else 
								goto done4;
							}
						else {
							if(VQ==VALUE_IS_0 || VQ==VALUE_IS_D0)		// 0... non so bene perché, se espressione forse
								PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,u[0].mode,&u[0].s,u[0].ofs);
							}
done4:
						VQ=VALUE_IS_EXPR;
						}
#endif
          break;
        case MODE_IS_CONSTANT2:
#if ARCHI
//          *myBuf='#';
//          _tcscpy(myBuf+1,RCost->s);
//	        PROCOper(AS,Dr,Dr,myBuf,NULL);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D /*Dr*/,
						OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_SHIFT,RCost->l);
#elif Z80 || I8086 || MICROCHIP
          l=RCost->l;
          goto myAOXl;
#elif MC68000 
          l=RCost->l;
          goto myAOXl;
#endif
          break;                        
        default:
          break;
        }
	    break;
    }
//  VQ=VALUE_IS_EXPR;		// sistemare o lasciare com'è nei vari casi! a seconda che valore e flag siano già in D0 ecc oppure no

  if(bAutoAssign && OutSource)
    _tcscpy(LastOut->rem,VVar->name);

  return VQ;
  }


enum Ccc::OPERANDO_CONDIZIONALE Ccc::subCMP(const char *TS, int cond, int Mode, int8_t VQ, struct VARS *VVar, O_TYPE VType, O_SIZE VSize, 
																						int RQ,O_TYPE RType, O_SIZE RSize, union STR_LONG *VCost, union STR_LONG *RCost, 
																						struct OP_DEF *u, struct OP_DEF *u1, struct OP_DEF *u2, struct OP_DEF *u3) {
  char AS[16],myBuf[16];
  int i;
	enum OPERANDO_CONDIZIONALE CC;
  long l;


  switch(*TS) {
    case '<':
			if(VType & VARTYPE_UNSIGNED) {		// FARE! 2025
				if(*(TS+1) == '=')
					CC=CONDIZ_MINORE_UGUALE_UNSIGNED;   //-13;
				else
					CC=CONDIZ_MINORE_UNSIGNED; 	//-10;
				}
			else {
				if(*(TS+1) == '=')
					CC=CONDIZ_MINORE_UGUALE;   //-13;
				else
					CC=CONDIZ_MINORE; 	//-10;
				}
      break;
    case '>':
			if(VType & VARTYPE_UNSIGNED) {		// FARE! 2025
				if(*(TS+1) == '=')
					CC=CONDIZ_MAGGIORE_UGUALE_UNSIGNED;   //-11;
				else
					CC=CONDIZ_MAGGIORE_UNSIGNED;   //-12;
				}
			else {
				if(*(TS+1) == '=')
					CC=CONDIZ_MAGGIORE_UGUALE;   //-11;
				else
					CC=CONDIZ_MAGGIORE;   //-12;
				}
      break;
    case '=': 
      CC=CONDIZ_UGUALE;     //-14;
      break;
    case '!':
      CC=CONDIZ_DIVERSO;     //-15;
      break;
    default:
      break;
    }
  VSize=FNGetMemSize(VType,VSize,0/*dim*/,1);
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
	if(Mode==1 || Mode==2)		// costante
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
  if(VType & VARTYPE_FLOAT) {
		struct VARS *v;
		v=FNCercaVar("_fcmp",0);
  	if(!v)
    	v=PROCAllocVar("_fcmp",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	

		switch(VSize) {
			case 4:
				break;
			case 8:
				break;
			}
		}
	else {
	switch(VSize) {
	  case 1:
      switch(Mode) {
        case MODE_IS_OTHER:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
#elif Z80
          OpA("cp",&u[0],&u[1]);
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D);
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[0],&u[1]);
#endif
          break;
        case MODE_IS_VARIABLE:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1);
#elif Z80
          OpA("cp",&u[0],&u[1]);
#elif I8086
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D);
#elif MC68000
/*					switch(VVar->classe) {
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,Regs->D);
							break;
						case CLASSE_AUTO:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO,Regs->D);
							break;
						case CLASSE_REGISTER:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
							break;
						}
						v. sotto long */
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
#endif
          break;
        case MODE_IS_CONSTANT1:
          i=VCost->l;
#if ARCHI
          if(i>=0) {            
//            *myBuf='#';
//            _tcscpy(myBuf+1,VCost->s);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO,i);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO,-i);
  		      }
#elif Z80
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("cp",i,&u[0]);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#elif I8086
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,i);
#elif MC68000
					/* come andava sta roba?? v. microchip 
					if(!l && Optimize & OPTIMIZE_CONST) {
					  if(CC == CONDIZ_UGUALE) {
				    	CC = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
						}
					else*/
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,u[0].mode,&u[0].s,u[0].ofs);
					// se=0 si potrebbe usare TST ma non è chiaro se tocca tutti i flag come CMP
#elif MICROCHIP
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("XORLW",i,&u[0]);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#endif
          break;
        case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//if(RCost->l == 0)		// :)
//						break; 
						}
          i=RCost->l;
#if ARCHI
          if(i>=0) {            
//            *myBuf='#';
//            _tcscpy(myBuf+1,VCost->s);
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,i);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,-i);
  		      }
#elif Z80
          if(i || (VType & VARTYPE_UNSIGNED))
            OpA("cp",&u[0],i);
          else {
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
            PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
            }  
#elif I8086
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO8,i);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i,u[0].mode,&u[0].s,u[0].ofs);
					// se=0 si potrebbe usare TST ma non è chiaro se tocca tutti i flag come CMP
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
      switch(Mode) {
        case MODE_IS_OTHER:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1);
#elif Z80
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[1].mode,&u[1].s,u[1].ofs);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
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
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",&u[1]);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	}
#endif
          break;
        case MODE_IS_VARIABLE:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1);
#elif Z80
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[1].mode,&u[1].s,u[1].ofs);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[1].mode,&u[1].s,u[1].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	}
#elif I8086
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
#elif MC68000
/*					switch(VVar->classe) {
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO16,Regs->D);
							break;
						case CLASSE_AUTO:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO16,Regs->D);
							break;
						case CLASSE_REGISTER:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
							break;
						} v. sotto long
						*/
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
#elif MICROCHIP
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",&u[1]);
			    	}
			    IncOp(&u[0]);
			    IncOp(&u[1]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
			    	}
#endif
          break;
        case MODE_IS_CONSTANT1:
          i=VCost->l;
#if ARCHI
          if(i>=0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,i);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,-i);
  		      }
#elif Z80
					if(!i) {
					  if(CC >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) | 4);		// CC= CC + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[0].mode,&u[0].s,u[0].ofs);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i >> 8);
					  if(CC & 4) {             // gestisco == e !=
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
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,i);
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO16,i);
#elif MC68000
					/* come andava sta roba?? v. microchip 
					if(!l && Optimize & OPTIMIZE_CONST) {
					  if(CC == CONDIZ_UGUALE) {
				    	CC = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
						}
					else*/
//      		PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.b",OPDEF_MODE_IMMEDIATO,i,OPDEF_MODE_REGISTRO_HIGH8,3);
					PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1],&u[0]);
#elif MICROCHIP
					if(!i) {
					  if(CC >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) | 4);		// CC= CC + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[0].mode,&u[0].s,u[0].ofs);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[0].mode,&u[0].s,u[0].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[0].mode,&u[0].s,u[0].ofs);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i >> 8);
					  if(CC & 4) {             // gestisco == e !=
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
        case MODE_IS_CONSTANT2:
					if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//if(RCost->l == 0)		// :)
//						break; 
						}
          i=RCost->l;
#if ARCHI
          if(i>=0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,i);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,-i);
  		      }
#elif Z80
					if(!i) {
					  if(CC >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ^ 1) | 4);		//  CC= (CC ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,i);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO,i);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,i);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
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
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO16,i);
#elif MC68000
      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,i,u[0].mode,&u[0].s,u[0].ofs);
					// se=0 si potrebbe usare TST ma non è chiaro se tocca tutti i flag come CMP
#elif MICROCHIP
					if(!i) {
					  if(CC >= CONDIZ_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",u[0].mode,&u[0].s,u[0].ofs);
		      		   // confronto = a 0
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+1);  // mancherebbe unsigned...
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ^1) | 4);//						  CC= (CC ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
					    IncOp(&u[0]);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
	      		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,i);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
				    	}
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,i);
				    	}
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
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
      switch(Mode) {
        case MODE_IS_OTHER:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);

#elif Z80
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1]);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
#elif I8086
				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO16,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO16,Regs->D+1,u[2].mode,&u[2].s,u[2].ofs);
#elif MC68000
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
#endif
					break;
        case MODE_IS_VARIABLE:
#if ARCHI
          PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);

#elif Z80
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"sub",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,&u[1]);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
#elif I8086
				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO16,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
		    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO16,Regs->D+1,u[2].mode,&u[2].s,u[2].ofs);
#elif MC68000
/*					switch(VVar->classe) {
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO32,Regs->D);
							break;
						case CLASSE_AUTO:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO32,Regs->D);
							break;
						case CLASSE_REGISTER:
							PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label),u[1].mode,&u[1].s,u[1].ofs);
							break;
						}
					POTREBBE andare ma la condizione va invertita!*/
				  PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[2].mode,&u[2].s,u[2].ofs);
				    FNGetLabel(myBuf,1);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else if(CC & 2) {     // per >, <=
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
			    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
			    	}
			    else {	
			      PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",u[2].mode,&u[2].s,u[2].ofs);
			      }
			    IncOp(&u[0]);
			    IncOp(&u[2]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",&u[1]);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[2].mode,&u[2].s,u[2].ofs);
				    }
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
			    IncOp(&u[1]);
			    IncOp(&u[3]);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
				  if(CC & 4) {             // gestisco == e !=
				    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[3].mode,&u[3].s,u[3].ofs);
				    PROCOutLab(myBuf);
				    }
				  else {
				    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,u[3].mode,&u[3].s,u[3].ofs);
				    }
#endif
          break;
        case MODE_IS_CONSTANT1:
#if ARCHI
          i=VCost->l;
          if(i>0) {            
            PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,VCost->l);
            }
          else {                        
  		      PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,-VCost->l);
  		      }
#elif Z80
          l=VCost->l;
myCPl:		                        
					if(!l) {
					  if(CC >= CONDIZ_UGUALE) {
				    	CC = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"bit",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+3);
				    	CC = (enum OPERANDO_CONDIZIONALE)(((int)CC ) | 4);		// CC= CC + 4;    // >= diventa nz, < diventa z, spero...
						  }
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO,0);
					    }  
					  }
					else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[2].mode,&u[2].s,u[2].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"scf",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^ 2);
				    	}
				    else {	
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
					    }
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    }
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    }
				    IncOp(&u[1]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    }
				    }
#elif I8086
          l=VCost->l;
myCPl:		                        
				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
		    	PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,HIWORD(l));
//      		PROCOper(LINE_TYPE_ISTRUZIONE,AS,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_IMMEDIATO16,i);
//			    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,LOWORD(l));
//			    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,HIWORD(l));
#elif MC68000
          l=VCost->l;
myCPl:		                        
					/* come andava sta roba?? v. microchip 
					if(!l && Optimize & OPTIMIZE_CONST) {
					  if(CC == CONDIZ_UGUALE) {
				    	CC = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
						}
					else*/
				    PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO32,l,u[0].mode,&u[0].s,u[0].ofs);
#elif MICROCHIP
          l=VCost->l;
myCPl:		                        
					if(!l) {
					  if(CC >= CONDIZ_UGUALE) {
				    	CC = (enum OPERANDO_CONDIZIONALE)1;	//						       // confronto = a 0, forzo GenCondBranch(1)
					    }
					  else if(CC==CONDIZ_MINORE || CC==CONDIZ_MAGGIORE_UGUALE) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"BTFSS",OPDEF_MODE_IMMEDIATO,7,u[0].mode,&u[0].s,u[0].ofs+3);
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ^1) | 4);//						  CC= (CC ^1) + 4;    // >= diventa z, < diventa nz
						  }
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO8,0);
					    }  
					  }
					else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",u[2].mode,&u[2].s,u[2].ofs);
					    FNGetLabel(myBuf,1);
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else if(CC & 2) {     // per >, <=
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"BSF STATUS,C",OPDEF_MODE_NULLA,0);
				    	PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
				    	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) ^2);
				    	}
				    else {	
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO,LOBYTE(LOWORD(l)));
					    }
				    IncOp(&u[0]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,*(((char *)&l)+1));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
					    }
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO,*(((char *)&l)+2));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
					    }
				    IncOp(&u[1]);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[1].mode,&u[1].s,u[1].ofs);
					  if(CC & 4) {             // gestisco == e !=
					    PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&myBuf,0);
					    }
					  else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
					    }
				    }
#endif
          break;

        case MODE_IS_CONSTANT2:
#if ARCHI
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo or/xor?					if(RCost->l == 0)		// :)
//						break; 
						}
          if(RCost->l>0) {
        		PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,RCost->l);
            }
          else {
        		PROCOper(LINE_TYPE_ISTRUZIONE,"CMN",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,-RCost->l);
            }
#elif Z80
						if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo or/xor?					if(RCost->l == 0)		// :)
//						break; 
						}
          l=RCost->l;
          goto myCPl;
#elif I8086
					if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//					if(RCost->l == 0)		// :)
//						break; 
						}
          l=RCost->l;
          goto myCPl;
#elif MC68000
					if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo or/xor?					if(RCost->l == 0)		// :)
//						break; 
						}
          l=RCost->l;
          goto myCPl;
#elif MICROCHIP
					if(Optimize & OPTIMIZE_CONST) {		// :)
							//if(Optimize   fare solo se?
//occhio qua, solo or/xor?					if(RCost->l == 0)		// :)
//						break; 
						}
          l=RCost->l;
          goto myCPl;
#endif
          break;
        }
      break;
    }
		}

//  if(VType & VARTYPE_UNSIGNED)
//   	CC = (enum OPERANDO_CONDIZIONALE )(((int)CC ) | VALUE_CONDITION_UNSIGNED);
//    CC |= 0x20;  

  return CC;
  }

int Ccc::subInc(bool m, int16_t *cond, uint8_t prePost, int8_t VQ, struct VARS *VVar, uint8_t qty, O_TYPE VType, O_SIZE VSize, 
								struct OP_DEF *u, struct OP_DEF *u1,uint8_t isPtr) {
	char TS[16],MyBuf1[64];
	int i;

  if(m)
#if ARCHI
    _tcscpy(TS,"ADD");
#elif Z80 || I8086 || MC68000 || I8051 || MICROCHIP
    _tcscpy(TS,incString);
#endif
  else 
#if ARCHI
    _tcscpy(TS,"SUB");
#elif Z80 || I8086 || MC68000 || I8051 || MICROCHIP
    _tcscpy(TS,decString);
#endif

	VSize=FNGetMemSize(VVar,1);

#if ARCHI
	char loadString2[16],storString2[16];
	_tcscpy(loadString2,loadString);
	_tcscpy(storString2,storString);
	switch(VSize) {
		case 1:
		case 2:
			_tcscat(loadString2,"B");
			_tcscat(storString2,"B");
			break;
		case 4:
			break;
		}   
#elif MC68000
	char movString2[16];
	_tcscpy(movString2,movString);
	switch(VSize) {
		case 1:
			_tcscat(movString2,".b");
			_tcscat(TS,".b");
			break;
		case 2:
			_tcscat(movString2,".w");
			_tcscat(TS,".w");
			break;
		case 4:
			_tcscat(movString2,".l");
			_tcscat(TS,".l");
			break;
		}   
/*										switch(qty) {
		case 1:
			_tcscat(TS,"\t#1,");
			break;
		case 2:
			_tcscat(TS,"\t#2,");
			break;
		case 4:
			_tcscat(TS,"\t#4,");
			break;
		}*/

#elif I8086
	switch(VSize) {
		case 1:
			_tcscat(TS," BYTE PTR ");
			break;
		case 2:
			_tcscat(TS," WORD PTR ");
			break;
		case 4:
			_tcscat(TS," DWORD PTR ");		// verificare! CPU86 ecc
			break;
		}   
#endif

  if(VType & VARTYPE_FLOAT) {
		struct VARS *v;
		if(m) {
			v=FNCercaVar("_fadd",0);
  		if(!v)
    		v=PROCAllocVar("_fadd",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
			}
		else {
			v=FNCercaVar("_fsub",0);
  		if(!v)
    		v=PROCAllocVar("_fsub",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
			}
		if(VQ==VALUE_IS_VARIABILE) {
			switch(VVar->classe) {
				case CLASSE_EXTERN:
				case CLASSE_GLOBAL:
				case CLASSE_STATIC:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
#endif
					break;
		    case CLASSE_AUTO:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label),OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
#endif
					break;
		    case CLASSE_REGISTER:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
	        PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
#endif
					break;
				}
			}
		else {		// D0
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
#endif
			}

// leggere costante 1 		
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"__1",0,OPDEF_MODE_REGISTRO32,1);
    PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&v,0);

		switch(VSize) {
			case 4:
				break;
			case 8:
				break;
			}

		if(VQ==VALUE_IS_VARIABILE) {
			switch(VVar->classe) {
				case CLASSE_EXTERN:
				case CLASSE_GLOBAL:
				case CLASSE_STATIC:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
#elif MICROCHIP
#endif
					break;
		    case CLASSE_AUTO:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(VVar->label));
#elif MICROCHIP
#endif
					break;
		    case CLASSE_REGISTER:
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
	        PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label));
#elif MICROCHIP
#endif
					break;
				}
			}
		else {		// D0
#if ARCHI
#elif Z80 			
#elif I8086
#elif MC68000
      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
#elif MICROCHIP
#endif
			}
		}		// float
	else {
	if(VQ==VALUE_IS_VARIABILE) {
	  switch(VVar->classe) {
	    case CLASSE_EXTERN:
	    case CLASSE_GLOBAL:
	    case CLASSE_STATIC:
//    	                  _tcscpy(AS,(V->var)->label);
#if ARCHI
				char T1S[64],AS[64];
				*T1S=0;
				*AS=0;
	      switch(VSize) {
					// c'era BS in tutti!! cos'era??   ev OPDEF_MODE_SHIFT
	        case 1:
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString,OPDEF_MODE_REGISTRO,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          break;
	        case 2:
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString,OPDEF_MODE_REGISTRO,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString,OPDEF_MODE_REGISTRO,Regs->D+1,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,1);
	          PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1,
							OPDEF_MODE_SHIFT,+8);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_SHIFT,-8);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,1);
	          break;
	        case 4:  
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString2,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	          break;
	        case 8:
	          break;
	        default:
	          break;
	        }
#elif Z80 			
	      switch(VSize) {
	        case 1:
//	                          FNGetFPStr(MyBuf,0,AS);
/*                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,2,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,1,Regs->D,2,3);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,2,3);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->var,0,2,3);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,1,Regs->D,2,3);
 	            */
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,0);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,0);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,0);
	          break;
	        case 2:  
//	                          FNGetFPStr(MyBuf,0,AS);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,&u[0].s,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==2) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,&u[0].s,0);
              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,&u[0].s,0);
 	            }
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,&u[0].s,0);
            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,&u[0].s,0);
	          break;
	        case 4:// non ritorna pre-inc
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,2);
 	          if(*TS=='i') {
 	            i=qty;
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
              }  
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          if(*TS=='d') {
	            i=qty;
     	        while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
              }  
            FNGetLabel(MyBuf1,2);  
 	          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
    	      i=qty;
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	          PROCOutLab(MyBuf1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,2,OPDEF_MODE_REGISTRO,Regs->D+1);
	          break;
	        default:
	          break;
	        }
#elif I8086
	      switch(VSize) {
	        case 1:
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
 	          while(qty--)  
	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
						break;
	        case 2:  
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
 	          while(qty--)  
	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
	          break;
	        case 4:
  if(CPU86>=3)
		;
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,2);
 	          if(*TS=='i') {
 	            i=qty;
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
              }  
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          if(*TS=='d') {
	            i=qty;
     	        while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
              }  
            FNGetLabel(MyBuf1,2);  
 	          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
    	      i=qty;
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	          PROCOutLab(MyBuf1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,2,OPDEF_MODE_REGISTRO,Regs->D+1);
	          break;
	        case 8:
	          break;
	        default:
	          break;
	        }
#elif MC68000
	      switch(VSize) {
	        case 1:
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO8,Regs->D);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO8,Regs->D);
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D);			// per flag qua...
						break;
	        case 2:  
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO16,Regs->D);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO16,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 4:
            if(prePost==2)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO32,Regs->D);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&VVar->label,0);
            if(prePost==1)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&VVar->label,0,OPDEF_MODE_REGISTRO32,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
						// se LValue verrà copiato da Dn in An dopo
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 8:
	          break;
	        default:
	          break;
	        }
#elif MICROCHIP
	      switch(VSize) {
	        case 1:
						if(VVar->type & VARTYPE_ROM) {
							PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
							}
						else {
//															PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							// NO, NON SERVE!
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLB high",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							if(prePost==2)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
 							if(qty==1)  
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,qty);
								PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='qty' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
								}
							if(prePost==1)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							}
						break;
					case 2:  // FINIRE!
						if(VVar->type & VARTYPE_ROM) {
							PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLB high",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							if(prePost==2)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
 							if(qty==1)  
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,qty);
								PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='qty' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
								}
	            FNGetLabel(MyBuf1,2);  
 		          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
							// C18 usa ADDWFC ...; opp. usare INFSNZ
 							if(qty==1)  
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,1);
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,qty);
								PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='qty' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,1);
								}
	 	          PROCOutLab(MyBuf1);
							if(prePost==1)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							}
	          break;
					case 4:// FINIRE!
						if(VVar->type & VARTYPE_ROM) {
							PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							if(prePost==2)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
 							while(qty--)  
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,9);
							if(prePost==1)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
							}
						if(VVar->type & VARTYPE_ROM) {
							PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
							if(prePost==2)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
 							while(qty--)  
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,9);
							if(prePost==1)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
							}
	          break;
	        case 8:
	          break;
	        default:
	          break;
	        }
#endif
	      break;
	    case CLASSE_AUTO:
	      i=MAKEPTROFS(VVar->label);
#if ARCHI
	      switch(VSize) {
					// c'era BS in tutte... cos'era??  ev OPDEF_MODE_SHIFT
	        case 1:                        
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          break;
	        case 2:
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString2,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	          PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D+1,
							OPDEF_MODE_SHIFT,+8);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO8,Regs->D,
							OPDEF_MODE_SHIFT,-8);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	          break;
	        case 4:
	          PROCOper(LINE_TYPE_ISTRUZIONE,loadString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO32,Regs->D,
							OPDEF_MODE_IMMEDIATO8,qty);
	          PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          break;
	        case 8: 
	          break;
	        default:
	          break;
	        }
#elif Z80
	      switch(VSize) {
	        case 1:
//	                          FNGetFPStr(MyBuf,i,NULL);
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          break;
	        case 2:
	        /*
 	          PROCOper(movString,BS+1,FNGetFPStr(MyBuf,i));
 	          PROCOper(movString,B1S,FNGetFPStr(MyBuf,i+1));
            if(prePost==2) {
 	            PROCOper(movString,Regs->DSl,BS+1);
 	            PROCOper(movString,Regs->DSh,B1S);
 	            }
 	          while(qty--)  
              PROCOper(TS,BS);
 	          PROCOper(movString,FNGetFPStr(MyBuf,i,NULL),BS+1);
 	          PROCOper(movString,FNGetFPStr(MyBuf,i+1,NULL),B1S);
 	          */
 	          
            if(prePost==2) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	            }
 	          while(qty--) { 
// 	                            FNGetFPStr(MyBuf,i,NULL);
 	            if(*TS=='d')
 	              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	            if(*TS=='d')
 	              PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
              PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	            }
 	          if(prePost==1) {
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	 	          }
	          break;
	        case 4:// non ritorna pre-inc
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
 	          if(*TS=='i')
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          if(*TS=='d')
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	          PROCOutLab(MyBuf1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	          break;
	        default:
	          break;
	        }
#elif I8086	                        
	      switch(VSize) {
	        case 1:                        
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          break;
	        case 2:
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          while(qty--)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          if(prePost==1)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	          break;
	        case 4:
  if(CPU86>=3)
		;
            if(prePost==2) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
							}
 	          if(*TS=='i') {
   	          while(qty--)  
	 	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							}
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1);
 	          if(*TS=='d') {
   	          while(qty--)  
	 	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							}
 	          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	          while(qty--)  
 	            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          PROCOutLab(MyBuf1);
            if(prePost==1) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
							}
	          break;
	        case 8: 
	          break;
	        default:
	          break;
	        }
#elif MC68000	                        
	      switch(VSize) {
	        case 1:                        
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO8,Regs->D);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO8,Regs->D);
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D);
	          break;
	        case 2:
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,Regs->D);
//                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          if(prePost==1)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 4:
            if(prePost==2)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,Regs->D);
            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
//                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          if(prePost==1)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
						// se LValue verrà copiato da Dn in An dopo
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 8: 
	          break;
	        default:
	          break;
	        }
#elif MICROCHIP
	      switch(VSize) {
	        case 1:
//	                          FNGetFPStr(MyBuf,i,NULL);
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO8,0,OPDEF_MODE_IMMEDIATO,i);
            if(prePost==2)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
 	          while(qty--) {
// 															if(qty==1)			// qui non posso usare W (e non so nemmeno se converrebbe fare ADD)
								PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
//															else {
//																PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,qty);
//																PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='qty' ? "ADDWF" : "SUBWF",OPDEF_MODE_REGISTRO,9);
//																}
								}

//                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
            if(prePost==1)
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
	          break;
	        case 2:
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,i);
            if(prePost==2) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
//FINIRE! 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	            }
 	          while(qty--) { 
// 	                            FNGetFPStr(MyBuf,i,NULL);
 	            if(*TS=='d')
 	              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
 	            if(*TS=='d')
 	              PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
              PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
 	            }
 	          if(prePost==1) {
 	            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
//	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	 	          }
	          break;
	        case 4:// non ritorna pre-inc
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
 	          if(*TS=='i')
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          if(*TS=='d')
   	          while(qty--)  
                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	          while(qty--)  
              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	          PROCOutLab(MyBuf1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	          break;
	        default:
	          break;
	        }
#endif	                        
	      break;
	    case CLASSE_REGISTER:
#if ARCHI
	      switch(VSize) {
	        case 1:
						if(prePost) {                 
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,
								OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/);
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO8,Regs->D,
								OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,0);
							}
						break;
	        case 2:
						if(prePost) {                 
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,
								OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label)/*Regs[VVar]*/);
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,
								OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,0);
							}
						break;
	        case 4:
						if(prePost) {                 
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D,
								OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/);
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,
								OPDEF_MODE_IMMEDIATO8,qty);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D,
								OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label)/*Regs[VVar]*/,0);
							}
						break;
					}
#elif Z80
	      switch(VSize) {
	        case 1:
						if(prePost==2) {                 
							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
							PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,Regs->D);
							}
						while(qty--)  
							 PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_LOW8,MAKEPTRREG(VVar->label));
						if(prePost==1) {
							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
							PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,Regs->D);
							}
	          break;
	        case 2:
						if(prePost==2) {                 
							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
							PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,Regs->D);
							}
						while(qty--)  
							 PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
						if(prePost==1) {
							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
							PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,Regs->D);
							}
	          break;
	        case 4:// 
	          break;
	        default:
	          break;
	        }
#elif I8086
	      switch(VSize) {
	        case 1:                        
						if(prePost==2)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,
								OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label));
						while(qty--)  
							 PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_LOW8,MAKEPTRREG(VVar->label));
						if(prePost==1)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,
								OPDEF_MODE_REGISTRO_LOW8,MAKEPTRREG(VVar->label));
	          break;
	        case 2:
						if(prePost==2)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
						while(qty--)  
							 PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
						if(prePost==1)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,isPtr ? Regs->P :	Regs->D,
								OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
	          break;
	        case 4:
  if(CPU86>=3)
		;
	else
			// non dovrebbe esistere, direi
			;
	          break;
	        case 8: 
	          break;
	        default:
	          break;
	        }
#elif MC68000
	      switch(VSize) {
	        case 1:
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label),
								OPDEF_MODE_REGISTRO8,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label));
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,MAKEPTRREG(VVar->label),
								OPDEF_MODE_REGISTRO8,Regs->D);
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D);			// per flag qua...
						break;
	        case 2:  
            if(prePost==2)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO16,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label));
            if(prePost==1)
	            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO16,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 4:
            if(prePost==2)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO32,Regs->D);
		        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label));
            if(prePost==1)
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(VVar->label),OPDEF_MODE_REGISTRO32,
								(isPtr && isRValue) ? Regs->P :	Regs->D);	// per flag qua...
						// se LValue verrà copiato da Dn in An dopo
            if(prePost==2 && (isRValue || (*cond & 0xff)))
	 	          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,
								isPtr ? Regs->P :	Regs->D);	// per flag qua...
	          break;
	        case 8:
	          break;
	        default:
	          break;
	        }
#elif MICROCHIP
	      if(prePost==2) {                 
	        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label));
	        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	        }
        while(qty--)  
           PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label));
	      if(prePost==1) {
	        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(VVar->label));
	        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	        }
#endif
	      break;
	    }
	  }			// VALUE_IS_VARIABLE

	else {		// D0
#if ARCHI
		struct LINE *t;
		char *p1;
		char T1S[64],AS[64];;
		*T1S=0;
		*AS=0;
	  _tcscpy(AS,LastOut->s1.s.label);
	  p1=strchr(AS,',');
		if(p1)			// non c'era, 2025
			*(p1+1)=0;
	  t=LastOut;
	  LastOut=LastOut->prev;
	  PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D,T1S);
	  switch(qty) {
	    case 1:
	      PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D+1,
					OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	      break;
	    case 2:
	      PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D+1,
					OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV"/*movString*/,OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_REGISTRO8,Regs->D+1,
					OPDEF_MODE_SHIFT,-8);
	      PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO8,Regs->D+1,
					OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,1);
	      break;
	    case 4:
	      PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO32,Regs->D+1,
					OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&VVar->label,0);
	      break;
	    case 8: 
	      break;
	    default:
	      break;
	    }
	  LastOut=t;
#elif Z80
	  u[0].s.n=(int)VVar->func;
	  if(u[0].s.n <= 3 && (int)VVar->parm) {
		  u[0].mode=OPDEF_MODE_REGISTRO;
	    u[0].ofs=0;
			Op2A("add",&u[0],(int)VVar->parm,0);
	    }
	  else { 
	    u[0].ofs=(int)VVar->parm;
	    }
	  u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	  switch(VSize) {
	    case 1:
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(prePost==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	      break;
	    case 2:
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        while(qty--) { 
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      }
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    case 4:
	    PROCError(1001,"++ long");
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    default:
	      break;
	    }
#elif I8086
	  u[0].s.n=(int)VVar->func;
	  if(u[0].s.n <= 3 && (int)VVar->parm) {
		  u[0].mode=OPDEF_MODE_REGISTRO;
	    u[0].ofs=0;
//											Op2A("add",&u[0],(int)VVar->parm,0);
	    }
	  else { 
	    u[0].ofs=(int)VVar->parm;
	    }
	  u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	  switch(VSize) {
	    case 1:
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(prePost==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	      break;
	    case 2:
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        while(qty--) { 
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      }
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    case 4:
  if(CPU86>=3)
		;
	else
	    PROCError(1001,"++ long");
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    default:
	      break;
	    }
#elif MC68000
	  u[0].s.n=(int)VVar->func;
	  if(u[0].s.n <= 3 && (int)VVar->parm) {
		  u[0].mode=OPDEF_MODE_REGISTRO;
	    u[0].ofs=0;
//											Op2A("add",&u[0],(int)VVar->parm,0);
	    }
	  else { 
	    u[0].ofs=(int)VVar->parm;
	    }
	  u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	  switch(VSize) {
	    case 1:
        if(prePost==2)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
//	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)VVar->func);
        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,qty,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
        if(prePost==1)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO8,Regs->D);
//          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO,Regs->D);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D);			// per flag qua...
	      break;
	    case 2:
        if(prePost==2)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO16,qty,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
        if(prePost==1)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO16,Regs->D);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D);			// per flag qua...
	      break;
	    case 4:
        if(prePost==2)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO32,qty,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
        if(prePost==1)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);			// per flag qua...
	      break;
	    default:
	      break;
	    }
#elif MICROCHIP
	  u[0].s.n=(int)VVar->func;
	  if(u[0].s.n <= 3 && (int)VVar->parm) {
		  u[0].mode=OPDEF_MODE_REGISTRO;
	    u[0].ofs=0;
			Op2A("add",&u[0],(int)VVar->parm,0);
	    }
	  else { 
	    u[0].ofs=(int)VVar->parm;
	    }
	  u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	  switch(VSize) {
	    case 1:
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(prePost==1)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
        if(prePost==2)
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	      break;
	    case 2:
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        while(qty--) { 
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
	        if(*TS=='d')
	          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
		      }
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    case 4:
	    PROCError(1001,"++ long");
        if(prePost==2) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(uint8_t)VVar->func);
        if(*TS=='d')
          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
	      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
        if(prePost==1) {
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(uint8_t)VVar->func);
          }
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	      break;
	    default:
	      break;
	    }
#endif
	  }
	  }

  if(OutSource) {
//    i=_tcslen(LastOut->s)+_tcslen(V->name)+20;
//    PROCOut(NULL,"\t\t\t\t; ",V->name,NULL,NULL);	
//    LastOut=(struct LINE *)_frealloc(LastOut,i);
//    LastOut->prev->next=LastOut;
//    _tcscat(LastOut->s,"\t\t; ");
		if(VQ==VALUE_IS_VARIABILE)		// dovrebbe andare sopra... esce alla riga dopo se c'è lettura pre o post, ma ok
	    _tcscpy(LastOut->rem,VVar->name);
    }

	return VALUE_IS_EXPR;
	}


