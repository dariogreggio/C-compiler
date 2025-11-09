#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"
//#include "..\openCdoc.h"

#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

// QUOTE% = TRUE SE NEGLI APICI "
// QUOTE1% = TRUE SE NEGLI APICI '
char *Ccc::FNLO(char *s) {
  register int ch;
  int ch1;
	bool Exit=FALSE,Q=FALSE,Q1=FALSE;
  register char *p;

	p=s;
	*p=0;  
  while((ch=FIn->get()) <= ' ' /*|| ch==EOF*/) {
    if(ch==EOF) {
      return s;
      }
    if(ch=='\n') {
//      getc(FIn);
//      __line__++;
      
//  if(debug)
//    printf("FNLO vuoto\n");
//      FNLO(s);                    // tanto prosegue...
//      return s;
      }
    }

  do {
    if(!Q1) {
      if(ch=='\"') {
        if(!*s) {
          Q=!Q;
          }
        else {
          Exit=TRUE;
          }
        }
      }
    if(!Q) {
      if(ch=='\'') {
        if(!*s) {
          Q1=!Q1;
          }
        else {
          Exit=TRUE;
          }
        }
      }
//  PRINT CH%
    if(Q || Q1) {
			if(ch=='\n')
				PROCError(2001);
			else if(ch=='\\') {
		    *p++='\\'; *p++='x';
				ch=FNGetEscape();
				sprintf(p,"%02x",ch);
				p+=2;
				ch=0;
				}
      }
		else {
      if(!__iscsym(ch))
        Exit=TRUE;
      }
		if(ch)
			*p++=ch;
    *p=0;
    if(!Exit) {
      ch=FIn->get();
//	    if(ch=='\n')
//	      __line__++;
			}
    } while(!Exit);

//  if(debug)
//    printf("FNLO [1]: %s\n",s);
    
  if(*(s+1)) {
    if((ch != '\"') && (ch != '\'')) {
      p--;
      *p=0;
      FIn->unget(ch);		//Seek(-1,CFile::current);
	    if(ch=='\n')
				if(!*s)
					__line__--;
      }
    }
    
  if((*s==ch) && !*(s+1)) {               // creo gli operatori a due caratteri
rifo:  
    ch1=FIn->get();
//    if(ch1=='\n')
//      __line__++;
    switch(ch1) {
      case '&':
      case '+':
      case '-':
      case '<':
//      case '>':                         v. sotto
      case '|':
Op3:      
        if(ch==ch1) {
          *p++=ch;
          *p=0;
          if(ch=='<' || ch=='>')            // op a 3 caratteri
            goto rifo;
          }         
        else
					goto do_unget; 
        break;
      case '*':
        if(ch=='/') {
			    for(;;) {
			      ch=FIn->get();
			      if(ch==EOF) {
			        break;
			        }
/*				    if(ch=='\n')
							__line__++;
*/

			      if(ch=='*') {
			        ch1=FIn->get();
			        if(ch1=='/')
			          break;
							else {
			          FIn->unget(ch1);  
/*								if(ch1=='\n')
									__line__++;*/
								}
			        }
			      } 
//          *s=0;
					FNLO(s);
          }
        else
					goto do_unget; 
        break;
      case '/':
        if(ch=='/') {             // per commenti nuovi
			    while(((ch=FIn->get()) != '\n') && ch!=EOF);
//          *s=0;
					FNLO(s);
          }
        else
					goto do_unget; 
        break;
      case '=':
        if(strchr("!<=>+-*/%&|^",ch)) {
          *p++=ch1;
          *p=0;
          }         
        else
					goto do_unget; 
        break; 
      case '>':
        if(ch=='-' || ch=='>') {
          *p++=ch1;
          *p=0;
          if(ch=='>')
            goto rifo;
          }
        else
					goto do_unget; 
        break;
      default:
do_unget:
	      FIn->unget(ch1);			//FIn->Seek(-1,CFile::current);
		    if(ch1=='\n')
					if(!*s)
						__line__--;
        break;
      }
    }

  if(isdigit(*s)) {               // creo numeri floating
    if((ch1=FIn->get()) == '.') {
      *p++=ch1;
      *p=0;
      FNLO(p);
      }
    else {
      FIn->unget(ch1);			//FIn->Seek(-1,CFile::current);
			if(ch1=='\n')
				if(!*s)
					__line__--;
			}
    }
      
  if(debug) {
    myLog->print(0,"FNLO [2]: %s",s);
		}
    
  return s;    
  }
 
