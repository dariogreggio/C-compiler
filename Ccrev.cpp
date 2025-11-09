#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <stdlib.h>
#include <ctype.h>
#include <math.h>


void Ccc::subEvEx(uint8_t Pty, int16_t *cond, char *Clabel, struct OPERAND *V) {
  
  FNRev(Pty,cond,Clabel,V);
	if(!(V->Q & VALUE_IS_CONDITION) && Pty>=14)		// se non condizionale e livello + esterno, esco 2025
		return;
  if((V->Q & 0xf) == VALUE_IS_D0) {
#if MICROCHIP
    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,V->cost->l,FALSE,0);
#elif MC68000
		if(!(V->Q & 0x20))		// già a posto qua..
	    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,V->cost->l,FALSE);
#else
    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,V->cost->l,FALSE);
#endif
		}
  else if((V->Q & 0xf) == VALUE_IS_VARIABILE) {
#if MICROCHIP
    ReadVar(V->var,0,0,(*cond & VALUE_CONDITION_MASK) ? TRUE : FALSE,FALSE,0);
#elif MC68000
		if(!(V->Q & 0x20))		// già a posto qua..
	    ReadVar(V->var,0,0,(*cond & VALUE_CONDITION_MASK) ? TRUE : FALSE,FALSE);
#else
    ReadVar(V->var,0,0,(*cond & VALUE_CONDITION_MASK) ? TRUE : FALSE,FALSE);
#endif
		}
  else if(V->Q & VALUE_IS_COSTANTE) {
#if MICROCHIP
    PROCUseCost(V->Q,V->type,V->size,V->cost,FALSE,0);		// FINIRE
#elif MC68000
		if(!(V->Q & 0x20))		// già a posto qua..
	    PROCUseCost(V->Q,V->type,V->size,V->cost,FALSE);
#else
    PROCUseCost(V->Q,V->type,V->size,V->cost,FALSE);
#endif
		}
  }
 
uint16_t Ccc::FNEvalExpr(uint8_t Pty, char *C) {
	int16_t i;
  struct VARS VPtr;
	struct OPERAND V;
  char Clabel[32];
  
	ZeroMemory(&V,sizeof(struct OPERAND));
	ZeroMemory(&VPtr,sizeof(struct VARS));
  ZeroMemory(C,sizeof(union STR_LONG));
	*Clabel=0;
  V.var=&VPtr;
	V.cost=(union STR_LONG*)C;
  GlblOut=LastOut;
  TempProg=0;
  i=0;
  subEvEx(Pty,&i,Clabel,&V);
// V.Q: -5 niente                      	0
//     0 per valore gen. in D0 o hl   	1
//     1 per valore da ptr in D0 o hl 	2 
//     2 variabile in *V              	3
//     -1 per costante integral       	8
//     -2 per altra costante          	9
//     -9 per ! condizionale      (bit 4)
//     -10..-15 per condizionale  (bit 7)
//     -20..-25 per condizionale multipla && ||   (bit 6)
// bit 5 indica comparazione tra signed (0) o unsigned (1)
  
  return V.size;                  // ritorno size espressione
  }
 
int Ccc::FNEvalECast(char *C, O_TYPE *T, O_SIZE *S) {
  int16_t i;
  struct VARS VPtr;
  struct OPERAND V;
  char Clabel[32];
  
  GlblOut=LastOut;
  ZeroMemory(&V,sizeof(struct OPERAND));
	ZeroMemory(&VPtr,sizeof(struct VARS));
  ZeroMemory(C,sizeof(union STR_LONG));
	*Clabel=0;
	V.var=&VPtr;
	V.cost=(union STR_LONG *)C;
  TempProg=0;
  i=0;
  FNRev(15,&i,Clabel,&V);
  if(*S) {                    // se ho newSize, faccio cast...
		if(V.Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
	    ReadVar(V.var,*T,*S,0,0,FALSE);		//FINIRE
#else
	    ReadVar(V.var,*T,*S,0,FALSE);
#endif
			}
		else if(V.Q==VALUE_IS_D0) {
#if MICROCHIP
	    PROCReadD0(V.var,*T,*S,0,0,0,FALSE);
#else
	    PROCReadD0(V.var,*T,*S,0,0,FALSE);
#endif
			}
		else if(V.Q & VALUE_IS_COSTANTE) {
#if MICROCHIP
	    PROCUseCost(V.Q,*T,*S,(union STR_LONG *)C,0,FALSE);		// FINIRE
#else
	    PROCUseCost(V.Q,*T,*S,(union STR_LONG *)C,FALSE);
#endif
			}
	  else if(V.Q==VALUE_IS_ALTRO)
	    PROCCast(*T,*S,V.type,V.size);
	  }
	else {                      // altrimenti no cast e ritorno i valori T & S
		if(V.Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
	    ReadVar(V.var,V.type,V.size,0,0,FALSE);	// FINIRE
#else
	    ReadVar(V.var,V.type,V.size,0,FALSE);
#endif
			}
		else if(V.Q==VALUE_IS_D0) {
#if MICROCHIP
	    PROCReadD0(V.var,V.type,V.size,0,0,0,FALSE);
#else
	    PROCReadD0(V.var,V.type,V.size,0,0,FALSE);
#endif
			}
		else if(V.Q & VALUE_IS_COSTANTE) {
#if MICROCHIP
	    PROCUseCost(V.Q,V.type,V.size,(union STR_LONG *)C,0,FALSE);		// FINIRE
#else
	    PROCUseCost(V.Q,V.type,V.size,(union STR_LONG *)C,FALSE);
#endif
			}
	  *T=V.type;
	  *S=V.size;
	  }  
  return 0;
  }
 
