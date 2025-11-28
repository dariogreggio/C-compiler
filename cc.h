//#define I8086 0
//#define Z80   1
//#define ARCHI 0
//#define I8051 0
//#define MICROCHIP 1
// MESSE IN PROGETTO!!

#include <stdint.h>
       
#define ANSI  TRUE
#define ACORN FALSE
#define GD    FALSE   // REM CONVENZIONI SUI NOMIFILE

//#define TRUE 1
//#define FALSE 0

#if ARCHI
  #define INT_SIZE 4
	#define STACK_ITEM_SIZE INT_SIZE
#elif Z80 
  #define INT_SIZE 2
	#define STACK_ITEM_SIZE INT_SIZE
#elif I8086 
  #define INT_SIZE 2
	#define STACK_ITEM_SIZE INT_SIZE
#elif I8051 
  #define INT_SIZE 2
	#define STACK_ITEM_SIZE INT_SIZE
#elif MICROCHIP
  #define INT_SIZE 2
	#define STACK_ITEM_SIZE 1			//C18 lo fa da 2... io provo con 1, mi sembra + performante
	#define ROM_PTR_SIZE 3			// 24 bit...
#elif MC68000 
  #define INT_SIZE 4
	#define STACK_ITEM_SIZE 2
#endif
#define PTR_SIZE INT_SIZE

#if ARCHI
#define __VER__ MAKEWORD(0,1)
#elif Z80
#define __VER__ MAKEWORD(7,2)
#elif I8086
#define __VER__ MAKEWORD(3,2)
#elif I8051
#define __VER__ MAKEWORD(1,0)
#elif MICROCHIP
#define __VER__ MAKEWORD(2,1)
#elif MC68000
#define __VER__ MAKEWORD(10,1)
#endif

enum {
	PWM_COMMWRITE =  WM_USER+1,
	WM_UPDATE_PANE,
	};


#define MAX_NAME_LEN 31
#define MAX_DIM 4
#define MAX_TIPI 50

struct ERRORE {
  uint16_t t;
	uint8_t l;
  const char *s;
  };

union SUB_OP_DEF {
  char label[MAX_NAME_LEN+1];
  int n;
//  struct VARS *v;
  };
    
typedef uint32_t O_TYPE;
typedef uint16_t O_SIZE;
typedef uint32_t O_DIM[MAX_DIM];		// max 5 dim!

enum OPDEF_MODE {
	OPDEF_MODE_NULLA=0,
	OPDEF_MODE_REGISTRO=3,
	OPDEF_MODE_REGISTRO16=3,		// FINIRE (per i8086/68000)
	OPDEF_MODE_REGISTRO32=3,		// FINIRE (per i8086/68000)
	OPDEF_MODE_REGISTRO8=3,			// ev.
	OPDEF_MODE_REGISTRO_LOW8=1,
	OPDEF_MODE_REGISTRO_HIGH8=2,
	OPDEF_MODE_IMMEDIATO=4,
	OPDEF_MODE_IMMEDIATO8=4,
	OPDEF_MODE_IMMEDIATO16=5,
	OPDEF_MODE_IMMEDIATO32=6,
	OPDEF_MODE_FRAMEPOINTER=8,
	OPDEF_MODE_VARIABILE=9,
	OPDEF_MODE_COSTANTE=10,
	OPDEF_MODE_STACKPOINTER=11,
	OPDEF_MODE_CONDIZIONALE=16,
	OPDEF_MODE_INDIRETTO=0x80,
	OPDEF_MODE_REGISTRO_INDIRETTO=(OPDEF_MODE_INDIRETTO | OPDEF_MODE_REGISTRO),
	OPDEF_MODE_IMMEDIATO_INDIRETTO=0x86,
	OPDEF_MODE_FRAMEPOINTER_INDIRETTO=(OPDEF_MODE_INDIRETTO | OPDEF_MODE_FRAMEPOINTER),
	OPDEF_MODE_VARIABILE_INDIRETTO=(OPDEF_MODE_INDIRETTO | OPDEF_MODE_VARIABILE),
	OPDEF_MODE_STACKPOINTER_INDIRETTO=(OPDEF_MODE_INDIRETTO | OPDEF_MODE_STACKPOINTER),
	};

struct OP_DEF {
  enum OPDEF_MODE mode;     // 3 registro in n (1 se solo parte bassa, 2 se solo alta), 4=imm. in n (4 se 8 bit, 5 16 bit, 6 32 bit), 8=FpS (ofs in ofs), 9=Variabile in v e ofs, 10=label in label e ofs, 11 SP, 16=condiz in n
                        // bit 7=indiretto
  union SUB_OP_DEF s;
  int16_t ofs;			// anche 32?
  };
  
enum LINE_TYPE {
	LINE_TYPE_NULLA=0,
	LINE_TYPE_COMMENTO=0,
	LINE_TYPE_LABEL,
	LINE_TYPE_DATA_DEF,
	LINE_TYPE_LABEL_CON_ISTRUZIONE,
	LINE_TYPE_JUMP=8,
	LINE_TYPE_JUMPC,
	LINE_TYPE_CALL,
	LINE_TYPE_ISTRUZIONE=16,
	LINE_TYPE_READ=16,			// usare, per differenziare...
	LINE_TYPE_WRITE=16,
	LINE_TYPE_PUSH=16,
	LINE_TYPE_POP=16,
	};

