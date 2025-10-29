#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>


#pragma warning FARE COME IN AS!! 2024

int Ccc::FNGetAritElem(int *OP, char *OS, int *VQ, struct VARS * *V, long *TYPE, int *SIZE, union STR_LONG *VS, int Co) {
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
      fseek(FIn,-1,SEEK_CUR);
      return 0;
    case ')':
      if(!Brack) {
        fseek(FIn,-1,SEEK_CUR);
        return 0;
        }
      else {
        *OP=1;
        *OS=')';
        *(OS+1)=0;
        return 3;
        }
      break;
    case '\'':
      i=_tcslen(TS);
      if(i>6) 
        PROCError(2015,NULL);
      if(i==2) 
        PROCError(2137,NULL);
      T1=0;
      p=TS+1;
      while(*(p+1) /*&& *(p+1)!='\''*/) {
        T1=(T1<<8) | ((unsigned char)*p++);
        }
      *TYPE=0;
      *SIZE=1;
      if(T1 & 0xffffff00)
        *SIZE=2;
      if(T1 & 0xffff0000)
        *SIZE=4;
      VS->l=T1;
      *VQ=VALUE_IS_COSTANTE;
      return 1;
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
            sprintf(T1S+i,"\",%d,\"",*p);
          else                           
            sprintf(T1S+i-1,"%d,\"",*p);
#elif MC68000 
					if(!TipoOut) {
						if(T1S[i-1] != '\"')
							sprintf(T1S+i,"\",%d,\"",*p);
						else                           
							sprintf(T1S+i-1,"%d,\"",*p);
						}
					else {
						*TS='\'';
						if(T1S[i-1] != '\'')
							sprintf(T1S+i,"\',%d,\'",*p);
						else                           
							sprintf(T1S+i-1,"%d,\'",*p);
						}
#endif
          i=_tcslen(T1S);
          }
        else {
          T1S[i++]=*p;
#if MC68000 
					if(TipoOut)
						*T1S='\'';
#endif
					}
        p++;
        }  
#if MC68000 
			if(!TipoOut) {
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
      *TYPE=VARTYPE_POINTER;
#else
      *TYPE=VARTYPE_ROM | VARTYPE_POINTER;			// x default literal strings sono ROM
#endif
      *SIZE=1;                 
      _tcscpy(VS->s,FNAllocCost(T1S,1,*TYPE)->label);
      *VQ=9;
      return 1;
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
      if(strchr(TS,'.')) {
        _tcscpy(T1S,"0 ; FLOAT");
        *TYPE=VARTYPE_FLOAT;
        *SIZE=4;
        _tcscpy(VS->s,FNAllocCost(T1S,3)->label);
        *VQ=9;
        return 1;
        }
      else {
 	      i=_tcslen(TS)-1;
 	      j=toupper(TS[i]);
 	      if(j=='L') {
//        myLog->print(0,"\atrovato un long\n");
	        TS[i]=0;
  	      *SIZE=4;
 	        }
 	      else
 	        *SIZE=INT_SIZE;
// si potrebbe auto-stabilire SIZE... 	        
        if(*TS=='0' && i>1) {
          if(toupper(*(TS+1))=='X')
            sscanf(TS,"%lx",&VS->l);
          else 
            VS->l=FNGetOct(TS+1);
          }
 	      else
          VS->l=atol(TS);
//        myLog->print(0,"numero\a: %ld\n",VS->l);
	      *TYPE=0;
        *VQ=VALUE_IS_COSTANTE;
	      return 1;
        }
      break;
    default:
      *OP=FNIsOp(TS,Co);
      if(*OP) {
        _tcscpy(OS,TS);
        return 3;
        }
      else {
        _tcscpy(VS->s,TS);
        *V=FNCercaVar(TS,FALSE);
        if(!*V) {
          if(*FNLA(MyBuf)=='(') {
            *V=PROCAllocVar(TS,VARTYPE_FUNC,CLASSE_EXTERN,0,INT_SIZE,0,0);
            PROCWarn(4013,TS);
            }
          else {
            PROCError(2065,TS);
						goto fine;
						}
          }
        *SIZE=(*V)->size;
        *TYPE=(*V)->type;
        *VQ=3;
        return 2;
        }
      break;
    }
fine:
  return -1;
  }