int Ccc::FNEvalCond(char *C, const char *TS, uint16_t cond) {
  int8_t i;
	int16_t i2;
  struct VARS VPtr;
  struct OPERAND V;
  char MyBuf[128];
    
  GlblOut=LastOut;
  ZeroMemory(&V,sizeof(struct OPERAND));
	ZeroMemory(&VPtr,sizeof(struct VARS));
	ZeroMemory(MyBuf,sizeof(union STR_LONG));
	V.var=&VPtr;
	V.cost=(union STR_LONG *)MyBuf;
//  *C=0;                   // gli stmt passano qui la label per && e ||
  TempProg=0;
  i2=1;
  subEvEx(15,&i2,C,&V);
// aggiungo qui un Read VAR reso intelligente dal fatto che si ha un IF o un expr normale...    
  if(V.Q & VALUE_IS_COSTANTE) {
    PROCWarn(4127);
// in questo caso bisognerebbe anche stroncarlo...    
    }

  i=V.Q & ~VALUE_HAS_CONDITION;      // tolgo cond. multipla
#if MC68000
	//ev. qua si potrebbe usare Scc, Set su condizione??
	PROCGenCondBranch(TS,cond,&i,FNGetMemSize(V.type,V.size,0/*dim*/,0));
#else
	PROCGenCondBranch(TS,cond,&i,FNGetMemSize(V.type,V.size,0/*dim*/,0));
#endif
	if(V.Q & VALUE_HAS_CONDITION) {
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
void Ccc::subSpezReg(uint8_t S, struct OP_DEF *u) {

  _tcscpy(d[0].Dr,Regs->DS);
  _tcscpy(d[1].Dr,Regs->D1S);
  if(S>2) {
	  _tcscpy(d[2].Dr,Regs->D2S);
	  _tcscpy(d[3].Dr,Regs->D3S);
    }
  }
#elif Z80
void Ccc::subSpezReg(uint8_t S, struct OP_DEF *u) {

//no!	ZeroMemory(u,sizeof(struct OP_DEF)*4);
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
void Ccc::subSpezReg(uint8_t S, struct OP_DEF *u) {

//no!	ZeroMemory(u,sizeof(struct OP_DEF)*4);
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
void Ccc::subSpezReg(uint8_t S, struct OP_DEF *u) {

//no!	ZeroMemory(u,sizeof(struct OP_DEF)*4);
	// bah qua??
  u[0].mode=OPDEF_MODE_REGISTRO;
  u[0].s.n=Regs->D;
  u[1].mode=OPDEF_MODE_REGISTRO;
  u[1].s.n=Regs->D+1;
  if(S>2) {
	  u[2].mode=OPDEF_MODE_REGISTRO;
	  u[2].s.n=Regs->D+2;
	  u[3].mode=OPDEF_MODE_REGISTRO;
	  u[3].s.n=Regs->D+3;
    }

  }
#elif MICROCHIP
void Ccc::subSpezReg(uint8_t S, struct OP_DEF *u) {

//no!	ZeroMemory(u,sizeof(struct OP_DEF)*4);
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

int Ccc::FNRev(int8_t Pty,int16_t *cond,char *Clabel,struct OPERAND *V) {
  int i,j,I;
	O_TYPE T1;
	uint32_t T;
	int8_t v;
  int AR,reg2,SavedR;
	int8_t OP,oOP,Co=0;
	bool Exit=FALSE;
  int VQ1;
  char Rlabel[32];
  char AS[32],*BS,B1S[32],TS[32],T1S[32],MyBuf[32],MyBuf1[32];
  char *p1;
  struct LINE *ROut,*t;
	struct VARS RPtr;
	struct OPERAND R;
  union STR_LONG RCost;
  struct VARS *VPtr;
  long OT,l,l1;
	long ol;
//	int8_t isWhat=0;		// 0 inizio, 1=value, 2=operand; (v. anche EVAL   alla fine forse non serve!
  struct OP_DEF u[4];
//  int BrackOP[10],BrackPty[10];
 
	*Rlabel=0;
  ZeroMemory(&R,sizeof(struct OPERAND));
  ZeroMemory(&RPtr,sizeof(struct VARS));
  ZeroMemory(&u,sizeof(u));
  ZeroMemory(&RCost,sizeof(union STR_LONG));
//  RVar=&RPtr;  // preparata per i ptr..
  VPtr=V->var;  // salvo quello che arriva... (usato da chi crea PTR)  MA SERVE ANCORA?? 2025
	R.cost=&RCost;

	if(__line__== 192) {
//		isWhat=0;			// DEBUG BREAK
//		ol=0;
		}

//  ROut=LastOut;
  oOP=0;
  SavedR=0;
  do {
	  R.var=&RPtr;        // recupero RVar alterata   IDEM serve ancora??
    OT=FIn->GetPosition();
		FIn->SavePosition();
		ol=__line__;
    AR=FNGetAritElem(&OP,TS,V,Co);
// AR% = 1 SE COSTANTE, 2 SE VARIABILE, 3 SE OPERANDO, 0 SE FINE LINEA, -1 se errore!
   if(debug>2) 
     myLog->print(0,"LETTO AritmElem : %d, Brack %d\n",AR,Brack);

    switch(AR) {
      case ARITM_IS_EOL:
        Exit=TRUE;
        break;
      case ARITM_IS_COSTANTE:
        V->tag=NULL;
        ZeroMemory(V->dim,sizeof(V->dim));
//     myLog->print(0,"LETTO AritmElem : %ld\n",V->cost.l);
//        _tcscpy(V->cost,TS);
				if(Co)
//				if(isWhat==1)
					PROCError(2059,TS);
//				isWhat=1;
        break;
      case ARITM_IS_VARIABILE:
//      	*V->cost=0;
        V->tag=V->var->hasTag;
        memcpy(V->dim,V->var->dim,sizeof(V->dim));
				if(Co)
//				if(isWhat==1)
					PROCError(2059,V->var->name);
//				isWhat=1;
        break;
      case ARITM_IS_OPERANDO:
        if(OP>Pty) {
          if(OP>=3 && OP<=10) {          // se c'è un operatore (*,+,<=,&...), TOLGO cond subito!
            *cond=0;           // in realtà anche alcuni op.2 andrebbero tolti...
            }
					if(!Co)
						PROCError(2059,TS);
          Exit=TRUE;
//          FIn->Seek(OT,CFile::begin);
					FIn->RestorePosition(OT);
//					__line__=ol;
          }
        else {
          *T1S=0;
          switch(OP) {
            case 1:
              switch(*TS) {
                case '(':
                  if(Co>0) {
                    if(V->type & VARTYPE_FUNC) {
#if MC68000
                      PROCUsaFun(V->var,Regs->D>1,Pty>2 && Pty<14);		//2025, Pty solo se operazione binary
#else
                      PROCUsaFun(V->var,Regs->D>0,Pty>2 && Pty<14);		//2025, Pty solo se operazione binary
#endif
//                      V->size=V->var->size;
                      V->type &= ~(VARTYPE_FUNC | VARTYPE_FUNC_USED | VARTYPE_FUNC_BODY) /*0xfffffc7f*/;
                      V->Q=1;
                      }
                    else
                      PROCError(2064);
                    }
                  else {
                    if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) {     // cast
											O_DIM d;
											int16_t i2;
                      l1=FIn->GetPosition();
                      FNLO(MyBuf);
                      V->type=VARTYPE_PLAIN_INT;
                      V->size=0;
											ZeroMemory(V->var,sizeof(struct VARS));
                      PROCGetType(&V->type,&V->size,&V->tag,d,l1);
                      PROCCheck(')');
                      i2=0;
										  FNRev(2,&i2,Rlabel,&R);
										  if(R.Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
										    ReadVar(R.var,V->type,V->size,0,0,V->type & VARTYPE_IS_POINTER ? (Pty<=14 ? TRUE : FALSE) : FALSE);		//FINIRE
#else
												ReadVar(R.var,V->type,V->size,0,V->type & VARTYPE_IS_POINTER ? (Pty<=14 ? TRUE : FALSE) : FALSE);
#endif
										    V->Q=1;
										    }
										  else if(R.Q==VALUE_IS_ALTRO) {
                      	PROCCast(V->type,V->size,R.type,R.size);
                      	V->Q=VALUE_IS_ALTRO;
                      	}
										  else if(R.Q==VALUE_IS_D0) {
#if MICROCHIP
                      	PROCReadD0(R.var,V->type,V->size,0,0,0,FALSE);
#else
                      	PROCReadD0(R.var,V->type,V->size,0,0,FALSE);
#endif
										    V->Q=VALUE_IS_ALTRO;
										    }
										  else if(R.Q & VALUE_IS_COSTANTE) {
										    V->cost=R.cost;
										    V->Q=R.Q;
										    }
//										    PROCUseCost(RQ,R.type,R.size,&R.cost);
                      }
                    else {
//                      BrackOP[Brack]=OP;
//                      BrackPty[Brack]=Pty;
//                      Pty=99;
                      Brack++;
//                      if(Brack>=10)
//												PROCError(1035);                      
                      FNRev(99,cond,Clabel,V);
//                      FNRev(99,cond,&R.type,&R.size,&RQ,&RVar,&R.cost,&RTag,&RDim);
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
//										isWhat=0;
                    }
                  else {
                    FIn->unget(')');		//FIn->Seek(-1,CFile::current);
//                    Exit=TRUE;
//										isWhat=0;
                    }
                  Exit=TRUE;
                  break;
                case ']':
                  FIn->unget(']');		//FIn->Seek(-1,CFile::current);
                  Exit=TRUE;
          // FORSE SAREBBE MEGLIO METTERCI UN IDENT. DI "[" PENDENTE
                  break;
                case '[':
                  reg2=Regs->D;         // reg. per ReadD0
                  l=0;
                  v=0;               // flag per ReadD0
                  T=0;               // 1 quando leggo la base-array
                  T1=V->type;
									R.flag = V->type & VARTYPE_IS_POINTER;			// livelli di indirezione, ricorsivi a cascata
                  if(V->Q==VALUE_IS_VARIABILE || V->Q==VALUE_IS_COSTANTEPLUS) {		// per consentire uso di stringhe come array, credo...
                    T1 |= VARTYPE_ARRAY;
                    }  
                  for(;;) {
	                  if(T1 & VARTYPE_IS_POINTER) {
											R.type=T1;	// verrà usato per gli indici a seguire, se ci sono
	                    T1=(T1 & VARTYPE_NOT_A_POINTER) | ((T1 & VARTYPE_IS_POINTER) -1);
											// v. anche R.flag



	                    i=Regs->Inc((uint8_t)FNGetMemSize(T1,V->size,0/*dim*/,0));
// NON fare se costante su 68000! spostare sotto e v sotto altro

	                    if(i)
	                      PROCError(1035);
	  	                j=*cond;
		                  *cond = VALUE_CONDITION_UP;
	//                    *cond=0;
	                    FNRev(15,cond,Rlabel,&R);
	                    if(!i)
	                      Regs->Dec((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
		                  if(!T && (R.Q>=VALUE_IS_ALTRO && R.Q<=VALUE_IS_VARIABILE)) {
			                  if(V->Q==VALUE_IS_VARIABILE) {
			                    if(V->type & VARTYPE_ARRAY) {
			                      PROCGetAdd(VALUE_IS_VARIABILE,V->var,l,TRUE);
//											myLog->print(0,"faccio GETADD 1 con ofs %d\n",l);
			                      l=0;
			                      }
			                    else if(V->type & VARTYPE_POINTER) {
				                    if(V->var->classe == CLASSE_REGISTER)
						                  reg2=MAKEPTRREG(V->var->label);
														else {
#if MICROCHIP
			                        ReadVar(V->var,0,0,0,0,TRUE);		// FINIRE
#else
			                        ReadVar(V->var,0,0,0,TRUE);
#endif
															}
			                      }
			                    T=1;  
			                    }
			                  else if(V->Q == VALUE_IS_COSTANTEPLUS) {		// cose tipo "abcd"[1]
#if MICROCHIP
											    PROCUseCost(V->Q,V->type,V->size,V->cost,0,TRUE);		// FINIRE
#else
											    PROCUseCost(V->Q,V->type,V->size,V->cost,TRUE);
#endif
											// e ovviamente se costante pure l'indice si potrebbe ottimizzare! v.sotto
								          T=1;  
										      }
			                  }  


	                    i=Regs->Inc((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
// NON fare se costante su 68000! spostare sotto idem v.sopra



	                    if(i)
	                      PROCError(1035);
	                    switch(R.Q) {
	                      case VALUE_IS_ALTRO:
	                        PROCCast(VARTYPE_UNSIGNED,INT_SIZE,R.type,R.size);        // l'indice array dev'essere unsigned int
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case VALUE_IS_D0:
#if MICROCHIP
											    PROCReadD0(R.var,VARTYPE_UNSIGNED,INT_SIZE,*cond & VALUE_CONDITION_MASK,0,0,TRUE);
#else
											    PROCReadD0(R.var,VARTYPE_UNSIGNED,INT_SIZE,*cond & VALUE_CONDITION_MASK,0,TRUE);
#endif
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case VALUE_IS_VARIABILE:
#if MICROCHIP
	                        ReadVar(R.var,VARTYPE_UNSIGNED,INT_SIZE,0,TRUE,0);			// FINIRE
#else
	                        ReadVar(R.var,VARTYPE_UNSIGNED,INT_SIZE,0,FALSE);
#endif
													if(R.flag>1)
														;
													else
														;
													if(R.flag>1)
		                        PROCOper(LINE_TYPE_ISTRUZIONE,"mulu",OPDEF_MODE_IMMEDIATO16,V->dim[R.flag-1],OPDEF_MODE_REGISTRO,Regs->D);// (multidim)
														// più o meno va, ma manca poi il secondo indice se constante - se var pare ok!
													// o si somma esplicitamente come una var, o sarebbe da ottimizzare in StoreVar...
R.cost->l=0;
	                        v |= 1;   // segnalo indice in R
	                        break;
	                      case VALUE_IS_COSTANTE:
/*					da MSVC
;|***    	ch=provaaa[2][3];
	*** 000574	a0 11 00 		mov	al,BYTE PTR _provaaa+17		2*7+3
	*** 000577	a2 00 00 		mov	BYTE PTR _ch,al
;|*** 	ch=provaaa[4];
	*** 00057a	b0 1c 			mov	al,OFFSET DGROUP:_provaaa+28		4*7
	*** 00057c	a2 00 00 		mov	BYTE PTR _ch,al*/
													if(R.flag>1)
														l += (R.cost->l * FNGetMemSize(T1 /* era V->type*/ & ~VARTYPE_ARRAY,V->size*V->dim[R.flag-1],NULL,0));  // (multidim)
													else
														l += (R.cost->l * FNGetMemSize(T1 /* era V->type*/ & ~VARTYPE_ARRAY,V->size,NULL,0));  // (multidim)
													R.cost->l=l;
	                        v |= 2;   // segnalo indice cost. in l
	                        break;  
	                      case VALUE_IS_COSTANTEPLUS:
		                      PROCError(1035);
	                        break;
	                      }  
                      if(R.Q>=VALUE_IS_ALTRO && R.Q<=VALUE_IS_VARIABILE) {
#if ARCHI                       
                      if(R.size>1) {
                        sprintf(T1S,"ASL #%d",log(R.size)/log(2));
                        }
                      else 
                        *T1S=0;
                      if(RQ & VALUE_IS_COSTANTE) {
                        _tcscpy(AS,"#");
                        i=V->cost.l;
                        if(i >= 0) {
                          _tcscat(AS,V->cost.s);
                          BS="ADD";
                          }
                        else {            
                          sprintf(MyBuf,"%s%d",AS,-i);
                          BS="SUB";
                          }
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO,Regs->P,OPDEF_MODE_REGISTRO,Regs->P,AS,T1S);
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO,Regs->P,OPDEF_MODE_REGISTRO,Regs->P,OPDEF_MODE_REGISTRO,Regs->P+1,T1S);
                        }
#elif Z80
                        u[1].mode=u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
                        u[1].ofs=0;
                        u[1].s.n=Regs->P+1;
//                        Op2A("add",u,&u[1]);   // per adesso lo tolgo (multidim) FARE COME 68000 2025
#elif I8086
                      if(R.Q & VALUE_IS_COSTANTE) {
                        i=V->cost->l;
                        if(i >= 0) {
                          BS="add";
                          }
                        else {            
                          BS="sub";
                          }
                        i *= R.size;
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO16,Regs->P,OPDEF_MODE_IMMEDIATO,i);
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,Regs->P,OPDEF_MODE_REGISTRO,Regs->P+1 /*Regs->D1S*/);
                        }
#elif MC68000
                      if(R.Q & VALUE_IS_COSTANTE) {
                        i=V->cost->l;
                        if(i >= 0) {
                          BS="add";
                          }
                        else {            
                          BS="sub";
                          }
                        i *= R.size;
                        PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO16,Regs->P);			// NON dovrebbe servire qua! lascio incompleto
                        }
                      else {
                        PROCOper(LINE_TYPE_ISTRUZIONE,"adda.l",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO,Regs->P);
                        }
#elif MICROCHIP
                        u[1].mode=u[0].mode=OPDEF_MODE_REGISTRO_LOW8;
                        u[1].ofs=0;
                        u[1].s.n=Regs->D+1;
//                        Op2A("add",u,&u[1]);   // per adesso lo tolgo (multidim)
#endif                        
                        }
	//                    V->Q=RQ;  
	//                    V->var=RVar;
	//                    *V->cost=R.cost;
	  	                *cond=*cond ? j : 0;


											// v.sopra
	                    if(!i)
	                      Regs->Dec((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));

											R.flag--;
	                    PROCCheck(']');
	                    }
	                  else
	                    PROCError(2109);
										FNLA(MyBuf);
										if(*MyBuf == '[')
											FNLO(MyBuf);
										else
											break;
	                  } 

                  if(!T) {  // se non l'ho letto prima...
	                  if(V->Q==VALUE_IS_VARIABILE) {
	                    if(V->type & VARTYPE_ARRAY) {
	                      PROCGetAdd(VALUE_IS_VARIABILE,V->var,l,TRUE);
//											myLog->print(0,"faccio GETADD 2 con ofs %d\n",l);
	                      l=0;
	                      }
	                    else if(V->type & VARTYPE_POINTER) {
		                    if(V->var->classe == CLASSE_REGISTER)
				                  reg2=MAKEPTRREG(V->var->label);
												else  {
#if MICROCHIP
	                        ReadVar(V->var,0,0,0,0,FALSE);			// FINIRE
#else
	                        ReadVar(V->var,0,0,0,FALSE);
#endif
													}
	                      }
	                    T=1;  
	                    }
	                  else if(V->Q == VALUE_IS_COSTANTEPLUS) {		// cose tipo "abcd"[1]
#if MICROCHIP
									    PROCUseCost(V->Q,V->type,V->size,V->cost,0,TRUE);		// FINIRE
#else
									    PROCUseCost(V->Q,V->type,V->size,V->cost,TRUE);
#endif
											// e ovviamente se costante pure l'indice si potrebbe ottimizzare! pare lo faccia già cmq, 68000 SE NON l'ha letto prima... v. sopra
	                    T=1;  
	                    }
	                  }  
                  if(T1 & VARTYPE_ARRAY) {
                    V->size=FNGetMemSize(T1 & ~VARTYPE_ARRAY,V->size,0/*dim*/,0);
//       myLog->print(0,"\aVQ %d, VarClass %d\n",V->Q,(V->Q ==3)?V->var->classe : 0);
										if(v & 2) {
//                        u[1].ofs=l;
                      i=0;
                      }
										if(v & 1) {
                      i=Regs->Inc((uint8_t)FNGetMemSize(T1,V->size,0/*dim*/,0));
                      l=Regs->D;
                      if(!i)
                        Regs->Dec((uint8_t)FNGetMemSize(T1,V->size,0/*dim*/,0));
											i=1;
											}
                    V->type = T1 & ~VARTYPE_ARRAY /*0xFFFFFBFF*/;
                    if(FNIsOp(MyBuf,0)>=2)
                      *cond=0;
//                      u[0].mode=0x80;
//                      PROCReadD0(V->size,V->type,&u[0],&u[1],i,*cond & 0xff);
#ifdef MC68000
										if(!(*cond & VALUE_CONDITION_MASK))			// qua non serve, lo faccio da EvalCond (v.
											PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,R.Q == VALUE_IS_COSTANTE ? R.cost->l : 0,FALSE);
#endif
										V->var=VPtr;
										V->var->modif=i;
										V->var->type=V->type;
										V->var->size=V->size;
										V->var->func=(struct VARS *)reg2;
										V->var->parm=(char *)l;
										V->flag=R.flag+1;
										V->cost=R.cost;
//											myLog->print(0,"array esce con ofs %d\n",l);
                    V->Q=VALUE_IS_D0;
                    }
                  else
                    PROCError(2109);
                  break;
                case '.':
                case '-':
                  if(!V->tag)
                    PROCError((*TS=='.') ? 2224 : 2223);
                  FNLO(T1S);
                  R.var=FNGetAggr(V->tag,T1S,(V->type & VARTYPE_STRUCT) ? 1 : 0,&reg2);
//                  myLog->print(0,"GetAGGR\a: %d\n",reg2);
                  u[0].s.n=Regs->D;
                  u[0].mode=OPDEF_MODE_NULLA;
                  u[0].ofs=0;
                  u[1].s.n=0;
                  u[1].mode=OPDEF_MODE_IMMEDIATO_INDIRETTO;
                  u[1].ofs=reg2;
                  if(!R.var)
                    PROCError(2038,T1S);
                  else if(V->Q==VALUE_IS_D0) {
                    if(*TS=='.') {
                      if(V->type & VARTYPE_IS_POINTER)
                        PROCError(2221);
                      else 
                        PROCGetAdd(VALUE_IS_COSTANTE,V->var,0,TRUE);
                      }
                    else {
                      if(V->type & VARTYPE_IS_POINTER) {
#if MICROCHIP
                        PROCReadD0(V->var,0,0,0,0,0,TRUE);		//FINIRE
#else
                        PROCReadD0(V->var,0,0,0,0,TRUE);
#endif
												}
                      else 
                        PROCError(2222);
                      }
                    }
                  else if(V->Q==VALUE_IS_VARIABILE) {
                    if(*TS=='.') {
                      if(V->type & VARTYPE_IS_POINTER)
                        PROCError(2221);
                      else 
                        PROCGetAdd(VALUE_IS_VARIABILE,V->var,0,TRUE);
                      }
                    else {
                      if(V->type & VARTYPE_IS_POINTER) {
#if MICROCHIP
	                        ReadVar(V->var,0,0,0,0,TRUE);			// FINIRE
#else
	                        ReadVar(V->var,0,0,0,TRUE);
#endif
													}
                      else 
                        PROCError(2222);
                      }
                    }
                  V->type=R.var->type;
                  V->size=R.var->size;
                  if(V->type & VARTYPE_IS_POINTER)
                    R.size=getPtrSize(V->type);
                  else 
                    R.size=V->size;        
//                  PROCReadD0(R.size,V->type & VARTYPE_NOT_A_POINTER,&u[0],&u[1],0,*cond & 0xff);
									V->var=VPtr;
//									*V->var=*R.var  /*era VPtr  -2025*/;
									// non viene copiato il nome della var o membro struct... boh? serve? 2025
									V->var->type=V->type/* & VARTYPE_NOT_A_POINTER*/;
									V->var->modif=0;
									V->var->size=R.size;
									V->var->func=(struct VARS *)Regs->D;
									V->var->parm=(char *)reg2;
                  V->tag=R.var->tag;
                  V->Q=VALUE_IS_D0;
							    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);
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
									else
										goto LBinaryMinus;
                case '+':      
                  if(*(TS+1) != '+') 
										continue;//                    goto LUnaryPlus;
LBinaryMinus:
//									if(isWhat==2)
//										PROCError(2059,TS);
                  if(!Co) {
                    T=1;											// se pre-inc
                    u[0].s.n=Regs->D;
//                    BS=Regs->DS;
                    *cond=0;
                    FNRev(2,cond,Clabel,V);
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

                  v=V->size;		// a che serve?? 2025
                  I=1;
#if ARCHI
                  _tcscpy(T1S,"#");
                  if(V->type & VARTYPE_IS_POINTER) {  
                    sprintf(MyBuf,"%d",V->size);
                    _tcscat(T1S,MyBuf);
                    v=getPtrSize();
                    I=V->size;
                    }
                  else {
                    _tcscat(T1S,"1");
                    }
#elif Z80 || I8086 || MC68000 || MICROCHIP
                  if(V->type & VARTYPE_IS_POINTER) {
                    v=getPtrSize(V->type);
                    I=V->size;
                    }
#endif
									subInc(*TS=='+',cond,T,V->Q,V->var,I,V->type,V->size,&u[1],&u[2]);

                  V->Q=1;
	                break;
	              case '*':
	                i=*cond;
	                *cond = VALUE_CONDITION_UP;
								  FNRev(2,cond,Clabel,&R);
						      reg2=Regs->D;
								  if(R.Q==VALUE_IS_VARIABILE) {
								    if(R.var->classe == CLASSE_REGISTER) {
			                reg2=MAKEPTRREG(R.var->label);
											}								      
								    else {
#if MICROCHIP
											ReadVar(R.var,0,0,*cond & VALUE_CONDITION_MASK,0,TRUE);		// FINIRE
#else
											ReadVar(R.var,0,0,*cond & VALUE_CONDITION_MASK,TRUE);	// Pty<=14 ? TRUE : FALSE ...
#endif
#if MC68000
											if(Pty<=14)		// se NON per Store (ossia se r-value
												R.Q=0;		// ho già il ptr in A0 e quindi stronco la lettura seguente
											// GESTIRE MEGLIO! ossia anche in Store
#endif
								      }
								    }
									else if(R.Q==VALUE_IS_D0) {
#if MICROCHIP
								    PROCReadD0(R.var,0,0,*cond & VALUE_CONDITION_MASK,0,0,TRUE);
#else
								    PROCReadD0(R.var,0,0,*cond & VALUE_CONDITION_MASK,0,TRUE);
#endif
										}
									else if(R.Q & VALUE_IS_COSTANTE) {
#if MICROCHIP
								    PROCUseCost(R.Q,R.type,R.size,R.cost,0,TRUE);		// FINIRE
#else
								    PROCUseCost(R.Q,R.type,R.size,R.cost,TRUE);
#endif
										}
	                if(R.type & VARTYPE_IS_POINTER) {
	                  V->type=(R.type & VARTYPE_NOT_A_POINTER) | ((R.type & VARTYPE_IS_POINTER) -1);
	                  V->size=FNGetMemSize(V->type,R.size,0/*dim*/,1);
	                  }
	                else
	                  PROCError(2100);
	                *cond=*cond ? i : 0;
//	                PROCReadD0(V->size,V->type,&u[0],0,*cond & 0xff,			FALSE /*o altro liv ptr??*/);
									V->var=VPtr; 
									V->var->modif=0;
									V->var->type=V->type;
									V->var->size=V->size;
									V->var->classe=R.var->classe;
									V->var->func=(struct VARS *)reg2;		// credo cazzata, 2025
									V->var->parm=(char*)reg2;
									V->tag=R.tag;
									memcpy(V->dim,R.dim,sizeof(V->dim));
									V->cost=R.cost;
									V->Q= R.Q ? VALUE_IS_D0 : 0;
	                break;
	              case '&':
	                *cond=0;
	                FNRev(2,cond,Clabel,&R);
	                switch(R.Q) {
	                  case VALUE_IS_ALTRO:
	                    PROCError(2101);
	                  case VALUE_IS_D0:
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
	                    PROCGetAdd(VALUE_IS_COSTANTE,R.var,0,FALSE);
#endif	                    
  	                  break;
	                  case VALUE_IS_VARIABILE:
	                    PROCGetAdd(VALUE_IS_VARIABILE,R.var,0,FALSE);
	                    break;
	                  case VALUE_IS_COSTANTEPLUS:		// boh, 2025...s erve??
	                    PROCGetAdd(VALUE_IS_COSTANTEPLUS,R.var,0,FALSE);
	                    break;
	                  default:
	                    break;
	                  }
	                V->Q=1;
	                V->type=VARTYPE_UNSIGNED;
		              V->size=getPtrSize(V->type);
		              break;
		            case '~':
		              *cond=0;
		              FNRev(2,cond,Clabel,V);
		              switch(V->Q) {
		                case VALUE_IS_COSTANTE:
		                  V->cost->l=~V->cost->l;
		                  break;
		                case VALUE_IS_COSTANTEPLUS:		// 9
#if MICROCHIP
		                  PROCUseCost(VALUE_IS_COSTANTE /*era-2*/,V->type,V->size,V->cost,0,FALSE);		// FINIRE
#else
		                  PROCUseCost(VALUE_IS_COSTANTE /*era-2*/,V->type,V->size,V->cost,FALSE);
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
		                  V->Q=1;
		                  break;
		                default:
											if(V->Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
												ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);		// FINIRE
#else
												ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,FALSE);
#endif
												}
											if(V->Q==VALUE_IS_D0) {
#if MICROCHIP
										    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,0,FALSE);
#else
										    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);
#endif
												}
		                  if(V->Q & VALUE_IS_CONDITION)
		                    PROCAssignCond(&V->Q,&V->type,&V->size,Clabel);
#if ARCHI
		                  switch(V->size) {
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
		                  switch(V->size) {
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
		                  switch(V->size) {
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
		                  switch(V->size) {
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
		                  switch(V->size) {
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
		                  V->Q=1;
		                  break; 
		                }
		              break;
	              case '!':
	                FNRev(2,cond,Clabel,V);
	                if(V->Q & VALUE_IS_CONDITION) {
                    V->Q ^= 1;
                    }
                  else if(V->Q == VALUE_IS_COSTANTE) {                   // mancherebbe V->Q=-2 ossia 9...
                    V->cost->l=!V->cost->l;
	                  V->Q=VALUE_IS_COSTANTE;
	                  }
	                else {
										if(V->Q==VALUE_IS_D0) {
#if MICROCHIP
									    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,0,FALSE);
#else
									    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);
#endif
											}
										if(V->Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
	                    ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);		// FINIRE
#else
	                    ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,FALSE);
#endif
											}
#if ARCHI
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVNE",OPDEF_MODE_REGISTRO,Regs->D,"#0");
	                  PROCOper(LINE_TYPE_ISTRUZIONE,"MVNEQ",OPDEF_MODE_REGISTRO,Regs->D,"#0");
 	                  V->Q=1;
#elif Z80
	                  if(!(*cond & VALUE_CONDITION_MASK)) {
	                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  if(V->size == 1) {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
  		                  }
		                  else {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			                  if(V->size > 2) {
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
			                    }
		                    }
 		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,0);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  	                  V->Q=1;
		                  V->size=INT_SIZE;
			                V->type=VARTYPE_PLAIN_INT;
		                  }  
		                else  
  	                  V->Q |= 0x20;            // segnala ! condizionale
#elif I8086
	                  if(!(*cond & VALUE_CONDITION_MASK)) {
	                    switch(V->size) {
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
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
//		                  PROCOper(LINE_TYPE_JUMPC,"jnz",FNGetLabel(MyBuf,2));
		                  PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
		                  if(V->size > 1) {
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA);
			                  if(V->size > 2) {
	    	                  PROCOper(LINE_TYPE_ISTRUZIONE,"cwd",OPDEF_MODE_NULLA);
			                    }
		                    }
  	                  V->Q=1;
		                  V->size=INT_SIZE;
			                V->type=VARTYPE_PLAIN_INT;
		                  }  
		                else  
  	                  V->Q |= 0x20;            // segnala ! condizionale
#elif MC68000
	                  if(!(*cond & VALUE_CONDITION_MASK)) {
//		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE | 0x80,5,OPDEF_MODE_COSTANTE,
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"bne.s",OPDEF_MODE_COSTANTE,
												TipoOut==0 ? (union SUB_OP_DEF *)"$" : (union SUB_OP_DEF *)"*"/* *=$ (sistemare merda di easy68k*/,4+2);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,1,OPDEF_MODE_REGISTRO,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,
												TipoOut==0 ? (union SUB_OP_DEF *)"$" : (union SUB_OP_DEF *)"*"/* *=$ (sistemare merda di easy68k*/,2+2);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,0,OPDEF_MODE_REGISTRO,Regs->D);
		                  if(V->size > 1) {
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,0);
			                  if(V->size > 2) {
	    	                  PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,0);
			                    }
		                    }
  	                  V->Q=1;
		                  V->size=INT_SIZE;
			                V->type=VARTYPE_PLAIN_INT;
		                  }  
		                else  
  	                  V->Q |= 0x20;            // segnala ! condizionale
#elif MICROCHIP
	                  if(!(*cond & VALUE_CONDITION_MASK)) {
	                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  if(V->size == 1) {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
  		                  }
		                  else {
		                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			                  if(V->size > 2) {
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			                    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
			                    }
		                    }
 		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,0);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpCondString,OPDEF_MODE_CONDIZIONALE,5,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"$",3);
		                  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  	                  V->Q=1;
		                  V->size=INT_SIZE;
			                V->type=VARTYPE_PLAIN_INT;
		                  }  
		                else  
  	                  V->Q |= 0x20;            // segnala ! condizionale
