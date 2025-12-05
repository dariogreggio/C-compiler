#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <math.h>


#if MICROCHIP
int Ccc::PROCReadD0(struct VARS *V, O_TYPE T, O_SIZE S, uint16_t cond, int ofs, bool asPtr, uint8_t lh) {   // m=0 se norm, 1 se condiz.
#else
int Ccc::PROCReadD0(struct VARS *V, O_TYPE T, O_SIZE S, uint16_t cond, int ofs, bool asPtr) {   // m=0 se norm, 1 se condiz.
#endif
  int I,i,t,s,s1;                                            //   modif=1 se offset reg., 0 se offs. num
  char T1S[64],AS[64],myBuf[64];
  struct OP_DEF u;
  int A=MAKEPTROFS(V->label);
//  int A=(int)V->func/*,B=(int)V->parm*/;
  struct VARS *v;
//  struct OP_DEF *A=(struct OP_DEF *)(V->func),*B=(struct OP_DEF *)(V->parm);
  

//  	myLog->print(0,"ReadD0:  TIPO %x, SIZE %x, cond %d, ofs %d asPtr %d\n",T,S,cond,ofs,asPtr,V->modif);

  t=V->type;
  if(!S) {
	  S=V->size;
    T=V->type;                    // preservo solo unsigned... (da rivedere)
	  }
	else {
	  T=(V->type & ~(VARTYPE_UNSIGNED | VARTYPE_IS_POINTER)) | (T & (VARTYPE_UNSIGNED | VARTYPE_IS_POINTER));		// e ROM??
	  }  
  s1=FNGetMemSize(T,S,0/*dim*/,1);
  s=FNGetMemSize(V->type,V->size,0/*dim*/,1);
  s1=__min(s,s1);
  if(debug)
  	myLog->print(0,"TIPO %x, SIZE %x, BS/ofs %d, F %d\n",t,s,ofs,V->modif);
  if(((t & VARTYPE_ARRAY) && ((t & VARTYPE_IS_POINTER) <2)) || ((t & (VARTYPE_STRUCT | VARTYPE_UNION)) && (!(t & VARTYPE_IS_POINTER)))) {
#if ARCHI
		if(*B->s != 'R') {
		  I=B->l;
		  if(I>=0) 
		  	_tcscpy(AS,"ADD");
		  else {
			  _tcscpy(AS,"SUB");
				I=-I;
				}
//		sprintf(TS,"#%d",I);
			if(I)
				PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO,I,NULL);
		  }
		else {
		  if(F && (s>1))
			  sprintf(T1S,"ASL #%d",log(s)/log(2));
		  else 
			  *T1S=0;
		  PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,B->s,T1S);
		  }
#elif Z80
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,B);
			// bah non serve... qua idem SICURO??? 2025
		  }
		else {
		  if(s>1)
			  sprintf(T1S,"ASL #%u",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l);      // non ha senso, v. sotto
		  }
#elif I8086
		if(!V->modif) {
		  if(ofs >=0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,
				asPtr ? Regs->P : Regs->D);
		  }
		else {
		  if(s>1)
			  sprintf(T1S,"ASL #%u",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l,T1S,NULL);    // così non ha senso
		  }
#elif MC68000
		if(!V->modif) {
			if((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) {
				if(ofs >= 0) 
	  			_tcscpy(AS,"addi.l");		// memorymodel!
				else {
		  		_tcscpy(AS,"subi.l");
					ofs=-ofs;
					}
				}
			else {
				if(ofs >= 0) 
	  			_tcscpy(AS,"addi.w");		// memorymodel!
				else {
		  		_tcscpy(AS,"subi.w");
					ofs=-ofs;
					}
				}
			if(ofs)
				PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO,ofs,OPDEF_MODE_REGISTRO32,
					asPtr ? Regs->P : Regs->D);
			// (bah non serve... qua  SE SI OTTIMIZZA lettura indiretta con offset!
		  }
		else {
		  if(s>1)
			  sprintf(T1S,"lsl #%u",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l,T1S,NULL);    // così non ha senso
		  }
#elif MICROCHIP
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADDLW");
		  else {
		  	_tcscpy(AS,"SUBLW");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO8,ofs);
		  }
		else {
		  if(s>1)
			  sprintf(T1S,"ASL #%d",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l);      // non ha senso, v. sotto
		  }