int Ccc::PROCGetType(int *s, long *t, struct TAGS **tag, int *dim, long TT) {
  int S,I,J=0;    
  long T=0;
  char AS[64],MyBuf[64];
  long OT;
  struct VARS *V;
  
  OT=ftell(FIn);
  fseek(FIn,TT,SEEK_SET);
  S=-1;
  FNLO(AS);          // ATTENZIONE ALLE PARENTESI
//  *s=0;
//  *t=0l;
//  *tag=*dim=0;
  if(!_tcscmp(AS,"struct") || !_tcscmp(AS,"union")) {
    if(!_tcscmp(AS,"struct"))
      T=VARTYPE_STRUCT;
    else
      T=VARTYPE_UNION;
    *tag=FNAllocAggr();           // legge il nome o ne crea uno, poi è pronto per i membri

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
            I=I*V->dim;
            }
          else {
            if(V->type & VARTYPE_IS_POINTER)
              I=PTR_SIZE;
            }
          if(T & VARTYPE_STRUCT) {
            if(I>= INT_SIZE) {
              *s=((*s+INT_SIZE-1) & -INT_SIZE) +I;
              }
            else
              *s += I;
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
      S=*s;
      *t=T;
      OT=ftell(FIn);
      }
    }
  else {
    if(!_tcscmp(AS,"unsigned")) {
      *t |= VARTYPE_UNSIGNED;
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != -1) {
        FNLO(AS);
        TT=ftell(FIn);
        OT=TT;
        }
      }
#ifdef MICROCHIP
    if(!_tcscmp(AS,"rom")) {
      *t |= VARTYPE_ROM;
      T=*t;
      if(FNIsType(FNLA(MyBuf)) != -1) {
        FNLO(AS);
        TT=ftell(FIn);
        OT=TT;
        }
      }
#endif
    if(!_tcscmp(AS,"__attribute__")) {		// GCC extension generica
			}

	  for(I=0; I<MaxTypes; I++) {
	    if(!_tcscmp(AS,Types[I].s)) {
	      *s=Types[I].size;
	      *t=Types[I].type | *t;
	      *tag=Types[I].tag;
	      *dim=Types[I].dim;
	      break;
	      }
	    }
    S=*s;
    T=*t;
	  }
  if(*s != -2) {
    *s=S;
    fseek(FIn,OT,SEEK_SET);
    J=1;
    while(*FNLA(MyBuf)=='*') {
      FNLO(AS);
      *t=T + J;
      J++;
      }
    T=*t;
    OT=ftell(FIn);
    FNLO(AS);
    switch(*FNLO(MyBuf)) {
      case '[':
        do {
          *t= (T | VARTYPE_ARRAY) | J;
          J++;
          if(*FNLA(MyBuf) != ']') {
            if(!*dim)
              *dim=FNGetConst(MyBuf,0);
            else
              *dim = *dim*FNGetConst(MyBuf,0);
            }
          PROCCheck(']');
          } while(*FNLO(MyBuf)=='[');
        break;
      case '(':
        *t |= VARTYPE_FUNC;
        break;
      case ':':
        *t |= VARTYPE_BITFIELD;
        PROCError(1001,"bitfield");		// 
        break;
      default:
        break;
      }
    fseek(FIn,OT,SEEK_SET);
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
#ifdef MICROCHIP
  if(!_tcscmp(A,"rom"))
    return -5;
#endif
  if(!_tcscmp(A,"__attribute__"))			// microchip extension, GCC
    return -6;
  return -1;
  }

struct VARS *Ccc::FNGetAggr(struct TAGS *Tag, const char *TS, int F, int *o) {  //F=1 per struct, 0 union
  int T1;
  long T;
  struct VARS *V;
  
  *o=0;
  V=Var;
  while(V) {
//    myLog->print(0,"sono su %Fs..",V->name);
    if(V->tag==Tag) {
//    myLog->print(0,("ok\n");
      if(!_tcscmp(V->name,TS)) {
        break;
//      o%=(o%+3)&& &FFFC
	      }   
	    else {
	      T=V->type;
	      T1=V->size;
	      if(T & VARTYPE_ARRAY) {
	        T1 *= V->dim;
	        }
	      else {
	        if(T & VARTYPE_IS_POINTER)
	          T1=PTR_SIZE;
	        }
		    if(F) {
		      if(((!T) && (T1==INT_SIZE)) || ((T & VARTYPE_IS_POINTER) && !(T & VARTYPE_ARRAY))) {
		        *o=((*o+INT_SIZE-1) & -INT_SIZE) +T1;
		        }
		      else {
		        *o += T1;
		        }
		      }  
	      }
      }
    V=V->next;
    }
  return V;
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
  C->next=(struct TAGS *)NULL;
  _tcsncpy(C->label,TS,31);
  C->label[31]=0;
  return C;
  }