struct LINE {
  struct LINE *next;
  struct LINE *prev;
  enum LINE_TYPE type;             // 0 commento, 1 label, 2 data def, 3 label con istr., 8 jump, 9 call, 16 istr.
  char opcode[32];		// usato anche in blocchi _asm per tutta la riga... attenzione
  struct OP_DEF s1;
  struct OP_DEF s2;
  char rem[128];
  };
  
struct LINE_DEF {
	uint16_t used;
	struct VARS *vars;
  struct LINE_DEF *next;
  struct LINE_DEF *prev;
  char name[MAX_NAME_LEN+1];
  char *text;
  };

enum {
	OPTIMIZE_JUMP=1,
	OPTIMIZE_SUBEXPR=2,
	OPTIMIZE_INLINECALLS=4,
	OPTIMIZE_CONST=16,
	OPTIMIZE_SIZE=0x100,
	OPTIMIZE_SPEED=0x200,
	};
enum {
	MEMORY_MODEL_SMALL=0,
	MEMORY_MODEL_COMPACT=0,		// finire
	MEMORY_MODEL_MEDIUM=0,		// finire
	MEMORY_MODEL_LARGE=1,
	MEMORY_MODEL_ABSOLUTE=0,
	MEMORY_MODEL_RELATIVE=0x80,
	};
enum {
	TIPO_SPECIALE=1,		// output speciale, tipo Easy68K
	TIPO_EMBEDDED=2,		// copia Initialized in Const, affinché possano essere copiati in RAM alla partenza
	};

union STR_LONG {
  char s[128];
  long l;
	uint64_t l64;		// VA GESTITO IN MOLTI POSTI! e serve ulltoa(
  };

// VARTYPE% BITS:
// 0-3 = PTR,
// 7=corpo FUNZ o VAR, 8=FUNZ., 9=FUNZ usata,
// 10=ARRAY, 11=STRUCT, 12=UNION, 13=FLOAT,
// 16=ENUM,
// 31=UNSIGNED 
// 30=ROM (su Microchip)
// VARSIZE% DIMENSIONI (PER LE FUNZIONI E' LA DIM. DI QUELLO CHE RITorNANO)
enum VAR_CLASSES {			// v. anche class Ccc
// 0 extern, 1 global, 2 static, 3 auto, 4 register; 
	CLASSE_EXTERN=0,
	CLASSE_GLOBAL=1,
	CLASSE_STATIC=2,
	CLASSE_AUTO=3,
	CLASSE_REGISTER=4,

	CLASSE_INTERRUPT=0x10,			// nello stesso ordine, shiftati di 4 rispetto ai MODIFIERS
	CLASSE_PASCAL=0x20,
	// CLASSE_C
	CLASSE_INLINE=0x80,
	CLASSE_FASTCALL=0x100,
	};
enum VAR_MODIFIERS {		// v. anche class Ccc
// 1 interrupt, 2 pascal, 4 C
	FUNC_MODIF_INTERRUPT=1,
	FUNC_MODIF_PASCAL=2,
	FUNC_MODIF_C=4,
	FUNC_MODIF_INLINE=8,
	FUNC_MODIF_FASTCALL=16
	};
enum VAR_ATTRIBUTES {		// __attribute__ 
	FUNC_ATTRIB_NORETURN=1,
	FUNC_ATTRIB_NAKED=2,
	FUNC_ATTRIB_WEAK=4,			// in teoria anche variabili, ma raro
	VAR_ATTRIB_FIXED=0x100,
	VAR_ATTRIB_UNUSED=0x200,
	VAR_ATTRIB_PACKED=0x400,
	// poi volendo ci sono ripetuti i vari interrupt, const e altro
	};
enum VAR_TYPES {		// v. anche class Ccc
// 0= label, 7=public defined, 8=FUNZ., 9=public, 10=extern, 11=STRUCT
	VARTYPE_NOTYPE=-1,
	VARTYPE_PLAIN_INT=0,
	VARTYPE_POINTER=1,			//0..15
	VARTYPE_2POINTER=2,			//**
	VARTYPE_IS_POINTER=0xf,			//0..15
	VARTYPE_NOT_A_POINTER=(uint32_t)(~(VARTYPE_IS_POINTER)),			// mask

	VARTYPE_FUNC_POINTER=0x40,
	VARTYPE_FUNC_BODY=0x80,
	VARTYPE_FUNC=0x100,
	VARTYPE_FUNC_USED=0x200,

	VARTYPE_ARRAY=0x400,		// questo IMPLICA 1 ossia POINTER
	VARTYPE_STRUCT=0x800,
	VARTYPE_UNION=0x1000,
	VARTYPE_FLOAT=0x2000,		// per double metto size=8 opp. 4
	VARTYPE_BITFIELD=0x4000,
	VARTYPE_ENUM=0x10000,

	VARTYPE_VOLATILE=0x8000000L,

	VARTYPE_FAR=0x10000000L,
	VARTYPE_SIGNED=0x00000000L,
	VARTYPE_UNSIGNED=0x80000000L,
#if MICROCHIP
	VARTYPE_ROM=0x20000000,
// in alcuni sarebbero la stessa cosa	VARTYPE_CONST=0x40000000
#else
#endif
	VARTYPE_CONST=0x40000000
	};
