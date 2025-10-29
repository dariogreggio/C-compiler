#include "stdafx.h"
#include "cc.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern int Optimize;

int Ccc::OPComp(struct OP_DEF far *a, struct OP_DEF far *b) {
  
  if(a->mode != b->mode || a->ofs != b->ofs)
    return 1;
  switch(a->mode & 0x7f) {
    case 0:
      return 1;
      break;
    case 1:
    case 2:
    case 3:
      if(a->s.n != b->s.n)
        return 1;
      break;
    case 4:
    case 5:
    case 6:
      if(a->s.n != b->s.n)
        return 1;
      break;
    case 8:
      if(a->ofs != b->ofs)
        return 1;
      break;
    case 9:
      if(a->s.v != b->s.v)
        return 1;
      break;
    case 10:
      if(_tcscmp(a->s.label,b->s.label))
        return 1;
      break;
    case 16:
      if(a->s.n != b->s.n)
        return 1;
      break;
    }
  return 0;
  }
  
struct LINE far *Ccc::GetNextNoRem(struct LINE far *r) {
  
  while(r) {
    if(r->type)
      return r;
    r=r->next;  
    }
  return 0;  
  }
  
int Ccc::Ottimizza(struct LINE far *r) {
  register int j,i1,j1;
  register enum OPDEF_MODE i;
  int k;
  struct OP_DEF R[5][4];
  struct LINE far *r1,far *r2,far *r3;
  char myBuf[128];

  if(Optimize) {
    r1=r;
    while(r) {
      if(Optimize & 2) {           // common expr
        if(r->type==1 || r->type==2 || r->type==3 || r->type==9) {   // una label o call distrugge tutto
delAll:       
			    for(i1=0; i1<5; i1++)
			      for(j=0; j<4; j++)
			        R[i1][j].mode=OPDEF_MODE_NULLA;
//       puts("cancello");
			    }    
			  else {  
        if(!_tcscmp(r->opcode,"call"))
          goto delAll;
        if(!_tcscmp(r->opcode,"ld")) {
          i=r->s1.mode;
          if(i && (i <= 3)) {     // se è load in registro...
            j=r->s1.s.n;
	          if(!OPComp(&R[j][i],&r->s2)) {
	            r->type=LINE_TYPE_NULLA;          // se c'è già in quel registro, cancello
//	            printf("trovato");
	            }
	          else {
	            k=0;
	            if(i==1 || i==2) {          // solo su 8 bit (per ora?)
						    for(i1=0; i1<5; i1++) {
			  		      if(!OPComp(&R[i1][i],&r->s2)) {
			  		        k=1;          // se quel valore è in un altro registro (dello stesso tipo), lo copio
			  		        printf("registro in registro: %d %d\n",i1,i);
			  		        goto foundReg;
				  		      }
				  		    }
				  		  }  
foundReg:			  		        
            	R[j][i]=r->s2;     // salvo...
//	            printf("salvato per il reg. %d, %d: mode: %x, ofs %d\n",r->s1.s.n,r->s1.mode,r->s2.mode,r->s2.ofs);
	            if(k) {
	              r->s2.mode=i;
	              r->s2.s.n=i1;
	              r->s2.ofs=0;
	              }
							if(i==1 || i==2)
							  R[j][3].mode=OPDEF_MODE_NULLA;						    // registro 8 cancella reg-16
							if(i==3)
							  R[j][1].mode=R[j][2].mode=OPDEF_MODE_NULLA;
					    for(i1=0; i1<5; i1++) {
					      for(j1=0; j1<4; j1++) {
			  		      if(R[i1][j1].mode && R[i1][j1].mode==i && R[i1][j1].s.n==j) {
			  		        // se in un altro registro è salvato questo registro, lo elimino
//			  		        printf("registro %d %d in registro %d %d\a\n",i1,j1,j,i);
								  	R[i1][j1].mode=OPDEF_MODE_NULLA;
										if(j1==1 || j1==2)
										  R[i1][3].mode=OPDEF_MODE_NULLA;						    // registro 8 cancella reg-16
										if(j1==3)
										  R[i1][1].mode=R[i1][2].mode=OPDEF_MODE_NULLA;
			  		        }
			  		      }
			  		    }
	            }
	          }
	        else {  
	          i=r->s2.mode;
            j=r->s2.s.n;
	          if(i && (i <= 3)) {     // se è store da registro...
	            R[j][i]=r->s1;
							if(i==1 || i==2)
							  R[j][3].mode=OPDEF_MODE_NULLA;
							if(i==3)
							  R[j][1].mode=R[j][2].mode=OPDEF_MODE_NULLA;
		          }
		        }  
          }
          }
        if(!_tcscmp(r->opcode,"inc") || !_tcscmp(r->opcode,"dec") || !strncmp(r->opcode,"ad",2) || (*r->opcode=='s') || (*r->opcode=='r') || !_tcscmp(r->opcode,"pop")) {
          i=r->s1.mode;
          j=r->s1.s.n;
          if(i && (i <= 3))     // queste op stroncano il registro
					  R[j][i].mode=OPDEF_MODE_NULLA;
			    for(i1=0; i1<5; i1++) {
			      for(j1=0; j1<4; j1++) {
			        if(R[i1][j1].s.n == j)         // se il registro alterato era usato da un altro...
			          R[i1][j1].mode=OPDEF_MODE_NULLA;            // ...cancello
			        }
			      }  
					  
          }
        }
      if(Optimize & 1) {           // salti
        if(r->type == 8) {
          if(r->s1.mode == 10)
            _tcscpy(myBuf,r->s1.s.label);
          else if(r->s1.mode == 9)
            _tcscpy(myBuf,r->s1.s.v->label);
          else if(r->s2.mode == 10)
            _tcscpy(myBuf,r->s2.s.label);
          else if(r->s2.mode == 9)
            _tcscpy(myBuf,r->s2.s.v->label);
          else {
            printf("ecco: %d, %d\n",r->s1.mode,r->s2.mode);
            PROCError(1001,"ottimizza salti");  
            }
          r2=r1;
          while(r2) {
            if(r2->type==1) {        // codice di LABEL
	            if(!_tcscmp(myBuf,r2->s1.s.label)) {
	              printf("trovata la label del salto %s\n",myBuf);
	              if(r2==r->next) {   // elimino un salto a subito dopo
	                r->type=LINE_TYPE_NULLA;
	                break;
	                }
	              r3=GetNextNoRem(r->next);  
	              if(r2==r3) {   // elimino un salto a subito dopo
	                r->type=LINE_TYPE_NULLA;
	                break;
	                }
	              r2=GetNextNoRem(r2->next);  
	              if(r2->type == 8) {
	                if(r2->s1.mode != 16) {
	                  if(r->s1.mode != 16)
	                    r->s1=r2->s1;
	                  else  
	                    r->s2=r2->s1;
	                  }
	                else {
	                  if(r->s1.mode != 16)
	                    r->s1=r2->s2;
	                  else  
	                    r->s2=r2->s2;
	                  }  
	                break;  
	                }
	              }
	            }  
            r2=r2->next;
            }
          }
        }  
      r=r->next;
      }
    }
    
  return 0;  
  }