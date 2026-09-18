#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>


int Ccc::PROCUsaFun(struct VARS *V,bool tosave1,bool tosave2) {    // r per salvare reg, r1 per copiare in altro reg.
  int I,T=0;
	int16_t i,j;
  int totParm,prParm=0;
  int *parmPtr;
	O_TYPE parmType;
	O_SIZE parmSize;
  char Clabel[32],MyBuf[128];
  struct LINE *t,*t1;
  struct VARS RPtr;
  struct OPERAND R;
  union STR_LONG RCost;
	bool parmProto;
#if MC68000 || GD24032
	char pushString2[16]={0},movString2[16]={0};
#endif
	struct OP_DEF op[5];		// per ora 5 diciamo
			  
  if(debug)
    myLog->print(0,"USAFUN %x\n",V);           
		   
  I=0;
  ZeroMemory(&R,sizeof(struct OPERAND));
  ZeroMemory(&RPtr,sizeof(struct VARS));
  ZeroMemory(&RCost,sizeof(union STR_LONG));
	*Clabel=0;
  
#if ARCHI		
  FuncCalled=TRUE;
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
#endif    

	if(!_tcsncmp(V->name,"_builtin_",9)) {		// in effetti andrebbe beccato prima, e non inserita nelle VAR...

// e poi sotto, naturalmente
#if ARCHI		

#elif Z80
		if(!_tcscmp(V->name+9,"di") || !_tcscmp(V->name+9,"ei")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
		else if(!_tcscmp(V->name+9,"nop")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
//block_copy  e search
		//__builtin_inp(port) / __builtin_outp(port, val)
#elif I8086 
		if(!_tcscmp(V->name+9,"di") || !_tcscmp(V->name+9,"ei")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
		else if(!_tcscmp(V->name+9,"nop")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
//__builtin_into
#elif MC68000 
		if(!_tcscmp(V->name+9,"swap")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=2;// creo lista parametri
			*((int*)V->parm+1)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+2)=2;
			*((int*)V->parm+3)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+4)=2;
			V->size=0;
			}
		else if(!_tcscmp(V->name+9,"di") || !_tcscmp(V->name+9,"ei")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
		else if(!_tcscmp(V->name+9,"nop")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
		//__builtin_abcd(a, b) e __builtin_sbcd(a, b)
#elif GD24032 
		if(!_tcscmp(V->name+9,"nand") || !_tcscmp(V->name+9,"nor")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=2;// creo lista parametri
			*((int*)V->parm+1)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+2)=4;
			*((int*)V->parm+3)=VARTYPE_PLAIN_INT;
			*((int*)V->parm+4)=4;
			V->size=4;
			}
		else if(!_tcscmp(V->name+9,"mas") || !_tcscmp(V->name+9,"mss")) {		// e poi VMA ecc
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
//			V->classe = CLASSE_BUILTIN;		//fare...
			*(int*)V->parm=3;// creo lista parametri
			*((int*)V->parm+1)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+2)=4;
			*((int*)V->parm+3)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+4)=4;
			*((int*)V->parm+5)=VARTYPE_PLAIN_INT;
			*((int*)V->parm+6)=4;
			V->size=4;
			}
		else if(!_tcscmp(V->name+9,"vma")) {		// e poi VMA ecc
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
//			V->classe = CLASSE_BUILTIN;		//fare...
			*(int*)V->parm=4;// creo lista parametri
			*((int*)V->parm+1)=VARTYPE_ARRAY | VARTYPE_POINTER | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+2)=4;
			*((int*)V->parm+3)=VARTYPE_PLAIN_INT | VARTYPE_NOIMMEDIATE;
			*((int*)V->parm+4)=4;
			*((int*)V->parm+5)=VARTYPE_PLAIN_INT;
			*((int*)V->parm+6)=4;
			*((int*)V->parm+7)=VARTYPE_PLAIN_INT;
			*((int*)V->parm+8)=4;
			V->size=4;
			}
		else if(!_tcscmp(V->name+9,"getflags")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=4;
			}
		else if(!_tcscmp(V->name+9,"di") || !_tcscmp(V->name+9,"ei")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
		// fare anche __builtin_get_and_disable_interrupts() e __builtin_restore_interrupts(status)
		else if(!_tcscmp(V->name+9,"nop")) {
			V->modif |= (FUNC_MODIF_INLINE | FUNC_MODIF_BUILTIN);
			*(int*)V->parm=0;
			V->size=0;
			}
#elif MICROCHIP
#endif    

		}

	if(!(V->type & VARTYPE_FUNC_POINTER)) {
		struct VARS *v;
		V->type |= VARTYPE_FUNC_USED;         // funzione usata almeno una volta  
		// siccome ora questa V è una copia, vado a settare il flag di quella vera SE C'ERA PROTOTIPO! (credo valga per inline ? 2026
		v=FNCercaVar(V->name,0);
		if(v)
			v->type |= VARTYPE_FUNC_USED;
		}

	if(V->modif & FUNC_MODIF_INLINE) {
		if(!_tcscmp(CurrFunc->name,V->name))
			PROCError(4710,"function has recursion");
		// ovvero si potrebbe convertire a una call normale
		}

  if(tosave1)
    Regs->Save();
	parmPtr=(int*)V->parm;
	if(parmPtr) {
		totParm=parmPtr[0];		// il primo int è il #parm da prototipo
		parmPtr++;
		if(debug)
		  myLog->print(0,"La fun %s ha %d parm\n",V->name,totParm);
		}
	else {
		totParm=-1;
	  }	

	op[0].mode=OPDEF_MODE_REGISTRO32;		// in caso non ci fosse nulla, per inline e builtin!
	op[0].s.n=0;
  if(*FNLA(MyBuf) != ')') {
		if(!PascalCall && !(V->modif & FUNC_MODIF_PASCAL)) {
		  t1=LastOut;
		  t=0;
		  }
		do {
		  R.Q=0;
		  R.size=0;
		  R.type=0l;
			R.var=&RPtr;
			R.cost=&RCost;
//		  isRValue=isPtrUsed=0;
// no!			Regs->Reset();
		  i=0;
		  FNRev(14,&i,Clabel,&R);

      if(totParm != -1 && prParm<totParm) {
				parmType=parmPtr[0];
				parmSize=parmPtr[1];
				parmProto=TRUE;
				}
			else {
				parmType=R.type;
				parmSize=R.size;
				parmProto=FALSE;
				}

		  i=FNGetMemSize(parmType,parmSize,0/*dim*/,1);
		  j=FNGetMemSize(R.type,R.size,0/*dim*/,1);

#if MC68000
			_tcscpy(pushString2,pushString);
			_tcscpy(movString2,movString);		// in pratica qua son la stessa cosa :)
			switch(i) {
				case 1:		// il 68000 mantiene cmq SP pari anche se pusho un byte (il secondo esce 0, credo
					if(!parmProto) 	// se non c'è un prototipo char, estendo (specie per printf
						goto forced_size2;
					_tcscat(pushString2,
						parmType & VARTYPE_POINTER ? ((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM ? ".l" : ".w") : ".b");
					_tcscat(movString2,
						parmType & VARTYPE_POINTER ? ((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM ? ".l" : ".w") : ".b");
					break;
				case 2:
forced_size2:
					_tcscat(pushString2,
						parmType & VARTYPE_POINTER ? ((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM ? ".l" : ".w") : ".w");
					_tcscat(movString2,
						parmType & VARTYPE_POINTER ? ((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM ? ".l" : ".w") : ".w");
					break;
				case 4:
					_tcscat(pushString2,".l");
					_tcscat(movString2,".l");
					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#endif    
#if GD24032
			_tcscpy(pushString2,pushString);
			_tcscpy(movString2,movString);		// in pratica qua son la stessa cosa :)
			switch(i) {
				case 1:		// 
					if(!parmProto) { 	// se non c'è un prototipo char, estendo (specie per printf - sotto andrebbe fatto cast
						goto forced_size4;
						}
					_tcscat(pushString2,parmType & VARTYPE_POINTER ? ".d" : ".b");
					_tcscat(movString2,parmType & VARTYPE_POINTER ? ".d" : ".b");
					break;
				case 2:
					if(!parmProto) { 	// se non c'è un prototipo char, estendo (specie per printf - sotto andrebbe fatto cast
						goto forced_size4;
						}
					_tcscat(pushString2,parmType & VARTYPE_POINTER ? ".d" : ".w");
					_tcscat(movString2,parmType & VARTYPE_POINTER ? ".d" : ".w");
					break;
				case 4:
forced_size4:
					_tcscat(pushString2,".d");
					_tcscat(movString2,".d");
					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#endif    

		if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {		// se fastcall, se pascal si potrebbero usare i registri al rovescio...
#if ARCHI
			I+=STACK_ITEM_SIZE;
#elif Z80 
			switch(i) {
				case 1:
					I+=parmType & VARTYPE_POINTER /*puntatore*/ ? getPtrSize(parmType) : STACK_ITEM_SIZE;
					break;
				case 2:
					I+=STACK_ITEM_SIZE;
					break;
				case 4:
					I+=2*STACK_ITEM_SIZE;
					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#elif MC68000 || I8086
			switch(i) {
				case 1:
				case 2:
					I += parmType & VARTYPE_POINTER /*puntatore*/ ? getPtrSize(parmType)
						: STACK_ITEM_SIZE;		//
					break;
				case 4:
					I += 2*STACK_ITEM_SIZE;
					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#elif GD24032
			switch(i) {
				case 1:
				case 2:
				case 4:
					I += parmType & VARTYPE_POINTER ? getPtrSize(parmType) : STACK_ITEM_SIZE;		//
					break;
				case 8:			// fare qua
					PROCError(4099);

					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#elif MICROCHIP
			switch(i) {
				case 1:
					I+=parmType & VARTYPE_POINTER /*puntatore*/ ? getPtrSize(parmType) : STACK_ITEM_SIZE;
					break;
				case 2:
					I+=2*STACK_ITEM_SIZE;
					break;
				case 4:
					I+=4*STACK_ITEM_SIZE;
					break;
				case 0:
					PROCError(4099);
					break;
				default:
					PROCError(1002,"dim. parametro troppo grande (array/struct)");
					break;
				}   
#endif
			}		// fastcall
		else {

#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
#elif GD24032
#elif MICROCHIP
#endif
			}






		if(R.Q==VALUE_IS_D0) {
			if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI
				// CREDO VADAN TUTTE FATTE COME 68000!! 2025
				PROCReadD0(R.var,parmType,parmSize,/*0,*/0,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,10);
				PROCReadD0(R.var,parmType,parmSize,/*0,*/1,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,10);
#elif Z80 
				PROCReadD0(R.var,parmType,parmSize,/*0,*/0,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,10);
				PROCReadD0(R.var,parmType,parmSize,/*0,*/1,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,10);
#elif I8086
				PROCReadD0(R.var,parmType,parmSize,/*0,*/0,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,10);
				PROCReadD0(R.var,parmType,parmSize,/*0,*/1,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,10);
#elif MC68000
/*mah non serve, v,sotto
				PROCReadD0(R.var,parmType,parmSize,0,0,0);
				switch(i) {
					case 1:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 2:                    
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
			  	case 4:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}*/
#elif GD24032

#elif MICROCHIP
				PROCReadD0(R.var,parmType,parmSize,/*0,*/0,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,10);
				PROCReadD0(R.var,parmType,parmSize,/*0,*/1,0,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,10);
#endif
				}		// fastcall
			else {
				op[prParm].mode=OPDEF_MODE_REGISTRO32;
				op[prParm].s.n=prParm;

				if(!(V->modif & FUNC_MODIF_BUILTIN)) {

#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
				PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,prParm);
#elif GD24032
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
#endif
					}
				}
			}
		else if(R.Q==VALUE_IS_EXPR || R.Q==VALUE_IS_EXPR_FUNC) {			// se è risultato di un'espressione, pare
			if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,10);
#elif Z80 
	  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,10);
#elif I8086
				switch(i) {		// VERIFICARE!
					case 1:			// minimo 16bit anche qua? sì pare di sì
						if(!parmProto /*&& parmSize != 1*/) {	// se non c'è un prototipo char, estendo
							if(parmType & VARTYPE_UNSIGNED)
								PROCOper(LINE_TYPE_ISTRUZIONE,"and",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_REGISTRO16,Regs->D);
							}
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						break;
					case 2:                    
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						break;
			  	case 4:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
#elif MC68000
				switch(i) {
					case 1:			// DEVO castare a 16bit! per stack dispari AH NO NON SERVE :) PERO' serve cmq, v. 
						if(!parmProto /*&& parmSize != 1*/) {	// se non c'è un prototipo char, estendo
							if(parmType & VARTYPE_UNSIGNED)
								PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
							}
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 2:                    
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
			  	case 4:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
#elif GD24032
				switch(i) {
					case 1:			// 
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D);
						break;
					case 2:                    
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D);
						break;
			  	case 4:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D);
						break;
			  	case 8:
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D);
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D+1);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
#elif MICROCHIP
#endif
				}		// fastcall
			else {
				op[prParm].mode=OPDEF_MODE_REGISTRO32;
				op[prParm].s.n=prParm;

				if(!(V->modif & FUNC_MODIF_BUILTIN)) {

#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
				PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,prParm);
#elif GD24032
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
#endif
					}
				}
			}		// VALUE_IS_EXPR
	  else if(R.Q==VALUE_IS_VARIABILE) {
			if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
/*				if(R.var->classe==CLASSE_REGISTER) {
#if ARCHI
				  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER,"!,{",Regs[R.var],"}");
#elif Z80 || I8086
			  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(R.var->label));   // manca il cast
#elif MC68000
			  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,MAKEPTRREG(R.var->label),OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);   // manca il cast
#elif GD24032
#elif MICROCHIP
					if(CPUPIC<2) {
			  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(R.var->label));   // manca il cast
						}
					else {
			  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(R.var->label));   // manca il cast
						}
#endif
			  	goto L19440;
			  	}
				else BAH DIREI CAZZATA antica :) ci sono i cast, sign-extension ecc... sempre */{
#if ARCHI || Z80 
	        if(totParm != -1 && prParm<totParm)
					  ReadVar(R.var,parmType,parmSize,0,0);
					else  
					  ReadVar(R.var,VARTYPE_PLAIN_INT,0,0,0);
// FARE COME 68000!
//					if(R.size==1)
//						PROCCast(0,2,R.type,R.size);

#elif I8086
//  				if(CPU86<2)             // push word ptr [#] è ok anche su 8086
//	  			  ReadVar(R.var,VARTYPE_PLAIN_INT);

					switch(R.var->classe) {	// questo codice e' copiato pari-pari da ReadVar per l'8086, sostituendo PUSH a MOV e R.var a V
																	// magari unire le due cose!
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,0,
										OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
									PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA);
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,0);
									break;
								case 2:                    
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
									break;
			  				case 4:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,2);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_AUTO:
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,0,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
									PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA);
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,0);
									break;
								case 2:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
									break;
			  				case 4:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label)+1);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_REGISTER:
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO8,0,
										OPDEF_MODE_REGISTRO8,MAKEPTRREG(R.var->label));
									PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA);
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,0);
									break;
								case 2:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D,
										OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
									break;
								case 4:
									if(CPU86<3)
										PROCError(4099);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
			  				}
							break;
						}
						
			  	goto L19440;
