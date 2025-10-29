#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

/*int subAdd(int, int, int, struct VARS *,long *, int *, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *,int);
int subShift(int, int, long, int, long, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
int subMul(char, int, int, struct VARS *, long, int, int, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
int subAOX(char, int *, int, int, struct VARS *, long, int, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
int subCMP(char *, int, int, int, struct VARS *, long, int, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);*/

#pragma warning FARE COME IN AS!! 2024

void Ccc::subEvEx(int Pty, int *cond, char *Clabel,long *VType, int *VSize, int *VQ, struct VARS * *VVar, union STR_LONG *VCost, struct TAGS * *VTag, int *VDim) {
  
  FNRev(Pty,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
	if((*VQ & 0x80) && Pty>=14)		// se condizionale e livello + esterno, esco 2025
		return;
  if((*VQ & 0xf) == 2) {
#if MICROCHIP
    PROCReadD0(*VVar,0,0,*cond & 0xff,0);
#else
    PROCReadD0(*VVar,0,0,*cond & 0xff);
#endif
		}
  if((*VQ & 0xf) == 3) {
#if MICROCHIP
    ReadVar(*VVar,0,0,*cond & 0xff,0);
#else
    ReadVar(*VVar,0,0,*cond & 0xff);
#endif
		}
  if(*VQ & 8) {			// VALUE_IS_COSTANTE
#if MICROCHIP
    PROCUseCost(*VQ,*VType,*VSize,VCost,0);		// FINIRE
#else
    PROCUseCost(*VQ,*VType,*VSize,VCost);
#endif
		}
  }
 
int Ccc::FNEvalExpr(int Pty, char *C) {
  int S=0,D=0,VQ,i;
  struct VARS VPtr;
  struct VARS *V=&VPtr;
  struct TAGS *R;
  long T;
  char Clabel[32];
  
  GlblOut=LastOut;
  VQ=0;
  *C=0;
  TempProg=0;
  i=0;
  subEvEx(Pty,&i,Clabel,&T,&S,&VQ,&V,(union STR_LONG *)C,&R,&D);
// VQ: -5 niente                      	0
//     0 per valore gen. in D0 o hl   	1
//     1 per valore da ptr in D0 o hl 	2 
//     2 variabile in *V              	3
//     -1 per costante integral       	8
//     -2 per altra costante          	9
//     -9 per ! condizionale      (bit 4)
//     -10..-15 per condizionale  (bit 7)
//     -20..-25 per condizionale multipla && ||   (bit 6)
// bit 5 indica comparazione tra signed (0) o unsigned (1)
  
  return S;                  // ritorno size espressione
  }
 
int Ccc::FNEvalECast(char *C, long *T, int/*uint8_t*/ *S) {
  int s=0,D=0,VQ,i;
  struct VARS VPtr;
  struct VARS *V=&VPtr;
  struct TAGS *R;
  long t;
  char Clabel[32];
  
  GlblOut=LastOut;
  VQ=0;
  *C=0;
  TempProg=0;
  i=0;
  FNRev(15,&i,Clabel,&t,&s,&VQ,&V,(union STR_LONG *)C,&R,&D);
  if(*S) {                    // se ho newSize, faccio cast...
		if(VQ==3) {
#if MICROCHIP
	    ReadVar(V,*T,*S,0,0);		//FINIRE
#else
	    ReadVar(V,*T,*S,0);
#endif
			}
		else if(VQ==2) {
#if MICROCHIP
	    PROCReadD0(V,*T,*S,0,0);
#else
	    PROCReadD0(V,*T,*S,0);
#endif
			}
		else if(VQ & 8) {
#if MICROCHIP
	    PROCUseCost(VQ,*T,*S,(union STR_LONG *)C,0);		// FINIRE
#else
	    PROCUseCost(VQ,*T,*S,(union STR_LONG *)C);
#endif
			}
	  else if(VQ==1)
	    PROCCast(*T,*S,t,s);
	  }
	else {                      // altrimenti no cast e ritorno i valori T & S
		if(VQ==3) {
#if MICROCHIP
	    ReadVar(V,t,s,0,0);	// FINIRE
#else
	    ReadVar(V,t,s,0);
#endif
			}
		else if(VQ==2) {
#if MICROCHIP
	    PROCReadD0(V,t,s,0,0);
#else
	    PROCReadD0(V,t,s,0);
#endif
			}
		else if(VQ & 8) {
#if MICROCHIP
	    PROCUseCost(VQ,t,s,(union STR_LONG *)C,0);		// FINIRE
#else
	    PROCUseCost(VQ,t,s,(union STR_LONG *)C);
#endif
			}
	  *T=t;
	  *S=s;  
	  }  
  return 0;
  }
 
int Ccc::FNEvalCond(char *C, char *TS, uint8_t m) {
  int S=0,D=0,VQ,i;
  struct VARS VPtr;
  struct VARS *V=&VPtr;
  struct TAGS *R;
  long T;
  char MyBuf[128];
    
  GlblOut=LastOut;
  VQ=0;
//  *C=0;                   // gli stmt passano qui la label per && e ||
  TempProg=0;
  i=1;
  subEvEx(15,&i,C,&T,&S,&VQ,&V,(union STR_LONG *)MyBuf,&R,&D);
// aggiungo qui un Read VAR reso intelligente dal fatto che si ha un IF o un expr normale...    
  if(VQ & 8) {
    PROCWarn(4127);
// in questo caso bisognerebbe anche stroncarlo...    
    }
  D=VQ & 0xbf;      // tolgo cond. multipla
	PROCGenCondBranch(TS,m,&D,FNGetMemSize(T,S,0));
	if(VQ & 0x40) {
    PROCOutLab(C);
    return 1;
	  }
	else
	  return 0;
  }


#if Z80
void Ccc::OpA(char *s, struct OP_DEF *r, int i) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  if(*(WORD *)s=='da' || *(WORD *)s=='bs')
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
  else  
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_IMMEDIATO8,i);
  if(*(WORD *)s!='pc' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }
  
void Ccc::OpA(char *s, int i, struct OP_DEF *r) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
  if(*(WORD *)s=='da' || *(WORD *)s=='bs')
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  else  
    PROCOper(LINE_TYPE_ISTRUZIONE,s,r->mode,&r->s,r->ofs);
  if(*(WORD *)s!='pc' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }
  
void Ccc::OpA(char *s, struct OP_DEF *r, struct OP_DEF *i) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  if(i) {
	  if(*(WORD *)s=='da' || *(WORD *)s=='bs')
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,i->mode,&i->s,i->ofs);
	  else  
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,i->mode,&i->s,i->ofs);
	  }
	else {
	  if(*(WORD *)s=='da' || *(WORD *)s=='bs')
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3);
	  else  
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_NULLA,0);
	  }    
  if(*(WORD *)s!='pc' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }

void Ccc::IncOp(struct OP_DEF *u) {
  
  switch(u->mode & 0x7f) {
    case 1:
      u->mode=OPDEF_MODE_REGISTRO_HIGH8;
      break;
    case 3:
  	  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,u->s.n);
      break;
    case 8:
    case 9:
    case 10:
      u->ofs++;
//      myLog->print(0,"Incremento ofs: %d\n",u->ofs);
      break;  
    }
  }

void Ccc::DecOp(struct OP_DEF *u) {
  
  switch(u->mode & 0x7f) {
    case 2:
      u->mode=OPDEF_MODE_REGISTRO_LOW8;
      break;
    case 3:
  	  PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,u->s.n);
      break;
    case 8:
    case 9:
    case 10:
      u->ofs--;
//      myLog->print(0,"Incremento ofs: %d\n",u->ofs);
      break;  
    }
  }

void Ccc::Op2A(char *s, struct OP_DEF *r, int i, uint8_t m) {   //m=1 se si può alterare un eventuale registro in r
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8 && (!m || i>4)) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3) {

		if(i>=1 && i<=4) {     // solo se opero direttamente a 16 bit
		  r->mode=OPDEF_MODE_REGISTRO;
			while(i--) {
			  if(*s1 == 'a') {
					PROCOper(LINE_TYPE_ISTRUZIONE,incString,r->mode,&r->s,r->ofs);
				  }
				else { 
				  PROCOper(LINE_TYPE_ISTRUZIONE,decString,r->mode,&r->s,r->ofs);
				  }
			  }
			return;  
			}  
		else {			  
			r->mode=OPDEF_MODE_REGISTRO_LOW8;
			}
		}	
	if(i & 0xff) {
		OpA(s1,r,i);
		if(*s1 != 'a')
		  *(s1+1)='b';
	  *(s1+2)='c';
	  }
  IncOp(r);
	OpA(s1,r,*(((char *)&i)+1));
  }
					
void Ccc::Op2A(char *s, int i, struct OP_DEF *r, uint8_t m) {
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8 && (!m || i>4)) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3)
		r->mode=OPDEF_MODE_REGISTRO_LOW8;
	if(i & 0xff) {
		OpA(s1,i,r);
		if(*s1 != 'a')
		  *(s1+1)='b';
	  *(s1+2)='c';
	  }
  IncOp(r);
	OpA(s1,*(((char *)&i)+1),r);
  }
					
void Ccc::Op2A(char *s, struct OP_DEF *r, struct OP_DEF *i) {
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3)
		r->mode=OPDEF_MODE_REGISTRO_LOW8;
	if(i->mode<=3)
	  i->mode=OPDEF_MODE_REGISTRO_LOW8;
	OpA(s1,r,i);
	if(*s1 != 'a')
	  *(s1+1)='b';
  *(s1+2)='c';
  IncOp(r);
  IncOp(i);
	OpA(s1,r,i);
	}
					
#endif  
#if MICROCHIP
void Ccc::OpA(char *s, struct OP_DEF *r, int i) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  if(*(WORD *)s=='DA' || *(WORD *)s=='US')
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
  else  
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_IMMEDIATO8,i);
  if(*(WORD *)s!='PC' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }
  
void Ccc::OpA(char *s, int i, struct OP_DEF *r) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_IMMEDIATO8,i);
  if(*(WORD *)s=='DA' || *(WORD *)s=='US')
    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  else  
    PROCOper(LINE_TYPE_ISTRUZIONE,s,r->mode,&r->s,r->ofs);
  if(*(WORD *)s!='PC' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }
  
void Ccc::OpA(char *s, struct OP_DEF *r, struct OP_DEF *i) {

  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,r->mode,&r->s,r->ofs);
  if(i) {
	  if(*(WORD *)s=='DA' || *(WORD *)s=='BS')
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3,i->mode,&i->s,i->ofs);
	  else  
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,i->mode,&i->s,i->ofs);
	  }
	else {
	  if(*(WORD *)s=='DA' || *(WORD *)s=='BS')
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_REGISTRO_HIGH8,3);
	  else  
	    PROCOper(LINE_TYPE_ISTRUZIONE,s,OPDEF_MODE_NULLA,0);
	  }    
  if(*(WORD *)s!='PC' || *(s+2))
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,r->mode,&r->s,r->ofs,OPDEF_MODE_REGISTRO_HIGH8,3);
  }

void Ccc::IncOp(struct OP_DEF *u) {
  
  switch(u->mode & 0x7f) {
    case 1:
      u->mode=OPDEF_MODE_REGISTRO_HIGH8;
      break;
    case 3:
  	  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,u->s.n);
      break;
    case 8:
    case 9:
    case 10:
      u->ofs++;
//      myLog->print(0,"Incremento ofs: %d\n",u->ofs);
      break;  
    }
  }

void Ccc::DecOp(struct OP_DEF *u) {
  
  switch(u->mode & 0x7f) {
    case 2:
      u->mode=OPDEF_MODE_REGISTRO_LOW8;
      break;
    case 3:
  	  PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,u->s.n);
      break;
    case 8:
    case 9:
    case 10:
      u->ofs--;
//      myLog->print(0,"Incremento ofs: %d\n",u->ofs);
      break;  
    }
  }

void Ccc::Op2A(char *s, struct OP_DEF *r, int i, uint8_t m) {   //m=1 se si può alterare un eventuale registro in r
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8 && (!m || i>4)) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3) {

		if(i>=1 && i<=4) {     // solo se opero direttamente a 16 bit
		  r->mode=OPDEF_MODE_REGISTRO;
			while(i--) {
			  if(*s1 == 'a') {
					PROCOper(LINE_TYPE_ISTRUZIONE,incString,r->mode,&r->s,r->ofs);
				  }
				else { 
				  PROCOper(LINE_TYPE_ISTRUZIONE,decString,r->mode,&r->s,r->ofs);
				  }
			  }
			return;  
			}  
		else {			  
			r->mode=OPDEF_MODE_REGISTRO_LOW8;
			}
		}	
	if(i & 0xff) {
		OpA(s1,r,i);
		if(*s1 != 'a')
		  *(s1+1)='b';
	  *(s1+2)='c';
	  }
  IncOp(r);
	OpA(s1,r,*(((char *)&i)+1));
  }
					
void Ccc::Op2A(char *s, int i, struct OP_DEF *r, uint8_t m) {
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8 && (!m || i>4)) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3)
		r->mode=OPDEF_MODE_REGISTRO_LOW8;
	if(i & 0xff) {
		OpA(s1,i,r);
		if(*s1 != 'a')
		  *(s1+1)='b';
	  *(s1+2)='c';
	  }
  IncOp(r);
	OpA(s1,*(((char *)&i)+1),r);
  }
					