char *Ccc::FNLA(char *s) {
  long l;
  int ol;
  
  ol=__line__;
  l=FIn->GetPosition();
	FIn->SavePosition();
  FNLO(s);
//  FIn->Seek(l,CFile::begin);
	FIn->RestorePosition(l);
//  __line__=ol;
  
  return s;
  }
 
int Ccc::FNGetEscape() {
  int ch;              
  char A[128];
 
  switch(ch=FIn->get()) {
    case 'a':
      return 7;
      break;
    case 'b':
      return 8;
      break;
    case 'f':
      return 12;
      break;
    case 'n': 
      return '\n';
      break;
    case 'r':
      return '\r';
      break;
    case 't':
      return '\t';
      break;
    case 'v':
      return 11;
      break;
    case '\\':
    case '\'':
    case '\"': 
      return ch;
      break;
    case 'e': 
      return 0x1b;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      FIn->unget(ch);			//FIn->Seek(-1,CFile::current);
      ch=FNGetOct(FNLO(A));
      FIn->unget(ch);			//FIn->Seek(-1,CFile::current);
      return ch;
      break;
    case 'X':
    case 'x':
//      getc(FIn);
//      FNLO(A);
      ch=FIn->getHex();
//      fseek(FIn,-1,SEEK_CUR);
      return ch;
      break;
    case '\n':
      PROCError(2017,NULL);
      break;
    default:
      return ch;
      break;
    }
   return 0; 
  }
    
int Ccc::FNGetOct(const char *a) {
  register int t,s=0;
  
  while(*a) {
    t=*a-'0';
    if((t>=0) && (t<8))
      s=s*8+t;
    else
      return s;
    a++;
    }
  return 0;
  }
      
unsigned int Ccc::xtoi(const char *p) {
  unsigned int n=0;
  BYTE c;
	
  while(c=*p++) {
    if(((c>='0') && (c<='9')) || ((c>='A') && (c<='F')) ||((c>='a') && (c<='f')))
			;
		else
      break;
    n <<= 4;
    n |= xtob(c);
    }
	return n;
	}

BYTE Ccc::xtob(char c) {
  
  if(isdigit(c))
    return c-'0';
  else if(toupper(c) >='A' && toupper(c) <='F')
    return toupper(c)-'A'+10;
  }

unsigned int Ccc::btoi(const char *p) {
  unsigned int n=0;
  BYTE c;
	
  while(c=*p++) {
    n <<= 1;
    n |= (c == '1' ? 1 : 0);
    }
	return n;
	}

char *Ccc::itox(char *p,unsigned int n) {
	
  *p = '\0';
	do {
		*(--p) = "0123456789ABCDEF"[n & 0x0F];
		} while ((n >>= 4) > 0);
	return p;
	}

char *Ccc::itob(char *p,unsigned int n) {
	
  *p = '\0';
	do {
		*(--p) = "01"[n & 1];
		} while ((n >>= 1) > 0);
	return p;
	}

bool Ccc::PROCCheck(const char *s) {
  char myBuf[64];

  if(_tcscmp(s,FNLO(myBuf)))
    PROCError(2054,s); 
  return FALSE;
  }

bool Ccc::PROCCheck(char t) {
  char myBuf[64];

  if(*FNLO(myBuf) != t) {
    *myBuf=t;
    *(myBuf+1)=0;
    PROCError(2054,myBuf); 
    }
  return FALSE;
  }

long Ccc::FNGetLine(long p,char *s) {
  int i;
  register int ch;
  long l,l2;
	long ol;
  
  l=FIn->GetPosition();
  FIn->Seek(p,CFile::begin);
	ol=__line__;
	FIn->SavePosition();
rifo:
  do {
    ch=FIn->get();
    } while(/*ch != '\n' &&*/ ch!=EOF && ch<=' ');
  while(ch == '/') {
    ch=FIn->get();
    if(ch == '/') {
			while(ch != '\n' && ch!=EOF)
			  ch=FIn->get();
			goto rifo;  
			}  
    if(ch == '*') {
      do {
			  ch=FIn->get();
			  if(ch=='*') {
				  ch=FIn->get();
				  if(ch=='/') {
//				    ch=getc(FIn);
				    break;
				    }
				  else
				    FIn->unget(ch);
				  }
			  } while(ch!=EOF);
			goto rifo;  
      }
    }
  i=0;
  *s=0;
  while(ch != '\n' && ch!=EOF && i<126) {
    s[i++]=ch;                           
    ch=FIn->get();
    }
  s[i]=0;
//  __line__++;
  
  l2=FIn->GetPosition();
//  FIn->Seek(l,CFile::begin);
	FIn->RestorePosition(l);
//	__line__=ol;
  return l2;
  }

