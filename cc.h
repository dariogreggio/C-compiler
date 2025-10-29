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
#define __VER__ MAKEWORD(4,2)
#elif I8086
#define __VER__ MAKEWORD(1,2)
#elif I8051
#define __VER__ MAKEWORD(0,1)
#elif MICROCHIP
#define __VER__ MAKEWORD(1,1)
#elif MC68000
#define __VER__ MAKEWORD(0,1)
#endif

enum {
	PWM_COMMWRITE =  WM_USER+1,
	WM_UPDATE_PANE,
	};


#define MAX_NAME_LEN 31

struct ERRORE {
  int t,l;
  const char *s;
  };

union SUB_OP_DEF {
  char label[32];
  int n;
  struct VARS *v;
  };
    
enum OPDEF_MODE {
	OPDEF_MODE_NULLA=0,
	OPDEF_MODE_REGISTRO=3,
	OPDEF_MODE_REGISTRO16=3,		// FINIRE (per i8086/68000)
	OPDEF_MODE_REGISTRO32=3,		// FINIRE (per i8086/68000)
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
	OPDEF_MODE_REGISTRO_INDIRETTO=0x83,
	OPDEF_MODE_IMMEDIATO_INDIRETTO=0x86,
	OPDEF_MODE_FRAMEPOINTER_INDIRETTO=0x88,
	OPDEF_MODE_VARIABILE_INDIRETTO=0x89,
	OPDEF_MODE_STACKPOINTER_INDIRETTO=0x8b
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
	};

struct LINE {
  struct LINE *next;
  struct LINE *prev;
  enum LINE_TYPE type;             // 0 commento, 1 label, 2 data def, 3 label con istr., 8 jump, 9 call, 16 istr.
  char opcode[16];
  struct OP_DEF s1;
  struct OP_DEF s2;
  char rem[64];
  };
  
struct LINE_DEF {
  struct LINE_DEF *next;
  struct LINE_DEF *prev;
  char s[1];
  };
  
union STR_LONG {
  char s[128];
  long l;
  };

typedef int32_t NUM_TYPE;
  
struct SEGMENTED_VALUE {
	NUM_TYPE v;
	int16_t seg;
	bool valid;
	};

struct OPERAND {
	struct SEGMENTED_VALUE l;
//	long l;
	uint16_t S;
// verificare...	enum ARITM_ELEM T;
	int T;
	struct VARS *var;
//	enum VQ_ELEM V;
	int8_t V;
	int8_t op;
	int8_t Ind;
#if GD24032
	int8_t preOp,postOp;
	int8_t addr64;
	int8_t reg2reg;
#endif
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
	CLASSE_INTERRUPT=0x10,
	CLASSE_PASCAL=0x20,
	CLASSE_INLINE=0x40
	};
enum VAR_MODIFIERS {		// v. anche class Ccc
// 1 interrupt, 2 pascal, 4 C
	FUNC_MODIF_INTERRUPT=1,
	FUNC_MODIF_PASCAL=2,
	FUNC_MODIF_C=4,
	FUNC_MODIF_INLINE=8
	};
enum VAR_TYPES {		// v. anche class Ccc
// 0= label, 7=public defined, 8=FUNZ., 9=public, 10=extern, 11=STRUCT
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
	};
struct VARS {
  char label[MAX_NAME_LEN+1];
  char name[MAX_NAME_LEN+1];
  char classe;
// 0 extern, 1 global, 2 static, 3 auto, 4 register; 
  char modif;
// 1 interrupt, 2 pascal, 4 C
  long type;
  int  size;
  char block;
  struct VARS *func;
  char *parm;
  struct TAGS *tag;         // se <>0, la var. è un membro della struct tag
  struct TAGS *hasTag;      // questo indica il tag di questa struct
  int  dim;
  struct VARS *next;
  };

struct CONS {
  char label[MAX_NAME_LEN+1];
  char name[128];
  struct CONS *next;
  };  
   
struct TAGS {
  char label[MAX_NAME_LEN+1];
  struct VARS *member;
  struct TAGS *next;
  };  
   
struct OPERANDO {
  char *s;
  /*enum OPERANDI*/ int p;
  };

struct TIPI {
  char s[MAX_NAME_LEN+1];
  int size;
  long type;
  struct TAGS *tag;
  int dim;
  };
           