void Ccc::Op2A(char *s, struct OP_DEF *r, struct OP_DEF *i) {
  char s1[8];

	_tcscpy(s1,s);
	if(r->s.n >= 8) {
    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,r->s.n);
    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
    r->s.n=Regs->D;
	  }
	if(r->mode<=3)
		r->mode=OPDEF_MODE_REGISTRO_LOW8;
	if(i->mode<=3)
	  i->mode=OPDEF_MODE_REGISTRO_LOW8;
	OpA(s1,r,i);
	if(*s1 != 'a')
	  *(s1+1)='b';
  *(s1+2)='c';
  IncOp(r);
  IncOp(i);
	OpA(s1,r,i);
	}
					
#endif  


#if ARCHI
void Ccc::subSpezReg(int S, struct OP_DEF *u) {

  _tcscpy(d[0].Dr,Regs->DS);
  _tcscpy(d[1].Dr,Regs->D1S);
  if(S>2) {
	  _tcscpy(d[2].Dr,Regs->D2S);
	  _tcscpy(d[3].Dr,Regs->D3S);
    }
  }
#elif Z80
void Ccc::subSpezReg(int S, struct OP_DEF *u) {

  u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[0].s.n=Regs->D;
  u[1].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[1].s.n=Regs->D+1;
  if(S>2) {
	  u[2].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[2].s.n=Regs->D;
	  u[3].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[3].s.n=Regs->D+1;               // pacco... ri uso gli stessi
    }
  }
#elif I8086  
void Ccc::subSpezReg(int S, struct OP_DEF *u) {

  u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[0].s.n=Regs->D;
  u[1].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[1].s.n=Regs->D+1;
  if(S>2) {
	  u[2].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[2].s.n=Regs->D;
	  u[3].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[3].s.n=Regs->D+1;
    }
  }
#elif MC68000
void Ccc::subSpezReg(int S, struct OP_DEF *u) {

  u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[0].s.n=Regs->D;
  u[1].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[1].s.n=Regs->D+1;
  if(S>2) {
	  u[2].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[2].s.n=Regs->D;
	  u[3].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[3].s.n=Regs->D+1;
    }
  }
#elif MICROCHIP
void Ccc::subSpezReg(int S, struct OP_DEF *u) {

  u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[0].s.n=Regs->D;
  u[1].mode=OPDEF_MODE_REGISTRO_LOW8;
  u[1].s.n=Regs->D+1;
  if(S>2) {
	  u[2].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[2].s.n=Regs->D;
	  u[3].mode=OPDEF_MODE_REGISTRO_LOW8;
	  u[3].s.n=Regs->D+1;               // pacco... ri uso gli stessi
    }
  }
#endif

int Ccc::FNRev(int Pty, int *cond, char *Clabel,long *VType, int *VSize, int *VQ, struct VARS * *VVar, STR_LONG *VCost, struct TAGS **VTag, int *VDim) {
  int i,j,v,T,I,T1,Co=0;
  int OP,oOP,AR,Exit=0,J2,SavedR;
  int RSize=0,RDim=0,RQ,VQ1;
  struct TAGS *RTag;
  char Rlabel[32];
  long RType;
  char AS[32],*BS,B1S[32],TS[32],T1S[32],MyBuf[32],MyBuf1[32];
  STR_LONG RCost;
  char *p1;
  struct LINE *ROut,*t;
	struct VARS RPtr;
  struct VARS *RVar, *VPtr;
  long OT,p,l,l1;
  struct OP_DEF u[4];
//  int BrackOP[10],BrackPty[10];
 
  RQ=0;
  *RCost.s=0;
  *Rlabel=0;
//  RVar=&RPtr;  // preparata per i ptr..
  VPtr=*VVar;  // salvo quello che arriva... (usato da chi crea PTR)
  p=ftell(FIn);
//  ROut=LastOut;
  oOP=0;
  SavedR=0;
  do {
	  RVar=&RPtr;        // recupero RVar alterata
    OT=ftell(FIn);
    AR=FNGetAritElem(&OP,TS,VQ,VVar,VType,VSize,VCost,Co);
// AR% = 1 SE COSTANTE, 2 SE VARIABILE, 3 SE OPERANDO, 0 SE FINE LINEA, -1 se errore!
   if(debug>2) 
     myLog->print(0,"LETTO AritmElem : %d, Brack %d\n",AR,Brack);

    switch(AR) {
      case 0:
        Exit=TRUE;
        break;
      case 1:
        *VTag=0;
        *VDim=0;
//     myLog->print(0,"LETTO AritmElem : %ld\n",VCost->l);
//        _tcscpy(VCost,TS);
        break;
      case 2:
//      	*VCost=0;
        *VTag=(*VVar)->hasTag;
        *VDim=(*VVar)->dim;
        break;
      case 3:
        if(OP>Pty) {
          if(OP>=3 && OP<=10) {          // se c'è un operatore (*,+,<=,&...), TOLGO cond subito!
            *cond=0;           // in realtà anche alcuni op.2 andrebbero tolti...
            }
          Exit=TRUE;
          fseek(FIn,OT,SEEK_SET);
          }
        else {
          *T1S=0;
          switch(OP) {
            case 1:
              switch(*TS) {
                case '(':
                  if(Co>0) {
                    if(*VType & VARTYPE_FUNC) {
                      PROCUsaFun(*VVar,Regs->D>0,Pty>2 && Pty<14);		//2025, Pty solo se operazione binary
//                      *VSize=(*VVar)->size;
                      *VType &= ~(VARTYPE_FUNC | VARTYPE_FUNC_USED | VARTYPE_FUNC_BODY) /*0xfffffc7f*/;
                      *VQ=1;
                      }
                    else
                      PROCError(2064);
                    }
                  else {
                    if(FNIsType(FNLA(MyBuf)) != -1) {     // cast
                      l1=ftell(FIn);
                      FNLO(MyBuf);
                      *VType=VARTYPE_PLAIN_INT;
                      *VSize=0;
                      v=0;
                      PROCGetType(VSize,VType,VTag,&v,l1);
                      PROCCheck(')');
                      v=0;
										  FNRev(2,&v,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
										  if(RQ==3) {
#if MICROCHIP
										    ReadVar(RVar,*VType,*VSize,0,0);		//FINIRE
#else
										    ReadVar(RVar,*VType,*VSize,0);
#endif
										    *VQ=1;
										    }
										  else if(RQ==1) {
                      	PROCCast(*VType,*VSize,RType,RSize);
                      	*VQ=1;
                      	}
										  else if(RQ==2) {
#if MICROCHIP
                      	PROCReadD0(RVar,*VType,*VSize,0,0);
#else
                      	PROCReadD0(RVar,*VType,*VSize,0);
#endif
										    *VQ=1;
										    }
										  else if(RQ & 8) {
										    *VCost=RCost;
										    *VQ=RQ;
										    }
//										    PROCUseCost(RQ,RType,RSize,&RCost);
                      }
                    else {
//                      BrackOP[Brack]=OP;
//                      BrackPty[Brack]=Pty;
//                      Pty=99;
                      Brack++;
//                      if(Brack>=10)
//												PROCError(1035);                      
                      FNRev(99,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
//                      FNRev(99,cond,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
                      // su cond ho dei dubbi...
                      }
                    }
                  break; 
                case ')':
                  if(Pty==99) {
                    Brack--;
                    if(oOP==12)
                      PROCOutLab(Clabel);
//                  Pty=1;
//                    OP=BrackOP[Brack];
//                    Pty=BrackPty[Brack];
                    }
                  else {
                    fseek(FIn,-1,SEEK_CUR);
//                    Exit=TRUE;
                    }
                  Exit=TRUE;
                  break;
                case ']':
                  fseek(FIn,-1,SEEK_CUR);
                  Exit=TRUE;
          // FORSE SAREBBE MEGLIO METTERCI UN IDENT. DI "[" PENDENTE
                  break;
                case '[':
                  J2=Regs->D;         // reg. per ReadD0
                  l=0;
                  v=0;               // flag per ReadD0
                  T=0;               // 1 quando leggo la base-array
                  T1=*VType;
                  if(*VQ==3) {
                    T1 |= VARTYPE_ARRAY;
                    }  
                  do {
	                  if(T1 & 0xf) {
	                    T1=(T1 & VARTYPE_NOT_A_POINTER) | ((T1 & VARTYPE_IS_POINTER) -1);
	                    i=Regs->Inc(FNGetMemSize(T1,*VSize,0));
	                    if(i)
	                      PROCError(1035);
	  	                j=*cond;
		                  *cond = 0x100;
	//                    *cond=0;
	                    FNRev(15,cond,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
	                    if(!i)
	                      Regs->Dec(FNGetMemSize(*VType,*VSize,0));
		                  if(!T && (RQ>=1 && RQ<=3)) {
			                  if(*VQ==3) {
			                    if(*VType & VARTYPE_ARRAY) {
			                      PROCGetAdd(3,*VVar,l);
//											myLog->print(0,"faccio GETADD 1 con ofs %d\n",l);
			                      l=0;
			                      }
			                    else if(*VType & 0xf) {
				                    if((*VVar)->classe == CLASSE_REGISTER)
						                  J2=MAKEPTRREG((*VVar)->label);
														else {
#if MICROCHIP
			                        ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
			                        ReadVar(*VVar,0,0,0);
#endif
															}
			                      }
			                    T=1;  
			                    }
			                  }  
	                    i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
	                    if(i)
	                      PROCError(1035);
	                    switch(RQ) {
	                      case 1:
	                        PROCCast(VARTYPE_UNSIGNED,INT_SIZE,RType,RSize);        // l'indice array dev'essere unsigned int
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case 2:
#if MICROCHIP
											    PROCReadD0(RVar,VARTYPE_UNSIGNED,INT_SIZE,*cond & 0xff,0);
#else
											    PROCReadD0(RVar,VARTYPE_UNSIGNED,INT_SIZE,*cond & 0xff);
#endif
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case 3:
#if MICROCHIP
	                        ReadVar(RVar,VARTYPE_UNSIGNED,INT_SIZE,0,0);			// FINIRE
#else
	                        ReadVar(RVar,VARTYPE_UNSIGNED,INT_SIZE,0);
#endif
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case VALUE_IS_COSTANTE:
                          l+=RCost.l;
//                          l+=(RCost.l * FNGetMemSize(*VType & 0xfffffbffl,*VSize,0));// per adesso lo tolgo (multidim)
	                        v |= 2;   // segnalo indice cost. in l
	                        break;  
	                      case 9:
		                      PROCError(1035);
	                        break;
	                      }  
                      if(RQ>=1 && RQ<=3) {
#if ARCHI                       
                      if(RSize>1) {
                        sprintf(T1S,"ASL #%d",log(RSize)/log(2));
                        }
                      else 
                        *T1S=0;
                      if(RQ & 8) {
                        _tcscpy(AS,"#");
                        i=VCost->l;
                        if(i >= 0) {
                          _tcscat(AS,VCost->s);
                          BS="ADD";
                          }
                        else {            
                          sprintf(MyBuf,"%s%d",AS,-i);
                          BS="SUB";
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,AS,T1S);
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1,T1S);
                        }
#elif Z80
                        u[1].mode=u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
                        u[1].ofs=0;
                        u[1].s.n=Regs->D+1;
//                        Op2A("add",u,&u[1]);   // per adesso lo tolgo (multidim)
#elif I8086
                      if(RQ & 8) {
                        i=VCost->l;
                        if(i >= 0) {
                          BS="add";
                          }
                        else {            
                          BS="sub";
                          }
                        i*=RSize;
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO16,Regs->D,i);
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,Regs->D,Regs->D+1 /*Regs->D1S*/);
                        }
#elif MC68000
                      if(RQ & 8) {
                        i=VCost->l;
                        if(i >= 0) {
                          BS="add";
                          }
                        else {            
                          BS="sub";
                          }
                        i*=RSize;
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO16,Regs->D);
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D+1 /*Regs->D1S*/);
                        }
#elif MICROCHIP
                        u[1].mode=u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
                        u[1].ofs=0;
                        u[1].s.n=Regs->D+1;
//                        Op2A("add",u,&u[1]);   // per adesso lo tolgo (multidim)
#endif                        
                        }
	//                    *VQ=RQ;  
	//                    *VVar=RVar;
	//                    *VCost=RCost;
	  	                *cond=*cond ? j : 0;
	                    if(!i)
	                      Regs->Dec(FNGetMemSize(*VType,*VSize,0));

	                    PROCCheck(']');
	                    }
	                  else
	                    PROCError(2109);
	                  } while(*FNLA(MyBuf) == '[');  

                  if(!T) {  // se non l'ho letto prima...
	                  if(*VQ==3) {
	                    if(*VType & VARTYPE_ARRAY) {
	                      PROCGetAdd(3,*VVar,l);
//											myLog->print(0,"faccio GETADD 2 con ofs %d\n",l);
	                      l=0;
	                      }
	                    else if(*VType & 0xf) {
		                    if((*VVar)->classe == CLASSE_REGISTER)
				                  J2=MAKEPTRREG((*VVar)->label);
												else  {
#if MICROCHIP
	                        ReadVar(*VVar,0,0,0,0);			// FINIRE
#else
	                        ReadVar(*VVar,0,0,0);
#endif
													}
	                      }
	                    T=1;  
	                    }
	                  }  
                  if(T1 & VARTYPE_ARRAY) {
                    *VSize=FNGetMemSize(T1 & ~VARTYPE_ARRAY,*VSize,0);
//       myLog->print(0,"\aVQ %d, VarClass %d\n",*VQ,(*VQ ==3)?(*VVar)->classe : 0);
											if(v & 2) {
//                        u[1].ofs=l;
                        i=0;
                        }
											if(v & 1) {
                        i=Regs->Inc(FNGetMemSize(T1,*VSize,0));
                        l=Regs->D;
                        if(!i)
                          Regs->Dec(FNGetMemSize(T1,*VSize,0));
											  i=1;
											  }
                      *VType = T1 & ~VARTYPE_ARRAY /*0xFFFFFBFF*/;
                      if(FNIsOp(MyBuf,0)>=2)
                        *cond=0;
//                      u[0].mode=0x80;
//                      PROCReadD0(*VSize,*VType,&u[0],&u[1],i,*cond & 0xff);
									    PROCReadD0(*VVar,0,0,*cond & 0xff);
											*VVar=VPtr;
											(*VVar)->modif=i;
											(*VVar)->type=*VType;
											(*VVar)->size=*VSize;
											(*VVar)->func=(struct VARS *)J2;
											(*VVar)->parm=(char *)l;
//											myLog->print(0,"array esce con ofs %d\n",l);
                      *VQ=2;
                    }
                  else
                    PROCError(2109);
                  break;
                case '.':
                case '-':
                  if(!*VTag)
                    PROCError((*TS=='.') ? 2224 : 2223);
                  FNLO(T1S);
                  RVar=FNGetAggr(*VTag,T1S,(*VType & VARTYPE_STRUCT) ? 1 : 0,&J2);
//                  myLog->print(0,"GetAGGR\a: %d\n",J2);
                  u[0].s.n=Regs->D;
                  u[0].mode=OPDEF_MODE_NULLA;
                  u[0].ofs=0;
                  u[1].s.n=0;
                  u[1].mode=OPDEF_MODE_IMMEDIATO_INDIRETTO;
                  u[1].ofs=J2;
                  if(!RVar)
                    PROCError(2038,T1S);
                  else if(*VQ==2) {
                    if(*TS=='.') {
                      if(*VType & VARTYPE_IS_POINTER)
                        PROCError(2221);
                      else 
                        PROCGetAdd(2,*VVar,0);
                      }
                    else {
                      if(*VType & VARTYPE_IS_POINTER) {
#if MICROCHIP
                        PROCReadD0(*VVar,0,0,0,0);		//FINIRE
#else
                        PROCReadD0(*VVar,0,0,0);
#endif
												}
                      else 
                        PROCError(2222);
                      }
                    }
                  else if(*VQ==3) {
                    if(*TS=='.') {
                      if(*VType & VARTYPE_IS_POINTER)
                        PROCError(2221);
                      else 
                        PROCGetAdd(3,*VVar,0);
                      }
                    else {
                      if(*VType & VARTYPE_IS_POINTER) {
#if MICROCHIP
	                        ReadVar(*VVar,0,0,0,0);			// FINIRE
#else
	                        ReadVar(*VVar,0,0,0);
#endif
													}
                      else 
                        PROCError(2222);
                      }
                    }
                  *VType=RVar->type;
                  *VSize=RVar->size;
                  if(*VType & VARTYPE_IS_POINTER)
                    RSize=PTR_SIZE;
                  else 
                    RSize=*VSize;        
//                  PROCReadD0(RSize,*VType & 0xfffffff0,&u[0],&u[1],0,*cond & 0xff);
							    PROCReadD0(*VVar,0,0,*cond & 0xff);
									*VVar=VPtr;
									(*VVar)->type=*VType/* & 0xfffffff0*/;
									(*VVar)->modif=0;
									(*VVar)->size=RSize;
									(*VVar)->func=(struct VARS *)Regs->D;
									(*VVar)->parm=(char *)J2;
                  *VTag=RVar->tag;
                  *VQ=2;
                  break;
                default:
                  break;
                }
              break;

            case 2:
              switch(*TS) {
                case '-':
                  if(*(TS+1) != '-') 
                    goto LUnaryMinus;
                case '+':      
                  if(!Co) {
                    T=1;											// se pre-inc
                    u[0].s.n=Regs->D;
//                    BS=Regs->DS;
                    *cond=FALSE;
                    FNRev(2,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
                    }
                  else {
                    T=2;                       // se post-inc
//                    FNLA(MyBuf);
//                    if(!*MyBuf || *MyBuf==';' /*|| *MyBuf==')'*/) {   // mezza boiata...
										if(Pty>14 && !*cond) {
                      T=0;
                      u[0].s.n=Regs->D;
//		                  BS=Regs->DS;
		                  }
                    else  
                      u[0].s.n=Regs->D+1;
//		                  BS=Regs->D1S;
                    }
                  if(*TS=='+')
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
                  v=*VSize;
                  I=1;
#if ARCHI
                  _tcscpy(T1S,"#");
                  if(*VType & VARTYPE_IS_POINTER) {  
                    sprintf(MyBuf,"%d",*VSize);
                    _tcscat(T1S,MyBuf);
                    v=PTR_SIZE;
                    I=*VSize;
                    }
                  else {
                    _tcscat(T1S,"1");
                    }
#elif Z80 || I8086 || MC68000 || MICROCHIP
                  if(*VType & VARTYPE_IS_POINTER) {
                    v=PTR_SIZE;
                    I=*VSize;
                    }
#endif
	                if(*VQ==3) {
#if MC68000
										char movString2[16];
										_tcscpy(movString2,movString);
										switch(v) {
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
/*										switch(I) {
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
#endif
	                  switch((*VVar)->classe) {
	                    case CLASSE_EXTERN:
	                    case CLASSE_GLOBAL:
	                    case CLASSE_STATIC:
//    	                  _tcscpy(AS,(*VVar)->label);
#if ARCHI
	                      switch(v) {
	                        case 1:
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D,",",AS);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,OPDEF_MODE_REGISTRO,Regs->DS,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,",",AS);
	                          break;
	                        case 2:
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D,",",AS);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,",",AS,"+1");
	                          sprintf(MyBuf,",%s, ASL #8",Regs->D1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,MyBuf);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,Regs->DS,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,",",AS);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",BS,",",BS,", LSR #8");
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,",",AS,"+1");
	                          break;
	                        case 4:  
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D,",",AS);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,Regs->DS,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STR",BS,",",AS);
	                          break;
	                        case 8:
	                          break;
	                        default:
	                          break;
	                        }
#elif Z80 			
	                      switch(v) {
	                        case 1:
//	                          FNGetFPStr(MyBuf,0,AS);
/*                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,2,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,1,Regs->D,2,3);
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,2,3);
                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,2,3);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,1,Regs->D,2,3);
 	                            */
                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,0);
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,0);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,0);
	                          break;
	                        case 2:  
//	                          FNGetFPStr(MyBuf,0,AS);
                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,&u[0].s,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==2) {
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,&u[0].s,0);
                              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,&u[0].s,0);
 	                            }
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,&u[0].s,0);
                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO,&u[0].s,0);
	                          break;
	                        case 4:// non ritorna pre-inc
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,2);
 	                          if(*TS=='i') {
 	                            i=I;
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
                              }  
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          if(*TS=='d') {
	                            i=I;
     	                        while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
                              }  
                            FNGetLabel(MyBuf1,2);  
 	                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
    	                      i=I;
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	                          PROCOutLab(MyBuf1);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,2,OPDEF_MODE_REGISTRO,Regs->D+1);
	                          break;
	                        default:
	                          break;
	                        }