#endif
		CHECKPOINTER();
	  }		// se puntatore semplice, RIVERIFICARE 2025
  else {
    I=subOfsD0(V,s1,A,ofs);
		if(I || cond)
			CHECKPOINTER();
#if ARCHI
	  switch(s) {
		  case 1:
//			  _tcscpy(MyBuf,TS);
//			  _tcscat(MyBuf,"]");
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,",[",Regs->D,MyBuf);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I);
		    break;
			case 2:
//			  _tcscpy(MyBuf,TS);
//			  _tcscat(MyBuf,"]");
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I);
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D+1,",[",Regs->D,MyBuf);
//			  _tcscpy(MyBuf,T1S);
//			  _tcscat(MyBuf,"]");
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,",[",Regs->D,MyBuf);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO,Regs->D,"ASL #8");
			  break;
			case 4:
//			  _tcscpy(MyBuf,TS);
//			  _tcscat(MyBuf,"]");
//			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D,",[",Regs->D,MyBuf);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I);
			  break;
			case 8:
//			  _tcscpy(MyBuf,TS);
//			  _tcscat(MyBuf,"]");
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I);
//			  _tcscpy(MyBuf,T2S);
//			  _tcscat(MyBuf,"]");
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D,I+2);
			  break;
			default:
			  _tcscpy(MyBuf,TS);
			  _tcscat(MyBuf,"]");
			  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,Regs->D,",[",Regs->D,MyBuf);    // STRUTTURE
			  break;
		  }      
#elif Z80
	  if(I) {
		  i=I;
		  if(i>=(127-s)) {
		    u.mode=OPDEF_MODE_REGISTRO_LOW8;
		    u.s.n=Regs->D;
		    u.ofs=0;
		    Op2A("add",&u,i,0);
			  u.ofs=0;
		    }
		  else
		    u.ofs=i;
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,A);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_FRAMEPOINTER,0);
			  u.mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			  }
			else {
			  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			  u.s.n=A;
			  }  
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=Regs->P;
//		  u.s.n=A;
		  u.ofs=0;
		  }
	  switch(s1) {
		  case 1:
			  if(I) {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					else  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
          }
        else {
				  if(A>3 || A==0) {
					  if(cond)
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  else  
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  }  
				  else {
    		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
				    if(!cond)
				      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
				    }
				  }
		    break;
			case 2:
			  if(I) {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					else
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					u.ofs++;
				  if(cond)
				    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
				  else
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
					}
			  else {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
					  }
					else {
	  			  if(cond)
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  else
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  u.ofs++;
					  }
				  if(Regs->D == 0) {
	  			  if(cond) {
	  			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs,NULL);
	  			    }
	  			  else  {
	  			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
						  if(A <= 3)
	    			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	  			    }
	  			  }
				  else {
					  if(A <= 3) {
					    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
				      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
					    if(cond)
						    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  else
					      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
				      }
				    else {
					    if(cond)
						    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
					    else
						    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					    }
				    }
				  }
			  break;
			case 4:
			  if(I) {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					else
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
				  if(cond) {
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
				    }
				  else {  
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
						u.ofs++;
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
					  }
					}
			  else {
				  if(cond) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  if(A <= 3)
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
						else
						  u.ofs++;					  
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
					  if(A <= 3)
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
						else
						  u.ofs++;					  
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
					  if(A <= 3)
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
						else
						  u.ofs++;					  
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
				    }
				  else {  
					  if(A <= 3)
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
						else  
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  if(A <= 3) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
						  }
						else {
						  u.ofs++;					  
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
						  }
					  if(A <= 3) {
	  				  if(Regs->D > 0) {
	  					  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
		  				  }
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
		  				}  
						else
						  u.ofs++;					  
  				  if(Regs->D == 0) {
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
	  				  }
	  				else {  
						  if(A <= 3) {
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
		  				  }
		  				else  
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
	  				  }
					  if(A <= 3)
						  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
						else
						  u.ofs++;					  
  				  if(Regs->D == 0) {
   					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
	  				  }
	  				else {  
						  if(A <= 3) {
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
		  				  }
		  				else  
		  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
	  				  }
					  if(A <= 3) {
					    if(Regs->D > 0)
    					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	  				  }
					  if(A <= 3) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
					  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
					    }
					  }
				  }
			  break;
			default:
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);    // strutture
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			  break;
		  }      
	  if(I) {
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
			  }
		  }  
