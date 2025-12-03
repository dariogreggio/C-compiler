#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>



enum Ccc::ARITM_OP Ccc::FNGetAritElem(int8_t *OP, char *OS, struct OPERAND *O, int8_t Co) {
// O% (RISULTATO) = 1 SE COSTANTE
// 2 SE VARIABILE
// 3 SE OPERANDO
// 0 SE FINE LINEA
  int i,j;  
  long T1;
  char TS[128],T1S[64],MyBuf[64];
  char *p;
  
  FNLO(TS);
  switch(*TS) {
    case 0:
    case ':':
    case ';':
    case '}':
      FIn->unget(*TS);		//FIn->Seek(-1,CFile::current);
      return ARITM_IS_EOL;
      break;
    case ')':
      if(!Brack) {
	      FIn->unget(*TS);		//FIn->Seek(-1,CFile::current);
        return ARITM_IS_EOL;
        }
      else {
        *OP=1;
        *OS=')';
        *(OS+1)=0;
        return ARITM_IS_OPERANDO;
        }
      break;
    case '\'':
      i=_tcslen(TS);
#if MC68000 || I8086
      if(i>10)			// 8 char
        PROCError(2015,NULL);
#else
      if(i>6)			// 4 char... magari anche 8 su alcune CPU
        PROCError(2015,NULL);
#endif
      if(i==2) 
        PROCError(2137,NULL);
      T1=0;
      p=TS+1;
      while(*p && *p != '\'') {
				if(*p=='\\') {
					p++;
					if(toupper(*p)=='X') {
						T1=(T1<<8) | xtoi(p+1);
						p+=3;
						}
					else {//		PARE ARRIVINO GIA' espanse appunto ;)
						switch(toupper(*p)) {							// (GESTIRE! \n \t ecc , unire con altrove
							case 'A':
								T1=(T1<<8) | 7;
								break;
							case 'N':
								T1=(T1<<8) | 10;
								break;
							case 'R':
								T1=(T1<<8) | 13;
								break;
							case 'T':
								T1=(T1<<8) | 9;
								break;
							case 'C':
								T1=(T1<<8) | 12;
								break;
							default:
								T1=(T1<<8) | 0;
								PROCWarn(2017,p);
								break;
							}
						}
					}
				else {
					T1=(T1<<8) | ((unsigned char)*p++);
					}
        }
      O->type=0;
      O->size=FNGetSize(T1);
      O->cost->l=T1;
      O->Q=VALUE_IS_COSTANTE;
      return ARITM_IS_COSTANTE;
    case '\"':
      *T1S=0;
      p=TS;
      i=0;
      while(*p) {
        if(!isprint(*p)) {
#if ARCHI
          sprintf(T1S+i,"\"+CHR$%d+\"",*p);
#elif Z80 || I8086 || MICROCHIP
					if(T1S[i-1] != '\"')
            sprintf(T1S+i,"\",%u,\"",*p);
          else                           
            sprintf(T1S+i-1,"%u,\"",*p);
#elif MC68000 
					if(!(TipoOut & TIPO_SPECIALE)) {
						if(T1S[i-1] != '\"')
							sprintf(T1S+i,"\",%u,\"",*p);
						else                           
							sprintf(T1S+i-1,"%u,\"",*p);
						}
					else {
						sprintf(T1S+i,"%u\',\'",*p);			// v. anche in inizializza/costanti dc.b
						}
#endif
          i=_tcslen(T1S);
          }
        else {
          T1S[i++]=*p;
#if MC68000 
					if(TipoOut & TIPO_SPECIALE)
						*T1S='\'';
#endif
					}
        p++;
        }  
#if MC68000 
			if(!(TipoOut & TIPO_SPECIALE)) {
	      if(!_tcsncmp(T1S+i-2,"\"\"",2))
		      T1S[i-2]=0;
				}
			else {
	      T1S[i-1]='\'';
	      if(!_tcsncmp(T1S+i-2,"\'\'",2))
		      T1S[i-2]=0;
				}
#else
      if(!_tcsncmp(T1S+i-2,"\"\"",2))
        T1S[i-2]=0;
#endif
      T1S[i]=0;
#if ARCHI
      _tcscat(T1S,"CHR$0");
#elif Z80 || I8086 || MC68000
      if(!i)
        i++;
      else
       	i--;
      if(T1S[i-1] && (T1S[i-1] !=',')) 
        _tcscat(T1S,",");
      _tcscat(T1S,"0");
#elif MICROCHIP
      if(!i)
        i++;
      else
       	i--;
      if(T1S[i-1] && (T1S[i-1] !=',')) 
        _tcscat(T1S,",");
      _tcscat(T1S,"0");
#endif
#ifndef MICROCHIP
      O->type=VARTYPE_POINTER;
#else
      O->type=VARTYPE_ROM | VARTYPE_POINTER;			// x default literal strings sono ROM
#endif
      O->size=1;                 
      _tcscpy(O->cost->s,FNAllocCost(T1S,1,O->type)->label);
      O->Q=VALUE_IS_COSTANTEPLUS;		// 9
      return ARITM_IS_COSTANTE;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if(_tcschr(TS,'.')) {
				UseFloat=TRUE;		// mah sì diciamo
				O->type=VARTYPE_FLOAT;
				if(_tcslen(TS)<10) {			// diciamo :)
					float f;
					O->size=4;
					f=(float)atof(TS);
					*(float*)&O->cost->l=f;
					O->Q=VALUE_IS_COSTANTE;
					}
				else {
					double f;
					O->size=8;
					f=atof(TS);			// boh verificare, trovare;  https://baseconvert.com/ieee-754-floating-point
	        sprintf(T1S,"%X%X ; LONG FLOAT",(uint32_t)((*(uint64_t*)&f) >> 32),(*(uint64_t*)&f) & 0xffffffff);		// (verificare %llu ... forse qua non va
					_tcscpy(O->cost->s,FNAllocCost(T1S,4)->label);
					O->Q=VALUE_IS_COSTANTEPLUS;		// (potrei evitare se faccio cost->l a 64bit...magari poi vediamo
//					O->Q=VALUE_IS_COSTANTE;
//					*(double*)&O->cost->l64=f;		 VA GESTITO IN MOLTI POSTI! e serve ulltoa(
					}
				// prendere solo i 4 byte alti di un double non è esattamente uguale al float... 
				//	quindi servirà conversione, anche se inizializzazione (passare qua Tipo var
        return ARITM_IS_COSTANTE;
        }
      else {
 	      i=_tcslen(TS)-1;
        if(*TS=='0' && i>1) {
          if(toupper(*(TS+1))=='X')
            //sscanf(TS,"%lx",&O->cost->l);
						O->cost->l=xtoi(TS+2);
          else 
            O->cost->l=FNGetOct(TS+1);
          }
 	      else
          O->cost->l=atol(TS);
//        myLog->print(0,"numero\a: %ld\n",VS->l);
 	      j=toupper(TS[i]);
 	      if(j=='L') {
//        myLog->print(0,"\atrovato un long\n");
	        TS[i]=0;
  	      O->size=4;
 	        }
 	      else {
					i=FNGetSize(O->cost->l);
// (si potrebbe auto-stabilire SIZE...
#if MC68000
 	        O->size=max(2 /*INT_SIZE*/,i);
					if(i>2)
						PROCWarn(4309);		// mah sì :)
#else
 	        O->size=INT_SIZE /*max(INT_SIZE,i)*/;
#endif
					}
	      O->type=VARTYPE_PLAIN_INT;
        O->Q=VALUE_IS_COSTANTE;
	      return ARITM_IS_COSTANTE;
        }
      break;
    default:
      *OP=FNIsOp(TS,Co);
      if(*OP) {
        _tcscpy(OS,TS);
        return ARITM_IS_OPERANDO;
        }
      else {
				struct VARS *v;
// PERCHE'?? 2025        _tcscpy(O->cost->s,TS);
        v=FNCercaVar(TS,FALSE);
        if(!v) {
          if(*FNLA(MyBuf)=='(') {
            O->var=PROCAllocVar(TS,VARTYPE_FUNC,CLASSE_EXTERN,VARTYPE_PLAIN_INT,INT_SIZE,0,0);
            PROCWarn(4013,TS);
            }
          else {
						struct ENUMS *e;
		        e=FNCercaEnum(NULL,TS,FALSE);
						if(e) {
							O->cost->l=e->var.value;
							O->type=VARTYPE_PLAIN_INT;
							O->Q=VALUE_IS_COSTANTE;
							return ARITM_IS_COSTANTE;
							}
						else {
							PROCError(2065,TS);
							goto fine;
							}
						}
          }
				else
					*O->var=*v;
        O->size=O->var->size;
        O->type=O->var->type;
        O->Q=VALUE_IS_VARIABILE;
        return ARITM_IS_VARIABILE;
        }
      break;
    }
fine:
  return ARITM_IS_UNKNOWN;
  }