#elif I8086
	                      switch(v) {
	                        case 1:
                            if(T==2)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
 	                          while(I--)  
	                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==1)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
														break;
	                        case 2:  
                            if(T==2)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
 	                          while(I--)  
	                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==1)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
	                          break;
	                        case 4:// non ritorna pre-inc
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,2);
 	                          if(*TS=='i') {
 	                            i=I;
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
                              }  
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          if(*TS=='d') {
	                            i=I;
     	                        while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
                              }  
                            FNGetLabel(MyBuf1,2);  
 	                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
    	                      i=I;
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	                          PROCOutLab(MyBuf1);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,2,OPDEF_MODE_REGISTRO,Regs->D+1);
	                          break;
	                        case 8:
	                          break;
	                        default:
	                          break;
	                        }
#elif MC68000
	                      switch(v) {
	                        case 1:
                            if(T==2)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO,Regs->D);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==1)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
														break;
	                        case 2:  
                            if(T==2)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO16,Regs->D);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==1)
	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO16,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D);
	                          break;
	                        case 4:
                            if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO32,Regs->D);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0);
                            if(T==1)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)VVar,0,OPDEF_MODE_REGISTRO32,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
	                          break;
	                        case 8:
	                          break;
	                        default:
	                          break;
	                        }
#elif MICROCHIP
	                      switch(v) {
	                        case 1:
														if(*VType & VARTYPE_ROM) {
															PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
															}
														else {
//															PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															// NO, NON SERVE!
															PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLB high",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															if(T==2)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
 															if(I==1)  
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															else {
																PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,I);
																PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='I' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
																}
															if(T==1)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															}
														break;
													case 2:  // FINIRE!
														if(*VType & VARTYPE_ROM) {
															PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
															}
														else {
															PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLB high",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															if(T==2)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
 															if(I==1)  
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															else {
																PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,I);
																PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='I' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
																}
	                            FNGetLabel(MyBuf1,2);  
 		                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf1,0);
															// C18 usa ADDWFC ...; opp. usare INFSNZ
 															if(I==1)  
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,1);
															else {
																PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,I);
																PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='I' ? "ADDWF" : "SUBWF",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,1);
																}
	 	                          PROCOutLab(MyBuf1);
															if(T==1)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															}
	                          break;
													case 4:// FINIRE!
														if(*VType & VARTYPE_ROM) {
															PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
															}
														else {
															PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															if(T==2)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
 															while(I--)  
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,9);
															if(T==1)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
															}
														if(*VType & VARTYPE_ROM) {
															PROCWarn(1001,"inc/dec di variabile ROM, ci provo");
															}
														else {
															PROCOper(LINE_TYPE_ISTRUZIONE,"LFSR 0",OPDEF_MODE_REGISTRO,9,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)VVar,0);
															if(T==2)
																PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,9);
 															while(I--)  
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,9);
															if(T==1)
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
	                      i=MAKEPTROFS((*VVar)->label);
#if ARCHI
	                      switch(v) {
	                        case 1:                        
	                          sprintf(MyBuf,",#%d]",i);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D,",[",Regs->FpS,MyBuf);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,OPDEF_MODE_REGISTRO,Regs->D,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          break;
	                        case 2:
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	                          sprintf(MyBuf,",%s, ASL #8",Regs->D1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",Regs->D,",",Regs->D,MyBuf);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,Regs->D,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",BS,",",BS,", LSR #8");
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",BS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
	                          break;
	                        case 4:
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->DS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,TS,BS,Regs->DS,T1S);
	                          PROCOper(LINE_TYPE_ISTRUZIONE,storString,BS,",[",Regs->FpS,MyBuf);
	                          break;
	                        case 8: 
	                          break;
	                        default:
	                          break;
	                        }
#elif Z80
	                      switch(v) {
	                        case 1:
//	                          FNGetFPStr(MyBuf,i,NULL);
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          break;
	                        case 2:
	                        /*
 	                          PROCOper(movString,BS+1,FNGetFPStr(MyBuf,i));
 	                          PROCOper(movString,B1S,FNGetFPStr(MyBuf,i+1));
                            if(T==2) {
 	                            PROCOper(movString,Regs->DSl,BS+1);
 	                            PROCOper(movString,Regs->DSh,B1S);
 	                            }
 	                          while(I--)  
                              PROCOper(TS,BS);
 	                          PROCOper(movString,FNGetFPStr(MyBuf,i,NULL),BS+1);
 	                          PROCOper(movString,FNGetFPStr(MyBuf,i+1,NULL),B1S);
 	                          */
 	                          
                          	if(T==2) {
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	                            }
 	                          while(I--) { 
// 	                            FNGetFPStr(MyBuf,i,NULL);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                              PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	                            }
 	                          if(T==1) {
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
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          if(*TS=='d')
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	                          while(I--)  
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
	                      switch(v) {
	                        case 1:                        
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	                          break;
	                        case 2:
                          	if(T==2) {
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	                            }
 	                          while(I--) { 
// 	                            FNGetFPStr(MyBuf,i,NULL);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                              PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	                            }
 	                          if(T==1) {
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
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          if(*TS=='d')
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	                          while(I--)  
                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1);
 	                          PROCOutLab(MyBuf1);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                          break;
	                        case 8: 
	                          break;
	                        default:
	                          break;
	                        }
#elif MC68000	                        
	                      switch(v) {
	                        case 1:                        
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO,Regs->D);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
	                          break;
	                        case 2:
                          	if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,Regs->D);
//                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                          if(T==1)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
	                          break;
	                        case 4:
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,Regs->D);
                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
//                            PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
 	                          if(T==1)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,Regs->D);
                          	if(T==2)
	 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
	                          break;
	                        case 8: 
	                          break;
	                        default:
	                          break;
	                        }