#elif I8086
	  if(I) {
		  i=I;
	    u.ofs=i;
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,OPDEF_MODE_FRAMEPOINTER,0);
			  u.mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
				u.s.n=Regs->P;
			  }
			else {
			  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
				u.s.n=Regs->P;
//			  u.s.n=A;
			  }  
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			u.s.n=Regs->P;
//		  u.s.n=A;
		  u.ofs=0;
			}
	  switch(s1) {
		  case 1:
			  if(I) {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					else  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
          }
        else {
				  if(A>3 || A==0) {
					  if(cond)
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  else  
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  }  
				  else {
    		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
/*				    if(!cond)
				      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);*/
				    }
				  }
		    break;
			case 2:
			  if(I) {
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,u.mode,&u.s,u.ofs);
				  if(cond)
				    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D);
					}
			  else {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					  }
					else {
	  			  if(cond)
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					  else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					  u.ofs++;
					  }
  			  if(cond) 
  			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
  			  else  {
  			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					  if(A <= 3)
    			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D);
	  			  }
				  }
			  break;
			case 4:
			  if(I) {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					else
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
				  if(cond) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,(asPtr ? Regs->P : Regs->D)+1,u.mode,&u.s,u.ofs+2);
				    }
				  else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,(asPtr ? Regs->P : Regs->D)+1,u.mode,&u.s,u.ofs+2);
					  }
					}
			  else {
			    if(cond)
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
					else
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,u.mode,&u.s,u.ofs);
				  if(cond) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,(asPtr ? Regs->P : Regs->D)+1,u.mode,&u.s,u.ofs+2);
				    }
				  else {  
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,(asPtr ? Regs->P : Regs->D)+1,u.mode,&u.s,u.ofs+2);
					  }
				  }
			  break;
			default:
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);    // strutture FINIRE
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			  break;
		  }      
	  if(I) {
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
			  }
		  }  
#elif MC68000
		char movString2[16];
		_tcscpy(movString2,movString);
		switch(s) {
			case 1:
				_tcscat(movString2,".b");
				break;
			case 2:
				_tcscat(movString2,".w");
				break;
			case 4:
				_tcscat(movString2,".l");
				break;
			}   
	  if(I) {
		  i=I;
	    u.ofs=i;
// mah non dovrebbe servire qua, uso indiretto su A0			PROCOper(LINE_TYPE_ISTRUZIONE,"addi.l",OPDEF_MODE_IMMEDIATO32,u.ofs,OPDEF_MODE_REGISTRO32,Regs->P);
			u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			u.s.n=Regs->P;
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
//		  u.s.n=A;
			u.s.n=Regs->P;
		  u.ofs=ofs		/*A*/;
			}
	  switch(s1) {
		  case 1:
			  if(I) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO,Regs->D);
					}
			  else {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO,Regs->D);
				  }
		    break;
			case 2:
			  if(I) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D);
					}
			  else {		// ok
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D);
				  }
			  break;
			case 4:
			  if(I) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO32,asPtr ? Regs->P : Regs->D);
					}
			  else {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO32,asPtr ? Regs->P : Regs->D);
				  }
			  break;
			default:
			  PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO,Regs->P);		    // strutture... finire!
			  break;
			case 0:			// arriva così se array è una costante! tipo literal string "abcd"[			.. vedere come va
			  break;
		  }      