#endif
	                  }      
	                break;
	              case 'z':                  // finto per lasciare il case!
LUnaryMinus:
	                *cond=0;
	                FNRev(2,cond,Clabel,V);
	                switch(V->Q) {
	                  case VALUE_IS_COSTANTE:
	                    V->cost->l=-V->cost->l;
//	                    myLog->print(0,"\aUNARY MINUS su COST %s\n",V->cost);
	                    break;
	                  case VALUE_IS_COSTANTEPLUS:
#if MICROCHIP
		                  PROCUseCost(VALUE_IS_COSTANTE /*era-2*/,V->type,V->size,V->cost,0,FALSE);		// FINIRE
#else
		                  PROCUseCost(VALUE_IS_COSTANTE /*era-2*/,V->type,V->size,V->cost,FALSE);
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
	                    V->Q=1;
	                    break;
	                  default:
											if(V->Q==VALUE_IS_D0) {
#if MICROCHIP
										    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#else
										    PROCReadD0(V->var,0,0,0,0,FALSE);
#endif
												}
											if(V->Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
	                      ReadVar(V->var,0,0,0,0,FALSE);			// FINIRE
#else
	                      ReadVar(V->var,0,0,0,FALSE);
#endif
												}
#if ARCHI
	                    switch(V->size) {
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
	                    switch(V->size) {
	                      case 1:
													OpA("neg",u,(struct OP_DEF *)0);
	                        break;
	                      case 2:
	                      case 4:
													OpA("cpl",u,(struct OP_DEF *)0);
    											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("cpl",u,(struct OP_DEF *)0);
			                    if(V->size==2)
			                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
			                    if(V->size==4) {
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
	                    switch(V->size) {
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
	                    switch(V->size) {
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
	                    switch(V->size) {
	                      case 1:
													OpA("COMF",u,(struct OP_DEF *)0);
	                        break;
	                      case 2:
	                      case 4:
													OpA("COMF",u,(struct OP_DEF *)0);
    											u[0].mode=OPDEF_MODE_REGISTRO_HIGH8;
													OpA("COMF",u,(struct OP_DEF *)0);
			                    if(V->size==2)
			                      PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
			                    if(V->size==4) {
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
	                    V->Q=1;
	                    break;
	                  }        
	                break;
	              case 's':			// ossia sizeof
									if(*FNLA(TS) == '(')
										PROCCheck('(');		// (IN EFFETTI è opzionale...
	                T=-1;
	                l1=FIn->GetPosition();
	                R.type=VARTYPE_PLAIN_INT;
//	                v=0;
	                FNLO(TS);
	                if(FNIsType(TS) != VARTYPE_NOTYPE) {
	                  PROCGetType(&R.type,(uint16_t*)&T,&R.tag,(uint32_t*)&I,l1);
	                  }
	                else {
//										FIn->Seek(l1,CFile::begin);
										FIn->RestorePosition(l1);
//										__line__=ol;
	                  *cond=0;
	                  FNRev(15,cond,Rlabel,&R);
	                  T=R.size;
		                if(R.type & VARTYPE_ARRAY)
		                  T=FNGetArraySize(R.var);
		                else if(R.type & VARTYPE_IS_POINTER)
		                  T=getPtrSize(R.type);
	                  }
									if(*FNLA(TS) == ')')
	                  while(*FNLO(MyBuf) != ')');
	                V->cost->l=LOWORD(T);		// anche >65536??
	                V->Q=VALUE_IS_COSTANTE;
	                V->type=VARTYPE_UNSIGNED;
	                V->size=INT_SIZE;
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
							if(!Co /*isWhat==2*/)
								PROCError(2059,TS);
	            reg2=0;
	            subSpezReg((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0),u);
	            ROut=LastOut;
#if ARCHI || Z80
  	          if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || (V->Q==VALUE_IS_VARIABILE && ((V->var->classe<CLASSE_AUTO) || (OP!=6 && OP!=7)))) {
//  	          if((*V->Q==1) || *V->Q==2 || (*V->Q==3 && (V->var->classe<3))) {
#elif MICROCHIP
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
#elif I8086 
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
#elif MC68000 
//							if(0) {
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
#endif  	            
                reg2=Regs->Inc((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
                }
              j=*cond;
              *cond = VALUE_CONDITION_UP;
	            FNRev(OP-1,cond,Rlabel,&R);
              *cond=*cond ? j : 0;
	            // qui usiamo i registri Increm. per le operazioni varie dell'exp 2...
#if ARCHI || Z80
  	          if((V->Q==VALUE_IS_ALTRO) || V->Q==VALUE_IS_D0 || (V->Q==VALUE_IS_VARIABILE && ((V->var->classe<CLASSE_AUTO) || (OP!=6 && OP!=7)))) {
//  	          if((*V->Q==1) || *V->Q==2 || (*V->Q==3 && (V->var->classe<3))) {

#elif I8051 || MICROCHIP
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
#elif I8086 
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
#elif MC68000 
	            if(V->Q==VALUE_IS_ALTRO || V->Q==VALUE_IS_D0 || V->Q==VALUE_IS_VARIABILE) {
//            if(0) {
#endif  	            
                if(!reg2)
                  Regs->Dec((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
//                myLog->print(0,"Dec 1: %d\n",Regs->D);
                }
              reg2=reg2 && (LastOut!=ROut);            // lo uso dopo...
	            if((V->Q & VALUE_IS_COSTANTE) && (R.Q & VALUE_IS_COSTANTE)) {
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
							  if((V->Q==VALUE_IS_COSTANTE) && (R.Q==VALUE_IS_COSTANTE)) {
							    sprintf(MyBuf,"%d %s %d",V->cost->l,BS,R.cost->l);
							    V->cost->l=EVAL(MyBuf);
							    V->Q=VALUE_IS_COSTANTE;
							    }
							  else {
							    _tcscat(V->cost->s,BS); 
							    _tcscat(V->cost->s,R.cost->s);
							    V->Q=VALUE_IS_COSTANTEPLUS;		// 9
							    }
	              }               // fine if costanti...
	            else {
  	            T=0;
	              switch(V->Q) {
	                case 0:
	                  break;
	                case VALUE_IS_ALTRO:
			              if(reg2)
		                  swap(&ROut,&LastOut);
		                break;  
	                case VALUE_IS_D0:
			              if(reg2)
		                  swap(&ROut,&LastOut);
#if MICROCHIP
                    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#else
                    PROCReadD0(V->var,0,0,0,0,FALSE);
#endif
		                break;  
	                case VALUE_IS_COSTANTE:
#if ARCHI
	                  if((V->cost.l>255) || (OP==3)) {        // OP=3 (*)
                      goto myUVcost;
	                    }
	                  else {
	                    T=MODE_IS_CONSTANT1;
	                    }
#elif Z80 || I8051 || MICROCHIP
                    if(OP==3)
                      goto myUVcost;
                    else
                      T=MODE_IS_CONSTANT1;
#elif I8086
                    if(OP==3 && CPU86<2)
                      goto myUVcost;
                    else
                      T=1;
#elif MC68000
          /*          if(OP==3)
                      goto myUVcost;
                    else
                      T=MODE_IS_CONSTANT1;
											*/
                      T=MODE_IS_CONSTANT1;
//												idem come CONSTANT2	beh ma in effetti qua ottimizzo cmq le costanti! 
#endif
	                  break;
	                case VALUE_IS_COSTANTEPLUS:
myUVcost:	                
#if MICROCHIP
	                  PROCUseCost(V->Q,V->type,V->size,V->cost,0,FALSE);		// FINIRE
#else
		                PROCUseCost(V->Q,V->type,V->size,V->cost,FALSE);
#endif
	                  break;
	                case VALUE_IS_VARIABILE:
                    switch(V->var->classe) {
                      case CLASSE_EXTERN:
                      case CLASSE_GLOBAL:
                      case CLASSE_STATIC:
myUVvar:	                
					              if(reg2)
				                  swap(&ROut,&LastOut);
#if MICROCHIP
	                      ReadVar(V->var,0,0,0,0,FALSE);			// FINIRE
#else
	                      ReadVar(V->var,0,0,0,FALSE);
#endif
	                      break;
	                    case CLASSE_REGISTER:
#if ARCHI
	                      sprintf(sRegs[0].Dr,"R%d",MAKEPTRREG(V->var->label));
#elif Z80 || I8051 || MICROCHIP
		                    if(0/*OP==4*/) {
		                      u[0].mode=OPDEF_MODE_REGISTRO;
		                      u[0].s.n=*(int *)(V->var->label);
//		                      _tcscpy(sRegs[0].Dr,Regs[V->var]);
//		                      _tcscpy(sRegs[0].Drh,Regs[V->var]);
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
	                      if((OP==6 || OP==7 || (*cond && OP==8)) && !(R.type & 0xf)) {
		                      i=MAKEPTRREG(V->var->label);
		                      u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[0].s.n=0;
		                      u[0].ofs=i;
							            if(V->size>2) {
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
                    PROCAssignCond(&V->Q,&V->type,&V->size,Clabel);
	                  break;              
	                }
//                ROut=LastOut;
	              if(reg2 && T==0) {
    	            SavedR=FNGetMemSize(V->type,V->size,0/*dim*/,0);
  	              Regs->Save(SavedR);
                  swap(&ROut,&LastOut);
      	          }
      	        reg2=0;
#if ARCHI || Z80
  	            if((T>=0 && ((V->Q==VALUE_IS_ALTRO) || (V->Q==VALUE_IS_D0) || (V->Q==VALUE_IS_VARIABILE))) || OP==3)
#elif I8051 || MICROCHIP
  	            if(T>=0 && ((V->Q==VALUE_IS_ALTRO) || (V->Q==VALUE_IS_D0) || (V->Q==VALUE_IS_VARIABILE)))
#elif I8086 
  	            if(T>=0 && ((V->Q==VALUE_IS_ALTRO) || (V->Q==VALUE_IS_D0) || (V->Q==VALUE_IS_VARIABILE)))
#elif MC68000 
  	            if(T>=0 && ((V->Q==VALUE_IS_ALTRO) || (V->Q==VALUE_IS_D0) || (V->Q==VALUE_IS_VARIABILE)))
#endif  	            
                  {
                  reg2=Regs->Inc((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
	            // ...e qui usiamo i registri Increm. per ReadVar e simili...
                  VQ1=V->Q;
	                }
	              else
	                VQ1=-1;

	              ROut=LastOut;  
#if ARCHI
	              j=(V->size > 2) ? 2 : 1;  
#elif Z80  || I8051 || MICROCHIP
	              j=(V->size > 2) ? 2 : 1;  
#elif I8086
	              j=(V->size > 2) ? 2 : 1;			// sì insomma
#elif MC68000
	              j=(V->size > 4) ? 2 : 1;  
#endif
		            subSpezReg(2,u+j);
	              switch(R.Q) {
	                case 0:
	                case VALUE_IS_ALTRO:
	                  break;
	                case VALUE_IS_D0:
#if MICROCHIP
                    PROCReadD0(R.var,0,0,0,0,0,FALSE);
#else
                    PROCReadD0(R.var,0,0,0,0,FALSE);
#endif
	                  break;
	                case VALUE_IS_COSTANTE:
#if ARCHI
	                  if((R.cost.l>255) || (OP==3))    // OP=3 (*) 
                      goto myURcost;
	                  else 
	                    T=MODE_IS_CONSTANT2;
#elif Z80  || I8051 || MICROCHIP
//	                  if(*V->Q>1) {
//	                    if(V->var->classe==4) {
//                        goto myURcost;
//                        }
//                      }  
                    if(OP==3)
                      goto myURcost;
                    else
                      T=MODE_IS_CONSTANT2;
#elif I8086
                    if(OP==3 && CPU86<2)
                      goto myURcost;
                    else
                      T=MODE_IS_CONSTANT2;
#elif MC68000
                    if(OP==3) {
											if(Optimize & OPTIMIZE_CONST) {		// :)
												if(i=FNIsPower2(R.cost->l)) 			// ottimizzo potenze di 2! v.dopo
													T=MODE_IS_CONSTANT2;
												else
													T=MODE_IS_CONSTANT2;
//												goto myURcost;	beh ma in effetti qua ottimizzo cmq le costanti! 
												}
											}
                    else
                      T=MODE_IS_CONSTANT2;	// appunto
#endif
	                  break;
	                case VALUE_IS_COSTANTEPLUS:
myURcost:	 
#if MICROCHIP
	                  PROCUseCost(R.Q,R.type,R.size,R.cost,0,FALSE);		// FINIRE
#else
	                  PROCUseCost(R.Q,R.type,R.size,R.cost,FALSE);
#endif
	                  break;
	                case VALUE_IS_VARIABILE:
	                  switch(R.var->classe) {
	                    case CLASSE_EXTERN:
                      case CLASSE_GLOBAL:
                      case CLASSE_STATIC:
#if ARCHI || Z80  || I8051 
	                      ReadVar(R.var,0,0,0,FALSE);
#elif I8086
	                      ReadVar(R.var,0,0,0,FALSE);
#elif MC68000
	                      ReadVar(R.var,0,0,0,FALSE);
#elif MICROCHIP
	                      ReadVar(R.var,0,0,0,0,FALSE);		// FINIRE
#endif
      									break;
	                    case CLASSE_REGISTER:
#if ARCHI
	                      sprintf(sRegs[j].Dr,"R%d",*(int *)R.var->label);
#elif Z80 || I8051 || MICROCHIP
	                      if(0/*OP==4*/) {
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=MAKEPTRREG(R.var->label);
		                      T=-1;  
		                      }
												else   {
#if MICROCHIP
	 	                      ReadVar(R.var,0,0,0,0,FALSE);		// FINIRE
#else
	 	                      ReadVar(R.var,0,0,0,FALSE);
#endif
													}
	                      // dovremmo riuscire a usare le istruzioni ADD IX ecc.
#elif I8086
	                      if(0/*OP==4*/) {
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=MAKEPTRREG(R.var->label);
		                      T=-1;  
		                      }
		                    else  
	 	                      ReadVar(R.var,0,0,0,FALSE);
#elif MC68000
	                      if(OP==4 || OP==8 || OP==9 || OP==10) {			// solo su +- /|^ posso usare registro direttamente (poi volendo qualcosa si potrebbe ancora limare...
		                      u[j].mode=OPDEF_MODE_REGISTRO;
		                      u[j].s.n=MAKEPTRREG(R.var->label);
		                      T=-1;  
		                      }
		                    else  
	 	                      ReadVar(R.var,0,0,0,FALSE);
#endif
	                      break;
											case CLASSE_AUTO:
#if ARCHI
 	                      ReadVar(R.var,0);
#elif Z80	|| I8051 || MICROCHIP
		                    if(/*!(V->type & 0x1d0f) && */ (T==0 && (OP==4 || OP>=8))/* || (OP==6 || OP==7)*/) {
		                    // dovrebbe prendere anche i char *...
		                      i=*(int *)R.var->label;
		                      u[j].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[j].s.n=0;
		                      u[j].ofs=i;
							            if(R.size>2) {
			                      u[j+1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                      u[j+1].s.n=0;
			                      u[j+1].ofs=i+2;
								            }
	                      	T=-1;
		                      }
												else {
#if MICROCHIP
	 	                      ReadVar(R.var,0,0,0,0,FALSE);		// FINIRE
#else
	 	                      ReadVar(R.var,0,0,0,FALSE);
#endif
													}
#elif I8086
		                    if(/*!(V->type & 0x1d0f) && */ (T==0 && (OP==4 || OP>=8))/* || (OP==6 || OP==7)*/) {
		                    // dovrebbe prendere anche i char *...
		                      i=*(int *)R.var->label;
		                      u[j].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
		                      u[j].s.n=0;
		                      u[j].ofs=i;
							            if(R.size>2) {
			                      u[j+1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                      u[j+1].s.n=0;
			                      u[j+1].ofs=i+2;
								            }
	                      	T=-1;
		                      }
	                      else 
	 	                      ReadVar(R.var,0,0,0,FALSE);
#elif MC68000
												// tolto qua tutto quel controllo... non so bene a cosa servisse, forse per ottimizzare

	 	                      ReadVar(R.var,0,0,0,FALSE);
#endif
												break;
	                    }
	                  break;  
	                default:
	                  PROCAssignCond(&R.Q,&R.type,&R.size,Rlabel);
	                  break;
                  }
//                if(R.var >= 0) {
//                  PROCCast(V->type,V->size,R.type,R.size);
//                  }
	              if(!SavedR) {
		              if(reg2 && (LastOut!=ROut)) {
		                swap(&ROut,&LastOut);
		                SavedR=FNGetMemSize(V->type,V->size,0/*dim*/,0);
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
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO,Regs->D);
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
                    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u[1].mode,&u[1].s,u[1].ofs,OPDEF_MODE_REGISTRO16,Regs->D);
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
										if(Optimize & OPTIMIZE_CONST &&  R.Q==VALUE_IS_COSTANTE && (i=FNIsPower2(R.cost->l))) {			// ottimizzo potenze di 2!
											R.cost->l=i;
											V->Q=subShift(*TS=='*',MODE_IS_CONSTANT2,V->Q,V->var,V->type,V->size,R.type,
												V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],FALSE);
											}
										else {
			                V->Q=subMul(*TS,T,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,V->cost,R.cost,
												&u[0],&u[1],&u[2],&u[3],FALSE);
											}
                    break;
                  case 4:
				            *cond=0;
		                V->Q=subAdd(*TS=='+',T,V->Q,V->var,&V->type,&V->size,R.Q,R.type,R.size,V->cost,R.cost,
											&u[0],&u[1],&u[2],&u[3],FALSE);
			              break;
		              case 5:
				            *cond=0;
		                V->Q=subShift(*TS=='<',T,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
											&u[0],&u[1],&u[2],&u[3],FALSE);
		                break;
                  case 6:
                  case 7:
				            *cond=0;
		                V->Q=subCMP(TS,*cond,T,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,V->cost,R.cost,
											&u[0],&u[1],&u[2],&u[3]);
                    break;
                  default:		// 8 9 10
//				            *cond=0;
		                V->Q=subAOX(*TS,cond,T,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,V->cost,R.cost,
											&u[0],&u[1],&u[2],&u[3],FALSE);
                    break;
                  }
#if ARCHI || Z80
  	            if((/*T>=0 &&*/ ((VQ1==VALUE_IS_ALTRO) || (VQ1==VALUE_IS_D0) || (VQ1==VALUE_IS_VARIABILE))) || OP==3)
#elif I8086
  	            if(/*T>=0 &&*/ ((VQ1==VALUE_IS_ALTRO) || (VQ1==VALUE_IS_D0) || (VQ1==VALUE_IS_VARIABILE)))
#elif MC68000
  	            if(/*T>=0 &&*/ ((VQ1==VALUE_IS_ALTRO) || (VQ1==VALUE_IS_D0) || (VQ1==VALUE_IS_VARIABILE)))
//  	            if(0)
#elif MICROCHIP
  	            if((/*T>=0 &&*/ ((VQ1==VALUE_IS_ALTRO) || (VQ1==VALUE_IS_D0) || (VQ1==VALUE_IS_VARIABILE))) || OP==3)
#endif  	            
                  {
                  if(!reg2)
                    Regs->Dec((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
                  }
                }  
              break;

            case 11:
            case 12:
							if(!Co /*isWhat==2*/)
//								PROCError(2059,TS);
//myLog->print(0,"OP logico: entro al livello %d con %d (cond è %d)\n",Pty,*V->Q,cond);
              T=1;
							if(V->Q==VALUE_IS_D0) {
#if MICROCHIP
						    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,0,FALSE);
#else
						    PROCReadD0(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);
#endif
								}
              else if(V->Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
                ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,0,FALSE);		// FINIRE
#else
                ReadVar(V->var,0,0,*cond & VALUE_CONDITION_MASK,FALSE);
#endif
								}
              else if(V->Q & VALUE_IS_COSTANTE) {   // auto ottim. costanti (forza T=0)
						    PROCWarn(4127,V->cost->s);
                if(V->Q==VALUE_IS_COSTANTE) {
                  if(V->cost->l) {
                    if(OP==12)
                      T=MODE_IS_VARIABLE;
                    else 
                      T=MODE_IS_CONSTANT2;        
                    }
                  else {
                    if(OP==11)
                      T=MODE_IS_VARIABLE;
                    else 
                      T=MODE_IS_CONSTANT2;
                    }
                  }
                else {
                  if(OP==12)
                    T=MODE_IS_VARIABLE;
                  else 
                    T=MODE_IS_CONSTANT2;
                  }
                }
						  ROut=LastOut;
  	          if(T) {
  	            if((/*!*cond ||*/(oOP && oOP!=12) && (OP==12)) && V->Q) {
    		          PROCOutLab(Clabel);
    		          }
    		        _tcscpy(Rlabel,Clabel);
    		        *cond |= 1;
	              T1=FNRev(OP-1,cond,Rlabel,&R);
	              myLog->print(0,"oOP era %d, Dopo il logico c'è %d\n",oOP,T1);
	              if((T1 == 12 &&    oOP!=12) || ((!(V->Q & VALUE_HAS_CONDITION)) && (/* !*cond || */ OP==/*==*/12)))		// solo se ||
	                FNGetLabel(Clabel,2);
	              if(T==1) {
                  V->Q &= ~VALUE_HAS_CONDITION;
  		            if(i=(ROut!=LastOut)) {
  		              swap(&ROut,&LastOut);
  		              }
									if(R.Q==VALUE_IS_COSTANTE) {		// se c'è una costante...
										if(OP==11) {		// se &&
											if(!R.cost->l) {		// e costante = 0
#if MC68000
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO,0,OPDEF_MODE_REGISTRO,Regs->D);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#else
			                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,0);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#endif
//												RQ=VALUE_HAS_CONDITION;
//												PROCOutLab(Clabel);
												goto skippa_condbranch;
												}
											}
										else {		// se ||
											if(R.cost->l) {		// e costante != 0
#if MC68000
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO,1,OPDEF_MODE_REGISTRO,Regs->D);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#else
			                  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,1);
			                  PROCOper(LINE_TYPE_ISTRUZIONE,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)Clabel,0);
#endif
												R.Q=VALUE_HAS_CONDITION;
//												PROCOutLab(Clabel);
												goto skippa_condbranch;
												}
											}
										}
	                PROCGenCondBranch(Clabel,(OP==11),&V->Q,FNGetMemSize(V->type,V->size,0/*dim*/,0));
skippa_condbranch: ;
  		            if(i)
  	  	            swap(&ROut,&LastOut);
	                }
	              T1=1;

/*	              if(RQ==3) {
	                ReadVar(R.var,0,0,*cond & 0xff);
//	                RQ=0x85;
	                }
	              if(RQ & 8) {
	                if(RQ==8) {
	                  if(R.cost.l) {
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
//	              if(RQ & VALUE_IS_CONDITION) {
	                V->Q=R.Q;
	                V->var=R.var;
	                V->size=R.size;
	                V->type=R.type;
									V->cost=R.cost;
//	                myLog->print(0,"RQ vale %x\n",RQ);
//	                }
	              if(!T1)
	 	              V->Q=0;
  	            }
	            if((/*!*cond ||*/ (oOP==12 || OP==12)) && V->Q) {
	              V->Q |= VALUE_HAS_CONDITION;
  		          }
//	            else
//	              V->Q=0;

//myLog->print(0,"OP logico: esco con %d\n\a",*V->Q);
              oOP=OP;
	            break;

            case 13:
							if(!Co /*isWhat==2*/)
								PROCError(2059,TS);
							if(V->Q==VALUE_IS_VARIABILE) {
#if MICROCHIP
		            ReadVar(V->var,0,0,1,0,FALSE);		// FINIRE
#else
		            ReadVar(V->var,0,0,1,FALSE);
#endif
								}
							else if(V->Q==VALUE_IS_D0) {
#if MICROCHIP
						    PROCReadD0(V->var,0,0,1,0,0,FALSE);
#else
						    PROCReadD0(V->var,0,0,1,0,FALSE);
#endif
								}
//		          if(*V->Q<0) {
		          else if(V->Q & VALUE_IS_COSTANTE) {      // boh autoottimizza cost..
#if MICROCHIP
						    PROCUseCost(V->Q,V->type,V->size,V->cost,0,FALSE);		// FINIRE
#else
						    PROCUseCost(V->Q,V->type,V->size,V->cost,FALSE);
#endif
						    PROCWarn(4127);
/*
		            if((*V->Q==VALUE_IS_COSTANTE) && (!&V->cost.l)) {
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
		            subEvEx(13,0,V->type,V->size,V->Q,V->var,&V->cost,V->tag,V.dim);
		            */
		            }
		          FNGetLabel(TS,2);
		          PROCGenCondBranch(TS,TRUE,&V->Q,FNGetMemSize(V->type,V->size,0/*dim*/,0));
		          *cond=0;
		          subEvEx(13,cond,Clabel,&R);
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
		          subEvEx(13,cond,Clabel,&R);
		          PROCOutLab(TS,"_",NULL);
		          V->type=R.type;          // andrebbero usate entrambe le expr
		          V->size=R.size;
		          V->var=NULL;
		          V->Q=VALUE_IS_ALTRO;
		          break;

		        case 14:
//							if(isWhat==2)
//								PROCError(2059,TS);
              if(Pty<14 || (V->Q & OPDEF_MODE_FRAMEPOINTER_INDIRETTO))           // bloccare le expr e cost a sinistra (a+3=b)
	              PROCError(2106);
             	u[0].mode=OPDEF_MODE_REGISTRO;		// preparo... 2025
             	u[0].ofs=0;
							u[0].s.n=0;
						  ROut=LastOut;
		          if(V->Q==VALUE_IS_ALTRO || *TS!='=') {
//		            PROCOut("; fine =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
#ifdef MC68000
								// tolgo, verificare
#else
		            i=Regs->Inc((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0));
#endif
		            }
		          else if(V->Q==VALUE_IS_D0) {     // separato da V->Q=1, per il ptr *
//		            PROCOut("; fine =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
								if(V->var->modif) {    // se devo sommare un ofs reg, lo faccio ora
									subOfsD0(V->var,V->var->size,(int)V->var->func,(int)V->var->parm);
									V->var->modif=0;
									V->var->parm=0;
									}
#ifndef MC68000		// mmm provare... 
		            i=Regs->Inc(getPtrSize());
#endif
		            }
		          *cond=0;
		          FNRev(14,cond,Rlabel,&R);
		          if(R.Q==VALUE_IS_VARIABILE && (V->Q != VALUE_IS_VARIABILE || V->var->classe != CLASSE_REGISTER)) {   // store in registri a parte...
                switch(R.var->classe) {
	                case CLASSE_EXTERN:
                  case CLASSE_GLOBAL:
                  case CLASSE_STATIC:
#if MICROCHIP
                    ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,FALSE);		// FINIRE
#else
                    ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,FALSE);
#endif
                    T=0;
  									break;
                  case CLASSE_REGISTER:
#if MICROCHIP
	  			          ReadVar(R.var,0,0,0,0,FALSE);		// FINIRE
#else
	  			          ReadVar(R.var,0,0,0,FALSE);
#endif
                    T=0;
                    break;
									case CLASSE_AUTO:
									  if(*TS == '=' /*|| *TS=='*' || *TS=='/' || *TS=='%'  && *V->Q==3 && V->var->classe<3*/) {
#if MICROCHIP
		                  ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,FALSE);		// FINIRE
#else
		                  ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,FALSE);
#endif
									    T=0;
		  			          }
		  			        else {
//                      ReadVar(R.var,0,FNGetMemSize(V->type,V->size,1),0);
		  			          T=1;
		  			          }
										break;
                  }
                }  
              else
                T=0;  
		          if(R.Q & VALUE_IS_CONDITION) {
								if(debug)
									myLog->print(0,"ASSIGNCOND: =%d\n\a",R.Q);
		            PROCAssignCond(&R.Q,&R.type,&R.size,Rlabel);
								}
			        else if(R.Q==VALUE_IS_COSTANTEPLUS) {             // tratto le costanti int. a parte
	              R.size=V->size;
#if MICROCHIP
	              PROCUseCost(R.Q,R.type,R.size/*cioè V->size*/,R.cost,0,FALSE);		// FINIRE
#elif MC68000
								// questo le può usare direttamente!
#else
	              PROCUseCost(R.Q,R.type,R.size/*cioè V->size*/,R.cost,FALSE);
#endif
		            }
						  else if(R.Q==VALUE_IS_D0) {
//						  myLog->print(0,"Read D0: %lx\n",R.var);
#if MICROCHIP
						    PROCReadD0(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,0,FALSE);
#elif MC68000
								if(!R.flag) {		// era puntatore e NON array (dovrebbe andare 
						  		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->P);	// qua è ok così
									CHECKPOINTER();
									}
#else
						    PROCReadD0(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,FALSE);
#endif
						    }
              else if(R.Q==VALUE_IS_ALTRO)
    					  PROCCast(V->type,V->size,R.type,R.size);
              else if(R.Q==VALUE_IS_COSTANTE) {
						    }
		          if(V->Q==VALUE_IS_ALTRO || *TS!='=') {
//                PROCOut("; inizio =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
#ifndef MC68000		// mmm provare... 
		            if(!i)
		              Regs->Dec(FNGetMemSize(V->type,V->size,0/*dim*/,0));
#endif
		            }
		          else if(V->Q==VALUE_IS_D0) {
//                PROCOut("; inizio =",NULL,NULL,NULL,NULL);
//		            swap(&ROut,&LastOut);
#ifndef MC68000		// mmm provare... 
		            if(!i)
		              Regs->Dec(getPtrSize());
#endif
		            }
		          switch(*TS) {
		            case '=':        // OCCHIO: fa casino se assegno dentro una cond...
				          if((R.type & (VARTYPE_UNION | VARTYPE_STRUCT)) && (V->type & (VARTYPE_UNION | VARTYPE_STRUCT)) && (!(R.type & VARTYPE_IS_POINTER)) && (!(V->type & VARTYPE_IS_POINTER))) {
				            if(V->tag != R.tag)
				              PROCError(2115);
    		            i=Regs->Inc(getPtrSize(R.type));
				            if(V->Q==VALUE_IS_VARIABILE) {
				              if(!(V->var->type & VARTYPE_IS_POINTER) && (V->var->type & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_FUNC /*0x1d00*/))) 
  				              PROCError(2106);   // dovrebbe bloccare i non lvalue a sinistra
											else {
#if MICROCHIP
  				              ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#else
  				              ReadVar(V->var,0,0,0,FALSE);
#endif
												}
				              }
    		            if(!i)
    		              Regs->Dec(getPtrSize(R.type));
#if ARCHI
				            FNGetLabel(TS,2);
				            Regs->Save();
				            PROCOutLab(TS);     
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,"R2",R.size);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB R3,[R0,R2]",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"STRB R3,[R1,R2]",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"SUBS R2,R2,#1",NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"BNE",TS);
				            Regs->Get();
#elif Z80
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,R.size);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"ldir",OPDEF_MODE_NULLA,0);
#elif I8086
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,R.size);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
#elif MC68000
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO16,R.size);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
#elif MICROCHIP
				            FNGetLabel(TS,2);
				            Regs->Save();
				            PROCOutLab(TS);     
				            PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,R.size);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB R3,[R0,R2]",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"STRB R3,[R1,R2]",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"SUBS R2,R2,#1",OPDEF_MODE_NULLA,NULL);
				            PROCOper(LINE_TYPE_ISTRUZIONE,"DECFSZ",OPDEF_MODE_NULLA,NULL,0,TS);
				            Regs->Get();
#endif
				            }
				          else {
				            if(V->size > 2 || R.size > 2) {		// OCCHIO memorymodel, CPU varie... PTR_SIZE
				              if((V->size != R.size) && !((V->type | R.type) & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_IS_POINTER | VARTYPE_FUNC /*0x1d0f*/)))  // integrali di diff. grandezza
				                PROCWarn(4761);
				              }
				            if(V->Q & OPDEF_MODE_FRAMEPOINTER_INDIRETTO || V->Q==VALUE_IS_ALTRO)
				              PROCError(2106);
				            else if(V->Q==VALUE_IS_VARIABILE) {
				              if(!(V->var->type & VARTYPE_IS_POINTER) && (V->var->type & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_FUNC /*0x1d00*/))) 
  				              PROCError(2106);
  				            else  
												StoreVar(V->var,R.Q,R.var,R.cost,(R.Q==0 || R.Q==VALUE_IS_D0) ? TRUE : FALSE);		// VERIFICARE! se R.Q non è nulla ossia =0 allora arrivo da D0 ossia puntatore...

											// VERIFICARE perché arriva 2!!


				              }
