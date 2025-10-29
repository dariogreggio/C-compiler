#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

char *Ccc::FNGetParm(FILE *FI, char *s) {
  int T=0,Go=0,i;
  char TS[128],*A;
  char *p;
                
  p=s;             
  do {
    FNGrab(FI,TS);
	  switch(*TS) {
	    case ')':
	      if(T)
	        T--;
	      else 
	        Go=TRUE;
	      break;
	    case '(':
	      T++;
	      break;
	    case ',':
	      if(!T)
	        Go=TRUE;
	      break;
	    }
	  if(!Go) {        
	    A=TS;
	    while(*A)
	      *p++=*A++;
	    }
	  } while(!Go);
	  
	*p=0;
  return s;
  }

char *Ccc::FNParse(char *A, int *I, char *s) {
  int B=0,Q=0,Go=0,i,ch;
  char *p;
              
  p=s;          
  do {
    (*I)++;
	  ch=*(A+ *I);
	  *p++=ch;
	  if(ch=='\"')
	    Q=!Q;
	  if(!Q) {
	    switch(ch) {
	      case '(':
	        B++;  
	        break;
	      case ')':
	        B--;
	        if(B<0)
	          Go=TRUE;
	        break;
	      case ',':
	        if(!B)
	          Go=TRUE;
	        break;
	      default:
	        break;
	      }
	    }
	  } while(!Go);
  *(p-1)=0;
  return s;            
  }

char *Ccc::FNGrab(FILE *FI, char *s) {
  int Q=0,Go=0,W=0,Q1=0;
  register int ch;
//  char MyBuf[200];
  char *p;

  p=s;
	do {
	  ch=getc(FI);
    if(ch==EOF) {
      ch=0;
      Q=Q1=0;
//      Go=TRUE;
      }
    if(ch==10 || ch==13) {
      Q=Q1=0;
      }
	  *p++=ch;
// PRINT T%
	  if(!Q1)
	    if(ch=='\"')
	      Q=!Q;
	  if(!Q)
	    if(ch=='\'')
	      Q1=!Q1;
	  if(!Q && !Q1) {
      if(__iscsym(ch))
	      W=TRUE;
	    else
	      Go=TRUE;
	    }
	  } while(!Go);
	if(W) {
	  fseek(FI,-1,SEEK_CUR);
	  p--;
	  } 
	*p=0;
	if(!UNDEFD[IfDefs]) {
	  if(PP) {
//	    strcpy(MyBuf,s);
	    FNPreProcess(FI,s);
	    }
	  }
	    
  return s;
  }

char *Ccc::FNGrab1(char *A, int *K, int *I, char *s) {
  int Q=0,Q1=0,Go=0,W=0,i;
  register int ch;
  char *p;
    
  p=s;
  do {             
    (*I)++;
    ch=*(A+ *I);
    if(!Q1)
      if(ch=='\"')
        Q=!Q;
    if(!Q)
      if(ch=='\'')
        Q1=!Q1;
    if(!Q && !Q1) {
      if(__iscsym(ch)) {
        if(!W) {
          W=TRUE;
          *K=*I;
          }
        *p++=ch;
        }
      else {
        if(W)
          Go=TRUE;
        }
      }
    } while(!Go && ch);
  *p=0;
  return s;                             
  }

struct LINE_DEF *Ccc::FNDefined(const char *A) {
  char *I;   
  struct LINE_DEF *T;
  
  T=RootDef;
  while(T) {
    I=strchr(T->s,'(');
    if(I) {              
      if(_tcslen(A) == (I-T->s)) {
        if(!strncmp(T->s,A,I-(T->s))) { 
          return T;
          }  
        }
      }
    else {
      if(!_tcscmp(T->s,A)) {
        return T;
        }
      }
    T=T->next;
//    T=T->next;
    }
  return 0;
  }

