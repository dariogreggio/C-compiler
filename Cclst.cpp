#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"
#include "..\OpenCView2.h"

#include <stdlib.h>
#include <conio.h>


struct ERRORE Errs[]={
  1000,1,"unknown internal error - contact Cyberdyne",
  1001,1,"internal error:",
  1004,1,"unexpected EOF",
  1016,1,"#if[n]def expected an identifier",
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
  1126,1,"automatic allocation exceeds size (128)" /*anche 2127*/,
  2001,1,"newline in constant",
  2007,1,"#define syntax",
  2010,1,"invalid formal list",
  2011,1,"redefinition:",
  2012,1,"bad char following include",
  2015,1,"too many chars in costant",
  2017,1,"illegal escape sequence",
  2025,1,"enum/struct/union type redefinition:",
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
  2054,1," expected",
  2057,1,"expected constant expression",
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
  2093,1,"cannot use address of automatic variable as static init",
  2094,1,"label undefined",
  2097,1,"illegal initialization",
  2100,1,"illegal indirection",
  2101,1,"'&' on constant",
  2103,1,"'&' on register variable",
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
  2153,1,"hex constant must have at least one digit",
  2156,1,"pragma must be outside function",
  2200,1,"warning treated as error",
  2221,1,"'.' left operand points to struct/union, use ->",/*anche 2231*/
  2222,1,"'->' left operand has struct/union type, use .",/*anche 2232*/
  2223,1,"left operand must point to struct/union type",/*anche 2227*/
  2224,1,"left operand must have struct/union type",/*anche 2228*/
  2371,1,"redefinition (different basic types):",/*anche altri*/
  2599,1,"local functions are not supported",
   3001,1,"interrupt function returning a value",
  4002,1,"ignoring unknown flag",/*Microsoft D4002*/
  4005,1,"macro redefinition",
  4013,3,"function undefined; assuming extern returning int",
  4035,1,"function no return value",
  4042,1,"bad storage class",
  4047,1,"different levels of indirection",
  4049,1,"indirection to different types",
  4098,1,"void function returning a value",
  4101,3,"unreferenced local variable",
  4102,3,"unreferenced label",
  4127,4,"conditional expression is constant",/*anche 4727*/
  4131,4,"old-style declaration",
  4701,3,"local variable used without initialization",
  4705,4,"statement has no effect",
  4761,3,"integral size mismatch in argument; conversion supplied",
  0,0,NULL
  };