int Ccc::lltoa(uint64_t num, char *str, /*int len, */uint8_t base) {
	uint64_t sum=num;
	uint8_t i=0;
	uint8_t digit;

//	if(len == 0)
//		return -1;
	do {
		digit = sum % base;
		if(digit < 0xA)
			str[i++] = '0' + digit;
		else
			str[i++] = 'A' + digit - 0xA;
		sum /= base;
		} while (sum /*&& (i < (len - 1))*/);
//	if(i == (len - 1) && sum)
//		return -1;
	str[i] = '\0';
	strrev(str);
	return 0;
	}

int Ccc::subGetType(O_TYPE *t, O_SIZE *s, O_DIM dim, long TT) {
  char AS[64],MyBuf[64];
	uint8_t J;
	long OT;
	uint8_t ndim=0;

	OT=FIn->GetPosition();
  FNLO(AS);
  switch(*FNLO(MyBuf)) {
    case '[':
			J=*t & VARTYPE_IS_POINTER;
      do {
        J++;
        *t= ((*t | VARTYPE_ARRAY) & ~VARTYPE_IS_POINTER) | J;
        if(*FNLA(MyBuf) != ']') {
					int32_t d=FNGetConst(MyBuf,0);
					if(d<0)
			      PROCError(1001,"dimensione array negativa");		// 
          dim[ndim] = d;
					ndim++;
					if(ndim>MAX_DIM-1)
						PROCError(1002,"dimensioni max=4");
          }
        PROCCheck(']');
        } while(*FNLO(MyBuf)=='[');
      break;
    case '(':
      *t |= VARTYPE_FUNC;
      break;
    case ')':		// questo chiude un puntatore a funzione... o anche altro, ma cmq ok
      ;
			OT=FIn->GetPosition();
      break;
    case ':':
      *t |= VARTYPE_BITFIELD;
      PROCWarn(1003,"bitfield");		// 
      break;
    default:
      break;
    }

	FIn->RestorePosition(OT);
	return 0;
  }

int Ccc::PROCGetType(O_TYPE *t, O_SIZE *s, struct TAGS **tag, O_DIM dim, uint32_t *attrib, long TT) {
  int I,J=0;  
	O_SIZE S;
  O_TYPE T=0;
  char AS[64],MyBuf[64];
  long OT;
	long ol;
  struct VARS *V;
  
  OT=FIn->GetPosition();
	FIn->Seek(TT,CFile::begin);
	FIn->SavePosition();
	ol=__line__;
  S=-1;

rifo:
  FNLO(AS);          // ATTENZIONE ALLE PARENTESI
//  *s=0;
//  *t=0l;
//  *tag=*dim=0;
  if(!_tcscmp(AS,"struct") || !_tcscmp(AS,"union")) {
    if(!_tcscmp(AS,"struct"))
      T=VARTYPE_STRUCT;
    else
      T=VARTYPE_UNION;
    *tag=FNAllocAggr();           // alloca tutta la struct o union (was: legge il nome o ne crea uno, poi è pronto per i membri

#pragma warning		fare magari come in ASsembler, i membri delle struct metterli qua e non in VARS  2025


    FNLA(MyBuf);
    if(!*MyBuf)
      PROCError(2059,NULL);
    if(*MyBuf==';') {
      *s=-2;
      }
    else {
      *s=0;
      V=Var;
      while(V) {
        if(V->tag==*tag) {
          I=V->size;
          if(V->type & VARTYPE_ARRAY) {
            I=FNGetArraySize(V);
            }
          else {
            if(V->type & VARTYPE_IS_POINTER)
              I=getPtrSize(V->type);
            }
          if(T & VARTYPE_STRUCT) {
	          if(V->type & VARTYPE_BITFIELD) {
							T |= VARTYPE_BITFIELD;		// me lo segno per dopo! (il padre NON ha l'attributo, anche perché possono coesistere - FINIRE
							*s += MAKEPTRBITF(V->label);
							}
						else {
							if(I>= StructPacking/*INT_SIZE*/) {
								*s=((*s+StructPacking-1) & -StructPacking) +I;
								}
							else
								*s += I;
							}
            }
          else {
            if(I > *s) 
              *s=I;
            }
          }
        V=V->next;
        }
/*      V=Var;                   // non dovrebbe servire, calcola dim struct
      while(V) {
        if(V->hasTag==*tag)
          V->size=*s;
        V=V->next;
        }
        */
      if(T & VARTYPE_BITFIELD) {
				S=(*s+8)/8;
				S = (S+INT_SIZE-1) & -INT_SIZE;
				}
			else
				S=*s;
      *t=T;
      OT=FIn->GetPosition();
      }
    }
  else {
    if(!_tcscmp(AS,"unsigned")) {
      *t |= VARTYPE_UNSIGNED;
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) {
        FNLO(AS);
        TT=FIn->GetPosition();
        OT=TT;
        }
      }
    if(!_tcscmp(AS,"signed")) {
      *t |= VARTYPE_SIGNED;		// :)
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) {
        FNLO(AS);
        TT=FIn->GetPosition();
        OT=TT;
        }
      }
    if(!_tcscmp(AS,"volatile")) {
      *t |= VARTYPE_VOLATILE;
			PROCWarn(1002,"volatile ignorato");
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) {
        FNLO(AS);
        TT=FIn->GetPosition();
        OT=TT;
        }
      }

		if(!_tcscmp(AS,"const"))	{	// GESTIRE! usare DATA_CONST mettere da qualche parte; v. anche di là
			FNLO(AS);
			TT=FIn->GetPosition();
			OT=TT;
			goto rifo;
			}


#ifdef MICROCHIP
    if(!_tcscmp(AS,"rom")) {
      *t |= VARTYPE_ROM;
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) {
        FNLO(AS);
        TT=FIn->GetPosition();
        OT=TT;
        }
      }