struct BLOCK_PTR {
  struct LINE *TX;   // CONTIENE I PUNTATORI PENDENTI LastOut AL FILE OUT
  char T[20];    // CONTIENE I NOMI DEI FINE-BLOCCHI, # SE DO, & SE SWITCH,% SE if
  char C[20];   // CONTIENE LE LABEL PER continue
  char B[20];   // CONTIENE LE LABEL PER break
  char *parm;
  };
   

#define MAKEPTRREG(s) (*(uint8_t*)s)
#define MAKEPTROFS(s) (*(int*)s)

class COpenCDoc;
class COpenCView2;
class CLogFile;

/////////////////////////////////////////////////////////////////////////////

#if ARCHI || Z80 || I8051
#elif I8086
	extern int CPU86;               // 0 8086, 1 80186, 2 80286, 3 80386, 4 80486, 5 Pentium, 6 Pentium2, 7 Pentium3
#elif MICROCHIP
	extern int CPUPIC,CPUEXTENDEDMODE,stackLarge;              // 0=12; 1=16; 2=18; 3=24; 4=30/33; 5=32
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
		static BOOL IsAlpha(char);
		BOOL IsAlpha(int);
		static BOOL IsAlnum(char);
		BOOL IsAlnum(int);
		static BOOL IsDigit(char);
		BOOL IsDigit(int);
		static BOOL IsPrint(char);
		BOOL IsPrint(int);
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

#define MAX_DEFS 10
class Ccc {
public:
	enum CLASSI {
		CLASSE_EXTERN=0,
		CLASSE_GLOBAL=1,
		CLASSE_STATIC=2,
		CLASSE_AUTO=3,
		CLASSE_REGISTER=4,
		CLASSE_INTERRUPT=0x10,
		CLASSE_PASCAL=0x20,
		CLASSE_INLINE=0x40
		};
	enum {
		FUNC_MODIF_INTERRUPT=1,
		FUNC_MODIF_PASCAL=2,
		FUNC_MODIF_C=4,
		FUNC_MODIF_INLINE=8
		};
	enum {
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
		};
	enum OPERANDO_CONDIZIONALE {
		CONDIZ_MINORE=0x80,
		CONDIZ_MAGGIORE_UGUALE=0x81,
		CONDIZ_MINORE_UGUALE=0x82,
		CONDIZ_MAGGIORE=0x83,
		CONDIZ_UGUALE=0x84,
		CONDIZ_DIVERSO=0x85,
		CONDIZ_MINORE_UNSIGNED=0x86,
		CONDIZ_MAGGIORE_UGUALE_UNSIGNED=0x87,
		CONDIZ_MINORE_UGUALE_UNSIGNED=0x88,
		CONDIZ_MAGGIORE_UNSIGNED=0x89,
		};
	enum {
		VALUE_IS_COSTANTE=8
		};

protected:
// Attributes
	CWnd *myOutput;
	CLogFile *myLog;

public:
	Ccc(const CWnd *p);
	~Ccc();


protected:
	int bExit;
	char buffer[128];
  FILE *FPre,*FO1,*FO2,*FO3,*FO4,*FObj,*FIn,*FCod;
	CStdioFile *FLst;
	int Warning;
  struct LINE *RootOut,*LastOut,*StaticOut,*BSSOut;
	struct LINE_DEF *RootDef,*LastDef;
	uint8_t UNDEFD[MAX_DEFS];
	struct VARS *Var;
	struct VARS *LVars;   // root, used, last
	struct VARS *CurrFunc;
	struct CONS *Con;
	struct CONS *LCons;
	struct TAGS *StrTag;
	struct TAGS *LTag;
	//struct VARS *TAG;           
	//int SX;
	uint8_t InBlock; 			// LIVELLO DI BLOCCHI
	uint8_t IfDefs;
	int TempSize;
	int MaxTypes;
	int UseLMul,UseFMul,UseIRQ,UseTemp;
	char *RootIn;
	uint8_t Brack;				// usati da FNRev
	int TempProg;				//
	struct LINE *GlblOut;	//

	char NFS[256],OUS[256];