struct VARS {
  char label[MAX_NAME_LEN+1];
  char name[MAX_NAME_LEN+1];
  enum VAR_CLASSES classe;
// 0 extern, 1 global, 2 static, 3 auto, 4 register; 
  uint8_t modif;
// 1 interrupt, 2 pascal, 4 C ecc
  O_TYPE type;
  O_SIZE size;
  uint8_t block;
  struct VARS *func;
  char *parm;
  struct TAGS *tag;         // se <>0, la var. è un membro della struct tag
  struct TAGS *hasTag;      // questo indica il tag di questa struct
  O_DIM dim;							// dim TOTALE dell'array o aggr
  uint8_t attrib;
  struct VARS *next;
  };

struct VARS2 {
  uint32_t value;
  O_TYPE type;
  O_SIZE size;
  struct VARS *func;
  uint8_t block;
  struct VARS2 *next;
  };

struct CONS {
  char label[MAX_NAME_LEN+1];
  char name[128];
  struct CONS *next;
  };  
//#pragma message USARE vars2 ANCHE PER STRUCT *********************
struct TAGS {
  char label[MAX_NAME_LEN+1];
  struct VARS *member;		
  struct TAGS *next;
  };  
   
struct ENUMS {
  char name[MAX_NAME_LEN+1];
  struct VARS2 var;
  struct ENUMS *next;
  };  
   
struct OPERAND {
//	enum VQ_ELEM V;
	int8_t Q;
	O_SIZE size;
// verificare...	enum ARITM_ELEM T;
	O_TYPE type;
	struct VARS *var;
	union STR_LONG *cost;
	struct TAGS *tag; 
	uint8_t tipo_D0;			// flag usato per le operazioni indirette su puntatori
	O_DIM dim;
	uint8_t flag;					// usato ad es. per calcolare pos. array
	};
  
struct OPERANDO {
  char *s;
  /*enum OPERANDI*/ int8_t p;
  };

struct TIPI {
  char s[MAX_NAME_LEN+1];
  O_SIZE size;
  O_TYPE type;
  struct TAGS *tag;
  O_DIM dim;
  };

struct BLOCK_PTR {
  struct LINE *TX;   // CONTIENE I PUNTATORI PENDENTI LastOut AL FILE OUT
  char T[32];    // CONTIENE I NOMI DEI FINE-BLOCCHI, # SE DO, & SE SWITCH,% SE if
  char C[32];   // CONTIENE LE LABEL PER continue
  char B[32];   // CONTIENE LE LABEL PER break
	char **gotos;		// usare, per memorizzare e controllare le label definite!
  char *parm;
	int8_t flag;		// usato per segnalare cose, tipo "default" già uscito in switch(
  };
   

#define MAKEPTRREG(s) (*(uint8_t*)s)
#define MAKEPTROFS(s) (*(uint32_t*)s)

class COpenCDoc;
class COpenCView2;
class CLogFile;

/////////////////////////////////////////////////////////////////////////////

#if ARCHI || Z80 || I8051
#elif I8086
	extern int CPU86;               // 0 8086, 1 80186, 2 80286, 3 80386, 4 80486, 5 Pentium, 6 Pentium2, 7 Pentium3
#elif MICROCHIP
	extern int CPUPIC,CPUEXTENDEDMODE,stackLarge;              // 0=12; 1=16; 2=18; 3=24; 4=30/33; 5=32
#elif MC68000
	extern int CPU68;
#endif


/*già in openC.h
	class CStringEx : public CString {
	public:
		enum Options {
			NO_OPTIONS=0,
			COMMA_DELIMIT=1,
			};
		static const int BASE64_MAXLINE;
		static const char *EOL;
		static const char decimalChar,thousandChar;
		static const char CRchar,LFchar,TABchar;
	public:
		static const char m_base64tab[];
	public:
		CString Tokenize(CString delimiter, int& first);
		static CStringEx CommaDelimitNumber(const char *);
		static CStringEx CommaDelimitNumber(CString);
		static CStringEx CommaDelimitNumber(DWORD);
		CStringEx SubStr(int begin, int len) const;					// substring from s[begin] to s[begin+len]
		BYTE Asc(int);
		int Val(int base=10);
		double Val();
		struct in_addr IPVal();
		void Repeat(int);
		void Repeat(const char *,int);
		void Repeat(char,int);
		void AddCR() { CStringEx::operator+=(LFchar); }
		void RemoveLeft(int n) { CStringEx::operator=(Mid(n)); }
		void RemoveRight(int n) { CStringEx::operator=(Mid(1,GetLength()-n)); }		// era un'idea per fare LEFT$ di tot char, ma Trimright c'è già anche se diversa...
		void Trim() { CString::TrimLeft(); CString::TrimRight(); }	
		static bool IsAlpha(char);
		bool IsAlpha(int);
		static bool IsAlnum(char);
		bool IsAlnum(int);
		static bool IsDigit(char);
		bool IsDigit(int);
		static bool IsPrint(char);
		bool IsPrint(int);
		int FindNoCase(CString substr,int start=0);
		int ReverseFindNoCase(CString substr);
		WORD GetAsciiLength();
		CStringEx Encode64();
		int Decode64();
		CStringEx FormatTime(int m=0,CTime mT=0);
		CStringEx FormatSize(DWORD);
		void Print();
		void Debug();
		CStringEx() : CString() {};		// servono tutti i costruttori "perché non ne ha di virtual, la CString" !
		// https://www.codeguru.com/cpp/cpp/string/ext/article.php/c2793/CString-Extension.htm
		// https://www.codeproject.com/Articles/2396/Simple-CString-Extension
		CStringEx(const CString& stringSrc) : CString(stringSrc) {};
		// bah, eppure non sembra... 2021...
//		CStringEx(const CStringEx& stringSrc) : CString(stringSrc) {};
		CStringEx(TCHAR ch, int nRepeat = 1) : CString(ch, nRepeat) {};
//		CStringEx(LPCTSTR lpch, int nLength) : CString(lpch, nLength) {};
//		CStringEx(const unsigned char *psz) : CString(psz) {};
		CStringEx(LPCWSTR lpsz) : CString(lpsz) {};
		CStringEx(LPCSTR lpsz) : CString(lpsz) {};
//		CStringEx(const char c) {char s[2]={'\0', '\0'}; s[0]=c; CString::operator=(s);}
		CStringEx(int i, const char* format="%d", DWORD options=NO_OPTIONS);
		CStringEx(double d, const char* format="%02lf", DWORD options=NO_OPTIONS);
		virtual ~CStringEx() {};
private:
	CString InsertSeparator(DWORD);
	};
*/