#elif MICROCHIP
	                      switch(v) {
	                        case 1:
//	                          FNGetFPStr(MyBuf,i,NULL);
														PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,i);
                            if(T==2)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
 	                          while(I--) {
// 															if(I==1)			// qui non posso usare W (e non so nemmeno se converrebbe fare ADD)
																PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
//															else {
//																PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,I);
//																PROCOper(LINE_TYPE_ISTRUZIONE,*TS=='I' ? "ADDWF" : "SUBWF",OPDEF_MODE_REGISTRO,9);
//																}
																}

//                              PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
                            if(T==1)
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
	                          break;
	                        case 2:
														PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO,i);
                          	if(T==2) {
 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,14);
//FINIRE! 	                            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
 	                            }
 	                          while(I--) { 
// 	                            FNGetFPStr(MyBuf,i,NULL);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
															PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
 	                            if(*TS=='d')
 	                              PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                              PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
															PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,14);
 	                            }
 	                          if(T==1) {
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
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
 	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
 	                          if(*TS=='d')
   	                          while(I--)  
                                PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D);
 	                          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)FNGetLabel(MyBuf1,2),0);
 	                          while(I--)  
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
	                      if(T) {                 
	                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,Regs[*VVar]);
	                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,Regs[*VVar],Regs[*VVar],T1S);
	                        }
	                      else {
	                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,Regs[*VVar],Regs[*VVar],T1S);
	                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,Regs[*VVar]);
	                        }
#elif Z80
	                      if(T==2) {                 
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
                        while(I--)  
                           PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                      if(T==1) {
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
#elif I8086
	                      if(T==2) {                 
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
                        while(I--)  
                           PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                      if(T==1) {
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
#elif MC68000
                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
#elif MICROCHIP
	                      if(T==2) {                 
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
                        while(I--)  
                           PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                      if(T==1) {
	                        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG((*VVar)->label));
	                        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
	                        }
#endif
	                      break;
	                    }
	                  }
	                else {
#if ARCHI
	                  _tcscpy(AS,LastOut->s);
	                  p1=strchr(AS,',');
	                  *(p1+1)=0;
	                  t=LastOut;
	                  LastOut=LastOut->prev;
	                  PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO,Regs->D,T1S);
	                  switch(I) {
	                    case 1:
	                      PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",OPDEF_MODE_REGISTRO,Regs->D1S,",",AS);
	                      break;
	                    case 2:
	                      PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",OPDEF_MODE_REGISTRO,Regs->D+1,",",AS);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D+1,",",OPDEF_MODE_REGISTRO,Regs->D+1,", LSR #8");
	                      PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",OPDEF_MODE_REGISTRO,Regs->D+1,",",AS,"+1");
	                      break;
	                    case 4:
	                      PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,Regs->D1S,",",AS);
	                      break;
	                    case 8: 
	                      break;
	                    default:
	                      break;
	                    }
	                  LastOut=t;
#elif Z80
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	                  switch(v) {
	                    case 1:
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(T==1)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	                      break;
	                    case 2:
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        while(I--) { 
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      }
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    case 4:
	                    PROCError(1001,"++ long");
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    default:
	                      break;
	                    }
#elif I8086
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	                  switch(v) {
	                    case 1:
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(T==1)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	                      break;
	                    case 2:
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        while(I--) { 
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      }
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    case 4:
	                    PROCError(1001,"++ long");
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    default:
	                      break;
	                    }
#elif MC68000
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	                  switch(v) {
	                    case 1:
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO,3);
//	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(T==1)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO,Regs->D);
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,3,OPDEF_MODE_REGISTRO,Regs->D);
	                      break;
	                    case 2:
                        if(T==2)
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO16,(int)(*VVar)->func);
//	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(T==1)
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO16,(int)(*VVar)->func);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
	                      break;
	                    case 4:
                        if(T==2)
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO32,(int)(*VVar)->func);
                        if(*TS=='s')
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,3,u[0].mode,&u[0].s,u[0].ofs);
//	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_IMMEDIATO,I,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(*TS=='s')
                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO,3);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO32,(int)(*VVar)->func);
                        if(T==1)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[0].mode,&u[0].s,u[0].ofs,OPDEF_MODE_REGISTRO32,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
	                      break;
	                    default:
	                      break;
	                    }
#elif MICROCHIP
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
	                  switch(v) {
	                    case 1:
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(T==1)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u[0].mode,&u[0].s,u[0].ofs);
                        if(T==2)
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	                      break;
	                    case 2:
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        while(I--) { 
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
	                        if(*TS=='d')
	                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
	                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
		                      }
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    case 4:
	                    PROCError(1001,"++ long");
                        if(T==2) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,OPDEF_MODE_REGISTRO_INDIRETTO,(int)(*VVar)->func);
                        if(*TS=='d')
                          PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
                        PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",5);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,TS,u[0].mode,&u[0].s,u[0].ofs);
	                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                        if(T==1) {
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u[0].mode,&u[0].s,u[0].ofs);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,(int)(*VVar)->func);
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
	                      break;
	                    default:
	                      break;
	                    }
#endif
	                  }
                  *VQ=1;
	                break;
	              case '*':
	                i=*cond;
	                *cond = 0x100;
								  FNRev(2,cond,Clabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
						      J2=Regs->D;
								  if(RQ==3) {
								    if(RVar->classe == CLASSE_REGISTER) {
			                J2=*((int *)RVar->label);
											}								      
								    else {
#if MICROCHIP
								      ReadVar(RVar,0,0,*cond & 0xff,0);		// FINIRE
#else
								      ReadVar(RVar,0,0,*cond & 0xff);
#endif
								      }
								    }
									else if(RQ==2) {
#if MICROCHIP
								    PROCReadD0(RVar,0,0,*cond & 0xff,0);
#else
								    PROCReadD0(RVar,0,0,*cond & 0xff);
#endif
										}
									else if(RQ & 8) {
#if MICROCHIP
								    PROCUseCost(RQ,RType,RSize,&RCost,0);		// FINIRE
#else
								    PROCUseCost(RQ,RType,RSize,&RCost);
#endif
										}
	                if(RType & VARTYPE_IS_POINTER) {
	                  *VType=(RType & 0xFFFFFFF0) | ((RType & VARTYPE_IS_POINTER) -1);
	                  *VSize=FNGetMemSize(*VType,RSize,1);
	                  }
	                else
	                  PROCError(2100);
	                *cond=*cond ? i : 0;
//	                PROCReadD0(*VSize,*VType,&u[0],0,0,*cond & 0xff);
									*VVar=VPtr;
									(*VVar)->modif=0;
									(*VVar)->type=*VType;
									(*VVar)->size=*VSize;
									(*VVar)->classe=RVar->classe;
									(*VVar)->func=(struct VARS *)J2;		// credo cazzata, 2025
									(*VVar)->parm=(char*)J2;
									*VTag=RTag;
									*VDim=RDim;
									*VCost=RCost;
	                *VQ=2;
	                break;
	              case '&':
	                *cond=FALSE;
	                FNRev(2,cond,Clabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
	                switch(RQ) {
	                  case 1:
	                    PROCError(2101);
	                  case 2:
#if ARCHI	                      
	                    t=LastOut;
	                    _tcscpy(AS,t->s);
	                    I=atoi(strchr(AS,'#')+1);
	                    if(I>=0)
	                      BS="ADD";
	                    else {
	                      BS="SUB";
	                      I=-I;
	                      }
	                    if(I) {
	                      sprintf(MyBuf,"%s%s,%s,#%d",BS,Regs->DS,Regs->DS,I);
	                      _tcscpy(t->s,MyBuf);
	                      }
	                    else
	                      _tcscpy(t->s,";");
#elif Z80 || I8086 || MC68000 || MICROCHIP
	                    PROCGetAdd(2,RVar,0);
#endif	                    
  	                  break;
	                  case 3:
	                    PROCGetAdd(3,RVar,0);
	                    break;
	                  default:
	                    break;
	                  }
	                *VQ=1;
	                *VType=VARTYPE_UNSIGNED;
		              *VSize=PTR_SIZE;
		              break;
		            case '~':
		              *cond=FALSE;
		              FNRev(2,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
		              switch(*VQ) {
		                case VALUE_IS_COSTANTE:
		                  VCost->l=~VCost->l;
		                  break;
		                case 9:
#if MICROCHIP
		                  PROCUseCost(-2,*VType,*VSize,VCost,0);		// FINIRE
#else
		                  PROCUseCost(-2,*VType,*VSize,VCost);
#endif
#if ARCHI
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
#elif Z80
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
											u[0].ofs=0;
											OpA("cpl",u,(struct OP_DEF *)0);
											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
											OpA("cpl",u,(struct OP_DEF *)0);
#elif I8086
											PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO,Regs->D);
#elif MC68000
											PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
											u[0].ofs=0;
											OpA("COMF",u,(struct OP_DEF *)0);
											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
											OpA("COMF",u,(struct OP_DEF *)0);
#endif
		                  *VQ=1;
		                  break;
		                default:
											if(*VQ==3) {
#if MICROCHIP
												ReadVar(*VVar,0,0,*cond & 0xff,0);		// FINIRE
#else
												ReadVar(*VVar,0,0,*cond & 0xff);
#endif
												}
											if(*VQ==2) {
#if MICROCHIP
										    PROCReadD0(*VVar,0,0,*cond & 0xff,0);
#else
										    PROCReadD0(*VVar,0,0,*cond & 0xff);
#endif
												}
		                  if(*VQ & 0x80)
		                    PROCAssignCond(VQ,VType,VSize,Clabel);
#if ARCHI
		                  switch(*VSize) {
		                    case 1:
		                      PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
		                      PROCOper(LINE_TYPE_ISTRUZIONE,"AND",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,"#255");
		                      break;
		                    case 2:
		                      PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,NULL,"ASL #16");
		                      PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,NULL,"LSR #16");
		                      break;
		                    case 4:
		                      PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
		                      break;
		                    default:
		                      break;
		                    }
#elif Z80
											u[0].ofs=0;
		                  switch(*VSize) {
		                    case 4:
													u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													u[0].s.n=Regs->D+1;
													OpA("cpl",u,(struct OP_DEF *)0);
													u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
													OpA("cpl",u,(struct OP_DEF *)0);
		                    case 2:
													u[0].s.n=Regs->D;
													u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("cpl",u,(struct OP_DEF *)0);
		                    case 1:
													u[0].s.n=Regs->D;
													u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
													OpA("cpl",u,(struct OP_DEF *)0);
		                      break;
		                    default:
		                      break;
		                    }
#elif I8086
		                  switch(*VSize) {
		                    case 4:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO16,Regs->D);
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO16,Regs->D);
													break;
		                    case 2:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO16,Regs->D);
													break;
		                    case 1:
    											PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO,Regs->D);
		                      break;
		                    default:
		                      break;
		                    }
#elif MC68000
		                  switch(*VSize) {
		                    case 4:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg.l",OPDEF_MODE_REGISTRO32,Regs->D);
													break;
		                    case 2:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg.w",OPDEF_MODE_REGISTRO16,Regs->D);
													break;
		                    case 1:
    											PROCOper(LINE_TYPE_ISTRUZIONE,"neg.b",OPDEF_MODE_REGISTRO,Regs->D);
		                      break;
		                    default:
		                      break;
		                    }
#elif MICROCHIP
											u[0].ofs=0;
		                  switch(*VSize) {
		                    case 4:
													u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													u[0].s.n=Regs->D+1;
													OpA("COMF",u,(struct OP_DEF *)0);
													u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
													OpA("COMF",u,(struct OP_DEF *)0);
		                    case 2:
													u[0].s.n=Regs->D;
													u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("COMF",u,(struct OP_DEF *)0);
		                    case 1:
													u[0].s.n=Regs->D;
													u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
													OpA("COMF",u,(struct OP_DEF *)0);
		                      break;
		                    default:
		                      break;
		                    }
#endif
		                  *VQ=1;
		                  break; 
		                }
		              break;
	              case '!':
	                FNRev(2,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
	                if(*VQ & 0x80) {
                    *VQ ^= 1;
                    }
                  else if(*VQ == VALUE_IS_COSTANTE) {                   // mancherebbe VQ=-2 ossia 9...
                    VCost->l=!VCost->l;
	                  *VQ=VALUE_IS_COSTANTE;
	                  }
	                else {
										if(*VQ==2) {
#if MICROCHIP
									    PROCReadD0(*VVar,0,0,*cond & 0xff,0);
#else
									    PROCReadD0(*VVar,0,0,*cond & 0xff);
#endif
											}
										if(*VQ==3) {
#if MICROCHIP
	                    ReadVar(*VVar,0,0,*cond & 0xff,0);		// FINIRE
#else
	                    ReadVar(*VVar,0,0,*cond & 0xff);
#endif
											}
#if ARCHI
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVNE",OPDEF_MODE_REGISTRO,Regs->D,"#0");
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MVNEQ",OPDEF_MODE_REGISTRO,Regs->D,"#0");
 	                  *VQ=1;
#elif Z80
	                  if(!(*cond & 0xff)) {
	                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  if(*VSize == 1) {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
  		                  }
		                  else {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			                  if(*VSize > 2) {
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
			                    }
		                    }
 		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,0);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  	                  *VQ=1;
		                  *VSize=INT_SIZE;
			                *VType=0;
		                  }  
		                else  
  	                  *VQ |= 0x20;            // segnala ! condizionale
#elif I8086
	                  if(!(*cond & 0xff)) {
	                    switch(*VSize) {
	                      case 1:
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
	                        break;
	                      case 2:
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
	                        break;
	                      case 4:
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
	                        break;
		                    }
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,OPDEF_MODE_REGISTRO);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
//		                  PROCOper(LINE_TYPE_JUMPC,"jnz",FNGetLabel(MyBuf,2));
		                  PROCOper(LINE_TYPE_ISTRUZIONE,decString,Regs->D,OPDEF_MODE_REGISTRO);
		                  if(*VSize > 1) {
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA);
			                  if(*VSize > 2) {
	    	                  PROCOper(LINE_TYPE_ISTRUZIONE,"cwd",OPDEF_MODE_NULLA);
			                    }
		                    }
  	                  *VQ=1;
		                  *VSize=INT_SIZE;
			                *VType=0;
		                  }  
		                else  
  	                  *VQ |= 0x20;            // segnala ! condizionale
