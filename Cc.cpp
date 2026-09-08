/*  ***************************
	  *   C-COMPILER            *
		*        BY G.Dar         *
		*            6/5/89       *
		*   c(Z80) version on     *
		*           24/5/93       *
		*   i86 version on        *
		*           19/9/94       *
		*   uChip version on      *
		*           21/8/10       *
		*   MC68000 version on    *
		*           22/10/25      *
		*   GD24032 version on    *
		*           24/8/26       *
    Versione Windows: 23/9/1996
		   dedicated to Nadia,,,
    Multi-platform project: 7/9/2001

// nell'8086 ci sono molte cose da finire... v. 68000 2025!
// (2025 ora dovrebbe essere ok se una funzione è vuota, l'epilogo viene messo PRIMA del ret
// se interrupt function vuota, push pop escono sminchiati (che poi si potrebbero omettere, in quel caso
//  se generica funzione vuota, idem
// (messi 2025, verificare tutte mancano i cast impliciti... ossia se si usano 2 variabili diverse si incasina (credo da sempre  MA PUSH PARM è ok!
// MANCA controllo su goto non definite!
// (ho la sensazione che le CONDIZIONALI debbano essere 10 e non 6, ossia GT e LS ecc sia signed che unsigned...
// (ri)fare Size a 32bit! :)
accetta più di un #else per blocco! e non dà errore se #istruzioni inesistenti
prende più operatori di seguito... * / 5
chkptr si potrebbe NON mettere su array... logicamente :) almeno se indice costante
#if deve prendere espressione!
(ok #elif non sembra funzionare... v.skybasic
(gli operatori automatici += ecc NON salvano in puntatori /array! (su var sembrano abb .ok
FASTCALL potrebbe usare anche i registri A su 68000, almeno a3 a4 a5 diciamo
su 68000 moltiplicazione e divisione POSSONO usare indirizzi variabili come operando... ottimizzare! (solo come WORD direi, e byte quindi)
 e idem le operazioni add sub cmp possono usare una variabile (anche 8086 un po' direi) per cui si può ottimizzare
 in pratica è come se fosse autoassign, SPECIE su CMP!
su 68000, se And Or XOR su un singolo bit, si potrebbe usare BCLR BSET BCHG (come in Z80)
se in una somma ci sono più costanti e una var, le costanti NON vengono accorpate se dopo la var... logico: funzia se le metti tra parentesi!

su 8086 a volte usa il reg1 anziché 0 (operazioni ecc): sembrano Inc dove non deve, o u[] usati male
	se ci sono register usati, di e si come puntatori sovrascrivono...

credo che su ARCHI e forse 8086 van riverificate le if/while con variabile o funzione... potrebbe servire una
	mov o test per flag

(ERRORE: in BSS vanno le var non inizializzate, in CONST le non-var, in DATA le var inizializzate
(	BSS va pulito dallo startup-code, DATA va copiato!
(	ma Z80 era all'opposto, verificare

(MIGLIORARE ERRORE SE = ;  O += ;  (manca r-value
CONST come parametro schianta! (su var funziona

(per gestire expr con 2 puntatori, si potrebbe passare a readvar/readd0 un numero al posto di bool asPtr, così se l-value usa A1
variabili register che sono puntatori potrebbero andare in A4 ecc

		**************************/

#include "stdafx.h"
#include "cc.h"
#include "..\OpenC.h"

#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <ctype.h>

struct OPERANDO Ccc::Op[]={
  "(",1,")",1,"[",1,"]",1,".",1,"->",1,      // 6
  "*",2,"&",2,"++",2,"--",2,"~",2,"sizeof",2,"(",2,"!",2,"+",2,"-",2, // 10
  "*",3,"/",3,"%",3, // 3
  "+",4,"-",4,   // 2
  "<<",5,">>",5,   // 2
  "<",6,">",6,"<=",6,">=",6,   // 4
  "==",7,"!=",7,   //2
  "&",8,   // 1
  "^",9,   // 1
  "|",10,  // 1
  "&&",11,  // 1
  "||",12,  // 1
  "?",13,  // 1
  "=",14,"+=",14,"-=",14,"*=",14,"/=",14,"%=",14,">>=",14,"<<=",14,"&=",14,"^=",14,"|=",14,  // 11
	// lascio un buco =15 per poter fare alcune differenze in FNRev
  ",",16,  // 1
  };         
  
struct TIPI Ccc::Types[50]={
  "void",0,0/* servirebbe un tipo diverso...*/,NULL,{0},
  "char",1,VARTYPE_PLAIN_INT,NULL,{0},
  "short",2,VARTYPE_PLAIN_INT,NULL,{0},
  "int",INT_SIZE,VARTYPE_PLAIN_INT,NULL,{0},
  "long",4,VARTYPE_PLAIN_INT,NULL,{0},
  "float",4,VARTYPE_FLOAT,NULL,{0},
  "double",8,VARTYPE_FLOAT,NULL,{0}
  };        

#if ARCHI || Z80 || I8051 || MC68000 || GD24032
#elif I8086
	int CPU86;               // 0 8086, 1 80186, 2 80286, 3 80386, 4 80486, 5 Pentium, 6 Pentium2, 7 Pentium3
#elif MICROCHIP
	int CPUPIC,CPUEXTENDEDMODE;              // 0=12; 1=16; 2=18; 3=24; 4=30/33; 5=32
	int StackLarge;
#endif

const char *movString,*storString,
#if ARCHI
	*loadString,			// qua sono diverse!
#endif
	*jmpString,*jmpShortString,*jmpCondString,*callString,*returnString,
	*incString,*decString,*pushString,*popString;


Ccc::Ccc(const CWnd *v) {

	myOutput=(CWnd *)v;
	Regs=new REGISTRI(this);
	myLog=new CLogFile("c:\\opencSpool.txt");
	}

Ccc::~Ccc() {

	delete myLog;
	delete Regs;
	}

int Ccc::CompilaC(int argc, char **argv) {
  char fpr[256];                 // nome del file prep
  int i,ch;
  char ARGS[128],myBuf[256];
  char *p;
  struct VARS *V;
	uint32_t startTime;

#if 0
	{int j;
		COutputFile f("ctypearr.txt");
		f.printf("__ctype:\n");
			for(i=0; i<255; i+=16) {
	//		f.printf("%04x: ",i);
			f.printf("\tdc.b ");

			for(j=0; j<16; j++) {
				ch = isdigit(i+j) ? 0x4 : 0;
				ch |= isupper(i+j) ? 0x1 : 0;
				ch |= islower(i+j) ? 0x2 : 0;
				ch |= isspace(i+j) ? 0x8 : 0;
				ch |= ispunct(i+j) ? 0x10 : 0;
				ch |= ((i+j)>0 && (i+j)<0x20) /*iscontrol(i+j)*/ ? 0x20 : 0;
//				ch |= isblank(i+j) ? 0x40 : 0;
				ch |= isprint(i+j) ? 0x40 : 0;
//				ch |= ishex(i+j) ? 0x80 : 0;
				ch |= (isdigit(i+j) || (i+j)>='A' && (i+j)<='F' || (i+j)>='a' && (i+j)<='f') /*ishex(i+j)*/ ? 0x80 : 0;
//				f.printf("%02x, ",ch);
				f.printf("$%02x%c ",ch,j==15 ? ' ' : ',');

				}
			f.put('\n');
			}
		}
#endif


	m_CPre=new CCPreProcessor(this,0);

  Reg=Regs->MaxUser;
#if _DEBUG
  Warning=3;
#else
  Warning=1;
#endif
// FLAGS:
  PreProcOnly=FALSE;
  CheckStack=FALSE;
  CheckPointers=FALSE;
  OutSource=FALSE;
  OutList=FALSE;
  OutAsm=FALSE;
  PascalCall=FALSE;
  InLineCall=FALSE;
  SynCheckOnly=FALSE;
  UnsignedChar=FALSE;
#if ARCHI
	StructPacking=4;
#elif Z80
	StructPacking=1;
#elif I8086
//	if(CPU86)
	StructPacking=2;
#elif MC68000
	StructPacking=2;		// per word-align
#elif GD24032
	StructPacking=4;		// per dword-align
#elif MICROCHIP
	StructPacking=1;
#endif
#if MC68000
	MemoryModel= /*MEMORY_MODEL_LARGE |*/ MEMORY_MODEL_ABSOLUTE;		// (large
	TipoOut=TIPO_SPECIALE | TIPO_EMBEDDED;				// modo Easy68K
#elif GD24032
	MemoryModel=0;		// fare poi 64bit addr
	TipoOut=0;
#else
	MemoryModel=0;		// 
	TipoOut=0;
#endif
  Optimize=0;           // bit 0=jump, bit 1=subexpr., 2=inlinecalls, b4=o1 (costanti), b8-9 size - speed
	OptimizeExpr=0;				// verificare...
  MultipleString=TRUE;	// se =0, le stringhe saranno accorpate!
  NoMacro=FALSE;

	InBlock=0;
	TempSize=0;
	UseTemp=0;

  Brack=isRValue=isPtrUsed=inCast=0;
	TempProg=0;
	GlblOut=NULL;

  __STDC__=0;
  debug=0;
	bExit=0;
	MaxTypes=7;		*Types[MaxTypes].s=0;		// e pulisco !
  LABEL=0;
	*NFS=*OUS=0;
	*TextSegm=*DataSegm=0;

#if I8086
	CPU86=0 /*2*/;
#endif
#if MICROCHIP
	CPUPIC=2;
#endif
	Var=LVars=NULL;
	CurrFunc=NULL;
	CurrFuncGotos=NULL;
	Con=LCons=NULL;
  Enums=LEnums=NULL;
	StrTag=LTag=NULL;
	RootIn=NULL;
  RootOut=LastOut=StaticOut=BSSOut=NULL;
	FIn=NULL;
	FPre=NULL;
	FO1=FO2=FO3=FO4=FO5=NULL;
	FObj=FCod=NULL;
	FLst=FErr=NULL;

  Declaring=FuncCalled=SaveFP=ASM=AutoOff=0;

#ifdef _DEBUG
//	debug=3;
#endif

	/*char *zz=0;
	*zz=0;*/

//  try {

  for(i=1; i<argc; i++) {
//AfxMessageBox(argv[i]);
		if(*argv[i]=='-' || *argv[i]=='/') {
		  switch(*(argv[i]+1)) {
				case '?':
				  exit(99);                   // help
				  break;
				case 'D':
				  p=strchr(argv[i],'=');
				  if(p) {
					_tcsncpy(buffer,argv[i]+2,p-argv[i]-2);
					m_CPre->PROCDefine(buffer,p+1);
					}   
				  break;
				case 'd':
				  debug=2;
				  break;
				case 'E':
				  PreProcOnly=1;
				  break;
				case 'F':
				  switch(*(argv[i]+2)) {
						case 'c':
							OutSource=TRUE;
							break;
						case 'a':
							OutAsm=TRUE;         // assembly listing
							break;
						case 'l':
							OutList=TRUE;
							break;
						case 'o':
							_tcscpy(OUS,argv[i]+4);         
							break;
						default:
							goto ukswitch;
							break;
						}
				  break;
				case 'G':
				   switch(*(argv[i]+2)) {
					  case 's':
							CheckStack=FALSE;
							break;
					  case 'e':
							CheckStack=TRUE;
							break;
					  case 'c':
							PascalCall=TRUE;
							break;
					  case 'd':
							PascalCall=FALSE;
							break;
					  case 'f':
							MultipleString=FALSE;
							break;
#if I8086							
					  case '0':
					  case '1':
					  case '2':
					  case '3':
							CPU86=*(argv[i]+2)-'0';
							break;
#endif							
					  default:
							goto ukswitch;
							break;
					  }     
				  break;
				case 'J':
				  UnsignedChar=TRUE;
      		m_CPre->PROCDefine("_CHAR_UNSIGNED","1");
				  break;
				case 'm':		// v. anche /A
				   switch(*(argv[i]+2)) {
					  case 'l':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_LARGE;		// memory model large, code e data >64K
							break;
					  case 'm':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_MEDIUM;		// memory model medium, code < 64K e data > 64K
							break;
					  case 's':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_SMALL;		// memory model small, code e data < 64K
							break;
					  case 'c':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_COMPACT;		// memory model compact, bah simile a small
							break;
					  case 'r':
							MemoryModel |= MEMORY_MODEL_RELATIVE;
							break;
					  case 'a':
							MemoryModel |= MEMORY_MODEL_ABSOLUTE;		// vabbe' :)
							break;
					  default:
							goto ukswitch;
							break;
					  }     
				  break;
				case 'N':
				   switch(*(argv[i]+2)) {
					  case 'T':
							_tcscpy(TextSegm,argv[i]+3);
							break;
					  case 'D':
							_tcscpy(DataSegm,argv[i]+3);
							break;
					  default:
							goto ukswitch;
							break;
					  }     
				  break;
				case 'O':
				  switch(*(argv[i]+2)) {
						case 't':               // speed
						  Optimize|=OPTIMIZE_SIZE;
						  break;
						case 's':               // size
						  Optimize|=OPTIMIZE_SPEED;
						  break;
						case 'i':
						  Optimize|=OPTIMIZE_INLINECALLS;
						  break;
						case 'g':
						  Optimize|=OPTIMIZE_SUBEXPR;
						  break;
						case 'l':         // sarebbe ottimizza loop, noi lo usiamo per i salti
						  Optimize|=OPTIMIZE_JUMP;


//						  Optimize|=OPTIMIZE_SUBEXPR;
// prove 31/8/26


						  break;
						case 'x':
						  Optimize=0xffff;
						  break;
						case '0':
						  Optimize=0x00;
						  break;
						case '1':         // per le costanti...?? usare, gestire!
						  Optimize|=OPTIMIZE_CONST;
						  break;
						case '2':
						  break;
						case '3':
						  Optimize=0xffff;
						  break;
						}   
				  break;
				case 'A':
				   switch(*(argv[i]+2)) {		// v. anche /m
					  case 'T':		// tiny, eventualmente!
					  case 'C':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_COMPACT;		// memory model compact, bah simile a small
							break;
					  case 'S':		// small
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_SMALL;		// memory model small, code e data < 64K
							break;
					  case 'L':		// large
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_LARGE;		// memory model large, code e data >64K
							break;
					  case 'M':
							MemoryModel = (MemoryModel & 0x80) | MEMORY_MODEL_MEDIUM;		// memory model medium, code < 64K e data > 64K
							break;
					  default:
							goto ukswitch;
							break;
					  }     
				  break;
				case 'P':
				  PreProcOnly=2;
				  break;
				case 'S':
				   switch(*(argv[i]+2)) {
					  case 't':
											  // imposta titolo
						break;
					  default:
							goto ukswitch;
						break;
					  }     
				  break;
				case 'u':
				  NoMacro=TRUE;
				  break;
				case 'w':
				  Warning=0;
				  break;
				case 'W':
				  switch(*(argv[i]+2)) {
						case 'X':
						  Warning=-1;
						  break;
						case '1':
						case '2':
						case '3':
						case '4':
						  Warning=*(argv[i]+2) - '0';
						  break;
						default:
						  goto ukswitch;
						  break;
						}     
				  
				  break;
				case 'Z':
				  switch(*(argv[i]+2)) {
					  case 'r':
							CheckPointers=TRUE;
							break;
					  default:
							goto ukswitch;
							break;
					  }     
				  break;
				default:
ukswitch:
				  PROCWarn(4002,argv[i]+1);
				  break;
				}
//		  *argv[i]=0;			// boh perché?? 2025
		  }
		else {
		  if(*argv[i]) { 
			  _tcscpy(NFS,argv[i]);
			  }
		  }
		}
  
	numErrors=numWarnings=0;
//MemoryModel|=MEMORY_MODEL_RELATIVE;		// prova
#if Z80 || MICROCHIP
	if(MemoryModel & MEMORY_MODEL_RELATIVE)
		PROCError(1002,"modello di memoria rilocabile");
#endif

	PROCInit();
  
  if(!*NFS) {
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		_tcscpy(p,"Sintassi: cc <nomefile> [switches]");
		myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		goto fine;
		}
  if(!*OUS)
		_tcscpy(OUS,NFS);
	CSourceFile::AddExt(OUS,"lst");

  _tcscpy(__file__,NFS);
  if(p=strrchr(__file__,'\\'))
    _tcscpy(__name__,p+1);
  else  
    _tcscpy(__name__,__file__);
  if(p=strchr(__name__,'.'))
    *p=0;
  myLog->print(0,"%s - %s\n\n",__date__,__file__);
  InBlock=0;
  CurrFunc=NULL;
	CurrFuncGotos=NULL;  
	Declaring=TRUE;
  FuncCalled=FALSE;
	UseIRQ=UseLMul=UseFloat=FALSE;
  SaveFP=FALSE;
  AutoOff=0;
  m_CPre->PP=TRUE;            // PROCESSA O NO
	__line__=0;

	startTime=timeGetTime();

	if(myOutput) {
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		_tcscpy(p,"Preprocessore...");
		myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		}
  if(PreProcOnly==1) {
		m_CPre->FNLeggiFile(NFS,new COutputFile(stdout),0);
		return 0;
		}
  _tcscpy(fpr,NFS);
  CSourceFile::AddExt(fpr,"i");
  if(!(FPre=new COutputFile(fpr)))
		PROCError(1069,fpr);
	
	m_CPre->setDebugLevel(debug);
  i=m_CPre->FNLeggiFile(NFS,FPre,0);
  delete FPre;	FPre=NULL;
	if(!i)
		goto fine;

  if(PreProcOnly) {
		if(myOutput) {
			char *p=(LPSTR)GlobalAlloc(GPTR,256);
			wsprintf(p,"%s (preprocessato) - %d errori, %d warning (%u mSec)",fpr,numErrors,numWarnings,
				timeGetTime()-startTime);
			myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
			}
		return 0;
		}
	
	if(myOutput) {
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		_tcscpy(p,"Compilatore...");
		myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		}
  _tcscpy(__file__,NFS);
  if(!(FIn=new CSourceFile(fpr)))
		PROCError(1068,fpr);

/*
  if(!(FObj=fopen(OUS,"wb"))) 
		PROCError(1037,OUS);
	AddExt(OUS,"$1");
  if(!(FO1=fopen(OUS,"wb"))) 
		PROCError(1037,OUS);
	AddExt(OUS,"$2");
  if(!(FO2=fopen(OUS,"wb"))) 
		PROCError(1037,OUS);
		*/
	{char tempPath[256];
	GetTempPath(255,tempPath);
	GetTempFileNameA(tempPath,"CCC",0,myBuf);
  if(!(FO1=new COutputFile(myBuf)))				// DATA
		PROCError(1069);
	GetTempFileNameA(tempPath,"CCC",0,myBuf);
  if(!(FO2=new COutputFile(myBuf)))				// BSS
		PROCError(1069);
	GetTempFileNameA(tempPath,"CCC",0,myBuf);
  if(!(FO3=new COutputFile(myBuf)))				// CONST
		PROCError(1069);
	GetTempFileNameA(tempPath,"CCC",0,myBuf);
  if(!(FO4=new COutputFile(myBuf)))				// CODE
		PROCError(1069);
	GetTempFileNameA(tempPath,"CCC",0,myBuf);
  if(!(FO5=new COutputFile(myBuf)))				// EXTERN (per praticità di gestione
		PROCError(1069);
	}
  if(OutList) {
		CSourceFile::AddExt(OUS,"map");
		FLst=new COutputFile(OUS);
    if(!FLst) 
	  	PROCError(1037,OUS);
//		PROCVarList(OUS);
    }
	CSourceFile::AddExt(OUS,"err");
	FErr=new COutputFile(OUS);
  if(!FErr) 
  	PROCError(1037,OUS);
	
  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0);
//  StaticOut=LastOut->prev;
//  BSSOut=StaticOut;
#if ARCHI
#elif Z80
  PROCOper(LINE_TYPE_ISTRUZIONE,"cseg",OPDEF_MODE_NULLA,(union SUB_OP_DEF*)0,0);
  if(*TextSegm)
		PROCOper(LINE_TYPE_ISTRUZIONE,"csect",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&TextSegm,0);
  if(!*DataSegm)
		_tcscpy(DataSegm,"data");
  PROCOut1(FO3,";",NULL);
  PROCOut1(FO3,"\tdsect","\tconst");
  PROCOut1(FO2,";",NULL,NULL,NULL);
  PROCOut1(FO2,"\tdsect","\tbss");
  PROCOut1(FO1,";",NULL);
  PROCOut1(FO1,"\tdsect","\t",DataSegm);