#elif MICROCHIP
	  if(I) {
		  i=I;
		  if(i>=(127-s)) {
		    u.mode=OPDEF_MODE_REGISTRO_LOW8;
		    u.s.n=Regs->D;
		    u.ofs=0;
		    Op2A("ADD",&u,i,0);
			  u.ofs=0;
		    }
		  else
		    u.ofs=i;
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,A);
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ex",OPDEF_MODE_STACKPOINTER_INDIRETTO,0,8,0);
			  u.mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
			  }
			else {
			  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			  u.s.n=A;
			  }  
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=A;
		  u.ofs=0;
		  }
	  switch(s1) {
		  case 1:
			  if(I) {
					if(cond) {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						}
					else {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						}
          }
        else {
				  if(A>3 || A==0) {
					  if(cond)
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  else  
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
					  }  
				  else {
    		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
				    if(!cond)
				      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
				    }
				  }
		    break;
			case 2:
			  if(I) {
					if(cond) {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						}
					else {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						}
					u.ofs++;
					if(cond) {
						if(V->type & VARTYPE_ROM) {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
						else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
						}
					else {
						if(V->type & VARTYPE_ROM) {
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
							}
						else {
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
							}
						}
					}
			  else {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
					  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
					  }
					else {
						if(cond) {
							if(V->type & VARTYPE_ROM) {
						    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
								}
							else {
						    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
								}
							}
						else {
							if(V->type & VARTYPE_ROM) {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
								}
							}
					  u.ofs++;
					  }
				  if(Regs->D == 0) {
	  			  if(cond) {
							if(V->type & VARTYPE_ROM) {
		  			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
								}
							else {
		  			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
								}
	  			    }
	  			  else  {
							if(V->type & VARTYPE_ROM) {
			  			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
								}
							else {
			  			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
								}
							if(A <= 3) {
								if(V->type & VARTYPE_ROM) {
		    			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
									}
								else {
		    			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
									}
								}
	  			    }
	  			  }
				  else {
					  if(A <= 3) {
					    PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
				      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
							if(cond) {
								if(V->type & VARTYPE_ROM) {
							    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
									}
								else {
							    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
									}
								}
							else {
								if(V->type & VARTYPE_ROM) {
						      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
									}
								else {
						      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
									}
								}
				      }
				    else {
							if(cond) {
								if(V->type & VARTYPE_ROM) {
							    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
									}
								else {
							    PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
									}
								}
							else {
								if(V->type & VARTYPE_ROM) {
							    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
									}
								else {
							    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
									}
								}
					    }
				    }
				  }
			  break;
			case 4:
			  if(I) {
					if(cond) {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							}
						}
					else {
						if(V->type & VARTYPE_ROM) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							}
						}
				  if(cond) {
						if(V->type & VARTYPE_ROM) {
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
						else {
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
				    }
				  else {  
						if(V->type & VARTYPE_ROM) {
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
							}
						else {
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
							u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
							}
					  }
					}
			  else {
				  if(cond) {
						if(V->type & VARTYPE_ROM) {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
							PROCOper(LINE_TYPE_ISTRUZIONE,"or",u.mode,&u.s,u.ofs);
							}
				    }
				  else {  
						if(V->type & VARTYPE_ROM) {
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
							else  
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,u.mode,&u.s,u.ofs);
							if(A <= 3) {
								PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
								}
							else {
								u.ofs++;					  
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
								}
							if(A <= 3) {
	  						if(Regs->D > 0) {
	  							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
		  						}
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
		  					}  
							else
								u.ofs++;					  
  						if(Regs->D == 0) {
	  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
	  						}
	  					else {  
								if(A <= 3) {
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
		  						}
		  					else  
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,u.mode,&u.s,u.ofs);
	  						}
							if(A <= 3)
								PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
								u.ofs++;					  
  						if(Regs->D == 0) {
   							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
	  						}
	  					else {  
								if(A <= 3) {
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
		  						}
		  					else  
		  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,u.mode,&u.s,u.ofs);
	  						}
							if(A <= 3) {
								if(Regs->D > 0)
    							PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
	  						}
							if(A <= 3) {
								PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
					  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
								}
							}
						}
				  }
			  break;
			default:
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,u.mode,&u.s,u.ofs);    // strutture
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,u.mode,&u.s,u.ofs);
			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			  break;
		  }      
	  if(I) {
		  if(A <= 3) {
			  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
			  }
		  }  
#endif

		if(s < FNGetMemSize(T,S,0/*dim*/,1))
		  PROCCast(T,S,V->type,V->size,-1);
		}      
	
  return 0;
  }