	int __STDC__;
	int LABEL;
	char __file__[256],__name__[256];
	int __line__;
	char __date__[11];
	char __time__[11];
	char TextSegm[64],DataSegm[64]; 
	int Declaring,FuncCalled,SaveFP,ASM,AutoOff,PP;
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
	int Optimize;
	static struct TIPI Types[50];
	static struct OPERANDO Op[46];
	uint16_t numErrors,numWarnings;

// Operations
public:
	int CompilaC(int,char **);

// Compiler
	char *AddExt(char *, char *);
	int PROCBlock();
	int PROCIsDecl();
	int PROCDclVar(enum CLASSI, int, int, long, struct TAGS *, int, int);
	int subAsm(char *);
	int FNIsStmt();
	char *FNGetLabel(char *,uint8_t);
	int PROCGenCondBranch(const char *, int, int *, int);
	int FNGetCondString(int, int);
	int PROCAssignCond(int *, long *, int *, char *);
	int PROCReturn();
	long FNGetConst(char *,int);
	int PROCLoops(char *, char *, char *, struct LINE *);
	int FNRegFree();
	struct VARS *FNCercaVar(const char *, uint8_t);
	struct VARS *PROCAllocVar(const char *, long, enum CLASSI, int, int, struct TAGS *, int);
	int PROCCast(long, uint8_t, long, uint8_t);
#if MICROCHIP
	int PROCReadD0(struct VARS *, long, uint8_t, uint8_t, uint8_t lh=0);
#else
	int PROCReadD0(struct VARS *, long, uint8_t, uint8_t);
#endif
	int PROCStoreD0(struct VARS *, int, struct VARS *, union STR_LONG *);
	int PROCGetAdd(int, struct VARS  *, int);
	int PROCUsaFun(struct VARS *,int,int);
	struct CONS *FNAllocCost(const char *, uint8_t, long type=0);
	int PROCInit();
	int subOfsD0(struct VARS *, int, int, int);
	
	void subEvEx(int, int *, char *,long *, int *, int *, struct VARS * *, union STR_LONG *, struct TAGS * *, int *);
	int FNEvalExpr(int, char *);
 	int FNEvalECast(char *, long *, int /*uint8_t*/ *);
	int FNEvalCond(char *, char *, uint8_t);

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
	void subSpezReg(int, struct OP_DEF *);
#elif Z80
	void subSpezReg(int, struct OP_DEF *);
#elif I8086  
	void subSpezReg(int, struct OP_DEF *);
#elif MC68000
	void subSpezReg(int, struct OP_DEF *);
#elif MICROCHIP
	void subSpezReg(int, struct OP_DEF *);
#endif

  int FNRev(int, int *, char *,long *, int *, int *, struct VARS * *, STR_LONG *, struct TAGS **, int *);
  char *ConRecEval(char *, int, long *);
  long EVAL(char *);

	int subShift(uint8_t, int, long, uint8_t, long, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
	int subAdd(int, int, int, struct VARS *, long *, int *, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *,int);
	int subMul(char, int, int, struct VARS *, long, uint8_t, int, long, uint8_t, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
	int FNIs1Bit(int);
	int subAOX(char, int *, int, int, struct VARS *, long, int, long, int, union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);
	enum OPERANDO_CONDIZIONALE subCMP(const char *, int, int, int, struct VARS *, long, uint8_t, long, uint8_t, 
		union STR_LONG *, union STR_LONG *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *, struct OP_DEF *);

	char *FNGetParm(FILE *, char *);
	char *FNParse(char *, int *, char *);
	char *FNGrab(FILE *, char *);
	char *FNGrab1(char *, int *, int *, char *);
	struct LINE_DEF *FNDefined(const char *);
	int PROCDefine(const char *, const char *);
	char *FNGetNextPre(FILE *, int , char *);
	char *FNPreProcess(FILE *, char *);
	void bumpIfs(int8_t direction,uint8_t state,uint8_t level);
	int FNLeggiFile(char *, FILE *, signed char);
  
	void subObj(FILE *,struct OP_DEF *);
	int PROCObj(FILE *);
  char *FNTrasfNome(char *);
  int PROCError(int, const char *s=NULL);
  int PROCWarn(int, const char *s=NULL);
  int PROCV(const char *);
	int PROCT();
	int PROCD();
	int PROCVarList(CStdioFile *, struct VARS *);

	char *FNLO(char *);
	char *FNLA(char *);
	int FNGetEscape();
	int FNGetOct(const char *);
	int PROCCheck(const char *);
	int PROCCheck(char);
	long FNGetLine(long,char *);

	int OPComp(struct OP_DEF *, struct OP_DEF *);
	struct LINE *GetNextNoRem(struct LINE *);
	int Ottimizza(struct LINE *);

  struct LINE *PROCInserLista(struct LINE *, struct LINE *, struct LINE *);
  struct LINE *PROCDelLista(struct LINE *, struct LINE *, struct LINE *);
	void swap(struct LINE * *, struct LINE * *);
  int PROCOut(enum LINE_TYPE, const char *, struct OP_DEF *, struct OP_DEF *, const char *R=NULL);
  int PROCOut1(FILE *,const char *, const char *, const char *s3=NULL, const char *s4=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, enum OPDEF_MODE, union SUB_OP_DEF *, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int, enum OPDEF_MODE, union SUB_OP_DEF *, int);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, union SUB_OP_DEF *, int, enum OPDEF_MODE, int);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int, enum OPDEF_MODE, int, const char *R=NULL);
  void PROCOper(enum LINE_TYPE, const char *, enum OPDEF_MODE, int s1=0);
  int PROCOutLab(const char *,const char *s1=NULL,const char *s2=NULL);