#elif I8086
  if(!*TextSegm)
		_tcscpy(TextSegm,"_TEXT");
	PROCOper(LINE_TYPE_DATA_DEF,TextSegm,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\tSEGMENT",0,NULL);
  if(!*DataSegm)
		_tcscpy(DataSegm,"_DATA");
  PROCOut1(FO3,";",NULL);
  PROCOut1(FO3,"CONST\t","SEGMENT");
  PROCOut1(FO2,";",NULL);
  PROCOut1(FO2,"_BSS\t","SEGMENT");
  PROCOut1(FO1,";",NULL);
  PROCOut1(FO1,DataSegm,"\tSEGMENT");
#elif MC68000
  if(!*TextSegm)
		_tcscpy(TextSegm,"_TEXT");
	if(!(TipoOut & TIPO_SPECIALE))
		PROCOper(LINE_TYPE_DATA_DEF,TextSegm,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\tSEGMENT",0,NULL);
  if(!*DataSegm)
		_tcscpy(DataSegm,"_DATA");
	if(!(TipoOut & TIPO_SPECIALE)) {
		PROCOut1(FO3,";",NULL);
		PROCOut1(FO3,"CONST\t","SEGMENT");
		PROCOut1(FO2,";",NULL);
		PROCOut1(FO2,"_BSS\t","SEGMENT");
		PROCOut1(FO1,";",NULL);
		PROCOut1(FO1,DataSegm,"\tSEGMENT");
		}
	else {
		PROCOut1(FO2,";",NULL);			// patch per 68Kdemo, v. anche alla fine come vengono mandati a ASM
		PROCOut1(FO2,"_BSS_START\n",NULL);
		PROCOut1(FO3,";",NULL);
		PROCOut1(FO3,"\torg","\t$2000 ; const/code (rom)\n");
		PROCOut1(FO3,"_CONST_START\n",NULL);
		PROCOut1(FO1,";",NULL);
		if((MemoryModel & 0xf) <= MEMORY_MODEL_SMALL)
			PROCOut1(FO1,"\torg","\t$0480 ; data(ram)\n");		// ev. futuro :) 
		else
			PROCOut1(FO1,"\torg","\t$10480 ; data(ram)\n");		// v. 68Kdemo e S68
		PROCOut1(FO1,"_DATA_START\n",NULL/*DataSegm*/);
		}
#elif GD24032
  if(!*TextSegm)
		_tcscpy(TextSegm,"_TEXT");
	PROCOper(LINE_TYPE_DATA_DEF,"CSECT",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&TextSegm,0);
  if(!*DataSegm)
		_tcscpy(DataSegm,"_DATA");
	PROCOut1(FO3,";",NULL);
	PROCOut1(FO3,"CONST\t","_CONST");
#if GD24032
		if(MemoryModel & MEMORY_MODEL_RELATIVE) {
			FO3->printf("\n__BaseAbs:\n\n");
			}
#endif
	PROCOut1(FO2,";",NULL);
	PROCOut1(FO2,"BSS\t","_BSS");
	PROCOut1(FO1,";",NULL);
	PROCOut1(FO1,"DSECT\t",DataSegm);
#elif MICROCHIP
  if(!*TextSegm)
		_tcscpy(TextSegm,"CODE");
	PROCOper(LINE_TYPE_DATA_DEF,TextSegm,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\t",0,NULL);
  if(!*DataSegm)
		_tcscpy(DataSegm,"UDATA");
  PROCOut1(FO3,";",NULL);
  PROCOut1(FO3,"ROMDATA\t",NULL);
  PROCOut1(FO2,";",NULL);
  PROCOut1(FO2,"ROMDATA2\t",NULL);
  PROCOut1(FO1,";",NULL);
  PROCOut1(FO1,DataSegm,NULL);
#endif

  FErr->printf("G.Dar compiler v%u.%02u on %s %s\n\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__,__time__);

  __line__=1;


	try {		// NON va e non si pianta su eccezioni in PROCOper ecc... forse dipende dal chiamante, OpenC?
  do {
		MSG msg;

		FNLA(ARGS);
		
		if(debug) 
			myLog->print(0,ARGS);
		  
		if(*ARGS == '{') {
			FNLO(ARGS);
		  PROCBlock();
		  }
		else {
		  if(*ARGS) {
				if(!FNIsStmt()) 
				  PROCIsDecl();
				__line__++;
				}
		  else {
				FNLO(ARGS);
				}
		  } 

		BOOL bMsgAvail=PeekMessage(&msg,NULL,0,0,PM_REMOVE /*| PM_NOYIELD*/);
		// serve per far comparire i messaggi nella finestra OpenC man mano che li posto!
		if(bMsgAvail) {
			if(msg.message == WM_QUIT)		// ovvero usarne uno speciale per fermare la compilazione...
		  	break;
			TranslateMessage(&msg); 	 /* Translates virtual key codes			 */
			DispatchMessage(&msg);		 /* Dispatches message to window			 */
			}

		} while(!FIn->Eof() && !bExit);
		}
	catch(CException e) {
		PROCError(1001,"exception");
		}
  if(InBlock>0)
		PROCError(1004);

//		PROCV("vartmp.map");

	delete FIn; FIn=NULL;
	CFile::Remove(fpr);
	delete m_CPre;

	CSourceFile::AddExt(OUS,"asm");
  if(!(FObj=new COutputFile(OUS)))
		PROCError(1037,OUS);
#if ARCHI
  FObj->printf("REM > %s\n",__file__);
  FObj->printf("REM *** Generated by G.Dar compiler for Archimedes v%u.%02u\n",HIBYTE(__VER__),LOBYTE(__VER__));
#elif Z80
  FObj->printf("; *** Generated by G.Dar Z80 compiler v%u.%02u on %s %s\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__,__time__);
#elif I8086
  FObj->printf("; *** Generated by G.Dar i8086 compiler v%u.%02u on %s %s\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__,__time__);
#elif MC68000
  FObj->printf("; *** Generated by G.Dar MC68000 compiler v%u.%02u on %s %s\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__,__time__);
	//println NON VA!! finire
#elif GD24032
  FObj->printf("; *** Generated by G.Dar GD24032 compiler v%u.%02u on %s %s\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__,__time__);
#elif I8051
  FObj->printf("; *** Generated by G.Dar 8051 compiler v%u.%02u on %s\n\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__);
#elif MICROCHIP
  FObj->printf("; *** Generated by G.Dar Microchip-PIC compiler v%u.%02u on %s\n",HIBYTE(__VER__),LOBYTE(__VER__),__date__);
#endif
#ifdef _DEBUG
#if ARCHI
		FObj->printf("REM [** DEBUG **]\n\n");
#else
		FObj->printf("; [** DEBUG **]\n\n");
#endif
#endif
	if(debug)
#if ARCHI
		FObj->printf("REM (debug level %u)\n\n");
#else
		FObj->printf("; (debug level %u)\n\n");
#endif

#if ARCHI
	FObj->printf("REM Command line: ");
#else
	FObj->printf("; Command line: ");
#endif
  for(i=2; i<argc; i++) {
		FObj->printf("%s; ",argv[i]);
		}
	FObj->putcr(); FObj->putcr();

#if ARCHI
  FObj->printf("REM Name %s\n",__file__);
#elif Z80
  FObj->printf("title \"%s\"\n\n",__file__);
#elif I8086
  FObj->printf("\tTITLE\t%s\n",__file__);
  FObj->printf("\tNAME\t%s\n",__name__);
	FObj->printf(";\tModello memoria: %s/%s\n\n",
		(MemoryModel & 0xf) == MEMORY_MODEL_SMALL ? "piccolo" :
		((MemoryModel & 0xf) == MEMORY_MODEL_LARGE ? "grande" : 
		(((MemoryModel & 0xf) == MEMORY_MODEL_COMPACT ? "compatto" : 
		((((MemoryModel & 0xf) == MEMORY_MODEL_MEDIUM ? "medio" : "n/a")))))),
		MemoryModel & MEMORY_MODEL_RELATIVE ? "PC-relativo" : "assoluto");
  if(CPU86)
    FObj->printf("\t.%c86\n",CPU86+'0');
  FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n",TextSegm);
  FObj->printf("%s\tENDS\n",TextSegm);
  FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n",DataSegm);
  FObj->printf("%s\tENDS\n",DataSegm);
  FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n","CONST");
  FObj->printf("%s\tENDS\n","CONST");
  FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n","_BSS");
  FObj->printf("%s\tENDS\n","_BSS");
  FObj->printf("DGROUP\tGROUP CONST, _BSS, _DATA\n");
  FObj->printf("\tASSUME CS: %s, DS: DGROUP, SS: DGROUP\n\n",TextSegm);
#elif MC68000
	if(!(TipoOut & TIPO_SPECIALE)) {
	  FObj->printf("\tTITLE\t%s\n",__file__);
		FObj->printf("\tNAME\t%s\n",__name__);
		FObj->printf(";\tFile assembler in uscita: standard\n");
		FObj->printf(";\tModello memoria: %s/%s\n\n",
			(MemoryModel & 0xf) == MEMORY_MODEL_SMALL ? "piccolo" :
			((MemoryModel & 0xf) == MEMORY_MODEL_LARGE ? "grande" : 
			(((MemoryModel & 0xf) == MEMORY_MODEL_COMPACT ? "compatto" : 
			((((MemoryModel & 0xf) == MEMORY_MODEL_MEDIUM ? "medio" : "n/a")))))),
			MemoryModel & MEMORY_MODEL_RELATIVE ? "PC-relativo" : "assoluto");
		FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n",TextSegm);
		FObj->printf("%s\tENDS\n",TextSegm);
		FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n",DataSegm);
		FObj->printf("%s\tENDS\n",DataSegm);
		FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n","CONST");
		FObj->printf("%s\tENDS\n","CONST");
		FObj->printf("%s\tSEGMENT WORD PUBLIC 'CODE'\n","_BSS");
		FObj->printf("%s\tENDS\n","_BSS");
		FObj->printf("DGROUP\tGROUP CONST, _BSS, _DATA\n\n");
		}
	else {
	  FObj->printf("* TITLE\t%s\n",__file__);
		FObj->printf("* NAME\t%s\n",__name__);
		FObj->printf("* File assembler in uscita: Easy68K/assoluto\n");
		FObj->printf("* Modello memoria: %s/%s\n\n",
			(MemoryModel & 0xf) == MEMORY_MODEL_SMALL ? "piccolo" :
			((MemoryModel & 0xf) == MEMORY_MODEL_LARGE ? "grande" : 
			(((MemoryModel & 0xf) == MEMORY_MODEL_COMPACT ? "compatto" : 
			((((MemoryModel & 0xf) == MEMORY_MODEL_MEDIUM ? "medio" : "n/a")))))),
			MemoryModel & MEMORY_MODEL_RELATIVE ? "PC-relativo" : "assoluto");
		FObj->printf("\tjmp _main\n");		// pratico per le prove! tanto poi c'è la LIB e CRT startup
		}
#elif GD24032
	FObj->printf("\tTITLE\t%s\n",__file__);
	FObj->printf("\tNAME\t%s\n",__name__);
	FObj->printf(";\tModello memoria: %s/%s\n\n",
		(MemoryModel & 0xf) == MEMORY_MODEL_SMALL ? "piccolo" :
		((MemoryModel & 0xf) == MEMORY_MODEL_LARGE ? "grande" : 
		(((MemoryModel & 0xf) == MEMORY_MODEL_COMPACT ? "compatto" : 
		((((MemoryModel & 0xf) == MEMORY_MODEL_MEDIUM ? "medio" : "n/a")))))),
		MemoryModel & MEMORY_MODEL_RELATIVE ? "PC-relativo" : "assoluto");

// magari poi	, v.sopra 8086  FObj->printf("DGROUP\tGROUP CONST, _BSS, _DATA\n\n");
#elif MICROCHIP
  FObj->printf("TITLE \"%s\"\n\n",__file__);
  FObj->printf("\tLIST P=%s,F=INHX32\n","18F4620");			// rendere modificabile!
  FObj->printf("\t#include <P18F4620.INC>\n\n");
  FObj->printf("RESET_VECTOR	CODE	0x%04u\n",0x0000);
  FObj->printf("\tGOTO __startup\n\n");

  if(UseIRQ) {
		FO5->printf("HIGH_INT_VECTOR	CODE	0x%04u\n",0x0008);
		FO5->printf("\tBRA	__HighInt\n\n");
		FO5->printf("LOW_INT_VECTOR	CODE	0x%04u\n",0x0018);
		FO5->printf("\tBRA	__LowInt\n\n");

		// aggiungere/usare ultime direttive LN 2024 25!

    }
#endif

#if MC68000
	if(MemoryModel & MEMORY_MODEL_RELATIVE) {
		FObj->printf("\tOFFSET 0\n");		// per easy68k, vedere se serve
		FO1->printf("__BaseAbs:\n\n");
		}
#endif
  V=Var;
  while(V) {
		if(!V->tag) {
			if(V->classe==CLASSE_GLOBAL) {
				if(!(V->type & (VARTYPE_FUNC_BODY | VARTYPE_FUNC))) {
#if ARCHI
#elif Z80
				  FO5->printf("public\t_%s\n",V->name);
#elif I8086
			  	FO5->printf("PUBLIC\t_%s\n",V->name);
#elif MC68000
					if(!(TipoOut & TIPO_SPECIALE))
						FO5->printf("public\t_%s\n",V->name);
#elif GD24032
					FO5->printf("PUBLIC\t_%s\n",V->name);
#elif MICROCHIP
			  	FO5->printf("GLOBAL\t_%s\n",V->name);
#endif
			  	}
			  }	
			}  
		V=V->next;  
		}
	// in effetti MSVC mette le extern in cima e le public nel code... ma vabbe'
  FO5->putcr();

  V=Var;
  while(V) {
		if(!V->tag) {
			if((V->classe == CLASSE_EXTERN && (!(V->type & VARTYPE_FUNC) || (V->type & VARTYPE_FUNC_USED))) || 
				(V->classe == CLASSE_EXTERN && !(V->type & VARTYPE_FUNC_POINTER)) || 
				(V->classe==CLASSE_GLOBAL && ((V->type & (VARTYPE_FUNC_BODY | VARTYPE_FUNC_USED | VARTYPE_FUNC)) == (VARTYPE_FUNC | VARTYPE_FUNC_USED)))) {
#if ARCHI
#elif Z80
			  FO5->printf("extrn\t%s\n",V->label);
#elif I8086
			  FO5->printf("EXTRN\t%c%s:NEAR\n",(PascalCall || (V->modif & FUNC_MODIF_PASCAL)) ? 0 : '_',V->name);
#elif MC68000
				if(!(TipoOut & TIPO_SPECIALE))
				  FO5->printf("extrn\t%c%s\n",(PascalCall || (V->modif & FUNC_MODIF_PASCAL)) ? 0 : '_',V->name);
				else {
					FO5->printf("%c%s: %s\t; extern\n",(PascalCall || (V->modif & FUNC_MODIF_PASCAL)) ? 0 : '_',V->name,
						(V->type & VARTYPE_FUNC) ? (!(V->type & VARTYPE_FUNC_POINTER) ? "rts" : "") : "");	
							// mmmm chemmerda, mi servono per testare/assemblare in Easy68k, poi togliere con le lib S68; anche lmul CLargs ecc
					}
#elif GD24032
			  FO5->printf("EXTRN\t%c%s\n",(PascalCall || (V->modif & FUNC_MODIF_PASCAL)) ? 0 : '_',V->name);
#elif MICROCHIP
			  FO5->printf("EXTERN\t%s\n",V->label);
#endif
			  }
			}  
skippa_var:
		V=V->next;  
		}

#if ARCHI
#elif Z80
//  fputs("extrn\t__stktop\n",FO5);
#elif I8086
  if(UseLMul) {
    FO5->printf("EXTRN\t__lmul:NEAR\n");
    FO5->printf("EXTRN\t__ldiv:NEAR\n");
    }
  if(UseFloat) {
    FO5->printf("EXTRN\t__fload:NEAR\n");
    FO5->printf("EXTRN\t__fstore:NEAR\n");
    FO5->printf("EXTRN\t__fcvti:NEAR\n");
    FO5->printf("EXTRN\t__fcvtl:NEAR\n");
    FO5->printf("EXTRN\t__fadd:NEAR\n");
    FO5->printf("EXTRN\t__fsub:NEAR\n");
    FO5->printf("EXTRN\t__fcmp:NEAR\n");
    FO5->printf("EXTRN\t__fneg:NEAR\n");
    FO5->printf("EXTRN\t__fmul:NEAR\n");
    FO5->printf("EXTRN\t__fdiv:NEAR\n");
    FO5->printf("EXTRN\t__0\n");		// 0x00000000
    FO5->printf("EXTRN\t__1\n");		// 0x3F800000
    }
	if(UseTemp) {
		FO5->printf("EXTERN\t__tmpdata\n\n");
		}
#elif MC68000
  if(UseLMul) {
//		FO5->printf("__BaseAbs:\n");
		if(!(TipoOut & TIPO_SPECIALE)) {
			FO5->printf("extrn\t__lmul\n");
			FO5->printf("extrn\t__ldiv\n");
			}
		else {
			FO5->printf("__lmul: rts\n");
			FO5->printf("__ldiv: rts\n");
			}
    }
  if(UseFloat) {		// https://baseconvert.com/ieee-754-floating-point
		if(!(TipoOut & TIPO_SPECIALE)) {
			FO5->printf("extrn\t__fload\n");
			FO5->printf("extrn\t__fstore\n");
			FO5->printf("extrn\t__fcvti\n");
			FO5->printf("extrn\t__fcvtl\n");
			FO5->printf("extrn\t__fadd\n");
			FO5->printf("extrn\t__fsub\n");
			FO5->printf("extrn\t__fcmp\n");
			FO5->printf("extrn\t__fneg\n");
			FO5->printf("extrn\t__fmul\n");
			FO5->printf("extrn\t__fdiv\n");
			FO5->printf("extrn\t__0\n");		// 0x00000000
			FO5->printf("extrn\t__1\n");		// 0x3F800000
			}
		else {
			/* no qui no
			FO5->printf("__fload: rts\n");
			FO5->printf("__fstore: rts\n");
			FO5->printf("__fcvti: rts\n");
			FO5->printf("__fcvtl: rts\n");
			FO5->printf("__fadd: rts\n");
			FO5->printf("__fsub: rts\n");
			FO5->printf("__fcmp: rts\n");
			FO5->printf("__fneg: rts\n");
			FO5->printf("__fdiv: rts\n");
			FO5->printf("__fmul: rts\n");
			*/
			FO5->printf("__0:\tdc.l\t$00000000\n");		// 0x00000000
			FO5->printf("__1:\tdc.l\t$3F800000\n");		// 0x3F800000
			}
    }
	if(UseTemp) {
		if(!(TipoOut & TIPO_SPECIALE)) {
			FO1->printf("extrn\t__tmpdata\n\n");
			}
		else {
			FO1->printf("\tds.w 0\n__tmpdata: \n\n");
			}
		}
  if(FNCercaVar("_CLArgs",0)) {
		if(!(TipoOut & TIPO_SPECIALE)) {
			FO5->printf("__CLArgs: rts\n");		// bah vabbe' :)
			}
		}
#elif GD24032
  FO5->printf("EXTRN\t__stktop\n");
	// ev. per 64bit
	if(UseTemp) {
		FO5->printf("EXTRN\t__tmpdata\n");
		}
#elif MICROCHIP
  FO5->printf("EXTERN\t__stktop\n");
  if(UseLMul) {
    FO5->printf("EXTERN\t__lmul\n");
    FO5->printf("EXTERN\t__ldiv\n");
    }
  if(UseFloat) {
		FO5->printf("EXTERN\t__fmul\n");
    FO5->printf("EXTERN\t__fdiv\n");
    }
	if(UseTemp) {
		FO5->printf("EXTERN\t__tmpdata\n");
		}
#endif

  FO1->Seek(0l,CFile::begin);  
  FO2->Seek(0l,CFile::begin);  
  FO3->Seek(0l,CFile::begin);  
  FO4->Seek(0l,CFile::begin);
  FO5->Seek(0l,CFile::begin);

	if(!(TipoOut & TIPO_SPECIALE)) {	// sarebbe per MC68000 ma in effetti può valere sempre, tipo se MemoryModel =COMPACT o simile
		while((ch=FO5->get()) != EOF)			// extern ecc
			FObj->put(ch);
		while((ch=FO3->get()) != EOF)			// prima BSS
			FObj->put(ch);
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n","CONST");
#elif MC68000
		FObj->printf("%s\tENDS\n","CONST");
#elif GD24032
		FObj->printf("%s\tENDS\n","CONST");
#elif MICROCHIP
#endif
		while((ch=FO2->get()) != EOF)			// poi CONST
			FObj->put(ch);
		if(TipoOut & TIPO_EMBEDDED) {
			CString S;
			FObj->printf("\n__init_data:\n",NULL);		// PUO' essere dispari, tanto la copia la fa a byte!
		  FO1->Seek(0l,CFile::begin);  
			while(!FO1->eof()) {			// copio DATA in CONST
//				FO1->read(myBuf);
				FO1->ReadString(S);
				if(p=strchr((LPCTSTR)S,'\t')) {		// porcata ma ok! tolgo il nome var e lascio il dato
					FObj->println(p);
					}
				}
			FObj->printf("\tALIGN 2\n",NULL);		// serve riallineare, per sicurezza!
		  FO1->Seek(0l,CFile::begin);  
			}
		else		// bah?!
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n","_BSS");
#elif MC68000
		FObj->printf("%s\tENDS\n","_BSS");
#elif GD24032
		FObj->printf("%s\tENDS\n","BSS");
#elif MICROCHIP
#endif
		while((ch=FO1->get()) != EOF)			// quindi DATA
			FObj->put(ch);
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n",DataSegm);
#elif MC68000
		FObj->printf("%s\tENDS\n",DataSegm);
#elif GD24032
		FObj->printf("DSECT\tENDS\n");
#elif MICROCHIP
#endif
		while((ch=FO4->get()) != EOF)			// infine CODE
			FObj->put(ch);
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n",TextSegm);
#elif MC68000
		FObj->printf("%s\tENDS\n",TextSegm);
#elif GD24032
		FObj->printf("CSECT\tENDS\n");
#elif MICROCHIP
#endif
#if ARCHI
#elif Z80
		FObj->printf("\n\tend");
#elif I8086
		FObj->printf("END");
#elif MC68000
		FObj->printf("\nend\n");
#elif GD24032
		FObj->printf("\nend\n");
#elif MICROCHIP
		FObj->printf("\nEND");
#endif
		}		// Tipo Speciale

	else {
		while((ch=FO1->get()) != EOF)			// prima DATA
			FObj->put(ch);
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n",DataSegm);
#elif MC68000
		FObj->printf("%s_END\n",DataSegm);
#elif GD24032
		FObj->printf("%s_END\n",DataSegm);
#elif MICROCHIP
#endif
		while((ch=FO2->get()) != EOF)			// poi BSS
			FObj->put(ch);
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n","CONST");
#elif MC68000
		FObj->printf("_BSS_END\n");
#elif GD24032
		FObj->printf("BSS_END\n");
#elif MICROCHIP
#endif
		while((ch=FO3->get()) != EOF)			// e CONST
			FObj->put(ch);
		if(TipoOut & TIPO_EMBEDDED) {
			CString S;
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// serve riallineare, per sicurezza!
#else
			FObj->printf("\tALIGN 4\n",NULL);		// serve riallineare, per sicurezza!
//			FObj->printf("\tdw 0\n",NULL);		// serve riallineare, per sicurezza!
#endif
			FObj->printf("\n__init_data:\n",NULL);		// PUO' essere dispari, tanto la copia la fa a byte!
		  FO1->Seek(0l,CFile::begin);  
			while(!FO1->eof()) {			// copio DATA in CONST
//				FO1->read(myBuf);
				FO1->ReadString(S);
				if((p=strchr((LPCTSTR)S,'\t')) && (p[1] != 'o' && p[2] != 'r' && p[3] != 'g')) {		// porcata ma ok! tolgo il nome var e lascio il dato
					FObj->println(p);
					}
				}
		  FO1->Seek(0l,CFile::begin);  
			}
#if MC68000
			FObj->printf("\tds.w 0\n",NULL);		// riallineo segmento successivo
#else
			FObj->printf("\tALIGN 4\n",NULL);		// riallineo segmento successivo
#endif
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n","_BSS");
#elif MC68000
		FObj->printf("_CONST_END\n");
#elif GD24032
		FObj->printf("CONST_END\n");
#elif MICROCHIP
#endif
		while((ch=FO5->get()) != EOF)			// extern ecc
			FObj->put(ch);
#if MC68000
		FObj->printf("\n%s_START\n",TextSegm);
#elif GD24032

#endif
		while((ch=FO4->get()) != EOF)			// infine CODE
			FObj->put(ch);
#if ARCHI
#elif Z80
#elif I8086
		FObj->printf("%s\tENDS\n",TextSegm);
#elif MC68000
		FObj->printf("%s_END\n",TextSegm);
#elif GD24032
		FObj->printf("%s_END\n",TextSegm);
#elif MICROCHIP
#endif
#if ARCHI
#elif Z80
		FObj->printf("\n\tend");
#elif I8086
		FObj->printf("END");
#elif MC68000
		FObj->printf("\n\tend %s\n",CurrFunc ? CurrFunc->label : "");			// per easy68k, cmq non va xché qua la CurrFunc è già andata... vabbe'
#elif GD24032
		FObj->printf("\n\tEND %s\n",CurrFunc ? CurrFunc->label : "");			//
#elif MICROCHIP
		FObj->printf("\nEND");
#endif
		}
	FO5->Close(); CFile::Remove(FO5->GetFilePath()); delete FO5;
	FO4->Close(); CFile::Remove(FO4->GetFilePath()); delete FO4;
	FO3->Close(); CFile::Remove(FO3->GetFilePath()); delete FO3;
	FO2->Close(); CFile::Remove(FO2->GetFilePath()); delete FO2;
	FO1->Close(); CFile::Remove(FO1->GetFilePath()); delete FO1;

//  PROCV("temp.map");
  if(FLst) {
    PROCVarList(FLst,(struct VARS*)NULL);
		delete FLst;		FLst=NULL;
    }
	LVars=Var;
	while(LVars) {
		Var=LVars;
		if(Var->definition) {
			struct LINE *t;
			while(Var->definition) {
				t=Var->definition->next;
				GlobalFree(Var->definition);
				Var->definition=t;
				}
			}
	  if(Var->type & VARTYPE_FUNC) 			// anche Pointer
			GlobalFree(Var->parm);
		LVars=LVars->next;
		GlobalFree(Var);
		}

	if(FErr)
		delete FErr;

//		}
//	catch(...) {
//   AfxMessageBox("exce");
//    }
	if(myOutput) 	{
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		wsprintf(p,"%s - %d errori, %d warning (%u righe, %u mSec)",OUS,numErrors,numWarnings,FObj->getTotalLines(),
			timeGetTime()-startTime);
		myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		}

//	PROCT();
	delete FObj; FObj=NULL;
  return 1;

fine:
	return 0;
  } 
	  
int Ccc::PROCBlock() {
  int I,i,j,m,M;
  char ARG[128];
  long OT;
	int ol;
  char MyBuf[128];
  struct VARS *v;
  char *p;
  
	if(!InBlock)
		AutoOff = 0;

  InBlock++;
	if(InBlock >= MAX_BLOCCHI)
		PROCError(1002,"blocchi nidificati");
	OldTX[InBlock].AutoOff = min(OldTX[InBlock-1].AutoOff,OldTX[InBlock].AutoOff);
  I=InBlock;    
  do {
		FIn->SavePosition();
		OT=FIn->GetPosition();
		ol=__line__;
		FNLA(ARG);
	
		if(debug) 
	  	myLog->print(0,">>>%s<<<\n",ARG);
	  
		switch(*ARG) {
		  case 0:                    // fine riga o fine FILE
				FNLO(ARG);
//    		PROCError(1004);
				__line__++;
		    break;
		  case '{':
				FNLO(ARG);
				Declaring=TRUE;
				PROCBlock();
				break;
		  case ';':
				FNLO(ARG);
				break;
		  case '}':
				FNLO(ARG);
//				__line__++;
				goto end_block;
				break;
		  default:
				FIn->RestorePosition(OT);
				__line__=ol;
				if(!FNIsStmt()) 
				  PROCIsDecl();
				__line__++;
				break;
		  }
		} while(*ARG != '}' && !FIn->Eof());
end_block:
  if(!I || FIn->Eof())			// bah sarebbe da gestire meglio la } finale
		PROCError(2054,"}");

	AutoOff = min(OldTX[I].AutoOff,AutoOff);
  if(I>1) {
//		OldTX[I-1].AutoOff = min(OldTX[I].AutoOff,OldTX[I-1].AutoOff);
		OldTX[I].AutoOff = 0;
		switch(*OldTX[I].T) {
		  case 0:

				break;
	  	case '&':			// se switch(
			  swap(&LastOut,&OldTX[I].TX);
        p=OldTX[I].parm;
        i=*(int *)p;
#if INT_SIZE==2
        M=0xffff8000;
        m=0x7fff;
#elif INT_SIZE==4
        M=0x80000000;
        m=0x7fffffff;
#endif
        while(i--) {
          p+=sizeof(int);
          j=*(int *)p;
					if(j<m)
					  m=j;          
					if(j>M)
					  M=j;          
          }
//          myLog->print("case min e max %d %d\n\a",m,M);
				char MyBuf_def[64];
        _tcscpy(MyBuf_def,OldTX[I].B);       // label per "default" se c'è, oppure fine blocco
			  if(OldTX[InBlock].flag) 
          _tcscat(MyBuf_def,"_");
        p=OldTX[I].parm;
        i=*(int *)p;
#if MC68000
        if(i<=8 /*10*/) {			// a seconda di MemoryModel potrebbe essere meglio fino a 10 o altro..
					// cmq sono 6*cond con Branch, 36+3*cond con linear search, Poi conta anche velocità esecuzione, forse
#elif GD24032
        if(i<=6 /*10*/) {			// VERIFICARE! secondo gemini siamo tra 4 e 6, con ancora piccola differenza tra jump table e search
#else
        if(i<=5) {
#endif
	        while(i--) {
	          p+=sizeof(int);
	          j=*(int *)p;
					  sprintf(MyBuf,"%s_%x",OldTX[I].B,j);
#if ARCHI
					  switch(OldTX[InBlock].C[1]) {
							case 1:
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,j);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
							case 2: // me ne fotto :)
							case 4:
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,j);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
//							PROCOutLab(MyBuf,"_");
							}
#elif Z80
//				  if(OldTX[InBlock].C[1]==1)		//direi sempre...
						if(i==(*(int *)p-1))		// LD solo la prima volta
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
//					  PROCOper(movString,Regs->Accu,Regs->DSl);    // non dovrebbe servire, qui
					  if(OldTX[InBlock].C[1]==1) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_IMMEDIATO8,j);
							PROCOper(LINE_TYPE_JUMPC,"jp",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_IMMEDIATO8,j);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"$",8);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_IMMEDIATO8,*(((char *)&j)+1));
							PROCOper(LINE_TYPE_JUMPC,"jp",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
//							PROCOutLab(MyBuf,"_");
							}
#elif I8086
//				  if(OldTX[InBlock].C[1]==1)		direi sempre...
//						if(i==(*(int *)p-1))		// MOV solo la prima volta
// qua non serve :)						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
//					  PROCOper(movString,Regs->Accu,Regs->DSl);    // non dovrebbe servire, qui
					  if(OldTX[InBlock].C[1]==1) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO8,j);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,j);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
//							PROCOutLab(MyBuf,"_");
							}
#elif MC68000
//				  if(OldTX[InBlock].C[1]==1)		direi sempre...
					  switch(OldTX[InBlock].C[1]) {
							case 1:
								PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.b",OPDEF_MODE_IMMEDIATO8,j,OPDEF_MODE_REGISTRO8,Regs->D);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
							case 2:
								PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.w",OPDEF_MODE_IMMEDIATO16,j,OPDEF_MODE_REGISTRO16,Regs->D);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
							case 4:
								PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.l",OPDEF_MODE_IMMEDIATO32,j,OPDEF_MODE_REGISTRO32,Regs->D);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
//							PROCOutLab(MyBuf,"_");
							}
#elif GD24032
					  switch(OldTX[InBlock].C[1]) {
							case 1:
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.b",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,j);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
							case 2:
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,j);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
							case 4:
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,j);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								break;
//							PROCOutLab(MyBuf,"_");
							}
#elif MICROCHIP
//					  PROCOper(movString,Regs->Accu,Regs->DSl);    // non dovrebbe servire, qui
					  if(OldTX[InBlock].C[1]==1) {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO8,j);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
						  }
						else {
						  PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO8,j);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"$",8);
						  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
						  PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO8,*(((char *)&j)+1));
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
//							PROCOutLab(MyBuf,"_");
							}
#endif
						}	
#if ARCHI
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif Z80
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);           // questo è default
#elif I8086
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif MC68000
					PROCOper(LINE_TYPE_JUMP,(MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? jmpString : jmpShortString,
						OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
#elif GD24032
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
#elif MICROCHIP
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#endif
//				  LastOut=LastOut->next;
					}
				else {
          _tcscpy(MyBuf,OldTX[I].B);       // preparo add. tab.
          _tcscat(MyBuf,"_N");
				  if(M-m <= i*2) {         // se la differenza tra i case estremi è minore del doppio dei casi, jump table
				  										// Z80,6502: in realtà dovrebbe essere 2x se int, 1.5x se char
															// su altre, fare prove
					  if(OldTX[InBlock].C[1]==1) {
#if ARCHI
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUB",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,m);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_IMMEDIATO8,M-m+1);     // +1 per jp nc...???
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
#elif Z80
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_IMMEDIATO16,M-m+1);        // +1 per jp nc...
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,5,0);
#elif I8086
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_IMMEDIATO16,M-m+1);        // +1 per jp nc...
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_IMMEDIATO16,0);
#elif MC68000
							PROCOper(LINE_TYPE_ISTRUZIONE,"subi.b",OPDEF_MODE_IMMEDIATO8,m,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.b",OPDEF_MODE_IMMEDIATO8,M-m+1,OPDEF_MODE_REGISTRO8,Regs->D);     // +1 per DBRA...???
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
#elif GD24032
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b",OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_REGISTRO8,Regs->D);
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUB.b",OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_IMMEDIATO8,m);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.b",OPDEF_MODE_REGISTRO8,Regs->D+1,OPDEF_MODE_IMMEDIATO8,M-m+1);     // +1 per DJNZ
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
#elif MICROCHIP
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    if(CPUPIC < 2)
								PROCOper(LINE_TYPE_ISTRUZIONE,"XORLW",OPDEF_MODE_IMMEDIATO16,M-m+1);        // +1 per jp nc...
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,"CP",OPDEF_MODE_IMMEDIATO16,M-m+1);        // +1 per jp nc...
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,5,0);
#endif					    
						  }
						else {
#if ARCHI			
							// 2 e 4
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUB",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,m);
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
							PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,M-m+1);     // +1 per jp nc...???
							PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);

#elif Z80
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    // carry già a 0
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_IMMEDIATO16,M-m+1);     // +1 per jp nc...
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#elif I8086
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    // carry già a 0
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_IMMEDIATO16,M-m+1);     // +1 per jp nc...
					    PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,1);
#elif MC68000
						  if(OldTX[InBlock].C[1]==2) {
								PROCOper(LINE_TYPE_ISTRUZIONE,"subi.w",OPDEF_MODE_IMMEDIATO16,m,OPDEF_MODE_REGISTRO32,Regs->D);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.w",OPDEF_MODE_IMMEDIATO16,M-m+1,OPDEF_MODE_REGISTRO16,Regs->D);     // +1 per DBRA...???
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								}
							else {			// fare anche per altri! alcuni almeno :)
								PROCOper(LINE_TYPE_ISTRUZIONE,"subi.l",OPDEF_MODE_IMMEDIATO32,m,OPDEF_MODE_REGISTRO32,Regs->D);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								PROCOper(LINE_TYPE_ISTRUZIONE,"cmpi.l",OPDEF_MODE_IMMEDIATO32,M-m+1,OPDEF_MODE_REGISTRO32,Regs->D);     // +1 per DBRA...???
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								}
#elif GD24032
						  if(OldTX[InBlock].C[1]==2) {
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,"SUB.w",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_IMMEDIATO16,m);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.w",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,M-m+1);     // +1 per DJNZ
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								}
							else {			// fare anche per altri! alcuni almeno :)
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								PROCOper(LINE_TYPE_ISTRUZIONE,"SUB.d",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_IMMEDIATO32,m);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MINORE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.d",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_IMMEDIATO32,M-m+1);     // +1 per DJNZ
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_MAGGIORE_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								}
#elif MICROCHIP
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_IMMEDIATO16,m);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    // carry già a 0
					    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_IMMEDIATO16,M-m+1);     // +1 per jp nc...
					    PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
					    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					    PROCOper(LINE_TYPE_ISTRUZIONE,"ADDWF",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#endif
					    }
#if ARCHI
#elif Z80
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,0);
//					    PROCOper(LINE_TYPE_ISTRUZIONE,"rl",2,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#elif I8086
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,0);
//					    PROCOper(LINE_TYPE_ISTRUZIONE,"rl",2,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#elif MC68000
#elif GD24032
#elif MICROCHIP
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ADDLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,0);
//					    PROCOper(LINE_TYPE_ISTRUZIONE,"rl",2,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"ADDLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,1);
#endif
Mm20:   ;
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,"LSL",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_IMMEDIATO8,2);
// MemoryModel ??
	          _tcscpy(MyBuf,OldTX[I].B);   
	          _tcscat(MyBuf,"_T");       // ripreparo add. jump
					  if(M-m <= i*2)          // se la differenza tra i case estremi è minore del doppio dei casi, jump table
							PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
		    		PROCOper(LINE_TYPE_ISTRUZIONE,"ADD",OPDEF_MODE_REGISTRO32,Regs->P,
							OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_REGISTRO32,Regs->D);
				    PROCOper(LINE_TYPE_JUMP,"B",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
#elif Z80
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
				    PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_REGISTRO_INDIRETTO,0);
#elif I8086
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
				    PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_REGISTRO_INDIRETTO,0);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,"lsl.l",OPDEF_MODE_IMMEDIATO8,
// in effetti non dovrebbe superare 64K mai!							(MemoryModel & 0xf) < MEMORY_MODEL_LARGE ? 1 : 2,OPDEF_MODE_REGISTRO,1);
							(MemoryModel & 0xf) < MEMORY_MODEL_LARGE ? 1 : 2,OPDEF_MODE_REGISTRO,Regs->D);
//				    PROCOper(LINE_TYPE_JUMP,"move.l",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"(8,pc,d0)",0,OPDEF_MODE_REGISTRO,8);
						// OCCHIO la merda di easy68k sembra non assemblare correttamente questa sopra... ignora offset 8, CMQ FACCIO diversamente!
//				    PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"(0,pc,d0)",0); no
	          _tcscpy(MyBuf,OldTX[I].B);   
	          _tcscat(MyBuf,"_T");       // ripreparo add. jump
//	 SERVE!! cmq				  if(M-m <= i*2)          // se la differenza tra i case estremi è minore del doppio dei casi, jump table
							if(MemoryModel & MEMORY_MODEL_RELATIVE) {
								PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0,
									OPDEF_MODE_REGISTRO32,Regs->P);			// questo mi serve cmq per avere parte alta dell'indirizzo lungo
								}
							else
								PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0,
									OPDEF_MODE_REGISTRO32,Regs->P);			// questo mi serve cmq per avere parte alta dell'indirizzo lungo
						// se no, basta quello sopra! 			    	
						if((MemoryModel & 0xf) < MEMORY_MODEL_LARGE) //MEMORY_MODEL_MEDIUM
 			    		PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"(a0,d0)",0,OPDEF_MODE_REGISTRO32,Regs->P);
						else		// in effetti uno switch non dovrebbe superare 64K!
 			    		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&"(a0,d0)",0,OPDEF_MODE_REGISTRO32,Regs->P);
						if(MemoryModel & MEMORY_MODEL_RELATIVE) {
							PROCOper(LINE_TYPE_JUMP,"bra",OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
						else {
							PROCOper(LINE_TYPE_JUMP,"jmp" /*jmpString fisso!*/,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->P);
							}
#elif GD24032
						if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
							if(MemoryModel & MEMORY_MODEL_RELATIVE)
								;		// tendenzialmente qua funzia lo stesso...
							PROCOper(LINE_TYPE_ISTRUZIONE,"SLA.w",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO8,1);
							if(MemoryModel & MEMORY_MODEL_RELATIVE) {
			          wsprintf(MyBuf,"%s%s-$",OldTX[I].B,"_T");       // ripreparo add. jump + registro
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&MyBuf,M-m <= i*2 ? 0 : -2);
								}
							else {
			          wsprintf(MyBuf,"%s%s",OldTX[I].B,"_T");       // ripreparo add. jump + registro
								PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)&MyBuf,M-m <= i*2 ? 0 : -2);
								}
							PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"[R0+R1]",0);
							PROCOper(LINE_TYPE_ISTRUZIONE,"ADD.d",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
							PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_REGISTRO,Regs->D);
							}
						else {
							if(MemoryModel & MEMORY_MODEL_RELATIVE) {
								PROCOper(LINE_TYPE_ISTRUZIONE,"SLA.d",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO8,2);
								wsprintf(MyBuf,"%s%s",OldTX[I].B,"_T");       // ripreparo add. jump + registro
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
									PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&MyBuf,M-m <= i*2 ? 0 : -4);
									}
								else
									PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)&MyBuf,M-m <= i*2 ? 0 : -4);
								PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"[R0+R1]",0);
								PROCOper(LINE_TYPE_ISTRUZIONE,"ADD.d",OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_REGISTRO,Regs->D+1);
								PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_REGISTRO,Regs->D);
								}
							else {
								PROCOper(LINE_TYPE_ISTRUZIONE,"SLA.d",OPDEF_MODE_REGISTRO,Regs->D+1,OPDEF_MODE_IMMEDIATO8,2);
								wsprintf(MyBuf,"[%s%s-%u+R%u]",OldTX[I].B,"_T",M-m <= i*2 ? 0 : 4,1);       // ripreparo add. jump + registro; v. anche ALTROVE per scegliere offset corretto!
								PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
								// si può unire!!
								}
							}

#elif MICROCHIP
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,incString,OPDEF_MODE_REGISTRO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_REGISTRO_INDIRETTO,0);
				    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_HIGH8,3);
				    PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_REGISTRO_INDIRETTO,0);
#endif
				    }
				  else {			// linear search in table
					  switch(OldTX[InBlock].C[1]) {
							case 1:
#if ARCHI
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif Z80
			    			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-1);
#elif I8086
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-1);
#elif MC68000
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i,
										OPDEF_MODE_REGISTRO32,Regs->P);
									}
								else
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i,
										OPDEF_MODE_REGISTRO32,Regs->P);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif GD24032
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
				          wsprintf(MyBuf,"%s%s-$",OldTX[I].B,"_T");       // 
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->P,
										OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&MyBuf,i);
									}
								else {
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO32,Regs->P,
										OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)&MyBuf,i);
									}
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif MICROCHIP
			    			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-1);
#endif
								break;
							case 2:
							case 4:		// per ora, e v. sotto
#if ARCHI
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,"ADR",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i);
			          _tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif Z80
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,
									OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
#elif I8086
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
#elif MC68000
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
	  			    		PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i,OPDEF_MODE_REGISTRO32,Regs->P);
									}
								else
	  			    		PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,i,OPDEF_MODE_REGISTRO32,Regs->P);
			          _tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif GD24032
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
				          wsprintf(MyBuf,"%s%s-$",OldTX[I].B,"_T");       // 
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_VARIABILE_INDIRETTO,
										(union SUB_OP_DEF*)&MyBuf,i);
									}
								else {
  			    			PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO32,Regs->P,OPDEF_MODE_VARIABILE_INDIRETTO,
										(union SUB_OP_DEF*)&MyBuf,i);
									}
			          _tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
#elif MICROCHIP
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,Regs->D,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,0);
#endif
								break;
				    	}
#if MC68000
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"moveq"/*direi*/,OPDEF_MODE_IMMEDIATO16,i-1 /*per DBEQ*/,
							OPDEF_MODE_REGISTRO16,Regs->D+1);
#elif GD24032
			    	PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_IMMEDIATO16,i);
						// max 65535 casi ...
#else
			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,2,OPDEF_MODE_IMMEDIATO16,i  /*+1*/);
#endif
						switch(OldTX[InBlock].C[1]) {
							case 1:
#if ARCHI
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"-(a0)",0);
								// finire!
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"SUB",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_IMMEDIATO16,1);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif Z80
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cpdr",OPDEF_MODE_NULLA,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif I8086
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"rep cmpb",OPDEF_MODE_NULLA,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif MC68000
//				    	PROCOper(LINE_TYPE_ISTRUZIONE,"cmp.b",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO_INDIRETTO,8,1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cmp.b",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"-(a0)",0,
									OPDEF_MODE_REGISTRO8,Regs->D);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"dbeq.w",OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								if(MemoryModel & MEMORY_MODEL_RELATIVE)				// gestire
									;
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif GD24032
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.b",
									OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"[--R16]",0);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"$+12",0);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"DJNZ.w",OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								if(MemoryModel & MEMORY_MODEL_RELATIVE)				// gestire
									;
								PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif MICROCHIP
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cpdr",OPDEF_MODE_NULLA,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#endif
					      break;
							case 2:
							case 4:		// per ora così! v. anche dw ecc sotto
								PROCOutLab(OldTX[I].B,"_J");
#if ARCHI
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"CMP",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"-(a0)",0);
								// finire!
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"SUB",OPDEF_MODE_REGISTRO16,Regs->D+1,OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_IMMEDIATO16,1);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"MOV",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif Z80
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J2");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J3");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOutLab(OldTX[I].B,"_J2");
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
								PROCOutLab(OldTX[I].B,"_J3");
#elif I8086
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J2");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cmp",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J3");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOutLab(OldTX[I].B,"_J2");
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
								PROCOutLab(OldTX[I].B,"_J3");
#elif MC68000
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cmp.l",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"-(a0)",0,
									OPDEF_MODE_REGISTRO32,Regs->D);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"dbeq.w",OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								if(MemoryModel & MEMORY_MODEL_RELATIVE)				// gestire
									;
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D+1,OPDEF_MODE_REGISTRO32,Regs->D);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif GD24032
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOutLab(OldTX[I].B,"_J");
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"CMP.d",
									OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"[--R16]",0);
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"$+12",0);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_ISTRUZIONE,"DJNZ.w",OPDEF_MODE_REGISTRO16,Regs->D+1,
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								if(MemoryModel & MEMORY_MODEL_RELATIVE)				// gestire
									;
								PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf_def,0);
								_tcscpy(MyBuf,OldTX[I].B);       // ripreparo add. tab.
  							_tcscat(MyBuf,"_T");
#elif MICROCHIP
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J2");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_INDIRETTO,Regs->D);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"cp",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J3");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOutLab(OldTX[I].B,"_J2");
  			    		PROCOper(LINE_TYPE_ISTRUZIONE,decString,OPDEF_MODE_REGISTRO,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,2);
				    		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,2);
								_tcscpy(MyBuf,OldTX[I].B);
  							_tcscat(MyBuf,"_J");
								PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_DIVERSO,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
								PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
								PROCOutLab(OldTX[I].B,"_J3");
#endif
								break;
				      }
#if ARCHI
#elif Z80
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,
							OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-2);
						PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,1,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,2,2);
#elif I8086
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,
							OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-2);
						PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,1,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,2,2);
#elif MC68000
//      	    PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO,1,OPDEF_MODE_REGISTRO,0);
#elif GD24032

