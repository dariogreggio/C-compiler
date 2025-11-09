#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <mmsystem.h>

char *CCPreProcessor::FNGetParm(CSourceFile *FI, char *s,bool UNDEFD[]) {
  int T=0;
	bool Go=FALSE;
  char TS[128],*A;
  char *p;
                
  p=s;             
  do {
    FNGrab(FI,TS,UNDEFD);
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

char *CCPreProcessor::FNParse(char *A, int *I, char *s) {
  int B=0;
	char ch;
	bool Go=FALSE,Q=FALSE;
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

char *CCPreProcessor::FNGrab(CSourceFile *FI, char *s, bool UNDEFD[]) {
	bool W=FALSE,Go=FALSE,Q=FALSE,Q1=FALSE;
  register int ch;
//  char MyBuf[200];
  char *p;

  p=s;
	do {
	  ch=FI->get();
    if(ch==EOF) {
      ch=0;
      Q=Q1=0;
      Go=TRUE;
			W=0;
      }
    else if(ch=='\n' /*|| ch==13*/) {
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
		FI->unget(ch);
	  p--;
	  } 
	*p=0;
	if(!UNDEFD[IfDefs]) {
	  if(PP) {
//	    strcpy(MyBuf,s);
	    FNPreProcess(FI,s,UNDEFD);
	    }
	  }
	    
  return s;
  }

char *CCPreProcessor::FNGrab1(char *A, int *K, int *I, char *s) {
	bool W=FALSE,Go=FALSE,Q=FALSE,Q1=FALSE;
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

char *CCPreProcessor::FNGetLine(CSourceFile *FI, char *s) {

	// NON va ancora bene, bisogna gestire i #defined / Grab ecc

	bool Go=FALSE;
  register int ch;
  char *p;

  p=s;
	do {
	  ch=FI->get();
    if(ch==EOF) {
      ch=0;
      Go=TRUE;
      }
    else if(ch=='\n' /*|| ch==13*/) {
      Go=TRUE;
      }
	  *p++=ch;
	  } while(!Go);
	*p=0;
	    
  return s;
  }

struct LINE_DEF *CCPreProcessor::FNDefined(const char *A) {
  char *I;   
  struct LINE_DEF *T;
  
  T=RootDef;
  while(T) {
    I=_tcschr(T->name,'(');
    if(I) {              
      if(_tcslen(A) == (I-T->name)) {
        if(!strncmp(T->name,A,I-(T->name))) { 
          return T;
          }  
        }
      }
    else {
      if(!_tcscmp(T->name,A)) {
        return T;
        }
      }
    T=T->next;
    }
  return 0;
  }

int CCPreProcessor::PROCDefine(const char *A, const char *B) {    
  struct LINE_DEF *New;
	char *p,*apici=NULL;
  int i;
	long l1,l2;
  

	l1=_tcslen(A);

	// dovrebbe spezzare B, se ci sono spazi, ma non dentro apici
	if(p=strstr(B,"##")) {			// concatena stringhe... verificare
		struct LINE_DEF *d1;
		*p=0;
		d1=FNDefined(B);
		if(d1) {
	    l2=_tcslen(d1->name);
			}
		else
			l2=_tcslen(B);

		if(apici=_tcschr(p+2,'\"'))		// PATCH urfida, spezzare meglio i token, tra apici e non!
			*apici=0;

		d1=FNDefined(p+2);
		if(d1) {
	    l2+=_tcslen(d1->text);
			}
		else
			l2+=_tcslen(p+2);
		if(apici)		// lo rimetterò sotto
			l2++;
		*p='#';		// ripristino
		}
	else {
		l2=_tcslen(B);
		}

//  m_Log->print(0,"define %s!%s\n",A,B);
  New=(struct LINE_DEF*)GlobalAlloc(GPTR,sizeof(struct LINE_DEF));
  if(!New) {
    m_Cc->PROCError(1001,"Fine memoria DEFINE");
    }
  if(RootDef)
    LastDef=(struct LINE_DEF *)PROCInserLista((struct LINE_DEF *)RootDef,(struct LINE_DEF *)LastDef,(struct LINE_DEF *)New);
  else                                          
    RootDef=LastDef=(struct LINE_DEF *)PROCInserLista((struct LINE_DEF *)RootDef,(struct LINE_DEF *)LastDef,(struct LINE_DEF *)New);  
  _tcscpy(New->name,A);
	New->text=(char*)GlobalAlloc(GPTR,l2+1);
  _tcscpy(New->text,B);
	New->used=0;
	New->vars=NULL;


	// dovrebbe spezzare B, se ci sono spazi, ma non dentro apici
	if(p=strstr(B,"##")) {			// concatena stringhe... verificare
		struct LINE_DEF *d1;
		*p=0;
		d1=FNDefined(B);
		if(d1) {
			_tcscat(New->text,d1->text);
			}
		else
			_tcscpy(New->text,B);

		if(_tcschr(p+2,'\"'))		// PATCH urfida, spezzare meglio i token, tra apici e non!
			*_tcschr(p+2,'\"')=0;

		d1=FNDefined(p+2);
		if(d1) {
			_tcscat(New->text,d1->text);
			}
		else
			_tcscat(New->text,p+2);
		if(apici)
			_tcscat(New->text,"\"");
		}
	else {
		_tcscpy(New->text,B);
		}
  
  return 1;
  }

char *CCPreProcessor::FNGetNextPre(CSourceFile *FI, bool avoidSpaces, char *s, bool UNDEFD[]) {
  char A[256];
  bool Go=FALSE;
	char ch;
  char *p,*p1;

//  i=0;
  p=s;
  FNGrab(FI,A,UNDEFD);
  while(*A == ' ') {		// isspace ??
    FNGrab(FI,A,UNDEFD);
    }
  do {
    switch(*A) {
//      case 13:
      case '\n':
      case '\t':
        Go=TRUE;
        break;
      case ' ':
        if(avoidSpaces)
          Go=TRUE;
        else 
          *p++=' ';
        break;
      case '\\':
        ch=FI->get();                      // permette a-capo con backslash
				// controllare che sia \n ??
        *A=0;
        break;
      case '/':
        ch=FI->get();
        if(ch=='/' || ch=='*') {
//					FI->Seek(-2,CFile::current);
				  FI->unget(ch);
          Go=TRUE;
					}
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
      FNGrab(FI,A,UNDEFD);
    } while(!Go);
  FI->Seek(-_tcslen(A),CFile::current);
//  FI->Seek(-1,CFile::current);
//  FI->unget(ch);		// NO! vedere quale carattere fu letto x ultimo...

  *p--=0;
	while(*p==' ' || *p=='\t'  /*isspace*/)		
		// se ci sono degli spazi dopo, li tolgo (tipo come quando c'è un commento a seguire
		//	(se avoidSpace era falso
		*p--=0;
  return s;
  }

char *CCPreProcessor::FNPreProcess(CSourceFile *FI, char *s,bool UNDEFD[]) {
  int J1,J,i;
  char ch,*I;
  struct LINE_DEF *p;
  char JS[256],TS[128],T1S[128];
  char MyBuf[256];

  p=FNDefined(s);
  if(p) {
//     m_Log->print(0,"FORM da trovare %s\n",s);
    _tcscpy(s,p->name);
//    p=p->next;
    _tcscpy(JS,p->text);
	  I=_tcschr(s,'(');
	  if(I) {
	    i=I-s;
      if(*FNGrab(FI,MyBuf,UNDEFD)=='(') {
	      do {
	        J=0;                            
	        FNParse(s,&i,TS);
	        if(debug>1)
						m_Log->print(0,"FORM %s\n",TS);
	        ch=*(s+i);
	        FNGetParm(FI,T1S,UNDEFD);
					if(!*TS)  {		// se non ci sono parametri nella definizione macro...
						if(*T1S)
							m_Cc->PROCError(2010);
						}
					if(!*T1S)  {		// e viceversa!
						if(*TS)
							m_Cc->PROCError(2010);
						}
	        if(debug>1)
	          m_Log->print(0,"ATT %s\n",T1S);
	        if(!ch)
	          m_Cc->PROCError(2010);
	        do {
	          J1=J;
	          FNGrab1(JS,&J1,&J,MyBuf);
	          if(!_tcscmp(TS,MyBuf)) {
	//             J$=LEFT$(J$,J1%-1)+T1$+MIDS(J$,J%);
	            _tcscpy(MyBuf,JS+J);
	            _tcscpy(JS+J1,T1S);
							if(*MyBuf)
								_tcscat(JS,MyBuf);
							else {								// questo, quando la macro è chiamata con 0 parm
								_tcscat(JS,")");
								break;
								}
	            J=J1+_tcslen(T1S);
	            }
	          } while((J < _tcslen(JS)) && J);
	        } while(ch != ')');
	      }
	    else
	      m_Cc->PROCError(2010);
	    }
    _tcscpy(s,JS);
    }
//  else {
//    strcpy(s,A);
//    }
  return s;
  }

int CCPreProcessor::FNLeggiFile(char *F, COutputFile *FO, uint8_t level) {
  bool Go=FALSE,First=FALSE;
  int8_t IfDefs=0;            // per gestire le nidificazioni, metto in Lst il livello
  CSourceFile *FI;
  char A[256],B[256],oldfile[256];
  struct LINE_DEF *L;

	FI=new CSourceFile(CSourceFile::FNTrasfNome(F));
  if(!FI) {
    m_Cc->PROCError(level ? 1024 : 1023,F);
		goto fine;
		}
  _tcscpy(oldfile,m_Cc->__file__);
  _tcscpy(m_Cc->__file__,F);
	_tcscpy(filesInfo[level].nomeFile,F);

    if(debug)
      m_Log->print(0,"%u: PreProcess %s\n",timeGetTime(),F);

  First=TRUE;
  m_Cc->__line__=1;
  while(!FI->Eof()) {
    if(debug) {
      m_Log->print(0,"Linea: %d\n",m_Cc->__line__);
//      while(!kbhit());
      }
    FNGrab(FI,A,UNDEFD);
    if(debug) 
      m_Log->print(0,"Grab: %s...\n",A);
    if(*A=='/') {
      if(!UNDEFD[IfDefs])
        FO->print(A);
      FNGrab(FI,B,UNDEFD);
      if(!UNDEFD[IfDefs])
        FO->print(B);
      if(*B == '/') {
gotoEOL:
        do {
          *A=FI->get();
  	      if(!UNDEFD[IfDefs])
	          FO->put(*A);
          } while(*A && *A!='\n'/* && *A!=13*/);
				First=TRUE;
        }
      else if(*B == '*') {
        do {
          *A=FI->get();
rifo:         
          if(!UNDEFD[IfDefs])
	          FO->put(*A);
          } while(*A && *A!='*');
        if(*A=='*') {
          *A=FI->get();
          if(*A != '/')
            goto rifo;
          else {
            if(!UNDEFD[IfDefs])
	            FO->put(*A);
	          }  
          }  
        }
      }
    else {
	    if((*A=='#') && First) {
				// errore 2012 invalid char se segue roba dopo #else ecc
	      FNGrab(FI,A,UNDEFD);
	//      m_Log->print(0,"... e poi Grab: %s\n",A);
	      if(!_tcscmp(A,"endif")) {
	//              m_Log->print(0,"ENDIF: IFS %d\n",IfDefs);
	        if(!IfDefs)
	          m_Cc->PROCError(1020);
	        IfDefs--;
	        if(!UNDEFD[IfDefs]) {
						bumpIfs(UNDEFD,-1,UNDEFD[IfDefs],&IfDefs);
		        if(!IfDefs)
		          UNDEFD[IfDefs]=FALSE;
		        }
	        }
	      else if(!_tcscmp(A,"else")) {
	        if(!IfDefs)
	          m_Cc->PROCError(1019);
//          m_Log->print(0,"Qui ELSE: UNDEF %d e IFS %d e LST %d\n",UNDEFD[IfDefs],IfDefs,LstIfs);
	        if(!UNDEFD[IfDefs-1])
//	          UNDEFD[IfDefs-1] = ! UNDEFD[IfDefs-1];
						bumpIfs(UNDEFD,0,!UNDEFD[IfDefs],&IfDefs);
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B,UNDEFD);
		      PP=TRUE;
	        }
	      else if(!_tcscmp(A,"elif")) {		// GESTIRE espressioni anche qua! defined ecc
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B,UNDEFD);
		      PP=TRUE;
	        if(!IfDefs)
	          m_Cc->PROCError(1018);
	        if(!UNDEFD[IfDefs-1]) {
	          if(FNDefined(B))
							bumpIfs(UNDEFD,0,!UNDEFD[IfDefs],&IfDefs);
	          else 
							bumpIfs(UNDEFD,0,!UNDEFD[IfDefs],&IfDefs);
//						goto ifdef;
	          }  
	        }
	      else if(!_tcscmp(A,"if")) {
					bool bInvert=FALSE;
					uint32_t OL=FI->GetPosition();
//		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B,UNDEFD);
//		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
						char *B1;
						if(*B=='!') {
							bInvert=TRUE;
							B1=B+1;
							while(*B1 && !iscsym(*B1))
								B1++;
				      _tcscpy(B,B1);
//			      FNGetNextPre(FI,TRUE,B,UNDEFD);		// SERVE SPEZZARE ev. operatori come ! attaccato alla parola che segue
							}
						if(!_tcsncmp(B,"defined",7)) {
							B1=B+7;
							while(*B1 && !iscsym(*B1))
								B1++;
							if(B1[_tcslen(B1)-1]==')')
								B1[_tcslen(B1)-1]=0;
							if(atoi(B1) /* patch per MACRO definite che diventano altro! */ || FNDefined(B))
								bumpIfs(UNDEFD,1,bInvert ? TRUE : FALSE,&IfDefs);
							else 
								bumpIfs(UNDEFD,1,bInvert ? FALSE : TRUE,&IfDefs);
							}
						else {

//							FI->Seek(OL,CFile::begin);
//							FNGetLine(FI,B);
// NON va ancora bene, bisogna gestire i #defined anche là, o dentro EVAL

							if(m_Cc->EVAL(B))
								bumpIfs(UNDEFD,1,FALSE,&IfDefs);
	//		          UNDEFD[IfDefs]=FALSE;
							else
								bumpIfs(UNDEFD,1,TRUE,&IfDefs);
							}
//		          UNDEFD[IfDefs]=TRUE;   
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
		        }  
	        }
	      else if(!_tcscmp(A,"ifdef")) {
//ifdef:
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B,UNDEFD);
		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
	          if(FNDefined(B))
							bumpIfs(UNDEFD,1,FALSE,&IfDefs);
	          else
							bumpIfs(UNDEFD,1,TRUE,&IfDefs);
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
	          }  
	        }
	      else if(!_tcscmp(A,"ifndef")) {
		      PP=FALSE;
		      FNGetNextPre(FI,TRUE,B,UNDEFD);
		      PP=TRUE;
	        IfDefs++;
	        if(!UNDEFD[IfDefs-1]) {
	          if(FNDefined(B))
							bumpIfs(UNDEFD,1,TRUE,&IfDefs);
	          else 
							bumpIfs(UNDEFD,1,FALSE,&IfDefs);
/*							    if(!UNDEFD[IfDefs])
										bumpIfs(1,!o1.l.v,IfDefs);
									else
										bumpIfs(1,1,IfDefs);*/
	          }  
	//          m_Log->print(0,"Qui A è %s, B è %s e UNDEF %d e IFS %d\n",A,B,UNDEFD[IfDefs],IfDefs);
	        }
	      else {
	        if(!UNDEFD[IfDefs]) {
			      PP=FALSE;
			      FNGetNextPre(FI,TRUE,B,UNDEFD);
			      PP=TRUE;
	          if(!_tcscmp(A,"include")) {
	            switch(*B) {
	              case '\"':
	                break;
	              case '<':
	                break;
	              default :
	                m_Cc->PROCError(2012);
	                break;
	              }
							if(B[_tcslen(B)-1] != ((*B == '<') ? '>' : '\"'))
								m_Cc->PROCError(2059,B);		// controllo char di chiusura... :)
	            B[_tcslen(B)-1]=0;
							{int oldLine=m_Cc->__line__;
	            if(!FNLeggiFile(B+1,FO,level+1))
								goto fine;
							m_Cc->__line__=oldLine;
							}
	            }
	          else if(!_tcscmp(A,"define")) {
	            L=FNDefined(B);
	            if(L) {
	              m_Cc->PROCWarn(4005,B);
//	              New=L->next;
	              LastDef=(struct LINE_DEF *)PROCDelLista((struct LINE_DEF *)RootDef,(struct LINE_DEF *)LastDef,(struct LINE_DEF *)L);
//	              LastDef=PROCDelLista(RootDef,New);
	              }
	            FNGetNextPre(FI,FALSE/*TRUE NO! gli spazi ci possono essere, ok*/,A,UNDEFD);
	            PROCDefine(B,A);
	            }
	          else if(!_tcscmp(A,"undef")) {
	            L=FNDefined(B);
	            if(L) {
//	              New=L->next;
	              LastDef=(struct LINE_DEF *)PROCDelLista((struct LINE_DEF *)RootDef,(struct LINE_DEF *)LastDef,(struct LINE_DEF *)L);
//	              LastDef=PROCDelLista(RootDef,New);
	              }
	            else {
	              m_Cc->PROCWarn(2065,B);
	              }
	            }
	          else if(!_tcscmp(A,"pragma")) {
	          // lo gestiamo come comando...
    	        FO->printf("%s %s ",A,B);
	            }
						else if(!_tcscmp(A,"warning")) {
    					FO->printf("%s %s ",A,B);
							}
	          else if(!_tcscmp(A,"line")) {
	            sscanf(B,"%u",&m_Cc->__line__);
	            }
/*	          else if(!_tcscmp(A,"#")) {	
// per concatenazione stringhe in macro... NO MA NON PASSA DI QUA!! va gestito dentor #define
    					FO->printf("%s%s ",A,B);		// finire!!
	            }*/
	          else {
	            m_Cc->PROCError(1021,A);
	            }
	          }
					{char ch=FI->get();
	        if(ch != '\n')
						FI->unget(ch);
					}
	        }
	      }
	    else {
	//       m_Log->print(0,"Sto per scrivere: A %s, e UNDEFD %d\n",A,UNDEFD[IfDefs]);
	      if(!UNDEFD[IfDefs] /*|| *A=='\r'*/) {		// butto fuori cmq le righe vuote, per non incasinare il #riga... BEH CAZZATA cmq!
	        FO->print(A);
	        }
		    if(*A=='\n') {		// arrivano così CR con modeText... NO non funziona quindi faccio io ;) v. get()
//					if(First)
//						FO->put('\n');
		      First=TRUE;
			    m_Cc->__line__++;
		      }
		    else {
		      if((*A != ' ') && (*A != '\t')) {		// isprint
		        First=FALSE;
		        }
		      }
	      }
      }
    }
//  FI->Close();
	delete FI; FI=NULL;
  if(IfDefs>0)
    m_Cc->PROCError(1022);
//  PRINT LEN A$; ">>>"A$"<<<"
  _tcscpy(m_Cc->__file__,oldfile);
  return TRUE;
fine:
	return FALSE;
  }


void CCPreProcessor::bumpIfs(bool UNDEFD[],int8_t direction,bool state,int8_t *IfDefs) {
	uint8_t i;

	if(direction>0) {
//		(*IfDefs)++;
		if(*IfDefs>=MAX_DEFS)
			m_Cc->PROCError(1017);
		}
	else if(direction<0) {
		if(*IfDefs<0)
			m_Cc->PROCError(1020);
//		(*IfDefs)--;
		}
	else {
		if(!*IfDefs)
			m_Cc->PROCError(1020);
		}

  for(i=*IfDefs; i<MAX_DEFS; i++)
		UNDEFD[i]=state;
	}


CCPreProcessor::CCPreProcessor(Ccc *p,uint8_t d) : m_Cc(p),debug(d) {
	int i;

	RootDef=LastDef=NULL;
	IfDefs=0;
	for(i=0; i<MAX_DEFS; i++)
		UNDEFD[i]=0;
	}

CCPreProcessor::~CCPreProcessor() {

  while(RootDef) {
    LastDef=RootDef->next;
    GlobalFree(RootDef);
    RootDef=LastDef;
    }
	}


struct LINE_DEF *CCPreProcessor::PROCInserLista(struct LINE_DEF *Root, struct LINE_DEF *Last, struct LINE_DEF *New) {
  struct LINE_DEF *A;      
 
  if(Root) {
    New->next=Last->next;
    Last->next=New;
    New->prev=Last;
    if(New->next) {
      A=New->next;
      A->prev=New;
      }      
    }
  else {
    Root=New;
    Root->next=NULL;
    Root->prev=NULL;
    }
  Last=New;   
   
  return Last;
  }
 
struct LINE_DEF *CCPreProcessor::PROCDelLista(struct LINE_DEF *Root, struct LINE_DEF *Last, struct LINE_DEF *l) {
  struct LINE_DEF *A;
  
  if(l==Root) {
    A=Root->next;
    if(A)
      A->prev=0;
    }
  else {
    A=l->prev;
    A->next=l->next;
    if(l->next) {
      A=l->next;
      A->prev=l->prev;
      }
//    else
    A=l->prev;
    }
/*	PERCHE'?? while(l->next) {
		struct LINE_DEF *l2=l->next;
		GlobalFree(l);
		l=l2;
		}*/
  GlobalFree(l->text);
  GlobalFree(l);
  if(l==Last)
    return A;
  else
    return Last;
  }
 
void CCPreProcessor::swap(struct LINE_DEF * *l1, struct LINE_DEF * *l2) {
  struct LINE_DEF *t;
  
  t=*l1;
  *l1=*l2;
  *l2=t;
  }                         
    