#elif MC68000
	                  if(!(*cond & 0xff)) {
//		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE | 0x80,5,OPDEF_MODE_COSTANTE,
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"bne.s",OPDEF_MODE_COSTANTE,
												TipoOut==0 ? (union SUB_OP_DEF *)"$" : (union SUB_OP_DEF *)"$"/*sistemare merda di easy68k*/,4);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,1,OPDEF_MODE_REGISTRO,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,
												TipoOut==0 ? (union SUB_OP_DEF *)"$" : (union SUB_OP_DEF *)"$"/*sistemare merda di easy68k*/,2);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,0,OPDEF_MODE_REGISTRO,Regs->D);
		                  if(*VSize > 1) {
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,0);
			                  if(*VSize > 2) {
	    	                  PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,0);
			                    }
		                    }
  	                  *VQ=1;
		                  *VSize=INT_SIZE;
			                *VType=0;
		                  }  
		                else  
  	                  *VQ |= 0x20;            // segnala ! condizionale
#elif MICROCHIP
	                  if(!(*cond & 0xff)) {
	                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  if(*VSize == 1) {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
  		                  }
		                  else {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			                  if(*VSize > 2) {
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
			                    }
		                    }
 		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,0);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  	                  *VQ=1;
		                  *VSize=INT_SIZE;
			                *VType=VARTYPE_PLAIN_INT;
		                  }  
		                else  
  	                  *VQ |= 0x20;            // segnala ! condizionale
#endif
	                  }      
	                break;
	              case 'z':                  // falso per lasciare il case!
LUnaryMinus:
	                *cond=FALSE;
	                FNRev(2,cond,Clabel,VType,VSize,VQ,VVar,VCost,VTag,VDim);
	                switch(*VQ) {
	                  case VALUE_IS_COSTANTE:
	                    VCost->l=-VCost->l;
//	                    myLog->print(0,"\aUNARY MINUS su COST %s\n",VCost);
	                    break;
	                  case 9:
#if MICROCHIP
		                  PROCUseCost(-2,*VType,*VSize,VCost,0);		// FINIRE
#else
		                  PROCUseCost(-2,*VType,*VSize,VCost);
#endif
#if ARCHI
	                    PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
	                    PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,"#1");
#elif Z80
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
											OpA("cpl",u,(struct OP_DEF *)0);
											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
											OpA("cpl",u,(struct OP_DEF *)0);
	                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
											PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO,Regs->D);
#elif MC68000
											PROCOper(LINE_TYPE_ISTRUZIONE,"neg.l",OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
											OpA("COMF",u,(struct OP_DEF *)0);
											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
											OpA("COMF",u,(struct OP_DEF *)0);
	                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
#endif
	                    *VQ=1;
	                    break;
	                  default:
											if(*VQ==2) {
#if MICROCHIP
										    PROCReadD0(*VVar,0,0,0,0);
#else
										    PROCReadD0(*VVar,0,0,0);
#endif
												}
											if(*VQ==3) {
#if MICROCHIP
	                      ReadVar(*VVar,0,0,0,0);			// FINIRE
#else
	                      ReadVar(*VVar,0,0,0);
#endif
												}
#if ARCHI
	                    switch(*VSize) {
	                      case 1:
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"AND",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->DS,"#255");
	                        break;
	                      case 2:
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->DS,NULL,"ASL #16");
	                        PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->DS,NULL,"LSR #16");
	                        break;
	                      case 4:
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"MVN",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->DS);
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->DS,"#1");
	                        break;
	                      default:
	                        break;
	                      }
#elif Z80
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
	                    switch(*VSize) {
	                      case 1:
													OpA("neg",u,(struct OP_DEF *)0);
	                        break;
	                      case 2:
	                      case 4:
													OpA("cpl",u,(struct OP_DEF *)0);
    											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("cpl",u,(struct OP_DEF *)0);
			                    if(*VSize==2)
			                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
			                    if(*VSize==4) {
														u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
														u[0].s.n++;
														OpA("cpl",u,(struct OP_DEF *)0);
														u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
														OpA("cpl",u,(struct OP_DEF *)0);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
					                  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D+1);
				                    }
	                        break;
	                      default:
	                        break;
	                      }
#elif I8086
	                    switch(*VSize) {
		                    case 1:
    											PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_NULLA,Regs->D);
		                      break;
		                    case 2:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_NULLA,Regs->D);
													break;
		                    case 4:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
													break;
		                    default:
		                      break;
	                      }
#elif MC68000
	                    switch(*VSize) {
		                    case 1:
    											PROCOper(LINE_TYPE_ISTRUZIONE,"neg.b",OPDEF_MODE_REGISTRO,Regs->D);
		                      break;
		                    case 2:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg.w",OPDEF_MODE_REGISTRO16,Regs->D);
													break;
		                    case 4:
													PROCOper(LINE_TYPE_ISTRUZIONE,"neg.l",OPDEF_MODE_REGISTRO32,Regs->D);
													break;
		                    default:
		                      break;
	                      }
#elif MICROCHIP
											u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
											u[0].s.n=Regs->D;
	                    switch(*VSize) {
	                      case 1:
													OpA("COMF",u,(struct OP_DEF *)0);
	                        break;
	                      case 2:
	                      case 4:
													OpA("COMF",u,(struct OP_DEF *)0);
    											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("COMF",u,(struct OP_DEF *)0);
			                    if(*VSize==2)
			                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
			                    if(*VSize==4) {
														u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
														u[0].s.n++;
														OpA("COMF",u,(struct OP_DEF *)0);
														u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
														OpA("COMF",u,(struct OP_DEF *)0);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
					                  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
				                    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D+1);
				                    }
	                        break;
	                      default:
	                        break;
	                      }
#endif
	                    *VQ=1;
	                    break;
	                  }        
	                break;
	              case 's':
	                PROCCheck('(');
	                T=-1;
	                l1=ftell(FIn);
	                RType=0;
//	                v=0;
	                FNLO(TS);
	                if(FNIsType(TS) != -1) {
	                  PROCGetType(&T,&RType,&RTag,&I,l1);
	                  }
	                else {
	                  fseek(FIn,l1,SEEK_SET);
	                  *cond=FALSE;
	                  FNRev(15,cond,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
	                  T=RSize;
	                  }
	                if(RType & VARTYPE_IS_POINTER)
	                  T=PTR_SIZE;
                  while(*FNLO(MyBuf) != ')');
	                VCost->l=T;
	                *VQ=VALUE_IS_COSTANTE;
	                *VType=VARTYPE_UNSIGNED;
	                *VSize=INT_SIZE;
	                break;
	              default:
	                break;
	              }
	            break;

            case 3:
            case 4:
	          case 5:
	          case 6:
	          case 7:
	          case 8:
	          case 9:
	          case 10:
	            J2=0;
	            subSpezReg(FNGetMemSize(*VType,*VSize,0),u);
	            ROut=LastOut;