struct TAGS *Ccc::FNAllocAggr() {
  int s,Go=0,d;
  long t;
  char MyBuf[64],TS[64],AS[64];
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
      OT=ftell(FIn);
      FNLO(TS);
      do {
        s=INT_SIZE;
        t=0;
        d=0;
        tag=0;
        PROCGetType(&s,&t,&tag,&d,OT);
//        myLog->print(0,"tipo %lx, size %d\n",t,s);
        FNLO(AS);
        V=PROCAllocVar(AS,t,CLASSE_EXTERN,0,s,tag,d);
        V->tag=C;
        while(*FNLA(MyBuf) == '[') {
          while(*FNLO(MyBuf) != ']');
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

int Ccc::StoreVar(struct VARS *V,int RQ, struct VARS *RVar, union STR_LONG *RCost) {
  int S,i;
  long T,l;
  char myBuf[64];
  
  if(debug)
    myLog->print(0,"STOREVAR in VAR# %d\n",V);
	
  S=V->size;
  T=V->type;
  if(T & VARTYPE_ARRAY)
		PROCError(2106,NULL);
  if((T & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY | VARTYPE_FUNC)) && !(T & VARTYPE_IS_POINTER))
		PROCError(2106,NULL);
	S=FNGetMemSize(T,S,1);
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
    Regs->Inc(PTR_SIZE);
  	PROCGetAdd(3,V,0);
    Regs->Dec(PTR_SIZE);
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
				      PROCUseCost(RQ,0,S,RCost);
	//			  		PROCOper(movString,Regs->Accu,RCost->l & 0xff);
				  	  }
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO,Regs->D);
					  if(S>2) {
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2,OPDEF_MODE_REGISTRO,Regs->D+1);
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
			    case 3:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar,0);
					  else {
					    ReadVar(RVar,V->type,V->size,0);
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
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(PTR_SIZE);
  	PROCGetAdd(3,V,0);
    Regs->Dec(PTR_SIZE);
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
				      PROCUseCost(RQ,0,S,RCost);
	//			  		PROCOper(storString,Regs->Accu,RCost->l & 0xff);
				  	  }
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost);
				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO,Regs->D);
					  if(S>2) {
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2,OPDEF_MODE_REGISTRO,Regs->D+1);
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
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_IMMEDIATO8,l);
								break;
							case 2:
							case 4:
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_IMMEDIATO8,l);
								if(S>2) {
	    						PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_IMMEDIATO8,*(((char *)&l)+2));
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
							  if(S>2) {
								  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								  }
							  break;
							}
						break;
					}		
			  break;
			default:
			  switch(RQ) {
			    case 3:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar,0);
					  else {
					    ReadVar(RVar,V->type,V->size,0);
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
#elif MC68000
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(PTR_SIZE);
  	PROCGetAdd(3,V,0);
    Regs->Dec(PTR_SIZE);
    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,2,OPDEF_MODE_IMMEDIATO8,V->size);
    PROCOper(LINE_TYPE_ISTRUZIONE,"rep movsb",OPDEF_MODE_NULLA,0);
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
	  switch(V->classe) {
			case CLASSE_EXTERN:
			case CLASSE_GLOBAL:
			case CLASSE_STATIC:
			  switch(S) {
					case 1:
						if(RQ==VALUE_IS_COSTANTE) {
							l=RCost->l;
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO8,l,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							}
						else
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
					  break;
					case 2:
						if(RQ==VALUE_IS_COSTANTE) {
							l=RCost->l;
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,l,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							}
						else
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
					  break;
					case 4:
						if(RQ==VALUE_IS_COSTANTE) {
							l=RCost->l;
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							}
						else
					  	PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
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
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO8,l,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 2:
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,l,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							case 4:
								l=RCost->l;
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,l,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
								break;
							}
						break;
			    default:
					  switch(S) {
							case 1:               
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							case 2:
	 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							case 4:
	 						  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							  break;
							}
						break;
					}		
			  break;
			case CLASSE_REGISTER:
			  switch(RQ) {
			    case 3:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar,0,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
					  else {
					    ReadVar(RVar,V->type,V->size,0);
					    goto rSVar;  
					    }
			      break;
			    case VALUE_IS_COSTANTE:
					  switch(S) {
							case 1:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO8,RCost->l,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO16,RCost->l,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
							  break;
							case 4:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_IMMEDIATO32,RCost->l,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
							  break;
							}  
						break;
					default:
rSVar:					
					  switch(S) {
							case 1:
			//		  PROCOper(movString,Regs->DSh,0);  // chissefrega del cast
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D+8,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
							  break;
							case 2:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D+8,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
							  break;
							case 4:
							  PROCOper(LINE_TYPE_ISTRUZIONE,storString2,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D+8,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
							  break;
							}
						break;	
					}				
			  break;
			}  
		} 
