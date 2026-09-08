//#define const		// PATCH! gestire const  ora lo prende ma NON nei parametri funzione
/*int t8;
//short int aa;			//
const  short int aaa;			//const   (ora lo salta ma non va oltre col tipo
const  short int t5; // 
const  short int t6;
const  short int t7; float rf;
int t8;*/

#ifdef Z80 
#define O_APPEND   1     // culo
static char aa=O_APPEND;
#elif MC68000
#define O_APPEND   5     // culo
static char aa=O_APPEND;
#elif GD24032

#define O_APPEND   7     // culo
static char aa=O_APPEND;
#else
#define O_APPEND   157     // culo
static char aa=O_APPEND;

#endif

//static char aa=0;

//#include <fcntl.h>
//#include <conio.h>
#include <ctype.h>
//#include <time.h>

/*int printf(char *,...);
short int strlen(char*);
short int prova(char,int);
void vuota(int);*/

inline unsigned int prova_inline(unsigned int a,int b) {
	unsigned char h=3;
	static char c;
{short int z;
c++;
}
//prova_inline(1,1);  //test ricorsiva!
#ifdef MC68000
_asm {
	and.l d1,d0
	not.l d0
	}
#endif
#ifdef I8086
_asm {
	and ax,dx
	not ax
	}
#endif
#ifdef GD24032
	_asm NAND R0,R1
#endif
a=b+1;
return;
goto pippino;
h=5;
c=b;
pippino:
	ciao();
;
	}

interrupt void __attribute__(naked) piciu() {
int culo=8;
#ifdef MC68000
_asm	move culo,d1
#endif
#ifdef I8086
_asm	lea si,[culo]
#endif
#ifdef GD24032
_asm LEA  R17,[culo]
#endif
	}

int stampa2() {
#ifdef MC68000
_asm {
	move.l d0,a0
	moveq #9,aa
	}
#endif
#ifdef I8086
_asm {
	mov ax,[aa]
	mov di,9
	}
#endif
#ifdef GD24032
_asm {
	MOV R4,[aa]
	MOV R6,9
	}
#endif
}


int  stampa(int n) {
register int t1,t2;
char *p;
	short int i;
	register char ch;
long l=3;

n=20+n;
n=n+20;
_builtin_ei();
/*n=*/vuota(0);
aa=*p;
aa=_builtin_nor(37,45);
aa=_builtin_nand(30,48);
//aa=_builtin_nand(30,n);
aa=_builtin_nand(*p,14);
//aa=_builtin_nand(n+4,11);
aa=_builtin_nand(aa,11);
aa=_builtin_nand(n,11);
aa=_builtin_nand(n<<aa,11);
aa=prova_inline(30,48);
aa=prova_inline(t1,t2);
aa=_builtin_nand(t1+2,t2);
t1=_builtin_nand(t1+2,t2);
n=_builtin_mas(30,49,62);
n=_builtin_getflags();
_builtin_nop();

aa=aa+n;
aa+=n;
aa=aa*n;
n=n*n;
t1=t1*n;


//p=90;
*p=55;
aa=*p++;
aa++;
n+=5;
aa <<= n;
aa = aa >> n;
n= n >> 1;
aa <<= 1;
n<<=2;
aa >>= 9;
aa='\x7'+strlen("aaa");
aa='\x10'-strlen("aaa");
aa=strlen("aaa")-1;
n=O_APPEND;
	goto pippo;
	printf("i=%d\n",n);
	return 0;
pippo:
	;
	}

main() {
/*	short int*/char i;

pippo2:
	prova(i<<aa,8);
	printf("Hello, world!");
pippo3:
	for(i=0; i<10; i++) 
		stampa(i);
	goto pippo2;
	}