extern const char *movString,*storString,*jmpString,*jmpCondString,*callString,*returnString,*incString,*decString,*pushString,*popString;

class CSourceFile : public CFile {
public:
	CSourceFile(LPCTSTR);
  static char *FNTrasfNome(char *);
	static char *AddExt(char *n, const char *x);
	int get();
	char *FNLO(char *);
	char *FNLA(char *);
	bool Eof() { return GetPosition() >= GetLength(); }
	void unget(char);
// fare?? v. hex ecc	int scanf(const TCHAR *s,...);
	int getInt();
	int getHex();
	int getOct();
	void SavePosition();
	void RestorePosition();
	void RestorePosition(uint32_t);
	uint32_t getLineFromPosition(long pos=-1);

public:
	uint32_t savedPosition[10];
	uint16_t lineno;			// 65535 :)

private:
	uint16_t savedLineno;
	uint8_t savedPositionIdx;

	};

class COutputFile : public CStdioFile {
public:
	COutputFile(LPCTSTR s);
	COutputFile(FILE *f);
//	COutputFile();
	int printf(const TCHAR *s,...);
	void print(const TCHAR *s);		// usata per NON espandere i %d ecc, tipo stringhe literal
//	void println(const TCHAR *s);
	void println(const TCHAR *s,...);
	void put(char ch);
	void putcr() { put('\n'); }
	void write(const TCHAR *);
	int get();
	bool eof() { return GetPosition() >= GetLength(); }
	int getTotalLines() { return totLines; }

private:
	uint32_t totLines;
//	CMemFile *mF;

	};

class CCPreProcessor;

