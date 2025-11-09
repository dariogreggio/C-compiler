#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>

struct LINE *Ccc::PROCInserLista(struct LINE *Root, struct LINE *Last, struct LINE *New) {
  struct LINE *A;      
 
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
 
struct LINE *Ccc::PROCDelLista(struct LINE *Root, struct LINE *Last, struct LINE *l) {
  struct LINE *A;
  
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
  GlobalFree(l);
  if(l==Last)
    return A;
  else
    return Last;
  }
 
void Ccc::swap(struct LINE * *l1, struct LINE * *l2) {
  struct LINE *t;
  
  t=*l1;
  *l1=*l2;
  *l2=t;
  }                         
    
int Ccc::PROCOut(enum LINE_TYPE n, const char *A, struct OP_DEF *B, struct OP_DEF *C, const char *R) {
  struct LINE *New;
  char myBuf[256];

  New=(struct LINE *)GlobalAlloc(GPTR,sizeof(struct LINE)); 
//  myLog->print(0,"OUT: %s\n",MyBuf);
  if(!New) {
    PROCError(1001,"Fine memoria OUT");
    }
  New->type=n;  
  if(A) {
	  _tcsncpy(New->opcode,A,sizeof(New->opcode));
		New->opcode[sizeof(New->opcode)-1]=0;
		}
	else
		*New->opcode=0;
  New->s1=*B;
  if(C)
    New->s2=*C;
  else
    New->s2.mode=OPDEF_MODE_NULLA;
  if(R) {
    _tcsncpy(New->rem,R,sizeof(New->rem));
		New->rem[sizeof(New->rem)-1]=0;
		}
  else  
    *New->rem=0;
  if(RootOut) 
    LastOut=PROCInserLista(RootOut,LastOut,New);
  else                                          
    RootOut=LastOut=PROCInserLista(RootOut,LastOut,New);  
    
  *myBuf=0;
	if(debug)
    myLog->print(0,"--------> %s\n",myBuf);  
    
  return 0;
  }
 
int Ccc::PROCOut1(COutputFile *f,const char *A, const char *A1, const char *A2, const char *A3) {
  char myBuf[256];
  
  f->printf("%s",A);  
  if(A1)
    f->printf("%s",A1);  
  if(A2)
    f->printf("%s",A2);  
  if(A3)
    f->printf("%s",A3);  
  f->put('\n');
  
  *myBuf=0;
  if(debug>2)
    myLog->print(0,"-------+> %s\n",myBuf);  
    
  return 0;
  }
                         
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, union SUB_OP_DEF *s1, int o1, 
									 enum OPDEF_MODE m2, union SUB_OP_DEF *s2, int o2, const char *R) {
  struct OP_DEF a,b;
  
  a.mode=m1;
  b.mode=m2;
  if(m1) {
		if(s1)
			a.s=*s1;
		else
			;
	  a.ofs=o1;
    if(m2) {
			if(s2)				// patch xche' a 16 bit funzionava cosi' e qui no!
				b.s=*s2;
			else
				;
		  b.ofs=o2;
		  }
	  }
  PROCOut(n,A,&a,&b,R);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, union SUB_OP_DEF *s1, int o1, const char *R) {
  struct OP_DEF a;
  
  a.mode=m1;
  if(m1) {
		if(s1)
			a.s=*s1;
		else
			;
	  a.ofs=o1;
	  }
  PROCOut(n,A,&a,0,R);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, int s1, enum OPDEF_MODE m2, 
									 union SUB_OP_DEF *s2, int o2) {
  struct OP_DEF a,b;
// usata soprattutto per condizionali    
  a.mode=m1;
  b.mode=m2;
  if(m1) {
	  a.s.n=s1;
	  a.ofs=0;
    if(m2) {
			if(s2)				// patch xche' a 16 bit funzionava cosi' e qui no!
				b.s=*s2;
			else
				;
		  b.ofs=o2;
		  }
	  }
  PROCOut(n,A,&a,&b,NULL);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, union SUB_OP_DEF *s1, int o1, 
									 enum OPDEF_MODE m2, int s2) {
  struct OP_DEF a,b;
    
  a.mode=m1;
  b.mode=m2;
  if(m1) {
		if(s1)
			a.s=*s1;
		else
			;
	  a.ofs=o1;
    if(m2) {
		  b.s.n=s2;
		  b.ofs=0;
		  }
	  }
  PROCOut(n,A,&a,&b,NULL);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, int s1, enum OPDEF_MODE m2, int s2, const char *R) {
  struct OP_DEF a,b;
// ld a,l opp. ld a,100
  a.mode=m1;
  b.mode=m2;
  if(m1) {
	  a.s.n=s1;
	  a.ofs=0;
    if(m2) {
		  b.s.n=s2;
		  b.ofs=0;
		  }
	  }
  PROCOut(n,A,&a,&b,R);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, enum OPDEF_MODE m1, int s1) {
  struct OP_DEF a;
    
  a.mode=m1;
  if(m1) {
	  a.s.n=s1;
	  a.ofs=0;
	  }
  PROCOut(n,A,&a,0,NULL);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, struct OP_DEF *od1, struct OP_DEF *od2) {

  PROCOut(n,A,od1,od2,NULL);
  }
      
void Ccc::PROCOper(enum LINE_TYPE n, const char *A, struct OP_DEF *od1) {

  PROCOut(n,A,od1,NULL,NULL);
  }
      
int Ccc::PROCOutLab(const char *A,const char *A1,const char *A2) {
  struct OP_DEF a;
    
  if(A)
	  _tcscpy(a.s.label,A);
	else		// potrebbe essere comodo, ma in genere se piazzo una LABEL poi la usero' di nuovo (dovrei restituire A...)
		FNGetLabel(a.s.label,2);
	a.mode=OPDEF_MODE_COSTANTE;
  a.ofs=0;
  if(A1 && *A1) {
    _tcscat(a.s.label,A1);
    }
  if(A2 && *A2) {
    _tcscat(a.s.label,A2);
    }
  PROCOper(LINE_TYPE_LABEL,0,a.mode,&a.s,a.ofs);
  
  return 0;
  }
