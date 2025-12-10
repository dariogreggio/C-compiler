#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"
#include "..\OpenCView2.h"

#include <stdlib.h>
#include <conio.h>


struct ERRORE Errs[]={
  1000,1,"unknown internal error - contact Cyberdyne",
  1001,1,"internal error:",
  1002,1,"unsupported:",
  1003,1,"partially ununimplemented:",
  1004,1,"unexpected EOF",
  1016,1,"#if[n]def expected an identifier",
  1017,1,"unexpected chars",
  1018,1,"unexpected #elif",
  1019,1,"unexpected #else",
  1020,1,"unexpected #endif",
  1021,1,"bad preprocessor command",
  1022,1,"expected #endif",
  1023,1,"cannot open source file",
  1024,1,"cannot open include file",
  1035,1,"expression too complex, please simplify",
  1037,1,"cannot open object file",
  1065,1,"out of tags space",
  1068,1,"cannot open file",
  1069,1,"write error on file",
#if MC68000 || ARCHI
  1126,1,"automatic allocation exceeds size (32768)" ,
#else
  1126,1,"automatic allocation exceeds size (128)" /*anche 2127*/,
#endif
  2001,1,"newline in constant",
  2007,1,"#define syntax",
  2010,1,"invalid formal list",
  2011,1,"redefinition:",
  2012,1,"bad char following include",
  2015,1,"too many chars in costant",
  2017,1,"illegal escape sequence",
  2025,1,"enum/struct/union type redefinition:",
  2026,1,"type redefinition:",
  2027,1,"use of undefined type",
  2030,1,"struct/union member redefinition:",
  2037,1,"left operand specifies undefined struct/union",
  2038,1,"not struct/union member:",   /* anche 2021*/
  2040,1,"different levels of indirection", /*anche 4047*/
  2041,1,"illegal digit for base",
  2043,1,"illegal break",
  2044,1,"illegal continue",
  2045,1,"label redefined",
  2046,1,"illegal case",
  2047,1,"illegal default",
  2048,1,"more than one default",
  2049,1,"case value already used:",
  2050,1,"non-integral switch expression",
  2051,1,"case expression not constant",
  2052,1,"case expression not integral",
  2053,1,"case expression too large for switch variable",
  2054,1," expected",
  2057,1,"expected constant expression",
  2058,1,"divide by zero",
  2059,1,"syntax error",
  2062,1,"unexpected",/*anche 2132*/
  2064,1,"not a function:",/*2063 anche ok*/
  2065,1,"undefined:",
  2068,1,"illegal cast",
  2070,1,"illegal sizeof operand",
  2071,1,"bad storage class",
  2078,1,"too many initializers",
  2079,1,"uses undefined struct/union",
  2082,1,"redefinition of formal parameter:",
  2084,1,"funtion already has a body:",
  2086,1,"redefinition:",/*anche 2011?*/
  2087,1,"missing subscript",
  2093,1,"can't use address of automatic variable as static init",
  2094,1,"label undefined",
  2097,1,"illegal initialization",
  2100,1,"illegal indirection",
  2101,1,"'&' on constant",
  2103,1,"'&' on register variable",
  2104,1,"'&' on bitfield variable",
  2105,1,"needs lvalue",
  2106,1,"left operand must be lvalue",
  2109,1,"subscript on non-array",
//   2110,1,"variable used as a pointer",
  2110,1,"pointer + pointer",
  2111,1,"pointer + non-integral value",
  2115,1,"incompatible types",
  2116,1,"function parameter list differed",
  2127,1,"stack allocation exceeds size (128)" /*anche 1126*/,
  2137,1,"empty character constant",
  2141,3,"value out of range for enum"/*anche 4341*/,
  2149,1,"named bitfield cannot have zero width",
  2153,1,"hex constant must have at least one digit",
  2156,1,"pragma must be outside function",
  2166,1,"l-value specifies const object",
  2200,1,"warning treated as error",
  2205,1,"can't initialize extern variable",
  2221,1,"'.' left operand points to struct/union, use ->",/*anche 2231*/
  2222,1,"'->' left operand has struct/union type, use .",/*anche 2232*/
  2223,1,"left operand must point to struct/union type",/*anche 2227*/
  2224,1,"left operand must have struct/union type",/*anche 2228*/
  2297,1,"operand is illegal (not integer)",
  2371,1,"redefinition (different basic types):",/*anche altri*/
  2599,1,"local functions are not supported",
   3001,1,"interrupt function returning a value",
   3002,1,"interrupt function with parms",
  4002,1,"ignoring unknown flag",/*Microsoft D4002*/
  4005,1,"macro redefinition",
  4013,3,"function undefined; assuming extern returning int",
  4018,3,"signed/unsigned mismatch",
  4035,1,"function with no return value",
  4042,1,"bad storage class",
  4047,1,"different levels of indirection",
  4049,1,"indirection to different types",
  4068,1,"#pragma o attributo sconosciuto",
  4069,1,"ignorato: ",
  4098,1,"void function returning a value",
  4099,1,"void type invalid",
  4101,3,"unreferenced local variable",
  4102,3,"unreferenced label",
  4127,4,"conditional expression is constant",/*anche 4727*/
  4131,4,"old-style declaration",
  4244,3,"conversion, truncation, possible loss of data",
  4305,3,"truncation from ",
  4309,3,"truncation of constant value",
  4701,3,"local variable used without initialization",
  4705,4,"statement has no effect",
  4761,3,"integral size mismatch in argument; conversion supplied",
  0,0,NULL
  };