	int FNGetAritElem(int *, char *, int *, struct VARS * *, long *, int *, union STR_LONG *, int);
	int PROCGetType(int *, long *, struct TAGS **, int *, long);
	long FNIsType(char *);
	struct VARS *FNGetAggr(struct TAGS *, const char *, int, int *);
	struct TAGS *subAllocTag(const char *);
	struct TAGS *FNAllocAggr();
	int StoreVar(struct VARS *,int, struct VARS *, union STR_LONG *);
	#if MICROCHIP
	int ReadVar(struct VARS *,long,int,int,int lh);
	#else
	int ReadVar(struct VARS *,long,int,int);
	#endif

	#if ARCHI
	int FNIsLshift(long);
	#elif Z80 || I8086 || MC68000 || MICROCHIP		// per i PIC32 forse serve in effetti
	#endif

#if MICROCHIP
	int PROCUseCost(int, long, uint8_t, union STR_LONG *,int lh);
#else
	int PROCUseCost(int, long, uint8_t, union STR_LONG *);
#endif
	int FNIsOp(const char *, int);
	int FNIsClass(const char *);
	uint8_t FNGetMemSize(long, uint8_t, uint8_t);
	uint8_t FNGetMemSize(struct VARS *, uint8_t);

	static char *OpCond[16];
	static char *StrOp[20];
	struct BLOCK_PTR OldTX[20];

  class REGISTRI *Regs;
          
	int Reg;

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
	int VSize[16];
	struct VARS *VVar[16];
#endif  
	int ToDec;                      // quanto decrementare (Inc, Dec)
	int ToGet;											// quanto pop-are (Save, Get)
public:
	int D;             
	int MaxD,UserBase,MaxUser;     // in ordine crescente
	const char *SpS;
#if I8086 || MC68000 
	const char *AbsS;
#endif
	const char *FpS;
	char *Accu;
	Ccc *myCC;
public:
	REGISTRI(Ccc *);
	int Inc(int );
	void Dec(int );
	const char *operator[](int );
	const char *operator[](struct VARS *);
	void Save();
	void Save(int );
	void Get();

//	void Store(long , int , struct VARS *);
//	int Comp(long , int , struct VARS *);

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
	static CString getNowGMT(BOOL bAddCR=TRUE);
	static CString getNowGoogle(BOOL bAddCR=TRUE);
	static int getMonthFromString(const CString);
	static int getMonthFromGMTString(const CString);
	static CString getMese() { return Num2Giorno(GetCurrentTime().GetDay()); };
	static CString getGiorno() { return Num2Mese(GetCurrentTime().GetMonth()); };
	static CString getFasciaDellaGiornata();
	static CString getSaluto();
	static WORD GetDayOfYear();
	static CTime parseGMTTime(const CString);
	static CTime parseTime(const CString);
	static BOOL isWeekend();
	BOOL isWeekend(CTime);
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
	BOOL GetStatus(CFileStatus &);
	int clearAll();
	static char *getAsHex(const BYTE *,char *,UINT );

private:
//	CString GetFileTitle() { CString a; return a; };
	};