int Ccc::PROCDefine(const char *A, const char *B) {    
  struct LINE_DEF *New;
  int i;
  
#pragma warning FARE COME IN AS86 2025!!


  i=_tcslen(A)+_tcslen(B);
//  myLog->print(0,"define %s!%s\n",A,B);
  New=(struct LINE_DEF *)GlobalAlloc(GPTR,i+sizeof(struct LINE_DEF)+4);
  if(!New) {
    PROCError(1001,"Fine memoria DEFINE");
    }
  if(RootDef)
    LastDef=(struct LINE_DEF *)PROCInserLista((struct LINE *)RootDef,(struct LINE *)LastDef,(struct LINE *)New);
  else                                          
    RootDef=LastDef=(struct LINE_DEF *)PROCInserLista((struct LINE *)RootDef,(struct LINE *)LastDef,(struct LINE *)New);  
  strcpy(New->s,A);
  i=_tcslen(New->s);
  strcpy(New->s+i+1,B);
  
  return 0;
  }

char *Ccc::FNGetNextPre(FILE *FI, int F, char *s) {
  char A[200];
  int Go=0,i;
  char *p,*p1;

//  i=0;
  p=s;
  FNGrab(FI,A);
  while(*A == ' ') {
    FNGrab(FI,A);
    }
  do {
    switch(*A) {
      case 13:
      case 10:
      case '\t':
        Go=TRUE;
        break;
      case ' ':
        if(F)
          Go=TRUE;
        else 
          *p++=' ';
        break;
      case '\\':
        getc(FI);
        getc(FI);                       // permette a-capo con backslash
        *A=0;
        break;
      case '/':
        i=getc(FI);
        fseek(FI,-2,SEEK_CUR);
        if(i=='/' || i=='*')
          Go=TRUE;
        else
          goto myDef;
        break;
      default:
myDef:
        p1=A;
        while(*p1)
          *p++=*p1++;
        break;
      }
    if(!Go)
      FNGrab(FI,A);
    } while(!Go);
  fseek(FI,-1,SEEK_CUR);
  *p=0;
  return s;
  }

char *Ccc::FNPreProcess(FILE *FI, char *s) {
  int J1,ch,J,i;
  char *I;
  struct LINE_DEF *p;
  char JS[200],TS[128],T1S[128];
  char MyBuf[200];

  p=FNDefined(s);
  if(p) {
//     myLog->print(0,"FORM da trovare %s\n",s);
    strcpy(s,p->s);
    i=_tcslen(p->s);
//    p=p->next;
    strcpy(JS,p->s+i+1);
	  I=strchr(s,'(');
	  if(I) {
	    i=I-s;
      if(*FNGrab(FI,MyBuf)=='(') {
	      do {
	        J=0;                            
	        FNParse(s,&i,TS);
	        if(debug>1)
						myLog->print(0,"FORM %s\n",TS);
	        ch=*(s+i);
	        if(!*TS) 
	          PROCError(2010);
	        FNGetParm(FI,T1S);
	        if(debug>1)
	          myLog->print(0,"ATT %s\n",T1S);
	        if(!ch)
	          PROCError(2010);
	        do {
	          J1=J;
	          FNGrab1(JS,&J1,&J,MyBuf);
	          if(!_tcscmp(TS,MyBuf)) {
	//             J$=LEFT$(J$,J1%-1)+T1$+MIDS(J$,J%);
	            strcpy(MyBuf,JS+J);
	            strcpy(JS+J1,T1S);
	            strcat(JS,MyBuf);
	            J=J1+_tcslen(T1S);
	            }
	          } while((J < _tcslen(JS)) && J);
	        } while(ch != ')');
	      }
	    else
	      PROCError(2010);
	    }
    strcpy(s,JS);
    }
//  else {
//    strcpy(s,A);
//    }
  return s;
  }