char *Ccc::OpCond[16]={		// v. OPERANDO_CONDIZIONALE , ne servono solo 6 (logicamente!  NO! direi 10, per unsigned/signed
#if ARCHI
  "LT","GE","LE","GT","EQ","NE", "LT","GE","LE","GT"
#elif Z80
  "m", "p", "m", "p", "z", "nz", "c", "nc","c", "nc"
#elif I8086
  "b", "ae","be","a", "z", "nz", "l", "ge","le","g"
#elif MC68000
  "lt","ge","le","gt","eq","ne", "cs","cc","ls","hi"		// ahem boh...
//  "ge","lt","gt","le","eq","ne", "cc","cs","hi","ls"		// OCCHIO le inverto perché gli operandi sono invertiti qua!
#elif I8051
  "b", "ae","be","a", "z", "nz", "l", "ge","le","g"
#elif MICROCHIP
  "C", "NC","C", "NC","Z", "NZ"," C", "NC","C", "NC"			// VERIFICARE ! 23/8/2010
#endif  
  };
  
char *Ccc::StrOp[20]={
#if ARCHI
  "LT","STMDB","LDMIA","ADD","SUB","B","BL","ADD","SUB","ADC","SBC","AND","OR","EOR","CPL","NEG"
#elif Z80
  "ld","push","pop","inc","dec","jr","jp","call","ret","add","sub","adc","sbc","and","or","xor","cpl","neg"
#elif I8086
  "mov","push","pop","inc","dec","jr","jp","call","ret","add","sub","adc","sbc","and","or","xor","not","neg"
#elif MC68000
  "move","move","move","addq #1","subq #1","bra","jmp","jsr","rts","add","sub","adc","sbc","and","or","eor","not","neg"
#elif I8051
  "l","ge","g","le","z","nz","b","ae","a","be"		// FARE!
#elif MICROCHIP
  "C","NC","C","NC","Z","NZ","C","NC","C","NC"			// VERIFICARE ! 23/8/2010 meglio FARE ;) 2025
#endif  
  };
  