int Ccc::PROCStoreD0(struct VARS *V, int8_t RQ, struct VARS *RVar, union STR_LONG *RCost, bool isPtr) {
  int I,i;                                            //   modif=1 se offset reg, 0 se offs. num
	O_TYPE t;
	O_SIZE s;
  char T1S[64],AS[64],myBuf[64];
  struct OP_DEF u;
  int A=(int)V->label, ofs=(int)V->parm;
  long l;
  struct VARS *v;
  
//  if(V->type & VARTYPE_CONST)		// mettere MA SULL'OGGETTO PUNTATO
//		PROCError(2166,NULL);
  
#if ARCHI
  _tcscpy(AS,Pt->s);
  switch(*AS) {
		case 'L':
			*AS='S';
			*(AS+1)='T';
			if(!V) {
				p=strchr(AS,',');   
				_tcsncpy(MyBuf,AS,p-AS);
				sprintf(MyBuf1,"%d",atoi(p-1)+1);
				_tcscat(MyBuf,MyBuf1);
				_tcscat(MyBuf,p);
				_tcscpy(AS,MyBuf);
		//        AS=LEFT$(A$,I%-2)+STR$(VALMIDS(A$,I%-1,1)+1)+MIDS(A$,I%);
				}
			break;
		case 'M':
			if(!V) {
				_tcsncpy(MyBuf,AS,5);
				sprintf(MyBuf1,"%d",atoi(AS+5)+1);
				_tcscat(MyBuf,MyBuf1);
				_tcscat(MyBuf,AS+6);
				_tcscpy(AS,MyBuf);
	//        A$=LEFT$(A$,5)+STR$(VALMIDS(A$,6,1)+1)+MIDS(A$,7);
				}
			p=strchr(AS,',');
			k=_tcslen(AS);
			_tcsncpy(MyBuf,AS,4);
			_tcsncat(MyBuf,p+1,k-(p-AS));
			_tcscat(MyBuf,",");
			_tcscat(MyBuf,AS+5);
			_tcscpy(AS,MyBuf);
	//      AS=LEFT$(A$,4)+MIDS(A$,I%+1,K%-I%)+","+MIDS(A$,5,I%-5);
			break;
		default:
	//    PROCWarn(2106)
			break;
		}
  _tcscpy(Pt->s,AS);
#endif

  t=V->type;
  s=V->size;

	// v. anche StoreVar
  if(RQ==VALUE_IS_VARIABILE) {		// SBAGLIATO Direi, servirebbe il tipo anche delle costanti!! passare type & size separatamente
//	if(RVar) {
		if((RVar->type & VARTYPE_IS_POINTER) && (V->type & VARTYPE_IS_POINTER)) {    // warning se puntatori diversi
			O_SIZE s1,s2;
			if((RVar->type & ~VARTYPE_ARRAY) != (V->type & ~VARTYPE_ARRAY))		
				PROCWarn(4047);

			// se array OCCHIO che mancano le dim... finire
			s1=FNGetMemSize(V->type,V->size,0,0);
			s2=FNGetMemSize(RVar->type,RVar->size,0,0);
			if(s1 != s2)		// !
				PROCWarn(4047);
			}
		}

  if(debug) {
		myLog->print(0,"TIPO %x, SIZE %x, ofs %u, F %d\n",t,s,ofs,V->modif);
		}
  if(((t & VARTYPE_ARRAY) && ((t & VARTYPE_IS_POINTER) <2)) || ((t & (VARTYPE_STRUCT | VARTYPE_UNION)) && (!(t & VARTYPE_IS_POINTER)))) {
#if Z80 
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,ofs);
		  }
		else {
		  if(V->modif && (s>1))
			  sprintf(T1S,"ASL #%d",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l);      // non ha senso, v. sotto
		  }
#elif I8086 
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,ofs);
		  }
		else {
		  if(V->modif && (s>1))
			  sprintf(T1S,"ASL #%d",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l);      // non ha senso, v. sotto
		  }
#elif MC68000 
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_IMMEDIATO16,ofs,OPDEF_MODE_REGISTRO,Regs->D);
		  }
		else {
		  if(V->modif && (s>1))
			  sprintf(T1S,"ASL %d",log(s)/log(2));
		  else 
			  *T1S=0;
		  }
#elif MICROCHIP
		if(!V->modif) {
		  if(ofs >= 0) 
	  		_tcscpy(AS,"ADD");
		  else {
		  	_tcscpy(AS,"SUB");
			  ofs=-ofs;
			  }
	  	PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,ofs);
		  }
		else {
		  if(V->modif && (s>1))
			  sprintf(T1S,"ASL #%d",log(s)/log(2));
		  else 
			  *T1S=0;
//		  PROCOper("ADD",Regs->DS,B->l);      // non ha senso, v. sotto
		  }