#elif MICROCHIP
      	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)&MyBuf,-2);
						PROCOper(LINE_TYPE_ISTRUZIONE,"ADDLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
				    PROCOper(LINE_TYPE_ISTRUZIONE,"ADDLW",OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,1,2);
//			    	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,2,2);
#endif
				    goto Mm20;
				    }  
	        p=OldTX[I].parm;
	        i=*(int *)p;
				  if(M-m <= i*2) {		// v. anche sopra
					  PROCOutLab(OldTX[I].B,"_T");
					  for(j=m; j<=M; j++) {
			        p=OldTX[I].parm;
			        i=*(int *)p;
			        while(i--) {
			          p+=sizeof(int);
		          	if(j==*(int *)p)
		          	  break;
		          	}
		          if(i==-1) {	
#if ARCHI
							  PROCOper(LINE_TYPE_DATA_DEF,"DCD\t",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif Z80 || I8086
							  PROCOper(LINE_TYPE_DATA_DEF,"dw",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif MC68000
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
									wsprintf(MyBuf,"%s-%s_T",OldTX[I].T+1,OldTX[I].B);
									PROCOper(LINE_TYPE_DATA_DEF,
										(!(TipoOut & TIPO_SPECIALE)) ? 
											((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "dd" : "dw") : ((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "\tdc.l" : "\tdc.w"),
										OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
									}
								else
									PROCOper(LINE_TYPE_DATA_DEF,
										(!(TipoOut & TIPO_SPECIALE)) ? 
											((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "dd" : "dw") : ((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "\tdc.l" : "\tdc.w"),
										OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#elif GD24032
								if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
									wsprintf(MyBuf,"%s-%s_T",OldTX[I].T+1,OldTX[I].B);
									PROCOper(LINE_TYPE_DATA_DEF,"DW",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
									}
								else {
									if(MemoryModel & MEMORY_MODEL_RELATIVE) {
										char MyBuf2[64];
										wsprintf(MyBuf2,"%s-%s_T",MyBuf,OldTX[I].B);
										PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf2,0);
										}
									else
										PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
									}
#elif MICROCHIP
							  PROCOper(LINE_TYPE_DATA_DEF,"dw",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
#endif
		            }
		          else {  
							  wsprintf(MyBuf,"%s_%x",OldTX[I].B,j);
#if MC68000
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
									PROCOper(LINE_TYPE_DATA_DEF,
										(!(TipoOut & TIPO_SPECIALE)) ? 
											((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "dd" : "dw") : ((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "\tdc.l" : "\tdc.w"),
										OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
									}
								else
									PROCOper(LINE_TYPE_DATA_DEF,
										(!(TipoOut & TIPO_SPECIALE)) ? 
											((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "dd" : "dw") : ((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "\tdc.l" : "\tdc.w"),
										OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
								if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
									char MyBuf2[64];
									wsprintf(MyBuf2,"%s-%s_T",MyBuf,OldTX[I].B);
									PROCOper(LINE_TYPE_DATA_DEF,"DW",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf2,0);
									}
								else
									if(MemoryModel & MEMORY_MODEL_RELATIVE) {
										char MyBuf2[64];
										wsprintf(MyBuf2,"%s-%s_T",MyBuf,OldTX[I].B);
										PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf2,0);
										}
									else
										PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif ARCHI
							  PROCOper(LINE_TYPE_DATA_DEF,"DCD\t",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#else
							  PROCOper(LINE_TYPE_DATA_DEF,"dw",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#endif
							  }
						  }
				    }
				  else {
					  PROCOutLab(OldTX[I].B,"_T");
		        while(i--) {
		          p+=sizeof(int);
		          j=*(int *)p;
						  sprintf(MyBuf,"%s_%x",OldTX[I].B,j);
#if ARCHI
						  PROCOper(LINE_TYPE_DATA_DEF,"DCD\t",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086 
						  PROCOper(LINE_TYPE_DATA_DEF,"dw",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MC68000
						  PROCOper(LINE_TYPE_DATA_DEF,
								(!(TipoOut & TIPO_SPECIALE)) ? 
									((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "dd" : "dw") : ((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "\tdc.l" : "\tdc.w"),
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
							if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
								char MyBuf2[64];
								wsprintf(MyBuf2,"%s-%s_T",MyBuf,OldTX[I].B);
								PROCOper(LINE_TYPE_DATA_DEF,"DW",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf2,0);
								}
							else
								if(MemoryModel & MEMORY_MODEL_RELATIVE) {
									char MyBuf2[64];
									wsprintf(MyBuf2,"%s-%s_T",MyBuf,OldTX[I].B);
									PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf2,0);
									}
								else
									PROCOper(LINE_TYPE_DATA_DEF,"DD",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MICROCHIP
						  PROCOper(LINE_TYPE_DATA_DEF,"dw",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#endif
						  }
					  PROCOutLab(OldTX[I].B,"_N");		// MEGLIO i salti prima, per agevolare un BRAnch PC-relative (68000...
		        p=OldTX[I].parm;
		        i=*(int *)p;
		        while(i--) {
		          p+=sizeof(int);
		          j=*(int *)p;
						  itoa(j,MyBuf,10);
#if ARCHI
						  PROCOper(LINE_TYPE_DATA_DEF,(OldTX[InBlock].C[1]==1) ? "DCB\t" : "DCD\t",
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086
						  PROCOper(LINE_TYPE_DATA_DEF,(OldTX[InBlock].C[1]==1) ? "db\t" : "dw\t",
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MC68000
							if(!(TipoOut & TIPO_SPECIALE))
							  PROCOper(LINE_TYPE_DATA_DEF,
									(OldTX[InBlock].C[1]==1) ? "db\t" : ((OldTX[InBlock].C[1]==2) ? "dw\t" : "dd\t"),
									OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
							else
							  PROCOper(LINE_TYPE_DATA_DEF,
									(OldTX[InBlock].C[1]==1) ? "\tdc.b\t" : ((OldTX[InBlock].C[1]==2) ? "\tdc.w\t" : "\tdc.l\t"),OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
							PROCOper(LINE_TYPE_DATA_DEF,
								(OldTX[InBlock].C[1]==1) ? "DB\t" : ((OldTX[InBlock].C[1]==2) ? "DW\t" : "DD\t"),
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MICROCHIP
						  PROCOper(LINE_TYPE_DATA_DEF,(OldTX[InBlock].C[1]==1) ? "db\t" : "dw\t",
								OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#endif
						  }
#if MC68000
						if(OldTX[InBlock].C[1]==1) {
							MyBuf[0]='0'; MyBuf[1]=0;
							PROCOper(LINE_TYPE_DATA_DEF,"\tds.w",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);		// ossia ALIGN a word: dice che l'assembler lo fa in automatico, ma...
							}
#elif GD24032
						if(OldTX[InBlock].C[1]==1) {
							MyBuf[0]='4'; MyBuf[1]=0;
							PROCOper(LINE_TYPE_DATA_DEF,"\tALIGN",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);		//
							}
#elif ARCHI
						if(OldTX[InBlock].C[1]==1) {
							MyBuf[0]='0'; MyBuf[1]=0;
							PROCOper(LINE_TYPE_DATA_DEF,"ALIGN",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);		//
							}
// prima facevamo così... controllare					    PROCOut1(FO1,"ALIGN",NULL);
#endif
					  }
				  }	

			  swap(&LastOut,&OldTX[I].TX);
			  
#if ARCHI
				PROCOutLab(OldTX[I].B);
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
				PROCOutLab(OldTX[I].B);
#endif
				*OldTX[I].T=0;
				*OldTX[I].B=0;
				*OldTX[I].C=0;
				GlobalFree(OldTX[I].parm);
				break;
	  	case '#':		// se do while
				if(_tcscmp(FNLA(MyBuf),"while"))
		  		PROCError(2054,"while");
				break;
	  	case '%':		// se if then else
				if(_tcscmp(FNLA(MyBuf),"else")) {
		  		LastOut=OldTX[I].TX;
//          *OLDT[I]=0;
//          *OLDB[I]=0;
//          *OLDC[I]=0;
		  		}           
				break;
	  	default:
				if(OldTX[I].TX) {
		  		LastOut=OldTX[I].TX;
		  		*OldTX[I].T=0;
		  		*OldTX[I].B=0;
		  		*OldTX[I].C=0;
		  		}
				break;
	  	}                          
		}
  else {
		if(!CurrFunc) {
			PROCError(/*2062*/1001,"inBlock=0, function=NULL");
			return -1;
			}
		swap(&OldTX[0].TX,&LastOut);
	  if(TempSize) {
  	  AutoOff-=TempSize;
  	  wsprintf(MyBuf,"temp = %d",AutoOff);
  	  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
	    }
		AutoOff &= -STACK_ITEM_SIZE;
		if(!_tcscmp(CurrFunc->name,"main")) {
#if ARCHI
//		  PROCOper(LINE_TYPE_ISTRUZIONE,"STR ",Regs->SpS,",__stktop",NULL,NULL);		SPOSTATO nel codice di startup CRT
#elif Z80
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,"(__stktop)",Regs->SpS);
#elif I8086
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,"mov WORD PTR","[__stktop]",Regs->SpS);
#elif MC68000
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,"move.l","[__stktop]",Regs->SpS);
			if(MemoryModel & MEMORY_MODEL_RELATIVE) {
	  		PROCOper(LINE_TYPE_ISTRUZIONE,"lea",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"__BaseAbs",0,OPDEF_MODE_REGISTRO32,13/*Regs->AbsS*/);		// cercare PRIMISSIMA var globale!
				}
#elif GD24032
			if(MemoryModel & MEMORY_MODEL_RELATIVE) {
	  		PROCOper(LINE_TYPE_ISTRUZIONE,"LEA",OPDEF_MODE_REGISTRO32,25 /*Regs->AbsS*/,
					OPDEF_MODE_VARIABILE_INDIRETTO,(union SUB_OP_DEF*)"__BaseAbs",0);		// cercare PRIMISSIMA var globale!
				}
#elif MICROCHIP
//	  	PROCOper(LINE_TYPE_ISTRUZIONE,"mov WORD PTR","[__stktop]",Regs->SpS);
#endif
				}
//    myLog->print("eccomi %d\n",CurrFunc);
//  	PROCV("vartmp.map");
		if(CurrFunc->modif & FUNC_MODIF_INTERRUPT && !(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
#if ARCHI

#elif Z80
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,3);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,2);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,0);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,8);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,9);
#elif I8086
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"pushf",OPDEF_MODE_NULLA);
      if(CPU86<1) {
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,0);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,1);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,2);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,3);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,8);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,9);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,10);
		  	}
		  else	
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"pusha",OPDEF_MODE_NULLA);
#elif MC68000
			PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"sr",0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
			PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l",OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"a0-a6/d0-d7",0,
				OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
#elif GD24032
			PROCOper(LINE_TYPE_ISTRUZIONE,"STST",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
			PROCOper(LINE_TYPE_ISTRUZIONE,"STM.d",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"{R0-R23}",0
				);
#elif MICROCHIP
			if(CPUPIC < 2) {
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,2);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,1);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,8);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,9);
				}
			else {
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,3);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,2);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,1);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,0);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,8);
	  		PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,9);
				}
#endif

	  	}
		/*else beh no*/ if(SaveFP || AutoOff) {
			if(!(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
//		myLog->print("SaveFP: %d, AutoOff %d, FuncCalled %d, Checkstack %d\n",SaveFP,AutoOff,FuncCalled,CheckStack);
#if ARCHI
				if(FuncCalled || CheckStack || CheckPointers) {
					_tcscpy(MyBuf,Regs->FpS);
					_tcscat(MyBuf,",R14");
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
						OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
					}
				else {
					_tcscpy(MyBuf,Regs->FpS);
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
						OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
					}
#elif Z80 
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER,0);
#elif I8086
				if(CPU86<1) {
					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
					}
#elif MC68000
//			PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_FRAMEPOINTER,0);
#elif GD24032

#elif MICROCHIP
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_REGISTRO,10);
				if(StackLarge) {		// FINIRE
					}
#endif
#if ARCHI
		  	PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
#elif Z80
				if(!AutoOff) {
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
					}
#elif I8086
				if(!AutoOff) {
					if(CPU86<1)
  					/*PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0)*/;
					else  
						/*PROCOper(LINE_TYPE_ISTRUZIONE,"enter",OPDEF_MODE_IMMEDIATO16,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"0",0)*/;
					}
#elif MC68000
//  			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
#elif GD24032

#elif MICROCHIP
				if(!AutoOff) {
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
					}
#endif
				}
			}

		if(AutoOff) {
			if(!(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
#if ARCHI
	  	if(abs(AutoOff) > 32767)		// beh direi almeno normalmente
				PROCError(1126);
#elif Z80
	  	if(abs(AutoOff) > 127) 
				PROCError(1126);
#elif I8086
	  	if(abs(AutoOff) > 32767) 
				PROCError(1126);
#elif MC68000
	  	if(abs(AutoOff) > 32767)		// credo qua long :) o v. MemoryModel
				PROCError(1126);
#elif GD24032
			if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
		  	if(abs(AutoOff) > 127)		// :)
					PROCError(1126);
				}
			else {
		  	if(abs(AutoOff) > 32767)		// VERIFICARE
					PROCError(1126);
				}
#elif MICROCHIP
			if(CPUPIC < 2) {
	  		if(abs(AutoOff) > 127) 
					PROCError(1126);
				}
			else {
				//if(CPUEXTENDED ...
	  		if(abs(AutoOff) > 127) 
					PROCError(1126);
				}
#endif
	  	if(CheckStack) {
#if ARCHI
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_IMMEDIATO16,-abs(AutoOff));
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif Z80
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_IMMEDIATO16,-abs(AutoOff));
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);      // qui ld iy,sp lo fa CHKSTK
#elif I8086
				PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_IMMEDIATO32,abs(AutoOff),OPDEF_MODE_REGISTRO32,0);
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif MC68000
				if((MemoryModel & 0xf) < MEMORY_MODEL_MEDIUM) {
					if(abs(AutoOff)<=256)
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq"/*movString*/,OPDEF_MODE_IMMEDIATO8,abs(AutoOff),OPDEF_MODE_REGISTRO16,0);
					else 
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.w"/*movString*/,OPDEF_MODE_IMMEDIATO8,abs(AutoOff),OPDEF_MODE_REGISTRO16,0);
					}
				else {
					if(abs(AutoOff)<=256)
						PROCOper(LINE_TYPE_ISTRUZIONE,"moveq"/*movString*/,OPDEF_MODE_IMMEDIATO8,abs(AutoOff),OPDEF_MODE_REGISTRO32,0);
					else if(abs(AutoOff)<65536)
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.w"/*movString*/,OPDEF_MODE_IMMEDIATO16,abs(AutoOff),OPDEF_MODE_REGISTRO32,0);
					else
						PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*movString*/,OPDEF_MODE_IMMEDIATO32,abs(AutoOff),OPDEF_MODE_REGISTRO32,0);
					}
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif GD24032
				if(abs(AutoOff)<=256)
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV,b"/*movString*/,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_IMMEDIATO8,abs(AutoOff));
				else if(abs(AutoOff)<65536)
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w"/*movString*/,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_IMMEDIATO16,abs(AutoOff));
				else
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.l"/*movString*/,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_IMMEDIATO32,abs(AutoOff));
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif MICROCHIP
				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_IMMEDIATO16,abs(AutoOff));
				v=FNCercaVar("_chkstk",0);
				PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);      // qui MOVFF FSR2L, POSTINC1 / MOVFF FSR1L, FSR2L  lo fa CHKSTK
#endif
				}		// checkstack
	  	else {
#if ARCHI
				PROCOper(LINE_TYPE_ISTRUZIONE,"SUB",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,abs(AutoOff));
#elif Z80
				if(abs(AutoOff)<8) {
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
					for(i=AutoOff; i<0; i+=2)		// creo lo spazio nello stack
  					PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,3);
				  }
				else {  
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,AutoOff);
					PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,abs(AutoOff));
					PROCOper(LINE_TYPE_ISTRUZIONE,"add",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
					}
#elif I8086
        if(CPU86<1)
				  PROCOper(LINE_TYPE_ISTRUZIONE,"sub",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO16,abs(AutoOff));
				else  
				  PROCOper(LINE_TYPE_ISTRUZIONE,"enter",OPDEF_MODE_IMMEDIATO16,abs(AutoOff),OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"0",0);
#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,"link",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,AutoOff);
#elif GD24032
					PROCOper(LINE_TYPE_ISTRUZIONE,"ENTER",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO32,abs(AutoOff/4));
					if(OutSource) {
						wsprintf(MyBuf,"\t ossia %u bytes",abs(AutoOff));
						PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
//						PROCOut1(FO2,MyBuf,NULL);
						}
#elif MICROCHIP
//				PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
				if((!StackLarge && abs(AutoOff)<2) || (StackLarge>0 && abs(AutoOff)<4)) {
					for(i=AutoOff; i<0; i++)
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,10);
				  }
				else {  
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_IMMEDIATO8,-AutoOff);
					PROCOper(LINE_TYPE_ISTRUZIONE,"SUBWF",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO8,"F,ACCESS");
					if(StackLarge) {
						PROCOper(LINE_TYPE_ISTRUZIONE,"MOVLW",OPDEF_MODE_IMMEDIATO8,0 /* AutoOff >> 8*/);
						PROCOper(LINE_TYPE_ISTRUZIONE,"SUBWFC",OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_IMMEDIATO8,"F,ACCESS");
						}
					}
#endif
				}
				}		// naked
	  	}			// AutoOff != 0
		else {
			if(!(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
#if ARCHI
	  	if(FuncCalled || CheckStack || CheckPointers) {
				PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
					OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"R14",0);
				}
#elif Z80 || MICROCHIP
#elif I8086
			if(SaveFP) {
        if(CPU86<1)
	  			/*PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0)*/;
				else  
				  PROCOper(LINE_TYPE_ISTRUZIONE,"enter",OPDEF_MODE_IMMEDIATO16,0,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"0",0);
				}
#elif MC68000
			if(SaveFP)
				PROCOper(LINE_TYPE_ISTRUZIONE,"link",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO16,0);
#elif GD24032
			if(SaveFP)
				PROCOper(LINE_TYPE_ISTRUZIONE,"ENTER",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_IMMEDIATO32,0);
#endif    
	  	}		// naked
			}

		if(Reg<Regs->MaxUser && !(CurrFunc->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {		
#if ARCHI
	  	wsprintf(MyBuf,"R%u-R%u",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086
	  	for(i=Regs->MaxUser-1; i>=Reg; i--)
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,i);
#elif MC68000
//	  	for(i=Regs->MaxUser-1; i>=Reg; i--)
//		  	PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*pushString*/,OPDEF_MODE_REGISTRO,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	  	wsprintf(MyBuf,"d%u-d%u",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
#elif GD24032
	  	wsprintf(MyBuf,"{R%u-R%u}",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"STM.d"/*pushString*/,
				OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif MICROCHIP
	  	for(i=Regs->MaxUser-1; i>=Reg; i--)
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,i);
#endif
	  	}
		LastOut=OldTX[0].TX;
		OldTX[0].TX=0;
		if(*OldTX[1].T)
		  PROCOutLab(OldTX[1].T);
		OldTX[1].TX=LastOut;
		if(SaveFP || AutoOff) {
			if(!(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
#if ARCHI
	  	if(FuncCalled || CheckStack || CheckPointers) {
				_tcscpy(MyBuf,Regs->FpS);
				_tcscat(MyBuf,",PC");
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
					OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
				FuncCalled=FALSE;		// per altro test sotto
				}
	  	else {
				_tcscpy(MyBuf,Regs->FpS);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
					OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
				}
#elif Z80
	  	PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
#elif I8086
      if(CPU86<1)
  	  	PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_FRAMEPOINTER,0);
  	  else	
  	  	PROCOper(LINE_TYPE_ISTRUZIONE,"leave",OPDEF_MODE_NULLA,0);
#elif MC68000
 	  	PROCOper(LINE_TYPE_ISTRUZIONE,"unlk",OPDEF_MODE_FRAMEPOINTER,0);
#elif GD24032
 	  	PROCOper(LINE_TYPE_ISTRUZIONE,"LEAVE",OPDEF_MODE_FRAMEPOINTER,0);
#elif MICROCHIP
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,11,OPDEF_MODE_FRAMEPOINTER,0);
			if(StackLarge) {
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_REGISTRO,11,OPDEF_MODE_FRAMEPOINTER,0);		// FINIRE!
				}
#endif
	  	}		// naked
	  	}

		if(CurrFunc->modif & FUNC_MODIF_INTERRUPT) {
			if(!(CurrFunc->attrib & FUNC_ATTRIB_NAKED)) {
#if ARCHI
				// fare
#elif Z80
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,9);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,8);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,0);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,1);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,2);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
#elif I8086
				if(CPU86<1) {
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,10);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,9);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,8);
	//			  PROCOper(LINE_TYPE_ISTRUZIONE,popString,"sp",NULL);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,0);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,1);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,2);
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
					}
				else  
					PROCOper(LINE_TYPE_ISTRUZIONE,"popa",OPDEF_MODE_NULLA,0);
				PROCOper(LINE_TYPE_ISTRUZIONE,"popf",OPDEF_MODE_NULLA,0);
#elif MC68000
				PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
					OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"a0-a6/d0-d7",0);
				PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"sr",0);
#elif GD24032
				PROCOper(LINE_TYPE_ISTRUZIONE,"LDM.d",
					OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"{R0-R23}",0,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1);
				PROCOper(LINE_TYPE_ISTRUZIONE,"LDST",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1);
#elif MICROCHIP
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,9);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,8);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,0);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,1);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,2);
				PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
#endif
			  }

#if ARCHI
				//boh?  v. SWI ecc
#elif Z80
			  PROCOper(LINE_TYPE_ISTRUZIONE,"reti",OPDEF_MODE_NULLA,0);
#elif I8086
				PROCOper(LINE_TYPE_ISTRUZIONE,"iret",OPDEF_MODE_NULLA,0);
#elif MC68000
				PROCOper(LINE_TYPE_ISTRUZIONE,"rte",OPDEF_MODE_NULLA,0);
#elif GD24032
				PROCOper(LINE_TYPE_ISTRUZIONE,"RETI",OPDEF_MODE_NULLA,0);
#elif MICROCHIP
				PROCOper(LINE_TYPE_ISTRUZIONE,"RETFIE",OPDEF_MODE_NULLA,0);
#endif

		  }

		if(!(CurrFunc->modif & FUNC_MODIF_INTERRUPT)) {
#if ARCHI
			if(FuncCalled) {
				if(CheckStack || CheckPointers) {
					PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
						OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)"PC",0);
					}  
	  		else
					PROCOper(LINE_TYPE_ISTRUZIONE,returnString,OPDEF_MODE_NULLA,0);
				}
#elif Z80 || I8086 || MC68000 || GD24032
		  	PROCOper(LINE_TYPE_ISTRUZIONE,returnString,OPDEF_MODE_NULLA,0);
#elif MICROCHIP
		  	PROCOper(LINE_TYPE_ISTRUZIONE,returnString,OPDEF_MODE_NULLA,0);
#endif
			}

#if ARCHI
#elif Z80
	  PROCOper(LINE_TYPE_DATA_DEF,CurrFunc->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"endp",0);
#elif I8086
	  PROCOper(LINE_TYPE_DATA_DEF,CurrFunc->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"ENDP",0);
#elif MC68000
		if(!(TipoOut & TIPO_SPECIALE))
		  PROCOper(LINE_TYPE_DATA_DEF,CurrFunc->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"ENDP",0);
#elif GD24032
	  PROCOper(LINE_TYPE_DATA_DEF,CurrFunc->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"ENDP",0);
#elif MICROCHIP
//		PROCOper(LINE_TYPE_DATA_DEF,"endp",9,(union SUB_OP_DEF*)&CurrFunc,0);
#endif
		swap(&LastOut,&OldTX[1].TX);
		PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"endfunction");
		if(Reg<Regs->MaxUser && !(CurrFunc->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
#if ARCHI
  		wsprintf(MyBuf,"R%u-R%u",Reg,Regs->MaxUser-1);
  		PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80
	  	for(i=Reg; i<Regs->MaxUser; i++)
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,i);
#elif I8086
	  	for(i=Reg; i<Regs->MaxUser; i++)
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,i);
#elif MC68000
//	  	for(i=Reg; i<Regs->MaxUser; i++)
//		  	PROCOper(LINE_TYPE_ISTRUZIONE,"move.l"/*popString*/,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,OPDEF_MODE_REGISTRO,i);
	  	wsprintf(MyBuf,"d%u-d%u",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"movem.l"/*pushString*/,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
	  	wsprintf(MyBuf,"{R%u-R%u}",Reg,Regs->MaxUser-1);
	  	PROCOper(LINE_TYPE_ISTRUZIONE,"LDM.d"/*popString*/,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
				OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif MICROCHIP
	  	for(i=Reg; i<Regs->MaxUser; i++)
	  	  PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,i);
#endif
	  	}
		if(AutoOff) {
#if ARCHI
	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
#elif Z80
      if(!CheckStack && abs(AutoOff)<=2)
		    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,3);
      else
		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
#elif I8086
      if(CPU86<1)
		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
//		  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,8,0);
#elif MC68000
//	    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_STACKPOINTER,0,OPDEF_MODE_FRAMEPOINTER,0);
//		  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,8,0);
#elif GD24032

#elif MICROCHIP
      if(!CheckStack && abs(AutoOff)<=1) {
//		    PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,3);
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFW",OPDEF_MODE_REGISTRO,11);
				}
      else {
		  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);
				if(StackLarge) {
//		  	PROCOper(LINE_TYPE_ISTRUZIONE,"MOVFF",OPDEF_MODE_FRAMEPOINTER,0,OPDEF_MODE_STACKPOINTER,0);		// FINIRE!
					}
				}
#endif
		  }
		LastOut=OldTX[1].TX;
		*OldTX[1].T=0;
		OldTX[1].TX=0;
		OldTX[1].AutoOff=0;
	  OldTX[1].id = __line__;		// v. altre, cmq ok qua
		AutoOff=0;
		Declaring=TRUE;		// v. C89 C99 :)
		Reg=Regs->MaxUser;
#if ARCHI		
		FuncCalled=FALSE;
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
#endif    
		TempSize=0;
		SaveFP=FALSE;

		Ottimizza(RootOut);
		// fare ricorsivamente fino a quando non ci sono più miglioramenti! 2026

		if(CurrFunc->modif & FUNC_MODIF_INLINE) {		// salvo il contenuto della inline! prima che venga flushata
			struct LINE *sl1=CurrFunc->definition,*sl2;
			i=0; j=0;
			CurrFunc->definition=sl2=(struct LINE*)GlobalAlloc(GPTR,sizeof(struct LINE));
			while(sl1 && (sl1->type != LINE_TYPE_COMMENTO || _tcsnicmp(sl1->rem,"----------",10))) {
//				PROCOut(sl->type,sl->opcode,&sl->s1,&sl->s2/*,&sl->s3*/);
				sl2->type=sl1->type;
				_tcscpy(sl2->opcode,sl1->opcode);
				sl2->s1=sl1->s1;
				sl2->s2=sl1->s2;
				_tcscpy(sl2->rem,sl1->rem);
				sl2->next=(struct LINE*)GlobalAlloc(GPTR,sizeof(struct LINE));
				sl2=sl2->next;
				sl1=sl1->next;
				i++;
				if(i>25 && !j) {		// diciamo :)
					PROCWarn(4710,"function too large for inlining");
					j=1;
					}
				}
			}
		PROCObj(FO4);            // flusha la funzione (ndr DEVE necessariamente cancellare quel che c'è o si incasina ...2026 per inline
		if(OutList) {
			PROCVarList(FLst,CurrFunc);
			}
		// cancella le variabili locali...
		while(CurrFuncGotos) {			    // cancello i goto
			struct VARS *g=CurrFuncGotos->next;
			GlobalFree(CurrFuncGotos);
			CurrFuncGotos=g;
			}
		CurrFunc=NULL;
		CurrFuncGotos=NULL;
		}
  InBlock--;     
					  
  return InBlock;
  }
  
int Ccc::PROCIsDecl() {
// Class= 0 SE EXTERN, 1 SE GLOBAL, 2 SE STATIC, 3 SE AUTO, 4 SE REGISTER
  int v,t,i;
	O_DIM dim={0};
	O_SIZE size=INT_SIZE;
	enum VAR_CLASSES Class;
  O_TYPE type=VARTYPE_PLAIN_INT;
	uint8_t modif=0l;
	uint32_t attrib=0;
  struct TAGS *tag=NULL;
  char *p;
  long OldTextp,OldTextp2;
  char T[64],MyBuf[256 /*STR_LONG ...*/];
  int ol;
  
  Class=InBlock>0 ? CLASSE_AUTO : CLASSE_GLOBAL;     // CLASSE DI DEFAULT: GLOBAL O AUTO
  OldTextp=FIn->GetPosition();
	FIn->SavePosition();
	ol=__line__;
  FNLO(T);                    // LEGGO UN ITEM

  OldTextp2=FIn->GetPosition();

  v=-1;
  do {
		i=FNIsClass(T);             // per gestire interrupt, pascal ecc.
		if(i>=0) {
		  if(i>=16) {
				modif |= i >> 4;
				}
		  else {
				v=i;
				if(i==CLASSE_GLOBAL)			// gestisco const, v.
					type |= VARTYPE_CONST;
			  }
		  OldTextp2=FIn->GetPosition();
		  FNLO(T);                 
		  }
		else {
			FIn->RestorePosition(OldTextp2);
			__line__=ol;
			}
		} while(i>=0);
	/*if(!_tcscmp(T,"const"))	{	// GESTIRE! usare ; v. anche di là
		type |= VARTYPE_CONST;
	  OldTextp2=FIn->GetPosition();
	  FNLO(T);                 
		}*/
  t=FNIsType(T);
  if(/*(m==0) && */(v<0) && (t==VARTYPE_NOTYPE) && (InBlock>0)) {
// SE NON SPECIFICA LA CLASSE, NE IL TIPO E SIAMO IN UN BLOCCO...
		if(Declaring) {
		  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"----------------------------------------");    // SEPARA LE DICHIARAZIONI DAL RESTO DELLA FUNZIONE
		  Declaring=FALSE;     
		  }
		if(OutSource) {
		  wsprintf(MyBuf,"<%5u>: ",__line__);
		  FNGetLine(OldTextp,MyBuf+9);		// 
//			MyBuf[_tcslen(MyBuf)-1]=0;		// tolgo CR se no diventa doppio
		  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
		  }
		FIn->RestorePosition(OldTextp);
		__line__=ol;
		FNEvalExpr(16,MyBuf);
// ...ALLORA E' UN'ESPRESSIONE
		PROCCheck(';');
		__line__=ol;
		}
  else {
		if(OutSource) {
		  wsprintf(MyBuf,"<%5u>: ",__line__);
		  FNGetLine(OldTextp,MyBuf+9);		// 
//			MyBuf[_tcslen(MyBuf)-1]=0;		// tolgo CR se no diventa doppio
		  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
		  }
		if(Declaring) {
// SE SI PUO' ANCORA DICHIARARE... v. C89 C99
		  if(v>=0 || modif>0) {
				OldTextp=FIn->GetPosition();
				FNLO(T);
				if(v>=0)
				  Class=(enum VAR_CLASSES)v;
				}           // SE SI SPECIFICAVA UNA CLASSE LEGGI IL PROSSIMO ITEM
			if(FNIsType(T)==VARTYPE_NOTYPE) {
//			  SE NON SI SPECIFICAVA (O SI SPECIFICA) UN TIPO TORNA INDIETRO
				FIn->RestorePosition(OldTextp);
				__line__=ol;
				}

			if(!_tcscmp(T,"unsigned") || !_tcscmp(T,"signed")) {
//				  OldTextp=FIn->GetPosition();
				}                  
			if(type & (VARTYPE_UNSIGNED | VARTYPE_UNION | VARTYPE_STRUCT)) {
//			  Var[Vars+1].tag=Var[Vars].tag;     // non si capisce...
				type &= VARTYPE_NOT_A_POINTER;
				}                  
			else {
				size=INT_SIZE;
				type |= VARTYPE_PLAIN_INT;		// vabbe' :)
				}
			PROCGetType(&type,&size,&tag,dim,&attrib,OldTextp);
			if(!_tcscmp(T,"short")/* || !_tcscmp(T,"signed")*/) {
			  FNLA(T);
				if(FNIsType(T)==VARTYPE_PLAIN_INT && !type) {		// PATCH rapida per "short int" ecc... MIGLIORARE
				  FNLO(T);
					}
				}

			goto primogiro;		// perché ho già l'ev. ptr qua! v.sotto, migliorare

		  do {
				
		if(debug) {
			char *tmp=(char*)(tag ? tag->label : "");/*NON CI PIACE @#£$% si incasina la printf del log...*/
			myLog->print(0,"TIPO: t=%x, c=%x, s=%x, tag=%s, dim=%d",type,Class,size,tmp,dim); 
			}

				type &= VARTYPE_NOT_A_POINTER;
				i=type & VARTYPE_ARRAY ? 0 : 0;		// gli array sono sempre anche puntatori, minimo
				while(*FNLA(MyBuf)=='*') {		// sarebbe da gestire in GetType... qua il * non appartiene al tipo dichiarato a inizio riga ma per ciascuno...
					FNLO(MyBuf);
					i++;
					}
				type=i;
				OldTextp=FIn->GetPosition();

				subGetType(&type, &size, dim, OldTextp);

primogiro:

				PROCDclVar(Class,modif,type,size,tag,dim,attrib,FALSE);
				FNLO(MyBuf);
				} while(*MyBuf==',');       // TUTTA UNA SERIE DI DICHIARAZIONI
		  }
		else {
		  PROCError(2143,"no more allocation allowed");
//		  PROCError(2062,"type"); o anche 2143 "...before type"
		  }
		      // qui non è bello... tiene conto delle funz
		if(*MyBuf=='{')
      FIn->unget('{');	
		else {
		  if(*MyBuf != ';')
		    PROCCheck(';');
			else
				__line__=ol;
		  }  
		}      
  return 0;
  }
 
int Ccc::PROCDclVar(enum VAR_CLASSES Class, uint8_t Modif, O_TYPE Type, O_SIZE Size, struct TAGS *tag, O_DIM dim, 
										uint32_t attrib, bool isParm) {
  int v,t1,T2,i;
  char T[128],nome[128],S[128],MyBuf[256 /*sizeof(union STR_LONG)*/];
  long OldTextp,t,t2;
  char *p;
  struct VARS *V;
	int ol;
	int8_t ndim=0;
	bool isInitialized,dontAllocate=0;
	COutputFile *myFO=NULL;			// file temporaneo per inizializzazione array di array o di stringhe
  
  *S=0;
  if((int32_t)(int16_t)Size==-2)		// marker per aggregato... MIGLIORARE
		return -1;
  OldTextp=FIn->GetPosition();
	ol=__line__;
  FNLO(nome);             // LEGGO UN ITEM
  // PRINT "Classe:"Class%" Tipo:"~type%" Size:"Size%" Dim:"dim%
  V=FNCercaVar(nome,TRUE);     // SE LA VARIABILE GIA' ESISTE... (nel blocco)
/*  if(V) {
//		if((Size != V->size) || ((V->classe>CLASSE_EXTERN) && (Class != V->classe))) 
		if(V->classe>CLASSE_EXTERN && Class >= V->classe)
	  	PROCError(2086,nome);
// SE LA DIMENSIONE E' DIVERSA O LA CLASSE, ERRORE
		} non è ancora ok per le static/global , ok gemini 2026*/
	if(V) {
		if(V->classe > CLASSE_EXTERN) {
			// 1. Se le classi non coincidono (es. STATIC vs GLOBAL) -> ERRORE
			if(Class != V->classe)
				PROCError(2086,nome);
			// 2. Se la classe è la stessa ma cambiano tipo o dimensione (es. long vs float) -> ERRORE
			else if((V->type & ~VARTYPE_FUNC_USED) != Type || V->size != Size || V->type & VARTYPE_INITIALIZED)
				PROCError(V->type & VARTYPE_FUNC ? 2371 : 2086,nome);
			// 3. Se siamo dentro una funzione (AUTO/REGISTER), vieta QUALSIASI ridefinizione nello stesso blocco
			else if(Class >= CLASSE_AUTO)		// var locali in diversi blocchi allo stesso livello ...
				PROCError(2086,nome);
			// Se arriviamo qui: sono due 'int x;' globali identiche (tentative definition valida)
			if(Class < CLASSE_AUTO)
				dontAllocate=TRUE;
			} 
		else {
			// Gestione EXTERN e Prototipi di funzioni
			if(V->type != Type)
				PROCError(2086,nome);
			}
		}
  if(!InBlock && (Class>CLASSE_STATIC)) {
		PROCWarn(2071,nome);
		Class=CLASSE_GLOBAL;        // AL LIVELLO PIU' ALTO NON CI POSSONO ESSERE Regs O AUTO
		}
  if(Class==CLASSE_REGISTER && FNGetMemSize(Type,Size,0/*dim*/,1) > INT_SIZE) {
		Class=CLASSE_AUTO;        // no reg + grandi di INT_SIZE
		}
  if(isParm && (Class<CLASSE_AUTO)) {
		PROCWarn(2071,nome);
		Class=CLASSE_AUTO;    // ...E COME PARAMETRI NIENTE STATICI O GLOBAL; anche se si "potrebbe" ... (v. microchip)
		}
  if(Class>CLASSE_STATIC && (Type & VARTYPE_FUNC)) {
	  if(!(Type & VARTYPE_FUNC_POINTER)) 
			Class=CLASSE_GLOBAL;           //   SE E' UNA FUNZIONE ED E' AUTO O REGISTER DIVENTA GLOBAL
		}
	if(Type & VARTYPE_FUNC && (Type & VARTYPE_CONST))			// solo C++?? ma...
//	  if(!(Type & VARTYPE_POINTER)) 
		PROCError(2071,nome);
	if(attrib & FUNC_ATTRIB_NAKED && Class == CLASSE_AUTO)
		PROCWarn(2215,nome);

	if(Type & VARTYPE_CONST) {
		char tempPath[256];
		if(Class>=CLASSE_AUTO)			// solo C++?? ma...
			PROCWarn(1002,"variabile const non statica");
		GetTempPath(255,tempPath);
		GetTempFileNameA(tempPath,"CCC",0,MyBuf);
		myFO=new COutputFile(MyBuf);		// sarebbe bello usare un CMemFile, v. 
		}
  if(!V) {
		V=PROCAllocVar(nome,Type,Class,Modif,Size,tag,dim);
		          //   SE LA VARIABILE NON ESISTEVA LA SI DICHIARA
		}
	V->attrib=attrib;
  if(Type & VARTYPE_FLOAT) {
		UseFloat=TRUE;
		}

  if(Type & VARTYPE_FUNC_POINTER) {			// patch... può esistere anche se var normale, pare (v. GetType
		while(*FNLA(MyBuf)==')') 
			FNLO(MyBuf);
	  }


  if(Type & VARTYPE_ARRAY) {			// SE E' UN ARRAY...

		if(Type & VARTYPE_2POINTER)                  // se (almeno) doppio puntatore o arr. di ptr
		  /* MA NO!! perché? 2025  Size=PTR_SIZE*/;

    v=FNGetArraySize(V);             // dim totale oggetto
#if MC68000
	  v=(v+2 /*STACK_ITEM_SIZE*/-1) & -2/*STACK_ITEM_SIZE*/;         //  pari
#elif GD24032
	  v=(v+4 /*STACK_ITEM_SIZE*/-1) & -4/*STACK_ITEM_SIZE*/;         //  dword
#else

#endif

		while(*FNLA(MyBuf)=='[') {
		  while(*FNLO(MyBuf) != ']');
		  }
//		  myLog->print(0,"Array: %s, tipo: %x, size %x\n",nome,type,Size);
		isInitialized=*FNLA(MyBuf) == '=';
		if(isInitialized)
			V->type |= VARTYPE_INITIALIZED;

		switch(Class) {
		  case CLASSE_EXTERN:
#if ARCHI
				PROCOut1(FO1,".",V->label);
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
//				PROCOut1(&StaticOut,Var[T1].label,NULL);
#endif
				break;

		  case CLASSE_GLOBAL:
		  case CLASSE_STATIC:                   // SE E' STATICO O GLOBAL
								// NOME DELL'ARRAY
#if ARCHI
				PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,".",V->label);
#elif Z80
				if(*FNLA(MyBuf) != '=') {
					itoa(v,MyBuf,10);
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,"\tdb ",MyBuf," dup (?)");     // ALLOCO v BYTES
					}
				else {
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,V->label,NULL);     // solo il nome
					}