#elif MICROCHIP
  if(T & VARTYPE_FLOAT) {               // float
    Regs->Inc(PTR_SIZE);
  	PROCGetAdd(3,V,0);
    Regs->Dec(PTR_SIZE);
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
				      PROCUseCost(RQ,0,S,RCost,0);		// FINIRE
	//			  		PROCOper(movString,Regs->Accu,RCost->l & 0xff);
				  	  }
						if(T & VARTYPE_ROM) {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,"TBLWR",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							}
					  break;
					case 2:
					case 4:
				    if(RQ==VALUE_IS_COSTANTE)
				      PROCUseCost(RQ,0,S,RCost,0);		// FINIRE
						if(T & VARTYPE_ROM) {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,"TBLWR",OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else {
				  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				  		PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							}
//				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO,Regs->D);
					  if(S>2) {
  				  	PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2,OPDEF_MODE_REGISTRO,Regs->D+1);
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
			    case 3:               // carico direttamente (var statica) in reg.
			      if(RVar->classe<=CLASSE_STATIC)
					    PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label),OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&RVar,0);
					  else {
					    ReadVar(RVar,V->type,V->size,0,0);		// FINIRE
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
int Ccc::ReadVar(struct VARS *V,long T,int S,int m,int lh) {   // m=0 se norm, 1 se condiz.
#else
int Ccc::ReadVar(struct VARS *V,long T,int S,int m) {   // m=0 se norm, 1 se condiz.
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
  s1=FNGetMemSize(T,S,1);
  s=FNGetMemSize(V,1);
  s1=__min(s,s1);
  if((T & VARTYPE_ARRAY) || ((T & (VARTYPE_UNION | VARTYPE_FLOAT | VARTYPE_STRUCT | VARTYPE_FUNC) /*0x3900*/) && (!(T & VARTYPE_IS_POINTER)))) {
		PROCGetAdd(3,V,0);
		}
  else {
#if MC68000
		char movString2[16];
		_tcscpy(movString2,movString);
		switch(S) {
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
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
#elif Z80
 						if(m)
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
 						else  
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
#elif I8086
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO,Regs->D);
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
 						if(m)
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
 						else  
 						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
#endif
						break;
				  case 2:                    
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDRB",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,1);
						sprintf(MyBuf,",%s, ASL #8",Regs->D1S);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ORR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1,MyBuf);
#elif Z80
						if(m) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
#elif I8086
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
					  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO,Regs->D);
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
						if(m) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,lh);
							}
#endif
						break;
			  	case 4:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LDR",Regs->D,AS);
#elif Z80 
						if(m) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
							}
#elif I8086
						if(m) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
							}
#elif MC68000
						// qua la differenza m tra cond e no non dovrebbe servire
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
						if(m) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,0);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,lh);
//							PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF *)&V,2);
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
   					if(m)
    					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
    				else	
     					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
#elif I8086
						if(m) {
    					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
    					PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,0);
							}
    				else	
     					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
#elif MC68000
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO,Regs->D);
#elif MICROCHIP
						if(m) {
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
						if(m) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+1);
						  }
#elif I8086
						if(m) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  }
#elif MC68000
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO16,Regs->D);
#elif MICROCHIP
						if(lh>1)
							PROCError(1001,"byte sbagliato in int/long");
						if(m) {
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
						if(m) {
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
						if(m) {
 						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->D);
						  }
						else {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i+2);
							}
#elif MC68000
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,i,OPDEF_MODE_REGISTRO32,Regs->D);
#elif MICROCHIP
						if(lh>3)
							PROCError(1001,"byte sbagliato in int/long");
						if(m) {
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
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,*(uint8_t *)V->label);
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
#elif I8086
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
//						PROCOper("xor",Regs->DSh,Regs->DSh);     // niente cast
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO,*(uint8_t *)V->label,OPDEF_MODE_REGISTRO,Regs->D);
//						PROCOper("xor",Regs->DSh,Regs->DSh);     // niente cast
#elif MICROCHIP
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,*(uint8_t *)V->label);
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
						if(m) {
  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  						PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  						}