#endif
	  }
  else {
//	*TS=0;
    I=subOfsD0(V,s,A,ofs);
		CHECKPOINTER();
#if ARCHI

#elif Z80
	  if(I) {
		  i=I;
		  if(i>=(127-s)) {
		    u.mode=OPDEF_MODE_REGISTRO_LOW8;
		    u.s.n=Regs->D;
		    u.ofs=0;
		    Op2A("add",&u,i,0);
			  u.ofs=0;
		    }
		  else
		    u.ofs=i;
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=Regs->P;
//		  u.s.n=A;
		  u.ofs=0;
		  }
#elif I8086
	  if(I) {
		  i=I;
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=Regs->P;
		  u.ofs=i;
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=Regs->P;
		  u.ofs=0;
		  }
#elif MC68000
	  if(I) {
		  i=I;
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			u.s.n=Regs->P;		// v. anche isPtr
		  u.ofs=i;
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
			u.s.n=Regs->P;		// v. anche isPtr
		  u.ofs=0;
		  }
#elif MICROCHIP
	  if(I) {
		  i=I;
		  if(i>=(127-s)) {
		    u.mode=OPDEF_MODE_REGISTRO_LOW8;
		    u.s.n=Regs->D;
		    u.ofs=0;
		    Op2A("add",&u,i,0);
			  u.ofs=0;
		    }
		  else
		    u.ofs=i;
		  }
		else {
		  u.mode=OPDEF_MODE_REGISTRO_INDIRETTO;
		  u.s.n=A;
		  u.ofs=0;
		  }
#endif

#if ARCHI
	  switch(RQ) {
	    case VALUE_IS_COSTANTE:
	      l=RCost->l;
			  switch(s) {
				  case 1:
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,l);
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,l);
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;
		    			}
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,*(((char *)&l)+1));
					  break;
					case 4:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,l);
					  if(I) {
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
							}
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,*(((char *)&l)+1));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,*(((char *)&l)+2));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,*(((char *)&l)+3));
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }
				break;
			default:
			  switch(s) {
				  case 1:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							}  
  			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
					  break;
					case 4:
					  if(I) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
							}
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;					  
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }  
			  break;  
      }

#elif Z80
	  switch(RQ) {
	    case VALUE_IS_COSTANTE:
	      l=RCost->l;
			  switch(s) {
				  case 1:
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,LOWORD(l));
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;
		    			}
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(LOWORD(l)));
					  break;
					case 4:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
					  if(I) {
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,HIBYTE(LOWORD(l)));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(HIWORD(l)));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,HIBYTE(HIWORD(l)));
							}
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(LOWORD(l)));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,LOBYTE(HIWORD(l)));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(HIWORD(l)));
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }
				break;
			default:
			  switch(s) {
				  case 1:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							}  
  			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
					  break;
					case 4:
					  if(I) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
							}
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;					  
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }  
			  break;  
      }
#elif I8086
		char movString2[16];
		_tcscpy(movString2,movString);
		switch(s) {
			case 1:
				_tcscat(movString2," BYTE PTR ");
				break;
			case 2:
				_tcscat(movString2," WORD PTR ");
				break;
			case 4:
				_tcscat(movString2," DWORD PTR ");
				break;
			}   
	  switch(RQ) {
	    case VALUE_IS_COSTANTE:
	      l=RCost->l;
			  switch(s) {
				  case 1:
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(l)));
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
					  break;
					case 4:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
						if(CPU86<3)
							;
					  if(I) {
							u.ofs+=2;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO16,HIWORD(l));
							}
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIWORD(l));
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }
				break;
			case VALUE_IS_D0:
			case VALUE_IS_EXPR:
			case VALUE_IS_EXPR_FUNC:
			case VALUE_IS_VARIABILE:
			  switch(s) {
				  case 1:
						if(isPtr)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->P+1);
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				    break;
					case 2:
						if(isPtr)
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->P+1);
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->D);
					  break;
					case 4:
						if(CPU86<3)
							;
					  if(I) {
							if(isPtr) {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->P+1);
								u.ofs+=2;
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->P+2);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->D);
								u.ofs+=2;
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->D+1);
								}
							}
					  else {
							if(isPtr) {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->P+1);
								if(A <= 3)
									PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
								else
									u.ofs++;					  
	 							PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->P+2);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->D);
								if(A <= 3)
									PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
								else
									u.ofs++;					  
	 							PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO16,Regs->D+1);
								}
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }  
			  break;  
			default:
				PROCError(1001,"bad mode");
				break;	
      }
#elif MC68000
		char movString2[16];
		_tcscpy(movString2,movString);
		switch(s) {
			case 1:
				_tcscat(movString2,".b");
				break;
			case 2:
				_tcscat(movString2,".w");
				break;
			case 4:
				_tcscat(movString2,".l");
				break;
			}   