#elif MC68000
					switch(R.var->classe) {	// (questo codice era copiato pari-pari da ReadVar per l'8086, sostituendo PUSH a MOV e R.var a V
																	// magari unire le due cose!
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							switch(i) {
								case 1:			// DEVO castare a 16bit! per stack dispari AH NO NON SERVE :)
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0,
										OPDEF_MODE_REGISTRO16,Regs->D);
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0,
											OPDEF_MODE_REGISTRO16,Regs->D);
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,R.var->type & VARTYPE_ARRAY ? OPDEF_MODE_VARIABILE_INDIRETTO : OPDEF_MODE_VARIABILE,
										// ev memorymodel...
											(union SUB_OP_DEF*)&R.var->label,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
			  				case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,Regs->D);
										if(j==1) {
											PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0,
												OPDEF_MODE_REGISTRO8,Regs->D);
											if(!(R.type & VARTYPE_UNSIGNED))
												PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO32,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0,
												OPDEF_MODE_REGISTRO32,Regs->D);
											}
										if(!(R.type & VARTYPE_UNSIGNED))
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,R.var->type & VARTYPE_ARRAY ? OPDEF_MODE_VARIABILE_INDIRETTO : OPDEF_MODE_VARIABILE,
											(union SUB_OP_DEF*)&R.var->label,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_AUTO:
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),OPDEF_MODE_REGISTRO16,Regs->D);
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),
											OPDEF_MODE_REGISTRO16,Regs->D);
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,Regs->D);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
										}
									else {
										if(R.var->type & VARTYPE_ARRAY) {
			  							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_REGISTRO32,Regs->D);
											PROCOper(LINE_TYPE_ISTRUZIONE,"addi.w",OPDEF_MODE_IMMEDIATO16,MAKEPTROFS(R.var->label),OPDEF_MODE_REGISTRO32,Regs->D);
											// memorymodel
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,
												0,MAKEPTROFS(R.var->label),OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
											}
										}
									break;
			  				case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,Regs->D);
										if(j==1) {
											PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),
												OPDEF_MODE_REGISTRO32,Regs->D);
											if(!(R.type & VARTYPE_UNSIGNED))
												PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO32,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),
												OPDEF_MODE_REGISTRO16,Regs->D);
											}
										if(!(R.type & VARTYPE_UNSIGNED))
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
										}
									else {
										if(R.var->type & VARTYPE_ARRAY) {
			  							PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_REGISTRO32,Regs->D);
											PROCOper(LINE_TYPE_ISTRUZIONE,"addi.l",OPDEF_MODE_IMMEDIATO32,MAKEPTROFS(R.var->label),OPDEF_MODE_REGISTRO32,Regs->D);
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,
												0,MAKEPTROFS(R.var->label),OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
											}
										}
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_REGISTER:
							switch(i) {
								case 1:
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,
												OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,MAKEPTRREG(R.var->label),
										OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,
												OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label),
										OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(j==1) {
											if(!(R.type & VARTYPE_UNSIGNED)) {
												PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
												PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
												}
											else
												PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO16,0x000000ff,		// risparmio un'istruzione...  ma verificare tempi e spazio!
													OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
											}
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label),
											OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label),
											OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
			  				}
							break;
						}
						
			  	goto L19440;
