#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"
#include "..\openCdoc.h"

#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

// QUOTE% = TRUE SE NEGLI APICI "
// QUOTE1% = TRUE SE NEGLI APICI '
char *Ccc::FNLO(char *s) {
  register int ch;
  int ch1,Exit=0,Q=0,Q1=0;
  register char *p;

	p=s;
	*p=0;  
  while((ch=getc(FIn)) <= ' ' /*|| ch==EOF*/) {
    if(ch==EOF) {
      return s;
      }
    if(ch=='\n') {
//      getc(FIn);
      __line__++;
      
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
    if ((Q || Q1) && (ch=='\\')) {
      ch=FNGetEscape();
      }
    if((!Q) && (!Q1)) {
      if(!__iscsym(ch)) {
        Exit=TRUE;
        }
      }        
    *p++=ch;
    *p=0;
    if(!Exit) 
      ch=getc(FIn);
    } while(!Exit);

//  if(debug)
//    printf("FNLO [1]: %s\n",s);
    
  if(*(s+1)) {
    if((ch != '\"') && (ch != '\'')) {
      p--;
      *p=0;
      fseek(FIn,-1,SEEK_CUR);
      }
    }
    
  if((*s==ch) && !*(s+1)) {               // creo gli operatori a due caratteri
rifo:  
    switch(ch1=getc(FIn)) {
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
          fseek(FIn,-1,SEEK_CUR);
        break;
      case '*':
        if(ch=='/') {
			    for(;;) {
			      ch=getc(FIn);
			      if(ch==EOF) {
			        break;
			        }
			      if(ch=='*') {
			        ch1=getc(FIn);
			        if(ch1=='/')
			          break;
			        else
			          ungetc(ch1,FIn);  
			        }
			      } 
//          *s=0;
					FNLO(s);
          }
        else
          fseek(FIn,-1,SEEK_CUR);
        break;
      case '/':
        if(ch=='/') {             // per commenti nuovi
			    while(((ch=getc(FIn)) != '\n') && ch!=EOF);
//          *s=0;
					FNLO(s);
          }
        else
          fseek(FIn,-1,SEEK_CUR);
        break;
      case '=':
        if(strchr("!<=>+-*/%&|^",ch)) {
          *p++=ch1;
          *p=0;
          }         
        else
          fseek(FIn,-1,SEEK_CUR);
        break; 
      case '>':
        if(ch=='-' || ch=='>') {
          *p++=ch1;
          *p=0;
          if(ch=='>')
            goto rifo;
          }
        else
          fseek(FIn,-1,SEEK_CUR);
        break;
      default:
        fseek(FIn,-1,SEEK_CUR);
        break;
      }
    }

  if(isdigit(*s)) {               // creo numeri floating
    if((ch1=getc(FIn)) == '.') {
      *p++=ch1;
      *p=0;
      FNLO(p);
      }
    else
      fseek(FIn,-1,SEEK_CUR);
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
  l=ftell(FIn);
  FNLO(s);
  fseek(FIn,l,SEEK_SET);    
  __line__=ol;
  
  return s;
  }
 
int Ccc::FNGetEscape() {
  int ch;              
  char A[128];
 
  switch(ch=getc(FIn)) {
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
      fseek(FIn,-1,SEEK_CUR);
      ch=FNGetOct(FNLO(A));
      fseek(FIn,-1,SEEK_CUR);
      return ch;
      break;
    case 'X':
    case 'x':
//      getc(FIn);
//      FNLO(A);
      fscanf(FIn,"%x",&ch);
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
    t=*a-48;
    if((t>=0) && (t<8)) {
      s=s*8+t;
      }
    else {
      return s;
      }
    a++;
    }
  return 0;
  }
      
int Ccc::PROCCheck(const char *s) {
  char myBuf[64];

  if(_tcscmp(s,FNLO(myBuf)))
    PROCError(2054,s); 
  return 0;
  }

int Ccc::PROCCheck(char t) {
  char myBuf[64];

  if(*FNLO(myBuf) != t) {
    *myBuf=t;
    *(myBuf+1)=0;
    PROCError(2054,myBuf); 
    }
  return 0;
  }

long Ccc::FNGetLine(long p,char *s) {
  int i;
  register int ch;
  long l,l2;
  
  l=ftell(FIn);
  fseek(FIn,p,SEEK_SET);
rifo:
  do {
    ch=getc(FIn);
    } while(/*ch != '\n' &&*/ ch!=EOF && ch<=' ');
  while(ch == '/') {
    ch=getc(FIn);
    if(ch == '/') {
			while(ch != '\n' && ch!=EOF)
			  ch=getc(FIn);
			goto rifo;  
			}  
    if(ch == '*') {
      do {
			  ch=getc(FIn);
			  if(ch=='*') {
				  ch=getc(FIn);
				  if(ch=='/') {
//				    ch=getc(FIn);
				    break;
				    }
				  else
				    ungetc(ch,FIn);  
				  }
			  } while(ch!=EOF);
			goto rifo;  
      }
    }
  i=0;
  *s=0;
  while(ch != '\n' && ch!=EOF && i<126) {
    s[i++]=ch;                           
    ch=getc(FIn);
    }
  s[i]=0;
//  __line__++;
  
  l2=ftell(FIn);
  fseek(FIn,l,SEEK_SET);
  return l2;
  }