#elif I8086
				if(*FNLA(MyBuf) != '=') {
					itoa(v,MyBuf,10);
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,"\tDB ",MyBuf," DUP (?)");     // ALLOCO v BYTES
					}
				else {
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,V->label,NULL);     // solo il nome
					}
#elif MC68000
				if(!isInitialized) {
					itoa(v,MyBuf,10);
					if(!(TipoOut & TIPO_SPECIALE)) {
						if(MemoryModel & MEMORY_MODEL_RELATIVE)
							;
						PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,V->label,"\tDB ",MyBuf," DUP (?)");     // ALLOCO v BYTES
						}
					else {
						if(MemoryModel & MEMORY_MODEL_RELATIVE)
							;
						PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,V->label,"\tds.b ",MyBuf);     // ALLOCO v BYTES
						}
					}
				else {
					if(MemoryModel & MEMORY_MODEL_RELATIVE)
						;
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,NULL);     // solo il nome
					}
#elif GD24032
				if(!isInitialized) {
					itoa(v,MyBuf,10);
					if(MemoryModel & MEMORY_MODEL_RELATIVE) 
						;
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,V->label,"\tDB ",MyBuf," DUP (?)");     // ALLOCO v BYTES
					}
				else {
					if(MemoryModel & MEMORY_MODEL_RELATIVE)
						;
					PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,NULL);     // solo il nome
					}
#elif MICROCHIP
				if(!isInitialized) {
					itoa(v,MyBuf,10);
					if(Type & VARTYPE_ROM) {
						if(CPUPIC<2) {
							sprintf(S,"%s\tDT",V->label);			// table per PCL
							}
						else {
							sprintf(S,"%s\tDA",V->label);			// char per lettura diretta
							}
						}
					else
						PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,"\tRES ",MyBuf);     // ALLOCO v BYTES
					}
				else {
					if(Type & VARTYPE_ROM) {
						if(CPUPIC<2) {
							sprintf(S,"%s\tDT",V->label);			// table per PCL
							}
						else {
							sprintf(S,"%s\tDA",V->label);			// char per lettura diretta
							}
						}
					else
						PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,V->label,"\tRES ",MyBuf);     // ALLOCO v BYTES, solo il nome; segue poi routine che copia da ROM a RAM
					}
#endif
#if ARCHI
				while(v>0) {
				  PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"DCD 0",NULL);     // ALLOCO 4 BYTES
				  v-=4;
				  }
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
#endif
#if ARCHI
				_tcscpy(S,"ALIGN");
#elif Z80 || I8086 || MC68000 || GD24032 || MICROCHIP
				*S='\t';
				*(S+1)=0;
#endif
				if(MemoryModel & MEMORY_MODEL_RELATIVE) {
					if(isInitialized) {		// dATA
						}
					else {		// BSS
						}
					}
				break;

		  case CLASSE_AUTO:
				if(isParm) {
//   SE E' UN PARAMETRO
				  AutoOff+=PTR_SIZE;            // OFFSET POSITIVI
				  }
				else {
#if MC68000
				  OldTX[InBlock].AutoOff-=(v+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  ALTRIMENTI NEGATIVI (e pari
#elif GD24032
				  OldTX[InBlock].AutoOff-=(v+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  ALTRIMENTI NEGATIVI (e pari
#else
				  OldTX[InBlock].AutoOff-=v;         // ALTRIMENTI NEGATIVI
#endif
				  }
//				sprintf(V->label,"%d",AutoOff);
				MAKEPTROFS(V->label)=OldTX[InBlock].AutoOff;			// 
				if(OutSource) {
  				sprintf(MyBuf,"%s = %d",nome,MAKEPTROFS(V->label));
				  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
				  }
				if(*FNLA(MyBuf) == '=') {
				  PROCError(1002,"init auto array");		// c'è anche altrove;  forse C99 ecc??
				  }
				break;
		  default:
				break;
	  	}  

		}		// se array
  else {
		isInitialized= *FNLA(MyBuf) == '=';
		if(isInitialized)
			V->type |= VARTYPE_INITIALIZED;
    Size=FNGetMemSize(Type,Size,0/*dim*/,1);
		switch(Class) {
// ALLOCO VAR. NORMALE
		  case CLASSE_GLOBAL:
		  case CLASSE_STATIC:
#if ARCHI
				sprintf(S,".%s DC",V->label);
#elif Z80
				sprintf(S,"%s\td",V->label);
#elif I8086
				sprintf(S,"%s\tD",V->label);
#elif MC68000
				if(!(TipoOut & TIPO_SPECIALE))
					wsprintf(S,"%s\tD",V->label);
				else
					wsprintf(S,"%s\td",V->label);
#elif GD24032
				wsprintf(S,"%s\tD",V->label);
#elif MICROCHIP
				if(Type & VARTYPE_ROM) {
					if(CPUPIC<2) {
						wsprintf(S,"%s\tD",V->label);			// table per PCL
						}
					else {
						wsprintf(S,"%s\tD",V->label);			// char per lettura diretta
						}
					}
				else
					wsprintf(S,"%s\tRES ",V->label);			// segue poi routine che copia da ROM a RAM
#endif
				switch(Size) {
				  case 1:
#if ARCHI
						_tcscat(S,"B 0");
#elif Z80
						_tcscat(S,"b 0");
#elif I8086
						_tcscat(S,"B 0");
#elif MC68000
						if(!(TipoOut & TIPO_SPECIALE))
							_tcscat(S,"B 0");
						else
							_tcscat(S,"c.b 0");
#elif GD24032
						_tcscat(S,"B 0");
#elif MICROCHIP
					if(Type & VARTYPE_ROM) {
						if(CPUPIC<2) {
							_tcscat(S,"T ");						// table per PCL
							}
						else {
							_tcscat(S,"A ");						// char per lettura diretta
							}
//						_tcscat(S,"B 0");
						}
					else {
						_tcscat(S,"1");
						}
#endif
					break;
				  case 2:
#if ARCHI
						_tcscat(S,"W 0");
#elif Z80
						_tcscat(S,"w 0");
#elif I8086
						_tcscat(S,"W 0");
#elif MC68000
						if(!(TipoOut & TIPO_SPECIALE))
							_tcscat(S,"W 0");
						else
							_tcscat(S,"c.w 0");
#elif GD24032
						_tcscat(S,"W 0");
#elif MICROCHIP
					if(Type & VARTYPE_ROM) {
						_tcscat(S,"W 0");
						}
					else {
						_tcscat(S,"2");
						}
#endif
						break;
		  		case 4:
#if ARCHI
						_tcscat(S,"D 0");
#elif Z80
						_tcscat(S,"d 0");
#elif I8086
						_tcscat(S,"D 0");
#elif MC68000
						if(!(TipoOut & TIPO_SPECIALE))
							_tcscat(S,"D 0");
						else
							_tcscat(S,"c.l 0");
#elif GD24032
						_tcscat(S,"D 0");
#elif MICROCHIP
						if(Type & VARTYPE_ROM) {
							_tcscat(S,"W 0,0");
							}
						else {
							_tcscat(S,"4");
							}
#endif
						break;
					case 0:
						if(!(Type & VARTYPE_FUNC))
							PROCError(1001,"size=0");		// beh sì :)
						break;
		  		default:
#if ARCHI
						sprintf(MyBuf,"B STRING$(%u,CHR$ 0)",Size);
#elif Z80
						sprintf(MyBuf,"b %u dup(0)",Size);
#elif I8086
						sprintf(MyBuf,"B %u DUP(0)",Size);
#elif MC68000
					  Size=(Size+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  pari
						if(!(TipoOut & TIPO_SPECIALE))
							wsprintf(MyBuf,"B %u DUP(0)",Size);
						else
							wsprintf(MyBuf,"s.b %u 0",Size);
#elif GD24032
					  Size=(Size+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  dword
						wsprintf(MyBuf,"B %u DUP(0)",Size);
#elif MICROCHIP
						if(Type & VARTYPE_ROM) {
							wsprintf(MyBuf,"B ");
							for(int i=0; i<Size; i++) 
								_tcscat(MyBuf,"0,");
							MyBuf[_tcslen(MyBuf)-1]=0;
							}
						else {
							wsprintf(MyBuf,"%u",Size);
							}
#endif
						_tcscat(S,MyBuf);
						break;
					}     
				if(MemoryModel & MEMORY_MODEL_RELATIVE) {
					if(isInitialized) {			// DATA
						}
					else {		// BSS
						}
					}
			break;

	  case CLASSE_AUTO:
L5080:
			if(!isParm) {
			  if((Type==VARTYPE_PLAIN_INT && Size==INT_SIZE) || (((Type & VARTYPE_IS_POINTER) > 0) && 
					(!(Type & (VARTYPE_UNION | VARTYPE_STRUCT | VARTYPE_ARRAY /*0x1C00*/))))) {
//	  			AutoOff=(AutoOff & -STACK_ITEM_SIZE)-Size;	mah credo fosse sbagliato
#if MC68000									// verificare per altri
					if(OldTX[InBlock].AutoOff & 1)			// pari cmq
						OldTX[InBlock].AutoOff--;
#elif GD24032
					if(OldTX[InBlock].AutoOff & 1)			// pari cmq VERIFICARE QUA
						OldTX[InBlock].AutoOff--;
#endif
				  OldTX[InBlock].AutoOff-=(Size+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;
		  		}
			  else {
#if MC68000
					if(Size>=2  /*STACK_ITEM_SIZE*/) {
						if(OldTX[InBlock].AutoOff & 1)			// pari cmq
							OldTX[InBlock].AutoOff--;
						OldTX[InBlock].AutoOff-=(Size+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  pari
						}
					else {
						OldTX[InBlock].AutoOff-=Size;
						}
#elif GD24032
					if(Size>=4 /*STACK_ITEM_SIZE*/) {
						if(OldTX[InBlock].AutoOff & 1)			// pari cmq VERIFICARE
							OldTX[InBlock].AutoOff--;
						OldTX[InBlock].AutoOff-=(Size+STACK_ITEM_SIZE-1) & -STACK_ITEM_SIZE;         //  pari
						}
					else {
						OldTX[InBlock].AutoOff-=Size;
						}
#else
					OldTX[InBlock].AutoOff-=Size;		// verificare per altri
#endif
					}
				MAKEPTROFS(V->label)=OldTX[InBlock].AutoOff;
		  	}			// non parm
			else {
	// GESTISCO IL PARAMETRO
#if MC68000
				if(AutoOff>0 && Size==1)		// (was: a causa di Big-endian, per accedere ai parm char pushati come word devo spostarmi! NO!
					MAKEPTROFS(V->label)=AutoOff;
				else
					MAKEPTROFS(V->label)=AutoOff;
#elif GD24032
				if(AutoOff>0 && Size==1)		// (was: a causa di Big-endian, per accedere ai parm char pushati come word devo spostarmi! NO!
					MAKEPTROFS(V->label)=AutoOff;
				else
					MAKEPTROFS(V->label)=AutoOff;
#else
				MAKEPTROFS(V->label)=AutoOff;
#endif
			  if(Size<=STACK_ITEM_SIZE)
					AutoOff+=STACK_ITEM_SIZE;
			  else 
					AutoOff+=Size;
			  }
//			sprintf(V->label,"%d",AutoOff);
			if(OutSource) {
  			itoa(AutoOff,MyBuf,10);
 				wsprintf(MyBuf,"%s = %d",nome,MAKEPTROFS(V->label));
			  PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
			  }
			break;

	  case CLASSE_REGISTER:
			t1=FNRegFree();
			if(t1) {
// SE NON HO Regs DIVENTA AUTO
//		  	sprintf(V->label,"%d",t1);
  			MAKEPTRREG(V->label)=t1;
		  	if(isParm) {
					if(!(V->func->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE))) {
						OldTX[InBlock].AutoOff+=STACK_ITEM_SIZE; 
//					FNGetFPStr(MyBuf,AutoOff,NULL);
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label),
							OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff);
#elif Z80
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff+1);
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
#elif I8086
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,MAKEPTRREG(V->label),
							OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label),
							OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff);
#elif GD24032
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff
							,OPDEF_MODE_REGISTRO32,MAKEPTRREG(V->label));
#elif MICROCHIP
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,OldTX[InBlock].AutoOff+1);
						PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,0);
						PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,MAKEPTRREG(V->label));
#endif
// PARAMETRO REGISTRO opp fastcall
						}
					else if(V->func->modif & FUNC_MODIF_INLINE) {
		  			MAKEPTRREG(V->label)=Regs->MaxUser-1-t1;			// inizio a usare da D0/R0/AL
						}
					else {			// aggiusto #registro 
		  			MAKEPTRREG(V->label)=Regs->MaxUser-1-t1;			// inizio a usare da D0/R0/AL
						}
						}
		  	if(OutSource) {
		  	  sprintf(MyBuf,"register %s = %s",(*Regs)[V],nome);
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
					}
				  }
				else {
					if(!(V->func->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE)))
						;
					else
						PROCWarn(4042,"nessun registro disponibile");
				  V->classe=Class=CLASSE_AUTO;
				  goto L5080;
				  }
				break;
		  default:
				break;
		  }
		}

  ol=__line__;
  FNLO(MyBuf);
  switch(*MyBuf) {
		case '(':
			if(!(V->type & VARTYPE_FUNC_POINTER))
				*S=0;
		  T2=1;
		  t=FIn->GetPosition();
		  do {
				FNLO(T);
				switch(*T) {
				  case '(':
						T2++;
						break;
				  case ')':
						T2--;
						break;
				  case 0:
						PROCError(2059,T);
						break;
				  }
				} while(T2);
		  t2=FIn->GetPosition();
		  FNLA(T);
			if(*T == ',') {
			  FNLO(T);
				goto do_declare_ext;
				}
		  if(*T != ';') {		// ossia se segue '{'
				if(V->type & VARTYPE_FUNC_POINTER)
				  PROCError(2054,";");
				else if(V->type & VARTYPE_FUNC_BODY)
				  PROCError(2084,nome);
				else {
				  V->type |= VARTYPE_FUNC_BODY;
				  V->classe = Class;
				  if(Class==CLASSE_STATIC) {
//						sprintf(V->label,"$%s_0",FNGetLabel(MyBuf,0));      // mezza boiata...
						FNGetLabel(MyBuf,3);
						_tcscpy(V->label,MyBuf);
//						_tcscat(V->label,"_0");        // non capisco...
						}
				  }
				if(V->modif & FUNC_MODIF_INTERRUPT) {
					if(V->size>0)
						PROCError(3001,nome);
					}
				if(InBlock>0)
				  PROCError(2599,nome);
				InBlock++;
			  OldTX[InBlock].id = __line__;		// o rand()? v. altrove
				Declaring=TRUE;
				Regs->Reset();
				CurrFunc=V;
// COMINCIO AD ALLOCARE LE VARIABILI locali parm
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0);
#if ARCHI
				AutoOff=STACK_ITEM_SIZE;
				PROCOutLab(nome);            // INDIRIZZO FUNZIONE
#elif Z80
				AutoOff=2*STACK_ITEM_SIZE;		// (was : in DclVar aggiungiamo subito 2, PRIMA; (SEMBRA strano, 2025: dovremmo partire da 2+2 ossia caller address e vecchio FramePtr
				if(V->classe == CLASSE_GLOBAL)
				  PROCOper(LINE_TYPE_DATA_DEF,"public",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"proc",0);            // INDIRIZZO FUNZIONE
#elif I8086
				AutoOff=2*STACK_ITEM_SIZE;		// MemoryModel
				if(V->classe == CLASSE_GLOBAL)
  				PROCOper(LINE_TYPE_DATA_DEF,"PUBLIC\t",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,
					(union SUB_OP_DEF*)((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? "PROC FAR" : "PROC NEAR"),0);
#elif MC68000
				if((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE)
					AutoOff=2*4;		// (was: in DclVar aggiungiamo subito 2, PRIMA; (v.Z80
				else	// no, cmq abbiamo 2 dword pushate, sp e fp
					AutoOff=2*4;
				if(V->classe == CLASSE_GLOBAL) {
					if(!(TipoOut & TIPO_SPECIALE))
  					PROCOper(LINE_TYPE_DATA_DEF,"public\t",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
					}
				if(!(TipoOut & TIPO_SPECIALE))
					PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"PROC",0);
				else
					PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"",0);		// se metto ":" easy68k si incazza @#$%
#elif GD24032
				AutoOff=2*4;
				if(V->classe == CLASSE_GLOBAL) {
 					PROCOper(LINE_TYPE_DATA_DEF,"PUBLIC\t",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
					}
				PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"PROC",0);
#elif MICROCHIP
				AutoOff=STACK_ITEM_SIZE;
				if(V->classe == CLASSE_GLOBAL)
				  PROCOper(LINE_TYPE_DATA_DEF,"GLOBAL",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
			  PROCOper(LINE_TYPE_DATA_DEF,V->label,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)":",0);
#endif
				OldTX[0].TX=LastOut;
				*OldTX[0].T=0;
				FIn->RestorePosition(t);
				__line__=ol;
				FNLO(T);
				if(*T != ')') {
// CI SONO PARAMETRI
					*(int *)V->parm=0;
// PROTOTIPI
				  v=FNIsClass(T);
				  if((v>=0) || (FNIsType(T) != VARTYPE_NOTYPE)) {
						int totParm=0;
						int *parmPtr;

//						FIn->Seek(t,CFile::begin);
					  do {
						  v=FNIsClass(T);
							if(!_tcscmp(FNLA(T),"const"))	{	// GESTIRE! usare v. anche di là
								FNLO(T);                 
								}
						  if(v>=0) {
								v &= 0xf;
								Class=(enum VAR_CLASSES)v;
								t=FIn->GetPosition();
								FNLO(T);
								}
							else {
								if(!(V->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE)))
									Class=CLASSE_AUTO;
								else
									Class=CLASSE_REGISTER;
							  }

							Size=INT_SIZE;
							Type=VARTYPE_PLAIN_INT;
							tag=NULL;

							PROCGetType(&Type,&Size,&tag,dim,&attrib,t);

								if(debug)  {
									char *tmp=(char*)(tag ? tag->label : "");/*NON CI PIACE @#£$% si incasina la printf del log...*/
								myLog->print(0,"TIPO in FUNZ: t=%x, s=%x, tag=%s, dim=%d\n",Type,Size,tmp,dim);
								}

							PROCDclVar(Class,0,Type,Size,tag,dim,attrib,TRUE);
	  					if(Type & VARTYPE_POINTER && Size==0)
								Size=PTR_SIZE;     // scavalco VOID, ma non void*
	  					if(FNGetMemSize(Type,Size,0/*dim*/,0) != 0) {     // scavalco VOID, ma non void*
								if(!(V->modif & FUNC_MODIF_FASTCALL)) 		// e inline pure
									parmPtr=(int*)V->parm;
								else
									parmPtr=(int*)V->parm;
								i=totParm;
								if(parmPtr[0]) {		// se c'è già stato un prototipo per la funzione, ricontrollo i parm
									if(parmPtr[i*2+1] != Type || parmPtr[i*2+2] != Size)
									  PROCError(2082,nome);		// mettere sia nome funzione che parm diverso...
									}
								parmPtr[i*2+1]=Type;
								parmPtr[i*2+2]=Size;
								totParm=i+1;
								if(totParm>20)
								  PROCError(1001,"func parm > 20");

								}
							FNLO(T);
							if(*T == ',') {
							  t=FIn->GetPosition();
							  FNLO(T);
							  }
							else {
							  if(*T != ')')
  								PROCError(2059);
							  }
							} while(*T != ')');
						parmPtr[0]=totParm;
					  }		// no class no type
				  else {
// DICHIARAZIONE OLD STYLE                 // non è completa, non controlla parm con lista
  					*(int *)V->parm=0;
						PROCWarn(4131);
						FIn->RestorePosition(t2);
						__line__=ol;
						FNLO(T);
						do {
						  v=FNIsClass(T);
						  if(v>0)
							v &= 0xf;
						  if(v>=0) {
								Class=(enum VAR_CLASSES)v;
								t2=FIn->GetPosition();
								}
							else {
								if(!(V->func->modif & (FUNC_MODIF_FASTCALL | FUNC_MODIF_INLINE)))
									Class=CLASSE_AUTO;
								else
									Class=CLASSE_REGISTER;
							  }
						  if(FNIsType(FNLA(MyBuf)) != VARTYPE_NOTYPE) 
								FNLO(T);
						  do {
								Size=INT_SIZE;
							  Type=0;
							  tag=NULL;
								PROCGetType(&Type,&Size,&tag,dim,&attrib,t2);
								PROCDclVar(Class,0,Type,Size,tag,dim,attrib,TRUE);
								} while(*FNLO(MyBuf) == ',');
						  t2=FIn->GetPosition();
						  FNLO(T);
						  } while(*T != '{');
						FIn->RestorePosition(t2);
						__line__=ol;
						}
				  }
//				else
//     		  FIn->Seek(-1,SEEK_CUR);
				if(V->modif & FUNC_MODIF_INTERRUPT) {
					if(*(int*)V->parm>0)
						PROCError(3002,nome);
					}
#if MC68000
				if(AutoOff > (((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) ? (2 /*in effetti no, sono le 2 dword pushate */ *4) : (2*4)))     // serve a ricordarsi che ho dei parm. (non se fastcall
				  SaveFP=TRUE;
#elif GD24032
				if(AutoOff > (((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) ? (2 /*in effetti no, sono le 2 dword pushate */ *4) : (2*4)))     // serve a ricordarsi che ho dei parm. (non se fastcall
				  SaveFP=TRUE;
#elif I8086
				if(AutoOff > (((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM) ? (2*4) : (2*2)))     // serve a ricordarsi che ho dei parm. (non se fastcall
				/*CPU86*/   // serve a ricordarsi che ho dei parm. (non se fastcall
				  SaveFP=TRUE;
#elif ARCHI
				if(AutoOff>STACK_ITEM_SIZE )                      // serve a ricordarsi che ho dei parm. (non se fastcall
				  SaveFP=TRUE;
#else
				if(AutoOff>STACK_ITEM_SIZE   *2 /*VERIFICARE 2025*/)                      // serve a ricordarsi che ho dei parm. (non se fastcall
				  SaveFP=TRUE;
#endif
				InBlock=0;
				Declaring= TRUE;
    		PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"----------------------------------------");
	// serve quando la funzione è vuota
				LastOut=LastOut->prev;
//				OldTX[0].TX=LastOut;
#if MC68000
				if((!_tcscmp(nome,"main")) && (AutoOff>((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? 2*4 : 2*4))) {		// idem
#elif GD24032
				if((!_tcscmp(nome,"main")) && (AutoOff>(2*4))) {		// idem
#elif I8086
				if((!_tcscmp(nome,"main")) && (AutoOff>((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE ? 2*4 : 2*2))) {
#else
				if((!_tcscmp(nome,"main")) && (AutoOff>2*STACK_ITEM_SIZE)) {
#endif
// main PUO' AVERE PARAMETRI DELLA COMMAND LINE
#if ARCHI
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
//				  PROCOper(LINE_TYPE_CALL,"BL __CLArgs",NULL,NULL,NULL,NULL);
	      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO32,1,
						OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,12);
				  PROCOper(LINE_TYPE_ISTRUZIONE,storString,OPDEF_MODE_REGISTRO32,0,
						OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,8);
//				  PROCOper(LINE_TYPE_ISTRUZIONE,storString,"STR R0,[",Regs->FpS,",#8]",NULL,NULL);
				  FuncCalled=TRUE;
#elif Z80
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
	      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,7,OPDEF_MODE_REGISTRO_HIGH8,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,6,OPDEF_MODE_REGISTRO_LOW8,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,5,OPDEF_MODE_REGISTRO_HIGH8,1);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,4,OPDEF_MODE_REGISTRO_LOW8,1);
#elif I8086
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
	      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"mov WORD PTR",
						OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)1,4,OPDEF_MODE_REGISTRO16,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"mov WORD PTR",
						OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,2,OPDEF_MODE_REGISTRO16,1);
					// MemoryModel!
#elif MC68000
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
					if(MemoryModel & MEMORY_MODEL_RELATIVE) {
						// finire!
		      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
						}
					else
		      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,8);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,1,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,4);
					// MemoryModel!
#elif GD24032
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
					if(MemoryModel & MEMORY_MODEL_RELATIVE) {
	          wsprintf(MyBuf,"%s-$",V->label);       // 
	      		PROCOper(LINE_TYPE_CALL,"CALLR",OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&MyBuf,0);
						}
					else
	      		PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,8,OPDEF_MODE_REGISTRO32,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_FRAMEPOINTER_INDIRETTO,0,4,OPDEF_MODE_REGISTRO32,1);
#elif MICROCHIP
		      V=FNCercaVar("_CLArgs",0);
	  	    if(!V)
	    		  V=PROCAllocVar("_CLArgs",VARTYPE_FUNC,CLASSE_EXTERN,0,0,0,0);	
	      	PROCOper(LINE_TYPE_CALL,callString,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&V->label,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,7,OPDEF_MODE_REGISTRO_HIGH8,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,6,OPDEF_MODE_REGISTRO_LOW8,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,5,OPDEF_MODE_REGISTRO_HIGH8,1);
				  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_FRAMEPOINTER_INDIRETTO,(union SUB_OP_DEF*)0,4,OPDEF_MODE_REGISTRO_LOW8,1);
#endif
				  }
				if(V->modif & FUNC_MODIF_INTERRUPT) {
//mah 2025				if(V->classe & CLASSE_INTERRUPT) {         // interrupt
					UseIRQ=1;				
				  }
				AutoOff=0;
				}
			else {
				FIn->RestorePosition(t);
				__line__=ol;
				FNLO(T);
				if(*T != ')') {
				  if(*T=='.' || FNIsType(T) != VARTYPE_NOTYPE) {
// PROTOTIPI e basta
  					*(int *)V->parm=0;
					  do {
							if(*T == '.') {            // gestisco ... variable parm
								int *parmPtr;
								parmPtr=(int*)V->parm;
								i=parmPtr[0];
								parmPtr[i*2+1]=-1;
								parmPtr[i*2+2]=0;
								parmPtr[0]=i+1;
//					    myLog->print("trovo ... e scrivo a %x\n",p+2+i*8);
								FIn->RestorePosition(t2);
								__line__=ol;
							  break;
							  }
							Size=INT_SIZE;
							Type=0;
							tag=NULL;
							PROCGetType(&Type,&Size,&tag,dim,&attrib,t);

							if(debug) {
								char *tmp=(char*)(tag ? tag->label : "");/*NON CI PIACE @#£$% si incasina la printf del log...*/
								myLog->print(0,"TIPO in prototyp: t=%x, s=%x, tag=%s, dim=%d\n",Type,Size,tmp,dim);
								}

	  					if(Type & VARTYPE_POINTER && Size==0)
								Size=4;     // scavalco VOID, ma non void*
	  					if(FNGetMemSize(Type,Size,0/*dim*/,0) != 0) {     // scavalco VOID, ma non void*
								int *parmPtr;
								parmPtr=(int*)V->parm;
								i=parmPtr[0];
								parmPtr[i*2+1]=Type;
								parmPtr[i*2+2]=Size;
								parmPtr[0]=i+1;
								if(i>20)
								  PROCError(1001,"max func parm");
								}  
							FNLO(T);
							if(*T == ',') {
							  t=FIn->GetPosition();
							  FNLO(T);
							  }
							else if(*T != ')')
								PROCError(2059);
							} while(*T != ')');
					  }
  				}
			  }	
		  break;

		case '=':          // INIZIALIZZAZIONI
		  switch(Class) {
				case CLASSE_EXTERN:
					PROCWarn(2205);		// non è chiaro, forse in C++ si può (solo se fuori blocco) ma in C boh
					break;
				case CLASSE_GLOBAL:
				case CLASSE_STATIC:
					{
					O_SIZE s2=FNGetMemSize(Type,Size,0/*dim*/,2);
					if(Type & VARTYPE_ARRAY) {
						char MyBuf1[128];
	//				myLog->print("Inizz array: %s, tipo %x, size %d\n",nome,type,Size);
						i=0;
	//					p=strstr(StaticOut->s,"\td");
	//					*p=0;
						PROCCheck('{');
						do {									// bisognerebbe gestire le dim array in init..
							if(/* *FNLA(MyBuf1) != ','*/  !*FNLA(MyBuf1) || *FNLA(MyBuf1) == '}')			// OCCHIO se c'è la virgola DOPO ultimo elemento lo prende come 0...
																																											// pare ok ora
								break;
							FNGetConst(MyBuf,1);
							switch(s2) {		// servirebbe messaggio se costante troppo grande per tipo var
								case 1:
#if MC68000
									if(!(TipoOut & TIPO_SPECIALE))
										PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"\tdb\t",MyBuf);
									else
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"\tdc.b\t",MyBuf);
#elif GD24032
									PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"\tDB\t",MyBuf);
#elif ARCHI
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"\tDCB\t",MyBuf);
#else
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO1,"\tdb\t",MyBuf);
#endif
  								break;
								case 2:
#if MC68000
									if(!(TipoOut & TIPO_SPECIALE))
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdw\t",MyBuf);
									else
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdc.w\t",MyBuf);
#elif GD24032
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tDW\t",MyBuf);
#elif ARCHI
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tDCW\t",MyBuf);
#else
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdw\t",MyBuf);
#endif
  								break;
								case 4:
#if MC68000
									if(!(TipoOut & TIPO_SPECIALE))
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdd\t",MyBuf);
									else
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdc.l\t",MyBuf);
#elif GD24032
	  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tDD\t",MyBuf);
#elif ARCHI
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tDCD\t",MyBuf);
#else
  								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tdd\t",MyBuf);
#endif
  								break;
								}
							i++;
							} while(*FNLO(MyBuf) == ',');
						if(dim[ndim]>0) {
							if(i > dim[ndim])
								PROCError(2078);
							}
						else
							dim[ndim]=i;
						ndim++;
						if(ndim>MAX_DIM-1)
							PROCError(1002,"dimensioni max=4");
						memcpy(V->dim,dim,sizeof(O_DIM));
  					*S=' ';	
#if MC68000
						if(i && s2==1)
							if(!(TipoOut & TIPO_SPECIALE))
								PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tds.w 0",NULL);		// ossia ALIGN a word: dice che l'assembler lo fa in automatico, ma...
#elif GD24032
						if(i && s2==1)
							PROCOut1(Type & VARTYPE_CONST ? myFO : FO2,"\tALIGN 4",NULL);		// 
#endif
						}
					else {
#if ARCHI
						p=strstr(S,"DC");
						*(p+4)=0;
						FNGetConst(MyBuf,1);
						_tcscat(S,MyBuf);
#elif Z80
						p=strstr(S,"\td");
						*(p+4)=0;
						FNGetConst(MyBuf,1);
						_tcscat(S,MyBuf);
#elif I8086
						p=strstr(S,"\tD");
						*(p+4)=0;
						FNGetConst(MyBuf,1);
						_tcscat(S,MyBuf);
#elif MC68000
						if(!(TipoOut & TIPO_SPECIALE)) {
							p=strstr(S,"\tD");
							*(p+4)=0;
							}
						else {
							p=strstr(S,"\tdc.");			// gestiamo entrambi...
							if(!p)
								p=strstr(S,"\tds.");
							*(p+6)=0;
							}
						FNGetConst(MyBuf,1);
						_tcscat(S,MyBuf);
	//					if(i && s2==1)		// bisognerebbe farlo solo se quello prima ha lasciato dispari...
	//						_tcscat(S,"\n\tds.w 0");		// ossia ALIGN a word: dice che l'assembler lo fa in automatico, ma...
						// schifezza ma è pratico!
#elif GD24032
						p=strstr(S,"\tD");
						*(p+4)=0;
						/*struct CONS myC=*/FNGetConst(MyBuf,1);
						if(MemoryModel & MEMORY_MODEL_RELATIVE)
							;
						_tcscat(S,MyBuf);
#elif MICROCHIP
						FNGetConst(MyBuf,1);
						_tcscat(S,";");		// finire !! allocare in rom... per copiatura
						_tcscat(S,MyBuf);
#endif
						}
					}
					break;
				case CLASSE_AUTO:
				case CLASSE_REGISTER:
	//				if(Type & VARTYPE_ARRAY) 
	// c'è già sopra					PROCError(1002,"inizializzazione array auto");		// forse C99 ecc??
					FIn->RestorePosition(OldTextp);
					__line__=ol;
	//				*MyBuf=0;
					FNEvalExpr(15,MyBuf);
					break;
				}
		  break;
		default:
do_declare_ext:
			FIn->unget(*MyBuf);		//FIn->Seek(-1,CFile::current);     
			__line__=ol;
		  break;
		}

	if(!dontAllocate) {
		if(*S) {
			if(Type & VARTYPE_CONST) {
				int ch;
				myFO->Seek(0,CFile::begin);
				while((ch=myFO->get()) != EOF)			// inserisco l'array!
					FO3->put(ch);
				PROCOut1(FO3,S," ;",nome,NULL);
				}
			else {
				if(MemoryModel & MEMORY_MODEL_RELATIVE)
					;
				if(!isInitialized)
					PROCOut1(FO2,S," ;",nome,NULL);
				else
					PROCOut1(FO1,S," ;",nome,NULL);
		//				PROCOut1((*S==' ' /*|| *S=='\t' pare ok così 2025*/ ) ? FO2 : FO1,S," ;",nome,NULL);
				}
	#if ARCHI
			if(_tcscmp(S,"ALIGN"))
				PROCOut1(FO1,"ALIGN",NULL);
	#elif Z80 || I8086 || MICROCHIP
	#elif MC68000 
	//	  if(_tcscmp(S,"ALIGN"))
	//			PROCOut1(FO2,"\tds.w 0",NULL);		// boh no so a cosa servisse, cmq ossia ALIGN a word
	#elif GD24032
	#endif      
			}
		}
	
	if(myFO) {
		myFO->Close();
		CFile::Remove(myFO->GetFilePath());
		delete myFO;
		}
	   
  return 0;
  }
	 
int Ccc::subAsm(char *s) {
  char MyBuf[64],buf[3][64],ch;
  struct VARS *V;
  int i,ol;
  long ot;
	int8_t state=0;
	bool go=FALSE;
	struct OP_DEF u[3];
  
  *MyBuf=0;
	*buf[0]=*buf[1]=*buf[2]=0;
	ZeroMemory(&u,sizeof(u));

  FNLO(s);
	ot=FIn->GetPosition();
	ol=__line__;
  do {
//    l=__line__;
		  myLog->print(0,"SUBASM: %s,  %d, ",s,state);
		switch(state) {
			case 0:		// istruzione
				_tcscpy(buf[0],s);
#if MC68000
				FNLA(MyBuf);
				if(*MyBuf == '.') {
					FNLO(MyBuf);
					_tcscat(buf[0],MyBuf);
					FNLO(s);
					_tcscat(buf[0],s);
					}
#elif GD24032

#else
				// ev altre varianti...
#endif
				state++;
				break;
			case 1:		// 1st op
			case 2:		// 2nd op
#if ARCHI || GD24032
			case 3:		// 3nd op		(Archimedes, GD24032
#endif
			  if(*s==',') {
					}
				else {
					if(FNIsOp(s,0)) {
//  					_tcscat(buf[state],s);
						// virgola, implicita; oppure parentesi, ++ ecc
						state--;
						}
					else if(i=Regs->FNIsReg(s)) {		// 
					  u[state-1].mode=OPDEF_MODE_REGISTRO32;
					  u[state-1].s.n=i-1;
						}
					else if(V=FNCercaVar(s,FALSE)) {
						switch(V->classe) {
							case CLASSE_EXTERN:
							case CLASSE_GLOBAL:
							case CLASSE_STATIC:
							  u[state-1].mode=OPDEF_MODE_VARIABILE_INDIRETTO;
								_tcscpy(u[state-1].s.label,V->label);
								break;
							case CLASSE_AUTO:
								i=MAKEPTROFS(V->label);
#if MC68000
				        sprintf(MyBuf," %+d(%s)",i,Regs->FpS);
#elif GD24032
				        sprintf(MyBuf,"%s%+d",Regs->FpS,i);
#else
					      sprintf(MyBuf," %s%+d",Regs->FpS,i);
#endif
							  u[state-1].mode=OPDEF_MODE_FRAMEPOINTER_INDIRETTO;
								u[state-1].ofs=MAKEPTROFS(V->label);
								break;
							case CLASSE_REGISTER:
							  u[state-1].mode=OPDEF_MODE_REGISTRO32;
								u[state-1].s.n=MAKEPTRREG(V->label);
								break;
							}
						}
					else {
#if MC68000
			      if(*s=='#' || isdigit(*s))
						  u[state-1].mode=OPDEF_MODE_IMMEDIATO32;
#elif _6502
			      if(*s=='#' || isdigit(*s))
						  u[state-1].mode=OPDEF_MODE_IMMEDIATO16;
#else
			      if(isdigit(*s))
						  u[state-1].mode=OPDEF_MODE_IMMEDIATO32;
#endif
					  u[state-1].s.n=atoi(s);
						}
					state++;
					}
				break;
			default:
				go=TRUE;
				break;
			}
	  if(*s=='}') {
			FIn->RestorePosition(ot);
			__line__=ol;
			return 0;
	    }
		else {
			ot=FIn->GetPosition();
			ch=FIn->get();
			if(ch=='\n') {
	//			FIn->RestorePosition(ot);
				go=TRUE;
				}
			else {
				FIn->unget(ch);
				FNLO(s); 
				}
			}

		} while(!go);

//  PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf,OPDEF_MODE_NULLA,0);		// ATTENZIONE lim max= sizeof(opcode), SPEZZARE operandi

#if ARCHI || GD24032
  if(buf[2][0])
	  PROCOper(LINE_TYPE_ISTRUZIONE,buf[0],&u[0],&u[1],&u[2]); 
	else
#endif
	  PROCOper(LINE_TYPE_ISTRUZIONE,buf[0],&u[0],&u[1]);  
	
  
  return 1;
  }

int Ccc::FNIsStmt() {
  int OldDcl,T1,I,i,c;
  struct LINE *t,*t1,*t2;
  char TS[128],T1S[64],C[sizeof(union STR_LONG)],MyBuf[128],MyBuf1[64];
	uint32_t attrib=0;
  char *p;
  long OldTextp,l,l1;
	int ol;
  
  OldDcl=Declaring;
  Declaring=FALSE;
rifoStmt:  
  OldTextp=FIn->GetPosition();
	ol=__line__;
  t=LastOut;
  FNLO(TS);
  switch(*(WORD*)TS) {
		case '#':
			FNLO(MyBuf);
			FNLO(MyBuf);
			__line__=atoi(MyBuf);
			FNLO(MyBuf);
			_tcsncpy(__file__,MyBuf+1,_tcslen(MyBuf)-2);
			__file__[_tcslen(MyBuf)-2]=0;
			FIn->get();		// mangio il CR per non alterare contatore line!
			Declaring=OldDcl;
			return TRUE;
			break;
		case 'a_':
			if(!_tcscmp(TS,"_asm")) {                 // CODICE Assembly
				if(*FNLA(MyBuf) == '{') {
					FNLO(MyBuf);
				  FNLA(MyBuf);
					if(*MyBuf!='}') {		// safety se vuoto!
						for(;;) {
							subAsm(MyBuf);
							FNLA(MyBuf);
							if(!*MyBuf || *MyBuf == '}') {
								FNLO(MyBuf);
								break;
								}
							}
						}
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\");
					}
				else {
					subAsm(MyBuf);
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\");
					}
				}
			else
				goto noStmt;
		  break;
		case 'rb':
			if(!_tcscmp(TS,"break")) {
				T1=InBlock;
				while(T1) {
					if(*OldTX[T1].B) {
	#if ARCHI
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
	#elif Z80
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
	#elif I8086
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
	#elif MC68000
						PROCOper(LINE_TYPE_JUMP,((MemoryModel & 0xf) <= MEMORY_MODEL_MEDIUM) ? jmpShortString : jmpString,
							OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
#elif GD24032
						PROCOper(LINE_TYPE_JUMP,jmpShortString,
							OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
	#elif MICROCHIP
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].B,0);
	#endif
						T1=1;
						}
					T1--;
					if(T1==1)
						PROCError(2043);
					}
				}
			else 
				goto noStmt;
	    break;
		case 'ac':
			if(!_tcscmp(TS,"case")) {
				_tcscpy(T1S,OldTX[InBlock].T);
				if(*T1S != '&') {
					PROCError(2046);
					}
				else {
					p=OldTX[InBlock].parm;
					i=*((int *)p);
					
					c=FNGetConst(C,0);
					if((OldTX[InBlock].C[1]==1 && abs(c) & 0xffffff00) || (OldTX[InBlock].C[1]==2 && abs(c) & 0xffff0000))
						PROCWarn(2053);
					if(OldTX[InBlock].C[1]==1)
						c &= 0xff;
					else if(OldTX[InBlock].C[1]==2)
						c &= 0xffff;
					*((int *)(p+sizeof(int)+i*sizeof(int)))=c;
					*((int *)p)=i+1;
					while(i--) {
						p+=sizeof(int);
						if(*(int *)p == c)
							PROCError(2049,C);
						}
					PROCCheck(':');
					sprintf(MyBuf,"%s_%x",T1S+1,c);
					PROCOutLab(MyBuf);
					}
				}
			else
				goto noStmt;
			break;
		case 'oc':
			if(!_tcscmp(TS,"continue")) {
				T1=InBlock;
				while(T1) {
					if(*OldTX[T1].C) {
	#if ARCHI
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
	#elif Z80
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
	#elif I8086
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
	#elif MC68000
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
#elif GD24032
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
	#elif MICROCHIP
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[T1].C,0);
	#endif
						T1=1;
						}
					T1--;
					if(T1==1)
						PROCError(2044);
					}
				}
			else
				goto noStmt;
			break;
		case 'ed':
			if(!_tcscmp(TS,"default")) {
				_tcscpy(T1S,OldTX[InBlock].T);
				if(*T1S != '&') {
					PROCError(2047);
					}
				else {
					char T2S[128];

					if(OldTX[InBlock].flag)
						PROCError(2048);

					OldTX[InBlock].flag=1;
					_tcscpy(MyBuf,T1S+1);
					_tcscpy(T2S,MyBuf);
					_tcscat(T2S,"_");
	//				_tcscat(OldTX[InBlock].TX->next->s,"_");



	//				_tcscat(OldTX[InBlock].T,"_");



	//      $((OLDTX%(InBlock%)!0)+8)+="_";
					PROCCheck(':');
					PROCOutLab(T2S);
	//			  swap(&LastOut,&OLDTX[InBlock]);
	//			  swap(&LastOut,&OLDTX[InBlock]);
	/*		  if(*FNLA(MyBuf)) {
				if(FNIsStmt()) {
					}
				else 
					PROCIsDecl();
				}
				*/
					}
				}
			else
				goto noStmt;
		  break;
		case 'od':
			if(!*(TS+2)) {
				FNGetLabel(TS,1);
				PROCOutLab(TS);
				*OldTX[InBlock+1].T='#';
				_tcscpy(OldTX[InBlock+1].T+1,TS);
				_tcscat(TS,"_");
				_tcscpy(MyBuf,TS);
				_tcscat(MyBuf,"1");
				PROCLoops(NULL,TS,MyBuf,LastOut);
				}
			else
				goto noStmt;
			break;
		case 'le':
			if(!_tcscmp(TS,"else")) {
				I=InBlock+1;
	//    myLog->print("leggo da %d\n",I);
				if(*OldTX[I].T != '%')
					PROCError(2062,"else");
				_tcscpy(TS,OldTX[I].T+1);
				*TS='E';
	//		_tcscat(TS,"_else");
				LastOut=OldTX[I].TX->prev;
	#if ARCHI
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
	#elif Z80
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
	#elif I8086
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
	#elif MC68000
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
#elif GD24032
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
	#elif MICROCHIP
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)TS,0);
	#endif
				t1=LastOut;
				LastOut=OldTX[I].TX;
				t2=LastOut;
				*MyBuf=' ';
				_tcscpy(MyBuf+1,TS);
				PROCLoops(MyBuf,"","",t2);
				}
			else
				goto noStmt;
		  break;
		case 'ne':  
			if(!_tcscmp(TS,"enum")) {
				uint32_t enum_cnt=0;

				PROCWarn(1003,"enum tag non gestiti ");

				FNLA(TS);
				if(*TS != '{') {		// qua c'è il TAG dell'enum.. per ora me ne frego
					FNLO(TS);
					FNCercaEnum(TS,NULL,TRUE);
	//				i=FNAllocEnum(MyBuf);

					}
				PROCCheck('{');
				t2=LastOut;
				for(;;) {
					int j;
					int8_t OP=0,Co=0;
					char AS[64];
					struct OPERAND V;
					struct VARS v;
					union STR_LONG VCost;
					ZeroMemory(&V,sizeof(struct OPERAND));
					ZeroMemory(&VCost,sizeof(union STR_LONG));
					ZeroMemory(&v,sizeof(struct VARS));
					V.cost=&VCost;
					V.var=&v;
					FNLO(AS);
					FNLA(MyBuf);		// buttare fuori in ASM queste linee?
					if(*MyBuf == '=') {
						PROCCheck('=');
						j=FNGetAritElem(&OP,MyBuf,&V,Co);
						if(j != ARITM_IS_COSTANTE)
							PROCError(2141);		// migliorare messaggio
						enum_cnt=V.cost->l;
						}
					else
						enum_cnt;
					if(FNCercaEnum(TS,AS,TRUE))
						PROCError(2011,AS);		// 
					FNAllocEnum(TS,AS,enum_cnt,FNGetSize(V.cost->l));
					if(OutSource) {
						wsprintf(MyBuf,"|%5u| : %s=%u",__line__,AS,enum_cnt);
	//					FNGetLine(OldTextp,MyBuf+10);		// complicato... lascio solo nomi
	//					MyBuf[_tcslen(MyBuf)-2]=0;		// tolgo CR se no diventa doppio
						PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
						}
					FNLA(MyBuf);
					if(*MyBuf == ',')
						PROCCheck(',');
					else if(*MyBuf == '}') {
						PROCCheck('}');
						break;
						}
					else
						PROCError(2054,"}");
					enum_cnt++;
					}
				PROCCheck(';');
				swap(&t2,&LastOut);		// in alcuni casi "enum" nel file ASM finisce al fondo... boh, e questo non aiuta
				Declaring=TRUE;
				}	
			else
				goto noStmt;
			break;
		case 'of':
			if(!_tcscmp(TS,"for")) {
				PROCCheck('(');
	//		*MyBuf=0;
				FNEvalExpr(16,MyBuf);              // 1° expr
				FNGetLabel(T1S,1);
				PROCOutLab(T1S,"_");
				PROCCheck(';');     //    FNLO(TS);
				l=FIn->GetPosition();
				I=1;
				do {
					FNLO(TS);
					switch(*TS) {
						case '(':
							I++;
							break;
						case ')':
							I--;
							break;
						case 0:
							PROCError(2059);
							break;
						}
					} while(I);
		//    t2=LastOut;
				l1=FIn->GetPosition();
				FIn->RestorePosition(l);
				__line__=ol;
				if(*FNLA(MyBuf) != ';') {
		//      _tcscpy(MyBuf,"!");
					_tcscpy(MyBuf,T1S);
					FNEvalCond(MyBuf,T1S,TRUE);
					}
				t2=LastOut;
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"endfor");
				PROCCheck(';');
	//		*MyBuf=0;
				FNEvalExpr(16,MyBuf);                 // 2° expr
	#if ARCHI
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif Z80
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif I8086
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif MC68000
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif MICROCHIP
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");
				PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#endif
				PROCOutLab(T1S);
				FIn->RestorePosition(l1);
				__line__=ol;
				_tcscpy(OldTX[InBlock+1].T,T1S);
	//			_tcscat(T1S,"_");  
				_tcscpy(MyBuf,T1S);
				_tcscat(MyBuf,"_");  
				PROCLoops(NULL,T1S,MyBuf,t2);
				}
			else
				goto noStmt;
			break;
		case 'og':
			if(!_tcscmp(TS,"goto")) {
				if(*FNLA(MyBuf) != ';') {
	  			FNLO(MyBuf);
	#if ARCHI
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif Z80
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif I8086
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif MC68000
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif MICROCHIP
	  			PROCOper(LINE_TYPE_JUMPGOTO,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#endif
		  		}
				else
					PROCError(2059,TS);
				}  
			else
				goto noStmt;
		  break;
		case 'fi':
			if(!*(TS+2)) {
				PROCCheck('(');
				FNGetLabel(TS,2);
			//    *MyBuf='!';
				_tcscpy(MyBuf,TS);
				FNEvalCond(MyBuf,TS,TRUE);
				PROCCheck(')');
				*MyBuf='%';
				_tcscpy(MyBuf+1,TS);
				PROCLoops(MyBuf,"","",LastOut);
				}
			else
				goto noStmt;
			break;
		case 'rp':		
			if(!_tcscmp(TS,"pragma")) {
				if(!*FNLO(MyBuf))
					PROCError(2059,TS);
				if(*FNLA(MyBuf1) == '(') {
					PROCCheck('(');
					if(*FNLA(MyBuf1) != ')')
						FNLO(MyBuf1);			// ev. finire...
					PROCCheck(')');
					}
				else
					FNLO(MyBuf1);			// ev. finire...
				PROCWarn(1003,MyBuf);
				if(!_tcscmp(MyBuf,"code_seg")) {
	//	      if(InBlock>0)                       // sembra di no... ma mi fa strano! (2010)
	//	        PROCError(2156);
					FNLO(MyBuf);
	#if ARCHI
	#elif Z80
					PROCOper(LINE_TYPE_ISTRUZIONE,"csect",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#elif I8086
					PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\tSEGMENT",0);
	#elif MC68000
					PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\tSEGMENT",0);
#elif GD24032
					PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"\tSEGMENT",0);
	#elif MICROCHIP
					PROCOper(LINE_TYPE_ISTRUZIONE,"CODE",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
	#endif
					}
				Declaring=OldDcl;
				}
			else
				goto noStmt;
			break;
//		case 'il':			// #line! ma arriva sopra col cancelletto, per forza
//			FNLO(MyBuf);
//			FNLO(MyBuf);
//			break;
		case 'er':		
			if(!_tcscmp(TS,"return")) {
				if(*FNLA(MyBuf) != ';') {
					if(!FNGetMemSize(CurrFunc,1)) 
	  				PROCError(4098);
					// sarebbe da dare anche il Warning se non-void senza return...
					l=CurrFunc->type & ~(VARTYPE_FUNC | VARTYPE_FUNC_BODY | VARTYPE_FUNC_USED);
					i=CurrFunc->size;
					FNEvalECast(MyBuf,(O_TYPE*)&l,(O_SIZE*)&i);
					}
				else {
					if(FNGetMemSize(CurrFunc,1)) 
	  				PROCWarn(4035);
					}
				PROCReturn();
				}
			else
				goto noStmt;
			break;
		case 'ws':
			if(!_tcscmp(TS,"switch")) {
				PROCCheck('(');
		//		*MyBuf=0;
				l=0;
				i=0;
				FNEvalECast(MyBuf,(O_TYPE*)&l,(O_SIZE*)&i);
				if(
	#if !defined(I8086) && !defined(MC68000)  && !defined(GD24032) && !defined(ARCHI)
					i>2 ||
	#endif
					(l & (VARTYPE_STRUCT | VARTYPE_UNION | VARTYPE_ARRAY | VARTYPE_IS_POINTER | VARTYPE_FUNC | VARTYPE_FLOAT /*0x1d0f*/)))
					PROCError(2050);
				PROCCheck(')');
				FNGetLabel(TS,1);
				t2=LastOut;
				I=InBlock+1;
				*OldTX[I].T='&';
				_tcscpy(OldTX[I].T+1,TS);
				OldTX[I].flag=0;
				OldTX[I].C[0]=0;            // salvo qui (tanto non si usa) SIZE
				OldTX[I].C[1]=i;
				p=OldTX[I].parm=(char *)GlobalAlloc(GPTR,1024);
				*((int *)p)=0;
				swap(&t2,&LastOut);
				PROCLoops(NULL,TS,"",t2);
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"endswitch");// SISTEMARE! esce DOPO la label finale
				}
			else
				goto noStmt;
			break;
		case 'yt':	
			if(!_tcscmp(TS,"typedef")) {
				l=FIn->GetPosition();
				FNLO(TS);
				T1=MaxTypes;
				PROCGetType(&Types[T1].type,&Types[T1].size,&Types[T1].tag,Types[T1].dim,&attrib,l);
				FNLO(MyBuf);
				if(FNIsType(MyBuf) != VARTYPE_NOTYPE)
					PROCError(2026,MyBuf);
				MaxTypes++;
				if(MaxTypes>=MAX_TIPI)
					PROCError(1002,"massimo numero di typedef");
				_tcscpy(Types[T1].s,MyBuf);
				while(*FNLO(MyBuf) != ';');
				Declaring=OldDcl;
				PROCOper(LINE_TYPE_NULLA,0,OPDEF_MODE_NULLA,0,0);
				}
			else
				goto noStmt;
			break;
		case 'hw':	
			if(!_tcscmp(TS,"while")) {
				PROCCheck('(');
				if(*OldTX[InBlock+1].T=='#') {         // se chiude do...
					I=InBlock+1;
					PROCOutLab(OldTX[I].C);
		//		  *MyBuf='!';
					_tcscpy(MyBuf,OldTX[I].B);
					i=FNEvalCond(MyBuf,OldTX[I].B,TRUE);
					PROCCheck(')');
	//			  if(i)
						PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)(OldTX[I].T+1),0);
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"enddo");
					PROCOutLab(OldTX[I].B);
					*OldTX[I].T=0;
					*OldTX[I].B=0;
					*OldTX[I].C=0;
					PROCCheck(';');
					}
				else {                                  // ...altrimenti
					FNGetLabel(TS,1);
					PROCOutLab(TS,"_");
			//      *MyBuf='!';
					_tcscpy(MyBuf,TS);
					FNEvalCond(MyBuf,TS,TRUE);
					PROCCheck(')');
					PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,"endwhile");
					t2=LastOut;
					_tcscpy(MyBuf,TS);
#if ARCHI
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif I8086
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MC68000
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif GD24032
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif MICROCHIP
					_tcscat(MyBuf,"_");
					PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#endif
					*MyBuf=' ';
					_tcscpy(MyBuf+1,TS);
					_tcscpy(MyBuf1,TS);
					_tcscat(MyBuf1,"_");
					PROCLoops(MyBuf,TS,MyBuf1,t2);
					}
				}
			else
				goto noStmt;
			break;
		default:
noStmt:		
			FNLA(MyBuf);
			if(*MyBuf==':') {
				if(!FNCercaGoto(TS)) {
					PROCAllocGoto(TS);
					PROCOutLab(TS,CurrFunc->label);
					}
				else
					PROCError(2045,TS);
				FNLO(TS);
				goto rifoStmt;       // la label da sola non fa stmt...
				}
/*			else if(*MyBuf=='{') {
				*OldTX[InBlock+1].T=0;
				PROCLoops(NULL,TS,MyBuf,LastOut);
				}*/
			else {
				FIn->RestorePosition(OldTextp);
				__line__=ol;
				Declaring=OldDcl;
				return FALSE;
				}
			break;  
		}
	if(!CurrFunc) {
//		PROCError(2062,TS);		// oppure provare a dichiarare... ma NON eseguire! anche se funzione
		}
  if(OutSource) {    
		swap(&t,&LastOut);
		wsprintf(MyBuf,"|%5d| : ",__line__);
		FNGetLine(OldTextp,MyBuf+10);
//		MyBuf[_tcslen(MyBuf)-2]=0;		// tolgo CR se no diventa doppio
		PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
		swap(&t,&LastOut);
		}
  return TRUE;
  }
 
char *Ccc::FNGetLabel(char *A,uint8_t m) {

  switch(m) {
    case 0:
      m='L';
      break;
    case 1:
      m='J';
      break;  
    case 2:
      m='L';
      break;  
    case 3:
#if MC68000 
//				PROCOut1(&StaticOut,Var[T1].label,NULL);
      m= TipoOut & TIPO_SPECIALE ? 'S' : '$';		// madonnaeasy68
#else
      m='$';
#endif
      break;  
    default:
      m='X';
      break;  
    }
  sprintf(A,"%c%05d",m,++LABEL);
  return A;
  }

 
int Ccc::PROCGenCondBranch(const char *Alabel, int T, int8_t *VQ, O_SIZE Size) {
  int i,B;
 
//  if(V & 0xf)
//    S=PTR_SIZE;
  if(*VQ & VALUE_IS_CONDITION) {		// qua se condizione con operatore ossia > < == ecc...
		B=FNGetCondString(*VQ & 0x2f,T);
		}
  else {				//...altrimenti è semplice 0 o !0 su variabile
    i=*VQ & 0xf;
#if ARCHI
/*		char b[64];			// sistemare, è pre-2010

		*b=0;

    _tcscpy(b,LastOut->s1.s.label);
		if((B==76) || (B==83)) {
		  PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
		  }
		else {
		  if(*(b+3) != 'S') {
				_tcsncpy(LastOut->s1.s.label,b,3);
				_tcscat(LastOut->s1.s.label,"S");
				_tcscat(LastOut->s1.s.label,b+3);
				}
		  }
	  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,B,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
		*/

		if(i==VALUE_IS_EXPR_FUNC || (*VQ & VALUE_IS_CONDITION_VALUE)) {
			switch(Size) {
				case 1:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO8,Regs->D,OPDEF_MODE_REGISTRO8,Regs->D);
					break;
				case 2:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D);
					break;
				case 4:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOVS",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
					break;
			  }  
		  }  

#elif Z80
    switch(Size) {
      case 1:
				if(i==VALUE_IS_EXPR || i==VALUE_IS_EXPR_FUNC || i & VALUE_IS_COSTANTE) {
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
					PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,3);		// era OPDEF_MODE_REGISTRO_HIGH8 2025
					}
		    break;
      case 2:
				if(i==VALUE_IS_EXPR || i==VALUE_IS_EXPR_FUNC || i & VALUE_IS_COSTANTE) {
//		      if(V == -14 || V==-15)      // non si capisce...

  		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
	  	    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  	      }
		    break;
      case 4:
				if(i==VALUE_IS_EXPR || i==VALUE_IS_EXPR_FUNC || i & VALUE_IS_COSTANTE) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
  	      }
		    break;
		  }  
#elif I8086
    switch(Size) {			// verificare se come 68000...
      case 1:
		    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,0,OPDEF_MODE_REGISTRO_LOW8,0);
		    break;
      case 2:
//		    if(!V || V==-1 || V==-2)       // NO su 16 bit
//		      if(V == -14 || V==-15) {      // non si capisce...
				    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_REGISTRO16,0);
//	  		    PROCOper(LINE_TYPE_ISTRUZIONE,"or",1,(union SUB_OP_DEF*)Regs->Accu,1,(union SUB_OP_DEF*)Regs->Accu);

//	  	      }
//	  	    else { 
//	  		    PROCOper(movString,Regs->Accu,Regs->DSh);
//		  	    PROCOper("or",Regs->Accu,NULL);
//	  	      }
		    break;
      case 4:     // non ancora auto-condiz
//	      if(V == -14 || V==-15) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_REGISTRO16,0);
				  PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_REGISTRO16,1);
//  	      }
//  	    else { 
//			    PROCOper(movString,Regs->Accu,Regs->D1Sh);
//			    PROCOper("or",Regs->Accu,NULL);
//  	      }
		    break;
		  }  
#elif MC68000
		/* qua non serve, MOVE tocca già i flag TRANNE CHE se funzione!
		*/
		if(i==VALUE_IS_EXPR_FUNC || (*VQ & VALUE_IS_CONDITION_VALUE)) {
			switch(Size) {
				case 1:
					PROCOper(LINE_TYPE_ISTRUZIONE,"tst.b",OPDEF_MODE_REGISTRO8,0);
					break;
				case 2:
					PROCOper(LINE_TYPE_ISTRUZIONE,"tst.w",OPDEF_MODE_REGISTRO16,0);
					break;
				case 4:
					PROCOper(LINE_TYPE_ISTRUZIONE,"tst.l",OPDEF_MODE_REGISTRO32,0);
					break;
				}
			}
		/*
    switch(S) {
      case 1:
		    PROCOper(LINE_TYPE_ISTRUZIONE,"move.b",OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		    break;
      case 2:
//		    if(!V || V==-1 || V==-2)       // NO su 16 bit
//		      if(V == -14 || V==-15) {      // non si capisce...
				    PROCOper(LINE_TYPE_ISTRUZIONE,"move.w",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_REGISTRO16,Regs->D);

//	  	      }
//	  	    else { 
//	  		    PROCOper(movString,Regs->Accu,Regs->DSh);
//		  	    PROCOper("or",Regs->Accu,NULL);
//	  	      }
		    break;
      case 4:     // non ancora auto-condiz
//	      if(V == -14 || V==-15) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->D);
//  	      }
//  	    else { 
//			    PROCOper(movString,Regs->Accu,Regs->D1Sh);
//			    PROCOper("or",Regs->Accu,NULL);
//  	      }
		    break;
		  }  */
#elif GD24032
		/* QUA????   usare.f ?  bisognerebbe "segnarselo" da istruzione precedente, oppure fare in ottimizzazione
		*/
		if(i==VALUE_IS_EXPR_FUNC || (*VQ & VALUE_IS_CONDITION_VALUE)) {
			switch(Size) {
				case 1:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.b.f",OPDEF_MODE_REGISTRO8,0,OPDEF_MODE_REGISTRO8,0);
					break;
				case 2:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.w.f",OPDEF_MODE_REGISTRO16,0,OPDEF_MODE_REGISTRO16,0);
					break;
				case 4:
					PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d.f",OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO32,0);
					break;
				}
			}
#elif MICROCHIP
    switch(Size) {
      case 1:
		    if(i==VALUE_IS_EXPR || i & VALUE_IS_COSTANTE)
		      PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
		    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,3);
		    break;
      case 2:
		    if(i==VALUE_IS_EXPR || i & VALUE_IS_COSTANTE) {
//		      if(V == -14 || V==-15)      // non si capisce...

  		    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
	  	    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
  	      }
		    break;
      case 4:
		    if(i==VALUE_IS_EXPR || i & VALUE_IS_COSTANTE) {
			    PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,Regs->D);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
			    PROCOper(LINE_TYPE_ISTRUZIONE,"IORLW",OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1);
  	      }
		    break;
		  }  
#endif
    if(*VQ & VALUE_IS_CONDITION_VALUE)
      T=!T;          
		if(T)
		  B=CONDIZ_UGUALE & 0xf;
		else
		  B=CONDIZ_DIVERSO & 0xf;
    *VQ=CONDIZ_DIVERSO & 0xf;                // converto una var in cond != 0
		}
#if ARCHI
  PROCOper(LINE_TYPE_JUMPC,jmpCondString /*B*/,OPDEF_MODE_CONDIZIONALE,B,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#elif Z80
  if(T)
    *VQ ^= 1;
  if(*VQ==CONDIZ_MAGGIORE /*0x83  (OPDEF_MODE_REGISTRO_INDIRETTO*/)
    PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"$+5",0);
  PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,B,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
  if(*VQ==CONDIZ_MINORE_UGUALE)
    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#elif I8086
  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,B,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#elif MC68000
  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,B /*| 0x80  verificare*/,
		OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#elif GD24032
  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,B /*| 0x80  verificare*/,
		OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#elif MICROCHIP
  if(T)
    *VQ ^= 1;
  if(*VQ==CONDIZ_MAGGIORE  /*0x83/*OPDEF_MODE_REGISTRO_INDIRETTO*/) {
		if(CPUPIC<2)
			PROCOper(LINE_TYPE_JUMPC,"GOTO",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"$+5",0);
		else
			PROCOper(LINE_TYPE_JUMPC,"GOTO/BRA",OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)"$+5*2",0);
		}
  PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,B,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
  if(*VQ==CONDIZ_MINORE_UGUALE)
    PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,4,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)Alabel,0);
#endif

  return 0;
  }
 
int Ccc::FNGetCondString(uint8_t A, uint8_t B) {   // (was: bit 5 di A vale 0 se signed, 1 unsigned
  
  if(B)
		A ^= 1;
//  if((A & 0x20 && A < 0x24))       // <, <=, >, >= possono essere signed o unsigned
//    A = (A & 0x1f) + 6;
	// NON SERVE PIU' 2025, v. condizioni _UNSIGNED e subCmp
//  if(A & 0x20 && (A < 0x24 || A>=0x26))       // questo serve cmq...
    A &= 0xf;

  return A;
  }
 
int Ccc::PROCAssignCond(int8_t *VQ, O_TYPE *T, O_SIZE *S, char *Clabel) {
  char MyBuf[32],MyBuf1[32];
  int i;
				 
#if ARCHI
  _tcscpy(MyBuf1,"MVN");
 // _tcscat(MyBuf1,FNGetCondString(*VQ & 0xf,FALSE));
  PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf1,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,0);
  _tcscpy(MyBuf1,"MOV");
//  _tcscat(MyBuf1,FNGetCondString(*VQ & 0xf,TRUE));
  PROCOper(LINE_TYPE_ISTRUZIONE,MyBuf1,OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_IMMEDIATO32,0);
#elif Z80
  i=*VQ & 0x3f;
//  i &= 0xbf;
  FNGetLabel(MyBuf1,2);
  PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,FNGetCondString(i,TRUE),OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf1,0);
	if((*VQ & VALUE_HAS_CONDITION) && *Clabel) {          // or logico ||
    PROCOutLab(Clabel);
	  }
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO16,1);
  PROCOper(LINE_TYPE_JUMP,jmpCondString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)FNGetLabel(MyBuf,1),0);
	if(!(*VQ & VALUE_HAS_CONDITION) && *Clabel) {           // and logico &&
    PROCOutLab(Clabel);
	  }
  PROCOutLab(MyBuf1);
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO16,0);
  PROCOutLab(MyBuf);
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_IMMEDIATO16,0);
  /*                  // variante usando i FLAG
  PROCOper(pushString,"af",NULL);
  PROCOper(popString,Regs->DS,NULL);
  PROCOper(movString,Regs->Accu,Regs->DSl);
  PROCOper("and",MyBuf,NULL);
  PROCOper("xor",MyBuf,NULL);
  PROCOper(movString,Regs->DSl,Regs->Accu);
  PROCOper(movString,Regs->DSh,Regs->Accu);
  */
#elif I8086
  i=*VQ+10;
	if(i<=-10)
    i+=10;
  FNGetLabel(MyBuf1,2);
  PROCOper(LINE_TYPE_JUMPC,"j",OPDEF_MODE_CONDIZIONALE,FNGetCondString(-i,TRUE),OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf1,0);
	if(*VQ<=-20 && *Clabel) {          // or logico ||
    PROCOutLab(Clabel);
	  }
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,1);
  PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)FNGetLabel(MyBuf,1),0);
	if(*VQ>-20 && *Clabel) {           // and logico &&
    PROCOutLab(Clabel);
	  }
  PROCOutLab(MyBuf1);
  PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO16,Regs->D,OPDEF_MODE_IMMEDIATO16,0);
  PROCOutLab(MyBuf);
#elif MC68000
  i=*VQ & (VALUE_IS_CONDITION | 0xf);
//  FNGetLabel(MyBuf1,2);
//  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,
//		FNGetCondString(i & VALUE_IS_CONDITION ? (i & 0xf) : (CONDIZ_UGUALE ^ (i & 1)),TRUE),
//		OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf1,0);
  PROCOper(LINE_TYPE_JUMPC /* per formato istruzione..*/,"s",OPDEF_MODE_CONDIZIONALE,
		FNGetCondString(i & VALUE_IS_CONDITION ? i : CONDIZ_UGUALE | ((i ^ 1) & 1),FALSE),
		OPDEF_MODE_REGISTRO32,Regs->D);
//	if((*VQ & VALUE_HAS_CONDITION) && *Clabel) {          // or logico ||
//    PROCOutLab(Clabel);
//	  }
//  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,1,OPDEF_MODE_REGISTRO32,Regs->D);
//  PROCOper(LINE_TYPE_JUMP,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)FNGetLabel(MyBuf,1),0);
//	if(!(*VQ & VALUE_HAS_CONDITION) && *Clabel) {           // and logico &&
//    PROCOutLab(Clabel);
//	  }
//  PROCOutLab(MyBuf1);
	if(*S>1) {
		PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
		if(*S>2)
			PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,Regs->D);
		}