char *Ccc::OpCond[16]={		// v. OPERANDO_CONDIZIONALE , ne servono solo 6 (logicamente!
#if ARCHI
  "LT","GE","LE","GT","EQ","NE", "LT","GE","LE","GT"
#elif Z80
  "m", "p", "m", "p", "z", "nz", "c", "nc","c", "nc"
#elif I8086
  "b", "ae","be","a", "z", "nz", "l", "ge","le","g"
#elif MC68000
  "ls","ge","le","gt","eq","ne", "cc","cs","lt","ge","mi","pl","vc","vs"
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
  
void Ccc::subObj(FILE *FO,struct OP_DEF *s) {
  
  if(s->mode & OPDEF_MODE_INDIRETTO)			// b7=1 se indiretto
#if ARCHI
    fputc('(',FO);
#elif Z80
    fputc('(',FO);
#elif I8086
    fprintf(FO,"%s [",s->mode & 0x7f ==OPDEF_MODE_REGISTRO16 ? "WORD" : "BYTE");		// FINIRE...
#elif MC68000
// dopo!    
		fprintf(FO,"");		// vuole la printf cmq... boh
#elif MICROCHIP
		// credo che gli "indiretti" qua non siano mai usati, per ora. si potrebbe convertire in doppia sequenza MOVFW / PLUSW0!
		if(CPUEXTENDEDMODE)
	    fputc('[',FO);
		else 
			;
#endif
  switch(s->mode & 0x7f) {
    case OPDEF_MODE_NULLA:				//
			break;
    case OPDEF_MODE_REGISTRO_LOW8:				// parte low di registro 16bit
			if(s->s.n<0 || s->s.n>15) {
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************666",0);
				PROCError(1001,"bad register number");
				return;
				}
      fputc(*((*Regs)[s->s.n]+1),FO);
      break;
    case OPDEF_MODE_REGISTRO_HIGH8:				// parte high di registro 16bit
			if(s->s.n<0 || s->s.n>15) {
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************666",0);
				PROCError(1001,"bad register number");
				return;
				}
      fputc(*(*Regs)[s->s.n],FO);
      break;
//    case OPDEF_MODE_REGISTRO16:			// registro 16bit 
//    case OPDEF_MODE_REGISTRO32:			// registro 132it 
    case OPDEF_MODE_REGISTRO:				// registro 8 o 16bit intero
			if(s->s.n<0 || s->s.n>15) {
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)"********************666",0);
				PROCError(1001,"bad register number");
				return;
				}
#if MC68000
		  if(s->s.n>=8 && (s->mode & OPDEF_MODE_INDIRETTO))      // metto anche ofs se è index reg
        fprintf(FO,"%d(%s)",s->ofs,(*Regs)[s->s.n]);
			else
				fputs((*Regs)[s->s.n],FO);
#else
      fputs((*Regs)[s->s.n],FO);
		  if(s->s.n>=8 && (s->mode & OPDEF_MODE_INDIRETTO))      // metto anche ofs se è index reg
        fprintf(FO,"%+d",s->ofs);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO8:				// quantità 8bit
#if MC68000
      fprintf(FO,"#%d",(int8_t)s->s.n);
#else
      fprintf(FO,"%d",(int8_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO16:				// quantità 16bit
#if MC68000
      fprintf(FO,"#%d",(int16_t)s->s.n);
#else
      fprintf(FO,"%d",(int16_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_IMMEDIATO32:				// quantità 32bit
#if MC68000
      fprintf(FO,"#%ld",(int32_t)s->s.n);
#else
      fprintf(FO,"%ld",(int32_t)s->s.n);
#endif
      break;
    case OPDEF_MODE_FRAMEPOINTER:
#if MC68000
			if(!(s->mode & OPDEF_MODE_INDIRETTO))
#endif
				fputs(Regs->FpS,FO);
			if(s->mode & OPDEF_MODE_INDIRETTO || s->ofs) {      // metto anche +0 se è ind.
#if MICROCHIP
//				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,s->ofs);
//			  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,14); FARE, forse, v. sopra
        fprintf(FO,"%+d",(int16_t)s->ofs);
#elif Z80
        fprintf(FO,"%+d",(int8_t)s->ofs);
#elif I8086
        fprintf(FO,"%+d",(int16_t)s->ofs);
#elif MC68000
        fprintf(FO,"%d(%s)",(int16_t)s->ofs,Regs->FpS);
#endif
				}
      break;
    case OPDEF_MODE_VARIABILE:
#if I8086
//      fprintf(FO,"%s PTR %s","BYTE",s->s.v->label);
      fprintf(FO,"%s",s->s.v->label);
#elif MC68000
			if(s->mode & OPDEF_MODE_INDIRETTO && TipoOut)		// cagate di Easy68k, qua dovrebbe bastare il nome var - v. anche il secondo operando, sotto
	      fprintf(FO,"#%s",s->s.v->label);
			else
	      fprintf(FO,"%s",s->s.v->label);
#else
      fprintf(FO,"%s",s->s.v->label);
#endif  
      if(s->ofs)
        fprintf(FO,"%+d",s->ofs);
      break;
    case OPDEF_MODE_COSTANTE:
#if I8086
//      fprintf(FO,"%s PTR %s","BYTE",s->s.label);
      fprintf(FO,"%s",s->s.label);
#elif MC68000
//      fprintf(FO,"%s %s","BYTE",s->s.v->label);
      fprintf(FO,"%s",s->s.label);
#else
      fprintf(FO,"%s",s->s.label);
#endif  
      if(s->ofs)
        fprintf(FO,"%+d",s->ofs);
      break;
    case OPDEF_MODE_STACKPOINTER:
    case OPDEF_MODE_STACKPOINTER_INDIRETTO:
			if(s->mode & OPDEF_MODE_INDIRETTO) {
#if MC68000
				switch((int8_t)s->s.v) {
					case 0:
						fprintf(FO,"(%s)",Regs->SpS);
						break;
					case -1:
						fprintf(FO,"-(%s)",Regs->SpS);
						break;
					case 1:
						fprintf(FO,"(%s)+",Regs->SpS);
						break;
					}
#elif I8086
				fprintf(FO,"%s",Regs->SpS);

#else
				fprintf(FO,"%s",Regs->SpS);
#endif
				}
			else
				fputs(Regs->SpS,FO);
      break;
    case OPDEF_MODE_CONDIZIONALE:
#if MC68000
      fprintf(FO,s->s.n & 0x80 ? "%s.s" : "%s",OpCond[s->s.n & 0x7f],FO);
#else
      fputs(OpCond[s->s.n],FO);
#endif
      break;
    }
  if(s->mode & OPDEF_MODE_INDIRETTO)
#if ARCHI
    fputc(')',FO);
#elif Z80
    fputc(')',FO);
#elif I8086
    fputc(']',FO);
#elif MC68000
//			fputc(')',FO)
			;
#elif MICROCHIP
		if(CPUEXTENDEDMODE)
			fputc(']',FO);		// forse... finire!
		else
			;
#endif
  }
  
int Ccc::PROCObj(FILE *FO) {
  struct LINE *TEXT,*t;
  
  TEXT=RootOut;
  while(TEXT) {
//  myLog->print(0,"istr: %s: %s,%s (%x)\n",TEXT->opcode,TEXT->s1,TEXT->s2,TEXT->type);
    switch(TEXT->type) {  
      case LINE_TYPE_COMMENTO:
				if(TEXT->s1.mode != OPDEF_MODE_NULLA)
					subObj(FO,&TEXT->s1);
		    fputc('\n',FO);
        break;
      case LINE_TYPE_LABEL:
#if ARCHI
	      fputc('.',FO);
#endif
		    subObj(FO,&TEXT->s1);
#if Z80 || I8086 || MC68000 || I8051 || MICROCHIP
	      fputc(':',FO);
#endif
        break;
      case LINE_TYPE_DATA_DEF:
        fprintf(FO,"%s\t",TEXT->opcode);
		    subObj(FO,&TEXT->s1);
        break;
      case LINE_TYPE_LABEL_CON_ISTRUZIONE:
#if ARCHI
	      fputc('.',FO);
#endif
		    subObj(FO,&TEXT->s1);
        fprintf(FO,"\t%s",TEXT->opcode);
        break;
			case LINE_TYPE_JUMP:
			case LINE_TYPE_JUMPC:
			case LINE_TYPE_CALL:
			case LINE_TYPE_ISTRUZIONE:
      default:
	      fputc('\t',FO); 				  // prima delle istruzioni TAB
        fprintf(FO,"%s",TEXT->opcode);
		    if(TEXT->s1.mode) {
					if(TEXT->opcode[1] &&			// serve per i Branch "B" o "J" :)
						!_tcschr(TEXT->opcode,'#'))		// patch per 68000/costanti, ma ok... la lascio
						fputc('\t',FO);
			    subObj(FO,&TEXT->s1);
		      }
		    if(TEXT->s2.mode) {
#ifdef MC68000
					if(TEXT->type == LINE_TYPE_JUMPC)
						fputc('\t',FO);
					else
#endif
						fputc(',',FO);
#ifdef MC68000
					if(TEXT->s2.mode == OPDEF_MODE_VARIABILE_INDIRETTO && TipoOut)		// cagate di Easy68k, se no mi esce il cancelletto a dx! - v. anche sopra
						TEXT->s2.mode=OPDEF_MODE_VARIABILE;
#endif
			    subObj(FO,&TEXT->s2);
			    }  
        break;
			}
    if(*TEXT->rem) {
      if(TEXT->type) {
        fputc('\t',FO);
        fputc('\t',FO);
        }
	    fprintf(FO,"; %s",TEXT->rem);
      }
//    if(TEXT->type != LINE_TYPE_COMMENTO)		// c'è già nella riga che arriva da OutSource... NO!
		  fputc('\n',FO);
    t=TEXT;
    TEXT=TEXT->next;
    if(t!=RootOut)
      LastOut=PROCDelLista(RootOut,LastOut,t);
    }
// OSCLI("SETTYPE "+B$+" &FE0")

  return 0;
  }

char *Ccc::FNTrasfNome(char *A) {
  char *T,B[256];
  
  if(ANSI)
    return A;
  _tcscpy(B,A);
  if(ACORN) {
    T=strchr(B,'.');
    if(T) { 
      _tcscpy(A,T+1);
      _tcscat(A,".");
      strncat(A,B,T-B-1);
      return A;
      }
    else 
      return A;
    }
  if(GD) {
    T=strchr(A,'.');
    if(T) {   
      *T='_';
      return A;
      }
    else 
      return A;
    }      
                    
  return A;
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
  CStdioFile FO;
  struct VARS *V;
  struct CONS *C;
  
  B=FNTrasfNome((char *)n);
  FO.Open(B,CFile::modeCreate | CFile::modeWrite);
  if(!FO) 
    PROCError(1069,OUS);
//  FO=stderr;  
  V=Var;
  while(V) {
		sprintf(myBuf,"Ogg. %s\t\tTipo %lx\tLabel %s\tSize %d\tdi %s\n",
			V->name,V->type,V->label,V->size,V->func ? V->func->name : "");
    FO.WriteString(myBuf);
    sprintf(myBuf,"Blocco attuale: %d, blocco var: %d\n",InBlock,V->block);
		FO.WriteString(myBuf);
		if(V->tag) {
      sprintf(myBuf,"\tFa parte del TAG: %s\n",V->tag->label);
		  FO.WriteString(myBuf);
			}
		if(V->hasTag) {
      sprintf(myBuf,"\tIl suo TAG e': %s\n",V->hasTag->label);
	    FO.WriteString(myBuf);
			}
    V=V->next;
    }
  FO.WriteString("\n");
  C=Con;
  while(C) {
    sprintf(myBuf,"Cost. %s\t\tLabel %s\n",C->name,C->label);
    FO.WriteString(myBuf);
    C=C->next;
    }
  FO.Close();
  return 0;
  }

int Ccc::PROCT() {
  int t;
	char myBuf[256];
  
  for(t=0; t<MaxTypes; t++) {
    myLog->print(1,"Tipo %s:\t\t%lx\t\tSize: %x\t\tTag: %s\n",
			Types[t].s,Types[t].type,Types[t].size,Types[t].tag->label);
    }
    
  return 0;
  }

int Ccc::PROCD() {
  struct LINE_DEF *t;
  
  t=RootDef;
  while(t) {
    printf("%s \t\t\tè ",t->s);
    t=t->next;
    }
    
  return 0;
  }
  
int Ccc::PROCVarList(CStdioFile *FO, struct VARS *func) {
  static int T=1;
  int I,i;
  char *p;
  char myBuf[256];
  struct VARS *V;
  
  if(func) {
    wsprintf(myBuf,"%s: variabili locali\n",func->name);
    FO->WriteString(myBuf);
		}
  else {
    FO->WriteString("Variabili globali\n");
		}
  V=Var;
  while(V) {
    if(V->func==func) {
			if(!(T % 60)) {
				wsprintf(myBuf,"\f%32s%10s%14s%6s%10s\n\n","Nome","Classe","Tipo","Dim.","Offset/Registro");
				FO->WriteString(myBuf);
				}
			wsprintf(myBuf,"%32s",V->name);
			FO->WriteString(myBuf);
	//    i=26-strlen(Var[T].name)/2;
	//    while(i--)
	//      fputc('.',FO);
	//    fprintf(FO,"\t");
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
					i=*(int *)V->label;
					if(i<0)
						p="auto";
					else 
						p="param";
					break;
				case CLASSE_REGISTER:
					p="register";
					break;
				}
			wsprintf(myBuf,"%10s",p);
			FO->WriteString(myBuf);
	//	  fprintf(FO,"\t");
			if(V->type & VARTYPE_ARRAY) 
				p="array";
			else if(V->type & VARTYPE_IS_POINTER) 
				p="pointer";
			else {
				if(V->type & VARTYPE_FUNC) 
					p="function";
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
#if MICROCHIP
						if(V->type & VARTYPE_ROM)
							_tcscat(myBuf,"rom ");
#endif
						}
					}
				}
			wsprintf(myBuf,"%14s",p);
			FO->WriteString(myBuf);
	//	  fprintf(FO,"\t");
			if(V->type & VARTYPE_FUNC)
				p="***";
			else {
				p=myBuf;
				if(V->type & VARTYPE_ARRAY)
					sprintf(myBuf,"%d",V->dim*V->size);
				else {
					if(V->type & (VARTYPE_STRUCT | VARTYPE_UNION))
						sprintf(myBuf,"%d",V->size);
					else {
						if(V->type & VARTYPE_IS_POINTER)
							sprintf(myBuf,"%d",PTR_SIZE);
						else
							sprintf(myBuf,"%d",V->size);
						}
					}
				}
			wsprintf(myBuf,"%5s",p);
			FO->WriteString(myBuf);
			if(V->classe==CLASSE_AUTO) {
				p=myBuf;
				I=*(int *)V->label;
				sprintf(myBuf,"%d",I);
				}
			else {
				p="***";
  			}
			wsprintf(myBuf,"%5s",p);
			FO->WriteString(myBuf);
			if(V->classe==4) {
				_tcscpy(myBuf,(*Regs)[V]);
				}
			else
				*myBuf=0;
			FO->WriteString(myBuf);
			FO->WriteString("\n");
			T++;
			}
    V=V->next;
    }
	FO->WriteString("\n");
  
  return 0;
  }
  