class Ccc {
public:
/*	enum VAR_CLASSES {
		CLASSE_EXTERN=0,
		CLASSE_GLOBAL=1,
		CLASSE_STATIC=2,
		CLASSE_AUTO=3,
		CLASSE_REGISTER=4,
		CLASSE_INTERRUPT=0x10,
		CLASSE_PASCAL=0x20,
		CLASSE_INLINE=0x40
		};*/
	enum {
		FUNC_MODIF_INTERRUPT=1,
		FUNC_MODIF_PASCAL=2,
		FUNC_MODIF_C=4,
		FUNC_MODIF_INLINE=8,
		FUNC_MODIF_FASTCALL=16
		};
/*	enum {
		VARTYPE_PLAIN_INT=0,
		VARTYPE_POINTER=1,			//0..15
		VARTYPE_2POINTER=2,			//**
		VARTYPE_IS_POINTER=0xf,			//0..15
		VARTYPE_NOT_A_POINTER=0xfffffff0L,			// mask
		VARTYPE_FUNC_BODY=0x80,
		VARTYPE_FUNC=0x100,
		VARTYPE_FUNC_USED=0x200,
		VARTYPE_ARRAY=0x400,
		VARTYPE_STRUCT=0x800,
		VARTYPE_UNION=0x1000,
		VARTYPE_FLOAT=0x2000,
		VARTYPE_BITFIELD=0x4000,
		VARTYPE_ENUM=0x10000,
		VARTYPE_UNSIGNED=0x80000000L
#if MICROCHIP
		,VARTYPE_ROM=0x40000000
#endif
		};*/
	enum VALUES {
		VALUE_IS_0=0,					// forse se espressione a dx di =
		VALUE_IS_EXPR=1,			// boh... non ho ancora capito bene! forse proprio "altro" ossia espressione! tipo op. ? :
		VALUE_IS_EXPR_FUNC=2,
		VALUE_IS_D0=3,
		VALUE_IS_VARIABILE=4,
		VALUE_IS_COSTANTE=8,
		VALUE_IS_COSTANTEPLUS=8 + 1,

//                V->Q |= 0x20;            // segnala ! condizionale  VERIFICARE! era anche usato per indicare condizione di tipo unsigned...
//ma è usato anche come "condizione"...		VALUE_CONDITION_UNSIGNED=0x20,				// potrebbe non servire più, 2025 con le nuove condizioni per unsigned, ma è ancora usata in giro!
		VALUE_CONDITION_UNSIGNED=0x10,		// non più usata 2025 cmq (creati valori espliciti per unsigned
		VALUE_IS_CONDITION=0x20,					// è una condizione < > == ecc
		VALUE_HAS_CONDITION=0x40,
		VALUE_IS_CONDITION_VALUE=0x80,		// significa che usiamo un'expr come vero o falso
		VALUE_CONDITION_UP=0x0100,			// usata come mask... non so bene perché
		VALUE_CONDITION_MASK=0x00ff			// e viceversa
		};
	enum OPERANDO_CONDIZIONALE {
		CONDIZ_MINORE=VALUE_IS_CONDITION | 0,
		CONDIZ_MAGGIORE_UGUALE=VALUE_IS_CONDITION | 1,
		CONDIZ_MINORE_UGUALE=VALUE_IS_CONDITION | 2,
		CONDIZ_MAGGIORE=VALUE_IS_CONDITION | 3,
		CONDIZ_UGUALE=VALUE_IS_CONDITION | 4,
		CONDIZ_DIVERSO=VALUE_IS_CONDITION | 5,
		CONDIZ_MINORE_UNSIGNED=VALUE_IS_CONDITION | 6,
		CONDIZ_MAGGIORE_UGUALE_UNSIGNED=VALUE_IS_CONDITION | 7,
		CONDIZ_MINORE_UGUALE_UNSIGNED=VALUE_IS_CONDITION | 8,
		CONDIZ_MAGGIORE_UNSIGNED=VALUE_IS_CONDITION | 9,
		};
	enum OPERAND_MODES {
		MODE_IS_OTHER=-1,
		MODE_IS_VARIABLE=0,
		MODE_IS_CONSTANT1=1,
		MODE_IS_CONSTANT2=2,
		};
	enum ARITM_OP {
		ARITM_IS_EOL=0,
		ARITM_IS_COSTANTE=1,
		ARITM_IS_VARIABILE=2,
		ARITM_IS_OPERANDO=3,
		ARITM_IS_UNKNOWN=-1,
		};
	friend class CCPreProcessor;

protected:
// Attributes
	CWnd *myOutput;
	CLogFile *myLog;
	CCPreProcessor *m_CPre;

public:
	Ccc(const CWnd *p);
	~Ccc();


protected:
	int8_t bExit;
	char buffer[128];
  COutputFile *FPre;
	CSourceFile *FIn;
	COutputFile *FO1,*FO2,*FO3,*FO4,*FO5;
	COutputFile *FObj,*FCod;
	COutputFile *FLst,*FErr;
	int Warning;
  struct LINE *RootOut,*LastOut,*StaticOut,*BSSOut;
	struct VARS *Var;
	struct VARS *LVars;   // root, used, last
	struct VARS *CurrFunc;
	struct CONS *Con;
	struct CONS *LCons;
  struct ENUMS *LEnums;
	struct ENUMS *Enums;
	struct TAGS *StrTag;
	struct TAGS *LTag;
	//struct VARS *TAG;           
	//int SX;
	uint8_t InBlock; 			// LIVELLO DI BLOCCHI
	uint32_t TempSize;
	uint16_t MaxTypes;
	int8_t UseLMul,UseIRQ,UseTemp,UseFloat;
	char *RootIn;
	uint8_t Brack;				// usati da FNRev
	int8_t isRValue,isPtrUsed,inCast;	// tutti questi potrebbero andare in  struct OPERAND
	uint32_t TempProg;				//
	struct LINE *GlblOut;	//

	char NFS[256],OUS[256];