int Ccc::FNLeggiFile(char *F, FILE *FO, signed char level) {
  unsigned char Go=0,First=0;
  unsigned char IfDefs=0,LstIfs=0;            // per gestire le nidificazioni, metto in Lst il livello
  FILE *FI;
  char A[200],B[200];
  struct LINE_DEF *L;

  FI=fopen(FNTrasfNome(F),"rb");
  if(!FI) {
    PROCError(level ? 1024 : 1023,F);
		goto fine;
		}
  strcpy(__file__,F);
  First=TRUE;
  __line__=1;
  while(!feof(FI)) {
    if(debug) {
      myLog->print(0,"Linea: %d\n",__line__);
//      while(!kbhit());
      }
    FNGrab(FI,A);
    if(debug) 
      myLog->print(0,"Grab: %s...\n",A);
    if(*A=='/') {
      if(!UNDEFD[IfDefs])
        fputs(A,FO);
      FNGrab(FI,B);
      if(!UNDEFD[IfDefs])
        fputs(B,FO);
      if(*B == '/') {
        do {
          *A=getc(FI);
  	      if(!UNDEFD[IfDefs])
	          fputc(*A,FO);
          } while(*A && *A!=10 && *A!=13);
        }
      else if(*B == '*') {
        do {
          *A=getc(FI);
rifo:         
          if(!UNDEFD[IfDefs])
	          fputc(*A,FO);
          } while(*A && *A!='*');
        if(*A=='*') {
          *A=getc(FI);
          if(*A != '/')
            goto rifo;
          else {
            if(!UNDEFD[IfDefs])
	            fputc(*A,FO);
	          }  
          }  
        }
      }
    else {
	    if((*A=='#') && First) {
	      FNGrab(FI,A);
	//      myLog->print(0,"... e poi Grab: %s\n",A);
	      if(!_tcscmp(A,"endif")) {
	//              myLog->print(0,"ENDIF: IFS %d\n",IfDefs);
	        if(!IfDefs)
	          PROCError(1020);
	        IfDefs--;
	        if(!UNDEFD[IfDefs] || (IfDefs==(LstIfs-1))) {
		        LstIfs=IfDefs;
						bumpIfs(-1,UNDEFD[IfDefs-1],IfDefs);
		        if(!IfDefs)
		          UNDEFD[IfDefs]=FALSE;
		        }
	        }
	      else if(!_tcscmp(A,"else")) {
	        if(!IfDefs)
	          PROCError(1019);
//          myLog->print(0,"Qui ELSE: UNDEF %d e IFS %d e LST %d\n",UNDEFD[IfDefs],IfDefs,LstIfs);
	        if(!UNDEFD[IfDefs-1] /*|| (IfDefs==LstIfs)*/)
//	          UNDEFD[IfDefs-1] = ! UNDEFD[IfDefs-1];
						bumpIfs(0,!UNDEFD[IfDefs],IfDefs);
	        }
	      else if(!_tcscmp(A,"elif")) {
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B);
		      PP=TRUE;
	        if(!IfDefs)
	          PROCError(1018);
	        if(!UNDEFD[IfDefs-1] /*|| (IfDefs==LstIfs)*/) {
		        LstIfs=IfDefs;
	          if(FNDefined(B))
							bumpIfs(0,!UNDEFD[IfDefs],IfDefs);
	          else 
							bumpIfs(0,!UNDEFD[IfDefs],IfDefs);
						goto ifdef;
	          }  
	        }
	      else if(!_tcscmp(A,"if")) {
//		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B);
//		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
		        LstIfs=IfDefs;
						if(!_tcsncmp(B,"defined",7)) {
							char *B1=B+7;
							while(*B1 && !iscsym(*B1))
								B1++;
							if(atoi(B1) /* patch per MACRO definite che diventano altro! */ || FNDefined(B))
								bumpIfs(1,0,IfDefs);
							else 
								bumpIfs(1,1,IfDefs);
							}
						else {
							if(EVAL(B))
								bumpIfs(1,0,IfDefs);
	//		          UNDEFD[IfDefs]=FALSE;
							else
								bumpIfs(1,1,IfDefs);
							}
//		          UNDEFD[IfDefs]=TRUE;   
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
		        }  
	        }
	      else if(!_tcscmp(A,"ifdef")) {
ifdef:
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B);
		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
		        LstIfs=IfDefs;
	          if(FNDefined(B))
							bumpIfs(1,0,IfDefs);
	          else
							bumpIfs(1,1,IfDefs);
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
	          }  
	        }
	      else if(!_tcscmp(A,"ifndef")) {
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B);
		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
		        LstIfs=IfDefs;
	          if(FNDefined(B))
							bumpIfs(1,1,IfDefs);
	          else 
							bumpIfs(1,0,IfDefs);
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
	          }  
	//          myLog->print(0,"Qui A è %s, B è %s e UNDEF %d e IFS %d\n",A,B,UNDEFD[IfDefs],IfDefs);
	        }
	      else {
	        if(!UNDEFD[IfDefs]) {
			      PP=FALSE;
			      FNGetNextPre(FI,TRUE,B);
			      PP=TRUE;
	          if(!_tcscmp(A,"include")) {
	            switch(*B) {
	              case 34:
	                break;
	              case 60:
	                break;
	              default :
	                PROCError(2012);
	                break;
	              }
	            B[_tcslen(B)-1]=0;
							{int oldLine=__line__;
	            if(!FNLeggiFile(B+1,FO,level+1))
								goto fine;
							__line__=oldLine;
							}
	            }
	          else if(!_tcscmp(A,"define")) {
	            L=FNDefined(B);
	            if(L) {
	              PROCWarn(4005,B);
//	              New=L->next;
	              LastDef=(struct LINE_DEF *)PROCDelLista((struct LINE *)RootDef,(struct LINE *)LastDef,(struct LINE *)L);
//	              LastDef=PROCDelLista(RootDef,New);
	              }
	            FNGetNextPre(FI,FALSE,A);
	            PROCDefine(B,A) ;
	            }
	          else if(!_tcscmp(A,"undef")) {
	            L=FNDefined(B);
	            if(L) {
//	              New=L->next;
	              LastDef=(struct LINE_DEF *)PROCDelLista((struct LINE *)RootDef,(struct LINE *)LastDef,(struct LINE *)L);
//	              LastDef=PROCDelLista(RootDef,New);
	              }
	            else {
	              PROCWarn(2065,B);
	              }
	            }
	          else if(!_tcscmp(A,"pragma")) {
	          // lo gestiamo come comando...
    	        fprintf(FO,"%s %s ",A,B);
	            }
						else if(!_tcscmp(A,"warning")) {
    					fprintf(FO,"%s %s ",A,B);
							}
	          else if(!_tcscmp(A,"line")) {
	            sscanf(B,"%u",&__line__);
	            }
	          else {
	            PROCError(1021,A);
	            }
	          }
	        getc(FI);
	        }
	      }
	    else {
	//       myLog->print(0,"Sto per scrivere: A %s, e UNDEFD %d\n",A,UNDEFD[IfDefs]);
	      if(!UNDEFD[IfDefs] || *A=='\n') {
	        fputs(A,FO);
	        }
		    if(*A=='\n') {
		      First=TRUE;
			    __line__++;
		      }
		    else {
		      if((*A != ' ') && (*A != 9)) {		// isprint
		        First=FALSE;
		        }
		      }
	      }
      }
    }
  fclose(FI);
  if(IfDefs>0)
    PROCError(1022);
//  PRINT LEN A$; ">>>"A$"<<<"
  return TRUE;
fine:
	return FALSE;
  }


void Ccc::bumpIfs(int8_t direction,uint8_t state,uint8_t level) {
	uint8_t i;

	if(direction>0) {
		IfDefs++;
		if(IfDefs>MAX_DEFS)
			PROCError(1017);
		}
	else if(direction<0) {
		if(!IfDefs)
			PROCError(1020);
		IfDefs--;
		}
	else {
		if(!IfDefs)
			PROCError(1020);
		}

  for(i=IfDefs; i<MAX_DEFS; i++)
		UNDEFD[i]=state;
	}