#elif GD24032
					switch(R.var->classe) {	// (questo codice era copiato pari-pari da ReadVar per l'8086, sostituendo PUSH a MOV e R.var a V
																	// magari unire le due cose!
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							switch(i) {
								case 1:			// 
									PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D);
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D);
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,R.var->type & VARTYPE_ARRAY ? OPDEF_MODE_VARIABILE_INDIRETTO : OPDEF_MODE_VARIABILE,
										// ev memorymodel...
											(union SUB_OP_DEF*)&R.var->label,0);
									break;
			  				case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"CLR.d",OPDEF_MODE_REGISTRO32,Regs->D);
										if(j==1) {
											PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
											if(!(R.type & VARTYPE_UNSIGNED))
												PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO32,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0);
											}
										if(!(R.type & VARTYPE_UNSIGNED))
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.d",OPDEF_MODE_REGISTRO32,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,R.var->type & VARTYPE_ARRAY ? OPDEF_MODE_VARIABILE_INDIRETTO : OPDEF_MODE_VARIABILE,
											(union SUB_OP_DEF*)&R.var->label,0);
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_AUTO:
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,Regs->D);
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,Regs->D);
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),
											OPDEF_MODE_REGISTRO16,Regs->D);
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D);
										}
									else {
										if(R.var->type & VARTYPE_ARRAY) {
			  							PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER,0);
											PROCOper(LINE_TYPE_ISTRUZIONE,"ADD.w",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO16,MAKEPTROFS(R.var->label));
											// memorymodel
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,
												0,MAKEPTROFS(R.var->label));
											}
										}
									break;
			  				case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"CLR.d",OPDEF_MODE_REGISTRO32,Regs->D);
										if(j==1) {
											PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
											if(!(R.type & VARTYPE_UNSIGNED))
												PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO32,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
											}
										if(!(R.type & VARTYPE_UNSIGNED))
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.l",OPDEF_MODE_REGISTRO32,Regs->D);
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D);
										}
									else {
										if(R.var->type & VARTYPE_ARRAY) {
			  							PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_FRAMEPOINTER,0);
											PROCOper(LINE_TYPE_ISTRUZIONE,"ADD.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,MAKEPTROFS(R.var->label));
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->D);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,
												0,MAKEPTROFS(R.var->label));
											}
										}
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							break;
						case CLASSE_REGISTER:
							switch(i) {
								case 1:
									if(!parmProto) {		// se non c'è un prototipo char, estendo
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label),OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO8,MAKEPTRREG(R.var->label));
									break;
								case 2:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(R.type & VARTYPE_UNSIGNED)
											PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label),OPDEF_MODE_IMMEDIATO16,0x00ff);
										else
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
										}
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label));
									break;
								case 4:
									if(j<i)	{	// solo se la var è + piccola del parm...
										if(j==1) {
											if(!(R.type & VARTYPE_UNSIGNED)) {
												PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
												PROCOper(LINE_TYPE_ISTRUZIONE,"SE.d",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
												}
											else
												PROCOper(LINE_TYPE_ISTRUZIONE,"AND.d",		// risparmio un'istruzione...  ma verificare tempi e spazio!
													OPDEF_MODE_REGISTRO16,MAKEPTRREG(R.var->label),OPDEF_MODE_IMMEDIATO16,0x000000ff);
											}
										else {
											PROCOper(LINE_TYPE_ISTRUZIONE,"SE.l",OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
											}
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
										}
									else
										PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
									break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
			  				}
							break;
						}
						
			  	goto L19440;