	int __STDC__;
	int LABEL;
	char __file__[256],__name__[256];
	int __line__;
	char __date__[11];
	char __time__[11];
	char TextSegm[64],DataSegm[64]; 
	int Declaring,FuncCalled,SaveFP,ASM,AutoOff;
	uint8_t debug;
	uint8_t PreProcOnly;          // PREPROCESSA SOLO SU stdout  -E
	uint8_t CheckStack;            // INSERISCE LO STACK PROBE    -Gs
	uint8_t CheckPointers;        // INSERISCE check dei puntatori nulli
	uint8_t OutSource;             // INSERISCE LE RIGHE C NELL'OUTPUT  -Fc
	uint8_t OutAsm;             // crea un file asm senza righe C (non usato) -Fa
	uint8_t OutList;               // CREA FILE LISTING          -Fl
	uint8_t TipoOut;						// varia tipo ASM file generato (v.68000->Easy68K
	uint8_t PascalCall;           // PARAMETRI IN CONV. Pascal   -Gc
	uint8_t InLineCall;           // INLINE STRINGHE E MATH     -Oi
	uint8_t OptimizeExpr;         // RICORDA le SUB-Expr        -Og
	uint8_t SynCheckOnly;         // SOLO SYNTAX CHECKING        -Zs
	uint8_t UnsignedChar;         // CHAR E' UNSIGNED PER DEFAULT  -J
	uint8_t MemoryModel;						// small s o large l 
	uint8_t NoMacro;              // DISABILITA MACRO PREDEF.    -u
	uint8_t MultipleString;       // stringhe costanti uguali ripetute o compresse
	uint8_t StorageDefault;				// storage class di default (finire)
	uint8_t StructPacking;				// packing delle struct
	uint16_t Optimize;
	static struct TIPI Types[MAX_TIPI];
	static struct OPERANDO Op[];
	uint16_t numErrors,numWarnings;

// Operations
public:
	int CompilaC(int,char **);

// Compiler
	char *AddExt(char *, char *);
	int PROCBlock();
	int PROCIsDecl();
	int PROCDclVar(enum VAR_CLASSES, uint8_t, O_TYPE type, O_SIZE size, struct TAGS *, O_DIM dim, uint32_t attrib, bool isparm);
	int subAsm(char *);
	int FNIsStmt();
	char *FNGetLabel(char *,uint8_t);
	int PROCGenCondBranch(const char *, int T, int8_t *VQ, O_SIZE );
	int FNGetCondString(uint8_t , uint8_t);
	int PROCAssignCond(int8_t *VQ, O_TYPE *type, O_SIZE *size, char *);
	int PROCReturn();
	long FNGetConst(char *,bool);
	int PROCLoops(const char *, const char *, const char *, struct LINE *);
	int FNRegFree();
	struct VARS *FNCercaVar(const char *, bool);
	struct VARS *PROCAllocVar(const char *name, O_TYPE type, enum VAR_CLASSES, uint8_t modif, O_SIZE size, struct TAGS *, O_DIM dim);
  struct ENUMS *FNCercaEnum(const char *,const char *,bool);
	int PROCCast(O_TYPE, O_SIZE, O_TYPE, O_SIZE, int8_t);
#if MICROCHIP
	int PROCReadD0(struct VARS *, O_TYPE type, O_SIZE size, uint16_t cond, int ofs, bool asPtr, uint8_t lh=0);
#else
	int PROCReadD0(struct VARS *, O_TYPE type, O_SIZE size, uint16_t cond, int ofs, bool asPtr);
#endif
	int PROCStoreD0(struct VARS *, int8_t VQ, struct VARS *, union STR_LONG *, bool isPtr);
	int PROCGetAdd(int8_t VQ, struct VARS *, int ofs, bool asPtr);
	int PROCUsaFun(struct VARS *,bool tosave1,bool tosave2);
	struct CONS *FNAllocCost(const char *, uint8_t, O_TYPE type=0);
	struct ENUMS *FNAllocEnum(const char *tag, const char *name, uint32_t value, O_SIZE Size);
	int PROCInit();
	int subOfsD0(struct VARS *, int, int, int);
	
	void subEvEx(uint8_t, int16_t *cond, char *, struct OPERAND *);
	uint16_t FNEvalExpr(uint8_t, char *);
 	int FNEvalECast(char *, O_TYPE *type, O_SIZE *size);
	int FNEvalCond(char *, const char *, uint16_t cond);

#if Z80 || MICROCHIP
	void OpA(char *, struct OP_DEF *, int);
	void OpA(char *, int, struct OP_DEF *);
	void OpA(char *, struct OP_DEF *, struct OP_DEF *);
	void IncOp(struct OP_DEF *);
	void DecOp(struct OP_DEF *);
	void Op2A(char *, struct OP_DEF *, int, uint8_t);
	void Op2A(char *, int, struct OP_DEF *, uint8_t);
	void Op2A(char *, struct OP_DEF *, struct OP_DEF *);
#endif  

#if ARCHI
	void subSpezReg(uint8_t, struct OP_DEF *);
#elif Z80
	void subSpezReg(uint8_t, struct OP_DEF *);
#elif I8086  
	void subSpezReg(uint8_t, struct OP_DEF *);
#elif MC68000
	void subSpezReg(uint8_t, struct OP_DEF *);
#elif MICROCHIP
	void subSpezReg(uint8_t, struct OP_DEF *);
#endif

  int8_t FNRev(int8_t Pty,int16_t *cond,char *,struct OPERAND *);
  char *ConRecEval(char *, uint8_t pty, long *);
  long EVAL(char *);