#endif

	  for(I=0; I<MaxTypes; I++) {
	    if(!_tcscmp(AS,Types[I].s)) {
	      *s=Types[I].size;
	      *t=Types[I].type | *t;
	      *tag=Types[I].tag;
	      memcpy(dim,Types[I].dim,sizeof(dim));
	      break;
	      }
	    }
		if(!_tcscmp(AS,"short") /*|| !_tcscmp(AS,"signed")*/) {
			if(FNIsType(FNLA(AS)) != VARTYPE_NOTYPE) {		// solita PATCH per short int, v. di là e COMPLETARE
				FNLO(AS);
				TT=FIn->GetPosition();
				OT=TT;
				}
			}
		FNLA(AS);
    if(!_tcscmp(AS,"far")) {
      *t |= VARTYPE_FAR;		// :)
			if((MemoryModel & 0xf)>=MEMORY_MODEL_MEDIUM)
				PROCWarn(4069,"far");		// magari dipende dalle CPU :)
      T=*t;
      FNLO(AS);
      TT=FIn->GetPosition();
      OT=TT;
      }
    if(!_tcscmp(AS,"__attribute__")) {		// GCC extension generica
      FNLO(AS);
			PROCCheck('(');
rifo_attr:
      FNLO(AS);
			if(!_tcscmp(AS,"noreturn")) {
				*attrib |= FUNC_ATTRIB_NORETURN; 
				}
			else if(!_tcscmp(AS,"naked")) {
				*attrib |= FUNC_ATTRIB_NAKED; 
				}
			else if(!_tcscmp(AS,"weak")) {
				*attrib |= FUNC_ATTRIB_WEAK; 
				}
			else if(!_tcscmp(AS,"packed")) {
				*attrib |= VAR_ATTRIB_PACKED;
				}
			else
				PROCWarn(4068,AS);		// finire :)
			if(*FNLA(AS) == ',') {
				PROCCheck(',');
				goto rifo_attr;
				}

			PROCCheck(')');
			TT=FIn->GetPosition();
			OT=TT;
			goto rifo;
			}
    S=*s;
    T=*t;
	  }
  if((int32_t)(int16_t)*s != -2) {		// marker per aggregato... MIGLIORARE
    *s=S;

	/*	if(*AS=='(') {		// potrebbero essercene più d'una... cmq non è perfetto, le parentesi possono circondare anche un Tipo qualsiasi
			T |= VARTYPE_FUNC_POINTER | VARTYPE_FUNC;
			*t = T;
			FNLO(AS);
      TT=FIn->GetPosition();
      OT=TT;
			}*/

//    FIn->Seek(OT,CFile::begin);
		FIn->RestorePosition(OT);
//		__line__=ol;
    J=0;
    while(*FNLA(MyBuf)=='*') {
      J++;
      FNLO(AS);
      *t=T + J;
      }
//    T=*t;
    T=*t;

    FNLA(AS);
		if(*AS=='(') {		// potrebbero essercene più d'una... cmq non è perfetto, le parentesi possono circondare anche un Tipo qualsiasi
			T |= VARTYPE_FUNC_POINTER | VARTYPE_FUNC;
			*t = T;
			FNLO(AS);
			PROCCheck('*');					// appunto, ma ok
      TT=FIn->GetPosition();
      OT=TT;
			}
		
		OT=FIn->GetPosition();
		
		subGetType(t, s, dim, TT);
		//    FIn->Seek(OT,CFile::begin);
		FIn->RestorePosition(OT);
//		__line__=ol;
    }      
    
  return 0;
  }

long Ccc::FNIsType(char *A) {
  int I;
  
  for(I=0; I<MaxTypes; I++) {
    if(!_tcscmp(A,Types[I].s)) { 
      return Types[I].type;     
      }
    }
  if(!_tcscmp(A,"struct"))
    return -2;
  if(!_tcscmp(A,"union"))
    return -3;
  if(!_tcscmp(A,"unsigned"))
    return -4;
  if(!_tcscmp(A,"signed"))		// sinonimi, diciamo
    return -4;
#ifdef MICROCHIP
  if(!_tcscmp(A,"rom"))
    return -5;
#endif
	if(!_tcscmp(A,"const"))	{	// GESTIRE! usare DATA_CONST mettere da qualche parte; v. anche di là
    return -6;
		}
	if(!_tcscmp(A,"volatile"))	{	
    return -8;
		}
  if(!_tcscmp(A,"__attribute__"))			// microchip extension, GCC 
    return -7;
  return VARTYPE_NOTYPE;
  }

struct VARS *Ccc::FNGetAggr(struct TAGS *Tag, const char *TS, bool F, int *o) {  //F=1 per struct, 0 union
  O_SIZE S;
  O_TYPE T;
  struct VARS *V;
  
  *o=0;
  V=Var;
  while(V) {
//    myLog->print(0,"sono su %Fs..",V->name);
    if(V->tag==Tag) {
//    myLog->print(0,("ok\n");
      if(!_tcscmp(V->name,TS)) {
				if(V->type & VARTYPE_BITFIELD) {
					}
				else {
					if(((!T) && (S==INT_SIZE)) || ((T & VARTYPE_IS_POINTER) && !(T & VARTYPE_ARRAY))) {
						if(S>1)
							*o=((*o+StructPacking-1) & -StructPacking);
						// v. anche  attrib & VAR_ATTRIB_PACKED;
						}
					else {
						*o=((*o+StructPacking-1) & -StructPacking);
						}
					}
        break;
//      o%=(o%+3)&& &FFFC
	      }   
	    else {
	      T=V->type;
	      S=V->size;
	      if(T & VARTYPE_ARRAY) {
	        S = FNGetArraySize(V);
	        }
	      else {
	        if(T & VARTYPE_IS_POINTER)
	          S=getPtrSize(T);
	        }
		    if(F) {
					if(V->type & VARTYPE_BITFIELD) {
						MAKEPTRBITF(V->label);		// gestito in FNGetAggr2
						}
					else {
						if(((!T) && (S==INT_SIZE)) || ((T & VARTYPE_IS_POINTER) && !(T & VARTYPE_ARRAY))) {
	//		        *o=((*o+INT_SIZE-1) & -INT_SIZE) +S;
							if(S>1)
								*o=((*o+StructPacking-1) & -StructPacking) +S;
							else
								*o=S;
							// v. anche  attrib & VAR_ATTRIB_PACKED;
							}
						else {
	//		        *o += S;
							*o=((*o+StructPacking-1) & -StructPacking) +S;
							}
						}  
					}  
	      }
      }
    V=V->next;
    }
  return V;
  }

uint32_t Ccc::FNGetAggr2(struct VARS *p, struct VARS *v, int *o, int *o2) {  // restituisce pos e mask per bitfield
	struct VARS *V;
	uint32_t i,j;
	O_TYPE T;
	char TS[64];

  i=0;
  V=Var;
  while(V) {
    if(V->hasTag==v->tag) {
			T=V->type;
			}
    if(V->tag==v->tag) {
			if(!_tcscmp(V->name,v->name)) {
				if(i+MAKEPTRBITF(V->label) > INT_SIZE*8) {		// se a-cavallo...  ev. Packed/StructPacking? forse su bitfield no
					i= (i+INT_SIZE) & -INT_SIZE;
					}
				break;
				}   
			else {
				if(v->type & VARTYPE_BITFIELD) {
					// vedere se consentire mix bitfield e non! ovunque
					}
				if(T & VARTYPE_STRUCT) {
					i += MAKEPTRBITF(V->label);
					if(i)
						;
					}  
				}
			}
    V=V->next;
    }

	*o=j=i;
	if(i=MAKEPTRBITF(V->label)) {		// safety :)
		uint32_t k= j ? (1 << j) : 1;
		j=0;
		do {
			j <<= 1;
			j |= k;
			} while(--i);
		}

	*o2=0;
	while(*o>=INT_SIZE*8) {		// 
		*o2+=INT_SIZE;
		*o-=INT_SIZE*8;
		}


  return j;
  }

struct TAGS *Ccc::subAllocTag(const char *TS) {
  struct TAGS *C;
  
  C=(struct TAGS *)malloc(sizeof(struct TAGS)); 
  if(!C) {
    PROCError(1001,"Fine memoria TAGS");
    }
  if(StrTag) {
    LTag->next=C;
    LTag=C;
    }
  else {
    LTag=StrTag=C;
    }
  C->next=(struct TAGS*)NULL;
	C->member=(struct VARS*)NULL;
  _tcsncpy(C->label,TS,MAX_NAME_LEN);
  C->label[MAX_NAME_LEN]=0;
  return C;
  }