//  PROCOutLab(MyBuf);
#elif GD24032
  i=*VQ & (VALUE_IS_CONDITION | 0xf);
//  FNGetLabel(MyBuf1,2);
//  PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,
//		FNGetCondString(i & VALUE_IS_CONDITION ? (i & 0xf) : (CONDIZ_UGUALE ^ (i & 1)),TRUE),
//		OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf1,0);
  PROCOper(LINE_TYPE_JUMPC /* per formato istruzione..*/,"s",OPDEF_MODE_CONDIZIONALE,
		FNGetCondString(i & VALUE_IS_CONDITION ? i : CONDIZ_UGUALE | ((i ^ 1) & 1),FALSE),
		OPDEF_MODE_REGISTRO32,Regs->D);
//	if((*VQ & VALUE_HAS_CONDITION) && *Clabel) {          // or logico ||
//    PROCOutLab(Clabel);
//	  }
//  PROCOper(LINE_TYPE_ISTRUZIONE,"moveq",OPDEF_MODE_IMMEDIATO32,1,OPDEF_MODE_REGISTRO32,Regs->D);
//  PROCOper(LINE_TYPE_JUMP,"bra.s",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)FNGetLabel(MyBuf,1),0);
//	if(!(*VQ & VALUE_HAS_CONDITION) && *Clabel) {           // and logico &&
//    PROCOutLab(Clabel);
//	  }
//  PROCOutLab(MyBuf1);
	if(*S>1) {
		PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,Regs->D);
		if(*S>2)
			PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,Regs->D);
		}
//  PROCOutLab(MyBuf);
#elif MICROCHIP
  i=*VQ & 0x3f;
//  i &= 0xbf;
  FNGetLabel(MyBuf1,2);
  PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,FNGetCondString(i,TRUE),OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf1,0);
	if((*VQ & VALUE_HAS_CONDITION) && *Clabel) {          // or logico ||
    PROCOutLab(Clabel);
	  }
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO16,1);
  PROCOper(LINE_TYPE_JUMP,"GOTO",OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)FNGetLabel(MyBuf,1),0);
	if(!(*VQ & VALUE_HAS_CONDITION) && *Clabel) {           // and logico &&
    PROCOutLab(Clabel);
	  }
  PROCOutLab(MyBuf1);
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D,OPDEF_MODE_IMMEDIATO16,0);
  PROCOutLab(MyBuf);
  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_IMMEDIATO16,0);
  /*                  // variante usando i FLAG
  PROCOper(pushString,"af",NULL);
  PROCOper(popString,Regs->DS,NULL);
  PROCOper(movString,Regs->Accu,Regs->DSl);
  PROCOper("and",MyBuf,NULL);
  PROCOper("xor",MyBuf,NULL);
  PROCOper(movString,Regs->DSl,Regs->Accu);
  PROCOper(movString,Regs->DSh,Regs->Accu);
  */
#endif
  *T=VARTYPE_PLAIN_INT;
  *S=INT_SIZE;
  *VQ=VALUE_IS_EXPR;
  return 0;
  }
 
int Ccc::PROCReturn() {

  if(!*OldTX[1].T) {
//		sprintf(OLDT[1],"_ret",Var[CurrFunc].name);     // era oltre 20 char...
		FNGetLabel(OldTX[1].T,1);
		*OldTX[1].T='R';
		}
  if(!AutoOff && !SaveFP && (Reg==Regs->MaxUser)) {
#if ARCHI
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,15,OPDEF_MODE_REGISTRO32,14);
#elif Z80 || I8086 || MC68000 || GD24032
		PROCOper(LINE_TYPE_ISTRUZIONE,returnString,OPDEF_MODE_NULLA,0);
#elif MICROCHIP
		PROCOper(LINE_TYPE_ISTRUZIONE,returnString,OPDEF_MODE_NULLA,0);
#endif
		}
  else {
#if ARCHI
		PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,NULL);
#elif Z80
		PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
#elif I8086
		PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
#elif MC68000
		if(MemoryModel & MEMORY_MODEL_RELATIVE)
			PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
		else
			PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
#elif GD24032
		if(MemoryModel & MEMORY_MODEL_RELATIVE)
			PROCOper(LINE_TYPE_JUMP,jmpShortString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
		else
			PROCOper(LINE_TYPE_JUMP,jmpShortString /*jmpString 512KB dovrebbero bastare!*/,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
#elif MICROCHIP
		PROCOper(LINE_TYPE_JUMP,jmpString,OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)OldTX[1].T,0);
#endif
		}      
	
  return 0;
  }
 
long Ccc::FNGetConst(char *s,bool m) {
  int16_t i;
  struct OPERAND V;
  char Clabel[32];
  long T;
 
  ZeroMemory(&V,sizeof(struct OPERAND));
	V.Q=-5;
	V.cost=(union STR_LONG *)s;
  ZeroMemory(s,sizeof(union STR_LONG));
	*Clabel=0;
  i=0;
  isRValue=isPtrUsed=0;
	Regs->Reset();
  FNRev(14,&i,Clabel,&V);		 //14, evitare virgole
  T=V.cost->l;
  
  if(debug)
		myLog->print(0,"Costante: %s\n",s);
	
  if(!m) {                          // accetto solo costanti
isError:
	  if(!(V.Q & VALUE_IS_COSTANTE))
			PROCError(2057);
	
isCost:
		if(V.Q==VALUE_IS_COSTANTE)	
		  ltoa(T,s,10);			// usare ulltoa
		else
			_tcscpy(s,V.cost->s);
    return T;
    }
  else {                            // accetto costanti e var statiche
    if(V.Q & VALUE_IS_COSTANTE)
      goto isCost;
    if(V.Q==VALUE_IS_VARIABILE) {
      if(V.var->classe<=CLASSE_STATIC)
        _tcscpy(s,V.var->label);
      else 
        goto isError;  
      }
    else   
      goto isError;  
    }
  return 0;  
  }
 
int Ccc::PROCLoops(const char *T, const char *T1, const char *T2, struct LINE *St) {
  int I,i;
  char MyBuf[128];
 
  I=InBlock+1;
  if(T && *T) {
		PROCOutLab(T+1);
		_tcscpy(OldTX[I].T,T);
//          myLog->print("scrivo in %d, %s\n",I,T);
		}
  OldTX[I].TX=LastOut;
  LastOut=St;        
  _tcscpy(OldTX[I].B,T1);
  _tcscpy(OldTX[I].C,T2);
  if(*FNLA(MyBuf) == '{') {
		Declaring=TRUE;
	  OldTX[I].id = __line__;		// o rand()?
		FNLO(MyBuf);
		PROCBlock();
		}
  else {   
    if(!*MyBuf)                  // per ignorare i commenti... (PROCBLOCK non ne ha bisogno)
      FNLO(MyBuf);
		InBlock=I;
//          myLog->print("inblock: %d\n",InBlock);
rifoIsStmt:
		if(FNIsStmt()) {
	  	if(*FNLA(MyBuf) == ';')
				PROCCheck(';');
	  	if(!_tcscmp(FNLA(MyBuf),"else")) {      // è l'unico caso in cui lego uno stmt al successivo
		  	goto rifoIsStmt;
		  	}
	  	}
		else {
		  if(OutSource) {
				wsprintf(MyBuf,"|%5d| : ",__line__);
				FNGetLine(FIn->GetPosition(),MyBuf+10);
//				MyBuf[_tcslen(MyBuf)-2]=0;		// tolgo CR se no diventa doppio
				PROCOper(LINE_TYPE_COMMENTO,0,OPDEF_MODE_NULLA,(union SUB_OP_DEF*)NULL,0,MyBuf);
				}
	  	FNEvalExpr(15,MyBuf);
	  	}
		LastOut=OldTX[I].TX;
		InBlock--;
		}
	
  return 0;
  }
 
int Ccc::FNRegFree() {

  if(Reg>Regs->UserBase) 
	  return --Reg;
  else 
	  return 0;
  }
 
struct VARS *Ccc::FNCercaVar(const char *N, bool M) {
// M% TRUE=RICERCA NEL BLOCCO, FALSE RICERCA GLOBALE
  register int Bl;
  struct VARS *F, *V;

//  PROCV();
  
  Bl=InBlock;
  F=CurrFunc;
  do {       
		if(!Bl) {
		  F=0;
		  }
		V=Var;
		while(V) {
//        myLog->print(0,"Cercavar\a: %s <> %s, livello %u",N,V->name,Bl);
		  if(!V->tag) {
				if(V->func==F) {
				  if(V->block==Bl && V->blockId==OldTX[Bl].id) {
						if(!_tcscmp(N,V->name)) { 
						  return V;
						  }
						}
				  }
				}
		  V=V->next;
		  }
	//    if(Bl)
		  Bl--;
		} while(!M && (Bl>=0));

  return NULL;
  }
 
struct VARS *Ccc::FNCercaVar(struct TAGS *tag,const char *N) {
  struct VARS *F, *V;

 	V=Var;
	while(V) {
		if(V->tag==tag) {
			if(!_tcscmp(N,V->name)) { 
				return V;
				}
			}
		V=V->next;
		}

  return NULL;
  }
 
struct VARS *Ccc::PROCAllocVar(const char *N, O_TYPE Type, enum VAR_CLASSES Class, uint8_t Modif, O_SIZE Size, 
															 struct TAGS *Tag, O_DIM Dim) {
  struct VARS *V;
  char MyBuf[64],MyBuf1[64];
  
  V=(struct VARS *)GlobalAlloc(GPTR,sizeof(struct VARS)); 
  if(!V) {
fineMem:
    PROCError(1001,"Fine memoria VARS");
    }
  if(Var) {
    LVars->next=V;
    LVars=V;
    }
  else {
    LVars=Var=V;
    }
  V->next=(struct VARS *)NULL;
 
  if(debug)  
    myLog->print(0,"Alloco: %s, size %d\n",N,Size);

  V->type=Type;
  if((UnsignedChar) && (Size==1) && (Type==VARTYPE_PLAIN_INT))
	  V->type |= VARTYPE_UNSIGNED;       // Manca signed
  V->hasTag=Tag;
  V->tag=NULL;                   // attenzione: vedi IsDecl
	if(Dim)
		memcpy(V->dim,Dim,sizeof(V->dim));
  _tcsncpy(V->name,N,MAX_NAME_LEN);
	V->name[MAX_NAME_LEN]=0;
  V->size=Size;
  V->block=InBlock;
  V->blockId=OldTX[InBlock].id;
  V->func=CurrFunc;
  V->classe=Class;
  V->modif=Modif;
	V->definition=NULL;
  if(Type & VARTYPE_FUNC) {			// anche Pointer
  	if(!(V->parm=(char *)GlobalAlloc(GPTR,256)))
  	  goto fineMem;
	  *((int *)V->parm)=-1;                // no lista parm
		if(Modif & FUNC_MODIF_INLINE) {
			if(!(Type & VARTYPE_FUNC_POINTER))
				V->definition=LastOut;		// ma poi viene cancellata... occorre copiare (v.)
			}
	  }
	else
		V->parm=0;
  switch(Class) {
		case CLASSE_EXTERN:
		case CLASSE_GLOBAL:
		  if(Type & VARTYPE_FUNC && !(Type & VARTYPE_FUNC_POINTER)) {
		    if(V->modif & FUNC_MODIF_C)       // "C" ha precedenza su "PASCAL" (v. chkstk...)
		      goto noPascal;
		    if(PascalCall || (V->modif & FUNC_MODIF_PASCAL)) {
					_tcsncpy(V->label,N,MAX_NAME_LEN);
					strupr(V->label);
					strupr(V->name);
		      }
		    else
		      goto noPascal;  
				}
		  else {
noPascal:		  
				*V->label='_';
				_tcsncpy(V->label+1,N,MAX_NAME_LEN);
				if(MemoryModel & MEMORY_MODEL_RELATIVE)
					;			// 
				}
		  V->block=0;
		  V->func=NULL;
		  break;
		case CLASSE_STATIC:
		  sprintf(MyBuf1,"%s_%d",FNGetLabel(MyBuf,3),InBlock);
			_tcscpy(V->label,MyBuf1);
			if(MemoryModel & MEMORY_MODEL_RELATIVE)
				;			// 
		  break;
		case CLASSE_AUTO:
		  MAKEPTROFS(V->label)=0;
		  break;
		case CLASSE_REGISTER:
		  MAKEPTRREG(V->label)=0;
		  break;
		}     
	
  return V;
  }

struct VARS *Ccc::PROCAllocGoto(const char *label) {
	struct VARS *g,*g2,*g3;

	g=(struct VARS *)GlobalAlloc(GPTR,sizeof(struct VARS)); 
	if(!g) {
		PROCError(1001,"Fine memoria GOTO");
		}
	if(CurrFuncGotos) {
		g2=g3=CurrFuncGotos;
		while(g2) {
			g3=g2;
			g2=g2->next;
			}
		g3->next=g;
		}
	else {
		CurrFuncGotos=g;
		}
	_tcsncpy(g->label,label,sizeof(g->label));		// uso questo e non Name...
	g->next=(struct VARS *)NULL;
	return g;
	}

struct VARS *Ccc::FNCercaGoto(const char *N) {
  register int Bl;
  struct VARS *V;
  
	V=CurrFuncGotos;
	while(V) {
		if(!_tcscmp(N,V->label)) { 
			return V;
			}
		V=V->next;
		}

  return NULL;
  }
 
struct ENUMS *Ccc::FNCercaEnum(const char *tag, const char *N, bool M) {
// M% TRUE=RICERCA NEL BLOCCO, FALSE RICERCA GLOBALE
  register int Bl;
  struct ENUMS *E;
	struct VARS *V;

// usare tag??
  Bl=InBlock;
  V=CurrFunc;
  do {       
		if(!Bl) {
		  V=NULL;
		  }
		E=Enums;
		while(E) {
			if(E->var.func==V) {
			  if(E->var.block==Bl) {
					if(!_tcscmp(N,E->name)) { 
					  return E;
						}
				  }
				}
		  E=E->next;
		  }
	//    if(Bl)
		  Bl--;
		} while(!M && (Bl>=0));

  return NULL;
  }
 
 
int Ccc::PROCCast(O_TYPE T1, O_SIZE S1, O_TYPE *T2, O_SIZE *S2, int8_t reg) {
// T2 e S2 si tramutano in T1 e S1
  char myBuf[64];
	int8_t reg2;
	O_SIZE s2;
  
	if(reg==-1)
		reg=Regs->D;
	reg2=reg+1;		// per Z80 e/o 8086, finire!

  S1=FNGetMemSize(T1,S1,0/*dim*/,1);
  s2=FNGetMemSize(*T2,*S2,0/*dim*/,1);
  if((T1 & VARTYPE_UNSIGNED) ^ (*T2 & VARTYPE_UNSIGNED))
		PROCWarn(4018);
  if(s2 != S1 /*|| (T1 & VARTYPE_POINTER != T2 & VARTYPE_POINTER) sottinteso quindi */) {
		switch(S1) {
		  case 1:
				switch(s2) {
				  case 2:
#if ARCHI
					  PROCOper(LINE_TYPE_ISTRUZIONE,"AND",OPDEF_MODE_REGISTRO8,reg,OPDEF_MODE_REGISTRO8,reg,
							OPDEF_MODE_IMMEDIATO16,0x000000ff);
#elif Z80
//					  PROCOper(movString,Regs->Accu,Regs->DSl);
#elif I8086
//					  PROCOper("xor",Regs->DSh,Regs->DSh);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,reg);
#elif GD24032
						PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,reg,OPDEF_MODE_IMMEDIATO16,0x00ff);
#elif MICROCHIP
//					  PROCOper(movString,Regs->Accu,Regs->DSl);
#endif
				    break;
				  case 4:
#if ARCHI
//					  PROCOper(LINE_TYPE_ISTRUZIONE,"AND",Regs->D,",",Regs->DS,",#255");
					  PROCOper(LINE_TYPE_ISTRUZIONE,"AND",OPDEF_MODE_REGISTRO8,reg,OPDEF_MODE_REGISTRO8,reg,
							OPDEF_MODE_IMMEDIATO32,0x000000ff);
#elif Z80
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
//					  PROCOper(movString,Regs->DSh,0);
//					  PROCOper(movString,Regs->D1Sl,Regs->DSh);
//					  PROCOper(movString,Regs->D1Sh,Regs->DSh);
#elif I8086
//					  PROCOper("xor",Regs->DSh,0);
//					  PROCOper("xor",Regs->D1S,Regs->D1S);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x00ff,OPDEF_MODE_REGISTRO32,reg);
#elif GD24032
						PROCOper(LINE_TYPE_ISTRUZIONE,"AND.d",OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_IMMEDIATO32,0x000000ff);
#elif MICROCHIP
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
#endif
		  			break;
				  default:
		  			break;
				  }
				break;
		  case 2:
				switch(s2) {
				  case 1:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,reg,OPDEF_MODE_REGISTRO16,reg,
							OPDEF_MODE_SHIFT,+8);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO16,reg,OPDEF_MODE_REGISTRO16,reg,
						  (T2 & VARTYPE_UNSIGNED) ? OPDEF_MODE_SHIFTU : OPDEF_MODE_SHIFT,-8);
#elif Z80
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sla",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg,OPDEF_MODE_IMMEDIATO16,0);
/*						PROCOper(movString,Regs->DSh,0);
						if(!(T1 & VARTYPE_UNSIGNED)) {
							PROCOper("bit","7",Regs->DSl);
							PROCOper(jmpCondString,"z",FNGetLabel(MyBuf,2));
							PROCOper(decString,Regs->DSh,NULL);
							PROCOutLab(MyBuf);
							}
							*/
#elif I8086
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA,NULL);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO8,reg /*Regs->DSh*/,
								OPDEF_MODE_REGISTRO8,reg /*Regs->DSh*/,0);
#elif MC68000
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER))
							PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,reg);
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"andi.w",OPDEF_MODE_IMMEDIATO16,0x00ff,OPDEF_MODE_REGISTRO16,reg);
#elif GD24032
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER))
							PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,reg);
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"AND.w",OPDEF_MODE_REGISTRO16,reg,OPDEF_MODE_IMMEDIATO16,0x00ff);
#elif MICROCHIP
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"RLF",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg,OPDEF_MODE_IMMEDIATO16,0);
/*						PROCOper(movString,Regs->DSh,0);
						if(!(T1 & VARTYPE_UNSIGNED)) {
							PROCOper("bit","7",Regs->DSl);
							PROCOper(jmpCondString,"z",FNGetLabel(MyBuf,2));
							PROCOper(decString,Regs->DSh,NULL);
							PROCOutLab(MyBuf);
							}
							*/
#endif
						break;
				  case 4:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
							OPDEF_MODE_SHIFT,+16);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
						  (T2 & VARTYPE_UNSIGNED) ? OPDEF_MODE_SHIFTU : OPDEF_MODE_SHIFT,-16);
#elif Z80
//						PROCOper(movString,Regs->D1S,0);
#elif I8086
//						PROCOper("xor",Regs->D1S,Regs->D1S);
#elif MC68000
						PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x0000ffff,OPDEF_MODE_REGISTRO32,reg);
#elif GD24032
						PROCOper(LINE_TYPE_ISTRUZIONE,"AND.d",OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_IMMEDIATO32,0x0000ffff);
#elif MICROCHIP
//						PROCOper(movString,Regs->D1S,0);
#endif
						break;
				  default:
						break;
				  }
				break;
		  case 4:
				switch(s2) {
				  case 1:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
							OPDEF_MODE_SHIFT,+24);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
						  (T2 & VARTYPE_UNSIGNED) ? OPDEF_MODE_SHIFTU : OPDEF_MODE_SHIFT,-24);
//						PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,Regs->D,",ASL #24");
//						sprintf(MyBuf,"%s 24",TS);
//						PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,Regs->D,MyBuf);
#elif Z80
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
  					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sla",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg2,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg,OPDEF_MODE_IMMEDIATO16,0);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,Regs->D);
#elif I8086
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"cbw",OPDEF_MODE_NULLA,NULL);
							PROCOper(LINE_TYPE_ISTRUZIONE,"cwd",OPDEF_MODE_NULLA,NULL);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO8,reg /*Regs->DSh*/,OPDEF_MODE_REGISTRO8,reg /*Regs->DSh*/,0);
					  PROCOper(LINE_TYPE_ISTRUZIONE,"cwd",OPDEF_MODE_NULLA,NULL);
#elif MC68000
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"ext.w",OPDEF_MODE_REGISTRO16,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO32,reg);
							}
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x000000ff,OPDEF_MODE_REGISTRO32,reg);
#elif GD24032
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"SE.w",OPDEF_MODE_REGISTRO16,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"SE.d",OPDEF_MODE_REGISTRO32,reg);
							}
						else
							PROCOper(LINE_TYPE_ISTRUZIONE,"AND.d",OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_IMMEDIATO32,0x000000ff);
#elif MICROCHIP
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
  					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_LOW8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"RLF",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,reg,OPDEF_MODE_IMMEDIATO16,0);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,reg);
					  PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,reg);
#endif
						break;
				  case 2:
#if ARCHI
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
							OPDEF_MODE_SHIFT,+16);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_REGISTRO32,reg,
						  (T2 & VARTYPE_UNSIGNED) ? OPDEF_MODE_SHIFTU : OPDEF_MODE_SHIFT,-16);
//						PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,Regs->D,",ASL #16");
//						sprintf(MyBuf,"%s#16",TS);
//						PROCOper(LINE_TYPE_ISTRUZIONE,movString,Regs->D,Regs->D,MyBuf);
#elif Z80
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sla",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"sbc",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_IMMEDIATO16,reg);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
#elif I8086
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"cwd",OPDEF_MODE_NULLA,NULL);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"xor",OPDEF_MODE_REGISTRO16,reg,OPDEF_MODE_REGISTRO16,reg);
#elif MC68000
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"ext.l",OPDEF_MODE_REGISTRO16,reg);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"andi.l",OPDEF_MODE_IMMEDIATO32,0x0000ffff,OPDEF_MODE_REGISTRO32,reg);
#elif GD24032
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,"SE.d",OPDEF_MODE_REGISTRO16,reg);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,"AND.d",OPDEF_MODE_REGISTRO32,reg,OPDEF_MODE_IMMEDIATO32,0x0000ffff);
#elif MICROCHIP
						if(!(T1 & VARTYPE_UNSIGNED) && !(T1 & VARTYPE_IS_POINTER)) {
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,reg);
							PROCOper(LINE_TYPE_ISTRUZIONE,"RLF",OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,"SUBLW",OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,3);
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_REGISTRO_HIGH8,3);
							}
						else	
							PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1,OPDEF_MODE_IMMEDIATO16,reg);
						PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,Regs->D+1,OPDEF_MODE_REGISTRO_LOW8,Regs->D+1);
#endif
						break;
				  default:
						break;
				  }
				break;
		  default:
				break;
		  }
		}      
	
	*T2=T1;
	*S2=S1;
  return 1; 
  }



struct CONS *Ccc::FNAllocCost(const char *A, uint8_t mode, O_TYPE Type) {
  int i;
  char MyBuf[256],MyBuf2[256];
  struct CONS *C;
  
  if(!MultipleString) {
    C=Con;
	  while(C) {
			if(!_tcscmp(C->name,A))
			  return C;
			C=C->next;
			}
	  }

  C=(struct CONS *)GlobalAlloc(GPTR,sizeof(struct CONS)); 
  if(!C) {
    PROCError(1001,"Fine memoria CONS");
    }
 
  if(Con) {
    LCons->next=C;
    LCons=C;
    }
  else {
    LCons=Con=C;
    }
  C->next=(struct CONS *)NULL;

	if(MemoryModel & MEMORY_MODEL_RELATIVE) 
 		;						// offset su BaseAbs se RELATIVE (v. output

  _tcsncpy(C->name,A,MAX_NAME_LEN);
	C->name[MAX_NAME_LEN]=0;
  _tcscpy(C->label,FNGetLabel(MyBuf,0));
  switch(mode) {
		case 1:
#if ARCHI
		  PROCOut1(FO1,".",MyBuf," EQUS ",A);
			PROCOut1(FO1,"ALIGN",NULL,NULL);
#elif Z80
		  PROCOut1(FO3,MyBuf," db ",A);
#elif I8086
		  PROCOut1(FO3,MyBuf," DB ",A);
#elif MC68000
			if(!(TipoOut & TIPO_SPECIALE))
			  PROCOut1(FO3,MyBuf," DB ",A);
			else {
				i=0;
				char *p=(char*)A,*p1=MyBuf2,MyBuf3[16];
				while(*p) {
					switch(*p) {
						case '\\':		// segue xhh  sequenza esc 2 char, creata da FNGetEscape ecc
							*p1++='\'';
							*p1++=',';
							itoa(xtoi(p+2),MyBuf3,10);
							_tcscpy(p1,MyBuf3);
							p1+=_tcslen(MyBuf3);
							*p1++=',';			// escono vuoti se consecutivi, ma amen! chissene
							*p1++='\'';
							p+=3;
							break;
						default:
							*p1++=*p;
							break;
						}
					p++;
					}
				*p1=0;
			  PROCOut1(FO3,MyBuf," dc.b ",MyBuf2);
				// bisogna tirar fuori le sequenze ESC e \n... Easy68 non lo fa
//			  PROCOut1(FO3,MyBuf," dc.b ",A);
				}
#elif GD24032
		  PROCOut1(FO3,MyBuf," DB ",A);
#elif MICROCHIP
			if(Type & VARTYPE_ROM) {
				if(CPUPIC<2) {
					PROCOut1(FO3,MyBuf," DT ",A);		// table per PCL
					}
				else {
					PROCOut1(FO3,MyBuf," DA ",A);		// char per lettura diretta
					}
				}
			else {
				itoa(sizeof(A)+1,MyBuf2,10);
				PROCOut1(FO1,MyBuf," RES ",MyBuf2);
				if(CPUPIC<2) {
					PROCOut1(FO3,MyBuf,"_ROM DT ",A);
					}
				else {
					PROCOut1(FO3,MyBuf,"_ROM DA ",A);
					}
				}
#endif
			break;
	  case 2:
#if ARCHI
			PROCOut1(FO1,".",MyBuf," EQUW ",A);
			PROCOut1(FO1,"ALIGN",NULL,NULL);
#elif Z80
#elif I8086
			PROCOut1(FO3,MyBuf," DW ",A);
#elif MC68000
			if(!(TipoOut & TIPO_SPECIALE))
				PROCOut1(FO3,MyBuf," DW ",A);
			else
				PROCOut1(FO3,MyBuf," dc.w ",A);
#elif GD24032
				PROCOut1(FO3,MyBuf," DW ",A);
#elif MICROCHIP
			// FINIRE!!
			if(Type & VARTYPE_ROM) {
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf," DT ",A);		// table per PCL
					}
				else {
					PROCOut1(FO1,MyBuf," DA ",A);		// char per lettura diretta
					}
				}
			else {
				itoa(sizeof(A)*2,MyBuf2,10);
				PROCOut1(FO3,MyBuf," RES ",MyBuf2);
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf,"_ROM DT ",A);
					}
				else {
					PROCOut1(FO1,MyBuf,"_ROM DA ",A);
					}
				}
#endif
			break;
	  case 3:
#if ARCHI
			PROCOut1(FO1,".",MyBuf," EQUD ",A);
#elif Z80
			PROCOut1(FO3,MyBuf," dd ",A);
#elif I8086
			PROCOut1(FO3,MyBuf," DD ",A);
#elif MC68000
			if(!(TipoOut & TIPO_SPECIALE))
				PROCOut1(FO3,MyBuf," DD ",A);
			else
				PROCOut1(FO3,MyBuf," dc.l ",A);
#elif GD24032
				PROCOut1(FO3,MyBuf," DD ",A);
#elif MICROCHIP
			// FINIRE!!
			if(Type & VARTYPE_ROM) {
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf," DT ",A);		// table per PCL
					}
				else {
					PROCOut1(FO1,MyBuf," DA ",A);		// char per lettura diretta
					}
				}
			else {
				itoa(sizeof(A)*4,MyBuf2,10);
				PROCOut1(FO3,MyBuf," RES ",MyBuf2);
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf,"_ROM DT ",A);
					}
				else {
					PROCOut1(FO1,MyBuf,"_ROM DA ",A);
					}
				}
#endif
			break;
	  case 4:		// 8 byte, come per double
#if ARCHI
			PROCOut1(FO1,".",MyBuf," EQUQ ",A);
#elif Z80
			PROCOut1(FO3,MyBuf," dq ",A);
#elif I8086
			PROCOut1(FO3,MyBuf," DQ ",A);
#elif MC68000
			if(!(TipoOut & TIPO_SPECIALE))
				PROCOut1(FO3,MyBuf," DQ 0x",A);
			else {
				char A1[16],A2[16];
//				PROCOut1(FO3,MyBuf," ds.b 8 $",A);		// no così non escono... alloca solo 8 byte ff
				_tcsncpy(A1,A,8);	A1[8]=0;
				_tcsncpy(A2,A+8,8);	A2[8]=0;
				wsprintf(MyBuf2,"$%s,$%s%s",A1,A2,A+16);
				PROCOut1(FO3,MyBuf," dc.l ",MyBuf2);
				}
#elif GD24032
			PROCOut1(FO3,MyBuf," DQ 0x",A);
#elif MICROCHIP
			// FINIRE!!
			if(Type & VARTYPE_ROM) {
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf," DT ",A);		// table per PCL
					}
				else {
					PROCOut1(FO1,MyBuf," DA ",A);		// char per lettura diretta
					}
				}
			else {
				itoa(sizeof(A)*4,MyBuf2,10);
				PROCOut1(FO3,MyBuf," RES ",MyBuf2);
				if(CPUPIC<2) {
					PROCOut1(FO1,MyBuf,"_ROM DT ",A);
					}
				else {
					PROCOut1(FO1,MyBuf,"_ROM DA ",A);
					}
				}
#endif
			break;
		}
  return C;
  }

struct ENUMS *Ccc::FNAllocEnum(const char *tag, const char *A, uint32_t value, O_SIZE Size) {
  int i;
  char MyBuf[256],MyBuf2[256];
  struct ENUMS *C;
  
  C=(struct ENUMS *)GlobalAlloc(GPTR,sizeof(struct ENUMS)); 
  if(!C) {
    PROCError(1001,"Fine memoria ENUMS");
    }

	//FINIRE con tag!
 
  if(Enums) {
    LEnums->next=C;
    LEnums=C;
    }
  else {
    LEnums=Enums=C;
    }
  C->next=(struct ENUMS *)NULL;
 
  _tcsncpy(C->name,A,MAX_NAME_LEN);
	C->name[MAX_NAME_LEN]=0;
	C->var.value=value;
  return C;
  }


int Ccc::PROCInit() {
  int i,t;
	char myBuf[128];
						   
	myOutput->PostMessage(WM_CLSWINDOW,0,(LPARAM)myBuf);

  _strdate(__date__);
  t=__date__[3];
  __date__[3]=__date__[0];
  __date__[0]=t;
  t=__date__[4];
  __date__[4]=__date__[1];
  __date__[1]=t;
  _strtime(__time__);
  if(!NoMacro) {                      
#if ARCHI
		m_CPre->PROCDefine("ARCHIMEDES","1");
		movString="MOV";
		loadString="LDR";
		storString="STR";
		jmpString="B";
		jmpShortString="B";
		jmpCondString="B";
		callString="BL";
		returnString="MOV PC,R14";
		incString="INC";
		decString="DEC";
		pushString="STMDB";
		popString="LDMIA";
#elif Z80
		m_CPre->PROCDefine("Z80","1");
		movString="ld";
		storString="ld";
		jmpString="jp";
		jmpShortString="jr";
		jmpCondString="jr";
		callString="call";
		returnString="ret";
		incString="inc";
		decString="dec";
		pushString="push";
		popString="pop";
#elif I8086
		m_CPre->PROCDefine("I8086","1");
		movString="mov";
		storString="mov";
		jmpString="jmp";
		jmpShortString="jmp";
		jmpCondString="j";
		callString="call";
		returnString="ret";
		incString="inc";
		decString="dec";
		pushString="push";
		popString="pop";
#elif MC68000
		m_CPre->PROCDefine("MC68000","1");
		movString="move";
		storString="move";
		jmpCondString="b";
		if((MemoryModel & 0xf) >= MEMORY_MODEL_LARGE) {		// (finire altri casi ev.
			callString="jsr";
			jmpString="jmp";
			jmpShortString="bra";
			}
		else {
			callString="bsr";
			jmpString="bra";
			if((MemoryModel & 0xf) <= MEMORY_MODEL_SMALL) 		// 
				jmpShortString="bra.s";
			else
				jmpShortString="bra";
			}
		returnString="rts";
		incString="addq";
		decString="subq";
//		pushString="move.l ,(a7)-";
//		popString="move.l +(a7),";
		pushString="move";
		popString="move";
#elif GD24032
		m_CPre->PROCDefine("GD24032","1");
		movString="MOV";
		storString="MOV";
		jmpCondString="B";
		callString="CALL";
		jmpString="JMP";
		jmpShortString="JR";
		returnString="RET";
		incString="INC";
		decString="DEC";
		pushString="PUSH";
		popString="POP";
#elif I8051
		m_CPre->PROCDefine("I8051","1");
		movString="mov";
		storString="mov";
		jmpString="jmp";
		jmpShortString="jmp";
		jmpCondString=jmpString;
		callString="call";
		returnString="ret";
		incString="inc";
		decString="dec";
		pushString="push";
		popString="pop";
#elif MICROCHIP
		m_CPre->PROCDefine("MICROCHIP","1");
		movString="MOVF";
		storString="MOVWF";
		jmpString="GOTO";
		jmpShortString="BRA";
		jmpCondString="B";
		callString="CALL";
		returnString="RETURN";
		incString="INCF";
		decString="DECF";
		pushString="PUSH";
		popString="POP";
#endif
		}
	if(CheckStack)
	  PROCAllocVar("_chkstk",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
	if(CheckPointers)
	  PROCAllocVar("_chkptr",VARTYPE_FUNC | VARTYPE_FUNC_USED,CLASSE_EXTERN,4,0,0,0);	
#if ARCHI
  wsprintf(myBuf,"The G.Dar C.Compiler for the Archimedes on PC, (C) 1989-2026 - Version %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif Z80
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per lo Z80 su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif I8086
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per l'8086 su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif MC68000
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per il 68000 su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif GD24032
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per il GD24032 su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif I8051
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per l'8051 su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#elif MICROCHIP
  wsprintf(myBuf,"Compilatore \"C\" di G.Dar per la serie PIC su PC, (C) 1989-2026 - Versione %d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
#endif
	if(myOutput) {
		char *p=(LPSTR)GlobalAlloc(GPTR,256);
		_tcscpy(p,myBuf);
		myOutput->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		}
	
	for(i=0; i<20; i++)			// fa schifo, sistemare!
		memset(&OldTX[i],0,sizeof(BLOCK_PTR));
  return 0;                        
  }

void Ccc::CHECKPOINTER() {
	struct VARS *v;

	if(CheckPointers) {
#if ARCHI
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,0,OPDEF_MODE_REGISTRO,Regs->P);
  	v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif Z80
		PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO_HIGH8,3,OPDEF_MODE_REGISTRO_HIGH8,Regs->P);
		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO_LOW8,Regs->P);
  	v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif I8086
		if(CPU86<3)
			;
		PROCOper(LINE_TYPE_ISTRUZIONE,"or",OPDEF_MODE_REGISTRO16,Regs->P,OPDEF_MODE_REGISTRO16,Regs->P,0);
//	  PROCOper(LINE_TYPE_JUMP,"jnz","$+3",NULL);
  	v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif MC68000
		PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->P);
  	v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif GD24032
		PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,Regs->D,OPDEF_MODE_REGISTRO32,Regs->P);
  	v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpCondString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#elif MICROCHIP
		if(v->type & VARTYPE_ROM) {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_REGISTRO,8);
			PROCOper(LINE_TYPE_ISTRUZIONE,"IORWF",OPDEF_MODE_REGISTRO_HIGH8,8);
			}
		else {
			PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO,0,OPDEF_MODE_VARIABILE,"TBLPTRL");
			PROCOper(LINE_TYPE_ISTRUZIONE,"IORWF",OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_VARIABILE,"TBLPTRH");
			if((MemoryModel & 0xf) >= MEMORY_MODEL_MEDIUM)
				PROCOper(LINE_TYPE_ISTRUZIONE,"IORWF",OPDEF_MODE_REGISTRO_HIGH8,0,OPDEF_MODE_VARIABILE,"TBLPTRU");
			}
		v=FNCercaVar("_chkptr",0);
		PROCOper(LINE_TYPE_JUMPC,jmpString,OPDEF_MODE_CONDIZIONALE,CONDIZ_UGUALE,OPDEF_MODE_VARIABILE,(union SUB_OP_DEF*)&v->label,0);