//				            if(*V->Q==1)
//				              PROCAssign(ROut/*LastOut*/,*V->Q,V->var,V->type,V->size,RQ,&R.cost);
//											PROCError(1001,"store in hl, no ptr");
				            else if(V->Q==VALUE_IS_D0)
				              PROCStoreD0(V->var,R.Q,R.var,R.cost,FALSE);
				            }
    		          V->Q=VALUE_IS_ALTRO;
			            break;
		            case '+':
		            case '-':
                  subSpezReg((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0),u);
		              if(T) {
	                	i=MAKEPTROFS(R.var->label);
		                if(FNGetMemSize(V->type,V->size,0/*dim*/,1) <=2) {
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
                  if(R.Q==VALUE_IS_COSTANTE) {
#if MICROCHIP
#elif MC68000
										if(abs(R.cost->l)>8)
	                    j=2;
										else {
											subInc(*TS=='+',cond,0,V->Q,V->var,LOBYTE(LOWORD(R.cost->l)),V->type,V->size,&u[1],&u[2]);
											V->Q=0;
	                    j=0;
											}
#else
#endif
										}
                  else {
                    j=0;
                    if(R.Q==VALUE_IS_COSTANTEPLUS) {
#if MICROCHIP
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,0,FALSE);		// FINIRE
#elif MC68000
 											PROCUseCost(R.Q,R.type,R.size,R.cost,FALSE);
#else
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,FALSE);
#endif
											}
                    }
			            switch(V->Q) {
										case VALUE_IS_VARIABILE:
											switch(V->var->classe) {
												case CLASSE_AUTO:
			                		i=MAKEPTRREG(V->var->label);
			                		u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                		u[0].ofs=i;
#ifdef MC68000
													PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);		// me ne frego della size!
#else
    											if(FNGetMemSize(V->type,V->size,0/*dim*/,1) > 2) {
														u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
														u[1].ofs=i+2;
	//				                  u[1].s=0;
														}
#endif
													goto my_add;
												case CLASSE_REGISTER:
													u[0].mode=OPDEF_MODE_REGISTRO;
													u[0].s.n=MAKEPTRREG(V->var->label);
	//				                  u[0].s=0;
my_add:
													V->Q=subAdd(*TS=='+',j,V->Q,V->var,&V->type,&V->size,R.Q,R.type,R.size,
														V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
													if(Pty==14) {             // ritorna expr in hl/do se serve
#if MICROCHIP
	  												ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#else
														ReadVar(V->var,0,0,0,FALSE);
#endif
														}
													break;
												default:
													// INVERTITI Dr e Dr1 (e h)
//							          if(RQ != VALUE_IS_COSTANTE)
//							            i=Regs->Inc(FNGetMemSize(V->type,V->size,0));
#ifdef MC68000
													PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);		// me ne frego della size!
#else
#endif
#if MICROCHIP
  												ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#else
													ReadVar(V->var,0,0,0,FALSE);
#endif

//							          if(RQ != VALUE_IS_COSTANTE)
//  			                  *V->Q=subAdd(*TS=='+',j,*V->Q,V->var,V->type,V->size,R.type,R.size,&V->cost,&R.cost,sRegs[1].Dr,sRegs[0].Dr,sRegs[1].Drh,sRegs[0].Drh,sRegs[3].Dr,sRegs[2].Dr,sRegs[3].Drh,sRegs[2].Drh);
//							          else
  			                  V->Q=subAdd(*TS=='+',j,V->Q,V->var,&V->type,&V->size,R.Q,R.type,R.size,V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
											    StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
//							          if(RQ != 8) {
//  						            if(!i)
//  						              Regs->Dec(FNGetMemSize(V->type,V->size,0));
//  						            }  
												  break;
												}
											break;
				            case VALUE_IS_D0:        // non va se (de) o (bc)...
#if ARCHI
	                		u[0].s.n=(int)V->var->func;
	                		if(u[0].s.n <= 3 && (int)V->var->parm) {
		                		u[0].mode=0x3;
	                			u[0].ofs=0;
												Op2A("add",&u[0],(int)V->var->parm,0);
	                			}
	                		else { 
	                			u[0].ofs=(int)V->var->parm;
	                			}
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif Z80
	                		u[0].s.n=(int)V->var->func;
	                		if(u[0].s.n <= 3 && (int)V->var->parm) {
		                		u[0].mode=OPDEF_MODE_REGISTRO;
	                			u[0].ofs=0;
												Op2A("add",&u[0],(int)V->var->parm,0);
	                			}
	                		else { 
	                			u[0].ofs=(int)V->var->parm;
	                			}
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif I8086
	                		u[0].s.n=(int)V->var->func;
	                		if(u[0].s.n <= 3 && (int)V->var->parm) {
		                		u[0].mode=OPDEF_MODE_REGISTRO;
	                			u[0].ofs=0;
	//											Op2A("add",&u[0],(int)V->var->parm,0);
	                			}
	                		else { 
	                			u[0].ofs=(int)V->var->parm;
	                			}
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MC68000
	                		u[0].s.n=(int)V->var->func;
	                		if(u[0].s.n <= Regs->UserBase && (int)V->var->parm) {
		                		u[0].mode=OPDEF_MODE_REGISTRO;
	                			u[0].ofs=0;
	//											Op2A("add",&u[0],(int)V->var->parm,0);
	                			}
	                		else { 
	                			u[0].ofs=(int)V->var->parm;
	                			}
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MICROCHIP
	                		u[0].s.n=(int)V->var->func;
	                		if(u[0].s.n <= 3 && (int)V->var->parm) {
		                		u[0].mode=OPDEF_MODE_REGISTRO;
	                			u[0].ofs=0;
												Op2A("ADDWF",&u[0],(int)V->var->parm,0);
	                			}
	                		else { 
	                			u[0].ofs=(int)V->var->parm;
	                			}
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#endif
	                		if(FNGetMemSize(V->type,V->size,0/*dim*/,0)>2) {
	                			u[1]=u[0];
	                			}
											V->Q=subAdd(*TS=='+',j,V->Q,V->var,&V->type,&V->size,R.Q,R.type,R.size,V->cost,R.cost,
												&u[0],&u[1],&u[2],&u[3],TRUE);
											if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
										    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#else
										    PROCReadD0(V->var,0,0,0,0,FALSE);
#endif
												}
				              break;
										case 0:
											// nulla da fare, v.68000 sopra +=
											break;
										default:
											PROCError(1001,"+=");
											break;
			              }
			            break;
		            case '*':
		            case '/':
		            case '%':
                  subSpezReg((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0),u);
			            switch(V->Q) {
			              case VALUE_IS_VARIABILE:
#if MICROCHIP
											ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#elif MC68000
											PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);		// me ne frego della size!
											ReadVar(V->var,0,0,0,FALSE);