struct TAGS *Ccc::FNAllocAggr() {
  bool Go=0;
	O_DIM dim;
	O_SIZE s;
  O_TYPE t;
	uint32_t attrib=0;
	int i;
  char MyBuf[sizeof(union STR_LONG)],TS[64],AS[64];
  long OT;
  struct VARS *V;
  struct TAGS *C,*tag;

  if(*FNLA(MyBuf) != '{') {
    FNLO(TS);
    C=StrTag;
    while(C) {
      if(!_tcscmp(TS,C->label))
        break;
      C=C->next;
      } 
    if(!C) {
      C=subAllocTag(TS); 
      if(*FNLA(MyBuf) != '{') 
        PROCError(2079,TS);
      else 
        FNLO(TS);
      }
    else {
      if(*FNLA(MyBuf) == '{') {
        _tcscpy(MyBuf,"tag ");
        _tcscat(MyBuf,TS);
        PROCError(2025,MyBuf);
        }
      else
        Go=TRUE;
      }
    }
  else {
    C=subAllocTag(FNGetLabel(MyBuf,3)); 
    PROCCheck('{');
    }
  if(!Go) {                 // guardo i singoli membri
    do {
			OT=FIn->GetPosition();
      FNLO(TS);
      s=INT_SIZE;
      t=VARTYPE_PLAIN_INT;
      ZeroMemory(dim,sizeof(dim));
      tag=NULL;

			PROCGetType(&t,&s,&tag,dim,&attrib,OT);
			FNLA(AS);
			if(!_tcscmp(AS,"short")/* || !_tcscmp(T,"signed")*/) {
				FNLA(AS);
				if(FNIsType(AS)==VARTYPE_PLAIN_INT && !t) {		// PATCH rapida per "short int" ecc... MIGLIORARE
					FNLO(AS);
					}
				}

			goto primogiro;		// perché ho già l'ev. ptr qua! v.sotto, migliorare

      do {



//        PROCGetType(&t,&s,&tag,dim,&attrib,OT);
//        myLog->print(0,"tipo %lx, size %d\n",t,s);
//			  FNLA(AS);




				t &= VARTYPE_NOT_A_POINTER;
				i=t & VARTYPE_ARRAY ? 0 : 0;		// gli array sono sempre anche puntatori, minimo
				while(*FNLA(MyBuf)=='*') {		// sarebbe da gestire in GetType... qua il * non appartiene al tipo dichiarato a inizio riga ma per ciascuno...
					FNLO(MyBuf);
					i++;
					}
				t=i;
				OT=FIn->GetPosition();

				subGetType(&t, &s, dim, OT);

primogiro:

				FNLO(AS);
				if(FNCercaVar(C,AS))
					PROCError(2011,AS);


	      V=PROCAllocVar(AS,t,CLASSE_EXTERN,0,s,tag,dim);
//				FNLO(MyBuf);


        V->tag=C;

        if(*FNLA(MyBuf) == ':') {
					FNLO(MyBuf);
					i=FNGetConst(MyBuf,0);
					if(!i)
						PROCError(2149);
					if(i>INT_SIZE*8) {
						PROCWarn(4309);		// vabbe' :)
						i=INT_SIZE*8;
						}
					MAKEPTRBITF(V->label)=i;		// la dim del bitfield
					}

				if(OutSource) {
					wsprintf(MyBuf,"|%5u| : .. %s %u",__line__,AS,t & VARTYPE_BITFIELD ? MAKEPTRBITF(V->label) : V->size);
//					FNGetLine(OldTextp,MyBuf+10);		// complicato... lascio solo nomi
//					MyBuf[_tcslen(MyBuf)-2]=0;		// tolgo CR se no diventa doppio
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF *)0,0,MyBuf);
					}

        while(*FNLA(MyBuf) == '[') {
          while(*FNLO(MyBuf) != ']')
						;
          }
        FNLO(TS);
        } while((*TS != ';') && (*TS));
      if(!*TS)
        PROCError(2054,";");
      if(*FNLA(MyBuf) == '}')
        Go=TRUE;
      } while(!Go);
    PROCCheck('}');
    }
  return C;
  }

int Ccc::StoreVar(struct VARS *V, int8_t RQ, struct VARS *RVar, union STR_LONG *RCost, bool isPtr) {
  O_SIZE S;
	O_TYPE T;
	int i;
  long l;
  char myBuf[64];
  
  if(debug)
    myLog->print(0,"STOREVAR in VAR# %d\n",V);
	
  S=V->size;
  T=V->type;
  if(T & VARTYPE_ARRAY)
		PROCError(2106,NULL);
  if(T & VARTYPE_CONST)
		PROCError(2166,NULL);
  if((T & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY | VARTYPE_FUNC)) && !(T & VARTYPE_IS_POINTER))
		PROCError(2106,NULL);
	if(RQ == VALUE_IS_VARIABILE) {
		if(RVar->size > V->size)
			PROCWarn(4244,NULL);
		}
	else if(RQ & VALUE_IS_COSTANTE) {
		if(FNGetSize(RCost->l) > V->size)
			PROCWarn(4244,NULL);
		}

	// anche su StoreD0
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

	S=FNGetMemSize(T,S,0/*dim*/,1);
#if ARCHI
  if(RQ==VALUE_IS_COSTANTE)
    PROCUseCost(RQ,0,S,RCost);
  switch(V->classe) {
		case CLASSE_EXTERN:
		case CLASSE_GLOBAL:
		case CLASSE_STATIC:
      _tcscpy(AS,V->label);
		  switch(S) {
				case 1:
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",",AS,NULL);
				  break;
				case 2:
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",",AS,NULL);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,",",Regs->DS,", LSR #8");
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",",AS,"+1");
				  break;
				case 4:
			  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,Regs->D,",",AS,NULL);
				  break;
				}   
		  break;
		case CLASSE_AUTO:
      i=MAKEPTROFS(V->label);
		  switch(S) {
				case 1:               
				  sprintf(MyBuf,",#%d]",i);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",[",Regs->FpS,MyBuf);
				  break;
				case 2:
				  sprintf(MyBuf,",#%d]",i);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",[",Regs->FpS,MyBuf);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,",",Regs->DS,", LSR #8");
				  sprintf(MyBuf,",#%d]",i+1);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"STRB",Regs->D,",[",Regs->FpS,MyBuf);
				  break;
				case 4:
				  sprintf(MyBuf,",#%d]",i);
				  PROCOper(LINE_TYPE_ISTRUZIONE,storString,Regs->D,",[",Regs->FpS,MyBuf);
				  break;
				}
		  break;
		default:
		  _tcscpy(BS,LastOut->s);
		  p=strstr(BS,Regs->DS);
		  if(p) {
//        BS=LEFT$(BS,T%)+AS+MIDS(BS,T%+2);
				_tcsncpy(LastOut->s,BS,p-BS);
				_tcscat(LastOut->s,Regs[V]);
				_tcscat(LastOut->s,p+2);
				}
		  else {
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOV R",AS,",",Regs->D,NULL);
				}
		  switch(S) {
				case 1:
				  PROCOper(LINE_TYPE_ISTRUZIONE,"AND R",AS,",R",AS,",#&FF");
				  break;
				case 2:
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV R",AS,",R",AS,", ASL #16");
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV R",AS,",R",AS,", LSR #16");
				  break;
				}  
		  break;
		}      