	int subShift(uint8_t, int mode, int8_t VQ, struct VARS *, O_TYPE type, O_SIZE size, O_TYPE, union STR_LONG *, 
		union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *,bool bAutoAssign);
	int subAdd(bool, int mode, int8_t VQ, struct VARS *, O_TYPE * type1, O_SIZE * size1, int8_t RQ, O_TYPE type2, O_SIZE size2,
		union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *,bool bAutoAssign);
	int subMul(char, int mode, int8_t VQ, struct VARS *, O_TYPE type1, O_SIZE size1, int8_t RQ, O_TYPE type2, O_SIZE size2, 
		union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *,bool bAutoAssign);
	uint8_t FNIs1Bit(uint32_t);
	uint8_t FNIsPower2(uint32_t);
	O_SIZE getPtrSize(O_TYPE t);
	int subAOX(char, int16_t *cond, int mode, int8_t VQ, struct VARS *, O_TYPE type1, O_SIZE size1, int8_t RQ, 
		O_TYPE type2, O_SIZE size2, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *,
		struct OP_DEF *, struct OP_DEF *,bool bAutoAssign);
	enum OPERANDO_CONDIZIONALE subCMP(const char *, int code, int mode, int8_t VQ, struct VARS *, O_TYPE type1, O_SIZE size1, 
		int RQ, O_TYPE type2, O_SIZE size2, 
		union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
	int subInc(bool, int16_t *cond, uint8_t T, int8_t VQ, struct VARS *, uint8_t qty, O_TYPE type, O_SIZE size, struct OP_DEF *, 
		struct OP_DEF *,uint8_t isPtr);

	void subObj(COutputFile *,struct OP_DEF *);
	int PROCObj(COutputFile *);
  int PROCError(int, const char *s=NULL);
  int PROCWarn(int, const char *s=NULL);
  int PROCV(const char *);
	int PROCT();
	int PROCD();
	int PROCVarList(COutputFile *, struct VARS *);

	char *FNLO(char *);
	char *FNLA(char *);
	int FNGetEscape();
	int FNGetOct(const char *);
	bool PROCCheck(const char *);
	bool PROCCheck(char);
	long FNGetLine(long,char *);
	unsigned int xtoi(const char *);
  BYTE xtob(char );
  unsigned int btoi(const char *);
	char *itox(char *,unsigned int);
	char *itob(char *,unsigned int);
	int lltoa(uint64_t num, char *str, /*int len, */uint8_t base);

	int OPComp(struct OP_DEF *, struct OP_DEF *);
	struct LINE *GetNextNoRem(struct LINE *);
	int Ottimizza(struct LINE *);
	void CHECKPOINTER();

  struct LINE *PROCInserLista(struct LINE *, struct LINE *, struct LINE *);
  struct LINE *PROCDelLista(struct LINE *, struct LINE *, struct LINE *);
	void PROCDelLastLine(struct LINE *);
	void swap(struct LINE * *, struct LINE * *);
  int PROCOut(enum LINE_TYPE, const char *, struct OP_DEF *, struct OP_DEF *, const char *R=NULL);
  int PROCOut1(COutputFile *,const char *, const char *, const char *s3=NULL, const char *s4=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, enum OPDEF_MODE, union SUB_OP_DEF *, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int, enum OPDEF_MODE, union SUB_OP_DEF *, int);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, enum OPDEF_MODE, int);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int, enum OPDEF_MODE, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int s1=0);
  void PROCOper(enum LINE_TYPE, const char *, struct OP_DEF *, struct OP_DEF *);
  void PROCOper(enum LINE_TYPE, const char *, struct OP_DEF *);
  int PROCOutLab(const char *,const char *s1=NULL,const char *s2=NULL);

	enum ARITM_OP FNGetAritElem(int8_t *OP, char *, struct OPERAND *, int8_t Co);
	int subGetType(O_TYPE *type, O_SIZE *size, O_DIM dim, long textpointer);
	int PROCGetType(O_TYPE *type, O_SIZE *size, struct TAGS **, O_DIM dim, uint32_t *attrib,long textpointer);
	long FNIsType(char *);
	struct VARS *FNGetAggr(struct TAGS *, const char *, bool, int *);
	struct TAGS *subAllocTag(const char *);
	struct TAGS *FNAllocAggr();
	int StoreVar(struct VARS *Vvar,int8_t VQ, struct VARS *RVar, union STR_LONG *, bool isPtr);
	#if MICROCHIP
	int ReadVar(struct VARS *,O_TYPE type,O_SIZE size,uint8_t/*bool*/ isCond,bool asPtr,uint8_t lh);
	#else
	int ReadVar(struct VARS *,O_TYPE type,O_SIZE size,uint8_t/*bool*/ isCond,bool asPtr);
	#endif

	#if ARCHI
	int FNIsLshift(long);
	#elif Z80 || I8086 || MC68000 || MICROCHIP		// per i PIC32 forse serve in effetti
	#endif

#if MICROCHIP
	int PROCUseCost(int8_t Q, O_TYPE type, O_SIZE size, union STR_LONG *,bool asPtr,uint8_t lh);
#else
	int PROCUseCost(int8_t Q, O_TYPE type, O_SIZE size, union STR_LONG *,bool asPtr);
#endif
	int FNIsOp(const char *, int);
	int FNIsClass(const char *);
	O_SIZE FNGetSize(uint32_t);
	O_SIZE FNGetMemSize(O_TYPE type, O_SIZE size, O_DIM dim, uint8_t m);
	O_SIZE FNGetMemSize(struct VARS *, uint8_t);
	O_SIZE FNGetArraySize(struct VARS *);

	static char *OpCond[16];
	static char *StrOp[20];
	struct BLOCK_PTR OldTX[20];

  class REGISTRI *Regs;
          
	int8_t Reg;

// Implementation
	};



/////////////////////////////////////////////////////////////////////////////

class REGISTRI {
private:
#if ARCHI
	const char DT[16][16];               // Nomi dei registri
#elif Z80 || I8086 || MC68000 || I8051 || MICROCHIP
	const char *DT[16];                  // Nomi dei registri
	long VType[16];
	uint8_t VSize[16];
	struct VARS *VVar[16];
#endif  
	int8_t ToDec;                      // quanto decrementare (Inc, Dec)
	int8_t ToGet;											// quanto pop-are (Save, Get)
public:
	int8_t D,P;					// registro per dato, registro per indirizzo
	uint8_t MaxD,UserBase,MaxUser;     // in ordine crescente
	const char *SpS;
#if I8086 || MC68000 
	const char *AbsS;
#endif
	const char *FpS;
	char *Accu;
	Ccc *myCC;
public:
	REGISTRI(Ccc *);
	int8_t Inc(int8_t);
	void Dec(int8_t);
	int8_t IncP();
	void DecP();
	const char *operator[](uint8_t);
	const char *operator[](struct VARS *);
	void Save();
	void Save(int8_t);
	void Get();
	void Reset();

//	void Store(long , int , struct VARS *);
//	int Comp(long , int , struct VARS *);

		};