#if ARCHI || Z80
  	          if(*VQ==1 || *VQ==2 || (*VQ==3 && (((*VVar)->classe<3) || (OP!=6 && OP!=7)))) {
//  	          if((*VQ==1) || *VQ==2 || (*VQ==3 && ((*VVar)->classe<3))) {
#elif I8086 || MC68000 || I8051 || MICROCHIP
	            if(*VQ==1 || *VQ==2 || *VQ==3) {
#endif  	            
                J2=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
                }
              j=*cond;
              *cond = 0x100;
	            FNRev(OP-1,cond,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim/*VDim*/);
              *cond=*cond ? j : 0;
	            // qui usiamo i registri Increm. per le operazioni varie dell'exp 2...
#if ARCHI || Z80
  	          if((*VQ==1) || *VQ==2 || (*VQ==3 && (((*VVar)->classe<CLASSE_AUTO) || (OP!=6 && OP!=7)))) {
//  	          if((*VQ==1) || *VQ==2 || (*VQ==3 && ((*VVar)->classe<3))) {

#elif I8086 || MC68000 || I8051 || MICROCHIP
	            if(*VQ==1 || *VQ==2 || *VQ==3) {
#endif  	            
                if(!J2)
                  Regs->Dec(FNGetMemSize(*VType,*VSize,0));
//                myLog->print(0,"Dec 1: %d\n",Regs->D);
                }
              J2=J2 && (LastOut!=ROut);            // lo uso dopo...
	            if((*VQ & 8) && (RQ & 8)) {
							  switch(*TS) {
#if ARCHI
							    case '=':
							      BS="=";
							      break;
							    case '%':
							      BS="MOD";
							      break;
							    case '!':
							      BS="<>";
							      break;
							    case '|':
							      BS="OR";
							      break;
							    case '&':
							      BS="AND";
							      break;
							    case '^':
							      BS="EOR";
							      break;
							    default:
							      BS=TS;
							      break;
#elif Z80 || I8086 || MC68000 || I8051 || MICROCHIP
							    case '+':
							    case '-':
							    case '=':
							    case '*':
							    case '/':
							    case '%':
							    case '|':
							    case '&':
							    case '^':
							      BS=TS;
							      *(BS+1)=0;
							      break;
							    case '!':
							      BS="@";         // simboli usati da eval per semplicità
							      break;
							    case '<':
							      if(*(TS+1) == '=')
							        BS=TS;
							      else
							        BS="l";
							      break;
							    case '>':
							      if(*(TS+1) == '=')
							        BS=TS;
							      else
							        BS="r";
							      break;
							    default:
							//	  _tcscpy(BS,TS);
							      PROCError(2059,TS);
							      break;
#endif
							    }
							  if((*VQ==VALUE_IS_COSTANTE) && (RQ==VALUE_IS_COSTANTE)) {
							    sprintf(MyBuf,"%ld %s %ld",VCost->l,BS,RCost.l);
							    VCost->l=EVAL(MyBuf);
							    *VQ=VALUE_IS_COSTANTE;
							    }
							  else {
							    _tcscat(VCost->s,BS); 
							    _tcscat(VCost->s,RCost.s);
							    *VQ=9;
							    }
	              }               // fine if costanti...
	            else {
  	            T=0;
	              switch(*VQ) {
	                case 0:
	                  break;
	                case 1:
			              if(J2)
		                  swap(&ROut,&LastOut);
		                break;  
	                case 2:
			              if(J2)
		                  swap(&ROut,&LastOut);
#if MICROCHIP
                    PROCReadD0(*VVar,0,0,0,0);
#else
                    PROCReadD0(*VVar,0,0,0);
#endif
		                break;  
	                case VALUE_IS_COSTANTE:
#if ARCHI
	                  if((VCost->l>255) || (OP==3)) {        // OP=3 (*)
                      goto myUVcost;
	                    }
	                  else {
	                    T=1;
	                    }
#elif Z80 || I8051 || MICROCHIP
                    if(OP==3)
                      goto myUVcost;
                    else
                      T=1;
#elif I8086
                    if(OP==3 && CPU86<2)
                      goto myUVcost;
                    else
                      T=1;
#elif MC68000
                    if(OP==3)
                      goto myUVcost;
                    else
                      T=1;
#endif
	                  break;
	                case 9:
myUVcost:	                
#if MICROCHIP
	                  PROCUseCost(*VQ,*VType,*VSize,VCost,0);		// FINIRE
#else
		                PROCUseCost(*VQ,*VType,*VSize,VCost);
#endif
	                  break;
	                case 3:  
                    switch((*VVar)->classe) {
                      case CLASSE_EXTERN:
                      case CLASSE_GLOBAL:
                      case CLASSE_STATIC:
myUVvar:	                
					              if(J2)
				                  swap(&ROut,&LastOut);
#if MICROCHIP
	                      ReadVar(*VVar,0,0,0,0);			// FINIRE
#else
	                      ReadVar(*VVar,0,0,0);
#endif
	                      break;
	                    case CLASSE_REGISTER:
#if ARCHI
	                      sprintf(sRegs[0].Dr,"R%d",MAKEPTRREG((*VVar)->label));
#elif Z80 || I8051 || MICROCHIP
		                    if(0/*OP==4*/) {
		                      u[0].mode=OPDEF_MODE_REGISTRO;
		                      u[0].s.n=*(int *)((*VVar)->label);
//		                      _tcscpy(sRegs[0].Dr,Regs[*VVar]);
//		                      _tcscpy(sRegs[0].Drh,Regs[*VVar]);
		                      T=-1;
		                      }
		                    else
	                        goto myUVvar;
#elif I8086
                        goto myUVvar;
#elif MC68000
                        goto myUVvar;
#endif                
												break;   
											case CLASSE_AUTO:
#if ARCHI
                        goto myUVvar;
#elif Z80	|| I8051 || MICROCHIP
	                      if((OP==6 || OP==7 || (*cond && OP==8)) && !(RType & 0xf)) {
		                      i=MAKEPTRREG((*VVar)->label);
		                      u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[0].s.n=0;
		                      u[0].ofs=i;
							            if(*VSize>2) {
			                      u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                      u[1].s.n=0;
  		                      u[1].ofs=i+2;
								            }
								          T=-1;           // mi ricordo che la 1° expr è in memoria  
		                      }
		                    else
	                        goto myUVvar;
#elif I8086		                      
                        goto myUVvar;
#elif MC68000
                        goto myUVvar;
#endif
							          break;  
	                    }
	                  break;  
	                default:
                    PROCAssignCond(VQ,VType,VSize,Clabel);
	                  break;              
	                }
//                ROut=LastOut;
	              if(J2 && T==0) {
    	            SavedR=FNGetMemSize(*VType,*VSize,0);
  	              Regs->Save(SavedR);
                  swap(&ROut,&LastOut);
      	          }
      	        J2=0;
#if ARCHI || Z80
  	            if((T>=0 && ((*VQ==1) || (*VQ==2) || (*VQ==3))) || OP==3)
#elif I8086 || MC68000 || I8051 || MICROCHIP
  	            if(T>=0 && ((*VQ==1) || (*VQ==2) || (*VQ==3)))
#endif  	            
                  {
                  J2=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
	            // ...e qui usiamo i registri Increm. per ReadVar e simili...
                  VQ1=*VQ;
	                }
	              else
	                VQ1=-1;  
	              ROut=LastOut;  
	              j=(*VSize > 2) ? 2 : 1;  
		            subSpezReg(2,u+j);
	              switch(RQ) {
	                case 0:
	                case 1:
	                  break;
	                case 2:
#if MICROCHIP
                    PROCReadD0(RVar,0,0,0,0);
#else
                    PROCReadD0(RVar,0,0,0);
#endif
	                  break;
	                case VALUE_IS_COSTANTE:
#if ARCHI
	                  if((RCost.l>255) || (OP==3))    // OP=3 (*) 
                      goto myURcost;
	                  else 
	                    T=2;
#elif Z80  || I8051 || MICROCHIP
//	                  if(*VQ>1) {
//	                    if((*VVar)->classe==4) {
//                        goto myURcost;
//                        }
//                      }  
                    if(OP==3)
                      goto myURcost;
                    else
                      T=2;
#elif I8086
                    if(OP==3 && CPU86<2)
                      goto myURcost;
                    else
                      T=2;
#elif MC68000
                    if(OP==3)
                      goto myURcost;
                    else
                      T=2;
#endif
	                  break;
	                case 9:
myURcost:	 
#if MICROCHIP
	                  PROCUseCost(RQ,RType,RSize,&RCost,0);		// FINIRE
#else
	                  PROCUseCost(RQ,RType,RSize,&RCost);
#endif
	                  break;
	                case 3:
	                  switch(RVar->classe) {
	                    case CLASSE_EXTERN:
                      case CLASSE_GLOBAL:
                      case CLASSE_STATIC:
#if ARCHI || Z80  || I8051 
	                      ReadVar(RVar,0,0,0);
#elif I8086
	                      ReadVar(RVar,0,0,0);
#elif MC68000
	                      ReadVar(RVar,0,0,0);
#elif MICROCHIP
	                      ReadVar(RVar,0,0,0,0);		// FINIRE
#endif
      									break;
	                    case CLASSE_REGISTER:
#if ARCHI
	                      sprintf(sRegs[j].Dr,"R%d",*(int *)RVar->label);
#elif Z80 || I8051 || MICROCHIP
	                      if(0/*OP==4*/) {
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=*(int *)(RVar->label);
		                      T=-1;  
		                      }
												else   {
#if MICROCHIP
	 	                      ReadVar(RVar,0,0,0,0);		// FINIRE
#else
	 	                      ReadVar(RVar,0,0,0);
#endif
													}
	                      // dovremmo riuscire a usare le istruzioni ADD IX ecc.
#elif I8086
	                      if(0/*OP==4*/) {
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=*(int *)(RVar->label);
		                      T=-1;  
		                      }
		                    else  
	 	                      ReadVar(RVar,0,0,0);
#elif MC68000
	                      if(0/*OP==4*/) {
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=*(int *)(RVar->label);
		                      T=-1;  
		                      }
		                    else  
	 	                      ReadVar(RVar,0,0,0);
#endif
	                      break;
											case CLASSE_AUTO:
#if ARCHI
 	                      ReadVar(RVar,0);
#elif Z80	|| I8051 || MICROCHIP
		                    if(/*!(*VType & 0x1d0f) && */ (T==0 && (OP==4 || OP>=8))/* || (OP==6 || OP==7)*/) {
		                    // dovrebbe prendere anche i char *...
		                      i=*(int *)RVar->label;
		                      u[j].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[j].s.n=0;
		                      u[j].ofs=i;
							            if(RSize>2) {
			                      u[j+1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                      u[j+1].s.n=0;
			                      u[j+1].ofs=i+2;
								            }
	                      	T=-1;
		                      }
												else {
#if MICROCHIP
	 	                      ReadVar(RVar,0,0,0,0);		// FINIRE
#else
	 	                      ReadVar(RVar,0,0,0);
#endif
													}
#elif I8086
		                    if(/*!(*VType & 0x1d0f) && */ (T==0 && (OP==4 || OP>=8))/* || (OP==6 || OP==7)*/) {
		                    // dovrebbe prendere anche i char *...
		                      i=*(int *)RVar->label;
		                      u[j].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[j].s.n=0;
		                      u[j].ofs=i;
							            if(RSize>2) {
			                      u[j+1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                      u[j+1].s.n=0;
			                      u[j+1].ofs=i+2;
								            }
	                      	T=-1;
		                      }
	                      else 
	 	                      ReadVar(RVar,0,0,0);
#elif MC68000
												// tolto qua tutto quel controllo... non so bene a cosa servisse, forse per ottimizzare

	 	                      ReadVar(RVar,0,0,0);
#endif
												break;
	                    }
	                  break;  
	                default:
	                  PROCAssignCond(&RQ,&RType,&RSize,Rlabel);
	                  break;
                  }
//                if(RVar >= 0) {
//                  PROCCast(*VType,*VSize,RType,RSize);
//                  }
	              if(!SavedR) {
		              if(J2 && (LastOut!=ROut)) {
		                swap(&ROut,&LastOut);
		                SavedR=FNGetMemSize(*VType,*VSize,0);
		                Regs->Save(SavedR);
		                swap(&ROut,&LastOut);
		                }
	                }
                switch(SavedR) {
                  case 0:
                    break;
                  case 1:
                    TempProg+=STACK_ITEM_SIZE;
                    i=AutoOff-TempProg;
                    u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
                    u[0].ofs=i;
                    u[0].s.n=0;
//                    FNGetFPStr(sRegs[0].Dr,i,NULL);
#if ARCHI
#elif Z80 || I8051 || MICROCHIP
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#elif I8086
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#elif MC68000
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#endif
                    break;
                  case 2:
                    TempProg+=STACK_ITEM_SIZE;
                    i=AutoOff-TempProg;
                    u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
                    u[1].ofs=i;
                    u[1].s.n=0;
//                    FNGetFPStr(sRegs[1].Dr,i,NULL);
//                    FNGetFPStr(sRegs[1].Drh,i+1,NULL);
#if ARCHI
#elif Z80 || I8051 || MICROCHIP
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
#elif I8086
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#elif MC68000
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#endif
                    break;
                  case 4:
                    TempProg+=4;
                    i=AutoOff-TempProg;
//                    FNGetFPStr(sRegs[2].Dr,i,NULL);
//                    FNGetFPStr(sRegs[2].Drh,i+1,NULL);
//                    FNGetFPStr(sRegs[3].Dr,i+2,NULL);
//                    FNGetFPStr(sRegs[3].Drh,i+3,NULL);
										u[3].mode=u[2].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
//										u[2].s=0;
//										u[3].s=0;
										u[2].ofs=i;
										u[3].ofs=i+2;
#if ARCHI
#elif Z80 || I8051 || MICROCHIP
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
#elif I8086
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[3].mode,&u[3].s,u[3].ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
#elif MC68000
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[2].mode,&u[2].s,u[2].ofs,OPDEF_MODE_REGISTRO32,Regs->D);
#endif
                    break;
                  default:
                    break;
                  }
	              if(SavedR) {
	                TempSize=__max(TempSize,TempProg);
	                Regs->Get();
	                SavedR=0;
	                }
//	              if(T==-1) {
//	                T=0;
//	                }
                switch(OP) {
                  case 3:
				            *cond=0;
		                *VQ=subMul(*TS,T,*VQ,*VVar,*VType,*VSize,RQ,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
                    break;
                  case 4:
				            *cond=0;
		                *VQ=subAdd(*TS=='+',T,*VQ,*VVar,VType,VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3],0);
			              break;
		              case 5:
				            *cond=0;
		                *VQ=subShift(*TS=='<',T,*VType,*VSize,RType,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
		                break;
                  case 6:
                  case 7:
				            *cond=0;
		                *VQ=subCMP(TS,*cond,T,*VQ,*VVar,*VType,*VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
                    break;
                  default:
//				            *cond=0;
		                *VQ=subAOX(*TS,cond,T,*VQ,*VVar,*VType,*VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
                    break;
                  }
#if ARCHI || Z80
  	            if((/*T>=0 &&*/ ((VQ1==1) || (VQ1==2) || (VQ1==3))) || OP==3)
#elif I8086
  	            if(/*T>=0 &&*/ ((VQ1==1) || (VQ1==2) || (VQ1==3)))
#elif MC68000
  	            if(/*T>=0 &&*/ ((VQ1==1) || (VQ1==2) || (VQ1==3)))
#elif MICROCHIP
  	            if((/*T>=0 &&*/ ((VQ1==1) || (VQ1==2) || (VQ1==3))) || OP==3)
#endif  	            
                  {
                  if(!J2)
                    Regs->Dec(FNGetMemSize(*VType,*VSize,0));
                  }
                }  
              break;
            case 11:
            case 12:
//myLog->print(0,"OP logico: entro al livello %d con %d (cond è %d)\n",Pty,*VQ,cond);
              T=1;
							if(*VQ==2) {
#if MICROCHIP
						    PROCReadD0(*VVar,0,0,*cond & 0xff,0);
#else
						    PROCReadD0(*VVar,0,0,*cond & 0xff);
#endif
								}
              if(*VQ==3) {
#if MICROCHIP
                ReadVar(*VVar,0,0,*cond & 0xff,0);		// FINIRE
#else
                ReadVar(*VVar,0,0,*cond & 0xff);
#endif
								}
              if(*VQ & 8) {   // auto ottim. costanti (forza T=0)
						    PROCWarn(4127,VCost->s);
                if(*VQ==VALUE_IS_COSTANTE) {
                  if(VCost->l) {
                    if(OP==12)
                      T=0;
                    else 
                      T=2;        
                    }
                  else {
                    if(OP==11)
                      T=0;
                    else 
                      T=2;
                    }
                  }
                else {
                  if(OP==12)
                    T=0;
                  else 
                    T=2;
                  }
                }
						  ROut=LastOut;
  	          if(T) {
  	            if((/*!*cond ||*/(oOP && oOP!=12) && (OP==12)) && *VQ) {
    		          PROCOutLab(Clabel);
    		          }
    		        _tcscpy(Rlabel,Clabel);
    		        *cond |= 1;
	              T1=FNRev(OP-1,cond,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
	              myLog->print(0,"oOP era %d, Dopo il logico c'è %d\n",oOP,T1);
	              if((T1 == 12) || ((!(*VQ & 0x40)) && (/* !*cond || */ OP==12)))		// solo se ||
	                FNGetLabel(Clabel,2);
	              if(T==1) {
                  *VQ &= 0xbf;
  		            if(i=(ROut!=LastOut)) {
  		              swap(&ROut,&LastOut);
  		              }
									if(RQ==VALUE_IS_COSTANTE) {		// se c'è una costante...
										if(OP==11) {		// se &&
											if(!RCost.l) {		// e costante = 0
#if MC68000
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO,0,OPDEF_MODE_REGISTRO,Regs->D);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#else
			                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,0);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#endif
//												RQ=0x40;
//												PROCOutLab(Clabel);
												goto skippa_condbranch;
												}
											}
										else {		// se ||
											if(RCost.l) {		// e costante != 0
#if MC68000
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO,1,OPDEF_MODE_REGISTRO,Regs->D);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#else
			                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,1);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#endif
												RQ=0x40;
//												PROCOutLab(Clabel);
												goto skippa_condbranch;
												}
											}
										}
	                PROCGenCondBranch(Clabel,(OP==11),VQ,FNGetMemSize(*VType,*VSize,0));
skippa_condbranch: ;
  		            if(i)
  	  	            swap(&ROut,&LastOut);
	                }
	              T1=1;

/*	              if(RQ==3) {
	                ReadVar(RVar,0,0,*cond & 0xff);
//	                RQ=0x85;
	                }
	              if(RQ & 8) {
	                if(RQ==8) {
	                  if(RCost.l) {
	                    if(OP==12)
	                      T1=0;
	                    else 
	                      T1=2;
	                    }
	                  else {
	                    if(OP==11)
	                      T1=0;
	                    else 
	                      T1=2;
	                    }
	                  }
	                else {
	                  if(OP==12)
	                    T1=0;
	                  else
	                    T1=2;
	                  }
//                  RQ=0x85;
	                }
	                */
//	              if(RQ & 0x80) {
	                *VQ=RQ;
	                *VVar=RVar;
	                *VSize=RSize;
	                *VType=RType;
									*VCost=RCost;
//	                myLog->print(0,"RQ vale %x\n",RQ);
//	                }
	              if(!T1)
	 	              *VQ=0;
  	            }
	            if((/*!*cond ||*/ (oOP==12 || OP==12)) && *VQ) {
	              *VQ |= 0x40;
  		          }
//	            else
//	              *VQ=0;

//myLog->print(0,"OP logico: esco con %d\n\a",*VQ);
              oOP=OP;
	            break;

            case 13:
							if(*VQ==3) {
#if MICROCHIP
		            ReadVar(*VVar,0,0,1,0);		// FINIRE
#else
		            ReadVar(*VVar,0,0,1);
#endif
								}
							else if(*VQ==2) {
#if MICROCHIP
						    PROCReadD0(*VVar,0,0,1,0);
#else
						    PROCReadD0(*VVar,0,0,1);
#endif
								}
//		          if(*VQ<0) {
		          else if(*VQ & 8) {      // boh autoottimizza cost..
#if MICROCHIP
						    PROCUseCost(*VQ,*VType,*VSize,VCost,0);		// FINIRE
#else
						    PROCUseCost(*VQ,*VType,*VSize,VCost);
#endif
						    PROCWarn(4127);
/*
		            if((*VQ==VALUE_IS_COSTANTE) && (!VCost->l)) {
		              *AS=0;
		              T=0;
		              while((*AS != ':') && (!T)) {
		                FNLO(AS);
		                if(*AS=='(')
		                  T++;
		                if(*AS==')')
		                  T--;                     
		                }
		              }
		            subEvEx(13,0,VType,VSize,VQ,VVar,VCost,VTag,VDim);
		            */
		            }
		          FNGetLabel(TS,2);
		          PROCGenCondBranch(TS,TRUE,VQ,FNGetMemSize(*VType,*VSize,0));
		          *cond=0;
		          subEvEx(13,cond,Clabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
#if ARCHI
//		          PROCOut("B",TS,"_");
							_tcscpy(MyBuf,TS);
							_tcscat(MyBuf,"_");
		          PROCOper(LINE_TYPE_JUMPC,"jr",OPDEF_MODE_CONDIZIONALE,(union SUB_OP_DEF *)MyBuf,0);
#elif Z80     
							_tcscpy(MyBuf,TS);
							_tcscat(MyBuf,"_");
		          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,(union SUB_OP_DEF *)MyBuf,0);
#elif I8086
							_tcscpy(MyBuf,TS);
							_tcscat(MyBuf,"_");
		          PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,(union SUB_OP_DEF *)MyBuf,0);
#elif MC68000
							_tcscpy(MyBuf,TS);
							_tcscat(MyBuf,"_");
		          PROCOper(LINE_TYPE_JUMPC,"bra.s"/*jmpString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)MyBuf,0);
#elif MICROCHIP
							_tcscpy(MyBuf,TS);
							_tcscat(MyBuf,"_");
		          PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,(union SUB_OP_DEF *)MyBuf,0);
#endif
		          PROCOutLab(TS);
		          PROCCheck(':');
		          subEvEx(13,cond,Clabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
		          PROCOutLab(TS,"_",NULL);
		          *VType=RType;          // andrebbero usate entrambe le expr
		          *VSize=RSize;
		          *VVar=0;
		          *VQ=1;
		          break;

		        case 14:
              if(Pty<14 || (*VQ & OPDEF_MODE_FRAMEPOINTER_INDIRETTO))           // bloccare le expr e cost a sinistra (a+3=b)
	              PROCError(2106);
						  ROut=LastOut;
		          if(*VQ==1 || *TS!='=') {
//		            PROCOut("; fine =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
		            i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
		            }
		          else if(*VQ==2) {     // separato da VQ=1, per il ptr *
//		            PROCOut("; fine =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
								if((*VVar)->modif) {    // se devo sommare un ofs reg, lo faccio ora
									subOfsD0(*VVar,(*VVar)->size,(int)(*VVar)->func,(int)(*VVar)->parm);
									(*VVar)->modif=0;
									(*VVar)->parm=0;
									}
		            i=Regs->Inc(PTR_SIZE);
		            }
		          *cond=FALSE;
		          FNRev(14,cond,Rlabel,&RType,&RSize,&RQ,&RVar,&RCost,&RTag,&RDim);
		          if(RQ==3 && (*VQ!=3 || (*VVar)->classe != CLASSE_REGISTER)) {   // store in registri a parte...
                switch(RVar->classe) {
	                case CLASSE_EXTERN:
                  case CLASSE_GLOBAL:
                  case CLASSE_STATIC:
#if MICROCHIP
                    ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0,0);		// FINIRE
#else
                    ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0);
#endif
                    T=0;
  									break;
                  case CLASSE_REGISTER:
#if MICROCHIP
	  			          ReadVar(RVar,0,0,0,0);		// FINIRE
#else
	  			          ReadVar(RVar,0,0,0);
#endif
                    T=0;
                    break;
									case CLASSE_AUTO:
									  if(*TS == '=' /*|| *TS=='*' || *TS=='/' || *TS=='%'  && *VQ==3 && (*VVar)->classe<3*/) {
#if MICROCHIP
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0,0);		// FINIRE
#else
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0);
#endif
									    T=0;
		  			          }
		  			        else {
//                      ReadVar(RVar,0,FNGetMemSize(*VType,*VSize,1),0);
		  			          T=1;
		  			          }
										break;
                  }
                }  
              else
                T=0;  
		          if(RQ & 0x80) {
								if(debug)
									myLog->print(0,"ASSIGNCOND: =%d\n\a",RQ);
		            PROCAssignCond(&RQ,&RType,&RSize,Rlabel);
								}
			        else if(RQ==9) {             // tratto le costanti int. a parte
	              RSize=*VSize;
#if MICROCHIP
	              PROCUseCost(RQ,RType,RSize/*cioè VSize*/,&RCost,0);		// FINIRE
#else
	              PROCUseCost(RQ,RType,RSize/*cioè VSize*/,&RCost);
#endif
		            }
						  else if(RQ==2) {
//						  myLog->print(0,"Read D0: %lx\n",RVar);
#if MICROCHIP
						    PROCReadD0(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0,0);
#else
						    PROCReadD0(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0);
#endif
						    }
              else if(RQ==1)
    					  PROCCast(*VType,*VSize,RType,RSize);
              else if(RQ==VALUE_IS_COSTANTE) {
						    }
		          if(*VQ==1 || *TS!='=') {
//                PROCOut("; inizio =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
		            if(!i)
		              Regs->Dec(FNGetMemSize(*VType,*VSize,0));
		            }
		          else if(*VQ==2) {
//                PROCOut("; inizio =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
		            if(!i)
		              Regs->Dec(PTR_SIZE);
		            }
		          switch(*TS) {
		            case '=':        // OCCHIO: fa casino se assegno dentro una cond...
				          if((RType & (VARTYPE_UNION | VARTYPE_STRUCT)) && (*VType & (VARTYPE_UNION | VARTYPE_STRUCT)) && (!(RType & VARTYPE_IS_POINTER)) && (!(*VType & VARTYPE_IS_POINTER))) {
				            if(*VTag != RTag)
				              PROCError(2115);
    		            i=Regs->Inc(PTR_SIZE);
				            if(*VQ==3) {
				              if(!((*VVar)->type & VARTYPE_IS_POINTER) && ((*VVar)->type & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_FUNC /*0x1d00*/))) 
  				              PROCError(2106);   // dovrebbe bloccare i non lvalue a sinistra
											else {
#if MICROCHIP
  				              ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
  				              ReadVar(*VVar,0,0,0);
#endif
												}
				              }
    		            if(!i)
    		              Regs->Dec(PTR_SIZE);
#if ARCHI
				            FNGetLabel(TS,2);
				            Regs->Save();
				            PROCOutLab(TS);     
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,"R2",RSize);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB R3,[R0,R2]",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"STRB R3,[R1,R2]",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"SUBS R2,R2,#1",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"BNE",TS);
				            Regs->Get();