#else
											ReadVar(V->var,0,0,0,FALSE);
#endif
//				            i=Regs->Inc(FNGetMemSize(V->type,V->size,0));
											if(R.Q & VALUE_IS_COSTANTE) {
												if(Optimize & OPTIMIZE_CONST && (i=FNIsPower2(R.cost->l))) {			// ottimizzo potenze di 2!
													if(*TS=='*' || *TS=='/') {
														R.cost->l=i;
														V->Q=subShift(*TS=='*',MODE_IS_CONSTANT2,V->Q,V->var,V->type,V->size,R.type,
															V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
														}
													else 
														j=2;
													}
												else
													j=2;
	// 	                    PROCUseCost(RQ,R.type,R.size,&R.cost);
												}
											else {
#if MICROCHIP
												ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,FALSE);		// FINIRE
#elif MC68000
						// siamo già a posto...
#else
												ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,FALSE);
#endif
												j=0;
												}
											V->Q=subMul(*TS,j,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,
												V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
	//				            if(!i)
	//				              Regs->Dec(FNGetMemSize(V->type,V->size,0));
											// caso unico: prima DEC di store
											StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
											break;
										case VALUE_IS_D0:  		                   // non finito...
#if MICROCHIP
									    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#else
									    PROCReadD0(V->var,0,0,0,0,FALSE);
#endif
											if(R.Q & VALUE_IS_COSTANTE)
												j=2;
	// 	                    PROCUseCost(RQ,R.type,R.size,&R.cost);
											else {
#if MICROCHIP
			                  ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,0,FALSE);		// FINIRE
#else
			                  ReadVar(R.var,V->type,FNGetMemSize(V->type,V->size,0/*dim*/,1),0,FALSE);
#endif
												j=0;
												}
											V->Q=subMul(*TS,j,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,
												V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
											PROCStoreD0(V->var,V->Q,V->var,V->cost,FALSE);
											break;
										default:  
											PROCError(1001,"*=");
											break;
										}
									break;
		            case '<':
		            case '>':
                  subSpezReg((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0),u);
		              if(T) {
//		                j=FNGetMemSize(V->type,V->size,1)>2 ? 2 : 1;
	                	i=*(int *)R.var->label;
	                	u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[1].ofs=i;
	                	u[3].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[3].ofs=i+2;
                    }
                  if(R.Q==VALUE_IS_COSTANTE)
                    j=2;
                  else {
                    j=0;
                    if(R.Q==VALUE_IS_COSTANTEPLUS) {
#if MICROCHIP
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,0,FALSE);		// FINIRE
#else
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,FALSE);
#endif
											}
                    }
			            switch(V->Q) {
			              case VALUE_IS_VARIABILE:
			              switch(V->var->classe) {
			                case CLASSE_AUTO:
			                	i=MAKEPTROFS(V->var->label);
			                	u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                	u[0].ofs=i;
#if MC68000
												// SOLO se lo shift è 1, costante, e la var 16bit, si può fare DIRETTAMENTE operazione
												if(j==2 && V->size==2 && R.cost->l==1) {
													}
												else
			                    ReadVar(V->var,0,0,0,FALSE);
#else
    		                if(FNGetMemSize(V->type,V->size,0/*dim*/,1) > 2) {
				                  u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
				                  u[1].ofs=i+2;
//				                  u[1].s=0;
		                      }
#endif
				                V->Q=subShift(*TS=='<',j,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
													&u[0],&u[1],&u[2],&u[3],TRUE);
												if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
			                    ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#else
			                    ReadVar(V->var,0,0,0,FALSE);
#endif
													}
#if MC68000
												// in teoria se lo shift è 1, costante, e la var 16bit, si può fare DIRETTAMENTE operazione
//												if(j==2 && V->size==2 && R.cost->l==1) {
//													}
//												else
//													StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
												// TUTTO FATTO :)
#endif
			                  break;
			                case CLASSE_REGISTER:                // anche register
//							          if(RQ != 8)
//							            i=Regs->Inc(FNGetMemSize(V->type,V->size,0));
#if MICROCHIP
		                    ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#elif MC68000
#else
		                    ReadVar(V->var,0,0,0,FALSE);
#endif
#if MC68000
												u[0].s.n=MAKEPTRREG(V->var->label);
 			                  V->Q=subShift(*TS=='<',j,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
													&u[0],&u[1],&u[2],&u[3],TRUE);
#else
//							          if(RQ != 8) {
			                  // passo INVERTITI Dr e Dr1 (e h)
//  			                  *V->Q=subShift(*TS=='<',j,V->type,V->size,R.type,&V->cost,&R.cost,sRegs[1].Dr,sRegs[0].Dr,sRegs[1].Drh,sRegs[0].Drh,sRegs[3].Dr,sRegs[2].Dr,sRegs[3].Drh,sRegs[2].Drh);
//  			                  }
//  			                else  
  			                  V->Q=subShift(*TS=='<',j,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
														&u[0],&u[1],&u[2],&u[3],TRUE);
			                  StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
//							          if(RQ != 8) {
//  						            if(!i)
//  						              Regs->Dec(FNGetMemSize(V->type,V->size,0));
//  						            }
#endif
			                  break;
			                case CLASSE_EXTERN:
			                case CLASSE_GLOBAL:
			                case CLASSE_STATIC:                // (was anche register
//							          if(RQ != 8)
//							            i=Regs->Inc(FNGetMemSize(V->type,V->size,0));
#if MICROCHIP
		                    ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#elif MC68000
												// (in teoria se lo shift è 1, costante, e la var 16bit, si può fare DIRETTAMENTE operazione
												if(j==2 && V->size==2 && R.cost->l==1) {
													}
												else
			                    ReadVar(V->var,0,0,0,FALSE);
#else
		                    ReadVar(V->var,0,0,0,FALSE);
#endif
//							          if(RQ != 8) {
			                  // passo INVERTITI Dr e Dr1 (e h)
//  			                  *V->Q=subShift(*TS=='<',j,V->type,V->size,R.type,&V->cost,&R.cost,sRegs[1].Dr,sRegs[0].Dr,sRegs[1].Drh,sRegs[0].Drh,sRegs[3].Dr,sRegs[2].Dr,sRegs[3].Drh,sRegs[2].Drh);
//  			                  }
//  			                else  
#if MC68000
												// (in teoria se lo shift è 1, costante, e la var 16bit, si può fare DIRETTAMENTE operazione
												if(j==2 && V->size==2 && R.cost->l==1) {
													}
												// v. subShift
#endif
  			                  V->Q=subShift(*TS=='<',j,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
														&u[0],&u[1],&u[2],&u[3],TRUE);
#if MC68000
												// (in teoria se lo shift è 1, costante, e la var 16bit, si può fare DIRETTAMENTE operazione
												if(j==2 && V->size==2 && R.cost->l==1) {
													}
												else
#endif
			                  StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
//							          if(RQ != 8) {
//  						            if(!i)
//  						              Regs->Dec(FNGetMemSize(V->type,V->size,0));
//  						            }  
			                  break;
			                }
			              break;
			            case VALUE_IS_D0:  		                   // non va se (de) o (bc)...