#endif
		}
	}

// class REGISTRI
REGISTRI::REGISTRI(Ccc *c) {
	int i;

	myCC=c;
#if GD24032
	for(i=0; i<32; i++)
#else
	for(i=0; i<16; i++)
#endif
#if ARCHI || GD24032
		*DT[i]=0;
#else
		DT[i]=NULL;
#endif
#if ARCHI    
	for(i=0; i<16; i++)
		sprintf(DT[i],"R%u",i);
	sprintf(DT[15],"PC",i);
	FpS="R12";
	SpS="R13";
	Accu="";			// sembra non piu' usato... sistemare!
	D=0; P=8;
	MaxD=3;
	UserBase=4;
	MaxUser=12;
#elif Z80     
	DT[0]="hl";
	DT[1]="de";
	DT[2]="bc";
	DT[3]="af";
	DT[4]="ix";
	DT[5]="iy";              // lo uso così solo se non uso FP
	DT[6]="r6";		//?? bug?
	DT[7]="r7";		//?? bug?
	FpS="iy";
	SpS="sp";
	DT[8]="ix";
	DT[9]="iy";              // lo uso così solo se non uso FP
	DT[10]="r10";		//?? bug?
	DT[11]="r11";		//?? bug?
	DT[12]="r12";		//?? bug?
	DT[13]="r13";		//?? bug?
	DT[14]="r14";		//?? bug?
	DT[15]="r15";		//?? bug?
  Accu="a";
	D=0; P=0;
	MaxD=3;
	UserBase=4;
	MaxUser=5;
#elif I8086
	DT[0]="ax";
	DT[1]="dx";		// meglio per restituire long in funzioni!
	DT[2]="bx";
	DT[3]="cx";
	DT[4]="di";
	DT[5]="si";
	DT[6]="r6";		//?? bug?
	DT[7]="r7";		//?? bug?
	FpS="bp";
	SpS="sp";
	AbsS="bp";
// no...	DT[8]="bp";		// base pointer
	DT[8]="di";
	DT[9]="si";
	DT[10]="r10";		//?? bug?
	DT[11]="r11";		//?? bug?
	DT[12]="r12";		//?? bug?
	DT[13]="r13";		//?? bug?
	DT[14]="r14";		//?? bug?
	DT[15]="r15";		//?? bug?
  Accu="ax";
	D=0; P=8;
	MaxD=3;
	UserBase=4;
	MaxUser=5;
#elif MC68000
	DT[0]="d0";
	DT[1]="d1";
	DT[2]="d2";
	DT[3]="d3";
	DT[4]="d4";
	DT[5]="d5";
	DT[6]="d6";
	DT[7]="d7";
	FpS="a6";
	SpS="a7";
	AbsS="a5";
	DT[8]="a0";
	DT[9]="a1";
	DT[10]="a2";
	DT[11]="a3";
	DT[12]="a4";
	DT[13]="a5";		// ev. base address per codice rilocabile, quindi tutte le Global vanno basate su a5 - e usare flag tipo MemoryModel
	DT[14]="a6";		// base pointer
	DT[15]="a7";		// o SP
  Accu="d0";
	D=0; P=8;
	MaxD=7;
	UserBase=4;
	MaxUser=8;
#elif GD24032
	for(i=0; i<32; i++)
		sprintf(DT[i],"R%u",i);
	FpS="R24";
	SpS="SP";
	AbsS="R25";
	//DT[21]="R25";		// ev. base address per codice rilocabile, quindi tutte le Global vanno basate su R21 - e usare flag 
	//DT[22]="R24";		// base pointer
	//DT[23]="R23";		// o SP
  Accu="R0";
	D=0; P=16;
	MaxD=15;
	UserBase=4;
	MaxUser=16;
#elif I8051
	DT[0]="ax";
	DT[1]="dx";
	DT[2]="bx";
	DT[3]="cx";
	FpS="bp";
	SpS="sp";
	// e i segmenti? come fare?
	DT[8]="di";
	DT[9]="si";
	DT[10]="bp";
  Accu="ax";
	D=0; P=8;
	MaxD=3;
	UserBase=8;
	MaxUser=9;
#elif MICROCHIP
	DT[0]="W";
	DT[1]="PRODL";
	DT[2]="PRODH";
	DT[3]="W2";
	if(StackLarge) {
		SpS="FSR1";
		FpS="FSR2";			// come in C18
		}
	else {
		SpS="FSR1L";
		FpS="FSR2L";		// come in C18
		}
	DT[8]="FSR0";
	DT[9]="INDF0";
	DT[10]="POSTDEC1";
	DT[11]="PREINC1";
	DT[12]="POSTDEC2";
	DT[13]="PREINC2";
	DT[14]="PLUSW2";
  Accu="W";
	D=0; P=8;
	MaxD=1;
	UserBase=8;
	MaxUser=9;
#endif
	ToDec=0;
	ToGet=0;
	ZeroMemory(VType,sizeof(VType));
	ZeroMemory(VSize,sizeof(VSize));
	ZeroMemory(VVar,sizeof(VVar));
	};

int8_t REGISTRI::Inc(int8_t S) {

	if(S>INT_SIZE)
		S=2;
	else
		S=1;
	if((D+S   /*+S 2025*/)>MaxD) {
//	    D=MaxD;
		myCC->PROCWarn(1035);
//      Assegna(D);
		ToDec=0;
		return 1;
		}  
	else {
		D+=S;

#if MC68000			// provare altri,,,
//boh		P+=S;
#endif

		ToDec=S;
//      Assegna(D); 

		myCC->maxRegUsed=D;

		return 0;
		}
	};

void REGISTRI::Dec(int8_t S) {

	if(S>INT_SIZE)
		S=2;
	else
		S=1;
//    D-=ToDec;
	D-=S;
#if MC68000			// provare altri,,,
//boh	P-=S;
#endif
//    Assegna(D); 
	if(D<0) {
//	    D=0;
		myCC->PROCError(1001,"Regs Dec");
		}

	}         

int8_t REGISTRI::IncP() {

	P++;
#if GD24032			// finire altri,,,
	if(P> 23) {
#else
	if(P> 15 /*METTERE*/) {
#endif
//	    P=MaxP;
		myCC->PROCWarn(1035);
		return 1;
		}  
	return 0;
	}

void REGISTRI::DecP() {

	P--;
#if defined(MC68000) || defined(I8086) 
	if(P<8) {
//	    P=0;

//		myCC->PROCWarn /*Error*/(1001,"Regs DecP TOGLIERE**");

		P=8;		// una specie di patch... v. isPtrUSed: dovrebbe venire pulito a fine riga, se dopo '='
		}
#elif defined(GD24032)
	if(P<16) {
//	    P=0;

//		myCC->PROCWarn /*Error*/(1001,"Regs DecP TOGLIERE**");

		P=16;		// una specie di patch... v. isPtrUSed: dovrebbe venire pulito a fine riga, se dopo '='
		}
#else
	if(P<0) {
//	    P=0;

//		myCC->PROCWarn /*Error*/(1001,"Regs DecP TOGLIERE**");

		P=0;		// una specie di patch... v. isPtrUSed: dovrebbe venire pulito a fine riga, se dopo '='
		}
#endif
	}

const char *REGISTRI::operator[](uint8_t n) {
	return DT[n];
	}         

const char *REGISTRI::operator[](struct VARS *V) {
	int i;
	
	i=MAKEPTRREG(V->label);
	return DT[i];
	}         

void REGISTRI::Save() {
	register int i;

	if(D>0) {
#if ARCHI  
		char MyBuf[64];
		sprintf(MyBuf,"R0-R%u",D-1);
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
			OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086 
		for(i=0; i<D; i++)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,(union SUB_OP_DEF*)&i,0);
#elif MC68000
		for(i=0; i<D; i++)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
#elif GD24032
		for(i=0; i<D; i++)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,OPDEF_MODE_REGISTRO32,i);
#elif MICROCHIP
		for(i=0; i<D; i++) {
			if(CPUPIC < 2) {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			else {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			}
#endif
		}
	ToGet=0;  
	}

void REGISTRI::Save(int8_t n) {
	register int i;

	if(n>INT_SIZE)
		n=2;
	else
		n=1;
#if ARCHI    
	char MyBuf[64];
	sprintf(MyBuf,"R%d-R%d",D,D+n-1);
	myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,
		OPDEF_MODE_COSTANTE,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086 
	for(i=D; i<(D+n); i++)
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO16,(union SUB_OP_DEF*)&i,0,NULL);
	ToGet=n;  
#elif MC68000
	for(i=D; i<(D+n); i++)
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_REGISTRO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,-1);
	ToGet=n;  
#elif GD24032
	for(i=D; i<(D+n); i++)
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_STACKPOINTER_INDIRETTO,-1,OPDEF_MODE_REGISTRO32,i);
	ToGet=n;  
#elif MICROCHIP
	for(i=D; i<(D+n); i++) {
		if(CPUPIC < 2) {
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
			}
		else {
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,pushString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
			}
		}
	ToGet=n;  
#endif
	}