/*		if(V->classe=CLASSE_REGISTER)
			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,(uint8_t)V->func,OPDEF_MODE_REGISTRO32,Regs->P);
		else
			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->P);*/
	  switch(RQ) {
	    case VALUE_IS_COSTANTE:
			  switch(s) {
				  case 1:
						if(!LOBYTE(LOWORD(RCost->l)))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",u.mode,&u.s,u.ofs);
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),u.mode,&u.s,u.ofs);
				    break;
					case 2:
						if(!LOWORD(RCost->l))
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",u.mode,&u.s,u.ofs);
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),u.mode,&u.s,u.ofs);
					  break;
					case 4:
						if(!RCost->l)
							PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",u.mode,&u.s,u.ofs);
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_IMMEDIATO32,RCost->l,u.mode,&u.s,u.ofs);
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
//					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }
				break;
			case VALUE_IS_D0:
			case VALUE_IS_EXPR:
			case VALUE_IS_EXPR_FUNC:
			case VALUE_IS_VARIABILE:
			  switch(s) {
				  case 1:
						if(isPtr)
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P+1,u.mode,&u.s,u.ofs);
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,Regs->D,u.mode,&u.s,u.ofs);
				    break;
					case 2:
						if(isPtr)
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P+1,u.mode,&u.s,u.ofs);
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,Regs->D,u.mode,&u.s,u.ofs);
					  break;
					case 4:
					  if(I) {
							// boh qua
							if(isPtr)
							  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P+1,u.mode,&u.s,u.ofs);
							else
							  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,u.mode,&u.s,u.ofs);
							}
					  else {
							if(isPtr)
							  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P+1,u.mode,&u.s,u.ofs);
							else
							  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,u.mode,&u.s,u.ofs);
						  }
					  break;
				  }      
			  break;  
			default:
				PROCError(1001,"bad mode");
				break;	
      }
#elif MICROCHIP
	  switch(RQ) {
	    case VALUE_IS_COSTANTE:
	      l=RCost->l;
			  switch(s) {
				  case 1:
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,l);
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,l);
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;
		    			}
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(LOWORD(l)));
					  break;
					case 4:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,l);
					  if(I) {
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,HIBYTE(LOWORD(l)));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,LOBYTE(HIWORD(l)));
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO8,HIBYTE(HIWORD(l)));
							}
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(LOWORD(l)));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,LOBYTE(HIWORD(l)));
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_IMMEDIATO,HIBYTE(HIWORD(l)));
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }
				break;
			default:
			  switch(s) {
				  case 1:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
				    break;
					case 2:
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
					  if(I)
							u.ofs++;
					  else {
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;
							}  
  			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
					  break;
					case 4:
					  if(I) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
							u.ofs++;
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
							}
					  else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,A);
							else
							  u.ofs++;					  
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	  				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_LOW8,Regs->D+2);
						  if(A <= 3)
							  PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,Regs->D);
							else
							  u.ofs++;					  
	 					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,u.mode,&u.s,u.ofs,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+2);
						  }
					  break;
				  }      
			  if(I) {
				  if(A <= 3) {
					  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
					  }
				  }  
			  break;  
      }
#endif
		}
	
  return 0;
  }

int Ccc::PROCGetAdd(int8_t VQ, struct VARS *V, int ofs, bool asPtr) {
  long T;
  int i;
  char *BS;
  struct OP_DEF u;
  
  switch(VQ) {
    case VALUE_IS_COSTANTE:
#if ARCHI
			u.mode=OPDEF_MODE_REGISTRO;
			i=u.s.n=(int)V->func;
			if(i > 3) {
        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,i);
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
			  }
			i=u.ofs=(int)V->parm+ofs;
			if(i) {
			  i *= FNGetMemSize(V->type,V->size,1);
        Op2A("add",&u,i,0);
        }
#elif Z80 || I8051 || MICROCHIP
			u.mode=OPDEF_MODE_REGISTRO;
			i=u.s.n=(int)V->func;
			if(i > 3) {
        PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,i);
        PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
			  }
			i=u.ofs=(int)V->parm+ofs;
			if(i) {
			  i *= FNGetMemSize(V->type,V->size,0/*dim*/,1);
        Op2A("add",&u,i,0);
        }
#elif I8086
			u.mode=OPDEF_MODE_REGISTRO;
			i=u.s.n=(int)V->func;
			if(i > 3) {
        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,i,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D);
			  }
			i=u.ofs=(int)V->parm+ofs;
			if(i) {
			  i *= FNGetMemSize(V->type,V->size,0/*dim*/,1);
//        Op2A("add",&u,i,0);
        }