#elif MICROCHIP
					switch(i) {
			  		case 4:
							ReadVar(R.var,parmType,parmSize,0,3,0);
//				  			PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
							ReadVar(R.var,parmType,parmSize,0,2,0);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
						case 2:
							ReadVar(R.var,parmType,parmSize,0,1,0);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
						case 1:
							ReadVar(R.var,parmType,parmSize,0,0,0);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
							break;
						case 0:
							PROCError(4099);
							break;
						default:
							PROCError(1002,"dim. parametro troppo grande (array/struct)");
							break;
						}
#endif
			  	}
				}		// fastcall
		else {

			if(!(V->modif & FUNC_MODIF_BUILTIN)) {

#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
				switch(R.var->classe) {
					case CLASSE_EXTERN:
					case CLASSE_GLOBAL:
					case CLASSE_STATIC:
						if(MemoryModel & MEMORY_MODEL_RELATIVE)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,
								OPDEF_MODE_ABSPOINTER_INDIRETTO,(union SUB_OP_DEF*)&R.var->label,0,OPDEF_MODE_REGISTRO32,prParm);
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&R.var->label,0,
								OPDEF_MODE_REGISTRO32,prParm);
						break;
					case CLASSE_AUTO:
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label),
							OPDEF_MODE_REGISTRO32,prParm);
						break;
					case CLASSE_REGISTER:
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label),
							OPDEF_MODE_REGISTRO32,prParm);
						break;
					}
			  goto L19440;
#elif GD24032
				switch(R.var->classe) {
					case CLASSE_EXTERN:
					case CLASSE_GLOBAL:
					case CLASSE_STATIC:
						if(MemoryModel & MEMORY_MODEL_RELATIVE)
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,
								OPDEF_MODE_ABSPOINTER_INDIRETTO,(union SUB_OP_DEF*)&R.var->label,0);
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,
								OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)&R.var->label,0);
						break;
					case CLASSE_AUTO:
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(R.var->label));
						break;
					case CLASSE_REGISTER:
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_REGISTRO32,MAKEPTRREG(R.var->label));
						break;
					}
			  goto L19440;
#elif MICROCHIP
#endif
				}
				}
			}		// value_is_variable
		else if(R.Q & VALUE_IS_CONDITION) {
			if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI || Z80

#elif I8086

#elif MC68000
				PROCAssignCond(&R.Q,&R.type,&R.size,NULL);
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D,
					OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
#elif GD24032
				PROCAssignCond(&R.Q,&R.type,&R.size,NULL);
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP

#endif
				}		// fastcall
			else {

				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
#if ARCHI

#elif Z80 

#elif I8086

#elif MC68000
				PROCAssignCond(&R.Q,&R.type,&R.size,NULL);
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D,
					OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D);
#elif GD24032
					PROCAssignCond(&R.Q,&R.type,&R.size,NULL);
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP

#endif
					}
				}
			}		// if VALUE_IS_CONDITION
		else if(R.Q & VALUE_IS_COSTANTE) {
			if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI || Z80
			  PROCUseCost(R.Q,parmType /*R.type*/,parmSize /*R.size*/,R.cost,FALSE);
#elif I8086
				if(CPU86<1) {
				  PROCUseCost(R.Q,parmType,parmSize,R.cost,FALSE);
					}
				else {
					// questo codice e' copiato pari-pari da UseCost per l'8086, sostituendo PUSH a MOV
					// magari unire le due cose!
					if(T & (VARTYPE_FLOAT | VARTYPE_IS_POINTER)) { 
						if(R.Q == 9) {
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)RCost.s,0);
			//	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,"OFFSET DGROUP:",C->s,NULL);
							}
						else {
	  					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,RCost.l);
							}
						}
					else {
						if(R.Q==VALUE_IS_COSTANTE) {
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO8,RCost.l);
									break;
								case 2:
								case 4:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost.l));
									if(i==4) {
		  							PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,HIWORD(RCost.l));
		  							}
	  							break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							}
						else {
							switch(i) {
								case 1:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)RCost.s,0);
			//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->DSl,"OFFSET DGROUP:",C->s,NULL);
									break;
								case 2:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)RCost.s,0);
			//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"OFFSET DGROUP:",C->s,NULL);
									break;
								case 4:
									PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)RCost.s,0);
			//		        PROCOper(LINE_TYPE_ISTRUZIONE,"lds",Regs->D,C->s);
	  							break;
								case 0:
									PROCError(4099);
									break;
								default:
									PROCError(1002,"dim. parametro troppo grande (array/struct)");
									break;
								}
							}
						}
			  	goto L19440;
					}
#elif MC68000
// non serve, v.sotto			  PROCUseCost(R.Q,parmType,parmSize,R.cost,FALSE);
#elif GD24032
#elif MICROCHIP
				switch(i) {
			  	case 4:
						PROCUseCost(R.Q,parmType /*R.type*/,parmSize /*R.size*/,R.cost,FALSE,3);		// FINIRE
			  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
						PROCUseCost(R.Q,parmType /*R.type*/,parmSize /*R.size*/,R.cost,FALSE,2);
			  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
					case 2:
						PROCUseCost(R.Q,parmType /*R.type*/,parmSize /*R.size*/,R.cost,FALSE,1);
			  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
					case 1:
						PROCUseCost(R.Q,parmType /*R.type*/,parmSize /*R.size*/,R.cost,FALSE,0);
			  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,10);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