#elif I8086
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label));
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO16,Regs->D);
#elif MICROCHIP
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,Regs->D);
						if(m) {
  						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  						PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  						}
#endif
						break;
				  case 4:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,Regs->D,Regs[V],NULL,NULL);
#elif Z80 || I8086 || MICROCHIP
#elif MC68000 
						PROCOper(LINE_TYPE_ISTRUZIONE,movString2,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label),OPDEF_MODE_REGISTRO32,Regs->D);
#endif
						break;
			  	}
			  break;
		  }
		if(s < FNGetMemSize(T,S,1))
		  PROCCast(T,S,V->type,V->size);  
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
int Ccc::PROCUseCost(int V, long T, uint8_t S, union STR_LONG *C,int lh) {
#else
int Ccc::PROCUseCost(int V, long T, uint8_t S, union STR_LONG *C) {
#endif
  int i;

  if(V & 8) {
		if(T & (VARTYPE_FLOAT | VARTYPE_IS_POINTER /*0x200f*/)) { 
		  if(V == 9) {
#if ARCHI
			  PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif Z80
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
#elif I8086
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,"OFFSET DGROUP:",C->s,NULL);
#elif MC68000
				if(MemoryModel==0)			// gestire
					;
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"move.l #"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->D);
//		  	PROCOper(LINE_TYPE_ISTRUZIONE,"lea"/*movString*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0,OPDEF_MODE_REGISTRO32,Regs->D);
//	  		LEA ?? ma andrebbe su A0...
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
	  		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_IMMEDIATO16,C->l,OPDEF_MODE_REGISTRO,Regs->D);
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
	          if(!LOBYTE(C->l)) {
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
	          if(!(C->l & 0xff)) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO_HIGH8,3);     // qui non ha senso, ma chissà...
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,C->l);
			      break;
			    case 2:
			    case 4:
	          if(!(C->l & 0xffff))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"xor",Regs->D,OPDEF_MODE_REGISTRO,Regs->D);
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
	          if(!LOBYTE(C->l)) {
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq #0,",OPDEF_MODE_REGISTRO_LOW8,Regs->D);
			        }
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO8,C->l,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
			      break;
			    case 2:
	          if(!LOWORD(C->l))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq #0,",OPDEF_MODE_REGISTRO16,Regs->D);
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_IMMEDIATO16,C->l & 0xffff,OPDEF_MODE_REGISTRO16,Regs->D);
			      break;
			    case 4:
	          if(!(C->l & 0xffffffff))
			        PROCOper(LINE_TYPE_ISTRUZIONE,"moveq #0,",OPDEF_MODE_REGISTRO32,Regs->D);
			      else
			        PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_IMMEDIATO32,C->l,OPDEF_MODE_REGISTRO32,Regs->D);
	  			  break;
			    }
#elif MICROCHIP
			  switch(S) {
			    case 1:
//		      sprintf(BS,"0%xh",T & 0xff);
	          if(!LOBYTE(C->l)) {
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
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->DSl,"OFFSET DGROUP:",C->s,NULL);
			      break;
			    case 2:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,"OFFSET DGROUP:",C->s,NULL);
			      break;
			    case 4:
		        PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF *)C->s,0);
//		        PROCOper(LINE_TYPE_ISTRUZIONE,"lds",Regs->D,C->s);
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
      if(I==46)
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
  else if(!_tcscmp(A,"pascal")) {
    return CLASSE_PASCAL;
    }       
  else if(!_tcscmp(A,"inline")) {
    return CLASSE_INLINE;
    }       
  else
    return -1;
  }
 
uint8_t Ccc::FNGetMemSize(long T, uint8_t S, uint8_t m) {

  switch(m) {
    case 2:
	    if(T & 0xe)
	      return PTR_SIZE;
	    else
	      return S;  
	    break;  
    case 1:
	    if(T & VARTYPE_IS_POINTER)
	      return PTR_SIZE;
	    else
	      return S;  
	    break;  
	  case 0:  
	    if(T & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY | VARTYPE_FUNC | VARTYPE_IS_POINTER) /*0x1d0f*/)
	      return PTR_SIZE;
	    else
	      return S;  
	    break;  
    }
  return 0;  
  }

uint8_t Ccc::FNGetMemSize(struct VARS *v,uint8_t m) {

	return FNGetMemSize(v->type,v->size,m);
	}