struct LINE_NUMBER_IN_FILES {
	uint16_t lineno;			// il #riga (0=inizio
	uint32_t lineEndPos;	// la posizione in cui cambia ossia inizio nuova riga
	};
struct PROCESSED_FILES {
	char nomeFile[116];
	uint32_t begin,end;			// inizio e fine del file letto e processato, nel file uscente .i
	LINE_NUMBER_IN_FILES lines[8192];			// fare dinamico...
	};

class CCPreProcessor {
public:
	enum {
		MAX_DEFS=10
		};
	bool PP;
	struct LINE_DEF *RootDef,*LastDef;
	Ccc *m_Cc;
	CLogFile *m_Log;

private:
	bool UNDEFD[MAX_DEFS];
	uint8_t IfDefs;
	uint8_t debug;
	struct PROCESSED_FILES filesInfo[20];

public:
	CCPreProcessor(Ccc *p,uint8_t d);
	~CCPreProcessor();
  struct LINE_DEF *PROCInserLista(struct LINE_DEF *, struct LINE_DEF *, struct LINE_DEF *);
  struct LINE_DEF *PROCDelLista(struct LINE_DEF *, struct LINE_DEF *, struct LINE_DEF *);
	void swap(struct LINE_DEF * *l1, struct LINE_DEF * *l2);
	char *FNGetParm(CSourceFile *, char *, bool UNDEFD[]);
	char *FNParse(char *, int *, char *);
	char *FNGrab(CSourceFile *, char *, bool UNDEFD[]);
	char *FNGrab1(char *, int *, int *, char *);
	char *FNGetLine(CSourceFile *, char *);
	struct LINE_DEF *FNDefined(const char *);
	int PROCDefine(const char *, const char *);
	char *FNGetNextPre(CSourceFile *, bool, char *, bool UNDEFD[]);
	char *FNPreProcess(CSourceFile *, char *, bool UNDEFD[]);
	void bumpIfs(bool UNDEFD[],int8_t direction,bool state,int8_t *IfDefs);
	int FNLeggiFile(char *, COutputFile *, uint8_t);
	void setDebugLevel(uint8_t d) { debug=d; }
	};



class CTimeSpanEx : public CTimeSpan {
public:
	static CTimeSpan GetCurrentTime();
	};


class CTimeEx : public CTime {
public:
	CTimeEx(int nYear, int nMonth, int nDay,
		int nHour, int nMin, int nSec, int nDST = -1) { CTime t(nYear, nMonth, nDay,
      nHour, nMin, nSec, nDST); *this=*((CTimeEx*)&t);}
	CTimeEx(CTime t) { *this=*((CTimeEx*)&t); }
	static CString Num2Mese(int);
	static CString Num2Giorno(int);
	static CString Num2Month3(int);
	static CString Num2Day3(int);
	static CString getNow(int ex=0);
	static CString getNowGMT(bool bAddCR=TRUE);
	static CString getNowGoogle(bool bAddCR=TRUE);
	static int getMonthFromString(const CString);
	static int getMonthFromGMTString(const CString);
	static CString getMese() { return Num2Giorno(GetCurrentTime().GetDay()); };
	static CString getGiorno() { return Num2Mese(GetCurrentTime().GetMonth()); };
	static CString getFasciaDellaGiornata();
	static CString getSaluto();
	static WORD GetDayOfYear();
	static CTime parseGMTTime(const CString);
	static CTime parseTime(const CString);
	static bool isWeekend();
	bool isWeekend(CTime);
	void AddMonths(int n=1);
	int GetDaysOfMonth();
	};


class CLogFile : public CStdioFile {
	// in VC42 non riuscivo a ereditare da CStdioFile!!! d errore del c. con GetFileTitle
public:
	enum timeStampTypes {
		dontUseDate=0,
		date=1,
		dateTime=2,
		dateTimeMillisec=3,
		flagInfo=0x100,
		flagInfo2=0x100,
		flagValue=0x101,
		flagError=0x102,
		flagError2=0x103,
		flagWarning=0x104,
		flagAlert=0x105,
		flushImmediate=0x80000000,
		keepOpen=0x40000000,
		useIndex=0x20000000
		};
private:
	CString nomeFile,nomeFileNdx;
	const CWnd *textWnd;		// se c'e', indica dove visualizzare la riga di log
	DWORD mode;
	CFile *hIndexFile;
	CRITICAL_SECTION m_cs;

public:
	CLogFile(const CString,const CWnd *myWnd=NULL,DWORD m=dateTime | flushImmediate);
	CLogFile(CFile *f2,const CWnd *myWnd=NULL,DWORD m=dateTime);
	~CLogFile();
	int print(int m,const TCHAR *s,...);		 // m=0 info, 1= letture skynet, 2=errore
	int Open();
	void Close();
	void operator<<(const TCHAR *);
	int ReIndex();
	CString getNow() const;
	int RenameAndStore(int how=0 /* default: appende GG/MM/AAAA*/);
	static CString getNowApache();
	char *getLine(int ,char *,UINT nMax=255);
	DWORD getTotLines() const;
	CString getIndexFileName();
	bool GetStatus(CFileStatus &);
	int clearAll();
	static char *getAsHex(const BYTE *,char *,UINT );

private:
//	CString GetFileTitle() { CString a; return a; };
	};