#endif
				}		// fastcall
			else {
				op[prParm].mode=OPDEF_MODE_IMMEDIATO32;
				op[prParm].s.n=RCost.l;

				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
//non serve					PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_IMMEDIATO32,R.cost->l,OPDEF_MODE_REGISTRO32,prParm);
#elif GD24032
#elif MICROCHIP
#endif
					}
				}
			}		// if COSTANTE
		else {
      if(totParm != -1 && prParm<totParm)
        PROCCast(parmType,parmSize,&R.type,&R.size,-1);
      }
		
			




			// potrebbero SERVIRE i cast delle costanti, byte ecc...?
		if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI
			sprintf(MyBuf,"R%u",Regs->D);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80
		  if(i==4) {
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
  		  }
  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
		  if(R.Q == VALUE_IS_COSTANTE) {
				switch(i) {
					case 1:
						if(CPU86<1)
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						else
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(R.cost->l)));
						break;
					case 2:
						if(CPU86<1)
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						else
			  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,LOWORD(R.cost->l));
						break;
					case 4:
						if(CPU86<1) {
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D+1);
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
							}
						else {
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,HIWORD(R.cost->l));
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_IMMEDIATO16,LOWORD(R.cost->l));
							}
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
		  else if(R.Q == VALUE_IS_VARIABILE) {
// già fatto sopra		  			PROCOper(LINE_TYPE_ISTRUZIONE,"culooo"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				}
			else if(R.Q == VALUE_IS_COSTANTEPLUS) {
				switch(i) {
					case 1:
						if(CPU86<1)
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						else
			  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 2:
						if(CPU86<1)
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						else
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 4:
						if(CPU86<1)
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,Regs->D);
						else
			  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
			else if(R.Q == VALUE_IS_D0) {
	 			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
				}
			else if(R.Q == VALUE_IS_PTR) {
	 			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->P);	// memorymodel??
				}
#elif MC68000
//  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
		  if(R.Q == VALUE_IS_COSTANTE) {
				switch(i) {
					case 1:				// DEVO castare a 16bit! per stack dispari AH NO NON SERVE :)
						if(!LOBYTE(LOWORD(R.cost->l)))
		  				PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						else
		  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(R.cost->l)),
								OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 2:
						if(!LOWORD(R.cost->l))
			  			PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						else
			  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO16,LOWORD(R.cost->l),
								OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 4:
						if(!R.cost->l)
			  			PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						else
			  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO32,R.cost->l,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
		  else if(R.Q == VALUE_IS_VARIABILE) {
// già fatto sopra		  			PROCOper(LINE_TYPE_ISTRUZIONE,"culooo"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				}
			else if(R.Q == VALUE_IS_COSTANTEPLUS) {
				switch(i) {
					case 1:		// ha senso questo? boh ok
		  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.b #"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 2:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.w #"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 4:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
			else if(R.Q == VALUE_IS_D0) {
  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				}
			else if(R.Q == VALUE_IS_PTR) {
  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->P,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);		// memorymodel??
				}
#elif GD24032
//  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
		  if(R.Q == VALUE_IS_COSTANTE) {
				switch(i) {
					case 1:				// DEVO castare a 16bit! per stack dispari AH NO NON SERVE :)
	  				PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(R.cost->l)));
						break;
					case 2:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO16,LOWORD(R.cost->l));
						break;
					case 4:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_IMMEDIATO32,R.cost->l);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
		  else if(R.Q == VALUE_IS_VARIABILE) {
// già fatto sopra		  			PROCOper(LINE_TYPE_ISTRUZIONE,"culooo"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				}
			else if(R.Q == VALUE_IS_COSTANTEPLUS) {
				switch(i) {
					case 1:		// ha senso questo? boh ok
		  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 2:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 4:
		  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
						break;
					case 0:
						PROCError(4099);
						break;
					default:
						PROCError(1002,"dim. parametro troppo grande (array/struct)");
						break;
					}
				}
			else if(R.Q == VALUE_IS_D0) {
  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
				}
			else if(R.Q == VALUE_IS_PTR) {
  			PROCOper(LINE_TYPE_ISTRUZIONE,pushString2,OPDEF_MODE_REGISTRO32,Regs->P);
				}
#elif MICROCHIP
//		  if(i==4) {
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
//	  	  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVWF",OPDEF_MODE_REGISTRO_HIGH8,10 /*Regs->D+1*/);
//	  	  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVWF",OPDEF_MODE_REGISTRO_HIGH8,10 /*Regs->D+1*/);
//	  	  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVWF",OPDEF_MODE_REGISTRO_HIGH8,10 /*Regs->D+1*/);
//  		  I+= 3 /*STACK_ITEM_SIZE*/ ;
//  		  }
//		  if(i==2) {
//		  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
//	  	  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVWF",OPDEF_MODE_REGISTRO_HIGH8,10 /*Regs->D+1*/);
//  		  I+= 1 /*STACK_ITEM_SIZE*/ ;
//  		  }
//  	  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVWF",OPDEF_MODE_REGISTRO_HIGH8,10 /*Regs->D*/);
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,14,OPDEF_MODE_REGISTRO,10);
#endif
				}		// fastcall
			else {


#if ARCHI
#elif Z80 
#elif I8086
#elif MC68000
		  if(R.Q == VALUE_IS_COSTANTE) {
				// ottimizzare ev. e gestire <8 <255 ecc!
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
  				PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_IMMEDIATO32,R.cost->l,OPDEF_MODE_REGISTRO32,prParm);
					}
				else {
					op[prParm].mode=OPDEF_MODE_IMMEDIATO32;
					op[prParm].s.n=RCost.l;
					}
				}
		  else if(R.Q == VALUE_IS_VARIABILE) {
// già fatto sopra		  			PROCOper(LINE_TYPE_ISTRUZIONE,"culooo"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
				}
			else if(R.Q == VALUE_IS_COSTANTEPLUS) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_REGISTRO32,prParm);
					}
				else {
					op[prParm].mode=OPDEF_MODE_COSTANTE;
					_tcscpy(op[prParm].s.label,R.cost->s);
					}
				}
			else if(R.Q == VALUE_IS_D0 || R.Q == VALUE_IS_PTR) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
	  			PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,OPDEF_MODE_REGISTRO32,prParm);
					}
				else {
					op[prParm].mode=OPDEF_MODE_IMMEDIATO32;
					op[prParm].s.n=Regs->P;
					}
				}
#elif GD24032

			// gestire VARTYPE_NOIMMEDIATE !! copiare in registro e usare registro

		  if(R.Q == VALUE_IS_COSTANTE) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
  				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_IMMEDIATO32,R.cost->l);
					}
				else {
					if(parmType & VARTYPE_NOIMMEDIATE) {
	  				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_IMMEDIATO32,R.cost->l);
						// unire con altre costanti, saltare là
						op[prParm].mode=OPDEF_MODE_REGISTRO32;
						op[prParm].s.n=prParm;
						}
					else {
						op[prParm].mode=OPDEF_MODE_IMMEDIATO32;
						op[prParm].s.n=RCost.l;
						}
					}
				}
		  else if(R.Q == VALUE_IS_VARIABILE) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