#elif Z80
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,RSize);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"ldir",OPDEF_MODE_NULLA,0);
#elif I8086
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,RSize);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
#elif MC68000
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,RSize);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
#elif MICROCHIP
				            FNGetLabel(TS,2);
				            Regs->Save();
				            PROCOutLab(TS);     
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,RSize);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB R3,[R0,R2]",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"STRB R3,[R1,R2]",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"SUBS R2,R2,#1",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"DECFSZ",OPDEF_MODE_NULLA,NULL,0,TS);
				            Regs->Get();
#endif
				            }
				          else {
				            if(*VSize > 2 || RSize > 2) {
				              if((*VSize != RSize) && !((*VType | RType) & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_IS_POINTER | VARTYPE_FUNC /*0x1d0f*/)))  // integrali di diff. grandezza
				                PROCWarn(4761);
				              }
				            if(*VQ & OPDEF_MODE_FRAMEPOINTER_INDIRETTO || *VQ==1)
				              PROCError(2106);
				            else if(*VQ==3) {
				              if(!((*VVar)->type & VARTYPE_IS_POINTER) && ((*VVar)->type & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_FUNC /*0x1d00*/))) 
  				              PROCError(2106);
  				            else  
				                StoreVar(*VVar,RQ,RVar,&RCost);
				              }
//				            if(*VQ==1)
//				              PROCAssign(ROut/*LastOut*/,*VQ,*VVar,*VType,*VSize,RQ,&RCost);
//											PROCError(1001,"store in hl, no ptr");
				            else if(*VQ==2)
				              PROCStoreD0(*VVar,RQ,RVar,&RCost);
				            }
    		          *VQ=1;
			            break;
		            case '+':
		            case '-':
                  subSpezReg(FNGetMemSize(*VType,*VSize,0),u);
		              if(T) {
	                	i=*(int *)RVar->label;
		                if(FNGetMemSize(*VType,*VSize,1) <=2) {
		                  u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                  u[1].ofs=i;
//		                  u[1].s=0;
	                    }
	                  else {  
		                  u[2].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                  u[2].ofs=i;
//		                  u[2].s=0;
		                  u[3].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                  u[3].ofs=i+2;
//		                  u[3].s=0;
                      }
                    }
                  if(RQ==VALUE_IS_COSTANTE)
                    j=2;
                  else {
                    j=0;
                    if(RQ==9) {
#if MICROCHIP
   	                  PROCUseCost(RQ,RType,RSize,&RCost,0);		// FINIRE
#else
   	                  PROCUseCost(RQ,RType,RSize,&RCost);
#endif
											}
                    }
			            switch(*VQ) {
			              case 3:
			              switch((*VVar)->classe) {
			                case CLASSE_AUTO:
			                	i=MAKEPTRREG((*VVar)->label);
			                	u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                	u[0].ofs=i;
    		                if(FNGetMemSize(*VType,*VSize,1) > 2) {
				                  u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
				                  u[1].ofs=i+2;
//				                  u[1].s=0;
		                      }
			                  goto my_add;
			                case CLASSE_REGISTER:
			                  u[0].mode=OPDEF_MODE_REGISTRO;
			                  u[0].s.n=MAKEPTRREG((*VVar)->label);
//				                  u[0].s=0;
my_add:
				                *VQ=subAdd(*TS=='+',j,*VQ,*VVar,VType,VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3],1);
												if(Pty==14) {             // ritorna expr in hl se serve
#if MICROCHIP
  												ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
  												ReadVar(*VVar,0,0,0);
#endif
													}
			                  break;
			                default:
			                  // INVERTITI Dr e Dr1 (e h)
//							          if(RQ != VALUE_IS_COSTANTE)
//							            i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
#if MICROCHIP
  												ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
  												ReadVar(*VVar,0,0,0);
#endif

//							          if(RQ != VALUE_IS_COSTANTE)
//  			                  *VQ=subAdd(*TS=='+',j,*VQ,*VVar,VType,VSize,RType,RSize,VCost,&RCost,sRegs[1].Dr,sRegs[0].Dr,sRegs[1].Drh,sRegs[0].Drh,sRegs[3].Dr,sRegs[2].Dr,sRegs[3].Drh,sRegs[2].Drh);
//							          else
  			                  *VQ=subAdd(*TS=='+',j,*VQ,*VVar,VType,VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3],1);
			                  StoreVar(*VVar,*VQ,*VVar,VCost);
//							          if(RQ != 8) {
//  						            if(!i)
//  						              Regs->Dec(FNGetMemSize(*VType,*VSize,0));
//  						            }  
			                  break;
			                }
			              break;
			            case 2:        // non va se (de) o (bc)...