#elif Z80
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(getPtrSize(T));
  	PROCGetAdd(VALUE_IS_VARIABILE,V,0,TRUE);
    Regs->Dec(getPtrSize(T));
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
    PROCOper(LINE_TYPE_ISTRUZIONE,"ldir",OPDEF_MODE_NULLA,0);
    }
  else {  
	  switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
//	      _tcscpy(AS,V->label);
			  switch(S) {
					case 1:
				    if(RQ==VALUE_IS_COSTANTE) {
				      PROCUseCost(RQ,0,S,RCost,FALSE);
	//			  		PROCOper(movString,Regs->Accu,RCost->l & 0xff);
				  	  }
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost,FALSE);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO,Regs->D);
					  if(S>2) {
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2,OPDEF_MODE_REGISTRO,Regs->D+1);
					    }
					  break;
					}   
			  break;
			case CLASSE_AUTO:
	      i=MAKEPTROFS(V->label);
			  switch(RQ) {
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:               
							case 2:
							case 4:
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_IMMEDIATO8,l);
							  if(S>1) {
	  						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
								  if(S>2) {
	    						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
	    						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
									  }
								  }
								break;
							}
						break;
			    default:
					  switch(S) {
							case 1:               
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
							  break;
							case 2:
							case 4:
	 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
							  if(S>2) {
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
								  }
							  break;
							}
						break;
					}		
			  break;
			case CLASSE_REGISTER:
			  switch(RQ) {
			    case VALUE_IS_VARIABILE:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar->label,0);
					  else {
					    ReadVar(RVar,V->type,V->size,FALSE,FALSE);
					    goto rSVar;  
					    }
			      break;
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO8,RCost->l);
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO16,RCost->l);
							  break;
							}  
						break;
					default:
rSVar:					
					  switch(S) {
							case 1:
			//		  PROCOper(movString,Regs->DSh,0);  // chissefrega del cast
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
							  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							}
						break;	
					}				
			  break;
			}  
		}      
#elif I8086
	char storString2[16];
	_tcscpy(storString2,storString);
	switch(S) {			// vedere come in subshift ecc se serve flag per evitare questi doppioni in PROCObj
		case 1:
			_tcscat(storString2," BYTE PTR ");
			break;
		case 2:
			_tcscat(storString2," WORD PTR ");
			break;
		case 4:
			_tcscat(storString2," DWORD PTR ");
			break;
		}
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(getPtrSize(T));
  	PROCGetAdd(VALUE_IS_VARIABILE,V,0,TRUE);
    Regs->Dec(getPtrSize(T));
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
    PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
    }
  else {  
	  switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
			  switch(S) {
					case 1:
				    if(RQ==VALUE_IS_COSTANTE) {
				      PROCUseCost(RQ,0,S,RCost,FALSE);
				  	  }
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost,FALSE);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO16,Regs->D);
					  if(S>2)
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2,OPDEF_MODE_REGISTRO16,Regs->D+1);
					  break;
					}   
			  break;
			case CLASSE_AUTO:
	      i=MAKEPTROFS(V->label);
			  switch(RQ) {
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:               
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_IMMEDIATO8,l);
								break;
							case 2:
							case 4:
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_IMMEDIATO16,LOWORD(l));
								if(S>2)
	    						PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_IMMEDIATO16,HIWORD(l));
								break;
							}
						break;
			    default:
					  switch(S) {
							case 1:               
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
							  break;
							case 2:
							case 4:
	 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,Regs->D);
							  if(S>2)
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO16,Regs->D+1);
							  break;
							}
						break;
					}		
			  break;
			case CLASSE_REGISTER:
			  switch(RQ) {
			    case VALUE_IS_VARIABILE:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar->label,0);
					  else {
					    ReadVar(RVar,V->type,V->size,FALSE,FALSE);
					    goto rSVar;  
					    }
			      break;
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)));
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l));
							  break;
							}  
						break;
					default:
rSVar:					
					  switch(S) {		// serve qua??
							case 1:
			//		  PROCOper(movString,Regs->DSh,0);  // chissefrega del cast
							case 2:
								PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO,Regs->D);
//							  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
//							  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							}
						break;	
					}				
			  break;
			}  
		} 
#elif MC68000
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc((int8_t)getPtrSize(T));
  	PROCGetAdd(VALUE_IS_VARIABILE,V,0,TRUE);
    Regs->Dec((int8_t)getPtrSize(T));
		switch(S) {
			case 4:
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
				PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
				break;
			case 8:
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
				PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
				break;
			}

    }
  else {  
		char storString2[16];
		_tcscpy(storString2,storString);
		switch(S) {
			case 1:
				_tcscat(storString2,".b");
				break;
			case 2:
				_tcscat(storString2,".w");
				break;
			case 4:
				_tcscat(storString2,".l");
				break;
			}
		uint8_t srcReg;
		if(RQ==VALUE_IS_VARIABILE && RVar->classe==CLASSE_REGISTER)
			srcReg=MAKEPTRREG(RVar->label);
		else
			srcReg=Regs->D;
	  switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
			  switch(RQ) {
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:               
								if(!LOBYTE(LOWORD(RCost->l)))
					  			PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_VARIABILE /*_INDIRETTO*/,(union SUB_OP_DEF *)&V->label,0);
								else
					  			PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							case 2:
								if(!LOWORD(RCost->l))
						  		PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								else
						  		PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							case 4:
								if(!RCost->l)
					  			PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								else
					  			PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,RCost->l,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							}
						break;
			    case VALUE_IS_COSTANTEPLUS:
					  switch(S) {
							case 1:               
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							case 2:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							case 4:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								break;
							}
						break;
			    case VALUE_IS_D0:
			    case VALUE_IS_EXPR:
			    case VALUE_IS_EXPR_FUNC:
			    case VALUE_IS_VARIABILE:
					  switch(S) {
							case 1:               
								if(isPtr)
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								else
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,srcReg,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							  break;
							case 2:
								if(isPtr)
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								else
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO16,srcReg,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							  break;
							case 4:
								if(isPtr)
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								else
							  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO32,srcReg,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							  break;
							}
						break;
			    default:
						PROCError(1001,"bad mode");
						break;	
					}		
			  break;
			case CLASSE_AUTO:
	      i=MAKEPTROFS(V->label);
			  switch(RQ) {
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:               
								if(!LOBYTE(LOWORD(RCost->l)))
								  PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 2:
								if(!LOWORD(RCost->l))
								  PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 4:
								if(!RCost->l)
								  PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,RCost->l,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							}
						break;
			    case VALUE_IS_COSTANTEPLUS:
					  switch(S) {
							case 1:               
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 2:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 4:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							}
						break;
			    case VALUE_IS_D0:
			    case VALUE_IS_EXPR:
			    case VALUE_IS_EXPR_FUNC:
			    case VALUE_IS_VARIABILE:
					  switch(S) {
							case 1:
								if(isPtr)
									PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,srcReg,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							case 2:
								if(isPtr)
		 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
		 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,srcReg,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							case 4:
								if(isPtr)
		 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								else
		 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,srcReg,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							}
						break;
			    default:
						PROCError(1001,"bad mode");
						break;	
					}		
			  break;
			case CLASSE_REGISTER:
			  switch(RQ) {
					case VALUE_IS_VARIABILE:               // carico direttamente (var statica) in reg
			      if(RVar->classe<=CLASSE_STATIC)
							PROCOper(LINE_TYPE_ISTRUZIONE,storString2,RVar->type & VARTYPE_ARRAY ? OPDEF_MODE_VARIABILE_INDIRETTO : OPDEF_MODE_VARIABILE,
								(union SUB_OP_DEF *)&RVar->label,0,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label)); // caso particolare array, v.GetAdd in ReadVar altrimenti
					  else {
					    ReadVar(RVar,V->type,V->size,FALSE,FALSE);
					    goto rSVar;  
					    }
			      break;
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:
							  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq"/*storString2*/,OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(RCost->l)),
									OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,LOWORD(RCost->l),OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
							  break;
							case 4:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,RCost->l,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
							  break;
							}  
						break;
			    case VALUE_IS_COSTANTEPLUS:
					  switch(S) {
							case 1:               
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
								break;
							case 2:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
								break;
							case 4:
	  						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)&RCost->s,0,
									OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
								break;
							}
						break;
			    case VALUE_IS_D0:
			    case VALUE_IS_EXPR:
			    case VALUE_IS_EXPR_FUNC:
rSVar:					
					  switch(S) {
							case 1:
			//		  PROCOper(movString,Regs->DSh,0);  // chissefrega del cast
								if(isPtr)
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
								else
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,srcReg,
										OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							case 2:
								if(isPtr)
									PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO16,srcReg,
										OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
							  break;
							case 4:
								if(isPtr)
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P,
										OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
								else
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO32,srcReg,
										OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
							  break;
							}
						break;	
			    default:
						PROCError(1001,"bad mode");
						break;	
					}				
			  break;
			}  
		} 
#elif MICROCHIP
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(getPtrSize(T));
  	PROCGetAdd(VALUE_IS_VARIABILE,V,0,TRUE);
    Regs->Dec(getPtrSize(T));
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
    PROCOper(LINE_TYPE_ISTRUZIONE,"ldir",OPDEF_MODE_NULLA,0);
    }
  else {  
	  switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
//	      _tcscpy(AS,V->label);
			  switch(S) {
					case 1:
				    if(RQ==VALUE_IS_COSTANTE) {
				      PROCUseCost(RQ,0,S,RCost,0,FALSE);		// FINIRE
	//			  		PROCOper(movString,Regs->Accu,RCost->l & 0xff);
				  	  }
						if(T & VARTYPE_ROM) {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,"TBLWR",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							}
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost,0,FALSE);		// FINIRE
						if(T & VARTYPE_ROM) {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,"TBLWR",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							}
//				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO,Regs->D);
					  if(S>2) {
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2,OPDEF_MODE_REGISTRO,Regs->D+1);
					    }
					  break;
					}   
			  break;
			case CLASSE_AUTO:
	      i=MAKEPTROFS(V->label);
			  switch(RQ) {
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:               
							case 2:
							case 4:
								l=RCost->l;
								if(CPUEXTENDEDMODE) {
									}
								else {
									// qui, se offset è 0 (il primo, insomma) C18 usa il trucchetto di fare POSTINC/POSTDEC anziché passare da PRODL...
									if(l & 0xff) {
				  					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,l & 0xff);
				  					PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
										}
									else {
				  					PROCOper(LINE_TYPE_ISTRUZIONE,"CLRF",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
										}
			  					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,i);
								  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,1,OPDEF_MODE_REGISTRO,14);

									if(S>1) {
										if(*(((char *)&l)+1)) {
				  						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));
				  						PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
											}
										else {
				  						PROCOper(LINE_TYPE_ISTRUZIONE,"CLRF",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
											}
				  					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,i+1);
									  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,1,OPDEF_MODE_REGISTRO,14);
	//	  						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+1));

										if(S>2) {
											if(*(((char *)&l)+2)) {
				  							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
				  							PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
												}
											else {
				  							PROCOper(LINE_TYPE_ISTRUZIONE,"CLRF",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
												}
				  						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,i+2);
										  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,1,OPDEF_MODE_REGISTRO,14);
											if(*(((char *)&l)+3)) {
				  							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+3));
				  							PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
												}
											else {
				  							PROCOper(LINE_TYPE_ISTRUZIONE,"CLRF",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
												}
				  						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_IMMEDIATO8,i+3);
										  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,1,OPDEF_MODE_REGISTRO,14);
											}
										}
									}
								break;
							}
						break;
			    default:
					  switch(S) {
							case 1:               
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
							  break;
							case 2:
							case 4:
	 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
							  if(S>2) {
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
								  }
							  break;
							}
						break;
					}		
			  break;
			default:
			  switch(RQ) {
			    case VALUE_IS_VARIABILE:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar->label,0);
					  else {
					    ReadVar(RVar,V->type,V->size,FALSE,0,FALSE);		// FINIRE
					    goto rSVar;  
					    }
			      break;
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO8,RCost->l);
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_IMMEDIATO16,RCost->l);
							  break;
							}  
						break;
					default:
rSVar:					
					  switch(S) {
							case 1:
			//		  PROCOper(movString,Regs->DSh,0);  // chissefrega del cast
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,Regs->D);
							  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							}
						break;	
					}				
			  break;
			}  
		}      
#endif
  if(OutSource) {
//    i=_tcslen(LastOut->s)+_tcslen(V->name)+20;
//    PROCOut(NULL,"\t\t\t\t; ",V->name,NULL,NULL);	
//    LastOut=(struct LINE *)_frealloc(LastOut,i);
//    LastOut->prev->next=LastOut;
    _tcscpy(LastOut->rem,V->name);
    }
			   
  return 0;
  }

#if MICROCHIP
int Ccc::ReadVar(struct VARS *V,O_TYPE T,O_SIZE S,uint8_t isCond,bool asPtr,uint8_t lh) {   // m=0 se norm, 1 se condiz.
#else
int Ccc::ReadVar(struct VARS *V,O_TYPE T,O_SIZE S,uint8_t isCond,bool asPtr) {   // m=0 se norm, 1 se condiz.
#endif
  int i,s,s1;                                           // nSize è SIZE per CAST... 0 se non voluto
  char myBuf[64];   

  if(!S) {
	  S=V->size;
    T=V->type;                    // preservo solo unsigned... (da rivedere)
	  }
	else {
	  T=(V->type & ~(VARTYPE_UNSIGNED | VARTYPE_IS_POINTER)) | (T & (VARTYPE_UNSIGNED | VARTYPE_IS_POINTER));		// e ROM??
	  }  
  s1=FNGetMemSize(T,S,0/*dim*/,1);
  s=FNGetMemSize(V,1);
  s1=__min(s,s1);
  if((T & VARTYPE_ARRAY) || ((T & (VARTYPE_UNION | VARTYPE_FLOAT | VARTYPE_STRUCT | VARTYPE_FUNC) /*0x3900*/) && (!(T & VARTYPE_IS_POINTER)))) {
		PROCGetAdd(VALUE_IS_VARIABILE,V,0,asPtr);
		}
  else {
#if MC68000
		char movString2[16];
		_tcscpy(movString2,movString);
		switch(s) {
			case 1:
				_tcscat(movString2,T & VARTYPE_IS_POINTER ? ".l" : ".b");
				break;
			case 2:
				_tcscat(movString2,T & VARTYPE_IS_POINTER ? ".l" : ".w");
				break;
			case 4:
				_tcscat(movString2,".l");
				break;
			}   
#endif
		switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
//        _tcscpy(AS,V->label);
				switch(s1) {
				  case 1:
#if ARCHI
//						PROCOut(0x10,"LDRB",Regs->D,",",AS,NULL);
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
#elif Z80
 						if(isCond)
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
 						else  
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
#elif I8086
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
 						if(isCond)
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
 						if(isCond)
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
 						else  
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
#endif
						break;
				  case 2:                    
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,1);
						sprintf(MyBuf,",%s, ASL #8",Regs->D1S);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1,MyBuf);
#elif Z80
						if(isCond) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
#elif I8086
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,/* MemoryModel */ asPtr ? Regs->P : Regs->D,
							OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
 						if(isCond)
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D);
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO,
							/* MemoryModel */ asPtr ? Regs->P : Regs->D);
#elif MICROCHIP
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
						if(isCond) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,lh);
							}
#endif
						break;
			  	case 4:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",Regs->D,AS);
#elif Z80 
						if(isCond) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
							}
#elif I8086
						if(CPU86>=3) {
							if(asPtr) {
								}
							else {
								}
							}
						else {
							if(asPtr) {/* MemoryModel  */
								PROCOper(LINE_TYPE_ISTRUZIONE,Regs->P == 8 ? "les" : "lds",OPDEF_MODE_REGISTRO16, Regs->P,		// vabbe' :)
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16, Regs->D,
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								}
							}
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE/*_INDIRETTO*/,(union SUB_OP_DEF *)&V->label,0,OPDEF_MODE_REGISTRO32,
							asPtr ? Regs->P : Regs->D);