// già fatto sopra		  			PROCOper(LINE_TYPE_ISTRUZIONE,"culooo"/*pushString2*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
					}
				else {
					switch(R.var->classe) {
						case CLASSE_EXTERN:
						case CLASSE_GLOBAL:
						case CLASSE_STATIC:
							if(MemoryModel & MEMORY_MODEL_RELATIVE) {
								op[prParm].mode=OPDEF_MODE_ABSPOINTER_INDIRETTO;
								_tcscpy(op[prParm].s.label,R.var->label);
								op[prParm].ofs=0;
								}
							else {
								op[prParm].mode=OPDEF_MODE_VARIABILE_INDIRETTO;
								_tcscpy(op[prParm].s.label,R.var->label);
								op[prParm].ofs=0;
								}
							break;
						case CLASSE_AUTO:
							op[prParm].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
							op[prParm].ofs=MAKEPTROFS(R.var->label);
							break;
						case CLASSE_REGISTER:
							op[prParm].mode=OPDEF_MODE_REGISTRO32;
							op[prParm].s.n=MAKEPTRREG(R.var->label);
							break;
						}
					}
				}
			else if(R.Q == VALUE_IS_COSTANTEPLUS) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
	  			PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d "/*pushString2*/,OPDEF_MODE_REGISTRO32,prParm,
						OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)R.cost->s,0);
					}
				else {
					op[prParm].mode=OPDEF_MODE_COSTANTE;
					_tcscpy(op[prParm].s.label,R.cost->s);
					}
				}
			else if(R.Q == VALUE_IS_D0 || R.Q == VALUE_IS_PTR) {
				if(!(V->modif & FUNC_MODIF_BUILTIN)) {
	  			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,prParm,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
					}
				else {
					op[prParm].mode=OPDEF_MODE_REGISTRO_INDIRETTO;
					op[prParm].s.n=Regs->P;
					op[prParm].ofs=0;
					}
				}
#elif MICROCHIP
#endif
			}

// OVVIAMENTE intercettare i valori immediati dove non sono ammessi, v. NAND ecc: gemini dice di convertire in 
// MOV R0,  e poi usare il registro R0



L19440:
  	  prParm++;
			if(parmPtr)
		    parmPtr+=2;		// mi sposto al tipo e size del prossimo parm

			if(OutSource) {
				char myBuf[32];
				wsprintf(myBuf,"parm %u",prParm);
				_tcscat(LastOut->rem,myBuf);
				}
			if(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE)) {
				if(maxRegUsed>0)
					if(t)
	 					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D+1);
/*	  	wsprintf(MyBuf,"{R%u-R%u}",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"STM.d",		//pushString
				OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);*/
				}

			if(!PascalCall && !(V->modif & FUNC_MODIF_PASCAL)) {
				if(!t)
				  t=LastOut;
				LastOut=t1;
				}
//		  I+=STACK_ITEM_SIZE;


			FNLO(MyBuf);          
		  if((*MyBuf != ',') && (*MyBuf != ')')) 
				PROCError(2059,MyBuf);

			if(parmPtr) {                              // se finisce con ... (var args)
				if(parmPtr[0] == -1)
					totParm=-1;		// ...da qui in poi do tutto buono!
				}
		  } while(*MyBuf != ')');
		if(!PascalCall && !(V->modif & FUNC_MODIF_PASCAL)) {
		  LastOut=t;
		  }
		if(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE)) {
			if(maxRegUsed>0)
	 			PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D+1);
/*	  	wsprintf(MyBuf,"{R%u-R%u}",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"LDM.d",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,		//popString
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);*/
		  }
		}
  else {
	  FNLO(MyBuf);
	  }

//	myLog->print("totparm %d, prparm %d\n",totParm,prParm);  
  if(totParm != -1 && prParm!=totParm) {
    wsprintf(MyBuf,"%s, #%u",V->name,prParm);
    PROCError(2116,MyBuf);
    }

#if ARCHI
	if(V->type & VARTYPE_FUNC_POINTER) {
		switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				break;
			case CLASSE_AUTO:
				PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(V->label));
				break;
			case CLASSE_REGISTER:
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
				break;
			}
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
		}
	else {
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,
				OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
		}
#elif Z80 
	if(!_tcsncmp(V->name,"_builtin_",9)) {		// in effetti andrebbe beccato prima, e non inserita nelle VAR...
// v sopra

		if(!_tcscmp(V->name+9,"nop")) {
			PROCOper(LINE_TYPE_CALL,"nop",OPDEF_MODE_NULLA,0);
			}
		else if(!_tcscmp(V->name+9,"di")) {
			PROCOper(LINE_TYPE_CALL,"di",OPDEF_MODE_NULLA);
			}
		else if(!_tcscmp(V->name+9,"ei")) {
			PROCOper(LINE_TYPE_CALL,"ei",OPDEF_MODE_NULLA);
			}
		}

	if(V->type & VARTYPE_POINTER) {
	  PROCOper(LINE_TYPE_ISTRUZIONE,"ld",OPDEF_MODE_REGISTRO16,Regs->P,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
	  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
		}
	else
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
#elif I8086 
	if(!_tcsncmp(V->name,"_builtin_",9)) {		// in effetti andrebbe beccato prima, e non inserita nelle VAR...
// v sopra
		if(!_tcscmp(V->name+9,"nop")) {
			PROCOper(LINE_TYPE_CALL,"nop",OPDEF_MODE_NULLA,0);
			}
		else if(!_tcscmp(V->name+9,"di")) {
			PROCOper(LINE_TYPE_CALL,"di",OPDEF_MODE_NULLA);
			}
		else if(!_tcscmp(V->name+9,"ei")) {
			PROCOper(LINE_TYPE_CALL,"ei",OPDEF_MODE_NULLA);
			}

		}

	if(V->type & VARTYPE_POINTER) {
	  PROCOper(LINE_TYPE_ISTRUZIONE,"mov",OPDEF_MODE_REGISTRO16,Regs->P,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
		}
	else
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,
				OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
#elif MC68000
	if(!_tcsncmp(V->name,"_builtin_",9)) {		// in effetti andrebbe beccato prima, e non inserita nelle VAR...
// v sopra

		if(!_tcscmp(V->name+9,"swap")) {
			PROCOper(LINE_TYPE_CALL,"swap",&op[0]);
			}
		else if(!_tcscmp(V->name+9,"di")) {
			PROCOper(LINE_TYPE_CALL,"move #$2700, sr",OPDEF_MODE_NULLA);
			}
		else if(!_tcscmp(V->name+9,"ei")) {
			PROCOper(LINE_TYPE_CALL,"andi.w #$2000, sr",OPDEF_MODE_NULLA);
			}
		else if(!_tcscmp(V->name+9,"nop")) {
			PROCOper(LINE_TYPE_CALL,"nop",OPDEF_MODE_NULLA,0);
			}
		}

	if(V->type & VARTYPE_FUNC_POINTER) {
		switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
				if(MemoryModel & MEMORY_MODEL_RELATIVE)
				  PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_ABSPOINTER_INDIRETTO,(union SUB_OP_DEF*)&V->label,0,OPDEF_MODE_REGISTRO32,Regs->P);
				else
				  PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0,OPDEF_MODE_REGISTRO32,Regs->P);
				break;
			case CLASSE_AUTO:
				PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(V->label),OPDEF_MODE_REGISTRO32,Regs->P);
				break;
			case CLASSE_REGISTER:
				PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO32,Regs->P);
				break;
			}
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
		}
	else {
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else {
			if(MemoryModel & MEMORY_MODEL_RELATIVE) {
				PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				}
			else
				PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
			}
		}