#if ARCHI
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=0x3;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif Z80
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif I8086
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MC68000
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MICROCHIP
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("ADDWF",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#endif
	                	if(FNGetMemSize(*VType,*VSize,0)>2) {
	                	  u[1]=u[0];
	                	  }
		                *VQ=subAdd(*TS=='+',j,*VQ,*VVar,VType,VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3],1);
										if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
									    PROCReadD0(*VVar,0,0,0,0);
#else
									    PROCReadD0(*VVar,0,0,0);
#endif
											}
			              break;
			            default:
			              PROCError(1001,"+=");
			              break;
			              }
			            break;
		            case '*':
		            case '/':
		            case '%':
                  subSpezReg(FNGetMemSize(*VType,*VSize,0),u);
			            switch(*VQ) {
			              case 3:
#if MICROCHIP
											ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
											ReadVar(*VVar,0,0,0);
#endif
//				            i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
	                  if(RQ & 8) {
	                    j=2;
// 	                    PROCUseCost(RQ,RType,RSize,&RCost);
											}
                    else {
#if MICROCHIP
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0,0);		// FINIRE
#else
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0);
#endif
                      j=0;
                      }
	                  *VQ=subMul(*TS,j,*VQ,*VVar,*VType,*VSize,RQ,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
//				            if(!i)
//				              Regs->Dec(FNGetMemSize(*VType,*VSize,0));
				            // caso unico: prima DEC di store
	                  StoreVar(*VVar,*VQ,*VVar,VCost);
			              break;
			            case 2:  		                   // non finito...
#if MICROCHIP
								    PROCReadD0(*VVar,0,0,0,0);
#else
								    PROCReadD0(*VVar,0,0,0);
#endif
	                  if(RQ & 8)
	                    j=2;
// 	                    PROCUseCost(RQ,RType,RSize,&RCost);
                    else {
#if MICROCHIP
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0,0);		// FINIRE
#else
		                  ReadVar(RVar,*VType,FNGetMemSize(*VType,*VSize,1),0);
#endif
                      j=0;
                      }
	                  *VQ=subMul(*TS,j,*VQ,*VVar,*VType,*VSize,RQ,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
	                  PROCStoreD0(*VVar,*VQ,*VVar,VCost);
			              break;
			            default:  
			              PROCError(1001,"*=");
			              break;
			              }
			            break;
		            case '<':
		            case '>':
                  subSpezReg(FNGetMemSize(*VType,*VSize,0),u);
		              if(T) {
//		                j=FNGetMemSize(*VType,*VSize,1)>2 ? 2 : 1;
	                	i=*(int *)RVar->label;
	                	u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[1].ofs=i;
	                	u[3].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[3].ofs=i+2;
                    }
                  if(RQ==VALUE_IS_COSTANTE)
                    j=2;
                  else {
                    j=0;
                    if(RQ==9) {
#if MICROCHIP
   	                  PROCUseCost(RQ,RType,RSize,&RCost,0);		// FINIRE
#else
   	                  PROCUseCost(RQ,RType,RSize,&RCost);
#endif
											}
                    }
			            switch(*VQ) {
			              case 3:
			              switch((*VVar)->classe) {
			                case CLASSE_AUTO:
			                	i=MAKEPTROFS((*VVar)->label);
			                	u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                	u[0].ofs=i;
    		                if(FNGetMemSize(*VType,*VSize,1) > 2) {
				                  u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
				                  u[1].ofs=i+2;
//				                  u[1].s=0;
		                      }
				                *VQ=subShift(*TS=='<',j,*VType,*VSize,RType,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
												if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
			                    ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
			                    ReadVar(*VVar,0,0,0);
#endif
													}
			                  break;
			                default:                // anche register
//							          if(RQ != 8)
//							            i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
#if MICROCHIP
		                    ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
		                    ReadVar(*VVar,0,0,0);
#endif
//							          if(RQ != 8) {
			                  // passo INVERTITI Dr e Dr1 (e h)
//  			                  *VQ=subShift(*TS=='<',j,*VType,*VSize,RType,VCost,&RCost,sRegs[1].Dr,sRegs[0].Dr,sRegs[1].Drh,sRegs[0].Drh,sRegs[3].Dr,sRegs[2].Dr,sRegs[3].Drh,sRegs[2].Drh);
//  			                  }
//  			                else  
  			                  *VQ=subShift(*TS=='<',j,*VType,*VSize,RType,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
			                  StoreVar(*VVar,*VQ,*VVar,VCost);
//							          if(RQ != 8) {
//  						            if(!i)
//  						              Regs->Dec(FNGetMemSize(*VType,*VSize,0));
//  						            }  
			                  break;
			                }
			              break;
			            case 2:  		                   // non va se (de) o (bc)...
#if ARCHI
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif Z80
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif I8086
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MC68000
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MICROCHIP
	                	u[0].s.n=(int)(*VVar)->func;
	                	if(u[0].s.n <= 3 && (int)(*VVar)->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("ADD",&u[0],(int)(*VVar)->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)(*VVar)->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#endif
		                *VQ=subShift(*TS=='<',j,*VType,*VSize,RType,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
										if(Pty==14) {             // ritorna expr in hl se serve
#if MICROCHIP
									    PROCReadD0(*VVar,0,0,0,0);
#else
									    PROCReadD0(*VVar,0,0,0);
#endif
											}
									  break;  
			            default:  
			              PROCError(1001,"<<=");
			              break;
			              }
			            break;
		            case '&':
		            case '|':
		            case '^':
                  subSpezReg(FNGetMemSize(*VType,*VSize,0),u);
		              if(T) {
//		                j=FNGetMemSize(*VType,*VSize,1)>2 ? 2 : 1;
	                	i=*(int *)RVar->label;
	                	u[3].mode=u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[1].ofs=i;
	                	u[3].ofs=i+2;
                    }
                  if(RQ==VALUE_IS_COSTANTE)
                    j=2;
                  else {
                    j=0;
                    if(RQ==9) {
#if MICROCHIP
   	                  PROCUseCost(RQ,RType,RSize,&RCost,0);		// FINIRE
#else
   	                  PROCUseCost(RQ,RType,RSize,&RCost);
#endif
											}
                    }
			            switch(*VQ) {
			              case 3:
			              switch((*VVar)->classe) {
			                case CLASSE_AUTO:
			                	i=MAKEPTROFS((*VVar)->label);
			                	u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                	u[0].ofs=i;
    		                if(FNGetMemSize(*VType,*VSize,1) > 2) {
				                  u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
				                  u[1].ofs=i+2;
//				                  u[1].s=0;
		                      }
			                  goto my_aox;
											case CLASSE_REGISTER:
			                  u[0].mode=OPDEF_MODE_REGISTRO;
			                  u[0].s.n=MAKEPTRREG((*VVar)->label);
//				                  u[0].s=0;
my_aox:
	                      i=0;
                        *VQ=subAOX(*TS,&i,j,*VQ,*VVar,*VType,*VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
												if(Pty==14) {             // ritorna expr in hl se serve
#if MICROCHIP
			                    ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
			                    ReadVar(*VVar,0,0,0);
#endif
													}
			                  break;
			                default:
			                  // passo INVERTITI Dr e Dr1 (e h)
//							          if(RQ != 8)
//							            i=Regs->Inc(FNGetMemSize(*VType,*VSize,0));
#if MICROCHIP
			                    ReadVar(*VVar,0,0,0,0);		// FINIRE
#else
			                    ReadVar(*VVar,0,0,0);
#endif
			                  i=0;
                        *VQ=subAOX(*TS,&i,j,*VQ,*VVar,*VType,*VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
			                  StoreVar(*VVar,*VQ,*VVar,VCost);
//							          if(RQ != 8) {
//  						            if(i)
//  						              Regs->Dec(FNGetMemSize(*VType,*VSize,0));
//  						            }  
			                  break;
			                }
			              break;
			            case 2:
	                	subOfsD0(*VVar,(*VVar)->size,(int)(*VVar)->func,(int)(*VVar)->parm);	
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
                    *VQ=subAOX(*TS,&i,j,*VQ,*VVar,*VType,*VSize,RType,RSize,VCost,&RCost,&u[0],&u[1],&u[2],&u[3]);
										if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
									    PROCReadD0(*VVar,0,0,0,0);
#else
									    PROCReadD0(*VVar,0,0,0);
#endif
											}
									  break;  
			            default:  
			              PROCError(1001,"<<=");
			              break;
			              }
			            break;
			          }
		          break;
		        case 15:
//		          subEvEx(14,cond,&RType,&RSize,&RQ,&RVar,RCost,&RTag,&RDim);     inutile
		          break;
		        default:
		          break;
		        }
	        }
	      break;
			default:
				Exit=TRUE;		// basterebbe uno dei due...
				goto fine;
				break;
	    }
    Co++;
    } while(!Exit);

  if(!AR)
    return AR;
  else 
  	return OP;
fine:
	return -1;
  }  
  
char *Ccc::ConRecEval(char *s, int Pty, long *l1) {
  long l2;
  char ch;
  int Go=0,InBrack=0,Times=0,fError=0;
  char *p;
  int i,j;
  
  do {
	  ch=*s;
//	  myLog->print(0,"sono sul %c(%x), T %d, pty %d\n",ch,ch,Times,Pty);
	  switch(ch) {
	    case '(':
	      s++;
	      InBrack++;
	      break;
	    case ')':
	      if(InBrack) {
  	      s++;
	        InBrack--;
	        }
	      else
	        Go=1;
	      break;
	    case '+':
	    case '-':
//			      myLog->print(0,"- ??unario: f %x,l %ld, pty %d, times %d\n",*f1,*l1,Pty,Times);
	      if(Times) {
  	      if(Pty > 4) {
            s++;
			      s=ConRecEval(s,4,&l2);
		        if(ch=='+')
		          *l1=*l1+l2;
		        else
	            *l1=*l1-l2;
		        }
		      else
		        Go=1;
	        }
	      else {
  	      if(Pty > 2) {
            s++;
			      s=ConRecEval(s,2,l1);
			      if(ch=='-')
			        *l1=-*l1;
			      }
		      else
		        Go=1;
		      }
	      break;
	    case '*':
	    case '/':
	    case '%':
	      if(Pty > 3) {
          s++;
		      s=ConRecEval(s,3,&l2);
	        switch(ch) {
	          case '*':
              *l1=*l1 * l2;
              break;
            case '/':
              *l1=*l1 / l2;
              break;
	          case '%':
              *l1=*l1 % l2;
              break;
	          }
	        }
        else
	        Go=1;
	      break;
	    case '!':
	    case '~':
	      if(Pty > 2) {
          s++;
		      s=ConRecEval(s,2,l1);
	        switch(ch) {
	          case '!':
              *l1=!*l1;
              break;
            case '~':
              *l1=~*l1;
              break;
	          }
	        }
        else
	        Go=1;
	      break;
	    case 'l':
	    case 'r':
	      if(Pty > 5) {
          s++;
		      s=ConRecEval(s,5,&l2);
	        switch(ch) {
	          case 'l':
              *l1=*l1 << l2;
              break;
            case 'r':
              *l1=*l1 >> l2;
              break;
	          }
	        }
        else
	        Go=1;
	      break;
	    case '<':
	    case '@':                // diverso
	    case '=':
	    case '>':
	      i=0;
	      if(Pty > 5) {
          s++;
          if(*s == '=') {
            i=1;
            s++;
            }
          else {
	          if(*s == '>') {
	            i=-1;
	            s++;
	            }
	          }  
		      s=ConRecEval(s,5,&l2);
	        switch(ch) {
	          case '<':
	            if(!i)
                *l1=*l1 < l2;
              else {
                if(i>0) 
                  *l1=*l1 <= l2;
                else
                  *l1=*l1 != l2;
                }
              break;
            case '=':
              *l1=*l1 == l2;
              break;
            case '@':
              *l1=*l1 != l2;
              break;
            case '>':
	            if(i)
                *l1=*l1 >= l2;
              else
                *l1=*l1 > l2;
              break;
            }
          }
        else
	        Go=1;
	      break;
	    case '&':                        // and,or
	    case '|':
	    case '^':
	      if(Pty > 6) {
          s++;
		      s=ConRecEval(s,6,&l2);
//	    myLog->print(0,"qui i valori sono %ld e %ld\n",*l1,l2);
	        switch(ch) {
	          case '&':
	            *l1=*l1 & l2;
	            break;
	          case '|':
              *l1=*l1 | l2;
              break;
	          case '^':
              *l1=*l1 ^ l2;
              break;
            }  
	        }
        else
	        Go=1;
	      break;
	    case ' ':
	      s++;
	      Times--;
	      break;
	    case 0:
	      Go=1;
	      break;
	    default:
	      *l1=0;
        if(*s && isdigit(*s)) {
	        while(*s && isdigit(*s)) {
		        *l1=(*l1 * 10) + (*s - '0');
		        s++;
		        }
	        }
	      else 
	        s++;
	      break;
	    }
	  Times++;
	  } while(!Go && !fError);

  return s;
  }
   
long Ccc::EVAL(char *s) {
  long l;

//  myLog->print(0,"\aUso di EVAL su %s..!\n",s);
  ConRecEval(s,15,&l);
//  myLog->print(0,"\aEVAL ritorna %ld\n",l);
  return l;
  }
          