#if ARCHI
	                	u[0].s.n=(int)V->var->func;
	                	if(u[0].s.n <= 3 && (int)V->var->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)V->var->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)V->var->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif Z80
	                	u[0].s.n=(int)V->var->func;
	                	if(u[0].s.n <= 3 && (int)V->var->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("add",&u[0],(int)V->var->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)V->var->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif I8086
	                	u[0].s.n=(int)V->var->func;
	                	if(u[0].s.n <= 3 && (int)V->var->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)V->var->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)V->var->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MC68000
	                	u[0].s.n=(int)V->var->func;
	                	if(u[0].s.n <= Regs->UserBase && (int)V->var->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
//											Op2A("add",&u[0],(int)V->var->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)V->var->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#elif MICROCHIP
	                	u[0].s.n=(int)V->var->func;
	                	if(u[0].s.n <= 3 && (int)V->var->parm) {
		                	u[0].mode=OPDEF_MODE_REGISTRO;
	                		u[0].ofs=0;
											Op2A("ADD",&u[0],(int)V->var->parm,0);
	                	  }
	                	else { 
	                		u[0].ofs=(int)V->var->parm;
	                		}
	                	u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
#endif
		                V->Q=subShift(*TS=='<',j,V->Q,V->var,V->type,V->size,R.type,V->cost,R.cost,
											&u[0],&u[1],&u[2],&u[3],TRUE);
										if(Pty==14) {             // ritorna expr in hl se serve
#if MICROCHIP
									    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#elif MC68000
#else
									    PROCReadD0(V->var,0,0,0,0,FALSE);
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
                  subSpezReg((uint8_t)FNGetMemSize(V->type,V->size,0/*dim*/,0),u);
		              if(T) {
//		                j=FNGetMemSize(V->type,V->size,1)>2 ? 2 : 1;
	                	i=*(int *)R.var->label;
	                	u[3].mode=u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
	                	u[1].ofs=i;
	                	u[3].ofs=i+2;
                    }
                  if(R.Q==VALUE_IS_COSTANTE)
                    j=2;
                  else {
                    j=0;
                    if(R.Q==VALUE_IS_COSTANTEPLUS) {
#if MICROCHIP
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,0,FALSE);		// FINIRE
#else
   	                  PROCUseCost(R.Q,R.type,R.size,R.cost,FALSE);
#endif
											}
                    }
			            switch(V->Q) {
			              case VALUE_IS_VARIABILE:
											switch(V->var->classe) {
												int16_t i2;
												case CLASSE_AUTO:
			                		i=MAKEPTROFS(V->var->label);
			                		u[0].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			                		u[0].ofs=i;
    											if(FNGetMemSize(V->type,V->size,0/*dim*/,1) > 2) {
														u[1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
														u[1].ofs=i+2;
	//				                  u[1].s=0;
														}
													goto my_aox;
												case CLASSE_REGISTER:
													u[0].mode=OPDEF_MODE_REGISTRO;
													u[0].s.n=MAKEPTRREG(V->var->label);
	//				                  u[0].s=0;
my_aox:
													i2=0;
													V->Q=subAOX(*TS,&i2,j,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,
														V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
													if(Pty==14) {             // ritorna expr in hl se serve
#if MICROCHIP
				                    ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#elif MC68000
#else
				                    ReadVar(V->var,0,0,0,FALSE);
#endif
														}
													break;
												default:
													// passo INVERTITI Dr e Dr1 (e h)
//							          if(RQ != 8)
//							            i=Regs->Inc(FNGetMemSize(V->type,V->size,0));
#if MICROCHIP
				                    ReadVar(V->var,0,0,0,0,FALSE);		// FINIRE
#elif MC68000
#else
				                    ReadVar(V->var,0,0,0,FALSE);
#endif
													i2=0;
													V->Q=subAOX(*TS,&i2,j,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,
														V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
													StoreVar(V->var,V->Q,V->var,V->cost,FALSE);
//							          if(RQ != 8) {
//  						            if(i)
//  						              Regs->Dec(FNGetMemSize(V->type,V->size,0));
//  						            }  
				                  break;
				                }
				              break;
										case VALUE_IS_D0:
											{int16_t i2=v;
	                		subOfsD0(V->var,V->var->size,(int)V->var->func,(int)V->var->parm);	
	                		u[0].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
											V->Q=subAOX(*TS,&i2,j,V->Q,V->var,V->type,V->size,R.Q,R.type,R.size,
												V->cost,R.cost,&u[0],&u[1],&u[2],&u[3],TRUE);
											if(Pty==14) {              // ritorna expr in hl se serve
#if MICROCHIP
										    PROCReadD0(V->var,0,0,0,0,0,FALSE);
#elif MC68000
										    PROCReadD0(V->var,0,0,0,0,FALSE);			// SERVE?? verificare 7/11/25
#else
										    PROCReadD0(V->var,0,0,0,0,FALSE);
#endif
												}
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
//		          subEvEx(14,cond,&R.type,&R.size,&RQ,&RVar,R.cost,&RTag,&RDim);     inutile
		          break;
		        default:
		          break;
		        }
	        }		//  ARITM_IS_OPERANDO
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
  
char *Ccc::ConRecEval(char *s, uint8_t Pty, long *l1) {
  long l2;
  char ch;
  int Times=0;
	uint8_t InBrack=0;
	bool Go=0,fError=0;
	char firstchar=0;
  char *p;
	int8_t isWhat=0;		// 0 inizio, 1=value, 2=operand; FORSE dovrebbe essere statica... gestendo liv. ricorsione, ma pare cmq andare (v. anche FNRev
  int i,j;
  
  do {
	  ch=*s;
		if(iscsym(ch) && !firstchar)
			firstchar=ch;
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
					if(isWhat==1) {
  					if(Pty > 4) {
							s++;
							isWhat=2;
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
						PROCError(2059,s);
						Go=1;
						}
	        }
	      else {
					if(isWhat==1) {
  					if(Pty > 2) {
							s++;
							isWhat=2;
							s=ConRecEval(s,2,l1);
							if(ch=='-')
								*l1=-*l1;
							}
						else
							Go=1;
						}
					else {
						PROCError(2059,s);
						Go=1;
						}
					}
	      break;
	    case '*':
	    case '/':
	    case '%':
				if(isWhat==1) {
					if(Pty > 3) {
						s++;
						isWhat=2;
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
					}
				else {
					PROCError(2059,s);
					Go=1;
					}
	      break;
	    case '!':
	    case '~':
				if(isWhat==1) {
					if(Pty > 2) {
						s++;
						isWhat=2;
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
					}
				else {
					PROCError(2059,s);
					Go=1;
					}
	      break;
	    case 'l':
	    case 'r':
				if(isWhat==1) {
					if(Pty > 5) {
						s++;
						isWhat=2;
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
					}
				else {
					PROCError(2059,s);
					Go=1;
					}
	      break;
	    case '<':
	    case '@':                // diverso
	    case '=':
	    case '>':
	      i=0;
				if(isWhat==1) {
					if(Pty > 5) {
						isWhat=2;
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
					}
				else {
					PROCError(2059,s);
					Go=1;
					}
	      break;
	    case '&':                        // and,or
	    case '|':
	    case '^':
				if(isWhat==1) {
					if(Pty > 6) {
						char s2=*++s;
						s++;
						isWhat=2;
						s=ConRecEval(s,6,&l2);
	//	    myLog->print(0,"qui i valori sono %ld e %ld\n",*l1,l2);
						switch(ch) {
							case '&':
								if(s2=='&')
									*l1=*l1 && l2;
								else
									*l1=*l1 & l2;
								break;
							case '|':
								if(s2=='|')
									*l1=*l1 || l2;
								else
									*l1=*l1 | l2;
								break;
							case '^':
								*l1=*l1 ^ l2;
								break;
							}  
						}
					else
						Go=1;
					}
				else {
					PROCError(2059,s);
					Go=1;
					}
	      break;
	    case ' ':
	      s++;
	      if(Times)
					Times--;
	      break;
	    case 0:
	      Go=TRUE;
	      break;
			case 'd':
				// gestire FNDefined(
//				break;
	    default:
	      *l1=0;
        if(*s) {
					if(isWhat==1) {
						PROCError(2059,s);
						Go=1;
						}
					else {
						if(isdigit(firstchar) && isdigit(*s)) {
							while(*s && isdigit(*s)) {
								*l1=(*l1 * 10) + (*s - '0');
								s++;
								}
							}
						else 
							s++;
						isWhat=1;
						}
					}
				else
					Go=TRUE;
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
          