#elif MICROCHIP
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
						if(isCond) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,lh);
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
							}
#endif
						break;
				  }
				break;
		  case CLASSE_AUTO:
        i=MAKEPTROFS(V->label);
				switch(s1) {
				  case 1:
//				    FNGetFPStr(MyBuf,i,NULL);
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",Regs->D,MyBuf);
#elif Z80
   					if(isCond)
    					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
    				else	
     					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
#elif I8086
   					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						if(isCond)
    					PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
#elif MC68000
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
						if(isCond) {
//    					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,14);
							}
						else	{
//     					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,14);
							}
#endif
						break;
				  case 2:
#if ARCHI
//						sprintf(MyBuf,",#%s]",AS);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
//						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",Regs->D,",[",Regs->FpS,MyBuf);
//						sprintf(MyBuf,",#%s]",atoi(AS)+1);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
//						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",Regs->D1S,",[",Regs->FpS,MyBuf);
//						sprintf(MyBuf,",%s, ASL #8",Regs->D1S);
//						PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",Regs->D,",",Regs->DS,MyBuf);
					  PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO,", ASL #8");
#elif Z80 
						if(isCond) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
#elif I8086
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						if(isCond)
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,
							/* MemoryModel  asPtr ? Regs->P : Regs->D*/ Regs->D);
#elif MICROCHIP
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
						if(isCond) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,i+lh);
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,14);

//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
#endif
						break;
			  	case 4:
#if ARCHI
//						sprintf(MyBuf,",#%s]",AS);
//						PROCOut("LDR",OPDEF_MODE_REGISTRO,Regs->DS,",[",Regs->FpS,MyBuf);
					  PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",OPDEF_MODE_REGISTRO,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
#elif Z80
						if(isCond) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
							}
#elif I8086
						if(CPU86>=3) {
							if(asPtr) {
 								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32, Regs->P,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							if(isCond)
								PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							else {
 								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32, Regs->P,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								}
							}
						else {
							if(asPtr) {/* MemoryModel  */
								PROCOper(LINE_TYPE_ISTRUZIONE,Regs->P == 8 ? "les" : "lds",OPDEF_MODE_REGISTRO16, Regs->P,		// vabbe' :)
									OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,0);
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V->label,2);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16, Regs->D,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
	 							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16, Regs->D+1,
									OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D+1,
										OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
								}
							}
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,
							asPtr ? Regs->P : Regs->D);
#elif MICROCHIP
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
						if(isCond) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO,i+lh);
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,14);
							//finire!
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+3);
							}
#endif
						break;
				  }
				break;
		  default:
				switch(s1) {
				  case 1:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"AND",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs[V],OPDEF_MODE_IMMEDIATO8,0xFF);
#elif Z80
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,MAKEPTRREG(V->label));
//						PROCOper("xor",Regs->DSh,Regs->DSh);     // niente cast
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO,Regs->D);
//						PROCOper("xor",Regs->DSh,Regs->DSh);     // niente cast
#elif MICROCHIP
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
#endif
						break;
				  case 2:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs[V],", ASL #16");
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,", LSR #16");
#elif Z80                                     
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
						if(isCond) {
  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  						PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  						}
#elif I8086
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO16,
							/* MemoryModel asPtr ? Regs->P : Regs->D*/ Regs->D);
#elif MICROCHIP
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
						if(isCond) {
  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  						PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  						}
#endif
						break;
				  case 4:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D,Regs[V],NULL,NULL);
#elif Z80 || MICROCHIP
#elif I8086
						if(CPU86>=3) {
							if(asPtr) {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
								if(isCond)
									PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
								}
							}
						else {
// direi nulla			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,asPtr ? Regs->P : Regs->D,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
							}
#elif MC68000 
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO32,
							asPtr ? Regs->P : Regs->D);
#endif
						break;
			  	}
			  break;
		  }
		if(s < FNGetMemSize(T,S,0/*dim*/,1))
		  PROCCast(T,S,V->type,V->size,-1);
		}      

  if(OutSource) {
//    i=_tcslen(LastOut->s)+_tcslen(V->name)+20;
//    PROCOut(NULL,"\t\t\t\t; ",V->name,NULL,NULL);	
//    LastOut=(struct LINE *)_frealloc(LastOut,i);
//    LastOut->prev->next=LastOut;
//    _tcscat(LastOut->s,"\t\t; ");
    _tcscpy(LastOut->rem,V->name);
    }

fine:    
  return 0;
  }

#if ARCHI
int Ccc::FNIsLshift(long T) {

  if(T<257)
	  return TRUE;
  while(!(T & 1)) {
	  T = T >> 1;
	  }
  if(T<128)
	  return TRUE;
  else 
	  return FALSE;
  }
#elif Z80 || I8086 || MC68000 || MICROCHIP		// potrebbe servire di nuovo per PIC32
#endif


#if MICROCHIP
int Ccc::PROCUseCost(int8_t V, O_TYPE T, O_SIZE S, union STR_LONG *C,bool asPtr,uint8_t lh) {
#else
int Ccc::PROCUseCost(int8_t V, O_TYPE T, O_SIZE S, union STR_LONG *C,bool asPtr) {
#endif
  int i;

  if(V & VALUE_IS_COSTANTE) {
		if(T & VARTYPE_FLOAT) {		// fare, 2025
		  if(V == VALUE_IS_COSTANTEPLUS) {		// 9
#if ARCHI
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif Z80
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif I8086
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,"OFFSET DGROUP:",C->s,NULL);
#elif MC68000
				if((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM)			// gestire
					;
				if(asPtr)
		  		PROCOper(LINE_TYPE_ISTRUZIONE,"lea"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->P);
				else
		  		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
		  	PROCOper(LINE_TYPE_ISTRUZIONE,lh & 1 ? "MOVLW high" : "MOVLW low",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#endif
				}
			else {
#if ARCHI
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif Z80
	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif I8086
	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif MC68000
				PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
#elif MICROCHIP
	  		PROCOper(LINE_TYPE_ISTRUZIONE,lh & 1 ? "MOVLW high" : "MOVLW low",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#endif
				}
      }
		else if(T & VARTYPE_IS_POINTER) { 
		  if(V == VALUE_IS_COSTANTEPLUS) {		// 9
#if ARCHI
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif Z80
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif I8086
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,"OFFSET DGROUP:",C->s,NULL);
#elif MC68000
				if((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM)			// gestire
					;
				if(asPtr)
		  		PROCOper(LINE_TYPE_ISTRUZIONE,"lea"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->P);
				else
		  		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
		  	PROCOper(LINE_TYPE_ISTRUZIONE,lh & 1 ? "MOVLW high" : "MOVLW low",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#endif
				}
			else {
#if ARCHI
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif Z80
	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif I8086
	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#elif MC68000
				if(!C->l)
					PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
				else if(C->l<256)
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
				else
					PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO,asPtr ? Regs->P : Regs->D);
#elif MICROCHIP
	  		PROCOper(LINE_TYPE_ISTRUZIONE,lh & 1 ? "MOVLW high" : "MOVLW low",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,C->l);
#endif
				}
      }
		else {
		  if(V==VALUE_IS_COSTANTE) {			// (integer
		  	T=C->l;
#if ARCHI
		  	if(T>=0) 
					AS="MOV";
			  else 
					AS="MVN";
#elif Z80 || I8086 || MC68000 || MICROCHIP
#endif
#if ARCHI
				int F=FALSE;
				int I=0;
				do {
				  if(FNIsLshift(T)) { 
						sprintf(BS,"#%d",T << I);
						T=0;
						}
				  else
						sprintf(BS,"#%d",(T & 255) << I);
				  if(!F) {
						PROCOper(LINE_TYPE_ISTRUZIONE,AS,OPDEF_MODE_REGISTRO,Regs->D,BS);
						F=TRUE;
						}
				  else 
						PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D,BS,NULL);
				  T >>= 8;
				  I += 8;
				  } while(T);
#elif Z80
			  switch(S) {
			    case 1:
//		      sprintf(BS,"0%xh",T & 0xff);
	          if(!LOBYTE(LOWORD(C->l))) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO_HIGH8,3);     // qui non ha senso, ma chissà...
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,C->l);
	  			  break;
			    case 2:
			    case 4:
	//		      sprintf(BS,"0%xh",T);
			      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,LOWORD(C->l));
				    if(S==4) {
	//				    sprintf(BS,"0%xh",*(((int *)&T)+1));
		  			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO16,HIWORD(C->l));
		  			  }
	  			  break;
			    }
#elif I8086
			  switch(S) {
			    case 1:
	          if(!LOBYTE(LOWORD(C->l))) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO_HIGH8,3);     // qui non ha senso, ma chissà...
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,C->l);
			      break;
			    case 2:
			    case 4:
	          if(!(C->l & 0xffff))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO8,LOWORD(C->l));
				    if(S==4) {
	//				    sprintf(BS,"0%xh",*(((int *)&T)+1));
		  			  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO16,HIWORD(C->l));
		  			  }
	  			  break;
			    }