void REGISTRI::Get() {
	register int i;

	if(ToGet) {
#if ARCHI    
		char MyBuf[64];
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,0);
		sprintf(MyBuf,"R%u-R%u",D,D+ToGet-1);
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
			OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086 
		for(i=D+ToGet-1; i>=D; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,(union SUB_OP_DEF*)&i,0);
#elif MC68000
		for(i=D+ToGet-1; i>=D; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,OPDEF_MODE_REGISTRO32,i);
#elif GD24032
		for(i=D+ToGet-1; i>=D; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO32,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1);
#elif MICROCHIP
		for(i=D+ToGet-1; i>=D; i--) {
			if(CPUPIC < 2) {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			else {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			}
#endif
		}
	else if(D>0) {
#if ARCHI    
		char MyBuf[64];
//		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,movString,OPDEF_MODE_REGISTRO32,0,NULL);
		sprintf(MyBuf,"R0-R%u",D-1);
		myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,
			OPDEF_MODE_REGISTRI,(union SUB_OP_DEF*)MyBuf,0);
#elif Z80 || I8086
		for(i=D-1; i>=0; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO16,(union SUB_OP_DEF*)&i,0);
#elif MC68000
		for(i=D-1; i>=0; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"move.l",OPDEF_MODE_STACKPOINTER_INDIRETTO,+1,OPDEF_MODE_REGISTRO16,i);
#elif GD24032
		for(i=D-1; i>=0; i--)
			myCC->PROCOper(LINE_TYPE_ISTRUZIONE,"MOV.d",OPDEF_MODE_REGISTRO16,i,OPDEF_MODE_STACKPOINTER_INDIRETTO,+1);
#elif MICROCHIP
		for(i=D-1; i>=0; i--) {
			if(CPUPIC < 2) {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			else {
				myCC->PROCOper(LINE_TYPE_ISTRUZIONE,popString,OPDEF_MODE_REGISTRO,(union SUB_OP_DEF*)&i,0);
				}
			}
#endif
		}
	else {
		myCC->PROCError(1001,"Regs Get");
		}  
	ToGet=0;  
	}

/*
void REGISTRI::Store(long t, int s, struct VARS far *V) {

	VType[D]=t;
	VSize[D]=s;
	VVar[D]=V;
	}
int REGISTRI::Comp(long t, int s, struct VARS far *V) {

	return (((VType[D] & 0x7fffffff)==(t & 0x7fffffff)) && (VSize[D]==s) && (VVar[D]==V));
	}
	*/

void REGISTRI::Reset() {

#if ARCHI    
	D=0; P=8;
#elif Z80     
	D=0; P=0;
#elif I8086
	D=0; P=8;
#elif MC68000
	D=0; P=8;
#elif GD24032
	D=0; P=16;
#elif I8051
	D=0; P=8;
#elif MICROCHIP
	D=0; P=8;
#endif
	}

int REGISTRI::FNIsReg(const char *s) {
  int i;
  
//  rn=(struct COND *)bsearch((void *)&rc,Reg,sizeof(Reg)/sizeof(struct REGS),sizeof(Reg[0]),(int (*)(const void *, const void *))subCmpReg);

#if ARCHI
  if(strlen(s) > 3)
    return 0;
  i=0;
  while(i<32) {
    if(!stricmp(s,DT[i])) { 
      return i+1;     
      }          
    i++;
    }

#elif Z80
  if(strlen(s) > 2) {
    if(!stricmp(s,DT[16]))
      return 16+1 | 0x100;
    else  
      return 0;
    }
  i=0;
  while(DT[i]) {
    if(!stricmp(s,DT[i]) && DT[i][1]) { 
      return i+1 | 0x100;     
      }          
    i++;
    }
  i=0;
  while(DT[i]) {
    if(!stricmp(s,DT[i]) && !DT[i][1]) { 
      return i+1;     
      }          
    i++;
    }

#elif PIC==16 || PIC==18
  i=0;
  while(Reg[i].s) {
    if(!stricmp(s,Reg[i].s) && !Reg[i].s[1]) { 
      return i+1;     
      }          
    i++;
    }

#elif GD24032
  if(strlen(s) > 3)
    return 0;
  i=0;
  while(i<32) {
    if(i<33 /* WP e PC non vanno qua, SP sì 2026!*/ && !stricmp(s,DT[i])) { 
      return i+1;     
      }          
    i++;
    }

#elif I8086
  i=0;
  while(i<16) {
    if(!stricmp(s,DT[i]) && DT[i][2] && DT[i][3]) {			// 32bit
      return i+1 | 0x400;
      }          
    i++;
    }
  i=0;
  while(i<16) {
    if(!stricmp(s,DT[i]) && DT[i][2]) {		// 16bit
      return i+1 | 0x200;
      }          
    i++;
    }
  i=0;
  while(i<16) {
    if(!stricmp(s,DT[i])) {
      return (DT[i][1]=='H' || DT[i][1]=='L') ? i+1 : (i+1 | 0x100);
      }          
    i++;
    }
#elif MC68000
  i=0;
  while(i<16) {
    if(!stricmp(s,DT[i])) {			// 
      return i+1;
      }          
    i++;
    }

#endif
  return 0;
  }




BOOL WINAPI LibMain(HINSTANCE  hinstDLL,	// handle to DLL module 
    DWORD  fdwReason,	// reason for calling function 
    LPVOID  lpvReserved 	// reserved
		) {
	register int i,j;    

  if(fdwReason==DLL_PROCESS_ATTACH) {
		}
  else if(fdwReason==DLL_PROCESS_DETACH) {
		}
	return 1;
  }


/****************************************************************************
		FUNZIONE:  WEP(int)

		SCOPO:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
							called automatically by Windows when the DLL is unloaded (no
							remaining tasks still have the DLL loaded).  It is strongly
							recommended that a DLL have a WEP() function, even if it does
							nothing but returns success (1), as in this example.

*******************************************************************************/
int WINAPI WEP(int bSystemExit) {
	
	return(1);
  }


extern "C" int __stdcall Compila(CWnd *v, int argc, char **argv) {
	Ccc theCC(v);

	return theCC.CompilaC(argc, argv);
	}

extern "C" DWORD __stdcall GetVersione(char *s,char *s1) {
	
#if ARCHI
  wsprintf(s,"Acorn ARM (Archimedes)");
#elif Z80
  wsprintf(s,"Zilog Z80");
#elif I8086
  wsprintf(s,"Intel 80x86");
#elif MC68000
  wsprintf(s,"Motorola 68000");
#elif GD24032
  wsprintf(s,"Dario's GD24032");
#elif I8051
  wsprintf(s,"Intel/Philips 8051");
#elif MICROCHIP
  wsprintf(s,"Microchip PIC");
#elif GD24032
  wsprintf(s,"GD24032");
#endif
  wsprintf(s1,"%d.%02d",HIBYTE(__VER__),LOBYTE(__VER__));
	return __VER__;
	}





CString CTimeEx::getNow(int ex) {
	int i;
	CString S;

	switch(ex) {
		case 0:
			S=CTime::GetCurrentTime().Format("%d/%m/%Y %H:%M:%S");
			break;
		case 1:
		case 2:
			S.Format(ex == 1 ? _T("sono le ore %d e %d del %d %s %d") : _T("sono le ore %02d e %02d del %d %s %d"),
				GetCurrentTime().GetHour(),GetCurrentTime().GetMinute(),
				GetCurrentTime().GetDay(),(LPTSTR)(LPCTSTR)Num2Mese(GetCurrentTime().GetMonth()),
				GetCurrentTime().GetYear());
			break;
		}

	return S;
  }


CString CTimeEx::getNowGMT(bool bAddCR) {
	time_t aclock;
	struct tm *newtime;
	int i;
	CString S;

	S.Format(_T("%s %s %02u %02u:%02u:%02u %04u"),
		Num2Day3(CTime::GetCurrentTime().GetDayOfWeek()),
		Num2Month3(CTime::GetCurrentTime().GetMonth()),
		CTime::GetCurrentTime().GetDay(),
		CTime::GetCurrentTime().GetHour(),
		CTime::GetCurrentTime().GetMinute(),
		CTime::GetCurrentTime().GetSecond(),
		CTime::GetCurrentTime().GetYear());

#ifndef _WIN32_WCE
	i=-(_timezone/3600)+(_daylight ? 1 : 0);
#else
	i=0;
#endif
	if(!i)
		S+=_T(" UTC");				// anche "GMT"
	else {
		CString S2;
		S2.Format(_T(" %c%02d00"),i>=0 ? '+' : '-',abs(i));
		S+=S2;
		}
	if(bAddCR)
		S+="\r\n";

	return S;
	}

CString CTimeEx::getNowGoogle(bool bAddCR) {
	time_t aclock;
	struct tm *newtime;
	int i;
	CString S;

	S.Format(_T("%04u-%02u-%02uT%02u:%02u:%02uZ"),
		CTime::GetCurrentTime().GetYear(),
		CTime::GetCurrentTime().GetMonth(),
		CTime::GetCurrentTime().GetDay(),
		CTime::GetCurrentTime().GetHour(),
		CTime::GetCurrentTime().GetMinute(),
		CTime::GetCurrentTime().GetSecond());

#ifndef _WIN32_WCE
	i=-(_timezone/3600)+(_daylight ? 1 : 0);
#else
	i=0;
#endif
	if(!i)
		S+=_T(" UTC");				// anche "GMT"
	else {
		CString S2;
		S2.Format(_T(" %c%02d00"),i>=0 ? '+' : '-',abs(i));
		S+=S2;
		}
	if(bAddCR)
		S+="\r\n";

	return S;
	}

CString CTimeEx::Num2Mese(int i) {

  switch(i) {
		case 1:
      return _T("Gennaio");
			break;
		case 2:
      return _T("Febbraio");
			break;
		case 3:
      return _T("Marzo");
			break;
		case 4:
      return _T("Aprile");
			break;
		case 5:
      return _T("Maggio");
			break;
		case 6:
      return _T("Giugno");
			break;
		case 7:
      return _T("Luglio");
			break;
		case 8:
      return _T("Agosto");
			break;
		case 9:
	    return _T("Settembre");
			break;
		case 10:
      return _T("Ottobre");
			break;
		case 11:
      return _T("Novembre");
			break;
		case 12:
      return _T("Dicembre");
			break;
	  }
  
  }

CString CTimeEx::Num2Giorno(int i) {

  switch(i) {
		case 1:
      return _T("Domenica");
			break;
		case 2:
      return _T("Luned");
			break;
		case 3:
      return _T("Marted");
			break;
		case 4:
      return _T("Mercoled");
			break;
		case 5:
      return _T("Gioved");
			break;
		case 6:
      return _T("Venerd");
			break;
		case 7:
      return _T("Sabato");
			break;
	  }
  }

CString CTimeEx::Num2Month3(int i) {

  switch(i) {
		case 1:
      return _T("Jan");
			break;
		case 2:
      return _T("Feb");
			break;
		case 3:
      return _T("Mar");
			break;
		case 4:
      return _T("Apr");
			break;
		case 5:
      return _T("May");
			break;
		case 6:
      return _T("Jun");
			break;
		case 7:
      return _T("Jul");
			break;
		case 8:
      return _T("Aug");
			break;
		case 9:
	    return _T("Sep");
			break;
		case 10:
      return _T("Oct");
			break;
		case 11:
      return _T("Nov");
			break;
		case 12:
      return _T("Dec");
			break;
	  }
  
  }

CString CTimeEx::Num2Day3(int i) {

  switch(i) {
		case 1:
      return _T("Sun");
			break;
		case 2:
      return _T("Mon");
			break;
		case 3:
      return _T("Tue");
			break;
		case 4:
      return _T("Wed");
			break;
		case 5:
      return _T("Thu");
			break;
		case 6:
      return _T("Fri");
			break;
		case 7:
      return _T("Sat");
			break;
	  }
  }

int CTimeEx::getMonthFromString(const CString S) {

	if(!S.CompareNoCase(_T("JAN")))
		return 1;
	else if(!S.CompareNoCase(_T("FEB")))
		return 2;
	else if(!S.CompareNoCase(_T("MAR")))
		return 3;
	else if(!S.CompareNoCase(_T("APR")))
		return 4;
	else if(!S.CompareNoCase(_T("MAY")))
		return 5;
	else if(!S.CompareNoCase(_T("JUN")))
		return 6;
	else if(!S.CompareNoCase(_T("JUL")))
		return 7;
	else if(!S.CompareNoCase(_T("AUG")))
		return 8;
	else if(!S.CompareNoCase(_T("SEP")))
		return 9;
	else if(!S.CompareNoCase(_T("OCT")))
		return 10;
	else if(!S.CompareNoCase(_T("NOV")))
		return 11;
	else if(!S.CompareNoCase(_T("DEC")))
		return 12;

	return 0;
	}

int CTimeEx::getMonthFromGMTString(const CString S) {
	CString S2=S.Left(3);

	return getMonthFromString(S2);

	}


CString CTimeEx::getFasciaDellaGiornata() {

	if(GetCurrentTime().GetHour() >=7 && GetCurrentTime().GetHour()<13)
		return _T("stamattina");
	else if(GetCurrentTime().GetHour() >=13 && GetCurrentTime().GetHour()<20)
		return _T("oggi");
	else if(GetCurrentTime().GetHour() >=20 && GetCurrentTime().GetHour()<24)
		return _T("stasera");
	else if(GetCurrentTime().GetHour() >=0 && GetCurrentTime().GetHour()<7)
		return _T("stanotte");

	}

CString CTimeEx::getSaluto() {

	if(GetCurrentTime().GetHour() >=7 && GetCurrentTime().GetHour()<13)
		return _T("Buongiorno");
	else if(GetCurrentTime().GetHour() >=13 && GetCurrentTime().GetHour()<20)
		return _T("Buon pomeriggio");
	else if(GetCurrentTime().GetHour() >=20 && GetCurrentTime().GetHour()<24)
		return _T("Buonasera");
	else if(GetCurrentTime().GetHour() >=0 && GetCurrentTime().GetHour()<7)
		return _T("Buonanotte");

	}

CTime CTimeEx::parseGMTTime(const CString S) {
	char *p;
	int i,j,tzFound=0,reverseUTC=0;
	struct tm t;
	CString s=S;
	
//	_tzset();			// questo imposterebbe la timezone, che altrimenti potrebbe defaultare a -8h
	// v. Joshua.cpp::InitInstance

	while(_istspace(s.GetAt(0)))
		s=s.Mid(1);
	if(_istalpha(s.GetAt(0))) {
		s.MakeUpper();
		if(!s.Left(2).CompareNoCase(_T("SU")))
			i=0;
		else if(!s.Left(2).CompareNoCase(_T("MO")))
			i=1;
		else if(!s.Left(2).CompareNoCase(_T("TU")))
			i=2;
		else if(!s.Left(2).CompareNoCase(_T("WE")))
			i=3;
		else if(!s.Left(2).CompareNoCase(_T("TH")))
			i=4;
		else if(!s.Left(2).CompareNoCase(_T("FR")))
			i=5;
		else if(!s.Left(2).CompareNoCase(_T("SA")))
			i=6;
		else				// NON deve capitare... patch per evitare il peggio!
			i=0;
		t.tm_wday=i;
		s=s.Mid(3);
		while(!isspace(s.GetAt(0)))
			s=s.Mid(1);
		if(s.GetAt(0) ==',')
			s=s.Mid(1);
		s=s.Mid(1);
no_day:
		while(_istspace(s.GetAt(0)))
			s=s.Mid(1);
		if(_istdigit(s.GetAt(0))) {
			t.tm_mday=_ttoi((LPTSTR)(LPCTSTR)s);
			while(iswdigit(s.GetAt(0)))
				s=s.Mid(1);
			s=s.Mid(1);
			t.tm_mon=getMonthFromGMTString(s);
			s=s.Mid(4);
			t.tm_year=_ttoi((LPTSTR)(LPCTSTR)s);
			if(t.tm_year<80)
				t.tm_year+=100;
			if(t.tm_year>=200)
				t.tm_year-=1900;
			s=s.Mid(5);
			t.tm_hour=_ttoi(s);
			s=s.Mid(3);
			t.tm_min=_ttoi(s);
			s=s.Mid(3);
			t.tm_sec=_ttoi(s);
			if(s.GetAt(1) == '-')
				reverseUTC=1;
			s=s.Mid(2,2);
			i=_ttoi(s);
			if(reverseUTC)
				i=-i;
			}
		else {
			t.tm_mon=getMonthFromGMTString(s);
			s=s.Mid(4);
			t.tm_mday=_ttoi(s);
			s=s.Mid(3);
			t.tm_hour=_ttoi(s);
			s=s.Mid(3);
			t.tm_min=_ttoi(s);
			s=s.Mid(3);
			t.tm_sec=_ttoi(s);
			s=s.Mid(3);
			if(s.GetAt(0) == 'U') {		// variante con "UTC 1998
				t.tm_year=_ttoi(s.Mid(4))-1900;
				i=0;								// greenwich
				tzFound=1;
				}
			else {							  // variante con 1998 +0100
				t.tm_year=_ttoi(s)-1900;
				s=s.Mid(4);
				while(s.GetLength()>0 && _istspace(s.GetAt(0)))
					s=s.Mid(1);
				if(s.GetLength()>0) {		// se la timezone segue l'anno...
					if(_istdigit(s.GetAt(0)) || s.GetAt(0)=='+' || s.GetAt(0)=='-') {
						tzFound=1;
						if(s.GetAt(0) == '-')
							reverseUTC=1;
						if(s.GetAt(0)=='+' || s.GetAt(0)=='-')
							s=s.Mid(1);
						s=s.Mid(0,2);		// stronco i minuti!
						i=_ttoi(s);									// ...la leggo
						if(reverseUTC)
							i=-i;
						}
					else if(S.GetAt(0)=='U' || S.GetAt(0)=='G')	{		// per ora solo UTC o GMT (sono lo stesso?)!
						i=0;								// greenwich
						tzFound=1;
						}
					}
				}
			}
		}
	else {
		if(_istalpha(s.GetAt(4))) {		// caso in cui manca il giorno della settimana, poi idem come sopra...
			s.MakeUpper();
			t.tm_wday=0;
			goto no_day;
			}
		else {
			s=s.Mid(2);		// salto \xd\xa
			s=s.Mid(6);
			t.tm_year=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			t.tm_mon=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			t.tm_mday=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			t.tm_hour=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			t.tm_min=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			t.tm_sec=_ttoi((LPTSTR)(LPCTSTR)s);
			s=s.Mid(3);
			i=_ttoi((LPTSTR)(LPCTSTR)s);
			}
		}
	if(tzFound)
#ifndef _WIN32_WCE
		t.tm_hour=t.tm_hour-i-(_timezone/3600)+(_daylight ? 1 : 0);
#else
		t.tm_hour=t.tm_hour-i-(0/3600)+0;
#endif

	return CTime::CTime(t.tm_year+1900,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
	}

CTime CTimeEx::parseTime(const CString S) {		// semplice, tipo "05/10/10 12:00"
	char *p;
	int i,j,tzFound=0;
	WORD nDay,nMonth,nYear,nHour,nMinute,nSecond;
	CString s=S;
	
//	_tzset();			// questo imposterebbe la timezone, che altrimenti potrebbe defaultare a -8h
	// v. Joshua.cpp::InitInstance

	p=(char *)(LPCTSTR)S;
	while(isspace(*p) && *p)
		p++;
	nDay=atoi(p);		// 2 digit
	while(isdigit(*p))
		p++;
	p++;
	while(isspace(*p) && *p)
		p++;
	nMonth=atoi(p);			// 2 digit
	while(isdigit(*p))
		p++;
	p++;
	while(isspace(*p) && *p)
		p++;
	nYear=atoi(p);		// 2-4 digit (v.sotto)
	while(isdigit(*p))
		p++;

	nHour=0;
	nMinute=0;
	nSecond=0;
	if(*p) {
		p++;
		while(isspace(*p) && *p)
			p++;
		nHour=atoi(p);		// 2 digit
		while(isdigit(*p))
			p++;
		p++;
		while(isspace(*p) && *p)
			p++;
		nMinute=atoi(p);	// 2 digit

		while(isdigit(*p))
			p++;
		if(*p) {
			p++;
			while(isspace(*p) && *p)
				p++;
			nSecond=atoi(p); // 2 digit
			}
		}

	if(nYear < 100) {
		if(nYear < 80)
			nYear+=2000;
		else
			nYear+=1900;
		}


	i=0;
	if(tzFound)
#ifndef _WIN32_WCE
		nHour=nHour-i-(_timezone/3600)+(_daylight ? 1 : 0);
#else
		nHour=nHour-i-(0/3600)+0;
#endif

	return CTime::CTime(nYear,nMonth,nDay,nHour,nMinute,nSecond);
	}

bool CTimeEx::isWeekend() {

	return GetCurrentTime().GetDayOfWeek() == 1 || GetCurrentTime().GetDayOfWeek() == 7;
	}

bool CTimeEx::isWeekend(CTime t) {

	return t.GetDayOfWeek() == 1 || t.GetDayOfWeek() == 7;
	}

WORD CTimeEx::GetDayOfYear() {
	struct tm *myTm;

	myTm=CTime::GetCurrentTime().GetLocalTm();
	return myTm->tm_yday;
	}

void CTimeEx::AddMonths(int n) {
	int nYear  = GetYear();
	int nMonth = GetMonth();

	while(n--) {
		nMonth++;
		if(nMonth > 12) {
			nMonth = 1;
			++nYear;
			}
		}
	// construct first day of next month  
	CTimeEx tNext(nYear, nMonth, 1, 0, 0, 0); 
	// get the number of days in the next month
	int nDays = tNext.GetDaysOfMonth();
	// construct the date for next month
	int nNewDay = min(nDays, GetDay());
	CTimeEx tNew(nYear, nMonth, 
						nNewDay, 
						GetHour(), GetMinute(), GetSecond());
   // assign the new date
  *this = tNew;
	}

int CTimeEx::GetDaysOfMonth() { 
   CTimeEx tNext(GetYear(), GetMonth(), 1, 0, 0, 0); 

   tNext += CTimeSpan(31, 1, 0, 0); 

   return 32 - tNext.GetDay();
	}



//-------------------------------------------------------
/*
CinziaG    5.8.2017
*/

//--------------------------------------------------------------
CLogFile::CLogFile(const CString s, const CWnd *myWnd, DWORD m) :
	nomeFile(s),textWnd(myWnd),mode(m) { 
	
	hIndexFile=NULL;
	if((mode & 0xff) >= dateTimeMillisec) {
		timeBeginPeriod(1);
		}
	if(mode & keepOpen) {
	try {
		if(Open())
			SeekToEnd(); 
		}
	catch(CFileException e) {
		;
		}
	
		}

	if(mode & useIndex)
		hIndexFile=new CFile;

	InitializeCriticalSection(&m_cs);
	}

CLogFile::CLogFile(CFile *f2,const CWnd *myWnd,DWORD m) :
	textWnd(myWnd),mode(m) { 

	m_hFile=f2->m_hFile;
	hIndexFile=NULL;
	mode &= ~keepOpen;
	if((mode & 0xff) >= dateTimeMillisec) {
		timeBeginPeriod(1);
		}
	try {
		SeekToEnd(); 
		}
	catch(CFileException e) {
		;
		}
	
	if(mode & useIndex)
		hIndexFile=new CFile;

	}

CLogFile::~CLogFile() { 

	if((mode & 0xff) >= dateTimeMillisec) {
		timeEndPeriod(1);
		}

	if(mode & keepOpen)
		Close();

	if(hIndexFile)
		delete hIndexFile;			hIndexFile=NULL;

	}


int CLogFile::Open() { 
	int i;
	
	i=CStdioFile::Open(nomeFile,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::typeText /*2023 per multithread.. | CFile::shareDenyWrite*/);

	if(mode & useIndex) {
		getIndexFileName();
		if(hIndexFile)
			hIndexFile->Open(nomeFileNdx,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite /*| CFile::shareDenyWrite*/);
		}
	
	return i;
	}

void CLogFile::Close() { 
	
	CStdioFile::Close();

	if(mode & useIndex) {
		if(hIndexFile)
			hIndexFile->Close();
		}
	}


// RICORDARSI DI CASTARE A int gli eventuali valori 32bit che sforano (-2miliardi o + 2 miliardi) o vengono mal-messi dalla chiamata...
int CLogFile::print(int m,const TCHAR *s,...) {		 // m=0 info, 1= letture skynet, 2=errore
	TCHAR myBuf[2048],myBuf1[2048],myFmt[16];
	register int i,j,ch,k;
	int n,pad;
	double d;
	CTime myT;
	TCHAR *p,*p_myBuf;
//	static bool inUse;
  va_list vl;
	CString S;
	char padch=' ';

//	if(mode & keepOpen) {
//		while(inUse)
//			Sleep(20);
//		}
//	if(!inUse) {

	EnterCriticalSection(&m_cs);

//		inUse=1;
	va_start(vl,s);
	p_myBuf=myBuf;
	if(m & 0x100) {			// se un flag almeno...
		myBuf[0]=(m & 0xff)+'$';				// flag tipo riga
		myBuf[1]=' ';			// marker di 'gi letto'
		p_myBuf=myBuf+2;
		}
	if(mode & 0xff) {			// no dontUseDate...
		S=getNow();
		_tcscpy(p_myBuf,(LPTSTR)(LPCTSTR)S);
		j=_tcslen(myBuf);
		myBuf[j++]=' ';
		p_myBuf=&myBuf[j];
		}
	else
		j=p_myBuf-myBuf;
	myBuf[j]=0;
	i=0;
	while(ch=s[i++]) {
		pad=0;
		if(ch == '%') {
			if(s[i] == '%') {
				i++;
				goto no_var;
				}
			myFmt[0]=ch;
			k=1;
			while((ch=s[i++]) && !isalpha(ch))			// salvo dettagli...
				myFmt[k++]=ch;
			myFmt[k++]=ch;			// ...copio effettivo format-type
			myFmt[k]=0;
			if(myFmt[1]=='-') {
				if(isdigit(myFmt[2])) {
					pad=-atoi(&myFmt[2]);
					}
				// altrimenti...
				}
			else if(myFmt[1]=='+') {
				// segno ...
				}
			if(isdigit(myFmt[1])) {
				pad=atoi(&myFmt[1]);
				}
			switch(myFmt[k-1]) {
				case 'l':
				case 'L':
//					k++;
					// bah me ne fotto e proseguo!
					myFmt[k-1]=s[i++];
				case 'd':
				case 'D':
					n=va_arg(vl,int);
					goto subCopia0;
					break;
				case 'p':
				case 'P':
					myBuf[j++]='0';
					myBuf[j++]='x';
				case 'u':
				case 'U':
				case 'x':
				case 'X':
				case 'o':		//ottale
				case 'O':
					n=va_arg(vl,unsigned int);
subCopia0:
					sprintf(myBuf1,myFmt,n);
subCopia:
					if(pad) {
						pad-=_tcslen(myBuf1);
						if(pad<0)
							pad=0;
						while(pad--)
							myBuf[j++]=padch;		//put(' ');
						}
					_tcscpy(myBuf+j,myBuf1);
subCopia2:
					j+=_tcslen(myBuf1);
					break;
				case 'f':
				case 'g':
					d=va_arg(vl,double);
					sprintf(myBuf1,myFmt,d);
					goto subCopia;
					break;
				case 's':
					p=va_arg(vl,TCHAR *);
/*					if(p<(char*)100)
						p="culo";*/
					if(pad) {
						pad-=_tcslen(p);
						if(pad<0)
							pad=0;
						while(pad--)
							myBuf[j++]=padch;		//put(' ');
						}
					_tcsncpy(myBuf+j,p,2000-j);
					j+=min(_tcslen(p),2000-j);
					myBuf[j]=0;
					break;
				case 'c':
					n=va_arg(vl,char);
					myBuf[j++]=n;
					break;
				case 't':
					myT=va_arg(vl,CTime);
					S.Format(_T("%02u/%02u/%02u %02u:%02u:%02u"),
						myT.GetDay(),
						myT.GetMonth(),
						myT.GetYear(),
						myT.GetHour(),
						myT.GetMinute(),
						myT.GetSecond());
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
				case 'T':
					myT=va_arg(vl,CTime);
					S=myT.Format(_T("%02u %b %04u %02u:%02u:%02u"));
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
/*					case '%':
					myBuf[j++]='%';
					break;*/
				default:
					break;
				}
			}
		else {
			if(ch == '\n')
				myBuf[j++]=13;
no_var:
			myBuf[j++]=ch;
			}
		if(j>2000)
			break;
		}

	myBuf[j]=0;
//	((CMainFrame *)theApp.m_pMainWnd)->m_wndStatusBar.SetWindowText(myBuf+2);
	if(textWnd && *textWnd) {
		TCHAR *p=_tcsdup(myBuf+2);
		if(p) {
			if(::IsWindow(textWnd->m_hWnd))	{// serve in chiusura...
//					if(*p=='\'')
//						((CWnd *)textWnd)->PostMessage(WM_UPDATE_PANE2,0,(DWORD)p);	//a che serviva?? la 2 label la uso per server web...
//					else
					((CWnd *)textWnd)->PostMessage(WM_UPDATE_PANE,0,(DWORD)p);
				}
			else
				free(p);
			}
		else
			free(p);
		}
//	if(textWnd && *textWnd && ((CMainFrame *)(*textWnd))->m_wndStatusBar)
	//	((CMainFrame *)*textWnd)->m_wndStatusBar.SetPaneText(1,myBuf+2,TRUE);

//		myBuf[j++]=13;
	myBuf[j++]=10;
	myBuf[j]=0;

	i=0;
try {
	if(mode & keepOpen)
		goto already_open;
	if(Open()) {
		n=SeekToEnd(); 
already_open:
		WriteString(myBuf);

		if(mode & useIndex) {
			hIndexFile->SeekToEnd();
			hIndexFile->Write(&n,sizeof(DWORD));
			}

		if(mode & flushImmediate)
			Flush();	 // rimesso... anche se rallenta!
	//FlushFileBuffers
	if(!(mode & keepOpen))
		Close();
		}
	}
catch(CFileException e) {
	AfxMessageBox(e.m_cause);
	i=-1;
	}


	va_end(vl);
//		inUse=0;
	LeaveCriticalSection(&m_cs);
//		}
	return i;
  }

void CLogFile::operator<<(const TCHAR *s) {
	char *myBuf;
	int i,j;

	myBuf=new char[_tcslen(s)+1+64];

	myBuf[0]=(flagInfo & 0xff) + '$';				// flag tipo riga, sempre 0!
	myBuf[1]=' ';				// marker di 'gi letto'
	_tcscpy(myBuf+2,(LPTSTR)(LPCTSTR)getNow());
	j=_tcslen(myBuf);
	myBuf[j++]=' ';
//	i=_tcslen(s);
//	_tcsncpy(myBuf+j,s,min(i+1,1000));
	_tcscpy(myBuf+j,s);
	j=_tcslen(myBuf);
//	myBuf[j++]=13;
	myBuf[j++]=10;
	myBuf[j]=0;

	try {
		if(mode & keepOpen)
			goto already_open;
		if(Open()) {
			SeekToEnd(); 
already_open:
			WriteString(myBuf);

			if(mode & useIndex) {
				int n=SeekToEnd(); 
				hIndexFile->SeekToEnd();
				hIndexFile->Write(&n,sizeof(DWORD));
				}

			if(mode & flushImmediate)
				Flush();	 // rimesso... anche se rallenta!
		//FlushFileBuffers
		if(!(mode & keepOpen))
			Close();
			}
		}
	catch(CFileException e) {
		i=-1;
		}

	delete myBuf;
	}

CString CLogFile::getNow() const {
	int m=mode & 0xffff;
	CString S;

	S.Format(_T("%02u/%02u/%02u"),
		CTime::GetCurrentTime().GetDay(),
		CTime::GetCurrentTime().GetMonth(),
		CTime::GetCurrentTime().GetYear());
	if((mode & 0xff) >= dateTime) {
		CString S2;
		S2.Format(_T("%02u:%02u:%02u"),
			CTime::GetCurrentTime().GetHour(),
			CTime::GetCurrentTime().GetMinute(),
			CTime::GetCurrentTime().GetSecond());
		S+=_T(" ");
		S+=S2;
	if((mode & 0xff) >= dateTimeMillisec) {
			CString S3;
			S3.Format(_T("%u"),/*GetTickCount*/ timeGetTime());
			S+=_T(" ");
			S+=S3;
			}
		}

	return S;
	}

CString CLogFile::getNowApache() {
	CString S;

	S.Format(_T("%02u/%s/%02u:%02u:%02u:%02u %02d00"),
		CTime::GetCurrentTime().GetDay(),
		CTimeEx::Num2Month3(CTime::GetCurrentTime().GetMonth()),
		CTime::GetCurrentTime().GetYear(),
		CTime::GetCurrentTime().GetHour(),
		CTime::GetCurrentTime().GetMinute(),
		CTime::GetCurrentTime().GetSecond(),
		-(_timezone/3600)+(_daylight ? 1 : 0)
		);

	return S;
	}

char *CLogFile::getLine(int n,char *s,UINT nMax) {
	char myBuf[1024];
	CStdioFile mF;

	if(!n)
		goto errore;

	if(mode & useIndex) {
		int myPos;

		if(mF.Open(nomeFile,CFile::modeRead | CFile::typeText | CFile::shareDenyNone)) {
			CFile mF2;

			if(mF2.Open(nomeFileNdx,CFile::modeRead | CFile::shareDenyNone)) {
				mF2.Seek(n*sizeof(DWORD),CFile::begin);
				mF2.Read(&myPos,sizeof(DWORD));
				mF2.Close();

				mF.Seek(myPos,CFile::begin);
				mF.ReadString(myBuf,nMax);
				mF.Close();
				_tcscpy(s,myBuf);
				}
			}
		}
	else {

		nMax=min(nMax,1000);
		if(mF.Open(nomeFile,CFile::modeRead | CFile::typeText | CFile::shareDenyNone)) {
			while(n) {
				if(!mF.ReadString(myBuf,nMax))
					break;
				n--;
				}
			mF.Close();
			if(!n)
				_tcscpy(s,myBuf);
			else
				goto errore;
			}
		else {
errore:
			s=NULL;
			}
		}

fine:
	return s;
	}

DWORD CLogFile::getTotLines() const {
	DWORD n=0;
	char myBuf[1024];
	CStdioFile mF;

	if(mode & useIndex) {
		if(mF.Open(nomeFileNdx,CFile::modeRead | CFile::shareDenyNone)) {
			n=mF.SeekToEnd();
			mF.Close();

			n/=sizeof(DWORD);
			}

		}

	else {
		if(mF.Open(nomeFile,CFile::modeRead | CFile::typeText | CFile::shareDenyNone)) {
			while(mF.ReadString(myBuf,1000))
				n++;
			mF.Close();
			}
		}

	// fare un confronto tra i due e forzare ReIndex?? oppure... usare l'uno Oppure l'altro?


	return n;
	}

int CLogFile::clearAll() {
	
	if(mode & keepOpen)
		Close();
	CStdioFile::Open(nomeFile,CFile::modeCreate);
	CStdioFile::Close();
	if(mode & useIndex)
		ReIndex();
	if(mode & keepOpen)
		Open();
	return 1;
	}

bool CLogFile::GetStatus(CFileStatus &fs) {

	if(mode & keepOpen)
		return CStdioFile::GetStatus(fs) ? TRUE : FALSE;
	else {
		if(Open()) {
			int i=CStdioFile::GetStatus(fs);
			Close();
			return i ? TRUE : FALSE;
			}
		else
			return FALSE;
		}
	}


char *CLogFile::getAsHex(const byte *s,char *d,UINT nMax) {

	while(nMax--) {
		wsprintf(d,"%02X ",*s);
		d+=3;
		s++;
		}

	return d;
	}

#ifdef _WIN32_WCE

void CStdioFileEx::WriteString(CString S) {
	
	Write((LPTSTR)(LPCTSTR)S,S.GetLength());

	}

CString CStdioFileEx::ReadString() {
	CString S;	
	char myBuf[64];
	int i;

	do {
		i=Read(myBuf,1);
		if(i<1)
			break;
		S+=*myBuf;
		if(*myBuf=='\n')
			break;
		} while(1);

	return S;
	}

#endif

int CLogFile::ReIndex() {
	int i=0,n;
	CFile hTmpIndexFile;
	CStdioFile mF;

	if(!(mode & useIndex))
		return -1;

	if(mode & keepOpen) {
		Close();
		if(hIndexFile) 
			hIndexFile->Close();
		}

	getIndexFileName();

	hTmpIndexFile.Open(nomeFileNdx,CFile::modeCreate | CFile::modeReadWrite | CFile::shareExclusive);

	n=0;
	hTmpIndexFile.Write(&n,sizeof(DWORD));

	if(mF.Open(nomeFile,CFile::modeRead | CFile::typeText | CFile::shareDenyNone)) {
		char myBuf[1024];

		i=1;

		while(mF.ReadString(myBuf,1000) > 0) {
			n=mF.GetPosition();
			hTmpIndexFile.Write(&n,sizeof(DWORD));
			}
		mF.Close();
		}

	hTmpIndexFile.Close();

	if(mode & keepOpen)
		Open();

	return i;
	}

CString CLogFile::getIndexFileName() {
	int i;

	i=nomeFile.Find('.');
	if(i>=0) {
		nomeFileNdx=nomeFile.Left(i+1)+"ndx";
		}
	else {
		nomeFileNdx=nomeFile+".ndx";
		}

	return nomeFileNdx;
	}

int CLogFile::RenameAndStore(int how) {
	int i;
	CString S,S1;

	if(mode & keepOpen) {
		Close();
		if(hIndexFile) 
			hIndexFile->Close();
		}

	S1=nomeFile;
	S1=S1.Left(S1.Find('.'));
	S1=S1.Mid(S1.ReverseFind('\\'));
	S=S1+CTime::GetCurrentTime().Format("_%Y_%m_%d.txt");
//	S=nomeFile+'\\'+S+".txt";

	i=1;

	TRY
	{
		Rename(nomeFile,S);
	}
	CATCH( CFileException, e )
	{
		i=0;

    #ifdef _DEBUG
        afxDump << "Impossibile rinominare File " << nomeFile << " , cause = "
            << e->m_cause << "\n";
    #endif
	}
	END_CATCH

	clearAll();
	

	if(mode & keepOpen)
		Open();

	return i;
	}



// ----------------------------------------------------------------------------------------------------------------------------
CString CStringEx::Tokenize(CString delimiter, int& first) {
  CString token;
  int end = Find(delimiter, first);

  if(end != -1) {
    int count = end-first;
    token = Mid(first,count);
    first = end+delimiter.GetLength();
    return token;
	  }
  else {
    int count = GetLength() - first;
    if(count <= 0)
      return "";

    token = Mid(first,count);
    first = GetLength();
    return token;
		}
	}
CStringEx CStringEx::SubStr(int begin, int len) const {
	return CString::Mid(begin, len);
	}
int CStringEx::FindNoCase(CString substr,int start) {
	CString s1=*this,s2;
	s1.MakeUpper();
	s2=substr;
	s2.MakeUpper();
	return s1.Find(s2,start);
	}
int CStringEx::ReverseFindNoCase(CString substr) {
	CString s1=*this,s2;
	int start=s1.GetLength()-s2.GetLength();
	s1.MakeUpper();
	s2=substr;
	s2.MakeUpper();
	while(start>=0) {
		if(s1.Find(s2,start)>=0)
			return start;
		start--;
		}
	return -1;
	}
CStringEx::CStringEx(int i, const char *format, DWORD options) {

	Format(format,i);
	if(options & COMMA_DELIMIT)
		CString::operator=(CommaDelimitNumber(*this));
	}
CStringEx::CStringEx(double d, const char *format, DWORD options) {

	Format(format,d);
	if(options & COMMA_DELIMIT)
		CString::operator=(CommaDelimitNumber(*this));
	}
CStringEx CStringEx::CommaDelimitNumber(const char *s) {
	CStringEx s2=s;												// convert to CStringEx
	return CommaDelimitNumber(s2);
	}
CStringEx CStringEx::CommaDelimitNumber(CString s2) {
	CStringEx dp;
	CStringEx q2;											// working string
	CStringEx posNegChar=s2.Left(1);				// get the first char
	bool posNeg=!posNegChar.IsDigit(0);			// if not digit, then assume + or -

	if(posNeg) 											// if so, strip off
		s2=s2.Mid(1);
	if(s2.Find(decimalChar)>=0) {
		dp=s2.Mid(s2.Find(decimalChar)+1);							// remember everything to the right of the decimal point
		s2=s2.Left(s2.Find(decimalChar));				// get everything to the left of the first decimal point
		}
	while(s2.GetLength() > 3) {									// if more than three digits...
		CStringEx s3(thousandChar);
		s3+=s2.Right(3);		// insert a comma before the last three digits (100's)
		q2=s3+q2;											// append this to our working string
		s2=s2.Left(s2.GetLength()-3);							// get everything except the last three digits
		}
	q2=s2+q2;												// prepend remainder to the working string
	if(!dp.IsEmpty()) {									// if we have decimal point...
		q2+=decimalChar;							// append it and the digits
		q2+=dp;							// append it and the digits
		}
	if(posNeg)											// if we stripped off a +/- ...
		q2=posNegChar+q2;			// add it back in

	return q2;											// this is our final comma delimited string
	}

CStringEx CStringEx::CommaDelimitNumber(DWORD n) {
	CStringEx q2;

	q2.Format("%u",n);
	q2=CommaDelimitNumber(q2);
	return q2;
	}

BYTE CStringEx::Asc(int pos) {

	return GetAt(pos);
	}

int CStringEx::Val(int base) {

	switch(base) {
		case 10:
		default:
			return atoi((LPCTSTR)this);
			break;
		case 16:		// fare...
			break;
		}
	}

double CStringEx::Val() {

	return strtod((LPCTSTR)this,NULL);
	}

void CStringEx::Repeat(int n) {
	CString s2=*this;

	Empty();
	while(n--)
		CString::operator+=(s2);
	}

void CStringEx::Repeat(const char *s,int n) {

	Empty();
	while(n--)
		CString::operator+=(s);
	}

void CStringEx::Repeat(char c,int n) {

	Empty();
	while(n--)
		CString::operator+=(c);
	}

bool CStringEx::IsAlpha(char ch) {

	return (ch>='A' && ch<='Z') || (ch>='a' && ch<='z');
	}

bool CStringEx::IsAlpha(int pos) {

	return IsAlpha(GetAt(pos));
	}

bool CStringEx::IsAlnum(char ch) {

	return IsAlpha(ch) || IsDigit(ch);
	}

bool CStringEx::IsAlnum(int pos) {

	return IsAlnum(GetAt(pos));
	}

bool CStringEx::IsDigit(char ch) {

	return (ch>='0' && ch<='9');
	}

bool CStringEx::IsDigit(int pos) {

	return IsDigit(GetAt(pos));
	}

bool CStringEx::IsPrint(char ch) {

	return (ch>=' ' && ch<'\x7f');			//127 escluso
	}

bool CStringEx::IsPrint(int pos) {

	return IsPrint(GetAt(pos));
	}

void CStringEx::Print() {

	AfxMessageBox(*this);
	}

void CStringEx::Debug() {

#ifdef _DEBUG
	Print();
#endif
	}

WORD CStringEx::GetAsciiLength() {			// utile per saltare ESC ecc in stampa citofono LCD ecc
	WORD i,j;

	for(i=0,j=0; i<GetLength(); i++)
		if(IsPrint(i))
			j++;
	return j;
	}

// _T("%h %l %u %t \"%r\" %>s %b")		v. Apache...

CStringEx CStringEx::FormatTime(int m,CTime mT) {

	if(!(*((DWORD *)&mT)))
		mT=CTime::GetCurrentTime();

	switch(m) {
		case 0:
			CString::operator=(mT.Format("%d/%m/%Y %H:%M:%S"));
			break;
		case 1:
			Format(_T("%02u/%s/%02u:%02u:%02u:%02u %02d00"),
				mT.GetDay(),
"???",//				CTimeEx::Num2Month3(mT.GetMonth()), METTERE :D
				mT.GetCurrentTime().GetYear(),
				mT.GetCurrentTime().GetHour(),
				mT.GetMinute(),
				mT.GetSecond(),
				-(_timezone/3600)+(_daylight ? 1 : 0)
				);
			break;
		case 2:
			CString::operator=(mT.Format(_T("%a, %d %b %Y %H:%M:%S %Z")));
			break;
		}

	return *this;
	}


const char CStringEx::m_base64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz0123456789+/";
const int CStringEx::BASE64_MAXLINE=76;
const char *CStringEx::EOL="\r\n";
const char CStringEx::decimalChar=',',CStringEx::thousandChar='.';		// GetLocale ??
const char CStringEx::CRchar='\r',CStringEx::LFchar='\n',CStringEx::TABchar='\t';
CStringEx CStringEx::Encode64() {
	CStringEx S2;

  //Set up the parameters prior to the main encoding loop
  int nInPos  = 0;
  int nLineLen = 0;

  // Get three characters at a time from the input buffer and encode them
  for(int i=0; i<GetLength()/3; ++i) {

    //Get the next 2 characters
    int c1 = Asc(nInPos++) & 0xFF;
    int c2 = Asc(nInPos++) & 0xFF;
    int c3 = Asc(nInPos++) & 0xFF;

    //Encode into the 4 6 bit characters
    S2 += m_base64tab[(c1 & 0xFC) >> 2];
    S2 += m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
    S2 += m_base64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
    S2 += m_base64tab[c3 & 0x3F];
    nLineLen += 4;

    //Handle the case where we have gone over the max line boundary
    if(nLineLen >= BASE64_MAXLINE-3) {
      const char *cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      nLineLen = 0;
			}
		}

  // Encode the remaining one or two characters in the input buffer
  const char *cp;
  switch(GetLength() % 3) {
    case 0:
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    case 1:
    {
      int c1 = Asc(nInPos) & 0xFF;
      S2 += m_base64tab[(c1 & 0xFC) >> 2];
      S2 += m_base64tab[((c1 & 0x03) << 4)];
      S2 += '=';
      S2 += '=';
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    }
    case 2:
    {
      int c1 = Asc(nInPos++) & 0xFF;
      int c2 = Asc(nInPos) & 0xFF;
      S2 += m_base64tab[(c1 & 0xFC) >> 2];
      S2 += m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
      S2 += m_base64tab[((c2 & 0x0F) << 2)];
      S2 += '=';
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    }
    default: 
      ASSERT(FALSE); 
      break;
	  }

  CString::operator=(S2);
  return *this;
	}

int CStringEx::Decode64() {
	CStringEx sInput;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;

  m_nBitsRemaining = 0;

	sInput=*this;
  Empty();  
	if(sInput.GetLength() == 0)
		return 0;

	//Build Decode Table
  int nDecode[256];
	for(int i=0; i<256; i++) 
		nDecode[i] = -2; // Illegal digit
	for(i=0; i<64; i++) {
		nDecode[m_base64tab[i]] = i;
		nDecode[m_base64tab[i] | 0x80] = i; // Ignore 8th bit
		nDecode['='] = -1; 
		nDecode['=' | 0x80] = -1; // Ignore MIME padding char
		}

	// Decode the Input
  i=0;
  TCHAR* szOutput = GetBuffer(sInput.GetLength());
	for(int p=0; p<sInput.GetLength(); p++) {
		int c = sInput[p];
		int nDigit = nDecode[c & 0x7F];
		if(nDigit < -1) {
      ReleaseBuffer();  
			return 0;
			}
		else if(nDigit >= 0) {
			// i (index into output) is incremented by write_bits()
//			WriteBits(nDigit & 0x3F, 6, szOutput, i);
			UINT nScratch;

			m_lBitStorage = (m_lBitStorage << 6) | (nDigit & 0x3F);
			m_nBitsRemaining += 6;
			while(m_nBitsRemaining > 7) {
				nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
				szOutput[i++] = (TCHAR) (nScratch & 0xFF);
				m_nBitsRemaining -= 8;
				}
			}
		}	
  szOutput[i] = _T('\0');
  ReleaseBuffer();

	return i;
	}

CString CStringEx::InsertSeparator(DWORD dwNumber) {

  Format("%u", dwNumber);
  
  for(int i=GetLength()-3; i > 0; i -= 3) {
    Insert(i, ",");
    }

  return *this;
  }

CStringEx CStringEx::FormatSize(DWORD dwFileSize) {
  static const DWORD dwKB = 1024;          // Kilobyte
  static const DWORD dwMB = 1024 * dwKB;   // Megabyte
  static const DWORD dwGB = 1024 * dwMB;   // Gigabyte

  DWORD dwNumber, dwRemainder;

  if(dwFileSize < dwKB) {
//    InsertSeparator(dwFileSize) + " B";		// non funziona (usare  *this o Format) e poi non mi piace!
    InsertSeparator(dwFileSize);
		} 
  else {
    if(dwFileSize < dwMB) {
      dwNumber = dwFileSize / dwKB;
      dwRemainder = (dwFileSize * 100 / dwKB) % 100;

      Format("%s.%02d KB", (LPCSTR)InsertSeparator(dwNumber), dwRemainder);
			}
    else {
      if(dwFileSize < dwGB) {
        dwNumber = dwFileSize / dwMB;
        dwRemainder = (dwFileSize * 100 / dwMB) % 100;
        Format("%s.%02d MB", InsertSeparator(dwNumber), dwRemainder);
				}
      else {
        if(dwFileSize >= dwGB) {
          dwNumber = dwFileSize / dwGB;
          dwRemainder = (dwFileSize * 100 / dwGB) % 100;
          Format("%s.%02d GB", InsertSeparator(dwNumber), dwRemainder);
					}
				}
			}
		}

  // Display decimal points only if needed
  // another alternative to this approach is to check before calling str.Format, and 
  // have separate cases depending on whether dwRemainder == 0 or not.
  Replace(".00", "");

	return *this;
	}



// ------------------------------------------------------------------------------------------------
CSourceFile::CSourceFile(LPCTSTR s) : CFile(s,CFile::modeRead /*| NON VA! e dà pure eccezione CFile::typeText */
																						| CFile::shareDenyWrite 
																						| FILE_FLAG_RANDOM_ACCESS /*CFile::osRandomAccess=0x10000000*/) {

	lineno=savedLineno=savedPositionIdx=0;
	}

char *CSourceFile::FNTrasfNome(char *A) {
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

char *CSourceFile::AddExt(char *n, const char *x) {
  char *p;
  
  if(p=strchr(n,'.')) {
		_tcscpy(p+1,x);
		}
  else {
		_tcscat(n,".");
		_tcscat(n,x);
		}      
		
	return n;	
  }
  
int CSourceFile::get() {
	char ch;

rifo:
	if(Read(&ch,1) < 1)
		return EOF;
	if(ch=='\r')
		goto rifo;
	if(ch=='\n')
		lineno++;

	return ch;
	}

void CSourceFile::unget(char ch) {

	Seek(-1,CFile::current);
	if(ch=='\n') {
		lineno--;
		Seek(-1,CFile::current);		// riavvolgo anche CR...
		}
	}

void CSourceFile::SavePosition() {
	if(savedPositionIdx<10-1)
		savedPositionIdx++;
	else	// errore...
		;
	savedPosition[savedPositionIdx]=GetPosition();
	savedLineno=lineno;
	}
void CSourceFile::RestorePosition() {
	// non va... serve forse una specie di stack delle posizioni salvate, più d'una...
	if(savedPositionIdx>0)
		Seek(savedPosition[--savedPositionIdx],begin);
	else
		;		// errore...
	lineno=savedLineno;
	}
void CSourceFile::RestorePosition(uint32_t pos) {
	Seek(pos,begin);
	lineno=savedLineno;
	}

uint32_t CSourceFile::getLineFromPosition(long pos) {
	uint32_t oldpos=GetPosition();
	uint16_t line=1;
	char ch;
	
	if(pos==-1)
		pos=GetPosition();
	Seek(0,CFile::begin);
	do {
		if(Read(&ch,1) < 1)
			break;
		if(ch=='\n')
			line++;
		} while(GetPosition()<(uint32_t)pos);

	Seek(oldpos,CFile::begin);
	return line;
	}

int CSourceFile::getHex() {
	int ch;
	int n=0;

	while((ch=get()) != EOF) {
		if(isdigit(ch)) {
			n*=16;
			n+=ch-'0';
			}
		else {
			ch = toupper(ch);
			if(ch>='A' && ch<='F') {
				n*=16;
				n+=ch+10-'A';
				}
			else {
				unget(ch);
				break;
				}
			}
		}

	return n;
	}

int CSourceFile::getOct() {
  int t;
	int n=0;
	int ch;
  
	while((ch=get()) != EOF) {
    t=ch-48;
    if((t>=0) && (t<8)) {
      n=n*8+t;
      }
    else {
			unget(ch);
			break;
      }
    }
  return n;
  }
      
int CSourceFile::getInt() {
	int ch;
	int n=0;

	while((ch=get()) != EOF) {
		n*=10;
		if(isdigit(ch)) {
			n+=ch-'0';
			}
		else
			break;
		}

	return n;
	}

/*int CSourceFile::scanf(const TCHAR *s,...) {
	TCHAR myBuf[2048],myFmt[16];
	register int i,j,ch,k;
	int *n;
	double *d;
	CTime *myT;
	TCHAR *p,*p_myBuf;
  va_list vl;
	CString S;

	ReadString(myBuf);

	va_start(vl,s);
	p_myBuf=myBuf;
	myBuf[j]=0;
	i=0;
	while(ch=s[i++]) {
		if(ch == '%') {
			if(s[i] == '%') {
				i++;
				goto no_var;
				}
			myFmt[0]=ch;
			k=1;
			while((ch=s[i++]) && !isalpha(ch))			// salvo dettagli...
				myFmt[k++]=ch;
			myFmt[k++]=ch;			// ...copio effettivo format-type
			myFmt[k]=0;
			switch(myFmt[k-1]) {
				case 'd':
				case 'D':
					n=va_arg(vl,int*);
					goto subLeggi0;
					break;
				case 'p':
				case 'P':
					myBuf[j++]='0';
					myBuf[j++]='x';
				case 'u':
				case 'U':
				case 'x':
				case 'X':
				case 'o':		//ottale
				case 'O':
					n=va_arg(vl,int*);
subLeggi0:
					sscanf(myBuf1,myFmt,n);
subLeggi:
					_tcscpy(myBuf+j,myBuf1);
subLeggi2:
					j+=_tcslen(myBuf1);
					break;
				case 'f':
				case 'g':
					d=va_arg(vl,double*);
					sprintf(myBuf1,myFmt,d);
					goto subLeggi;
					break;
				case 's':
					p=va_arg(vl,TCHAR *);
					_tcsncpy(myBuf+j,p,2000-j);
					j+=min(_tcslen(p),2000-j);
					myBuf[j]=0;
					break;
				case 'c':
					n=va_arg(vl,char*);
					myBuf[j++]=n;
					break;
				case 't':
					myT=va_arg(vl,CTime*);
					S.Format(_T("%02u/%02u/%02u %02u:%02u:%02u"),
						myT.GetDay(),
						myT.GetMonth(),
						myT.GetYear(),
						myT.GetHour(),
						myT.GetMinute(),
						myT.GetSecond());
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
				case 'T':
					myT=va_arg(vl,CTime*);
					S=myT.Format(_T("%02u %b %04u %02u:%02u:%02u"));
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
				default:
					break;
				}
			}
		else {
			if(ch == '\n')
				myBuf[j++]=13;

no_var:
			myBuf[j++]=ch;
			}
		if(j>2000)
			break;
		}

	myBuf[j]=0;

	i=0;

	va_end(vl);
	return i;
	}*/



// ------------------------------------------------------------------------------------------------
COutputFile::COutputFile(LPCTSTR s) : CStdioFile(s,CFile::modeCreate | CFile::modeReadWrite /*CFile::modeWrite*/),
	totLines(0) { 

	 }
COutputFile::COutputFile(FILE *f) : CStdioFile(f),totLines(0) { 
	}

/*COutputFile::COutputFile() : totLines(0) {		// così non serve... vedere come fare, magari derivando da CFile
	mF=new CMemFile();
	}*/
/* non è base class COutputFile::COutputFile() : CMemFile(),totLines(0) {
	}
	*/

int COutputFile::get() {
	char ch;

rifo:
	if(Read(&ch,1) < 1)
		return EOF;
	if(ch=='\r')
		goto rifo;

	return ch;
	}

void COutputFile::put(char ch) {

	if(!totLines)			// :) vabbe' finezza
		totLines=1;
	
	Write(&ch,1);
	if(ch=='\n')
		totLines++;
	}

int COutputFile::printf(const TCHAR *s,...) {
	TCHAR myBuf[2048],myBuf1[2048],myFmt[16];
	register int i,j,ch,k;
	int n,pad;
	double d;
	CTime myT;
	TCHAR *p,*p_myBuf;
  va_list vl;
	CString S;
	char padch=' ';

	va_start(vl,s);
	p_myBuf=myBuf;
	j=0;
	myBuf[j]=0;
	i=0;
	while(ch=s[i++]) {
		pad=0;
		if(ch == '%') {
			if(s[i] == '%') {
				i++;
				goto no_var;
				}
			myFmt[0]=ch;
			k=1;
			while((ch=s[i++]) && !isalpha(ch))			// salvo dettagli...
				myFmt[k++]=ch;
			myFmt[k++]=ch;			// ...copio effettivo format-type
			myFmt[k]=0;
			if(myFmt[1]=='-') {
				if(isdigit(myFmt[2])) {
					pad=-atoi(&myFmt[2]);
					}
				// altrimenti...
				}
			else if(myFmt[1]=='+') {
				// segno ...
				}
			if(isdigit(myFmt[1])) {
				pad=atoi(&myFmt[1]);
				}
			switch(myFmt[k-1]) {
				case 'l':
				case 'L':
//					k++;
					// bah me ne fotto e proseguo!
					myFmt[k-1]=s[i++];
				case 'd':
				case 'D':
					n=va_arg(vl,int);
					goto subCopia0;
					break;
				case 'p':
				case 'P':
					myBuf[j++]='0';
					myBuf[j++]='x';
				case 'u':
				case 'U':
				case 'x':
				case 'X':
				case 'o':		//ottale
				case 'O':
					n=va_arg(vl,unsigned int);
subCopia0:
					sprintf(myBuf1,myFmt,n);
subCopia:
					if(pad) {
						pad-=_tcslen(myBuf1);
						if(pad<0)
							pad=0;
						while(pad--)
							myBuf[j++]=padch;		//put(' ');
						}
					_tcscpy(myBuf+j,myBuf1);
subCopia2:
					j+=_tcslen(myBuf1);
					break;
				case 'f':
				case 'g':
					d=va_arg(vl,double);
					sprintf(myBuf1,myFmt,d);
					goto subCopia;
					break;
				case 's':
					p=va_arg(vl,TCHAR *);
					if(pad) {
						pad-=_tcslen(p);
						if(pad<0)
							pad=0;
						while(pad--)
							myBuf[j++]=padch;		//put(' ');
						}
					_tcsncpy(myBuf+j,p,2000-j);
					j+=min(_tcslen(p),2000-j);
					myBuf[j]=0;
					break;
				case 'c':
					n=va_arg(vl,char);
					myBuf[j++]=n;
					break;
				case 't':
					myT=va_arg(vl,CTime);
					S.Format(_T("%02u/%02u/%02u %02u:%02u:%02u"),
						myT.GetDay(),
						myT.GetMonth(),
						myT.GetYear(),
						myT.GetHour(),
						myT.GetMinute(),
						myT.GetSecond());
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
				case 'T':
					myT=va_arg(vl,CTime);
					S=myT.Format(_T("%02u %b %04u %02u:%02u:%02u"));
					_tcscpy(myBuf+j,(LPCTSTR)S);
					j+=S.GetLength();
					break;
				default:
					break;
				}
			}
		else {

no_var:
			myBuf[j++]=ch;
			}
		if(j>2000)
			break;
		}

	myBuf[j]=0;

	i=0;
	write(myBuf);

	va_end(vl);
	return i;
	}

void COutputFile::print(const TCHAR *s) {

	write(s);
	}

void COutputFile::println(const TCHAR *fmt,...) {
  va_list argptr;

//  va_start(argptr, fmt);
//  print(fmt, argptr);		// non va... boh
//  va_end(argptr);
	write(fmt);
	putcr();
	totLines++;
	}

void COutputFile::write(const TCHAR *s) {
	
	while(*s) {
		put(*s++);
		}
	}