#elif GD24032
	if(!_tcsncmp(V->name,"_builtin_",9)) {		// in effetti andrebbe beccato prima, e non inserita nelle VAR...
// v sopra

		if(!_tcscmp(V->name+9,"nand")) {
			PROCOper(LINE_TYPE_CALL,"NAND",&op[0],&op[1]);
			}
		else if(!_tcscmp(V->name+9,"nor")) {
			PROCOper(LINE_TYPE_CALL,"NOR",&op[0],&op[1]);
			}
		else if(!_tcscmp(V->name+9,"mas")) {		// e poi VMA ecc
			PROCOper(LINE_TYPE_CALL,"MAS",&op[0],&op[1],&op[2]);
			}
		else if(!_tcscmp(V->name+9,"mss")) {		// 
			PROCOper(LINE_TYPE_CALL,"MAS",&op[0],&op[1],&op[2]);
			}
		else if(!_tcscmp(V->name+9,"nop")) {
			PROCOper(LINE_TYPE_CALL,"NOP",OPDEF_MODE_NULLA,0);
			}
		else if(!_tcscmp(V->name+9,"getflags")) {
			PROCOper(LINE_TYPE_CALL,"STST",&op[0]);
			}
		else if(!_tcscmp(V->name+9,"di")) {
			PROCOper(LINE_TYPE_CALL,"LDIM 0",OPDEF_MODE_NULLA);
			}
		else if(!_tcscmp(V->name+9,"ei")) {
			PROCOper(LINE_TYPE_CALL,"LDIM 31",OPDEF_MODE_NULLA);
			}

		}
	if(V->type & VARTYPE_FUNC_POINTER) {
		switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
				if(MemoryModel & MEMORY_MODEL_RELATIVE)
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_ABSPOINTER_INDIRETTO,(union SUB_OP_DEF*)&V->label,0);
				else
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)&V->label,0);
				break;
			case CLASSE_AUTO:
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,MAKEPTROFS(V->label));
				break;
			case CLASSE_REGISTER:
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
				break;
			}
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else
		  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
		}
	else {
		if(V->modif & FUNC_MODIF_INLINE)
			;
		else {
			if(MemoryModel & MEMORY_MODEL_RELATIVE) {
				wsprintf(MyBuf,"%s-$",V->label);       // ripreparo add. jump + registro
				PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? "JR" : "CALLR",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&MyBuf,0);
				}
			else
				PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
			}
		}
#elif MICROCHIP
	//CPUPIC
	if(V->modif & FUNC_MODIF_INLINE)
			;
	else
	  PROCOper(LINE_TYPE_CALL,V->attrib & FUNC_ATTRIB_NORETURN ? jmpString : callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V,0);
#endif

  if(I && !(V->attrib & FUNC_ATTRIB_NORETURN)) {
#if ARCHI
		PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_STACKPOINTER,13,OPDEF_MODE_STACKPOINTER,13,
			OPDEF_MODE_IMMEDIATO16,I);			//
#elif Z80
		if(I > 127) 
		  PROCError(2127);
		if(I<16) {                // 16 è la dimensione di ld iy,nn; add iy,sp; ld sp,iy
	  	for( ; I>0; I-=2) {
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
				}
	  	}
		else {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,I);
			PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_FRAMEPOINTER,0);
			}
#elif I8086
		if(I > 32767) 
		  PROCError(2127);
		PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO16,I);
#elif MC68000
		if(!((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM)) {
			if(I > 32767) 
			  PROCError(2127);
			if(I<=8 /*&& !V->size*/)
				PROCOper(LINE_TYPE_ISTRUZIONE,"addq.w",OPDEF_MODE_IMMEDIATO16,I,OPDEF_MODE_STACKPOINTER,0);
			else
				PROCOper(LINE_TYPE_ISTRUZIONE,"adda.w",OPDEF_MODE_IMMEDIATO16,I,OPDEF_MODE_STACKPOINTER,0);
			}
		// se non void, e condizionale, RILEGGerei D0 dopo pop/adda .. però in effetti ADDA non tocca i flag, quindi... MANCO ADDQ se su An :)
		else {
			if(I<=8 /*&& !V->size*/)
				PROCOper(LINE_TYPE_ISTRUZIONE,"addq.l",OPDEF_MODE_IMMEDIATO32,I,OPDEF_MODE_STACKPOINTER,0);
			else
				PROCOper(LINE_TYPE_ISTRUZIONE,"adda.l",OPDEF_MODE_IMMEDIATO32,I,OPDEF_MODE_STACKPOINTER,0);
			}
#elif GD24032
			PROCOper(LINE_TYPE_ISTRUZIONE,"ADD.d",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO32,I);
#elif MICROCHIP
		if(I > 127) 
		  PROCError(2127);
		if((StackLarge==0 && I<4) || (StackLarge!=0 && I<6)) {           // 4 è la dimensione di {save W} movlw n addwf fsr2,1 [movlw 0 addwfc fsr2,1] {restore W}
	  	for( ; I>0; I--) {			
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,11);
				}
	  	}
		else {
			PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_IMMEDIATO8,I);
			PROCOper(LINE_TYPE_ISTRUZIONE,"ADDWF",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO8,"F,ACCESS");
			if(StackLarge) {
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_IMMEDIATO8,0 /* I >> 8*/);
				PROCOper(LINE_TYPE_ISTRUZIONE,"ADDWFC",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO8,"F,ACCESS");
				}
			}