#elif MC68000
			u.mode=OPDEF_MODE_REGISTRO;
			i=u.s.n=(int)V->func;
			i=u.ofs=(int)V->parm+ofs;
			if(i) {
			  i *= FNGetMemSize(V->type,V->size,0/*dim*/,1);
//        Op2A("add",&u,i,0);
        }
#endif
     	break; 
    case VALUE_IS_VARIABILE: 	
		  T=V->type;
		  switch(V->classe) {
				case CLASSE_EXTERN:
				case CLASSE_GLOBAL:
				case CLASSE_STATIC:
		//      _tcscpy(AS,V->label);
#if ARCHI
			  	PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,AS);
#elif Z80 || I8051 
			  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&V->label,ofs);
#elif I8086
					/*MemoryModel*/
					if(CPU86>=3) {
						}
					if(asPtr)
				  	PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_REGISTRO,Regs->P,
							OPDEF_MODE_VARIABILE,/*"OFFSET DGROUP:",*/(union SUB_OP_DEF *)&V->label,ofs);
					else
			  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
							OPDEF_MODE_VARIABILE,/*"OFFSET DGROUP:",*/(union SUB_OP_DEF *)&V->label,ofs);
#elif MC68000
					if(asPtr)
			  		PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&V->label,ofs,
							OPDEF_MODE_REGISTRO32,Regs->P);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,(MemoryModel & 0xf) <= MEMORY_MODEL_SMALL ? "move.w" : "move.l",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,ofs,
							OPDEF_MODE_REGISTRO32,Regs->D);
// beh non serve				  PROCOper(LINE_TYPE_ISTRUZIONE,"addi.l",OPDEF_MODE_IMMEDIATO,ofs,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
			  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF *)&V->label,ofs);
#endif
				  break;
				case CLASSE_AUTO:
		      i=MAKEPTROFS(V->label+ofs);
				  if(i>0) {
#if ARCHI
						BS="ADD";
#elif Z80 || I8086
						BS="add";
#elif MC68000
						BS="addi.l";
#elif MICROCHIP
						BS="ADDLW";
#endif                    
						}
			  	else {
#if ARCHI
						BS="SUB";
					  i=abs(i);
#elif Z80 || I8086
						BS="sub";
					  i=abs(i);
#elif MC68000
						BS="subi.l";
#elif MICROCHIP
						BS="ADDLW";
#endif   
						}
#if ARCHI
				  sprintf(MyBuf,"#%d",i);
				  PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER,MyBuf);
#elif Z80
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
					u.mode=OPDEF_MODE_REGISTRO_LOW8;
					u.s.n=Regs->D;
					u.ofs=0;
    			if(i)
		  		  Op2A(BS,&u,i,0);
#elif I8086
					if(CPU86>=3) {
						}
					/*MemoryModel*/
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D,OPDEF_MODE_FRAMEPOINTER,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF *)Regs->D,i);
#elif MC68000
					if(asPtr)
			  		PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF *)0,i,OPDEF_MODE_REGISTRO32,Regs->P);
					else {
			  		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_REGISTRO32,Regs->D);
					  PROCOper(LINE_TYPE_ISTRUZIONE,BS,OPDEF_MODE_IMMEDIATO,abs(i),OPDEF_MODE_REGISTRO32,asPtr ? Regs->P : Regs->D);
						}
#elif MICROCHIP
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
					u.mode=OPDEF_MODE_REGISTRO_LOW8;
					u.s.n=Regs->D;
					u.ofs=0;
    			if(i)
		  		  Op2A(BS,&u,i,0);
#endif
				  break;
				case CLASSE_REGISTER:
				  PROCError(2103);
				  break;
				}  
			if(OutSource) {
		//    i=_tcslen(LastOut->s)+_tcslen(V->name)+25;
		//    PROCOut(NULL,"\t\t\t\t; ",V->name,NULL,NULL);	
		//    LastOut=(struct LINE *)_frealloc(LastOut,i);
		//    LastOut->prev->next=LastOut;
				_tcscpy(LastOut->rem,"addr. ");
				_tcscat(LastOut->rem,V->name);
				}
			break;	
		case VALUE_IS_D0:		// questo per cose tipo &pippo[5]
// no direi		case VALUE_IS_EXPR:
#if ARCHI
#elif Z80
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->P);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
				if(CPU86>=3) {
					}
					/*MemoryModel*/
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO,Regs->P);
#elif MC68000
	  		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
#endif
			break;	
		}
	
  return 0;
  }                   
 