void Ccc::subObj(COutputFile *FO,struct OP_DEF *s) {
  
  if(s->mode & OPDEF_MODE_INDIRETTO)			// b7=1 se indiretto
#if ARCHI
		if(s->mode != OPDEF_MODE_STACKPOINTER_INDIRETTO) {
			if(0)		// usare per STMDB LDMIA, v. anche OPDEF_MODE_REGISTRI
				FO->put('{');
			else
				FO->put('[');
			}
#elif Z80
    FO->put('(');
#elif I8086
//    FO->printf("%s ",s->mode & 0x7f == OPDEF_MODE_REGISTRO16 ? "WORD PTR" : "BYTE PTR");		// FINIRE...
    FO->put('[');
#elif MC68000
// dopo!    
		FO->printf("");		// vuole la printf cmq... boh
#elif MICROCHIP
		// credo che gli "indiretti" qua non siano mai usati, per ora. si potrebbe convertire in doppia sequenza MOVFW / PLUSW0!
		if(CPUEXTENDEDMODE)
	    FO->put('[');
		else 
			;
#endif
  switch(s->mode & 0x7f) {
    case OPDEF_MODE_NULLA:				//
			break;
    case OPDEF_MODE_REGISTRO_LOW8:				// parte low di registro 16bit
#if MC68000 || I8086
			if(s->s.n<0 || s->s.n>15) {
#else
			if(s->s.n<0 || s->s.n>15 /*7*/) {
#endif
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************666",s->s.n);
				PROCError(1001,"bad register number");
				return;
				}
#if I8086
      FO->put(*((*Regs)[s->s.n]));
      FO->put('l');
#else
      FO->put(*((*Regs)[s->s.n]+1));
#endif
      break;
    case OPDEF_MODE_REGISTRO_HIGH8:				// parte high di registro 16bit
#if MC68000 || I8086
			if(s->s.n<0 || s->s.n>15) {
#else
			if(s->s.n<0 || s->s.n>15 /*7*/) {
#endif
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************667",s->s.n);
				PROCError(1001,"bad register number");
				return;
				}
#if I8086
      FO->put(*(*Regs)[s->s.n]);
      FO->put('h');
#else
      FO->put(*(*Regs)[s->s.n]);
#endif
      break;
//    case OPDEF_MODE_REGISTRO16:			// registro 16bit 
//    case OPDEF_MODE_REGISTRO32:			// registro 32bit 
    case OPDEF_MODE_REGISTRO:				// registro 8 o 16 o 32bit intero
#if MC68000 || I8086
			if(s->s.n<0 || s->s.n>15) {
#else
			if(s->s.n<0 || s->s.n>15 /*7*/) {
#endif
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************668",s->s.n);
				PROCError(1001,"bad register number");
				return;
				}
#if MC68000
		  if(s->s.n>=8 && (s->mode & OPDEF_MODE_INDIRETTO))      // metto anche ofs se è index reg
        FO->printf("%d(%s)",s->ofs,(*Regs)[s->s.n]);
			else
				FO->print((*Regs)[s->s.n]);
#elif ARCHI
      FO->print((*Regs)[s->s.n]);
		  if(s->s.n>=8 && (s->mode & OPDEF_MODE_INDIRETTO))      // metto anche ofs se è index reg
        FO->printf(",#%d",s->ofs);
#elif Z80
      FO->print((*Regs)[s->s.n]);
		  if(s->mode & OPDEF_MODE_INDIRETTO)      // metto anche ofs se è index reg  ??? qua
        FO->printf("%+d",s->ofs);
#else
      FO->print((*Regs)[s->s.n]);
		  if(s->s.n>=8 && (s->mode & OPDEF_MODE_INDIRETTO))      // metto anche ofs se è index reg
        FO->printf("%+d",s->ofs);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO8:				// quantità 8bit
#if MC68000 || ARCHI
      FO->printf("#%d",(int8_t)s->s.n);
#else
      FO->printf("%d",(int8_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO16:				// quantità 16bit
#if MC68000 || ARCHI
      FO->printf("#%d",(int16_t)s->s.n);
#else
      FO->printf("%d",(int16_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO32:				// quantità 32bit
#if MC68000 || ARCHI
      FO->printf("#%ld",(int32_t)s->s.n);
#else
      FO->printf("%ld",(int32_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_FRAMEPOINTER:
#if MC68000
			if(!(s->mode & OPDEF_MODE_INDIRETTO))
#endif
				FO->print(Regs->FpS);
			if(s->mode & OPDEF_MODE_INDIRETTO || s->ofs) {      // metto anche +0 se è ind.
#if MICROCHIP
//				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,s->ofs);
//			  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,14); FARE, forse, v. sopra
        FO->printf("%+d",(int16_t)s->ofs);
#elif Z80
        FO->printf("%+d",(int8_t)s->ofs);
#elif I8086
        FO->printf("%+d",(int16_t)s->ofs);
#elif MC68000
        FO->printf("%d(%s)",(int16_t)s->ofs,Regs->FpS);
#elif ARCHI
        FO->printf(",#%+d",(int16_t)s->ofs);
#endif
				}
      break;
    case OPDEF_MODE_VARIABILE:
    case OPDEF_MODE_VARIABILE_INDIRETTO:
#if I8086
//      FO->printf("%s PTR %s","BYTE",s->s.v->label);
      FO->printf("%s",s->s.label);
      if(s->ofs)
        FO->printf("%+d",s->ofs);
#elif MC68000
			if(MemoryModel & MEMORY_MODEL_RELATIVE) {
				if(s->ofs) {
					if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM)
						FO->printf("%s.w%+d-%s(%s)",s->s.label,s->ofs,"__BaseAbs",Regs->AbsS);		// non lo accetta.. RIVERIFICARE
					else
						FO->printf("%s%+d-%s(%s)",s->s.label,s->ofs,"__BaseAbs",Regs->AbsS);
					}
				else {
					if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM)
						FO->printf("%s.w-%s(%s)",s->s.label,"__BaseAbs",Regs->AbsS);		// non lo accetta.. RIVERIFICARE
					else
						FO->printf("%s-%s(%s)",s->s.label,"__BaseAbs",Regs->AbsS);
					}
				}
			else {
				if(s->mode & OPDEF_MODE_INDIRETTO && TipoOut & TIPO_SPECIALE)	{	// cagate di Easy68k, qua dovrebbe bastare il nome var - v. anche il secondo operando, sotto
					if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM)
						FO->printf("#%s",s->s.label);		// v. cose tipo GetAdd...
					else
						FO->printf("#%s",s->s.label);
					}
				else {
					if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM)
						FO->printf("%s.w",s->s.label);
					else
						FO->printf("%s",s->s.label);
					}
				if(s->ofs)
					FO->printf("%+d",s->ofs);
				}
#else
      FO->printf("%s",s->s.label);
      if(s->ofs)
        FO->printf("%+d",s->ofs);
#endif  
      break;
    case OPDEF_MODE_COSTANTE:
#if I8086
//      FO->printf("%s PTR %s","BYTE",s->s.label);
      FO->printf("%s",s->s.label);
#elif MC68000
//      FO->printf("%s %s","BYTE",s->s.v->label);
      FO->printf("%s",s->s.label);
#else
      FO->printf("%s",s->s.label);
#endif  
      if(s->ofs)
        FO->printf("%+d",s->ofs);
      break;
    case OPDEF_MODE_REGISTRI:
#if ARCHI
      FO->printf("{%s}",s->s.label);		// cambiare usando range di registri!
#elif MC68000
      FO->printf("%s",s->s.label);
#else
			PROCError(1001,"range di registri non consentito qua");
#endif  
      if(s->ofs)
        FO->printf("%+d",s->ofs);
      break;
#if ARCHI
    case OPDEF_MODE_SHIFT:
			if(s->s.n>0) {
	      FO->printf("ASL #%u",s->s.n);
				}
			else {
	      FO->printf("ASR #%u",-s->s.n);
//      FO->printf("%s",s->s.label);
				}
      break;
    case OPDEF_MODE_SHIFTU:
			if(s->s.n>0) {
	      FO->printf("ASL #%u",s->s.n);
				}
			else {
	      FO->printf("LSR #%u",-s->s.n);
				}
//      FO->printf("%s",s->s.label);
      break;
    case OPDEF_MODE_SHIFTR:
			if(s->s.n>0) {
	      FO->printf("ASL R%u",s->s.n);
				}
			else {
	      FO->printf("LSR R%u",s->s.n);
				}
//      FO->printf("%s",s->s.label);
      break;
#endif
    case OPDEF_MODE_STACKPOINTER:
    case OPDEF_MODE_STACKPOINTER_INDIRETTO:
			if(s->mode & OPDEF_MODE_INDIRETTO) {
#if MC68000
				switch((int8_t)s->s.n) {
					case 0:
						FO->printf("(%s)",Regs->SpS);
						break;
					case -1:
						FO->printf("-(%s)",Regs->SpS);
						break;
					case 1:
						FO->printf("(%s)+",Regs->SpS);
						break;
					}
#elif I8086
				FO->printf("%s",Regs->SpS);

#elif ARCHI
				if((int8_t)s->s.n)			// se push/pop (forse ovvio ma ok; 
					FO->printf("%s!",Regs->SpS);
				else
					FO->print(Regs->SpS);
#else
				FO->printf("%s",Regs->SpS);
#endif
				}
			else
				FO->print(Regs->SpS);
      break;
    case OPDEF_MODE_CONDIZIONALE:
#if MC68000
      FO->printf(s->s.n & 0x80 ? "%s.s" : "%s",OpCond[s->s.n & 0x7f],FO);
#elif I8086
      FO->printf("%s",OpCond[s->s.n & 0x7f],FO);
#elif ARCHI
      FO->printf("%s",OpCond[s->s.n & 0x7f],FO);
#else
      FO->print(OpCond[s->s.n]);
#endif
      break;
    }
  if(s->mode & OPDEF_MODE_INDIRETTO)
#if ARCHI
		if(s->mode != OPDEF_MODE_STACKPOINTER_INDIRETTO) {
			if(0)		// usare per STMDB LDMIA, v. anche OPDEF_MODE_REGISTRI
				FO->put('}');
			else
		    FO->put(']');
			}
#elif Z80
    FO->put(')');
#elif I8086
    FO->put(']');
#elif MC68000
//			FO->put(')')
			;
#elif MICROCHIP
		if(CPUEXTENDEDMODE)
			FO->put(']');		// forse... finire!
		else
			;
#endif
  }
  
int Ccc::PROCObj(COutputFile *FO) {
  struct LINE *TEXT,*t;
  
  TEXT=RootOut;
  while(TEXT) {
//  myLog->print(0,"istr: %s: %s,%s (%x)\n",TEXT->opcode,TEXT->s1,TEXT->s2,TEXT->type);
    switch(TEXT->type) {  
      case LINE_TYPE_COMMENTO:
				if(TEXT->s1.mode != OPDEF_MODE_NULLA) {
					subObj(FO,&TEXT->s1);
					FO->putcr();
					}
        break;
      case LINE_TYPE_LABEL:
#if ARCHI
	      FO->put('.');
#endif
		    subObj(FO,&TEXT->s1);
#if Z80 || I8086 || MC68000 || I8051 || MICROCHIP
	      FO->put(':');
#endif
        break;
      case LINE_TYPE_DATA_DEF:
        FO->printf("%s\t",TEXT->opcode);
		    subObj(FO,&TEXT->s1);
        break;
      case LINE_TYPE_LABEL_CON_ISTRUZIONE:
#if ARCHI
	      FO->put('.');
#endif
		    subObj(FO,&TEXT->s1);
        FO->printf("\t%s",TEXT->opcode);
        break;
			case LINE_TYPE_JUMP:
			case LINE_TYPE_JUMPC:
			case LINE_TYPE_CALL:
			case LINE_TYPE_ISTRUZIONE:
      default:
	      FO->put('\t'); 				  // prima delle istruzioni TAB
        FO->printf("%s",TEXT->opcode);
		    if(TEXT->s1.mode) {
#if MC68000
					if(TEXT->opcode[1] &&			// serve per i Branch "B" o "J" :)
						!_tcschr(TEXT->opcode,'#'))		// patch per 68000/costanti, ma ok... la lascio
#elif I8086 || ARCHI
					if(TEXT->type != LINE_TYPE_JUMPC)
#endif
						FO->put('\t');
#if I8086
					if(TEXT->s1.mode & OPDEF_MODE_INDIRETTO && !strstr(TEXT->opcode," PTR")) {
						if(((TEXT->s2.mode & 0x7f) == OPDEF_MODE_REGISTRO_LOW8) || ((TEXT->s2.mode & 0x7f) == OPDEF_MODE_REGISTRO_HIGH8))
							FO->print(" BYTE PTR ");		// 
						else if((TEXT->s2.mode & 0x7f) == OPDEF_MODE_REGISTRO16)
							FO->print(" WORD PTR ");		// 
						}
#endif
			    subObj(FO,&TEXT->s1);
		      }
		    if(TEXT->s2.mode) {
#if MC68000
					if(TEXT->type == LINE_TYPE_JUMPC)
						FO->put('\t');
					else
#elif I8086 || ARCHI
					if(TEXT->type == LINE_TYPE_JUMPC)
						FO->put('\t');
					else
#endif
						FO->put(',');
#if MC68000
					if(TEXT->s2.mode == OPDEF_MODE_VARIABILE_INDIRETTO && TipoOut & TIPO_SPECIALE)		// cagate di Easy68k, se no mi esce il cancelletto a dx! - v. anche sopra
						TEXT->s2.mode=OPDEF_MODE_VARIABILE;
#elif I8086
					if(TEXT->s2.mode & OPDEF_MODE_INDIRETTO && !strstr(TEXT->opcode," PTR")) {
						if(((TEXT->s1.mode & 0x7f) == OPDEF_MODE_REGISTRO_LOW8) || ((TEXT->s1.mode & 0x7f) == OPDEF_MODE_REGISTRO_HIGH8))
							FO->print(" BYTE PTR ");		// 
						else if((TEXT->s1.mode & 0x7f) == OPDEF_MODE_REGISTRO16)
							FO->print(" WORD PTR ");		// 
						}
#endif
			    subObj(FO,&TEXT->s2);
			    }  
#if ARCHI
		    if(TEXT->s3.mode) {
					if(TEXT->type == LINE_TYPE_JUMPC)
						FO->put('\t');
					else
						FO->put(',');
			    subObj(FO,&TEXT->s3);
			    }  
#endif
        break;
			case LINE_TYPE_JUMPGOTO:
				{ struct VARS *g;
				if(g=FNCercaGoto(TEXT->s1.s.label)) {
//						PROCOutLab(g->label,"_",CurrFunc->name);
//						FO->println("%s_%s",g->label,CurrFunc->name);		// 
			      FO->put('\t'); 				  // prima delle istruzioni TAB
				    FO->printf("%s",TEXT->opcode);
			      FO->put('\t');
						_tcscat(TEXT->s1.s.label,"_");
						_tcscat(TEXT->s1.s.label,CurrFunc->name);
				    subObj(FO,&TEXT->s1);
						}
					else
						PROCError(2094,TEXT->s1.s.label);
				}
        break;
			}
    if(*TEXT->rem) {
      if(TEXT->type) {
        FO->put('\t');
        FO->put('\t');
        }
	    FO->printf("; %s",TEXT->rem);
      }
    if(TEXT->type == LINE_TYPE_COMMENTO) {		// c'è già nella riga che arriva da OutSource... NO!
			if(TEXT->rem[_tcslen(TEXT->rem)-1] != '\n')		// in certi casi...
			  FO->putcr();
			}
		else
		  FO->putcr();
    t=TEXT;
    TEXT=TEXT->next;
    if(t!=RootOut)
      LastOut=PROCDelLista(RootOut,LastOut,t);
		//ev. PROCDelLastLine(t);
    }
// OSCLI("SETTYPE "+B$+" &FE0")

  return 0;
  }

int Ccc::PROCError(int Er, const char *a) {
  int i;
  char myBuf[256],errBuf[256];

  wsprintf(errBuf,"%s: (%d): errore %d",__file__,__line__,Er);
  i=0;
  while (Er != Errs[i].t && Errs[i].t)
    i++;
  if(Errs[i].t) {
  	wsprintf(myBuf,": %s",Errs[i].s);
		_tcscat(errBuf,myBuf);
 	  if(a)
 	    wsprintf(myBuf,": %s",a);
 	  else  
 	    wsprintf(myBuf,".");
		_tcscat(errBuf,myBuf);
		if(FErr) {
			FErr->println(errBuf);
			FNGetLine(FIn->GetPosition()-80,myBuf);			// SISTEMARE posizione...
			FErr->println(myBuf);
			}
	  if(debug) {
	    PROCV("vartmp.map");
//	    PROCT();
	    }
    }
	else	{
	  wsprintf(myBuf,"(%d)",Er);
    PROCError(1000,myBuf);
    }
/*  if(FPre)
    fclose(FPre);
  if(FObj)
    fclose(FObj);
  if(FO1)
    fclose(FO1);
  if(FO2)
    fclose(FO2);
  if(FO3)
    fclose(FO3);
  if(FLst) {
    FLst->Close();
		delete FLst;
		FLst=NULL;
		}*/
	// NON dovremmo uscire al primo errore... forse
	bExit=1;

	numErrors++;
	if(myOutput) {
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		_tcscpy(p,errBuf);
		myOutput->PostMessage(WM_ADDTEXT,1,(LPARAM)p);
		}
//	throw; 
  return 0;
  }

int Ccc::PROCWarn(int Er, const char *a) {
  int i;
  char myBuf[256],errBuf[256];

  if(Warning) {
		numWarnings++;
	  i=0;
    while (Er != Errs[i].t && Errs[i].t)
      i++;
    if(Errs[i].t) {  
		  if(Warning<0 || Errs[i].l <= Warning) {
			  wsprintf(errBuf,"%s: (%d): warning %d",__file__,__line__,Er);
			  wsprintf(myBuf,": %s %s",Errs[i].s,a ? a : ".");
				_tcscat(errBuf,myBuf);
				{
				char *p=(LPSTR)GlobalAlloc(GPTR,256);
				_tcscpy(p,errBuf);
				myOutput->PostMessage(WM_ADDTEXT,2,(LPARAM)p);
				}
				if(FErr)
					FErr->println(errBuf);
		    if(Warning<0)
		      PROCError(2200,NULL);
			  }
			}
		else	
      PROCError(1000,NULL);
    
    }
  return 0;
  }

int Ccc::PROCV(const char *n) {
  int i;
  char *B;
	char myBuf[512];
  COutputFile *FO;
  struct VARS *V;
  struct CONS *C;
  
  B=CSourceFile::FNTrasfNome((char *)n);
  FO=new COutputFile(B);
  if(!FO) 
    PROCError(1069,OUS);
//  FO=stderr;  
  V=Var;
  while(V) {
		FO->printf("Ogg. %s\t\tTipo %x\tLabel %s\tSize %d\tdi %s\n",
			V->name,V->type,V->label,V->size,V->func ? V->func->name : "");
    FO->printf("Blocco attuale: %d, blocco var: %d\n",InBlock,V->block);
		if(V->tag) {
      FO->printf("\tFa parte del TAG: %s\n",V->tag->label);
			}
		if(V->hasTag) {
      FO->printf("\tIl suo TAG e': %s\n",V->hasTag->label);
			}
    V=V->next;
    }
  FO->putcr();
  C=Con;
  while(C) {
    FO->printf("Cost. %s\t\tLabel %s\n",C->name,C->label);
    C=C->next;
    }
  delete FO;
  return 0;
  }

int Ccc::PROCT() {
  int t;
	char myBuf[256];
  
  for(t=0; t<MaxTypes; t++) {
    myLog->print(1,"Tipo %s:\t\t%lx\t\tSize: %x\t\tTag: %s\n",
			Types[t].s,Types[t].type,Types[t].size,Types[t].tag ? Types[t].tag->label : "");
    }
    
  return 0;
  }

int Ccc::PROCD() {
  struct LINE_DEF *t;
  
  t=m_CPre->RootDef;
  while(t) {
    printf("%s \t\t\tè %s",t->name,t->text);
    t=t->next;
    }
    
  return 0;
  }
  
int Ccc::PROCVarList(COutputFile *FO, struct VARS *func) {
  /*static ??*/ int T=0;
  int I,i;
  char *p;
  char myBuf[256];
  struct VARS *V;
  
  if(func) {
    FO->printf("%s: variabili locali\n",func->name);
		}
  else {
    FO->println("Variabili globali");
		}
  V=Var;
  while(V) {
    if(V->func==func) {
			if(!(T % 60)) {
				FO->printf("\f\n%32s%10s%16s%10s%12s\n\n","Nome","Classe","Tipo","Dim.","Offset/Registro");
				//spostare FormFeed prima/fuori...
				}
			FO->printf("%32s",V->name);

	//    i=26-strlen(Var[T].name)/2;
	//    while(i--)
	//      FO->put('.',FO);
	//    FO->printf("\t");
			switch(V->classe) {
				case CLASSE_EXTERN:
					p="extern";
					break;
				case CLASSE_GLOBAL:
					p="global";
					break;
				case CLASSE_STATIC:
					p="static";
					break;
				case CLASSE_AUTO:
					i=MAKEPTROFS(V->label);
					if(i<0)
						p="auto";
					else 
						p="param";
					break;
				case CLASSE_REGISTER:
					p="register";
					break;
				}
			FO->printf("%10s",p);
	//	  FO->printf("\t");

			if(V->type & VARTYPE_FUNC) {
				if(V->type & VARTYPE_FUNC_POINTER)
					p="funct/ptr";
				else
					p="function";
				}
			// VA TUTTO RIVISTO, i tipi possono miscelarsi...

			else if(V->type & VARTYPE_ARRAY) 
				p="array";
			else if(V->type & VARTYPE_IS_POINTER) 
				p="pointer";
			else {

				if(V->type & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY))
					p="struct/array";
				else if(V->type & VARTYPE_FLOAT)
					p="float";
				else if(V->type & VARTYPE_BITFIELD)
					p="bitfield";
				else {             
					p=myBuf;
					if(V->type & VARTYPE_UNSIGNED)
						_tcscpy(myBuf,"unsigned ");
					else
						*myBuf=0;
					if(V->size == INT_SIZE) {
						_tcscat(myBuf,"int ");
						}
					else {  
						switch(V->size) {
							case 1:
								_tcscat(myBuf,"char ");
								break;
							case 2:
								_tcscat(myBuf,"short ");
								break;
							case 4:
								_tcscat(myBuf,"long ");
								break;
							} 
						}
					if(V->type & VARTYPE_FAR)
						_tcscat(myBuf,"far ");
#if MICROCHIP
					if(V->type & VARTYPE_ROM)
						_tcscat(myBuf,"rom ");
#endif
					}

				}
			FO->printf("%16s",p);

	//	  FO->printf("\t");
			if(V->type & VARTYPE_FUNC) {
				p=myBuf;
//				p="***";
				sprintf(myBuf,"%u %c",V->size,V->type & VARTYPE_POINTER ? '*' : ' ');
				}
			else {
				p=myBuf;
				if(V->type & VARTYPE_ARRAY)
					sprintf(myBuf,"%u",FNGetArraySize(V));
				else {
					if(V->type & (VARTYPE_STRUCT | VARTYPE_UNION))
						sprintf(myBuf,"%u",V->size);
					else {
						if(V->type & VARTYPE_IS_POINTER)
							sprintf(myBuf,"%u",getPtrSize(V->type));
						else
							sprintf(myBuf,"%u",V->size);
						}
					}
				}
			FO->printf("%9s",p);

			if(V->classe==CLASSE_AUTO) {
				p=myBuf;
				I=MAKEPTROFS(V->label);
				sprintf(myBuf,"%d",I);
				}
			else {
				p="***";
  			}
			FO->printf("%7s",p);

			if(V->classe==CLASSE_REGISTER) {
				_tcscpy(myBuf,(*Regs)[V]);
				}
			else
				*myBuf=0;

			FO->println(myBuf);
			T++;
			}
    V=V->next;
    }
	FO->putcr();
	FO->putcr();
  
  return 0;
  }
  