#endif
		}

	if(V->modif & FUNC_MODIF_INLINE) {
		struct LINE *sl=V->definition,*sl2;
		int32_t myAutoOff=AutoOff;
		if(V->attrib & FUNC_ATTRIB_NORETURN)
			PROCError(4710,"function cannot be no_return");
		CurrFunc->inlineCnt++;
		while(sl && (sl->type != LINE_TYPE_COMMENTO || _tcsnicmp(sl->rem,"----------",10))) {
			if(sl->type != LINE_TYPE_NULLA) {
				switch(sl->type) {
					struct OP_DEF od1,od2;
					case LINE_TYPE_ISTRUZIONE:
#if ARCHI
						if(!_tcscmp(sl->opcode,"RET")) {		// gestire altre CPU!
#elif Z80
						if(!_tcscmp(sl->opcode,"RET")) {		//
#elif I8086
						if(!_tcscmp(sl->opcode,"RET")) {		//
#elif MC68000
						if(!_tcscmp(sl->opcode,"rts")) {		//
#elif GD24032
						if(!_tcscmp(sl->opcode,"RET")) {		// 
#elif MICROCHIP
						if(!_tcscmp(sl->opcode,"RET")) {		//
#endif
							wsprintf(MyBuf,"ret_%s_%s_%u",V->label,CurrFunc->label,CurrFunc->inlineCnt);
							PROCOutLab(MyBuf);
							}
#if ARCHI
						else if(!_tcscmp(sl->opcode,"ENTER")) {		// gestire altre CPU!
#elif Z80
						else if(!_tcscmp(sl->opcode,"ENTER")) {		// 
#elif I8086
						else if(!_tcscmp(sl->opcode,"ENTER")) {		// 
#elif MC68000
						else if(!_tcscmp(sl->opcode,"link")) {		//
#elif GD24032
						else if(!_tcscmp(sl->opcode,"ENTER")) {		//
#elif MICROCHIP
						else if(!_tcscmp(sl->opcode,"ENTER")) {		//
#endif
							AutoOff -= sl->s2.s.n;		// mah... !
							// che poi in effetti sarebbe bello sovrapporle , le diverse funzioni inline, e quindi serve un "max" dei valori rilevati
							}
#if ARCHI
						else if(!_tcscmp(sl->opcode,"LEAVE")) {		
#elif Z80
						else if(!_tcscmp(sl->opcode,"LEAVE")) {		
#elif I8086
						else if(!_tcscmp(sl->opcode,"LEAVE")) {		
#elif MC68000
						else if(!_tcscmp(sl->opcode,"unlk")) {		
#elif GD24032
						else if(!_tcscmp(sl->opcode,"LEAVE")) {		
#elif MICROCHIP
						else if(!_tcscmp(sl->opcode,"LEAVE")) {		
#endif
							}
						else {
							od1=sl->s1;  od2=sl->s2;
							switch(sl->s1.mode & 0x7f) {
								case OPDEF_MODE_FRAMEPOINTER:
									od1.ofs += myAutoOff;
									break;
								case OPDEF_MODE_VARIABILE:
									break;
								case OPDEF_MODE_ABSPOINTER:
									break;
								case OPDEF_MODE_REGISTRI:
									break;
								default:
									break;
								}
							switch(sl->s2.mode & 0x7f) {
								case OPDEF_MODE_FRAMEPOINTER:
									od2.ofs += myAutoOff;
									break;
								case OPDEF_MODE_VARIABILE:
									break;
								case OPDEF_MODE_ABSPOINTER:
									break;
								case OPDEF_MODE_REGISTRI:
									break;
								default:
									break;
								}
							PROCOut(sl->type,sl->opcode,&od1,&od2 /*,&sl->s3*/);
							}
						break;
					case LINE_TYPE_JUMP:
					case LINE_TYPE_JUMPC:
//						if(!_tcscmp(sl->opcode,"JMP") || !_tcscmp(sl->opcode,"JR")) {		// gestire altre CPU!
//							wsprintf(MyBuf,"%s_%s_%s_%u",sl->s1.s.label,V->label,CurrFunc->label,CurrFunc->inlineCnt);
//							PROCOut(sl->type,sl->opcode,(struct OP_DEF*)MyBuf,NULL);
						od1=sl->s1;
						switch(sl->s1.mode & 0x7f) {
							case OPDEF_MODE_FRAMEPOINTER:
								od1.ofs += myAutoOff;
								break;
							case OPDEF_MODE_VARIABILE:// tendenzialmente questa non esiste!
								break;
							case OPDEF_MODE_ABSPOINTER:
								break;
							case OPDEF_MODE_REGISTRI:
								break;
							default:
								break;
							}
						if(*od1.s.label == 'R')		// gestisco "return" !
							wsprintf(od1.s.label,"ret_%s_%s_%u",V->label,CurrFunc->label,CurrFunc->inlineCnt);
						PROCOper(sl->type,sl->opcode,&od1);
//							}
						break;
					case LINE_TYPE_JUMPGOTO:
						od1.mode=sl->s1.mode;
						od1.ofs=0;
						od1.s.n=0;
//						if(!_tcscmp(sl->opcode,"JMP") || !_tcscmp(sl->opcode,"JR")) {		// gestire altre CPU!
							wsprintf(od1.s.label,"%s%s_%u",sl->s1.s.label,V->label,CurrFunc->inlineCnt,CurrFunc->label);
							PROCOper(LINE_TYPE_JUMPGOTO,sl->opcode,&od1);
//							}
						break;
					case LINE_TYPE_CALL:
//						if(!_tcscmp(sl->opcode,"CALL")) {		// gestire altre CPU!
						od1=sl->s1;
						switch(sl->s1.mode & 0x7f) {
							case OPDEF_MODE_FRAMEPOINTER:		// tendenzialmente questa non esiste!
								od1.ofs += myAutoOff;
								break;
							case OPDEF_MODE_VARIABILE:
							case OPDEF_MODE_ABSPOINTER:
//								PROCError(4710,"function has recursion");
								// beccato in UsaFun, giustamente
								break;
							case OPDEF_MODE_REGISTRI:
								break;
							default:
								break;
							}
						PROCOper(LINE_TYPE_CALL,sl->opcode,&od1);
//							}
						break;
					case LINE_TYPE_DATA_DEF:
						if(!_tcscmp(sl->s1.s.label,"PROC") || !_tcscmp(sl->s1.s.label,"ENDP")) {		// 
							}
						else if(!_tcsnicmp(sl->opcode,"PUBLIC",6)) {		// c'è il TAB...
							}
						else {
							wsprintf(MyBuf,"%s_%s_%s_%u",sl->s1.s.label,V->label,CurrFunc->label,CurrFunc->inlineCnt);
							PROCOut(sl->type,sl->opcode,&sl->s1,&sl->s2/*,&sl->s3*/);
							}
						break;
					case LINE_TYPE_LABEL:
					case LINE_TYPE_LABEL_CON_ISTRUZIONE:
						if(*sl->s1.s.label != 'R')	{	// "return" già gestito sopra
							wsprintf(MyBuf,"%s_%u",sl->s1.s.label,CurrFunc->inlineCnt);
							PROCAllocGoto(MyBuf);
							PROCOutLab(MyBuf,CurrFunc->label);
							}
						break;
					default:
						break;
					}
				}
			sl2=sl;
			sl=sl->next;
// METTERE in pulizia var!			GlobalFree(sl2);
			}

		}

  if(tosave1 || tosave2) {
#if ARCHI
    I=tosave1 ? Regs->D : Regs->D+1;
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
#elif Z80
    I=tosave1 ? Regs->D : Regs->D+1;
		i=FNGetMemSize(V,1);    // è una funz...
		if(i>2) {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,I+1,OPDEF_MODE_REGISTRO_LOW8,1);
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,I+1,OPDEF_MODE_REGISTRO_HIGH8,1);
			}
		if(i>1) {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,I,OPDEF_MODE_REGISTRO_HIGH8,0);
			}
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,I,OPDEF_MODE_REGISTRO_LOW8,0);
#elif I8086
    I=tosave1 ? Regs->D : Regs->D+1;
		if(FNGetMemSize(V,1) >2) {    // è una funz...
  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,I+1,OPDEF_MODE_REGISTRO16,1);
			}
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,I,OPDEF_MODE_REGISTRO_HIGH8,0);
#elif MC68000
//    I=tosave1 ? Regs->D : Regs->D+1;
		if(Regs->D>0)		// mah provare 2025
			PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO32,Regs->D /*I*/);
#elif GD24032
		if(Regs->D>0)		// mah provare 2026
			PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->D /*I*/,OPDEF_MODE_REGISTRO32,0);
#elif MICROCHIP
    I=tosave1 ? Regs->D : Regs->D+1;
		i=FNGetMemSize(V,1);    // è una funz...
		if(i>2) {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,I+1,OPDEF_MODE_REGISTRO_LOW8,1);
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,I+1,OPDEF_MODE_REGISTRO_HIGH8,1);
			}
		if(i>1) {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,I,OPDEF_MODE_REGISTRO_HIGH8,0);
			}
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,I,OPDEF_MODE_REGISTRO_LOW8,0);
#endif
	  }  
  if(tosave1)
    Regs->Get();
  
  return 0;
  }