#elif MC68000
			  switch(S) {
			    case 1:
	          if(!LOBYTE(LOWORD(C->l))) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"clr.b",OPDEF_MODE_REGISTRO,Regs->D);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,LOBYTE(LOWORD(C->l)),OPDEF_MODE_REGISTRO,Regs->D);
			      break;
			    case 2:
	          if(!LOWORD(C->l))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"clr.w",OPDEF_MODE_REGISTRO16,Regs->D);
	          else if(C->l < 256)
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO32,Regs->D);
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,LOWORD(C->l),OPDEF_MODE_REGISTRO16,Regs->D);
			      break;
			    case 4:
	          if(!(C->l & 0xffffffff))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"clr.l",OPDEF_MODE_REGISTRO32,Regs->D);
	          else if(C->l < 256)
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO32,Regs->D);
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO32,Regs->D);
	  			  break;
			    }
#elif MICROCHIP
			  switch(S) {
			    case 1:
//		      sprintf(BS,"0%xh",T & 0xff);
	          if(!LOBYTE(LOWORD(C->l))) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_REGISTRO_HIGH8,3);     // qui non ha senso, ma chissà...
			        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,C->l);
	  			  break;
			    case 2:
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO8,*(((char *)&C->l)+lh));
		        break;
			    case 4:
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
	//		      sprintf(BS,"0%xh",T);
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO8,*(((char *)&C->l)+lh));
	  			  break;
			    }
#endif      
				}
			else {		// array ecc
#if ARCHI
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif Z80
			  switch(S) {
			    case 1:
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
		        break;
			    case 4:
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,2);
			    case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
	  			  break;
			    }
#elif I8086
			  switch(S) {
			    case 1:
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->DSl,"OFFSET DGROUP:",C->s,NULL);
			      break;
			    case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"OFFSET DGROUP:",C->s,NULL);
			      break;
			    case 4:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lds",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,"lds",Regs->D,C->s);
	  			  break;
			    }
#elif MC68000
			  switch(S) {
			    case 1:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO,Regs->P);
			      break;
			    case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO16,Regs->P);
			      break;
			    case 4:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->P);
	  			  break;
			    }
#elif MICROCHIP
			  switch(S) {
			    case 4:
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
		        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,3);
		        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,2);
		        break;
			    case 2:
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
		        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,1);
		        break;
			    case 1:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
		        break;
			    }
#endif
				}
			}
		}
	
  return 0;
  }

int Ccc::FNIsOp(const char *A, int Co) {
  int I,T;
  
  I=0;
  do {
    do {
      if(I==47 /*(sizeof(Op)/sizeof(struct OPERANDO)) non lo prende @#$%*/)
        return 0;
      } while(_tcscmp(Op[I++].s,A));
    I--;
    T=Op[I].p;
    if(T==2) {
      switch(*A) {
        case '*':
        case '&':      
Lcase:        
          if(Co>0) {
            T=0;  
            I++;
            }
          break;
        case '+':		// unary plus e minus
        case '-':
          if(!*(A+1))
            goto Lcase;
          break;
        default:
          break;
        }
      }
    } while(!T);
  return T;
  }
  
/*enum VAR_CLASSES*/ int Ccc::FNIsClass(const char *A) {

  if(!_tcscmp(A,"auto")) {
    return CLASSE_AUTO;
    }
  else if(!_tcscmp(A,"extern")) {
    return CLASSE_EXTERN;
    }
  else if(!_tcscmp(A,"register")) {
    return CLASSE_REGISTER;
    }
  else if(!_tcscmp(A,"static")) {
    return CLASSE_STATIC;
    }       
  else if(!_tcscmp(A,"interrupt")) {
    return CLASSE_INTERRUPT;
    }       
  else if(!_tcscmp(A,"fastcall")) {
    return CLASSE_FASTCALL;
    }       
  else if(!_tcscmp(A,"pascal")) {
    return CLASSE_PASCAL;
    }       
  else if(!_tcscmp(A,"inline")) {
    return CLASSE_INLINE;
    }       
  else if(!_tcscmp(A,"const")) {
    return CLASSE_GLOBAL /*CLASSE_CONST*/;			// v. anche di là, diciamo così
    }       
  else
    return -1;
  }
 
O_SIZE Ccc::FNGetMemSize(O_TYPE T, O_SIZE S, O_DIM d,uint8_t m) {
	int i;

  switch(m) {
    case 2:
			if(T & VARTYPE_ARRAY) {
		    if(T & VARTYPE_2POINTER)
					return getPtrSize(T);
				else
		      return S;  
				}
	    else
	      return S;  
	    break;  
    case 1:
	    if(T & (VARTYPE_IS_POINTER | VARTYPE_FUNC_POINTER))
	      return getPtrSize(T);
	    else
	      return S;  
	    break;  
	  case 0:  
			if(T & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY | VARTYPE_FUNC | VARTYPE_IS_POINTER) /*0x1d0f*/) {
				if(T & VARTYPE_2POINTER) {
		      return getPtrSize(T);
					}
				else {
			    if(T & VARTYPE_2POINTER)
						S=getPtrSize(T);
					i=0;
					if(d) {
						while(d[i] && i<MAX_DIM) {
							S *= d[i];
							i++;
							} 
						}
		      return S;
					}
				}
	    else
	      return S;  
	    break;  
    }
  return 0;  
  }

O_SIZE Ccc::FNGetMemSize(struct VARS *v,uint8_t m) {

	return FNGetMemSize(v->type,v->size,v->dim,m);
	}

O_SIZE Ccc::FNGetArraySize(struct VARS *v) {
	int i=0;
	uint8_t ptrlev=0;
	O_SIZE s;
	
	s=1;		// OVVIAMENTE non posso chiamare GetMemSize qua!
	while(v->dim[i] && i<MAX_DIM) {
		s *= v->dim[i];
		ptrlev++;
		i++;
		} 
	if((v->type & VARTYPE_IS_POINTER) > ptrlev)
		s *= getPtrSize(v->type);
	else
		s *= v->size;

	return s;
	}

O_SIZE Ccc::FNGetSize(uint32_t n) {
	O_SIZE s;

  s=1;;
  if(n & 0xffffff00)
    s=2;
  if(n & 0xffff0000)
    s=4;
	// ampliare con 64bit??
	return s;
	}
