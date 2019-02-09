//              idapi.h - main include file for idapi clients
//
// $Revision:   4.128  $


#ifndef DBI_H
#define DBI_H
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus


#define DBIINTFVER         300                  // Version of the interface
#define DBIENGVERSION      300                  // Version of the engine (3.00)


// assume byte packing throughout
#if ! (defined(lint) || defined(_lint))
#  ifndef RC_INVOKED
#    if ( _MSC_VER >= 800 ) || (defined(__BORLANDC__) && defined(__FLAT__))
#      pragma warning(disable:4103)
#      if !(defined( MIDL_PASS )) || defined( __midl )
#        pragma pack(push)
#      endif
#      pragma pack(1)
#    elif defined(__BORLANDC__) && !defined(__FLAT__)
#      pragma option -a-
#    else
#      pragma pack(1)
#    endif
#  endif // ndef RC_INVOKED
#endif // ! (defined(lint) || defined(_lint))

#ifndef NODBITYPES

//-----------------------------------------------------------------------
//     DBI types
//-----------------------------------------------------------------------


// sdk debug layer defines
#define         DEBUGON         0x1
#define         OUTPUTTOFILE    0x2
#define         FLUSHEVERYOP    0x8
#define         APPENDTOLOG     0x10




// Constants

#define DBIMAXNAMELEN         31            // Name limit (table, field etc)
#define DBIMAXSPNAMELEN       64            // Max stored procedure name length
#define DBIMAXFUNCNAMELEN     255           // Max function name len
#define DBIMAXFLDSINKEY       16            // Max fields in a key
#define DBIMAXKEYEXPLEN      220            // Max Key expression length
#define DBIMAXEXTLEN           3            // Max file extension len, not incl. dot (excluding zero termination)
#if defined(__WIN32__)
#define DBIMAXTBLNAMELEN     260            // Max table name length
#define DBIMAXDRIVELEN       127            // Max drive length
#define DBIMAXPATHLEN        260            // Max path+file name len (excluding zero termination)
#else
#define DBIMAXTBLNAMELEN     127            // Max table name length
#define DBIMAXDRIVELEN         2            // Max drive length
#define DBIMAXPATHLEN         81            // Max path+file name len (excluding zero termination)
#endif // defined(WIN32)

#define DBIMAXMSGLEN         127            // Max message len
#define DBIMAXVCHKLEN        255            // Max val check len
#define DBIMAXPICTLEN        175            // Max picture len
#define DBIMAXFLDSINSEC      256            // Max fields in security spec

#define DBIMAXSCFIELDS        32            // max nbr of fields in a config section
#define DBIMAXSCFLDLEN       128            // max field length
#define DBIMAXSCRECSIZE     DBIMAXSCFIELDS*DBIMAXSCFLDLEN  // max rec size

#define DBIMAXUSERNAMELEN      14           // Max user name (general)
#define DBIMAXXBUSERNAMELEN    16           // Max user name length (xBASE)
#define DBIMAXBOOKMARKLEN    4104           // Max Bookmark length

#define DBIMAXTRACELEN       8192           // Max trace msg len

#define DBIMAXTYPEDESC       127            // Max Type Description size
#define DBIMAXDRSQLSTR       8192           // Max Size of SQL Constraint


#if defined(__FLAT__)
#ifdef far
#undef far
#endif // far
#define far
#endif // defined __FLAT__

#ifndef NODBIBASETYPES

#ifndef TRUE
#  define TRUE 1
#endif // TRUE
#ifndef FALSE
#  define FALSE 0
#endif // FALSE

//For pointers
#ifndef NULL
#  define NULL (0)
#endif // NULL

#if defined(__WIN32__) || defined (__NT__)
   #if !defined(VOID)
       #define VOID    void
       typedef char CHAR;                     // from winnt.h
       typedef short SHORT;
       typedef long LONG;
   #endif
#else
   #if !defined(VOID)
      #define VOID    void
   #endif // !defined(VOID)
   typedef char CHAR;                     // from windows.h
#endif // defined(WIN32) && !defined(VOID)

#ifndef _WINDEF_
typedef unsigned char       BYTE;
#endif

#ifndef INT8
#  define INT8    char
#endif // INT8

#ifndef UINT8
#  define UINT8   unsigned char
#endif // UINT8

#ifndef INT16
#if defined(__FLAT__)
#  define INT16   short
#else
#  define INT16   int
#endif // defined(__FLAT__)
#endif // INT16

#ifndef UINT16
#if defined(__FLAT__)
#  define UINT16  unsigned short
#else
#  define UINT16  unsigned int
#endif // defined(__FLAT__)
#endif // UINT16

#ifndef INT32
#  define INT32   long
#endif // INT32
#ifndef UINT32
#  define UINT32  unsigned long
#endif // UINT32

#ifndef BOOL
typedef int BOOL;                      // from windows.h
#endif

typedef unsigned char BOOL8;
typedef short         BOOL16;

#endif // NODBIBASETYPES

typedef double              DFLOAT;

typedef long      DBIDATE;
typedef long      TIME;
typedef double    TIMESTAMP;

typedef UINT32    hDBICfg;

typedef VOID      far *pVOID;
typedef pVOID     far *ppVOID;
typedef CHAR      far *pCHAR;
typedef const CHAR far *pCCHAR;
typedef BYTE      far *pBYTE;
typedef INT8      far *pINT8;
typedef UINT8     far *pUINT8;
typedef INT16     far *pINT16;
typedef UINT16    far *pUINT16;
typedef INT32     far *pINT32;
typedef UINT32    far *pUINT32;
typedef DFLOAT    far *pDFLOAT;
typedef DBIDATE   far *pDBIDATE;
typedef TIME      far *pTIME;
typedef BOOL      far *pBOOL;
typedef BOOL16    far *pBOOL16;
typedef TIMESTAMP far *pTIMESTAMP;

#if !defined(__WIN32__) || (defined(__WATCOMC__) && !defined (__NT__))
#define FLOAT     DFLOAT            // for compatibility with BDE 2.0
#define pFLOAT    pDFLOAT
#define DATE      DBIDATE
#define pDATE     pDBIDATE
#endif // WIN32

typedef pBYTE     far *ppBYTE;
typedef pCHAR     far *ppCHAR;
typedef pBOOL     far *ppBOOL;
typedef pBOOL16   far *ppBOOL16;

typedef hDBICfg   far *phDBICfg;


// Constants

//===========================================================================
//                            G e n e r a l
//===========================================================================

#if defined(__FLAT__)
#define DBIFN  __stdcall               // All functions are this type
#else
#define DBIFN  __pascal far            // All functions are this type
#endif // define(__FLAT__)
typedef UINT16          DBIResult;     // Function result
typedef DBIResult  far *pDBIResult;    // Function result pointer

typedef UINT32           hDBIObj;      // Generic object handle

typedef hDBIObj          hDBIDb;       // Database handle
typedef hDBIObj          hDBIQry;      // Query handle
typedef hDBIObj          hDBIStmt;     // Statement handle ("new query")
typedef hDBIObj          hDBICur;      // Cursor handle
typedef hDBIObj          hDBISes;      // Session handle
typedef hDBIObj          hDBIXlt;      // Translation handle

typedef hDBIObj    far *phDBIObj;      // pointer to generic object handle
typedef hDBIDb     far *phDBIDb;       // pointer to Database  handle
typedef hDBIQry    far *phDBIQry;      // pointer to Query     handle
typedef hDBIStmt   far *phDBIStmt;     // pointer to Statement handle
typedef hDBICur    far *phDBICur;      // pointer to Cursor    handle
typedef hDBISes    far *phDBISes;      // pointer to Session   handle
typedef hDBIXlt    far *phDBIXlt;      // pointer to Translation handle

typedef UINT32          hDBIXact;      // Transaction handle
typedef hDBIXact   far *phDBIXact;     // pointer to Transaction handle

typedef hDBIObj         hDBIDR ;       // Handle to a Client Data Repository (CDR) Object
typedef hDBIDR     far  *phDBIDR ;     // Ptr to CDR Handle

// typedefs for buffers of various common sizes:
//
typedef CHAR   DBIPATH    [DBIMAXPATHLEN+1];     // holds a DOS path
typedef CHAR   DBINAME    [DBIMAXNAMELEN+1];     // holds a name
typedef CHAR   DBIEXT     [DBIMAXEXTLEN+1];      // holds an extension EXT
typedef CHAR   DBIDOTEXT  [DBIMAXEXTLEN+2];      // holds an extension EXT including '.'
typedef CHAR   DBIDRIVE   [DBIMAXDRIVELEN+1];    // holds a drive name
typedef CHAR   DBITBLNAME [DBIMAXTBLNAMELEN+1];  // holds a table name
typedef CHAR   DBISPNAME  [DBIMAXSPNAMELEN+1];   // holds a stored procedure name
typedef CHAR   DBIUSERNAME[DBIMAXUSERNAMELEN+1]; // holds a user name
typedef UINT16 DBIKEY     [DBIMAXFLDSINKEY];     // holds list of fields in a key
typedef CHAR   DBIKEYEXP  [DBIMAXKEYEXPLEN+1];   // holds a key expression
typedef BYTE   DBIVCHK    [DBIMAXVCHKLEN+1];     // holds a validity check
typedef CHAR   DBIPICT    [DBIMAXPICTLEN+1];     // holds a picture (Pdox)
typedef CHAR   DBIMSG     [DBIMAXMSGLEN+1];      // holds an error message
typedef CHAR   DBIDRTYPEDESC [DBIMAXTYPEDESC+1]; // holds a Type Description string
typedef CHAR   DBISQLSTR   [DBIMAXDRSQLSTR+1];    // a SQL string


//===========================================================================
//                            Environmental
//===========================================================================

typedef struct                            // Struct for defining the environ.
   {
      DBIPATH        szWorkDir;                 // Working directory
      DBIPATH        szIniFile;                 // Configuration file
      BOOL16         bForceLocalInit;           // Force local initialization
      DBINAME        szLang;                    // System language driver
      DBINAME        szClientName;              // Client name (documentary)
   } DBIEnv;
typedef DBIEnv far   *pDBIEnv;

//============================================================================
//                   System Info
//============================================================================

typedef struct                         // System Version Info
   {
      UINT16         iVersion;            // Engine version
      UINT16         iIntfLevel;          // Client Interface level
      DBIDATE        dateVer;             // Version date (Compile/Release)
      TIME           timeVer;             // Version time (Compile/Release)
#if defined(__WIN32__)
      CHAR           szVersion[20];       // Version name (xxx.xxx.xxx.xxx)
#endif
   } SYSVersion;
typedef SYSVersion far *pSYSVersion;

typedef struct                         // System configuration (basic)
   {
      BOOL16         bLocalShare;         // If Local files will be shared
      UINT16         iNetProtocol;        // Net Protocol (35, 40 etc.)
      BOOL16         bNetShare;           // If Net files will be shared
      DBINAME        szNetType;           // Network type
      DBIUSERNAME    szUserName;          // Network user name
      DBIPATH        szIniFile;           // Configuration file
      DBINAME        szLangDriver;        // System language driver
   } SYSConfig;
typedef SYSConfig far *pSYSConfig;

typedef struct                         // System Status/Info
   {
      UINT16         iBufferSpace;           // in K
      UINT16         iHeapSpace;             // in K
      UINT16         iDrivers;               // Active/Loaded drivers
      UINT16         iClients;               // Active clients
      UINT16         iSessions;              // Number of sessions (For all clients)
      UINT16         iDatabases;             // Open databases
      UINT16         iCursors;               // Number of cursors
   } SYSInfo;
typedef SYSInfo far * pSYSInfo;

typedef struct
   {
      DBINAME        szName;                 // Documentary name
      UINT16         iSessions;              // Number of sessions
      DBIPATH        szWorkDir;              // Working directory
      DBINAME        szLang;                 // System language driver (Client supplied)
   } CLIENTInfo;
typedef CLIENTInfo far * pCLIENTInfo;

typedef struct
   {
      UINT16         iSession;               // Session id (1..n)
      DBINAME        szName;                 // Documentary name
      UINT16         iDatabases;             // Open databases
      UINT16         iCursors;               // Open cursors
      INT16          iLockWait;              // Lock wait time (in seconds)
      DBIPATH        szNetDir;               // Net directory location
      DBIPATH        szPrivDir;              // Current Private directory
   } SESInfo;
typedef SESInfo far * pSESInfo;

//============================================================================
//                             Table / Field Types
//============================================================================

// Driver Types

#define szPARADOX       "PARADOX"
#define szDBASE         "DBASE"
#define szASCII         "ASCIIDRV"


// Field Types (Logical)

#define fldUNKNOWN      0
#define fldZSTRING      1                 // Null terminated string
#define fldDATE         2                 // Date     (32 bit)
#define fldBLOB         3                 // Blob
#define fldBOOL         4                 // Boolean  (16 bit)
#define fldINT16        5                 // 16 bit signed number
#define fldINT32        6                 // 32 bit signed number
#define fldFLOAT        7                 // 64 bit floating point
#define fldBCD          8                 // BCD
#define fldBYTES        9                 // Fixed number of bytes
#define fldTIME         10                // Time        (32 bit)
#define fldTIMESTAMP    11                // Time-stamp  (64 bit)
#define fldUINT16       12                // Unsigned 16 bit integer
#define fldUINT32       13                // Unsigned 32 bit integer
#define fldFLOATIEEE    14                // 80-bit IEEE float
#define fldVARBYTES     15                // Length prefixed var bytes
#define fldLOCKINFO     16                // Look for LOCKINFO typedef

#define MAXLOGFLDTYPES  17                // Number of logical fieldtypes

// Sub Types (Logical)

// fldFLOAT subtype

#define fldstMONEY      21                // Money

// fldBLOB subtypes

#define fldstMEMO          22             // Text Memo
#define fldstBINARY        23             // Binary data
#define fldstFMTMEMO       24             // Formatted Text
#define fldstOLEOBJ        25             // OLE object (Paradox)
#define fldstGRAPHIC       26             // Graphics object
#define fldstDBSOLEOBJ     27             // dBASE OLE object
#define fldstTYPEDBINARY   28             // Typed Binary data

// fldZSTRING subtype

#define fldstPASSWORD      1              // Password

// fldINT32 subtype

#define  fldstAUTOINC      29

// Paradox types (Physical)

#define  fldPDXCHAR        0x101        // Alpha    (string)
#define  fldPDXNUM         0x102        // Numeric
#define  fldPDXMONEY       0x103        // Money
#define  fldPDXDATE        0x104        // Date
#define  fldPDXSHORT       0x105        // Short
#define  fldPDXMEMO        0x106        // Text Memo       (blob)
#define  fldPDXBINARYBLOB  0x107        // Binary data     (blob)
#define  fldPDXFMTMEMO     0x108        // Formatted text  (blob)
#define  fldPDXOLEBLOB     0x109        // OLE object      (blob)
#define  fldPDXGRAPHIC     0x10A        // Graphics object (blob)
#define  fldPDXBLOB        fldPDXMEMO
#define  fldPDXLONG        0x10B        // Long
#define  fldPDXTIME        0x10C        // Time
#define  fldPDXDATETIME    0x10D        // Time Stamp
#define  fldPDXBOOL        0x10E        // Logical
#define  fldPDXAUTOINC     0x10F        // Auto increment (long)
#define  fldPDXBYTES       0x110        // Fixed number of bytes
#define  fldPDXBCD         0x111        // BCD (32 digits)
#define  fldPDXUNICHAR     0x112        // not supported yet

// xBASE types (Physical)

#define  fldDBCHAR         0x201        // Char string
#define  fldDBNUM          0x202        // Number
#define  fldDBMEMO         0x203        // Memo          (blob)
#define  fldDBBOOL         0x204        // Logical
#define  fldDBDATE         0x205        // Date
#define  fldDBFLOAT        0x206        // Float
#define  fldDBLOCK         0x207        // Logical type is LOCKINFO
#define  fldDBOLEBLOB      0x208        // OLE object    (blob)
#define  fldDBBINARY       0x209        // Binary data   (blob)
#define  fldDBBYTES        0x20A        // Only for TEMPORARY tables

// xBASE key types (Cannot be used as field types)

#define  fldDBKEYNUM       0x210
#define  fldDBKEYBCD       0x211


// Ascii types (Physical)

#define  fldASCCHAR         0x301       // Char string
#define  fldASCNUM          0x302       // Number
#define  fldASCBOOL         0x303       // Logical
#define  fldASCDATE         0x304       // Date
#define  fldASCFLOAT        0x305       // Float
#define  fldASCLOCK         0x306       // Not used
#define  fldASCTIMESTAMP    0x307       // TimeStamp
#define  fldASCTIME         0x308       // Time
#define  fldASCLONG         0x309       // Long
#define  fldASCMONEY        0x30A       // Money


//============================================================================
//                    Field descriptor
//============================================================================


typedef enum                              // Field Val Check type
   {
      fldvNOCHECKS     = 0,                  // Does not have explicit val checks
      fldvHASCHECKS    = 1,                  // One or more val checks on the field
      fldvUNKNOWN      = 2                   // Dont know at this time
   } FLDVchk;

typedef enum                              // Field Rights
   {
      fldrREADWRITE     = 0,                 // Field can be Read/Written
      fldrREADONLY      = 1,                 // Field is Read only
      fldrNONE          = 2,                 // No Rights on this field
      fldrUNKNOWN       = 3                  // Dont know at this time
   } FLDRights;

typedef struct {                          // Field Descriptor
      UINT16         iFldNum;                // Field number (1..n)
      DBINAME        szName;                 // Field name
      UINT16         iFldType;               // Field type
      UINT16         iSubType;               // Field subtype (if applicable)
      INT16          iUnits1;                // Number of Chars, digits etc
      INT16          iUnits2;                // Decimal places etc.
      UINT16         iOffset;                // Offset in the record (computed)
      UINT16         iLen;                   // Length in bytes (computed)
      UINT16         iNullOffset;            // For Null bits (computed)
      FLDVchk        efldvVchk;              // Field Has vcheck (computed)
      FLDRights      efldrRights;            // Field Rights (computed)
      BOOL16         bCalcField;             // Is Calculated field (computed)
      UINT16         iUnUsed[2];
   } FLDDesc;
typedef FLDDesc far *pFLDDesc;

//============================================================================
//                    Index descriptor
//============================================================================


typedef struct                            // Index description
   {
      DBITBLNAME     szName;                 // Index name
      UINT16         iIndexId;               // Index number
      DBINAME        szTagName;              // Tag name (for dBASE)
      DBINAME        szFormat;               // Optional format (BTREE, HASH etc)
      BOOL16         bPrimary;               // True, if primary index
      BOOL16         bUnique;                // True, if unique keys
      BOOL16         bDescending;            // True, for descending index
      BOOL16         bMaintained;            // True, if maintained index
      BOOL16         bSubset;                // True, if subset index
      BOOL16         bExpIdx;                // True, if expression index
      UINT16         iCost;                  // Not used
      UINT16         iFldsInKey;             // Fields in the key (1 for Exp)
      UINT16         iKeyLen;                // Phy Key length in bytes (Key only)
      BOOL16         bOutofDate;             // True, if index out of date
      UINT16         iKeyExpType;            // Key type of Expression
      DBIKEY         aiKeyFld;               // Array of field numbers in key
      DBIKEYEXP      szKeyExp;               // Key expression
      DBIKEYEXP      szKeyCond;              // Subset condition
      BOOL16         bCaseInsensitive;       // True, if case insensitive index
      UINT16         iBlockSize;             // Block size in bytes
      UINT16         iRestrNum;              // Restructure number
      BOOL16         abDescending[DBIMAXFLDSINKEY];   // TRUE
      UINT16         iUnUsed[16];
   } IDXDesc;
typedef IDXDesc far *pIDXDesc;

//============================================================================
//                    Validity check, Referential integrity descriptors
//============================================================================



// Subtypes for Lookup

typedef enum                              // Paradox Lookup type
   {
      lkupNONE          = 0,                 // Has no lookup
      lkupPRIVATE       = 1,                 // Just Current Field + Private
      lkupALLCORRESP    = 2,                 // All Corresponding + No Help
      lkupHELP          = 3,                 // Just Current Fld + Help and Fill
      lkupALLCORRESPHELP= 4                  // All Corresponging + Help
   } LKUPType;

#define TODAYVAL   2                       // for Min/Max/Def val flags
#define NOWVAL     3                       // for Min/Max/Def val flags

   // In VCHKDesc below, if any of bHasMinVal/bHasMaxVal/bHasDefVal
   // = TODAYVAL, then TODAY is assumed , = NOWVAL, then current time/today is assumed

typedef struct                            // Val Check structure
   {
      UINT16         iFldNum;                // Field number
      BOOL16         bRequired;              // If True, value is required
      BOOL16         bHasMinVal;             // If True, has min value
      BOOL16         bHasMaxVal;             // If True, has max value
      BOOL16         bHasDefVal;             // If True, has default value
      DBIVCHK        aMinVal;                // Min Value
      DBIVCHK        aMaxVal;                // Max Value
      DBIVCHK        aDefVal;                // Default value
      DBIPICT        szPict;                 // Picture string
      LKUPType       elkupType;              // Lookup/Fill type
      DBIPATH        szLkupTblName;          // Lookup Table name
   } VCHKDesc;
typedef VCHKDesc far *pVCHKDesc;

typedef enum                              // Ref integrity type
   {
      rintMASTER     = 0,                    // This table is Master
      rintDEPENDENT  = 1                     // This table is Dependent
   } RINTType;

typedef enum                              // Ref integrity action/qualifier
   {
      rintRESTRICT   = 0,                    // Prohibit operation
      rintCASCADE    = 1                     // Cascade operation
   } RINTQual;

typedef struct                            // Ref Integrity Desc
   {
      UINT16         iRintNum;               // Ref integrity number
      DBINAME        szRintName;             // A name to tag this integegrity constraint
      RINTType       eType;                  // Whether master/dependent
      DBIPATH        szTblName;              // Other table name
      RINTQual       eModOp;                 // Modify qualifier
      RINTQual       eDelOp;                 // Delete qualifier
      UINT16         iFldCount;              // Fields in foreign key
      DBIKEY         aiThisTabFld;           // Fields in this table
      DBIKEY         aiOthTabFld;            // Fields in other table
   } RINTDesc;
typedef RINTDesc far *pRINTDesc;


//============================================================================
//                    Security descriptor
//============================================================================
// Family rights

#define NOFAMRIGHTS     0x00                 // No Family rights
#define FORMRIGHTS      0x01                 // Can change form
#define RPTRIGHTS       0x02                 // Can change reports
#define VALRIGHTS       0x04                 // Can change valchecks
#define SETRIGHTS       0x08                 // Can change settings
#define ALLFAMRIGHTS    (FORMRIGHTS | RPTRIGHTS | VALRIGHTS | SETRIGHTS)
                                             // All family rights

typedef enum  {                           // Privileges
      prvNONE        = 0,                    // No privilege
      prvREADONLY    = 0x01,                 // Read only Table or Field
      prvMODIFY      = 0x03,                 // Read and Modify fields (non-key)
      prvINSERT      = 0x07,                 // Insert + All of above
      prvINSDEL      = 0x0F,                 // Delete + All of above
      prvFULL        = 0x1F,                 // Full Writes
      prvUNKNOWN     = 0xFF                  // Unknown
   } PRVType;


typedef struct {                             // Security description
      UINT16         iSecNum;                   // Nbr to identify desc
      PRVType        eprvTable;                 // Table privileges
      UINT16         iFamRights;                // Family rights
      DBINAME        szPassword;                // Null terminated string
      PRVType        aprvFld[DBIMAXFLDSINSEC];  // Field level privileges (prvNONE/prvREADONLY/prvFULL)
   } SECDesc;
typedef SECDesc far *pSECDesc;


//======================================================================
//                         Miscellaneous
//======================================================================

// Index Id used to open table without a default index (i.e. no order)

#define  NODEFAULTINDEX    0xFFFF


//============================================================================
//                    Object types
//============================================================================

typedef enum
   {
       objSYSTEM                 = 1,      // System object
       objSESSION                = 2,      // Session object
       objDRIVER                 = 3,      // Driver object
       objDATABASE               = 4,      // Database object
       objCURSOR                 = 5,      // Cursor object
       objSTATEMENT              = 6,      // Statement object
       objCLIENT                 = 7,      // Client object
       objDBSEC                  = 8,      // DbSystem object (dBASE only)
       objREPOSITORY             = 9       // Data Repository object
   }  DBIOBJType;

//============================================================================
//                    Cursor properties
//============================================================================


typedef enum                              // Database/Table Share type
   {
      dbiOPENSHARED     = 0,                 // Open shared  (Default)
      dbiOPENEXCL       = 1,                 // Open exclusive
   } DBIShareMode;

typedef enum                              // Database/Table Access type
   {
      dbiREADWRITE      = 0,                 // Read + Write   (Default)
      dbiREADONLY       = 1                  // Read only
   } DBIOpenMode;

typedef enum                              // Lock types (Table level)
   {
      dbiNOLOCK         = 0,                 // No lock   (Default)
      dbiWRITELOCK      = 1,                 // Write lock
      dbiREADLOCK       = 2                  // Read lock
   } DBILockType;

typedef enum                              // Field translate mode
   {
      xltNONE        = 0,                    // No translation  (Physical Types)
      xltRECORD      = 1,                    // Record level translation (not supported)
      xltFIELD       = 2,                    // Field level translation (Logical types)
   } XLTMode;

typedef enum                              // Update lock mode (SQL only)
   {
      updWHEREALL    = 0,                    // All fields in WHERE clause
      updWHEREKEYCHG = 1,                    // Keyed and changed fields in WHERE clause
      updWHEREKEY    = 2                     // Keyed fields in WHERE clause
   } UPDLockMode;


// Table levels

#define TBLLEVEL3    3              // Paradox level 3 and dBASE level 3+
#define TBLLEVEL4    4              // Paradox level 4 and dBASE level 4
#define TBLLEVEL5    5              // Paradox level 5 and dBASE/Win
#define TBLLEVEL7    7              // Paradox level 7 , Win32
#define FOXLEVEL25   25             // Fox Table (Ver 2.5)

typedef struct {                          // Virtual Table properties
      DBITBLNAME     szName;                 // table name (no extension, if it can be derived)
      UINT16         iFNameSize;             // Full file name size
      DBINAME        szTableType;            // Driver type
      UINT16         iFields;                // No of fields in Table
      UINT16         iRecSize;               // Record size (logical record)
      UINT16         iRecBufSize;            // Record size (physical record)
      UINT16         iKeySize;               // Key size
      UINT16         iIndexes;               // Number of indexes
      UINT16         iValChecks;             // Number of val checks
      UINT16         iRefIntChecks;          // Number of Ref Integrity constraints
      UINT16         iBookMarkSize;          // Bookmark size
      BOOL16         bBookMarkStable;        // Stable book marks
      DBIOpenMode    eOpenMode;              // ReadOnly / RW
      DBIShareMode   eShareMode;             // Excl / Share
      BOOL16         bIndexed;               // Index is in use
      INT16          iSeqNums;               // 1: Has Seqnums; 0: Has Record#
                                             // <0 (-1, -2, ...): None (e.g. SQL)
      BOOL16         bSoftDeletes;           // Supports soft deletes
      BOOL16         bDeletedOn;             // If above, deleted recs seen
      UINT16         iRefRange;              // Not used
      XLTMode        exltMode;               // Translate Mode
      UINT16         iRestrVersion;          // Restructure version number
      BOOL16         bUniDirectional;        // Cursor is uni-directional
      PRVType        eprvRights;             // Table  rights
      UINT16         iFmlRights;             // Family rights
      UINT16         iPasswords;             // Number of Aux passwords
      UINT16         iCodePage;              // Codepage (0 if unknown)
      BOOL16         bProtected;             // Table is protected by password
      UINT16         iTblLevel;              // Driver dependent table level
      DBINAME        szLangDriver;           // Language driver name
      BOOL16         bFieldMap;              // Field map active
      UINT16         iBlockSize;             // Physical file blocksize in K
      BOOL16         bStrictRefInt;          // Strict referential integrity
      UINT16         iFilters     ;          // Number of filters
      BOOL16         bTempTable   ;          // Table is a temporary table
      UINT16         iUnUsed[16];
     } CURProps;
typedef CURProps far *pCURProps;

//============================================================================
//                   Record Properties
//============================================================================


typedef struct {                          // Record properties
      UINT32         iSeqNum;                // When Seq# supported only
      UINT32         iPhyRecNum;             // When Phy Rec#s supported only
      UINT16         bRecChanged;            // Used by Delayed Updates Cur
      BOOL16         bSeqNumChanged;         // Not used
      BOOL16         bDeleteFlag;            // When soft delete supported only
   } RECProps;
typedef RECProps far *pRECProps;


//============================================================================
//                   General properties  DbiGetProp/DbiSetProp
//============================================================================


// Cursor properties
// General

#define  curMAXPROPS         0x00050000l   //ro UINT16   , Number of defined properties
#define  curTABLENAME        0x00050001l   //ro pTBLNAME , Table name
#define  curTABLETYPE        0x00050002l   //ro pDBINAME , Table type
#define  curTABLELEVEL       0x00050003l   //ro UINT16   , Table level 1..n
#define  curFILENAME         0x00050004l   //ro pPATH    , Full filename
#define  curXLTMODE          0x00050005l   //rw XLTMode  , Translate mode
#define  curSEQREADON        0x00050006l   //rw BOOL     , Sequential read mode hint ON
#define  curONEPASSON        0x00050007l   //rw BOOL     , One pass mode hint ON
#define  curUPDATETS         0x00050008l   //ro TIMESTAMP, Update timestamp
#define  curSOFTDELETEON     0x00050009l   //rw BOOL     , Softdelete ON
#define  curLANGDRVNAME      0x0005000Al   //ro pCHAR    , Symbolic language drv. name
#define  curCURSORNAME       0x0005000Bl   //ro pCHAR    , name of the cursor
#define  maxcurPROPS      11               // keep in sync when adding cursor properties

// Paradox specific
#define  curPDXMAXPROPS      0x01050000l   //ro UINT16   , Number of defined properties
#define  maxcurPDXPROPS   0

// DBase specific
#define  curDBMAXPROPS       0x02050000l   //ro UINT16   , Number of defined properties
#define  curINEXACTON        0x02050001l   //rw BOOL     , InExact match ON
#define  curSHAREMODE        0x02050002l   //rw DBIShareMode, Share mode
#define  maxcurDBPROPS    2

// Text Driver specific
#define  curDELIMITALLFLDS   0x03050000l   //rw BOOL , Delimit all fields.
#define  curUSESCHEMAFILE    0x03050001l   //rw BOOL , read schema from a file

// SQL Driver specific
#define  curUPDLOCKMODE      0x04050000l   //rw UPDLockMode ,Update lock mode
#define  curNATIVEHNDL       0x04050001l   //ro pBYTE       ,Native cursor handle
#define  curMAXROWS          0x04050002l   //rw INT32       ,Max # of rows to fetch from server
#define  curGETEXTENDEDINFO  0x04050003l   //rw BOOL , Get RINT info etc.

// Delayed Updates Specific.
#define  curDELAYUPDRECSTATE        0x05050001l  // ro DELYUPDCbDesc
#define  curDELAYUPDABORTRESTORE    0x05050002l  // rw BOOL, restore state
                                                 // when commit is aborted.
#define  curDELAYUPDDISPLAYOPT      0x05050003l  // rw UINT16, view records
                                                 // with specific update stat
#define  curDELAYUPDGETOLDRECORD    0x05050004l  // rw BOOL, get un-modified
                                                 // rec buf for modified recs
#define  curDELAYUPDNUMUPDATES      0x05050005l  // ro INT32, num of updates
#define  curDELAYUPDUSECALLBACK     0x05050006l  // rw BOOL, callback usr.

// Driver properties
// General
#define  drvMAXPROPS         0x00030000l   //ro UINT16   , Number of defined properties
#define  drvDRIVERTYPE       0x00030002l   //ro pDBINAME , Driver type
#define  drvDRIVERVERSION    0x00030003l   //ro UINT16   , Driver version
#define  maxdrvPROPS      2                // keep in sync when adding driver properties

// Database properties
// General
#define  dbMAXPROPS         0x00040000l   //ro UINT16   , Number of defined properties
#define  dbDATABASENAME     0x00040001l   //ro pDBINAME , Database name/alias
#define  dbDATABASETYPE     0x00040002l   //ro pDBINAME , Database type
#define  dbDEFAULTDRIVER    0x00040003l   //rw pDBINAME , Default Driver name
#define  dbPARAMFMTQMARK    0x00040004l   //rw BOOL     , Stmt param marker fmt = ?
#define  dbUSESCHEMAFILE    0x00040005l   //rw BOOL , for text driver only.
#define  maxdbPROPS       16              // keep in sync when adding ANY db properties

// SQL Driver specific
#define  dbASYNCSUPPORT     0x04040000l   //ro BOOL     , Async. qry exec support
#define  dbPROCEDURES       0x04040001l   //ro BOOL     , Stored procedure support
#define  dbDEFAULTTXNISO    0x04040002l   //ro eXILType , Default transaction iso level
#define  dbNATIVEHNDL       0x04040003l   //ro pBYTE    , Native connection handle
#define  dbNATIVEPASSTHRUHNDL 0x04040004l //ro pBYTE    , Native passthru connection handle
#define  dbSERVERVERSION    0x04040005l   //ro UINT16   , Major server version number
#define  dbBATCHCOUNT       0x04040006l   //rw UINT16   , Batch modification count before auto-commit
#define  dbTRACEMODE        0x04040007l   //rw UINT16   , Trace mode
#define  dbCURRENTUSERNAME  0x04040008l   //ro UINT16   , Current User Name
#define  dbOWNERQUALIFIED   0x04040009l   //ro BOOL     , For SQL Link Drivers - does this driver support
#define  dbQUOTECHAR        0x0404000Al   //ro CHAR     , Quote character for quoting Object Names

// Session properties
// General
#define  sesMAXPROPS         0x00020000l   //ro UINT16   , Number of defined properties
#define  sesSESSIONNAME      0x00020001l   //ro pDBINAME , Name of seesion
#define  sesNETFILE          0x00020002l   //rw pCHAR    , Full filename of netfile (MAXPATHLEN)
#define  sesCFGMODE          0x00020003l   //rw CFGMode, Mode of configuration file.
#define  sesDEFAULTDRIVER    0x00020004l   //rw pDBINAME, default driver name
#define  sesGROUPNAME        0x00020005l   //rw pCHAR, Security - user group name (dBASE)
#define  sesUSERNAME         0x00020006l   //rw pCHAR, User Name
#define  sesUSERPASSWORD     0x00020007l   //rw pCHAR, User password
#define  sesSECFILENAME      0x00020008l   //rw pCHAR, Location of dBASE Security file
#define  sesDRNAME           0x00020009l   //rw pCHAR, Repository Database Name
#define  maxsesPROPS      9                // keep in sync when adding session properties

// System properties
// General
#define   sysMAXPROPS        0x00010000l   // ro UINT16  , Number of defined properties
#define   sysLOWMEMUSAGE     0x00010001l   // ro UINT16  , Low memory usage in (k)
#define   maxsysPROPS     1                // keep in sync when adding system properties

// Statement properties
// General
#define  stmtMAXPROPS        0x00060001l   //ro UINT16      Number of defined properties
#define  stmtPARAMETERCOUNT  0x00060002l   //ro UINT16      Count of parameters
#define  stmtPARAMETERDESCS  0x00060003l   //ro aFLDDesc    Array of parameters
#define  stmtLANGDRVNAME     0x00060004l   //ro pCHAR       Symbolic language drv. name
#define  stmtUNIDIRECTIONAL  0x00060010l   //rw BOOL        Cursor Unidirectional
#define  stmtCURSORNAME      0x00060011l   //rw pCHAR       Cursor name
#define  stmtNEWCONNECT      0x00060012l   //rw BOOL        Stmt on new connection
#define  stmtNATIVEHNDL      0x00060013l   //ro pBYTE       Native statement handle
#define  stmtROWCOUNT        0x00060014l   //ro UINT32      Rows effected by a stmt
#define  maxstmtPROPS     31               // keep in sync when adding ANY stmt properties

// specific to QBE or local SQL
#define  stmtANSTYPE         0x00060020l   //rw pBYTE       Table Type of Answer set
#define  stmtLIVENESS        0x00060021l   //rw LIVENESS    Preference for canned/live answers
#define  stmtQRYMODE         0x00060022l   //rw QryEvalMode Execute on server or local or either
#define  stmtBLANKS          0x00060023l   //rw BOOL        True if blanks to be regarded as zeros.
#define  stmtDATEFORMAT      0x00060024l   //rw FMTDate     Date format
#define  stmtNUMBERFORMAT    0x00060025l   //rw FMTNumber   Number format
#define  stmtAUXTBLS         0x00060026l   //rw BOOL        True if QBE to create CHANGED, etc.
#define  stmtTBLVECTOR       0x00060027l   //ro UINT16      Vector of tables generated by query.
#define  stmtALLPROPS        0x00060028l   //rw QueryLowProps
#define  stmtALLPROPSSIZE    0x00060029l   //rw INT16       size of QueryLowProps
#define  stmtANSNAME         0x00060030l   //rw pBYTE       Answer Table Name.
#define  stmtCONSTRAINED     0x00060031l   //rw BOOL        Constrain input
#define  stmtFIELDDESCS      0x00060032l   //rw pFLDDESC    Answer FieldDescs
#define  stmtCURPROPS        0x00060033l   //rw pCURProps    Answer Curprops
#define  stmtDEFDBNAME       0x00060034l   //rw pCURProps    Answer Curprops
#define  stmtXLTMODE         0x00060035l   //rw XLTMode      Xltmode
#define  stmtINSTBLNAME      0x00060036l   //ro DBITBLNAME  INSERT table's name
#define  stmtINSERRTBLNAME   0x00060037l   //ro DBITBLNAME  ERRINS table's name
#define  stmtUPDTBLNAME      0x00060038l   //ro DBITBLNAME  UPDATE table's name
#define  stmtUPDERRTBLNAME   0x00060039l   //ro DBITBLNAME  ERRUPD table's name
#define  stmtDELTBLNAME      0x00060040l   //ro DBITBLNAME  DELETED table's name
#define  stmtDELERRTBLNAME   0x00060041l   //ro DBITBLNAME  ERRDEL table's name
#define  stmtCANNEDREADONLY   0x00060042l  //ro BOOL canned answers are readonly


//============================================================================
//                    Transactions
//============================================================================

// Transaction support

typedef enum                           // Transaction isolation levels
   {
      xilDIRTYREAD,                    // Uncommitted changes read
      xilREADCOMMITTED,                // Committed changes, no phantoms
      xilREPEATABLEREAD                // Full read repeatability
   } eXILType;

typedef enum                           // Transaction end control
   {
      xendCOMMIT,       // Commit transaction
      xendCOMMITKEEP,   // Commit transaction, keep cursors
      xendABORT         // Rollback transaction
   } eXEnd;

typedef enum                           // Transaction end control
   {
      xsINACTIVE,       // Transaction inactive
      xsACTIVE          // Transaction active
   } eXState;

typedef struct
   {
      eXState        exState;          // xsActive, xsInactive
      eXILType       eXIL;             // Xact isolation level
      UINT16         uNests;           // Xact children
   } XInfo, *pXInfo;


//============================================================================
//                         BookMark compares
//============================================================================

typedef enum                              // Compare BookMark Results
   {
      CMPLess           = -1,             // Bkm1 < Bkm2
      CMPEql            = 0,              // BookMarks are exactly the same
      CMPGtr            = 1,              // Bkm1 > Bkm2
      CMPKeyEql         = 2               // Only Bkm1.key_val = Bkm2.key_val
   } CMPBkMkRslt;
typedef CMPBkMkRslt far *pCMPBkMkRslt;


//===========================================================================
//                            Key searches
//===========================================================================

typedef enum  {                           // Search condition for keys
      keySEARCHEQ       = 0,                 // =
      keySEARCHGT       = 1,                 // >
      keySEARCHGEQ      = 2,                 // >=
    } DBISearchCond;


//============================================================================
//                      Create/Restructure descriptor
//============================================================================

typedef enum                              // Create/Restruct Operation type
   {
      crNOOP         = 0,
      crADD          = 1,                    // Add a new element.
      crCOPY         = 2,                    // Copy an existing element.
      crMODIFY       = 3,                    // Modify an element.
      crDROP         = 4,                    // Removes an element.
      crREDO         = 5,                    // Reconstruct an element.
      crTABLE        = 6,                    // Not used
      crGROUP        = 7,                    // Not used
      crFAMILY       = 8,                    // Not used
      crDONE         = 9,                    // Used internally
      crDROPADD      = 10                    // Used internally
   } CROpType;

typedef CROpType far *pCROpType;

typedef struct                            // Create/Restruct Table descr
   {
      DBITBLNAME     szTblName;                 // TableName incl. optional path & ext
      DBINAME        szTblType;                 // Driver type (optional)
      DBIPATH        szErrTblName;              // Error Table name (optional)
      DBINAME        szUserName;                // User name (if applicable)
      DBINAME        szPassword;                // Password (optional)
      BOOL16         bProtected;                // Master password supplied in szPassword
      BOOL16         bPack;                     // Pack table (restructure only)

      UINT16         iFldCount;                 // Number of field defs supplied
      pCROpType      pecrFldOp;                 // Array of field ops
      pFLDDesc       pfldDesc;                  // Array of field descriptors

      UINT16         iIdxCount;                 // Number of index defs supplied
      pCROpType      pecrIdxOp;                 // Array of index ops
      pIDXDesc       pidxDesc;                  // Array of index descriptors

      UINT16         iSecRecCount;              // Number of security defs supplied
      pCROpType      pecrSecOp;                 // Array of security ops
      pSECDesc       psecDesc;                  // Array of security descriptors

      UINT16         iValChkCount;              // Number of val checks
      pCROpType      pecrValChkOp;              // Array of val check ops
      pVCHKDesc      pvchkDesc;                 // Array of val check descs

      UINT16         iRintCount;                // Number of ref int specs
      pCROpType      pecrRintOp;                // Array of ref int ops
      pRINTDesc      printDesc;                 // Array of ref int specs

      UINT16         iOptParams;                // Number of optional parameters
      pFLDDesc       pfldOptParams;             // Array of field descriptors
      pBYTE          pOptData;                  // Optional parameters

   } CRTblDesc;

typedef CRTblDesc far *pCRTblDesc;


//============================================================================
//                      Batch
//============================================================================


typedef struct                            // Batch Table definition struct
   {
      hDBIDb         hDb;                       // Database
      DBIPATH        szTblName;                 // Table name
      DBINAME        szTblType;                 // Optional Driver type
      DBINAME        szUserName;                // Optional User name
      DBINAME        szPassword;                // Optional Pass word
   } BATTblDesc;

typedef BATTblDesc far *pBATTblDesc;

typedef enum                   // Batch mode for DBIBatchMove
   {
      batAPPEND =       0,
      batUPDATE =       1,
      batAPPENDUPDATE = 2,
      batSUBTRACT =     3,
      batCOPY     =     4
   } eBATMode;

typedef enum                              // Sort Order
   {
      sortASCEND     = 0,                    // ascending (default)
      sortDESCEND    = 1,                    // descending
   } SORTOrder;

typedef SORTOrder far *pSORTOrder;

typedef INT16 (DBIFN  * pfSORTCompFn)     // pntr to client compare fn
   (
      pVOID          pLdObj,                 // Language driver, if needed
      pVOID          pValue1,                // first value
      pVOID          pValue2,                // second value
      UINT16         iLen                    // Length, if needed
   );
   // Returns: -1 if (Value 1 < Value 2),
   //           0 if (Value 1 == Value 2),
   //          +1 if (Value 1 > Value 2)


//===========================================================================
//                      Types/Structs for Capabilities
//===========================================================================

typedef enum                           // Driver Category
   {
      drvFILE           = 1,              // File based (Paradox, xBASE)
      drvOTHERSERVER    = 2,              // Other kind of server (IMS ?)
      drvSQLBASEDSERVER = 3               // SQL Based Server
   } DRVCat;

typedef enum                           // Config info & Optional Parameters
   {
      opDBCREATE        = 3,
      opDBOPEN          = 4,
      opTBLCREATE       = 5,
      opTBLOPEN         = 6
   } OPType;

typedef struct                         // Driver Description/Capabilities
   {
      DBINAME        szType;                 // Symbolic name to identify
      DBINAME        szText;                 // Descriptive text
      DRVCat         edrvCat;                // Driver category
      BOOL16         bTrueDb;                // Supports True Db concept
      DBINAME        szDbType;               // Db Type to be used
      BOOL16         bMultiUser;             // Supports Multi-user access
      BOOL16         bReadWrite;             // Read Write or Read-only
      BOOL16         bTrans;                 // Supports Transactions
      BOOL16         bPassThruSQL;           // Supports Pass-thru SQL
      BOOL16         bLogIn;                 // Requires explicit login
      BOOL16         bCreateDb;              // Can reate a Database
      BOOL16         bDeleteDb;              // Can drop database
      BOOL16         bCreateTable;           // Can create a Table
      BOOL16         bDeleteTable;           // Can delete a Table
      BOOL16         bMultiplePWs;           // Multiple passwords
      UINT16         iDriverVersion;         // Driver version 1..n
      BOOL16         bSQLRowid;              // Supports SQL rowid
      UINT16         iUnUsed[15];
   } DRVType;
typedef DRVType far *pDRVType;


typedef struct                            // Table Capabilities
   {
      UINT16         iId;                    // Id of Table Type
      DBINAME        szName;                 // Symbolic name; eg "dBASE"
      DBINAME        szText;                 // Descriptive text
      DBINAME        szFormat;               // Format; eg "HEAP"
      BOOL16         bReadWrite;             // User can Read/Write
      BOOL16         bCreate;                // Can create new tables
      BOOL16         bRestructure;           // Can restructure this table
      BOOL16         bValChecks;             // Val Checks can be specified
      BOOL16         bSecurity;              // Can be protected
      BOOL16         bRefIntegrity;          // Can participate in ref integrity
      BOOL16         bPrimaryKey;            // Supports primary key concept
      BOOL16         bIndexing;              // Can have other indexes
      UINT16         iFldTypes;              // Number of Phy Field types supported
      UINT16         iMaxRecSize;            // Max record size
      UINT16         iMaxFldsInTable;        // Max fields in a table
      UINT16         iMaxFldNameLen;         // Maximum field name length
      UINT16         iTblLevel;              // Driver dependent table level (version)
      UINT16         iUnUsed[16];
    } TBLType;
typedef TBLType far *pTBLType;


typedef struct
   {
      UINT16         iId;                    // Id of Index Type
      DBINAME        szName;                 // Symbolic name
      DBINAME        szText;                 // Descriptive text
      DBINAME        szFormat;               // Optional format(BTREE, HASH etc)
      BOOL16         bComposite;             // Supports composite keys
      BOOL16         bPrimary;               // True, if primary index
      BOOL16         bUnique;                // True, No duplicates supported
      BOOL16         bKeyDescending;         // If whole key can be descending
      BOOL16         bFldDescending;         // Field level descending
      BOOL16         bMaintained;            // Supports maintained option
      BOOL16         bSubset;                // Supports Subset expression
      BOOL16         bKeyExpr;               // If Key can be expres
      BOOL16         bCaseInsensitive;       // Supports Caseinsensitive indexes
      UINT16         iUnUsed[16];
   } IDXType;
typedef IDXType far *pIDXType;

typedef struct
   {
      UINT16         iId;                    // Id of Field Type
      DBINAME        szName;                 // Symbolic name; eg "ALPHA"
      DBINAME        szText;                 // Descriptive text
      UINT16         iPhyType;               // Physical/Native type
      UINT16         iXltType;               // Default xlated type
      UINT16         iXltSubType;            // Default xlated sub type
      UINT16         iMaxUnits1;             // Max units allowed (1)
      UINT16         iMaxUnits2;             // Max units allowed (2)
      UINT16         iPhySize;               // Physical size in bytes (per unit)
      BOOL16         bRequired;              // Supports 'required' option
      BOOL16         bDefaultVal;            // Supports user specified 'default'
      BOOL16         bMinVal;                // Supports MIN Val constraint
      BOOL16         bMaxVal;                // Supports MAX Val constraint
      BOOL16         bRefIntegrity;          // Can participate in ref integrity
      BOOL16         bOtherChecks;           // Supports other kinds of checks
      BOOL16         bKeyed;                 // The field type can be keyed
      BOOL16         bMultiplePerTable;      // Table can have more than 1 of this type
      UINT16         iMinUnits1;             // Min units required (1)
      UINT16         iMinUnits2;             // Min units required (2)
      BOOL16         bCreateable;            // Type can be created
      DBINAME        szNativeName;           // Native (SQL) name used in DDL
      UINT16         iUnUsed[16];
    } FLDType;
typedef FLDType far *pFLDType;


typedef enum                           // PROP Validity
   {
      epropINVALID     = 0,            // Property is INVALID (not supported)
      epropCANGET      = 1,            // You can GET value of the property
      epropCANSET      = 2             // You can GET and SET value of the property
   } PROPValid;


//===========================================================================
//                               Call Backs
//===========================================================================


typedef enum                              // Call back type
   {
       cbGENERAL      = 0,                    // General purpose
       cbRESERVED1    = 1,                    //
       cbRESERVED2    = 2,                    //
       cbINPUTREQ     = 3,                    // Input requested
       cbRESERVED4    = 4,                    //
       cbRESERVED5    = 5,                    //
       cbBATCHRESULT  = 6,                    // Batch processing rslts
       cbRESERVED7    = 7,                    //
       cbRESTRUCTURE  = 8,                    // Restructure
       cbRESERVED9    = 9,                    //
       cbRESERVED10   = 10,                   //
       cbRESERVED11   = 11,                   //
       cbRESERVED12   = 12,                   //
       cbRESERVED13   = 13,                   //
       cbRESERVED14   = 14,                   //
       cbRESERVED15   = 15,                   //
       cbRESERVED16   = 16,                   //
       cbRESERVED17   = 17,                   //
       cbTABLECHANGED = 18,                   // Table changed notification
       cbRESERVED19   = 19,                   //
       cbCANCELQRY    = 20,                   // Allow user to cancel Query
       cbRESERVED21   = 21,                   //
       cbRESERVED22   = 22,                   //
       cbGENPROGRESS  = 23,                   // Generic Progress report.
       cbDBASELOGIN   = 24,                   // dBASE Login
       cbDELAYEDUPD   = 25,
       cbFIELDRECALC  = 26,                   // Field(s) recalculation
       cbTRACE        = 27,                   // Trace
       cbDBLOGIN      = 28,                   // Database login
       cbRESERVED29   = 29,                   //
       cbRESERVED30   = 30,                   //
       cbRESERVED31   = 31,                   //
       cbRESERVED32   = 32,                   //
       cbRESERVED33   = 33,                   //
       cbNBROFCBS     = 34                    // Number of cbs
   } CBType;
typedef CBType far *pCBType;

typedef enum                              // Call-back return type
   {
      cbrUSEDEF        = 0,                  // Take default action
      cbrCONTINUE      = 1,                  // Continue
      cbrABORT         = 2,                  // Abort the operation
      cbrCHKINPUT      = 3,                  // Input given
      cbrYES           = 4,                  // Take requested action
      cbrNO            = 5,                  // Do not take requested action
      cbrPARTIALASSIST = 6,                  // Assist in completing the job
      cbrSKIP          = 7,                  // Skip this operation
      cbrRETRY         = 8                   // Retry this operation
   } CBRType;
typedef CBRType far *pCBRType;

typedef CBRType (DBIFN  * pfDBICallBack)  // Call-back funtion pntr type
   (
      CBType         ecbType,                // Callback type
      UINT32         iClientData,            // Client callback data
      pVOID          pCbInfo                 // Call back info/Client Input
   );

typedef pfDBICallBack far *ppfDBICallBack;

// Progress callback structure
typedef struct
   {
      INT16         iPercentDone;          // Percentage done
      DBIMSG        szMsg;                 // Message to display
   } CBPROGRESSDesc;

typedef  CBPROGRESSDesc far * pCBPROGRESSDesc;

typedef enum   // type of delayed update object (delayed updates callback)
   {
      delayupdNONE        = 0,
      delayupdMODIFY      = 1,
      delayupdINSERT      = 2,
      delayupdDELETE      = 3
   } DelayUpdErrOpType;

typedef struct       // delayed updates callback info
   {
      DBIResult           iErrCode;
      DelayUpdErrOpType   eDelayUpdOpType;
      UINT16              iRecBufSize;  // Record size (physical record)
      pBYTE               pNewRecBuf;
      pBYTE               pOldRecBuf;
   } DELAYUPDCbDesc;

typedef enum        // type of restructure object (restructure callback)
   {
      restrNONE        = 0,
      restrNEWFLD      = 1,
      restrOLDFLD      = 2,
      restrNEWINDEX    = 3,
      restrOLDINDEX    = 4,
      restrNEWVCHK     = 5,
      restrOLDVCHK     = 6,
      restrNEWRINT     = 7,
      restrOLDRINT     = 8,
      restrNEWSEC      = 9,
      restrOLDSEC      = 10,
      restrNEWTABLE    = 11
   } RESTErrObjType;

typedef struct  {               //  Database login
   DBINAME        szDbName;             // Database name
   DBIOpenMode    eOpenMode;            // Open type desired
   DBIShareMode   eShareMode;           // Share type desired
   DBINAME        szPassword;           // Password
   BOOL           bCallbackToClose;     // Returned flag
   hDBIDb         hDb;                  // db handle
} CBDBLogin;
typedef CBDBLogin *pCBDBLogin;

typedef enum         // trace categories
   {
      traceUNKNOWN   = 0x0000,
      traceQPREPARE  = 0x0001,   // prepared query statements
      traceQEXECUTE  = 0x0002,   // executed query statements
      traceERROR     = 0x0004,   // vendor errors
      traceSTMT      = 0x0008,   // statement ops (i.e. allocate, free)
      traceCONNECT   = 0x0010,   // connect / disconnect
      traceTRANSACT  = 0x0020,   // transaction
      traceBLOB      = 0x0040,   // blob i/o
      traceMISC      = 0x0080,   // misc.
      traceVENDOR    = 0x0100,   // vendor calls
   } TRACECat;
typedef TRACECat *pTRACECat;

typedef struct       // trace callback info
   {
      TRACECat    eTraceCat;
      UINT16      uTotalMsgLen;
      CHAR        pszTrace[];
   } TRACEDesc;
typedef TRACEDesc *pTRACEDesc;

#if !defined(MIDL)
typedef struct       // restructure callback info
   {
      DBIResult      iErrCode;
      UINT16         iTblNum;
      UINT16         iObjNum;
      // for old objects iObjNum is the sequence or field number,
      // for new objects it is the order in CRTblDesc
      RESTErrObjType   eRestrObjType;
      union
         {
            FLDDesc     fldDesc;
            IDXDesc     idxDesc;
            VCHKDesc    vchkDesc;
            RINTDesc    rintDesc;
            SECDesc     secDesc;
         } uObjDesc;
   } RESTCbDesc;

#endif // !defined(MIDL)

typedef struct
{
      DBINAME        szKeyWord;       // Keyword to display
      DBIMSG         szHelp;          // Help String
} CBEntry;

#define  MAXCBENTRIES    4

typedef enum
{
    cbiMDXMISSING = 1,                  // MDX file missing request
    cbiPDXLOOKUP  = 2,                  // Lookup table missing
    cbiPDXBLOB    = 3,                  // Blob file missing
    cbiDBTMISSING = 4,                   // DBT file missing request
    cbiDRINPUT    = 5                   // DR Manager needs user input
}   CBInputId;                          // id's for input requests

typedef struct
{
   CBInputId      eCbInputId;             // Id for this input request
   INT16          iCount;                 // Number of entries
   INT16          iSelection;             // Selection 1..n  (In/Out)
   BOOL16         bSave;                  // Save this option  (In/Out)
   DBIMSG         szMsg;                  // Message to display
   CBEntry        acbEntry[MAXCBENTRIES]; // Entries
} CBInputDesc;

typedef struct    // dBASE login callback structure
{
   DBINAME  szUserName;            // Login name of user
   DBINAME  szGroupName;           // Group to log in to
   DBINAME  szUserPassword;        // User password
} CBLoginDesc;

typedef  CBLoginDesc far * pCBLoginDesc;


//============================================================================
//                         Basic Query Types
//============================================================================

typedef enum DBIQryLang
{
   qrylangUNKNOWN = 0,  // UNKNOWN (Error)
   qrylangQBE = 1,      // QBE
   qrylangSQL = 2       // SQL
} DBIQryLang;

//============================================================================
//                         Statement parameter information
//============================================================================

typedef enum
{
   indTRUNC = -2,         // Value has been truncated
   indNULL  = -1,         // Value is NULL
   indVALUE = 1,          //
} eINDValues;

#define INDLEN             sizeof (INT16)

typedef enum STMTParamType
{
   paramUNKNOWN    = 0, // UNKNOWN (Error)
   paramIN         = 1, // Input parameter
   paramOUT        = 2, // Output parameter
   paramINOUT      = 3, // Input/Output parameter
   paramRET        = 4  // Procedure (or function) return
} STMTParamType;

typedef enum STMTParamFormat
{
   paramfmtUNKNOWN = 0, // UNKNOWN (Error)
   paramfmtNUMBER  = 1, // Numbered parameter markers of the form ? or :n
   paramfmtNAME    = 2  // Named parameters markers of the form :name
} STMTParamFormat;


//============================================================================
//                         Properties For DbiQPrepareExt
//============================================================================

#define qprepNONE       0     // like DbiQPrepare
#define qprepFORUPDATE  0x1   // do extra work, anticipating UPDATE WHERE CURRENT


//============================================================================
//                      Date, Time, Number Formats
//============================================================================


typedef struct                         // Date Format
   {
      CHAR        cDecimalSeparator;         // Default "."
      CHAR        cThousandSeparator;        // Default ","
      INT8        iDecimalDigits;            // Default 2
      INT8        bLeadingZero;              // Default TRUE.
   } FMTNumber;
typedef FMTNumber far *pFMTNumber;

typedef struct
   {
      CHAR        szDateSeparator[4];        // Default "/"
      INT8        iDateMode;                 // 0 = MDY (Def), 1 = DMY, 2 = YMD
      INT8        bFourDigitYear;            // Write Year as 4 digits (FALSE)
      INT8        bYearBiased;               // On input add 1900 to year (TRUE)
      INT8        bMonthLeadingZero;         // Month with Leading Zero (TRUE)
      INT8        bDayLeadingZero;           // Day with Leading Zero (TRUE)
   } FMTDate;
typedef FMTDate far *pFMTDate;

typedef struct
   {
     CHAR         cTimeSeparator;            // Default ":"
     INT8         bTwelveHour;               // Represent as 12 Hour time (FALSE)
     CHAR         szAmString[6];             // Default Null (Only for 12 Hr)
     CHAR         szPmString[6];             // Default Null (Only for 12 Hr)
     INT8         bSeconds;                  // Show Seconds (TRUE)
     INT8         bMilSeconds;               // Show Milli Seconds (FALSE)
   } FMTTime;
typedef FMTTime far *pFMTTime;


typedef struct
{
    BYTE    iPrecision;          // 1..64 considered valid
    BYTE    iSignSpecialPlaces;  // sign:1, special:1, places:6
    BYTE    iFraction [32];      // bcd nibbles, 00..99 per byte, high nibble 1st
} FMTBcd;

typedef FMTBcd far * pFMTBcd;


//============================================================================
//                              Error Info
//============================================================================

typedef struct {                       // Error info
      DBIResult      iError;                 // Last error code returned
      DBIMSG         szErrCode;              // Error Code
      DBIMSG         szContext1;             // Context dependent info
      DBIMSG         szContext2;             // More context
      DBIMSG         szContext3;             // Even more context
      DBIMSG         szContext4;             // Yet some more context
    } DBIErrInfo;

typedef DBIErrInfo far *pDBIErrInfo;

// Error contexts (To be used with DbiGetErrorContext)

#define    ecTOKEN         1              // Token (For QBE)
#define    ecTABLENAME     3              // Table name
#define    ecFIELDNAME     4              // Field Name
#define    ecIMAGEROW      5              // Image Row (For QBE)
#define    ecUSERNAME      6              // eg, In lock conflicts, user involved
#define    ecFILENAME      7              // File Name
#define    ecINDEXNAME     8              // Index Name
#define    ecDIRNAME       9              // Directory Name
#define    ecKEYNAME       10             // Key Name
#define    ecALIAS         11             // Alias
#define    ecDRIVENAME     12             // Drive name ('c:')
#define    ecNATIVECODE    13             // Native error code
#define    ecNATIVEMSG     14             // Native error message
#define    ecLINENUMBER    15             // Line Number
#define    ecCAPABILITY    16             // Capability
#define    ecCDRNAME       17             // Client Data Repository Name
#define    ecUSERERRMSG    18             // User Defined error msg
#define    ecDROBJNAME     19             // Data Repository Object Name
#define    ecINTERNALLIMIT 20             // Internal limit

//============================================================================
//                  Schema info structures
//============================================================================
//                    Database descriptor
//============================================================================


// DbiOpenDatabaseList
typedef struct                               // A given Database Description
   {
      DBINAME        szName;                 // Logical name (Or alias)
      DBINAME        szText;                 // Descriptive text
      DBIPATH        szPhyName;              // Physical name/path
      DBINAME        szDbType;               // Database type
   } DBDesc;
typedef DBDesc far *pDBDesc;


//============================================================================
//                    User info descriptor
//============================================================================


//DbiOpenUserList()
typedef struct                               // User description
   {
      DBIUSERNAME    szUserName;
      UINT16         iNetSession;            // Net level session number
      UINT16         iProductClass;          // Product class of user
      CHAR           szSerialNum[22];        // Serial number
   } USERDesc;
typedef USERDesc far *pUSERDesc;

//============================================================================
//                    Table descriptor
//============================================================================


// DbiOpenTableList()
typedef struct                            // Table description (Base)
   {
      DBITBLNAME     szName;                 // Table name(No extension or Dir)
      DBITBLNAME     szFileName;             // File name
      DBIEXT         szExt;                  // File extension
      DBINAME        szType;                 // Driver type
      DBIDATE        dtDate;                 // Date on the table
      TIME           tmTime;                 // Time on the table
      UINT32         iSize;                  // Size in bytes
      BOOL16         bView;                  // If this a view
      BOOL16         bSynonym;               // If this is a synonym
   } TBLBaseDesc;
typedef TBLBaseDesc far *pTBLBaseDesc;

typedef struct                            // Table description (Extended part)
   {
      DBINAME        szStruct;               // Physical structure
      UINT16         iRestrVersion;          // Version #
      UINT16         iRecSize;               // Physical record size
      UINT16         iFields;                // Number of fields
      UINT16         iIndexes;               // Number Indexes
      UINT16         iValChecks;             // Number of field validity checks
      UINT16         iRintChecks;            // Number of ref. integrity checks
      UINT32         iRecords;               // Number of records in table
      BOOL16         bProtected;             // If the table is prot
      BOOL16         bValidInfo;             // Info available for this table
                                             // FALSE, if all or part of the
                                             // extended data not available.
   } TBLExtDesc;
typedef TBLExtDesc far *pTBLExtDesc;

typedef struct                            // Table description (Base + Ext)
   {
      TBLBaseDesc    tblBase;                // Base      description
      TBLExtDesc     tblExt;                 // Extended  description
   } TBLFullDesc;
typedef TBLFullDesc far *pTBLFullDesc;

//============================================================================
//                    File descriptor
//============================================================================


// DbiOpenFileList()
typedef struct                            // File description
   {
      DBIPATH        szFileName;             // File name (No Dir or ext)
      DBIEXT         szExt;                  // Extension
      BOOL16         bDir;                   // True, if directory
      UINT32         iSize;                  // File size in bytes
      DBIDATE        dtDate;                 // Date on the file
      TIME           tmTime;                 // Time on the file
   } FILEDesc;
typedef FILEDesc far *pFILEDesc;

//======================================================================
//            Stored Procedure and Stored Procedure Param descriptor
//======================================================================

typedef struct {
      DBISPNAME      szName;
      DBIDATE        dtDate;
      TIME           tmTime;
   } SPDesc;
typedef SPDesc far *pSPDesc;

typedef struct {
      UINT16         uParamNum;
      DBINAME        szName;
      STMTParamType  eParamType;
      UINT16         uFldType;
      UINT16         uSubType;
      INT16          iUnits1;
      INT16          iUnits2;
      UINT16         uOffset;
      UINT16         uLen;
      UINT16         uNullOffset;
   } SPParamDesc;
typedef SPParamDesc far *pSPParamDesc;

//======================================================================
//            Function and Function Argument Descriptors
//======================================================================
typedef enum
   {
     fnAVG,
     fnCOUNT,
     fnMIN,
     fnMAX,
     fnSUM,
     fnSTDDEV,
     fnVARIANCE,
     fnABS,
     fnCEIL,
     fnCOS,
     fnCOSH,
     fnEXP,
     fnFLOOR,
     fnLN,
     fnLOG,
     fnMOD,
     fnPOWER,
     fnROUND,
     fnSIGN,
     fnSIN,
     fnSINH,
     fnSQRT,
     fnTAN,
     fnTANH,
     fnTRUNC,
     fnCHR,
     fnCONCAT,
     fnINITCAP,
     fnLOWER,
     fnLPAD,
     fnLTRIM,
     fnNLS_INITCAP,
     fnNLS_LOWER,
     fnNLS_UPPER,
     fnREPLACE,
     fnRPAD,
     fnRTRIM,
     fnSOUNDEX,
     fnSUBSTR,
     fnSUBSTRB,
     fnTRANSLATE,
     fnUPPER,
     fnASCII,
     fnINSTR,
     fnINSTRB,
     fnLENGTH,
     fnLENGTHB,
     fnNLSSORT,
     fnADD_MONTHS,
     fnLAST_DAY,
     fnMONTHS_BETWEEN,
     fnNEW_TIME,
     fnNEXT_DAY,
     fnSYSDATE,
     fnCONVERT,
     fnTO_CHAR,
     fnTO_DATE,
     fnTO_MULTI_BYTE,
     fnTO_NUMBER,
     fnTO_SINGLE_BYTE,
     fnUID,
     fnUSER,
     fnORACLEMISC,
     fnACOS,
     fnASIN,
     fnATAN,
     fnATN2,
     fnCOT,
     fnDEGREES,
     fnLOG10,
     fnPI,
     fnRADIANS,
     fnRAND,
     fnTEXTPTR,
     fnTEXTVALID,
     fnCHARINDEX,
     fnDIFFERENCE,
     fnPATINDEX,
     fnREPLICATE,
     fnREVERSE,
     fnRIGHT,
     fnSPACE,
     fnSTR,
     fnSTUFF,
     fnCOL_NAME,
     fnCOL_LENGTH,
     fnDATALENGTH,
     fnDB_ID,
     fnDB_NAME,
     fnHOST_ID,
     fnHOST_NAME,
     fnINDEX_COL,
     fnOBJECT_ID,
     fnOBJECT_NAME,
     fnUSER_ID,
     fnUSER_NAME,
     fnLEFT,
     fnLOCATE,
     fnTRUNCATE,
     fnCURTIME,
     fnDAYNAME,
     fnDAYOFMONTH,
     fnDAYOFWEEK,
     fnDAYOFYEAR,
     fnHOUR,
     fnMINUTE,
     fnMONTH,
     fnMONTHNAME,
     fnNOW,
     fnQUARTER,
     fnSECOND,
     fnWEEK,
     fnYEAR,

   } DBISTDFuncs;

#define     fnSCALARS_ALLOW_CONSTANTS            0x0001, // fn args may contain refeences to constants
#define     fnSCALARS_ALLOW_COLUMNS              0x0002, // fn args may contain refeences to columns
#define     fnSCALARS_ALLOW_PARAMETERS           0x0004, // fn args may contain refeences to parameters
#define     fnSCALARS_ALLOW_FUNCTIONS            0x0008, // fn args may contain refeences to functions
#define     fnSCALARS_ALLOW_USER_DEFINED_FUNCS   0x0010, // fn args may contain refeences to user defined functions
#define     fnSCALARS_ALLOW_SUBQUERIES           0x0020, // fn args can contain subqueries
#define     fnSCALARS_ALLOW_CORRELATION          0x0040  // fn subqueries can be correlated

typedef enum
   {
   fnListINCL_USER_DEF         = 0x0001   // include user-defined functions
   } DBIFUNCOpts;

typedef struct {
      DBINAME        szName;           // Function name
      CHAR           szDesc[255];      // Short description
      UINT16         uOverload;        // Number of function overloads
      DBISTDFuncs    eStdFn;           // Corresponds to DBI standard function
   } DBIFUNCDesc;
typedef DBIFUNCDesc far *pDBIFUNCDesc;

typedef struct {
      UINT16         uArgNum;          // Argument position num; 0 for fn return
      UINT16         uFldType;         // Field type
      UINT16         uSubType;         // Field subtype (if applicable)
      UINT16         ufuncFlags;       // Function flags
   } DBIFUNCArgDesc;

typedef DBIFUNCArgDesc far *pDBIFUNCArgDesc;

//============================================================================
//                   Configuration Info Descriptor
//============================================================================

typedef enum
   {
     cfgPersistent  = 0,   // Persistent only
     cfgSession     = 1,   // Session relative only
     cfgAll         = 2    // All (union)
   } CFGMode;


// DbiOpenCfgInfoList()
typedef struct                            // Config description
   {
      DBINAME        szNodeName;                // Node name
      CHAR           szDescription[DBIMAXSCFLDLEN]; // Node description
      UINT16         iDataType;                 // Value type
      CHAR           szValue[DBIMAXSCFLDLEN];   // Value
      BOOL16         bHasSubnodes;              // True, if not leaf node
   } CFGDesc;
typedef CFGDesc far *pCFGDesc;


//============================================================================
//                    Family descriptor
//============================================================================


typedef enum                              // Family member types
   {
      fmlUNKNOWN     = 0,
      fmlTABLE       = 1,
      fmlINDEX       = 2,
      fmlFORM        = 3,
      fmlREPORT      = 4,
      fmlVALCHECK    = 5,
      fmlSECINDEX    = 6,
      fmlSECINDEX2   = 7,
      fmlBLOBFILE    = 8
   } FMLType;
typedef FMLType far *pFMLType;

// DbiOpenFamilyList()
typedef struct {                          // Family record structure
      DBINAME        szName;                 // Member name (documentary)
      UINT16         iId;                    // Id (if applicable)
      FMLType        eType;                  // Member type
      DBIPATH        szFileName;             // File name of member
   } FMLDesc;
typedef FMLDesc far *pFMLDesc;


//============================================================================
//                    Language driver descriptor
//============================================================================


#define DBIOEM_CP                    1      // (dos)
#define DBIANSI_CP                   2      // (win)
#define DBIOS2_CP                    3
/* UNIX etc. */
#define DBISUNOS_CP                  4
#define DBIVMS_CP                    5
#define DBIHPUX_CP                   6
#define DBIULTRIX_CP                 7
#define DBIAIX_CP                    8
#define DBIAUX_CP                    9
#define DBIXENIX_CP                 10
#define DBIMAC_CP                   11
#define DBINEXT_CP                  12

// DbiOpenLdList()
typedef struct                         // Lang Driver description
   {
      DBINAME        szName;               // Driver symbolic name
      DBINAME        szDesc;               // Description
      UINT16         iCodePage;
      UINT16         PrimaryCpPlatform;
      UINT16         AlternateCpPlatform;
   } LDDesc;
typedef LDDesc far *pLDDesc;

//============================================================================
//                    Lock descriptor
//============================================================================

// Lock types in LOCKDesc:

#define lckRECLOCK         0                    // Normal Record lock (Write)
#define lckRRECLOCK        1                    // Special Pdox Record lock (Read)
#define lckGROUPLOCK       2                    // Pdox Group lock
#define lckIMGAREA         3                    // Pdox Image area
#define lckTABLEREG        4                    // Table registration/Open (No lock)
#define lckTABLEREAD       5                    // Table Read lock
#define lckTABLEWRITE      6                    // Table Write lock
#define lckTABLEEXCL       7                    // Table Exclusive lock
#define lckUNKNOWN         9                    // Unknown lock


// DbiOpenLockList()
typedef struct                               // Lock Description
   {
      UINT16         iType;                     // Lock type (0 for rec lock)
      DBIUSERNAME    szUserName;                // Lock owner
      UINT16         iNetSession;               // Net level Session number
      UINT16         iSession;                  // Idapi session#, if our lock
      UINT32         iRecNum;                   // If a record lock
      UINT16         iInfo;                     // Info for table locks
   } LOCKDesc;
typedef LOCKDesc far *pLOCKDesc;

//============================================================================
//                    Filter description
//============================================================================

typedef enum {

      // Relational operators.

      canNOTDEFINED        = 0,        //                                  (*)
      canISBLANK,                      // CANUnary;  is operand blank.     (*)
      canNOTBLANK,                     // CANUnary;  is operand not blank. (*)

      canEQ,                           // CANBinary, CANCompare; equal.                (*)
      canNE,                           // CANBinary; NOT equal.            (*)
      canGT,                           // CANBinary; greater than.         (*)
      canLT,                           // CANBinary; less than.            (*)
      canGE,                           // CANBinary; greater or equal.     (*)
      canLE,                           // CANBinary; less or equal.        (*)

      // Logical operators.

      canNOT,                          // CANUnary; NOT                    (*)
      canAND,                          // CANBinary; AND                   (*)
      canOR,                           // CANBinary; OR                    (*)

      // Operators identifing leaf operands.

      canTUPLE,                        // CANUnary; Entire record is operand.
      canFIELD,                        // CANUnary; operand is field       (*)
      canCONST,                        // CANUnary; operand is constant    (*)

      // Arithmetic operators.

      canMINUS,                        // CANUnary;  minus.
      canADD,                          // CANBinary; addition.
      canSUB,                          // CANBinary; subtraction.
      canMUL,                          // CANBinary; multiplication.
      canDIV,                          // CANBinary; division.
      canMOD,                          // CANBinary; modulo division.
      canREM,                          // CANBinary; remainder of division.

      // Aggregate type operators.

      canSUM,                          // CANBinary, accumulate sum of.
      canCOUNT,                        // CANBinary, accumulate count of.
      canMIN,                          // CANBinary, find minimum of.
      canMAX,                          // CANBinary, find maximum of.
      canAVG,                          // CANBinary, find average of.

      // Miscellaneous operators.

      canCONT,                         // CANBinary; provides a link between two
                                       // expression subtrees of a tree.
      canUDF,                          // CANBinary; invokes a User defined fn

      canCONTINUE,                     // CANUnary; Stops evaluating records
                                       // when operand evaluates to false (forward sequential access only)

      canLIKE                          // CANCompare, extended binary compare       (*)

   } CANOp;

typedef CANOp far *pCANOp;

typedef enum {                      // Node Class

      nodeNULL          = 0,           // Null node                  (*)
      nodeUNARY,                       // Node is a unary            (*)
      nodeBINARY,                      // Node is a binary           (*)
      nodeCOMPARE,                     // Node is a compare          (*)
      nodeFIELD,                       // Node is a field            (*)
      nodeCONST,                       // Node is a constant         (*)
      nodeTUPLE,                       // Node is a record
      nodeCONTINUE,                    // Node is a continue node    (*)
      nodeUDF                          // Node is a UDF node
   } NODEClass;

// NODE definitions including misc data structures
//-------------------------------------------------

typedef struct {                    // Header part common to all     (*)
      NODEClass      nodeClass;
      CANOp          canOp;
   } CANHdr;
typedef CANHdr far *pCANHdr;

typedef struct {                    // Unary Node                    (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iOperand1;        // Byte offset of Operand node
   } CANUnary;
typedef CANUnary far * pCANUnary;

typedef struct {                    // Binary Node                   (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iOperand1;        // Byte offset of Op1
      UINT16         iOperand2;        // Byte offset of Op2
   } CANBinary;
typedef CANBinary far * pCANBinary;

typedef struct {                    // Field
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iFieldNum;
      UINT16         iNameOffset;      // Name offset in Literal pool
   } CANField;
typedef CANField far * pCANField;

typedef struct {                    // Constant
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iType;            // Constant type.
      UINT16         iSize;            // Constant size. (in bytes)
      UINT16         iOffset;          // Offset in the literal pool.
   } CANConst;
typedef CANConst far * pCANConst;

typedef struct {                    // Tuple (record)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iSize;            // Record size. (in bytes)
   } CANTuple;
typedef CANTuple far * pCANTuple;

typedef struct {                    // Break Node                    (*)
      NODEClass      nodeClass;
      CANOp          canOp;
      UINT16         iContOperand;     // Continue if operand is true.
                                       // else stop evaluating records.
   } CANContinue;
typedef CANContinue far * pCANContinue;

typedef struct {                    // Extended compare Node (text fields) (*)
      NODEClass      nodeClass;
      CANOp          canOp;               // canLIKE, canEQ
      BOOL16         bCaseInsensitive;    // 3 val: UNKNOWN = "fastest", "native"
      UINT16         iPartialLen;         // Partial fieldlength (0 is full length)
      UINT16         iOperand1;           // Byte offset of Op1
      UINT16         iOperand2;           // Byte offset of Op2
   } CANCompare;
typedef CANCompare far * pCANCompare;


//This is the node to be used to pass User defined functions
#define iLangSQL     0              // Common SQL dialect
#define iDbaseExpr   2              // This is also the driver ID for dBASE

typedef struct {                    // A user defined function
      NODEClass      nodeClass;
      CANOp          canOp;

      UINT16         iOffSzFuncName;   // Offset in literal pool to Function Name string(0 terminated)

      UINT16         iOperands;        // Byte offset of Operands (concatenated using canCONT)

      //The fields below are hints that specific drivers can make use of
      UINT16         iDrvDialect;      // Driver Dialect ID for UDF string supplied
      UINT16         iOffSzUDF;        // Offset in literal pool to UDF string (0 terminated)
   } CANUdf;
typedef CANUdf far * pCANUdf;

#if !defined(MIDL)
typedef union {
      CANHdr         canHdr;
      CANUnary       canUnary;
      CANBinary      canBinary;
      CANField       canField;
      CANConst       canConst;
      CANTuple       canTuple;
      CANContinue    canContinue;
      CANCompare     canCompare;
   } CANNode;
typedef CANNode far *pCANNode;
#endif // !defined(MIDL)

// Linear exression tree
//----------------------

# define CANEXPRVERSION   2

typedef struct {                    // Expression Tree
      UINT16         iVer;             // Version tag of expression.
      UINT16         iTotalSize;       // Size of this structure
      UINT16         iNodes;           // Number of nodes
      UINT16         iNodeStart;       // Starting offet of Nodes in this
      UINT16         iLiteralStart;    // Starting offset of Literals in this
      // Nodes and Literals are here
   } CANExpr;
typedef CANExpr far * pCANExpr;
typedef pCANExpr far *ppCANExpr;


//pfGENFilter returns TRUE, FALSE or ABORT
#define ABORT     -2

#if !defined(MIDL)
typedef INT16 (DBIFN * pfGENFilter)
   (
      UINT32            ulClientData,
      pBYTE             pRecBuf,
      UINT32            iPhyRecNum
   ) ;
#endif // !defined(MIDL)

typedef UINT32 hDBIFilter ;
typedef hDBIFilter far *phDBIFilter ;


#if !defined(MIDL)
typedef struct
   {
      UINT16          iFilterId;    // Id for filter
      hDBIFilter      hFilter;      // Filter handle
      UINT32          iClientData;  // Client supplied data
      UINT16          iPriority;    // 1..N with 1 being highest
      BOOL16          bCanAbort;    // TRUE : pfFilter can return ABORT
      pfGENFilter     pfFilter;     // Client filter function
      pVOID           pCanExpr;     // Supplied expression
      BOOL16          bActive ;     // TRUE : filter is active
   } FILTERInfo;
typedef FILTERInfo far *pFILTERInfo;
#endif // !defined(MIDL)

#ifndef NODBIQBE

//---------------------------------------------------------------------------
//   DBI Query related types
//---------------------------------------------------------------------------

#define MAXQBEEXPRSIZE  300             // size of one QBE expr


typedef struct  _DBIQryProp
{
   DBINAME    szQryName;       // Name of query
   DBIQryLang eLang;           // Language
   INT16      iQryPrice;       // Query price 1..100 (1 = cheap, 100 = expensive)
   INT16      iNumTables;      // Number of tables in join.  0 = unknown.
   BOOL16     bHasAnswer;
   BOOL16     bInsert;
   BOOL16     bDelete;
   BOOL16     bChange;
}  DBIQryProp, far *pDBIQryProp;


#define DBIQBE_ANSWERBIT    (0x1)    // Answer table bit flag
#define DBIQBE_INSERTEDBIT  (0x2)    // Inserted table bit flag
#define DBIQBE_DELETEDBIT   (0x4)    // Deleted table bit flag
#define DBIQBE_CHANGEDBIT   (0x8)    // Changed table bit flag
#define DBIQBE_ERRORINSBIT  (0x10)   // Error inserted table bit flag
#define DBIQBE_ERRORDELBIT  (0x20)   // Error deleted table bit flag
#define DBIQBE_ERRORCHGBIT  (0x40)   // Error changed table bit flag


// answer cursor properties:

#define bAnsHasLiveFields  0x1
#define bAnsHasFilter      0x2
#define bAnsHasFieldMap    0x4
#define bAnsHasCalcField   0x8
#define bAnsHasLiveBlob    0x10

// answer field properties:

#define bIsAnsFieldLive    0x1



typedef enum
{
   dbiqryDEFAULT = 0,
   dbiqryDIRTY   = 1,
   dbiqryCLEAN   = 2,
   dbiqryRESTART = 3
} DBIQryType, far *pDBIQryType;


//---------------------------------------------------------------------------
typedef struct
{
   UINT16    stepsInQry;      // Total steps in query.
   UINT16    stepsCompleted;  // Steps completed out of total (steps may be skipped).
   UINT32    totElemInStep;   // Total elements in current step.
   UINT32    elemCompleted;   // Elements completed in current step.
} DBIQryProgress, far *pDBIQryProgress;


typedef enum
{
   qryModeNONE     = 0,       // Reserved
   qryModeLOCAL    = 1,
   qryModeSERVER   = 2,
   qryModeEITHER   = 3,
   qryModeNOWLOCAL = 4        // used only in call back, when failed on server
} QryEvalMode;


// values for client indicating live/canned preference about query execution
typedef enum
{
   wantDEFAULT     = 0,        // Default , same as wantCANNED
   wantLIVE        = 1,        // Want live data even if extra effort (no guarantee)
   wantCANNED      = 2,        // Want canned data even if extra effort (guaranteed)
   wantSPEED       = 3         // Let query manager decide, find out afterwards
} LIVENESS;


typedef struct
{
   INT16       length;        // Length in bytes of this structure
   BOOL16      blankzeroes;   // TRUE if blanks to be regarded as zeros
   FMTDate     dateFormat;    // Date format
   FMTNumber   numberFormat;  // Number format
   BOOL16      bNeedAuxTbls;  // If FALSE, don't bother with DELETED/ERRDEL, etc.
   QryEvalMode qryMode;       // qryModeSERVER, qryModeLOCAL or qryModeEITHER.
   BOOL16      perQrySqlMode;
   LIVENESS    livenessDesired;
} QueryLowProps, far *pQueryLowProps;

#endif  // NODBIQBE

#endif  // NODBITYPES

#ifndef NODBISYMBOLS
//=======================================================================================
//                      DBI symbols
//=======================================================================================

# define DBIMOD_BEGIN         (0x3F00U)

# define DBIMOD_QBE           (DBIMOD_BEGIN + 1)
# define DBIMOD_SQLG          (DBIMOD_BEGIN + 2)
# define DBIMOD_LEGO          (DBIMOD_BEGIN + 3)
# define DBIMOD_LOCKMNGR      (DBIMOD_BEGIN + 4)
# define DBIMOD_SQLDRIVER     (DBIMOD_BEGIN + 5)
# define DBIMOD_OS            (DBIMOD_BEGIN + 6)
# define DBIMOD_DBASEDRV      (DBIMOD_BEGIN + 7)
# define DBIMOD_CDR           (DBIMOD_BEGIN + 8)

# define DBIMOD_END           (DBIMOD_BEGIN + 9)

//---------------------------------------------------------------------------

# define DBISYM_BEGIN         (DBIMOD_END + 1)

# define DBISYM_TOKEN         (DBISYM_BEGIN + ecTOKEN)
# define DBISYM_TABLENAME     (DBISYM_BEGIN + ecTABLENAME)
# define DBISYM_FIELDNAME     (DBISYM_BEGIN + ecFIELDNAME)
# define DBISYM_IMAGEROW      (DBISYM_BEGIN + ecIMAGEROW)
# define DBISYM_USERNAME      (DBISYM_BEGIN + ecUSERNAME)
# define DBISYM_FILENAME      (DBISYM_BEGIN + ecFILENAME)
# define DBISYM_INDEXNAME     (DBISYM_BEGIN + ecINDEXNAME)
# define DBISYM_DIRNAME       (DBISYM_BEGIN + ecDIRNAME)
# define DBISYM_KEYNAME       (DBISYM_BEGIN + ecKEYNAME)
# define DBISYM_ALIAS         (DBISYM_BEGIN + ecALIAS)
# define DBISYM_DRIVENAME     (DBISYM_BEGIN + ecDRIVENAME)
# define DBISYM_NATIVECODE    (DBISYM_BEGIN + ecNATIVECODE)
# define DBISYM_NATIVEMSG     (DBISYM_BEGIN + ecNATIVEMSG)
# define DBISYM_LINENUMBER    (DBISYM_BEGIN + ecLINENUMBER)
# define DBISYM_CAPABILITY    (DBISYM_BEGIN + ecCAPABILITY)
# define DBISYM_CDRNAME       (DBISYM_BEGIN + ecCDRNAME)
# define DBISYM_USERERRMSG    (DBISYM_BEGIN + ecUSERERRMSG)
# define DBISYM_DROBJNAME     (DBISYM_BEGIN + ecDROBJNAME)
# define DBISYM_INTERNALLIMIT (DBISYM_BEGIN + ecINTERNALLIMIT)

# define DBISYM_BASEEND       (DBISYM_BEGIN + 100)

//-----------------------------------------------------------------------------

# define DBISYM_MISC          (DBISYM_BASEEND + 1)

# define DBISYM_WORK          (DBISYM_MISC + 1)
# define DBISYM_PRIV          (DBISYM_MISC + 2)
# define DBISYM_COPY          (DBISYM_MISC + 3)
# define DBISYM_APPEND        (DBISYM_MISC + 4)
# define DBISYM_TXTPROBFLD1   (DBISYM_MISC + 5)
# define DBISYM_TXTPROBFLD2   (DBISYM_MISC + 6)
# define DBISYM_TXTPROBFLD3   (DBISYM_MISC + 7)

# define DBISYM_END           (DBIMOD_BEGIN + 0x3FFF)



//=======================================================================================
//                      DBI Config symbols
//=======================================================================================



// Categories

#define szCFGSYSTEM        "SYSTEM"
#define szCFGDRIVER        "DRIVERS"
#define szCFGDATABASE      "DATABASES"
#define szCFGREPOSITORY    "REPOSITORIES"

//--------------------------------------------------------------------------
// System Fields
//--------------------------------------------------------------------------

#define szCFGSYSVERSION       "VERSION"
#define szCFGSYSNETTYPE       "NET TYPE"
#define szCFGSYSNETDIR        "NET DIR"
#define szCFGSYSLOCALSHARE    "LOCAL SHARE"
#define szCFGSYSLANGDRV       "LANGDRIVER"
#define szCFGSYSLANGDRVDIR    "LANGDRVDIR"
#define szCFGSYSMINBUF        "MINBUFSIZE"
#define szCFGSYSMAXBUF        "MAXBUFSIZE"
#define szCFGSYSLOCKRETRY     "LOCKRETRY"
#define szCFGSYSFLAGS         "SYSFLAGS"
#define szCFGMAXFILEHANDLES   "MAXFILEHANDLES"
#define szCFGSQLQRYMODE       "SQLQRYMODE"
#define szCFGLOWMEMLIMIT      "LOW MEMORY USAGE LIMIT"  // Use this instead of NOLOWMEMBUF
#define szCFGSYSODBCCFGIMPORT "AUTO ODBC"
#define szCFGAUTOODBC         "AUTO ODBC"
#define szCFGDEFDRV           "DEFAULT DRIVER"
#define szCFGSYSLOCALREPOSITORY "DATA REPOSITORY"
//#define szCFGSYSCOMMONREPOSITORY "COMMON REPOSITORY"
#define szCFGSYSSHAREDMEMSIZE      "SHAREDMEMSIZE"
#define szCFGSYSSHAREDMEMLOCATION  "SHAREDMEMLOCATION"


//--------------------------------------------------------------------------
// Driver Fields
//--------------------------------------------------------------------------

#define szCFGDRVVERSION       "VERSION"
#define szCFGDRVTYPE          "TYPE"
#define szCFGDRVDLL           "DLL"
#define szCFGDRVDLL32         "DLL32"
#define szCFGDRVFLAGS         "DRIVER FLAGS"
#define szCFGDRVLANGDRIVER    "LANGDRIVER"
#define szCFGDRVFILLFACTOR    "FILL FACTOR"
#define szCFGDRVBLOCKSIZE     "BLOCK SIZE"
#define szCFGDRVLOCKPROTOCOL  "LOCKPROTOCOL"
#define szCFGDRVLEVEL         "LEVEL"
#define szCFGDRVVENDINIT      "VENDOR INIT"
#define szCFGDRVTRACEMODE     "TRACE MODE"

//--------------------------------------------------------------------------
// Dbase Driver fields
//--------------------------------------------------------------------------

#define szCFGDRVMEMOBLOCKSIZE "MEMO FILE BLOCK SIZE"
#define szCFGDRVMDXBLOCKSIZE  "MDX BLOCK SIZE"


//--------------------------------------------------------------------------
// Driver Nodes
//--------------------------------------------------------------------------

#define szCFGDRVINIT         "INIT"
#define szCFGDBCREATE        "DB CREATE"
#define szCFGDBOPEN          "DB OPEN"
#define szCFGTBLCREATE       "TABLE CREATE"
#define szCFGTBLOPEN         "TABLE OPEN"

//--------------------------------------------------------------------------
// Database Nodes
//--------------------------------------------------------------------------

#define szCFGDBINFO          "DB INFO"

//--------------------------------------------------------------------------
// Database fields
//--------------------------------------------------------------------------

#define szCFGDBTYPE          "TYPE"
#define szCFGDBPATH          "PATH"
#define szCFGDBDEFAULTDRIVER "DEFAULT DRIVER"
#define szCFGDBENABLEBCD     "ENABLE BCD"

//--------------------------------------------------------------------------
// Others
//--------------------------------------------------------------------------

#define szCFGINIT            "INIT"
#define szTYPE               "TYPE"
#define szCFGDBSTANDARD      "STANDARD"
#define szCFGTRUE            "TRUE"
#define szCFGFALSE           "FALSE"
#define szOPENMODE           "OPEN MODE"
#define szREADWRITE          "READ/WRITE"
#define szREADONLY           "READ ONLY"
#define szSHAREMODE          "SHARE MODE"
#define szEXCLUSIVE          "EXCLUSIVE"
#define szSHARED             "SHARED"
#define szUSERNAME           "USER NAME"
#define szSERVERNAME         "SERVER NAME"
#define szDATABASENAME       "DATABASE NAME"
#define szSCHEMASIZE         "SCHEMA CACHE SIZE"
#define szCFGSTRICTINTEGRITY "STRICTINTEGRTY"
#define szSQLPASSMODE        "SQLPASSTHRU MODE"
#define szNOTSHARED          "NOT SHARED"
#define szSHAREDAUTOCOMMIT   "SHARED AUTOCOMMIT"
#define szSHAREDNOAUTOCOMMIT "SHARED NOAUTOCOMMIT"
#define szSCHEMATIME         "SCHEMA CACHE TIME"
#define szMAXQUERYTIME       "MAX QUERY TIME"
#define szMAXROWS            "MAX ROWS"
#define szLISTSYNONYMS       "LIST SYNONYMS"
#define szSYNNONE            "NONE"
#define szSYNALL             "ALL"
#define szSYNPRIVATE         "PRIVATE"
#define szBATCHCOUNT         "BATCH COUNT"
#define szENABLESCHEMACACHE  "ENABLE SCHEMA CACHE"
#define szSCHEMACACHEDIR     "SCHEMA CACHE DIR"
#define szSYBLHOST          "HOST NAME"
#define szSYBLAPP           "APPLICATION NAME"
#define szSYBLNATLANG       "NATIONAL LANG NAME"
#define szTDSPACKETSIZE      "TDS PACKET SIZE"
#define szORAINTEGER         "ENABLE INTEGERS"

//--------------------------------------------------------------------------
// Repository fields
//--------------------------------------------------------------------------

#define szCFGDRDBNAME        "DATABASE NAME"
#define szCFGDRTBLNAME       "TABLE NAME"
#define szCFGDRLANGDRIVER    "LANGUAGE DRIVER"
#define szCFGDRDESC          "DESCRIPTION"


//--------------------------------------------------------------------------
// SYSTEM DATE/TIME/NUMBER FORMATS
// SYSTEM nodes:
//--------------------------------------------------------------------------
#define  szCFGFORMAT          "FORMATS"

//--------------------------------------------------------------------------
// Format nodes:
//--------------------------------------------------------------------------
#define  szCFGDATE           "DATE"
#define  szCFGTIME           "TIME"
#define  szCFGNUMBER         "NUMBER"

//--------------------------------------------------------------------------
// DATE and/or TIME fields:
//--------------------------------------------------------------------------
#define szCFGSEPARATOR        "SEPARATOR"
#define szCFGMODE             "MODE"
#define szCFGFOURDIGITYEAR    "FOURDIGITYEAR"
#define szCFGYEARBIASED       "YEARBIASED"
#define szCFGLEADINGZEROM     "LEADINGZEROM"
#define szCFGLEADINGZEROD     "LEADINGZEROD"
#define szCFGTWELVEHOUR       "TWELVEHOUR"
#define szCFGAMSTRING         "AMSTRING"
#define szCFGPMSTRING         "PMSTRING"
#define szCFGSECONDS          "SECONDS"
#define szCFGMILSECONDS       "MILSECONDS"

//--------------------------------------------------------------------------
// Number fields:
//--------------------------------------------------------------------------
#define szCFGDECIMALSEPARATOR    "DECIMALSEPARATOR"
#define szCFGTHOUSANDSEPARATOR   "THOUSANDSEPARATOR"
#define szCFGDECIMALDIGITS       "DECIMALDIGITS"
#define szCFGLEADINGZERON        "LEADINGZERON"



// String resoure id's for each string listed above

#define   DBICFG_BASE              0x3A00

//--------------------------------------------------------------------------
// Categories
//--------------------------------------------------------------------------
#define iCFGSYSTEM               (DBICFG_BASE + 1)
#define iCFGDRIVER               (DBICFG_BASE + 2)
#define iCFGDATABASE             (DBICFG_BASE + 3)
#define iCFGREPOSITORY           (DBICFG_BASE + 210)

//--------------------------------------------------------------------------
// System Fields
//--------------------------------------------------------------------------
#define iCFGSYSVERSION           (DBICFG_BASE + 5)
#define iCFGSYSNETTYPE           (DBICFG_BASE + 6)
#define iCFGSYSNETDIR            (DBICFG_BASE + 7)
#define iCFGSYSLOCALSHARE        (DBICFG_BASE + 8)
#define iCFGSYSLANGDRV           (DBICFG_BASE + 9)
#define iCFGSYSLANGDRVDIR        (DBICFG_BASE + 10)
#define iCFGSYSMINBUF            (DBICFG_BASE + 11)
#define iCFGSYSMAXBUF            (DBICFG_BASE + 12)
#define iCFGSYSLOCKRETRY         (DBICFG_BASE + 13)
#define iCFGSYSFLAGS             (DBICFG_BASE + 14)
#define iCFGMAXFILEHANDLES       (DBICFG_BASE + 15)
#define iCFGSQLQRYMODE           (DBICFG_BASE + 16)
#define iCFGLOWMEMLIMIT          (DBICFG_BASE + 17)
#define iCFGSYSODBCCFGIMPORT     (DBICFG_BASE + 18)
#define iCFGSYSLOCALREPOSITORY   (DBICFG_BASE + 211)
//#define iCFGSYSCOMMONREPOSITORY  (DBICFG_BASE + 212)
#define iCFGSYSSHAREDMEMSIZE     (DBICFG_BASE + 250)
#define iCFGSYSSHAREDMEMLOCATION (DBICFG_BASE + 251)



//--------------------------------------------------------------------------
// Driver Fields
//--------------------------------------------------------------------------
#define iCFGDRVVERSION           (DBICFG_BASE + 20)
#define iCFGDRVTYPE              (DBICFG_BASE + 21)
#define iCFGDRVLANGDRIVER        (DBICFG_BASE + 22)
#define iCFGDRVFILLFACTOR        (DBICFG_BASE + 23)
#define iCFGDRVBLOCKSIZE         (DBICFG_BASE + 24)
#define iCFGDRVLOCKPROTOCOL      (DBICFG_BASE + 25)
#define iCFGDRVLEVEL             (DBICFG_BASE + 26)
#define iCFGDRVFLAGS             (DBICFG_BASE + 27)
#define iCFGDRVTRACEMODE         (DBICFG_BASE + 28)

//--------------------------------------------------------------------------
// Dbase Driver fields
//--------------------------------------------------------------------------
#define iCFGDRVMEMOBLOCKSIZE     (DBICFG_BASE + 30 )
#define iCFGDRVMDXBLOCKSIZE      (DBICFG_BASE + 31 )


//--------------------------------------------------------------------------
// Driver Nodes
//--------------------------------------------------------------------------
#define iCFGDRVINIT              (DBICFG_BASE + 40 )
#define iCFGDBCREATE             (DBICFG_BASE + 41 )
#define iCFGDBOPEN               (DBICFG_BASE + 42 )
#define iCFGTBLCREATE            (DBICFG_BASE + 43 )
#define iCFGTBLOPEN              (DBICFG_BASE + 44 )

//--------------------------------------------------------------------------
// Database Nodes
//--------------------------------------------------------------------------
#define iCFGDBINFO               (DBICFG_BASE + 50 )

//--------------------------------------------------------------------------
// Database fields
//--------------------------------------------------------------------------
#define iCFGDBTYPE               (DBICFG_BASE + 60)
#define iCFGDBPATH               (DBICFG_BASE + 61)
#define iCFGDBDEFAULTDRIVER      (DBICFG_BASE + 62)
#define iCFGDBENABLEBCD          (DBICFG_BASE + 63)

//--------------------------------------------------------------------------
// Others
//--------------------------------------------------------------------------
#define iCFGINIT                 (DBICFG_BASE + 70)
#define iTYPE                    (DBICFG_BASE + 71)
#define iCFGDBSTANDARD           (DBICFG_BASE + 72)
#define iCFGTRUE                 (DBICFG_BASE + 73)
#define iCFGFALSE                (DBICFG_BASE + 74)
#define iOPENMODE                (DBICFG_BASE + 75)
#define iREADWRITE               (DBICFG_BASE + 76)
#define iREADONLY                (DBICFG_BASE + 77)
#define iSHAREMODE               (DBICFG_BASE + 78)
#define iEXCLUSIVE               (DBICFG_BASE + 79)
#define iSHARED                  (DBICFG_BASE + 80)
#define iUSERNAME                (DBICFG_BASE + 81)
#define iSERVERNAME              (DBICFG_BASE + 82)
#define iDATABASENAME            (DBICFG_BASE + 83)
#define iSCHEMASIZE              (DBICFG_BASE + 84)
#define iCFGSTRICTINTEGRITY      (DBICFG_BASE + 85)
#define iTDSPACKETSIZE           (DBICFG_BASE + 86)
#define iORAINTEGER              (DBICFG_BASE + 87)

//--------------------------------------------------------------------------
// Repository Nodes
//--------------------------------------------------------------------------
#define iCFGDRDBNAME             (DBICFG_BASE + 213 )
#define iCFGDRTBLNAME            (DBICFG_BASE + 214 )
#define iCFGDRDESC               (DBICFG_BASE + 215 )
#define iCFGDRLANGDRIVER         (DBICFG_BASE + 212 )

//--------------------------------------------------------------------------
// System node:
//--------------------------------------------------------------------------
#define  iCFGFORMAT              (DBICFG_BASE + 130)

//--------------------------------------------------------------------------
// Format nodes:
//--------------------------------------------------------------------------
#define  iCFGDATE                (DBICFG_BASE + 131)
#define  iCFGTIME                (DBICFG_BASE + 132)
#define  iCFGNUMBER              (DBICFG_BASE + 133)

//--------------------------------------------------------------------------
// DATE and/or TIME fields:
//--------------------------------------------------------------------------
#define iCFGSEPARATOR            (DBICFG_BASE + 140)
#define iCFGMODE                 (DBICFG_BASE + 141)
#define iCFGFOURDIGITYEAR        (DBICFG_BASE + 142)
#define iCFGYEARBIASED           (DBICFG_BASE + 143)
#define iCFGLEADINGZEROM         (DBICFG_BASE + 144)
#define iCFGLEADINGZEROD         (DBICFG_BASE + 145)
#define iCFGTWELVEHOUR           (DBICFG_BASE + 146)
#define iCFGAMSTRING             (DBICFG_BASE + 147)
#define iCFGPMSTRING             (DBICFG_BASE + 148)
#define iCFGSECONDS              (DBICFG_BASE + 149)
#define iCFGMILSECONDS           (DBICFG_BASE + 150)

//--------------------------------------------------------------------------
// Number fields:
//--------------------------------------------------------------------------
#define iCFGDECIMALSEPARATOR     (DBICFG_BASE + 160)
#define iCFGTHOUSANDSEPARATOR    (DBICFG_BASE + 161)
#define iCFGDECIMALDIGITS        (DBICFG_BASE + 162)
#define iCFGLEADINGZERON         (DBICFG_BASE + 163)

#define iCFGDEFLANGDRV           (DBICFG_BASE + 165)
#define iCFGDBASEDEFLANGDRV      (DBICFG_BASE + 166)

//--------------------------------------------------------------------------
//Formats
//--------------------------------------------------------------------------
#define iCFGDEFSEPARATOR         (DBICFG_BASE + 170)
#define iCFGDEFMODE              (DBICFG_BASE + 171)
#define iCFGDEFFOURDIGITYEAR     (DBICFG_BASE + 172)
#define iCFGDEFYEARBIASED        (DBICFG_BASE + 173)
#define iCFGDEFLEADINGZEROM      (DBICFG_BASE + 174)
#define iCFGDEFLEADINGZEROD      (DBICFG_BASE + 175)
#define iCFGDEFTWELVEHOUR        (DBICFG_BASE + 176)
#define iCFGDEFAMSTRING          (DBICFG_BASE + 177)
#define iCFGDEFPMSTRING          (DBICFG_BASE + 178)
#define iCFGDEFSECONDS           (DBICFG_BASE + 179)
#define iCFGDEFMILSECONDS        (DBICFG_BASE + 180)
#define iCFGDEFDECIMALSEPARATOR  (DBICFG_BASE + 181)
#define iCFGDEFTHOUSANDSEPARATOR (DBICFG_BASE + 182)
#define iCFGDEFLEADINGZERO       (DBICFG_BASE + 183)

#define iCFGDEFVERSION             (DBICFG_BASE + 184)
#define iCFGDEFLOCALSHARE          (DBICFG_BASE + 185)
#define iCFGDEFMINBUFSIZE          (DBICFG_BASE + 186)
#define iCFGDEFMAXBUFSIZE          (DBICFG_BASE + 187)
#define iCFGDEFMAXFILEHANDLES      (DBICFG_BASE + 188)
#define iCFGDEFSYSFLAGS            (DBICFG_BASE + 189)
#define iCFGDEFLOWMEM              (DBICFG_BASE + 190)
#define iCFGDEFAUTOODBC            (DBICFG_BASE + 191)
#define iCFGDEFDEFDRV              (DBICFG_BASE + 192)

#define iCFGDEFDECIMALDIGITS       (DBICFG_BASE + 193)
#define iCFGDEFLEADINGZERON        (DBICFG_BASE + 194)

#define iCFGDEFPDXTYPE             (DBICFG_BASE + 195)
#define iCFGDEFPDXNETDIR           (DBICFG_BASE + 196)
#define iCFGDEFPDXLANGDRV          (DBICFG_BASE + 197)
#define iCFGDEFPDXLEVEL            (DBICFG_BASE + 198)
#define iCFGDEFPDXBLOCKSIZE        (DBICFG_BASE + 199)
#define iCFGDEFPDXFILLFACTOR       (DBICFG_BASE + 200)
#define iCFGDEFPDXSTRICTINTEGRTY   (DBICFG_BASE + 201)

#define iCFGDEFDBASETYPE           (DBICFG_BASE + 202)
#define iCFGDEFDBASELANGDRV        (DBICFG_BASE + 203)
#define iCFGDEFDBASELEVEL          (DBICFG_BASE + 204)
#define iCFGDEFDBASEMDXBLOCKSIZE   (DBICFG_BASE + 205)
#define iCFGDEFDBASEMEMOBLOCKSIZE  (DBICFG_BASE + 206)

#define iCFGAUTOODBC               (DBICFG_BASE + 207)
#define iCFGDEFDRV                 (DBICFG_BASE + 208)
#define iCFGENABLEBCD              (DBICFG_BASE + 209)
#define iCFGDEFSHAREDMEMSIZE       (DBICFG_BASE + 252)
#define iCFGDEFSHAREDMEMLOCATION   (DBICFG_BASE + 253)
#define iCFGDEFREPOSITORY          (DBICFG_BASE + 254)
#define iCFGDEFSQLQRYMODE          (DBICFG_BASE + 255)

// Categories
extern const char far SZCFGSYSTEM[];
extern const char far SZCFGDRIVER[];
extern const char far SZCFGDATABASE[];
extern const char far SZCFGREPOSITORY[];

// System Fields
extern const char far SZCFGSYSVERSION[];
extern const char far SZCFGSYSNETTYPE[];
extern const char far SZCFGSYSNETDIR[];
extern const char far SZCFGSYSLOCALSHARE[];
extern const char far SZCFGSYSLANGDRV[];
extern const char far SZCFGSYSLANGDRVDIR[];
extern const char far SZCFGSYSMINBUF[];
extern const char far SZCFGSYSMAXBUF[];
extern const char far SZCFGSYSLOCKRETRY[];
extern const char far SZCFGSYSFLAGS[];
extern const char far SZCFGMAXFILEHANDLES[];
extern const char far SZCFGSQLQRYMODE[];
extern const char far SZCFGLOWMEMLIMIT[];
extern const char far SZCFGSYSODBCCFGIMPORT[];
extern const char far SZCFGSYSLOCALREPOSITORY[];
//extern const char far SZCFGSYSCOMMONREPOSITORY[];
extern const char far SZCFGSYSSHAREDMEMSIZE[];
extern const char far SZCFGSYSSHAREDMEMLOCATION[];



// Driver Fields
extern const char far SZCFGDRVVERSION[];
extern const char far SZCFGDRVTYPE[];
extern const char far SZCFGDRVDLL[];
extern const char far SZCFGDRVDLL32[];
extern const char far SZCFGDRVFLAGS[];
extern const char far SZCFGDRVLANGDRIVER[];
extern const char far SZCFGDRVFILLFACTOR[];
extern const char far SZCFGDRVBLOCKSIZE[];
extern const char far SZCFGDRVLOCKPROTOCOL[];
extern const char far SZCFGDRVLEVEL[];
extern const char far SZCFGDRVTRACEMODE[];

// Dbase Driver fields
extern const char far SZCFGDRVMEMOBLOCKSIZE[];
extern const char far SZCFGDRVMDXBLOCKSIZE[];



// Driver Nodes
extern const char far SZCFGDRVINIT[];
extern const char far SZCFGDBCREATE[];
extern const char far SZCFGDBOPEN[];
extern const char far SZCFGTBLCREATE[];
extern const char far SZCFGTBLOPEN[];

// Database Nodes
extern const char far SZCFGDBINFO[];

// Database fields
extern const char far SZCFGDBTYPE[];
extern const char far SZCFGDBPATH[];
extern const char far SZCFGDBDEFAULTDRIVER[];
extern const char far SZCFGDBENABLEBCD[];

// Others
extern const char far SZCFGINIT[];
extern const char far SZTYPE[];
extern const char far SZCFGDBSTANDARD[];
extern const char far SZCFGTRUE[];
extern const char far SZCFGFALSE[];
extern const char far SZOPENMODE[];
extern const char far SZREADWRITE[];
extern const char far SZREADONLY[];
extern const char far SZSHAREMODE[];
extern const char far SZEXCLUSIVE[];
extern const char far SZSHARED[];
extern const char far SZUSERNAME[];
extern const char far SZSERVERNAME[];
extern const char far SZDATABASENAME[];
extern const char far SZSCHEMASIZE[];
extern const char far SZCFGSTRICTINTEGRITY[];
extern const char far SZTDSPACKETSIZE[];

// Repository fields
extern const char far SZCFGDRDBNAME[];
extern const char far SZCFGDRTBLNAME[];
extern const char far SZCFGDRDESC[];
extern const char far SZCFGDRLANGDRIVER[];


// SYSTEM DATE/TIME/NUMBER FORMATS
// SYSTEM nodes:
extern const char far  SZCFGFORMAT[];

// Format nodes:
extern const char far  SZCFGDATE[];
extern const char far  SZCFGTIME[];
extern const char far  SZCFGNUMBER[];

// DATE and/or TIME fields:
extern const char far SZCFGSEPARATOR[];
extern const char far SZCFGMODE[];
extern const char far SZCFGFOURDIGITYEAR[];
extern const char far SZCFGYEARBIASED[];
extern const char far SZCFGLEADINGZEROM[];
extern const char far SZCFGLEADINGZEROD[];
extern const char far SZCFGTWELVEHOUR[];
extern const char far SZCFGAMSTRING[];
extern const char far SZCFGPMSTRING[];
extern const char far SZCFGSECONDS[];
extern const char far SZCFGMILSECONDS[];

// Number fields:
extern const char far SZCFGDECIMALSEPARATOR[];
extern const char far SZCFGTHOUSANDSEPARATOR[];
extern const char far SZCFGDECIMALDIGITS[];
extern const char far SZCFGLEADINGZERON[];


extern const char far SZCFGDEFLANGDRV[];
extern const char far SZCFGDBASEDEFLANGDRV[];
extern const char far SZCFGDEFSEPARATOR[];
extern const char far SZCFGDEFMODE[];
extern const char far SZCFGDEFFOURDIGITYEAR[];
extern const char far SZCFGDEFYEARBIASED[];
extern const char far SZCFGDEFLEADINGZEROM[];
extern const char far SZCFGDEFLEADINGZEROD[];
extern const char far SZCFGDEFTWELVEHOUR[];
extern const char far SZCFGDEFAMSTRING[];
extern const char far SZCFGDEFPMSTRING[];
extern const char far SZCFGDEFSECONDS[];
extern const char far SZCFGDEFMILSECONDS[];
extern const char far SZCFGDEFDECIMALSEPARATOR[];
extern const char far SZCFGDEFTHOUSANDSEPARATOR[];
extern const char far SZCFGDEFLEADINGZERO[];


#endif  // NODBISYMBOLS

#ifndef NOCFGHLP
#define   CFGHLP_BASE              0x3B00

#define iCFGHLP_SYSNODE             (CFGHLP_BASE +1)
#define iCFGHLP_SYSINITNODE         (CFGHLP_BASE +2)
#define iCFGHLP_SYSVERSION          (CFGHLP_BASE +3)
#define iCFGHLP_SYSLOCALSHARE       (CFGHLP_BASE +4)
#define iCFGHLP_SYSMINBUFSIZE       (CFGHLP_BASE +5)
#define iCFGHLP_SYSMAXBUFSIZE       (CFGHLP_BASE +6)
#define iCFGHLP_SYSLANGDRIVER       (CFGHLP_BASE +7)
#define iCFGHLP_SYSNETTYPE          (CFGHLP_BASE +8)
#define iCFGHLP_SYSFLAGS            (CFGHLP_BASE +9)
#define iCFGHLP_SYSMAXFILE          (CFGHLP_BASE +10)
#define iCFGHLP_SYSLOWMEM           (CFGHLP_BASE +11)
#define iCFGHLP_SYSAUTOODBC         (CFGHLP_BASE +12)
#define iCFGHLP_SYSDEFDRV           (CFGHLP_BASE +13)
#define iCFGHLP_SYSSQLQRYMODE       (CFGHLP_BASE +14)
#define iCFGHLP_SYSSQLPASSTHRU      (CFGHLP_BASE +15)
#define iCFGHLP_SYSFORMATNODE       (CFGHLP_BASE +16)
#define iCFGHLP_DATENODE            (CFGHLP_BASE +17)
#define iCFGHLP_DATESEPARATOR       (CFGHLP_BASE +18)
#define iCFGHLP_DATEMODE            (CFGHLP_BASE +19)
#define iCFGHLP_DATEFOURDIGIT       (CFGHLP_BASE +20)
#define iCFGHLP_DATEYEARBIASED      (CFGHLP_BASE +21)
#define iCFGHLP_DATEZEROM           (CFGHLP_BASE +22)
#define iCFGHLP_DATEZEROD           (CFGHLP_BASE +23)
#define iCFGHLP_TIMENODE            (CFGHLP_BASE +24)
#define iCFGHLP_TIMETWELVEHOUR      (CFGHLP_BASE +25)
#define iCFGHLP_TIMEAMSTRING        (CFGHLP_BASE +26)
#define iCFGHLP_TIMEPMSTRING        (CFGHLP_BASE +27)
#define iCFGHLP_TIMESECONDS         (CFGHLP_BASE +28)
#define iCFGHLP_TIMEMILSEC          (CFGHLP_BASE +29)
#define iCFGHLP_NUMNODE             (CFGHLP_BASE +30)
#define iCFGHLP_NUMDECIMALSEPARATOR (CFGHLP_BASE +31)
#define iCFGHLP_NUMTHOUSANDSEPARATOR (CFGHLP_BASE +32)
#define iCFGHLP_NUMDECIMALDIGITS    (CFGHLP_BASE +33)
#define iCFGHLP_NUMZERON            (CFGHLP_BASE +34)
#define iCFGHLP_DRVNODE             (CFGHLP_BASE +35)
#define iCFGHLP_PDXNODE             (CFGHLP_BASE +36)
#define iCFGHLP_PDXINITNODE         (CFGHLP_BASE +37)
#define iCFGHLP_DRVTYPE             (CFGHLP_BASE +38)
#define iCFGHLP_PDXNETDIR           (CFGHLP_BASE +39)
#define iCFGHLP_PDXTBLNODE          (CFGHLP_BASE +40)
#define iCFGHLP_PDXLEVEL            (CFGHLP_BASE +41)
#define iCFGHLP_PDXBLOCKSIZE        (CFGHLP_BASE +42)
#define iCFGHLP_PDXFILLFACTOR       (CFGHLP_BASE +43)
#define iCFGHLP_PDXSTRICT           (CFGHLP_BASE +44)
#define iCFGHLP_DBNODE              (CFGHLP_BASE +45)
#define iCFGHLP_DBINITNODE          (CFGHLP_BASE +46)
#define iCFGHLP_DBVERSION           (CFGHLP_BASE +47)
#define iCFGHLP_DBTBLNODE           (CFGHLP_BASE +48)
#define iCFGHLP_DBLEVEL             (CFGHLP_BASE +49)
#define iCFGHLP_DBMDXBLOCKSIZE      (CFGHLP_BASE +50)
#define iCFGHLP_DBMEMOFILEBLOCKSIZE (CFGHLP_BASE +51)
#define iCFGHLP_INTNODE             (CFGHLP_BASE +52)
#define iCFGHLP_INTINITNODE         (CFGHLP_BASE +53)
#define iCFGHLP_INTVERSION          (CFGHLP_BASE +54)
#define iCFGHLP_SQLDLL              (CFGHLP_BASE +55)
#define iCFGHLP_SQLDLL32            (CFGHLP_BASE +56)
#define iCFGHLP_SQLDRIVERFLAGS      (CFGHLP_BASE +57)
#define iCFGHLP_INTDBNODE           (CFGHLP_BASE +58)
#define iCFGHLP_SQLSERVER           (CFGHLP_BASE +59)
#define iCFGHLP_SQLUSER             (CFGHLP_BASE +60)
#define iCFGHLP_SQLOPENMODE         (CFGHLP_BASE +61)
#define iCFGHLP_SQLSCHEMASIZE       (CFGHLP_BASE +62)
#define iCFGHLP_SQLSCHEMATIME       (CFGHLP_BASE +63)
#define iCFGHLP_SYBNODE             (CFGHLP_BASE +64)
#define iCFGHLP_SYBINITNODE         (CFGHLP_BASE +65)
#define iCFGHLP_SYBVERSION          (CFGHLP_BASE +66)
#define iCFGHLP_SQLCONNECT          (CFGHLP_BASE +67)
#define iCFGHLP_SQLTIMEOUT          (CFGHLP_BASE +68)
#define iCFGHLP_SYBDBNODE           (CFGHLP_BASE +69)
#define iCFGHLP_SQLDATABASE         (CFGHLP_BASE +70)
#define iCFGHLP_SQLBLOBEDIT         (CFGHLP_BASE +71)
#define iCFGHLP_SQLMAXQUERY         (CFGHLP_BASE +72)
#define iCFGHLP_ORANODE             (CFGHLP_BASE +73)
#define iCFGHLP_ORAINITNODE         (CFGHLP_BASE +74)
#define iCFGHLP_ORAVERSION          (CFGHLP_BASE +75)
#define iCFGHLP_SQLVENDOR           (CFGHLP_BASE +76)
#define iCFGHLP_ORADBNODE           (CFGHLP_BASE +77)
#define iCFGHLP_SQLNETPROTOCOL      (CFGHLP_BASE +78)
#define iCFGHLP_MSSNODE             (CFGHLP_BASE +79)
#define iCFGHLP_MSSINITNODE         (CFGHLP_BASE +80)
#define iCFGHLP_MSSVERSION          (CFGHLP_BASE +81)
#define iCFGHLP_MSSDBNODE           (CFGHLP_BASE +82)
#define iCFGHLP_INFNODE             (CFGHLP_BASE +83)
#define iCFGHLP_INFINITNODE         (CFGHLP_BASE +84)
#define iCFGHLP_INFVERSION          (CFGHLP_BASE +85)
#define iCFGHLP_INFDBNODE           (CFGHLP_BASE +86)
#define iCFGHLP_SQLLOCKMODE         (CFGHLP_BASE +87)
#define iCFGHLP_SQLTRACEMODE        (CFGHLP_BASE +88)
#define iCFGHLP_SQLMAXROWS          (CFGHLP_BASE +89)
#define iCFGHLP_SQLBATCHCOUNT       (CFGHLP_BASE +90)
#define iCFGHLP_SYSSHAREDMEMSIZ     (CFGHLP_BASE +91)
#define iCFGHLP_SYSSHAREDMEMLOC     (CFGHLP_BASE +92)
#define iCFGHLP_SYSDATAREP          (CFGHLP_BASE +93)
#define iCFGHLP_ALIASTYPE           (CFGHLP_BASE +94)
#define iCFGHLP_ALIASPATH           (CFGHLP_BASE +95)
#define iCFGHLP_ALIASDEFDRV         (CFGHLP_BASE +96)
#define iCFGHLP_ENABLESCHEMACACHE   (CFGHLP_BASE +97)
#define iCFGHLP_SCHEMACACHEDIR      (CFGHLP_BASE +98)
#define iCFGHLP_HOSTNAME            (CFGHLP_BASE +99)
#define iCFGHLP_APPLICATIONNAME     (CFGHLP_BASE +100)
#define iCFGHLP_NATIONALLANGNAME    (CFGHLP_BASE +101)
#define iCFGHLP_ALIASENABLEBCD      (CFGHLP_BASE +102)
#define iCFGHLP_TDSPACKETSIZE       (CFGHLP_BASE +103)
#define iCFGHLP_ORAINTEGER          (CFGHLP_BASE +104)
#define iCFGHLP_ORALISTSYNONYMS     (CFGHLP_BASE +105)

#endif // NOCFGHLP

#ifndef NODBIERRS
#define ErrCat(rslt)  ((UINT16)(rslt) >> 8)
#define ErrCode(rslt) ((UINT16)(rslt) & 0x00FF)


//============================================================================
//                            Error Categories
//============================================================================

#define ERRCAT_NONE                0   //  0   No error
#define ERRCAT_SYSTEM            0x21  //  33  System related (Fatal Error)
#define ERRCAT_NOTFOUND          0x22  //  34  Object of interest Not Found
#define ERRCAT_DATACORRUPT       0x23  //  35  Physical Data Corruption
#define ERRCAT_IO                0x24  //  36  I/O related error
#define ERRCAT_LIMIT             0x25  //  37  Resource or Limit error
#define ERRCAT_INTEGRITY         0x26  //  38  Integrity Violation
#define ERRCAT_INVALIDREQ        0x27  //  39  Invalid Request
#define ERRCAT_LOCKCONFLICT      0x28  //  40  Locking/Contention related
#define ERRCAT_SECURITY          0x29  //  41  Access Violation - Security related
#define ERRCAT_INVALIDCONTEXT    0x2A  //  42  Invalid context
#define ERRCAT_OS                0x2B  //  43  Os Error not handled by Idapi
#define ERRCAT_NETWORK           0x2C  //  44  Network related
#define ERRCAT_OPTPARAM          0x2D  //  45  Optional parameter related
#define ERRCAT_QUERY             0x2E  //  46  Query related
#define ERRCAT_VERSION           0x2F  //  47  Version Mismatch Category
#define ERRCAT_CAPABILITY        0x30  //  48  Capability not supported
#define ERRCAT_CONFIG            0x31  //  49  System configuration error
#define ERRCAT_WARNING           0x32  //  50
#define ERRCAT_OTHER             0x33  //  51  Miscellaneous
#define ERRCAT_COMPATIBILITY     0x34  //  52  Compatibility related
#define ERRCAT_REPOSITORY        0x35  //  53  Data Repository related

#define ERRCAT_DRIVER            0x3E  //  62  Driver related
#define ERRCAT_RC                0x3F  //  63  Internal


#define ERRBASE_NONE              0     // No error
#define ERRBASE_SYSTEM          0x2100  // System related (Fatal Error)
#define ERRBASE_NOTFOUND        0x2200  // Object of interest Not Found
#define ERRBASE_DATACORRUPT     0x2300  // Physical Data Corruption
#define ERRBASE_IO              0x2400  // I/O related error
#define ERRBASE_LIMIT           0x2500  // Resource or Limit error
#define ERRBASE_INTEGRITY       0x2600  // Integrity Violation
#define ERRBASE_INVALIDREQ      0x2700  // Invalid Request
#define ERRBASE_LOCKCONFLICT    0x2800  // Locking/Contention related
#define ERRBASE_SEC             0x2900  // Access Violation - Security related
#define ERRBASE_IC              0x2A00  // Invalid context
#define ERRBASE_OS              0x2B00  // Os Error not handled by Idapi
#define ERRBASE_NETWORK         0x2C00  // Network related
#define ERRBASE_OPTPARAM        0x2D00  // Optional Parameter related
#define ERRBASE_QUERY           0x2E00  // Query related
#define ERRBASE_VERSION         0x2F00  // Version Mismatch Category
#define ERRBASE_CAPABILITY      0x3000  // Capability not supported
#define ERRBASE_CONFIG          0x3100  // System configuration error
#define ERRBASE_WARNING         0x3200  //
#define ERRBASE_OTHER           0x3300  // Miscellaneous
#define ERRBASE_COMPATIBILITY   0x3400  // Compatibility related
#define ERRBASE_REPOSITORY      0x3500  // Data Repository related

#define ERRBASE_DRIVER          0x3E00  // Driver related
#define ERRBASE_RC              0x3F00  // Internal


//=============================================================================
//                           Error Codes By Category
//=============================================================================

// ERRCAT_NONE                  (0)
// ===========

#define ERRCODE_NONE             0

#define DBIERR_NONE                (ERRBASE_NONE + ERRCODE_NONE)

//  ERRCAT_SYSTEM
//  =============

#define  ERRCODE_SYSFILEOPEN     1   // Cannot open a system file
#define  ERRCODE_SYSFILEIO       2   // I/O error on a system file
#define  ERRCODE_SYSCORRUPT      3   // Data structure corruption
#define  ERRCODE_NOCONFIGFILE    4   // Cannot find config file
#define  ERRCODE_CFGCANNOTWRITE  5   // Cannot write config file (READONLY)
#define  ERRCODE_CFGMULTIFILE    6   // Initializing with different ini file
#define  ERRCODE_REENTERED       7   // System has been illegally re-entered
#define  ERRCODE_CANTFINDIDAPI   8   // Cannot locate IDAPIxx.DLL
#define  ERRCODE_CANTLOADIDAPI   9   // Cannot load IDAPIxx.DLL
#define  ERRCODE_CANTLOADLIBRARY 10  // Cannot load a service DLL
#define  ERRCODE_TEMPFILEERR     11  // Cannot create or open temporary file
#define  ERRCODE_MULTIPLEIDAPI   12  // Trying to load multiple IDAPIxx.DLL

#define  DBIERR_SYSFILEOPEN      (ERRBASE_SYSTEM + ERRCODE_SYSFILEOPEN)
#define  DBIERR_SYSFILEIO        (ERRBASE_SYSTEM + ERRCODE_SYSFILEIO)
#define  DBIERR_SYSCORRUPT       (ERRBASE_SYSTEM + ERRCODE_SYSCORRUPT)
#define  DBIERR_NOCONFIGFILE     (ERRBASE_SYSTEM + ERRCODE_NOCONFIGFILE)
#define  DBIERR_CFGCANNOTWRITE   (ERRBASE_SYSTEM + ERRCODE_CFGCANNOTWRITE)
#define  DBIERR_CFGMULTIFILE     (ERRBASE_SYSTEM + ERRCODE_CFGMULTIFILE)
#define  DBIERR_REENTERED        (ERRBASE_SYSTEM + ERRCODE_REENTERED)
#define  DBIERR_CANTFINDIDAPI    (ERRBASE_SYSTEM + ERRCODE_CANTFINDIDAPI)
#define  DBIERR_CANTLOADIDAPI    (ERRBASE_SYSTEM + ERRCODE_CANTLOADIDAPI)
#define  DBIERR_CANTLOADLIBRARY  (ERRBASE_SYSTEM + ERRCODE_CANTLOADLIBRARY)
#define  DBIERR_TEMPFILEERR      (ERRBASE_SYSTEM + ERRCODE_TEMPFILEERR)
#define  DBIERR_MULTIPLEIDAPI    (ERRBASE_SYSTEM + ERRCODE_MULTIPLEIDAPI)

#define  DBIERR_CANTFINDODAPI    DBIERR_CANTFINDIDAPI
#define  DBIERR_CANTLOADODAPI    DBIERR_CANTLOADIDAPI

//  ERRCAT_NOTFOUND
//  ===============

#define  ERRCODE_BOF             1        // Beginning of Virtual table
#define  ERRCODE_EOF             2        // End of Virtual table
#define  ERRCODE_RECMOVED        3        // Fly-away
#define  ERRCODE_KEYORRECDELETED 4        // Record Deleted/Key Modified
#define  ERRCODE_NOCURRREC       5        // No current record
#define  ERRCODE_RECNOTFOUND     6        // Record was not found
#define  ERRCODE_ENDOFBLOB       7        // End of Blob reached
#define  ERRCODE_OBJNOTFOUND     8        // Generic Not found
#define  ERRCODE_FMLMEMBERNOTFOUND 9      // Family member not found
#define  ERRCODE_BLOBFILEMISSING 10       // 0x0a Blob file for table is missing
#define  ERRCODE_LDNOTFOUND      11       // 0x0b Language driver not found

#define  DBIERR_BOF              (ERRBASE_NOTFOUND + ERRCODE_BOF)
#define  DBIERR_EOF              (ERRBASE_NOTFOUND + ERRCODE_EOF)
#define  DBIERR_RECMOVED         (ERRBASE_NOTFOUND + ERRCODE_RECMOVED)
#define  DBIERR_RECDELETED       (ERRBASE_NOTFOUND + ERRCODE_KEYORRECDELETED)
#define  DBIERR_KEYORRECDELETED  (ERRBASE_NOTFOUND + ERRCODE_KEYORRECDELETED)
#define  DBIERR_NOCURRREC        (ERRBASE_NOTFOUND + ERRCODE_NOCURRREC)
#define  DBIERR_RECNOTFOUND      (ERRBASE_NOTFOUND + ERRCODE_RECNOTFOUND)
#define  DBIERR_ENDOFBLOB        (ERRBASE_NOTFOUND + ERRCODE_ENDOFBLOB)
#define  DBIERR_OBJNOTFOUND      (ERRBASE_NOTFOUND + ERRCODE_OBJNOTFOUND)
#define  DBIERR_FMLMEMBERNOTFOUND (ERRBASE_NOTFOUND + ERRCODE_FMLMEMBERNOTFOUND)
#define  DBIERR_BLOBFILEMISSING (ERRBASE_NOTFOUND + ERRCODE_BLOBFILEMISSING)
#define  DBIERR_LDNOTFOUND (ERRBASE_NOTFOUND + ERRCODE_LDNOTFOUND)

// ERRCAT_DATACORRUPT
// ==================

#define  ERRCODE_HEADERCORRUPT   1        // Corrupt Header
#define  ERRCODE_FILECORRUPT     2        // File corrupt - other than header
#define  ERRCODE_MEMOCORRUPT     3        // Memo file corrupted
#define  ERRCODE_BMPCORRUPT      4        // BitMap is corrupt (Internal error)
#define  ERRCODE_INDEXCORRUPT    5        // Index is corrupt
#define  ERRCODE_CORRUPTLOCKFILE 6        // Corrupt lock file
#define  ERRCODE_FAMFILEINVALID  7        // Corrupt family file
#define  ERRCODE_VALFILECORRUPT  8        // Val file is missing or corrupt
#define  ERRCODE_FOREIGNINDEX    9        // Index is in a foreign format - import first


#define  DBIERR_HEADERCORRUPT    (ERRBASE_DATACORRUPT + ERRCODE_HEADERCORRUPT)
#define  DBIERR_FILECORRUPT      (ERRBASE_DATACORRUPT + ERRCODE_FILECORRUPT)
#define  DBIERR_MEMOCORRUPT      (ERRBASE_DATACORRUPT + ERRCODE_MEMOCORRUPT)
#define  DBIERR_BMPCORRUPT       (ERRBASE_DATACORRUPT + ERRCODE_BMPCORRUPT)
#define  DBIERR_INDEXCORRUPT     (ERRBASE_DATACORRUPT + ERRCODE_INDEXCORRUPT)
#define  DBIERR_CORRUPTLOCKFILE  (ERRBASE_DATACORRUPT + ERRCODE_CORRUPTLOCKFILE)
#define  DBIERR_FAMFILEINVALID   (ERRBASE_DATACORRUPT + ERRCODE_FAMFILEINVALID)
#define  DBIERR_VALFILECORRUPT   (ERRBASE_DATACORRUPT + ERRCODE_VALFILECORRUPT)
#define  DBIERR_FOREIGNINDEX     (ERRBASE_DATACORRUPT + ERRCODE_FOREIGNINDEX)


// ERRCAT_IO
// =========

#define  ERRCODE_READERR         1        // Read failure (not expected)
#define  ERRCODE_WRITEERR        2        // Write failure (not expected)
#define  ERRCODE_DIRNOACCESS     3        // No access to dir
#define  ERRCODE_FILEDELETEFAIL  4        // File delete failed
#define  ERRCODE_FILENOACCESS    5        // No access to file
#define  ERRCODE_ACCESSDISABLED  6        // Access to table disabled (previous error)

#define  DBIERR_READERR          (ERRBASE_IO + ERRCODE_READERR)
#define  DBIERR_WRITEERR         (ERRBASE_IO + ERRCODE_WRITEERR)
#define  DBIERR_DIRNOACCESS      (ERRBASE_IO + ERRCODE_DIRNOACCESS)
#define  DBIERR_FILEDELETEFAIL   (ERRBASE_IO + ERRCODE_FILEDELETEFAIL)
#define  DBIERR_FILENOACCESS     (ERRBASE_IO + ERRCODE_FILENOACCESS)
#define  DBIERR_ACCESSDISABLED   (ERRBASE_IO + ERRCODE_ACCESSDISABLED)

// ERRCAT_LIMIT
// ============

#define  ERRCODE_NOMEMORY        1        // Not enough Memory for this op
#define  ERRCODE_NOFILEHANDLES   2        // Not enough File handles
#define  ERRCODE_NODISKSPACE     3        // Not enough Disk space
#define  ERRCODE_NOTEMPTBLSPACE  4        // Temporary Table resource limit
#define  ERRCODE_RECTOOBIG       5        // Too big a record size for table
#define  ERRCODE_CURSORLIMIT     6        // Too many open cursors
#define  ERRCODE_TABLEFULL       7        // Table is full
#define  ERRCODE_WSSESLIMIT      8        // Too many sessions from this WS
#define  ERRCODE_SERNUMLIMIT     9        // Serial number limit (paradox)
#define  ERRCODE_INTERNALLIMIT   10       // 0x0a Some internal limit (see context)
#define  ERRCODE_OPENTBLLIMIT    11       // 0x0b Too many open tables
#define  ERRCODE_TBLCURSORLIMIT  12       // 0x0c Too many cursors per table
#define  ERRCODE_RECLOCKLIMIT    13       // 0x0d Too many record locks on table
#define  ERRCODE_CLIENTSLIMIT    14       // 0x0e Too many clients
#define  ERRCODE_INDEXLIMIT      15       // 0x0f Too many indexes (also in Table Create)
#define  ERRCODE_SESSIONSLIMIT   16       // 0x10 Too many sessions
#define  ERRCODE_DBLIMIT         17       // 0x11 Too many databases
#define  ERRCODE_PASSWORDLIMIT   18       // 0x12 Too many passwords
#define  ERRCODE_DRIVERLIMIT     19       // 0x13 Too many active drivers
#define  ERRCODE_FLDLIMIT        20       // 0x14 Too many Fields in Table Create
#define  ERRCODE_TBLLOCKLIMIT    21       // 0x15 Too many table locks
#define  ERRCODE_OPENBLOBLIMIT   22       // 0x16 Too many open blobs
#define  ERRCODE_LOCKFILELIMIT   23       // 0x17 Lock file has grown too big
#define  ERRCODE_OPENQRYLIMIT    24       // 0x18 Too many open queries
#define  ERRCODE_THREADLIMIT     25       // 0x19 Too many threads for client
#define  ERRCODE_BLOBLIMIT       26       // 0x1a Too many blobs
#define  ERRCODE_PDX50NAMELIMIT  27       // 0x1b Pathname is too long for a Paradox 5.0 or less table
#define  ERRCODE_ROWFETCHLIMIT   28       // 0x1c Row fetch limit
#define  ERRCODE_LONGNAMENOTALLOWED 29    // 0x1d Long name is not allowed for this tableversion

#define  DBIERR_NOMEMORY         (ERRBASE_LIMIT + ERRCODE_NOMEMORY)
#define  DBIERR_NOFILEHANDLES    (ERRBASE_LIMIT + ERRCODE_NOFILEHANDLES)
#define  DBIERR_NODISKSPACE      (ERRBASE_LIMIT + ERRCODE_NODISKSPACE)
#define  DBIERR_NOTEMPTBLSPACE   (ERRBASE_LIMIT + ERRCODE_NOTEMPTBLSPACE)
#define  DBIERR_RECTOOBIG        (ERRBASE_LIMIT + ERRCODE_RECTOOBIG)
#define  DBIERR_CURSORLIMIT      (ERRBASE_LIMIT + ERRCODE_CURSORLIMIT)
#define  DBIERR_TABLEFULL        (ERRBASE_LIMIT + ERRCODE_TABLEFULL)
#define  DBIERR_WSSESLIMIT       (ERRBASE_LIMIT + ERRCODE_WSSESLIMIT)
#define  DBIERR_SERNUMLIMIT      (ERRBASE_LIMIT + ERRCODE_SERNUMLIMIT)
#define  DBIERR_INTERNALLIMIT    (ERRBASE_LIMIT + ERRCODE_INTERNALLIMIT)
#define  DBIERR_OPENTBLLIMIT     (ERRBASE_LIMIT + ERRCODE_OPENTBLLIMIT)
#define  DBIERR_TBLCURSORLIMIT   (ERRBASE_LIMIT + ERRCODE_TBLCURSORLIMIT)
#define  DBIERR_RECLOCKLIMIT     (ERRBASE_LIMIT + ERRCODE_RECLOCKLIMIT)
#define  DBIERR_CLIENTSLIMIT     (ERRBASE_LIMIT + ERRCODE_CLIENTSLIMIT)
#define  DBIERR_INDEXLIMIT       (ERRBASE_LIMIT + ERRCODE_INDEXLIMIT)
#define  DBIERR_SESSIONSLIMIT    (ERRBASE_LIMIT + ERRCODE_SESSIONSLIMIT)
#define  DBIERR_DBLIMIT          (ERRBASE_LIMIT + ERRCODE_DBLIMIT)
#define  DBIERR_PASSWORDLIMIT    (ERRBASE_LIMIT + ERRCODE_PASSWORDLIMIT)
#define  DBIERR_DRIVERLIMIT      (ERRBASE_LIMIT + ERRCODE_DRIVERLIMIT)
#define  DBIERR_FLDLIMIT         (ERRBASE_LIMIT + ERRCODE_FLDLIMIT)
#define  DBIERR_TBLLOCKLIMIT     (ERRBASE_LIMIT + ERRCODE_TBLLOCKLIMIT)
#define  DBIERR_OPENBLOBLIMIT    (ERRBASE_LIMIT + ERRCODE_OPENBLOBLIMIT)
#define  DBIERR_LOCKFILELIMIT    (ERRBASE_LIMIT + ERRCODE_LOCKFILELIMIT)
#define  DBIERR_OPENQRYLIMIT     (ERRBASE_LIMIT + ERRCODE_OPENQRYLIMIT)
#define  DBIERR_THREADLIMIT      (ERRBASE_LIMIT + ERRCODE_THREADLIMIT)
#define  DBIERR_BLOBLIMIT        (ERRBASE_LIMIT + ERRCODE_BLOBLIMIT)
#define  DBIERR_PDX50NAMELIMIT   (ERRBASE_LIMIT + ERRCODE_PDX50NAMELIMIT)
#define  DBIERR_ROWFETCHLIMIT    (ERRBASE_LIMIT + ERRCODE_ROWFETCHLIMIT)
#define  DBIERR_LONGNAMENOTALLOWED (ERRBASE_LIMIT + ERRCODE_LONGNAMENOTALLOWED)


// ERRCAT_INTEGRITY
// ================

#define ERRCODE_KEYVIOL             1     // Key violation
#define ERRCODE_MINVALERR           2     // Min val check failed
#define ERRCODE_MAXVALERR           3     // Max val check failed
#define ERRCODE_REQDERR             4     // Field value required
#define ERRCODE_FORIEGNKEYERR       5     // Master record missing
#define ERRCODE_DETAILRECORDSEXIST  6     // Cannot MODIFY or DELETE this Master record
#define ERRCODE_MASTERTBLLEVEL      7     // Master Table Level is incorrect
#define ERRCODE_LOOKUPTABLEERR      8     // Field value out of lookup tbl range
#define ERRCODE_LOOKUPTBLOPENERR    9     // Lookup Table Open failed
#define ERRCODE_DETAILTBLOPENERR   10     // 0x0a Detail Table Open failed
#define ERRCODE_MASTERTBLOPENERR   11     // 0x0b Master Table Open failed
#define ERRCODE_FIELDISBLANK       12     // 0x0c Field is blank

#define ERRCODE_MASTEREXISTS       13     // 0x0d Master Table exists
#define ERRCODE_MASTERTBLOPEN      14     // 0x0e Master Table is open

#define ERRCODE_DETAILTABLESEXIST    15   // 0x0f Detail Tables exist ( cannot delete, rename ... )
#define ERRCODE_DETAILRECEXISTEMPTY  16   // 0x10 Cannot empty because details exist
#define ERRCODE_MASTERREFERENCEERR   17   // 0x11 Cannot modify while adding self referencing Referential Integrity
#define ERRCODE_DETAILTBLOPEN        18   // 0x12 Detail Table is opened
#define ERRCODE_DEPENDENTSMUSTBEEMPTY  19 // 0x13 Cannot make a master a detail of another table if its details are not empty !
#define ERRCODE_RINTREQINDEX         20   // 0x14 Ref. integrity fields must be indexed
#define ERRCODE_LINKEDTBLPROTECTED   21   // 0x15 Master Table is protected ( requires password to open)
#define ERRCODE_FIELDMULTILINKED     22   // 0x16 Field has more than one master


#define DBIERR_KEYVIOL             (ERRBASE_INTEGRITY + ERRCODE_KEYVIOL)
#define DBIERR_MINVALERR           (ERRBASE_INTEGRITY + ERRCODE_MINVALERR)
#define DBIERR_MAXVALERR           (ERRBASE_INTEGRITY + ERRCODE_MAXVALERR)
#define DBIERR_REQDERR             (ERRBASE_INTEGRITY + ERRCODE_REQDERR)
#define DBIERR_FORIEGNKEYERR       (ERRBASE_INTEGRITY + ERRCODE_FORIEGNKEYERR)
#define DBIERR_DETAILRECORDSEXIST  (ERRBASE_INTEGRITY + ERRCODE_DETAILRECORDSEXIST)
#define DBIERR_MASTERTBLLEVEL      (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLLEVEL)
#define DBIERR_LOOKUPTABLEERR      (ERRBASE_INTEGRITY + ERRCODE_LOOKUPTABLEERR)
#define DBIERR_LOOKUPTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_LOOKUPTBLOPENERR)
#define DBIERR_DETAILTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_DETAILTBLOPENERR)
#define DBIERR_MASTERTBLOPENERR    (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLOPENERR)
#define DBIERR_FIELDISBLANK        (ERRBASE_INTEGRITY + ERRCODE_FIELDISBLANK)
#define DBIERR_MASTEREXISTS        (ERRBASE_INTEGRITY + ERRCODE_MASTEREXISTS)
#define DBIERR_MASTERTBLOPEN       (ERRBASE_INTEGRITY + ERRCODE_MASTERTBLOPEN)
#define DBIERR_DETAILTABLESEXIST   (ERRBASE_INTEGRITY + ERRCODE_DETAILTABLESEXIST)
#define DBIERR_DETAILRECEXISTEMPTY (ERRBASE_INTEGRITY + ERRCODE_DETAILRECEXISTEMPTY)
#define DBIERR_MASTERREFERENCEERR  (ERRBASE_INTEGRITY + ERRCODE_MASTERREFERENCEERR)
#define DBIERR_DETAILTBLOPEN       (ERRBASE_INTEGRITY + ERRCODE_DETAILTBLOPEN)
#define DBIERR_DEPENDENTSMUSTBEEMPTY  (ERRBASE_INTEGRITY + ERRCODE_DEPENDENTSMUSTBEEMPTY)
#define DBIERR_RINTREQINDEX        (ERRBASE_INTEGRITY + ERRCODE_RINTREQINDEX)
#define DBIERR_LINKEDTBLPROTECTED  (ERRBASE_INTEGRITY + ERRCODE_LINKEDTBLPROTECTED)
#define DBIERR_FIELDMULTILINKED    (ERRBASE_INTEGRITY + ERRCODE_FIELDMULTILINKED)



// ERRCAT_INVALIDREQ
// =================

#define  ERRCODE_OUTOFRANGE           1     // Number out of range (e.g field no)
#define  ERRCODE_INVALIDPARAM         2     // Generic invalid parameter
#define  ERRCODE_INVALIDFILENAME      3     // Invalid file name
#define  ERRCODE_NOSUCHFILE           4     // No such file
#define  ERRCODE_INVALIDOPTION        5     // Invalid option for a parameter
#define  ERRCODE_INVALIDHNDL          6     // Invalid handle to the function
#define  ERRCODE_UNKNOWNTBLTYPE       7     // Table type given not known
#define  ERRCODE_UNKNOWNFILE          8     // Dont know how to open file
#define  ERRCODE_PRIMARYKEYREDEFINE   9     // Cannot redefine primary key
#define  ERRCODE_INVALIDRINTDESCNUM   10    // 0x0a Cannot change this RINTDesc
#define  ERRCODE_KEYFLDTYPEMISMATCH   11    // 0x0b Foreign & Primary Key Mismatch
#define  ERRCODE_INVALIDMODIFYREQUEST 12    // 0x0c Invalid modify request
#define  ERRCODE_NOSUCHINDEX          13    // 0x0d Index does not exist
#define  ERRCODE_INVALIDBLOBOFFSET    14    // 0x0e Invalid Offset into the Blob
#define  ERRCODE_INVALIDDESCNUM       15    // 0x0f Invalid descriptor number
#define  ERRCODE_INVALIDFLDTYPE       16    // 0x10 Invalid field type
#define  ERRCODE_INVALIDFLDDESC       17    // 0x11 Invalid field descriptor
#define  ERRCODE_INVALIDFLDXFORM      18    // 0x12 Invalid field transform
#define  ERRCODE_INVALIDRECSTRUCT     19    // 0x13 Invalid record structure
#define  ERRCODE_INVALIDDESC          20    // 0x14 Generic: invalid descriptor
#define  ERRCODE_INVALIDINDEXSTRUCT   21    // 0x15 Invalid array of indexes descriptors
#define  ERRCODE_INVALIDVCHKSTRUCT    22    // 0x16 Invalid array of  val. check descriptors
#define  ERRCODE_INVALIDRINTSTRUCT    23    // 0x17 Invalid array of ref. integrity descriptors
#define  ERRCODE_INVALIDRESTRTBLORDER 24    // 0x18 Invalid ordering of tables during restructure
#define  ERRCODE_NAMENOTUNIQUE        25    // 0x19 Name not unique in this context
#define  ERRCODE_INDEXNAMEREQUIRED    26    // 0x1a Index name required
#define  ERRCODE_INVALIDSESHANDLE     27    // 0x1b Invalid ses handle
#define  ERRCODE_INVALIDRESTROP       28    // 0x1c Invalid restructure operation
#define  ERRCODE_UNKNOWNDRIVER        29    // 0x1d Driver not known to system
#define  ERRCODE_UNKNOWNDB            30    // 0x1e Unknown db
#define  ERRCODE_INVALIDPASSWORD      31    // 0x1f Invalid password given
#define  ERRCODE_NOCALLBACK           32    // 0x20 No callback function
#define  ERRCODE_INVALIDCALLBACKBUFLEN 33   // 0x21 Invalid callback buffer length
#define  ERRCODE_INVALIDDIR            34   // 0x22 Invalid directory
#define  ERRCODE_INVALIDXLATION        35   // 0x23 Translate Error - Translate DID NOT happen
#define  ERRCODE_DIFFERENTTABLES       36   // 0x24 Cannot Set Cursor of one Table to another
#define  ERRCODE_INVALIDBOOKMARK       37   // 0x25 Bookmarks does not match table, etc.
#define  ERRCODE_INVALIDINDEXNAME      38   // 0x26 Index/Tag Name is invalid
#define  ERRCODE_INVALIDIDXDESC        39   // 0x27 Invalid index descriptor
#define  ERRCODE_NOSUCHTABLE           40   // 0x28 No such table
#define  ERRCODE_USECOUNT              41   // 0x29 Table has too many users
#define  ERRCODE_INVALIDKEY            42   // 0x2a Key does not pass filter condition
#define  ERRCODE_INDEXEXISTS           43   // 0x2b Index already exists
#define  ERRCODE_INDEXOPEN             44   // 0x2c Index is open
#define  ERRCODE_INVALIDBLOBLEN        45   // 0x2d Invalid Blob Length
#define  ERRCODE_INVALIDBLOBHANDLE     46   // 0x2e Invalid Blob handle (in record buffer)
#define  ERRCODE_TABLEOPEN             47   // 0x2f Table is open
#define  ERRCODE_NEEDRESTRUCTURE       48   // 0x30 Need to do (hard) restructure
#define  ERRCODE_INVALIDMODE           49   // 0x31 Invalid mode
#define  ERRCODE_CANNOTCLOSE            50  // 0x32 Cannot close index
#define  ERRCODE_ACTIVEINDEX            51  // 0x33 Index is being used to order tbl
#define  ERRCODE_INVALIDUSRPASS         52  // 0x34 Bad user name or password
#define  ERRCODE_MULTILEVELCASCADE      53  // 0x35 Multi level Cascade not supported
#define  ERRCODE_INVALIDFIELDNAME       54  // 0x36 Invalid field name
#define  ERRCODE_INVALIDTABLENAME       55  // 0x37 Invalid table name
#define  ERRCODE_INVALIDLINKEXPR        56  // 0x38 Invalid linked cursor expression
#define  ERRCODE_NAMERESERVED           57  // 0x39 Name is reserved
#define  ERRCODE_INVALIDFILEEXTN        58  // 0x3a Invalid file extention
#define  ERRCODE_INVALIDLANGDRV         59  // 0x3b Invalid language driver
#define  ERRCODE_ALIASNOTOPEN           60  // 0x3c Requested alias in not open
#define  ERRCODE_INCOMPATRECSTRUCTS     61  // 0x3d Incompatible record structures
#define  ERRCODE_RESERVEDDOSNAME        62  // 0x3e Reserved dos name
#define  ERRCODE_DESTMUSTBEINDEXED      63  // 0x3f Destination must be indexed
#define  ERRCODE_INVALIDINDEXTYPE       64  // 0x40 Invalid index type
#define  ERRCODE_LANGDRVMISMATCH        65  // 0x41 Language driver of table and index do not match
#define  ERRCODE_NOSUCHFILTER           66  // 0x42 Filter handle is invalid
#define  ERRCODE_INVALIDFILTER          67  // 0x43 Invalid filter

#define  ERRCODE_INVALIDTABLECREATE     68  // 0x44 Bad table create request (exact prob unknown)
#define  ERRCODE_INVALIDTABLEDELETE     69  // 0x45 Bad table delete request (exact prob unknown)
#define  ERRCODE_INVALIDINDEXCREATE     70  // 0x46 Bad index create request (exact prob unknown)
#define  ERRCODE_INVALIDINDEXDELETE     71  // 0x47 Bad index delete request (exact prob unknown)
#define  ERRCODE_INVALIDTABLE           72  // 0x48 Invalid table name specified
#define  ERRCODE_MULTIRESULTS           73  // 0X49 Multi results
#define  ERRCODE_INVALIDTIME            74  // 0X4A Multi results
#define  ERRCODE_INVALIDDATE            75  // 0X4B Multi results
#define  ERRCODE_INVALIDTIMESTAMP       76  // 0X4C Multi results
#define  ERRCODE_DIFFERENTPATH          77  // 0X4d Tables in different paths
#define  ERRCODE_MISMATCHARGS           78  // 0x4e MisMatch in the # of arguments
#define  ERRCODE_FUNCTIONNOTFOUND       79  // 0x4f Loaderlib cant find a func in the DLL (bad version?)
#define  ERRCODE_MUSTUSEBASEORDER       80  // 0x50 Must use baseorder for this operation
#define  ERRCODE_INVALIDPROCEDURENAME   81  // 0x51 Invalid procedure name
#define  ERRCODE_INVALIDFLDMAP          82  // 0x52 invalid field map


#define  DBIERR_OUTOFRANGE           (ERRBASE_INVALIDREQ + ERRCODE_OUTOFRANGE)
#define  DBIERR_INVALIDPARAM         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDPARAM)
#define  DBIERR_INVALIDFILENAME      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILENAME)
#define  DBIERR_NOSUCHFILE           (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHFILE)
#define  DBIERR_INVALIDOPTION        (ERRBASE_INVALIDREQ + ERRCODE_INVALIDOPTION)
#define  DBIERR_INVALIDHNDL          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDHNDL)
#define  DBIERR_UNKNOWNTBLTYPE       (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNTBLTYPE)
#define  DBIERR_UNKNOWNFILE          (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNFILE)
#define  DBIERR_PRIMARYKEYREDEFINE   (ERRBASE_INVALIDREQ + ERRCODE_PRIMARYKEYREDEFINE)
#define  DBIERR_INVALIDRINTDESCNUM   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRINTDESCNUM)
#define  DBIERR_KEYFLDTYPEMISMATCH   (ERRBASE_INVALIDREQ + ERRCODE_KEYFLDTYPEMISMATCH)
#define  DBIERR_INVALIDMODIFYREQUEST (ERRBASE_INVALIDREQ + ERRCODE_INVALIDMODIFYREQUEST)
#define  DBIERR_NOSUCHINDEX          (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHINDEX)
#define  DBIERR_INVALIDBLOBOFFSET    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBOFFSET)
#define  DBIERR_INVALIDDESCNUM       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDESCNUM)
#define  DBIERR_INVALIDFLDTYPE       (ERRBASE_INVALIDREQ +  ERRCODE_INVALIDFLDTYPE)
#define  DBIERR_INVALIDFLDDESC       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFLDDESC)
#define  DBIERR_INVALIDFLDXFORM      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFLDXFORM)
#define  DBIERR_INVALIDRECSTRUCT     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRECSTRUCT)
#define  DBIERR_INVALIDDESC          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDESC)
#define  DBIERR_INVALIDINDEXSTRUCT   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXSTRUCT)
#define  DBIERR_INVALIDVCHKSTRUCT    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDVCHKSTRUCT)
#define  DBIERR_INVALIDRINTSTRUCT    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRINTSTRUCT)
#define  DBIERR_INVALIDRESTRTBLORDER (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRESTRTBLORDER)
#define  DBIERR_NAMENOTUNIQUE        (ERRBASE_INVALIDREQ + ERRCODE_NAMENOTUNIQUE)
#define  DBIERR_INDEXNAMEREQUIRED    (ERRBASE_INVALIDREQ + ERRCODE_INDEXNAMEREQUIRED)
#define  DBIERR_INVALIDSESHANDLE     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDSESHANDLE)
#define  DBIERR_INVALIDRESTROP       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDRESTROP)
#define  DBIERR_UNKNOWNDRIVER        (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNDRIVER)
#define  DBIERR_UNKNOWNDB            (ERRBASE_INVALIDREQ + ERRCODE_UNKNOWNDB)
#define  DBIERR_INVALIDPASSWORD      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDPASSWORD)
#define  DBIERR_NOCALLBACK           (ERRBASE_INVALIDREQ + ERRCODE_NOCALLBACK)
#define  DBIERR_INVALIDCALLBACKBUFLEN \
  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDCALLBACKBUFLEN )
#define  DBIERR_INVALIDDIR          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDIR)
#define  DBIERR_INVALIDXLATION      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDXLATION)
#define  DBIERR_DIFFERENTTABLES     (ERRBASE_INVALIDREQ + ERRCODE_DIFFERENTTABLES)
#define  DBIERR_INVALIDBOOKMARK     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBOOKMARK)
#define  DBIERR_INVALIDINDEXNAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXNAME)
#define  DBIERR_INVALIDIDXDESC      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDIDXDESC)
#define  DBIERR_NOSUCHTABLE         (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHTABLE)
#define  DBIERR_USECOUNT            (ERRBASE_INVALIDREQ + ERRCODE_USECOUNT)
#define  DBIERR_INVALIDKEY          (ERRBASE_INVALIDREQ + ERRCODE_INVALIDKEY)
#define  DBIERR_INDEXEXISTS         (ERRBASE_INVALIDREQ + ERRCODE_INDEXEXISTS)
#define  DBIERR_INDEXOPEN           (ERRBASE_INVALIDREQ + ERRCODE_INDEXOPEN)
#define  DBIERR_INVALIDBLOBLEN      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBLEN)
#define  DBIERR_INVALIDBLOBHANDLE   (ERRBASE_INVALIDREQ + ERRCODE_INVALIDBLOBHANDLE)
#define  DBIERR_TABLEOPEN           (ERRBASE_INVALIDREQ + ERRCODE_TABLEOPEN)
#define  DBIERR_NEEDRESTRUCTURE     (ERRBASE_INVALIDREQ + ERRCODE_NEEDRESTRUCTURE)
#define  DBIERR_INVALIDMODE         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDMODE)
#define  DBIERR_CANNOTCLOSE         (ERRBASE_INVALIDREQ + ERRCODE_CANNOTCLOSE)
#define  DBIERR_ACTIVEINDEX         (ERRBASE_INVALIDREQ + ERRCODE_ACTIVEINDEX)
#define  DBIERR_INVALIDUSRPASS      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDUSRPASS)
#define  DBIERR_MULTILEVELCASCADE   (ERRBASE_INVALIDREQ + ERRCODE_MULTILEVELCASCADE)
#define  DBIERR_INVALIDFIELDNAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFIELDNAME)
#define  DBIERR_INVALIDTABLENAME    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLENAME)
#define  DBIERR_INVALIDLINKEXPR     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDLINKEXPR)
#define  DBIERR_NAMERESERVED        (ERRBASE_INVALIDREQ + ERRCODE_NAMERESERVED)
#define  DBIERR_INVALIDFILEEXTN     (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILEEXTN)
#define  DBIERR_INVALIDLANGDRV      (ERRBASE_INVALIDREQ + ERRCODE_INVALIDLANGDRV)
#define  DBIERR_ALIASNOTOPEN        (ERRBASE_INVALIDREQ + ERRCODE_ALIASNOTOPEN)
#define  DBIERR_INCOMPATRECSTRUCTS  (ERRBASE_INVALIDREQ + ERRCODE_INCOMPATRECSTRUCTS)
#define  DBIERR_RESERVEDOSNAME      (ERRBASE_INVALIDREQ + ERRCODE_RESERVEDDOSNAME)
#define  DBIERR_DESTMUSTBEINDEXED   (ERRBASE_INVALIDREQ + ERRCODE_DESTMUSTBEINDEXED)
#define  DBIERR_INVALIDINDEXTYPE    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXTYPE)
#define  DBIERR_LANGDRVMISMATCH     (ERRBASE_INVALIDREQ + ERRCODE_LANGDRVMISMATCH)
#define  DBIERR_NOSUCHFILTER        (ERRBASE_INVALIDREQ + ERRCODE_NOSUCHFILTER)
#define  DBIERR_INVALIDFILTER       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFILTER)
#define  DBIERR_INVALIDTABLECREATE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLECREATE)
#define  DBIERR_INVALIDTABLEDELETE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLEDELETE)
#define  DBIERR_INVALIDINDEXCREATE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXCREATE)
#define  DBIERR_INVALIDINDEXDELETE  (ERRBASE_INVALIDREQ + ERRCODE_INVALIDINDEXDELETE)
#define  DBIERR_INVALIDTABLE        (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTABLE)
#define  DBIERR_MULTIRESULTS        (ERRBASE_INVALIDREQ + ERRCODE_MULTIRESULTS)
#define  DBIERR_INVALIDTIME         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTIME)
#define  DBIERR_INVALIDDATE         (ERRBASE_INVALIDREQ + ERRCODE_INVALIDDATE)
#define  DBIERR_INVALIDTIMESTAMP    (ERRBASE_INVALIDREQ + ERRCODE_INVALIDTIMESTAMP)
#define  DBIERR_DIFFERENTPATH       (ERRBASE_INVALIDREQ + ERRCODE_DIFFERENTPATH)
#define  DBIERR_MISMATCHARGS        (ERRBASE_INVALIDREQ + ERRCODE_MISMATCHARGS)
#define  DBIERR_FUNCTIONNOTFOUND    (ERRBASE_INVALIDREQ + ERRCODE_FUNCTIONNOTFOUND)
#define  DBIERR_MUSTUSEBASEORDER    (ERRBASE_INVALIDREQ + ERRCODE_MUSTUSEBASEORDER)
#define  DBIERR_INVALIDPROCEDURENAME (ERRBASE_INVALIDREQ + ERRCODE_INVALIDPROCEDURENAME)
#define  DBIERR_INVALIDFLDMAP       (ERRBASE_INVALIDREQ + ERRCODE_INVALIDFLDMAP)

// ERRCAT_LOCKCONFLICT
// ===================

#define ERRCODE_LOCKED           1
#define ERRCODE_UNLOCKFAILED     2
#define ERRCODE_FILEBUSY         3
#define ERRCODE_DIRBUSY          4
#define ERRCODE_FILELOCKED       5
#define ERRCODE_DIRLOCKED        6
#define ERRCODE_ALREADYLOCKED    7
#define ERRCODE_NOTLOCKED        8
#define ERRCODE_LOCKTIMEOUT      9
#define ERRCODE_GROUPLOCKED      10    // 0x0a
#define ERRCODE_LOSTTBLLOCK      11    // 0x0b
#define ERRCODE_LOSTEXCLACCESS   12    // 0x0c
#define ERRCODE_NEEDEXCLACCESS   13    // 0x0d
#define ERRCODE_RECGROUPCONFLICT 14    // 0x0e
#define ERRCODE_DEADLOCK         15
#define ERRCODE_ACTIVETRAN       16
#define ERRCODE_NOACTIVETRAN     17
#define ERRCODE_RECLOCKFAILED    18
#define ERRCODE_OPTRECLOCKFAILED 19
#define ERRCODE_OPTRECLOCKRECDEL 20
#define ERRCODE_LOCKEDRECS       21
#define ERRCODE_NEEDWRITELOCK    22

#define DBIERR_LOCKED            (ERRBASE_LOCKCONFLICT + ERRCODE_LOCKED)
#define DBIERR_UNLOCKFAILED      (ERRBASE_LOCKCONFLICT + ERRCODE_UNLOCKFAILED)
#define DBIERR_FILEBUSY          (ERRBASE_LOCKCONFLICT + ERRCODE_FILEBUSY)
#define DBIERR_DIRBUSY           (ERRBASE_LOCKCONFLICT + ERRCODE_DIRBUSY)
#define DBIERR_FILELOCKED        (ERRBASE_LOCKCONFLICT + ERRCODE_FILELOCKED)
#define DBIERR_DIRLOCKED         (ERRBASE_LOCKCONFLICT + ERRCODE_DIRLOCKED)
#define DBIERR_ALREADYLOCKED     (ERRBASE_LOCKCONFLICT + ERRCODE_ALREADYLOCKED)
#define DBIERR_NOTLOCKED         (ERRBASE_LOCKCONFLICT + ERRCODE_NOTLOCKED)
#define DBIERR_LOCKTIMEOUT       (ERRBASE_LOCKCONFLICT + ERRCODE_LOCKTIMEOUT)
#define DBIERR_GROUPLOCKED       (ERRBASE_LOCKCONFLICT + ERRCODE_GROUPLOCKED)
#define DBIERR_LOSTTBLLOCK       (ERRBASE_LOCKCONFLICT + ERRCODE_LOSTTBLLOCK)
#define DBIERR_LOSTEXCLACCESS    (ERRBASE_LOCKCONFLICT + ERRCODE_LOSTEXCLACCESS)
#define DBIERR_NEEDEXCLACCESS    (ERRBASE_LOCKCONFLICT  + ERRCODE_NEEDEXCLACCESS)
#define DBIERR_RECGROUPCONFLICT  (ERRBASE_LOCKCONFLICT + ERRCODE_RECGROUPCONFLICT)
#define DBIERR_DEADLOCK          (ERRBASE_LOCKCONFLICT + ERRCODE_DEADLOCK)
#define DBIERR_ACTIVETRAN        (ERRBASE_LOCKCONFLICT + ERRCODE_ACTIVETRAN)
#define DBIERR_NOACTIVETRAN      (ERRBASE_LOCKCONFLICT + ERRCODE_NOACTIVETRAN)
#define DBIERR_RECLOCKFAILED     (ERRBASE_LOCKCONFLICT + ERRCODE_RECLOCKFAILED)
#define DBIERR_OPTRECLOCKFAILED  (ERRBASE_LOCKCONFLICT + ERRCODE_OPTRECLOCKFAILED)
#define DBIERR_OPTRECLOCKRECDEL  (ERRBASE_LOCKCONFLICT + ERRCODE_OPTRECLOCKRECDEL)

// ERRCAT_SECURITY
// ===============

#define ERRCODE_NOTSUFFFIELDRIGHTS   1        // Not sufficient field  rights for operation
#define ERRCODE_NOTSUFFTABLERIGHTS   2        // Not sufficient table  rights for operation
#define ERRCODE_NOTSUFFFAMILYRIGHTS  3        // Not sufficient family rights for operation
#define ERRCODE_READONLYDIR          4        // Is a read-only directory
#define ERRCODE_READONLYDB           5        // Database is read-only
#define ERRCODE_READONLYFLD          6        // Trying to modify read-only field
#define ERRCODE_TBLENCRYPTED         7        // Table is encrypted (dBASE only)
#define ERRCODE_NOTSUFFSQLRIGHTS     8        // Not sufficient sql rights for operation


#define DBIERR_NOTSUFFFIELDRIGHTS  (ERRBASE_SEC + ERRCODE_NOTSUFFFIELDRIGHTS)
#define DBIERR_NOTSUFFTABLERIGHTS  (ERRBASE_SEC + ERRCODE_NOTSUFFTABLERIGHTS)
#define DBIERR_NOTSUFFFAMILYRIGHTS (ERRBASE_SEC + ERRCODE_NOTSUFFFAMILYRIGHTS)
#define DBIERR_READONLYDIR         (ERRBASE_SEC + ERRCODE_READONLYDIR)
#define DBIERR_READONLYDB          (ERRBASE_SEC + ERRCODE_READONLYDB)
#define DBIERR_READONLYFLD         (ERRBASE_SEC + ERRCODE_READONLYFLD)
#define DBIERR_TBLENCRYPTED        (ERRBASE_SEC + ERRCODE_TBLENCRYPTED)
#define DBIERR_NOTSUFFSQLRIGHTS    (ERRBASE_SEC + ERRCODE_NOTSUFFSQLRIGHTS)


// ERRCAT_INVALIDCONTEXT
// =====================

#define ERRCODE_NOTABLOB         1        // Field is not a blob
#define ERRCODE_BLOBOPENED       2        // Blob already opened
#define ERRCODE_BLOBNOTOPENED    3        // Blob not opened
#define ERRCODE_NA               4        // Operation not applicable
#define ERRCODE_NOTINDEXED       5        // Table is not indexed
#define ERRCODE_NOTINITIALIZED   6        // Engine not initialized
#define ERRCODE_MULTIPLEINIT     7        // Attempt to re-initialize engine
#define ERRCODE_NOTSAMESESSION   8        // Attempt to mix objs from diff ses
#define ERRCODE_PDXDRIVERNOTACTIVE 9      // Paradox driver not active
#define ERRCODE_DRIVERNOTLOADED  10       // 0x0a Driver not loaded
#define ERRCODE_TABLEREADONLY    11       // 0x0b Table is read only
#define ERRCODE_NOASSOCINDEX     12       // 0x0c No index associated with the cursor
#define ERRCODE_HASOPENCURSORS   13       // 0x0d Has open cursors
#define ERRCODE_NOTABLESUPPORT   14       // 0x0e Op cannot be done on this table
#define ERRCODE_INDEXREADONLY    15       // 0x0f Index is read only
#define ERRCODE_NOUNIQUERECS     16       // 0x10 Records are not unique
#define ERRCODE_NOTCURSESSION    17       // 0x11 Not the current/active session
#define ERRCODE_INVALIDKEYWORD   18       // 0x12 Invalid use of keyword.
#define ERRCODE_CONNECTINUSE     19       // 0x13 Connection in use
#define ERRCODE_CONNECTNOTSHARED 20       // 0x14 Passthru SQL connection not share


#define DBIERR_NOTABLOB           (ERRBASE_IC + ERRCODE_NOTABLOB)
#define DBIERR_BLOBOPENED         (ERRBASE_IC + ERRCODE_BLOBOPENED)
#define DBIERR_BLOBNOTOPENED      (ERRBASE_IC + ERRCODE_BLOBNOTOPENED)
#define DBIERR_NA                 (ERRBASE_IC + ERRCODE_NA)
#define DBIERR_NOTINDEXED         (ERRBASE_IC + ERRCODE_NOTINDEXED)
#define DBIERR_NOTINITIALIZED     (ERRBASE_IC + ERRCODE_NOTINITIALIZED)
#define DBIERR_MULTIPLEINIT       (ERRBASE_IC + ERRCODE_MULTIPLEINIT)
#define DBIERR_NOTSAMESESSION     (ERRBASE_IC + ERRCODE_NOTSAMESESSION)
#define DBIERR_PDXDRIVERNOTACTIVE (ERRBASE_IC + ERRCODE_PDXDRIVERNOTACTIVE)
#define DBIERR_DRIVERNOTLOADED    (ERRBASE_IC + ERRCODE_DRIVERNOTLOADED)
#define DBIERR_TABLEREADONLY      (ERRBASE_IC + ERRCODE_TABLEREADONLY)
#define DBIERR_NOASSOCINDEX       (ERRBASE_IC + ERRCODE_NOASSOCINDEX)
#define DBIERR_HASOPENCURSORS     (ERRBASE_IC + ERRCODE_HASOPENCURSORS)
#define DBIERR_NOTABLESUPPORT     (ERRBASE_IC + ERRCODE_NOTABLESUPPORT)
#define DBIERR_INDEXREADONLY      (ERRBASE_IC + ERRCODE_INDEXREADONLY)
#define DBIERR_NOUNIQUERECS       (ERRBASE_IC + ERRCODE_NOUNIQUERECS)
#define DBIERR_NOTCURSESSION      (ERRBASE_IC + ERRCODE_NOTCURSESSION)
#define DBIERR_INVALIDKEYWORD     (ERRBASE_IC + ERRCODE_INVALIDKEYWORD)
#define DBIERR_CONNECTINUSE       (ERRBASE_IC + ERRCODE_CONNECTINUSE)
#define DBIERR_CONNECTNOTSHARED   (ERRBASE_IC + ERRCODE_CONNECTNOTSHARED)


// ERRCAT_OS
// =========
// DOS extended errors:

#define ERRCODE_OSEINVFNC     1        // Invalid function number
#define ERRCODE_OSENOENT      2        // No such file or directory
#define ERRCODE_OSENOPATH     3        // Path not found
#define ERRCODE_OSEMFILE      4        // Too many open files
#define ERRCODE_OSEACCES      5        // Permission denied
#define ERRCODE_OSEBADF       6        // Bad file number
#define ERRCODE_OSECONTR      7        // Memory blocks destroyed
#define ERRCODE_OSENOMEM      8        // Not enough core
#define ERRCODE_OSEINVMEM     9        // Invalid memory block address
#define ERRCODE_OSEINVENV     10       // 0x0a Invalid environment
#define ERRCODE_OSEINVFMT     11       // 0x0b Invalid format
#define ERRCODE_OSEINVACC     12       // 0x0c Invalid access code
#define ERRCODE_OSEINVDAT     13       // 0x0d Invalid data
#define ERRCODE_OSENODEV      15       // 0x0f No such device
#define ERRCODE_OSECURDIR     16       // 0x10 Attempt to remove curdir
#define ERRCODE_OSENOTSAM     17       // 0x11 Not same device
#define ERRCODE_OSENMFILE     18       // 0x12 No more files
#define ERRCODE_OSEINVAL      19       // 0x13 Invalid argument
#define ERRCODE_OSE2BIG       20       // 0x14 Arg list too long
#define ERRCODE_OSENOEXEC     21       // 0x15 Exec format error
#define ERRCODE_OSEXDEV       22       // 0x16 Cross-device link
#define ERRCODE_OSEDOM        33       // 0x21 Math argument
#define ERRCODE_OSERANGE      34       // 0x22 Result to large
#define ERRCODE_OSEEXIST      35       // 0x23 File already exists
#define ERRCODE_OSUNKNOWN     39       // 0x27 Unkown | illegal error from rtl

#define ERRCODE_OSSHAREVIOL   50       // 0x32 Share viol, ext. err 0x20
#define ERRCODE_OSLOCKVIOL    51       // 0x33 Lock viol, ext. err 0x21
#define ERRCODE_OSINT24FAIL   52       // 0x34 INT24 called
#define ERRCODE_OSDRIVENOTREADY   53   // 0x35 Drive not ready



// OTHER Os errors:
// 1. idapi errors
// 2. errors from non-dos systems ( i.e. NOVELL )

#define ERRCODE_NOTEXACT          100       // 0x64 Not exact read/write
#define ERRCODE_OSNETERR          101       // 0x65 Generic network error
#define ERRCODE_OSUNKNOWNSRVERR   102       // 0x66 Error from file server
#define ERRCODE_SERVERNOMEMORY    103       // 0x67 Server out of memory
#define ERRCODE_OSALREADYLOCKED   104       // 0x68 Record already locked (by you)
#define ERRCODE_OSNOTLOCKED       105       // 0x69 Record not locked
#define ERRCODE_NOSERVERSW        106       // 0x6a Server software not running the workstation/server



#define   DBIERR_OSEINVFNC       ( ERRBASE_OS + ERRCODE_OSEINVFNC )
#define   DBIERR_OSENOENT        ( ERRBASE_OS + ERRCODE_OSENOENT )
#define   DBIERR_OSENOPATH       ( ERRBASE_OS + ERRCODE_OSENOPATH )
#define   DBIERR_OSEMFILE        ( ERRBASE_OS + ERRCODE_OSEMFILE )
#define   DBIERR_OSEACCES        ( ERRBASE_OS + ERRCODE_OSEACCES )
#define   DBIERR_OSEBADF         ( ERRBASE_OS + ERRCODE_OSEBADF )
#define   DBIERR_OSECONTR        ( ERRBASE_OS + ERRCODE_OSECONTR )
#define   DBIERR_OSENOMEM        ( ERRBASE_OS + ERRCODE_OSENOMEM )
#define   DBIERR_OSEINVMEM       ( ERRBASE_OS + ERRCODE_OSEINVMEM )
#define   DBIERR_OSEINVENV       ( ERRBASE_OS + ERRCODE_OSEINVENV )
#define   DBIERR_OSEINVFMT       ( ERRBASE_OS + ERRCODE_OSEINVFMT )
#define   DBIERR_OSEINVACC       ( ERRBASE_OS + ERRCODE_OSEINVACC )
#define   DBIERR_OSEINVDAT       ( ERRBASE_OS + ERRCODE_OSEINVDAT )
#define   DBIERR_OSENODEV        ( ERRBASE_OS + ERRCODE_OSENODEV )
#define   DBIERR_OSECURDIR       ( ERRBASE_OS + ERRCODE_OSECURDIR )
#define   DBIERR_OSENOTSAM       ( ERRBASE_OS + ERRCODE_OSENOTSAM )
#define   DBIERR_OSENMFILE       ( ERRBASE_OS + ERRCODE_OSENMFILE )
#define   DBIERR_OSEINVAL        ( ERRBASE_OS + ERRCODE_OSEINVAL )
#define   DBIERR_OSE2BIG         ( ERRBASE_OS + ERRCODE_OSE2BIG )
#define   DBIERR_OSENOEXEC       ( ERRBASE_OS + ERRCODE_OSENOEXEC )
#define   DBIERR_OSEXDEV         ( ERRBASE_OS + ERRCODE_OSEXDEV )
#define   DBIERR_OSEDOM          ( ERRBASE_OS + ERRCODE_OSEDOM )
#define   DBIERR_OSERANGE        ( ERRBASE_OS + ERRCODE_OSERANGE )
#define   DBIERR_OSEEXIST        ( ERRBASE_OS + ERRCODE_OSEEXIST )
#define   DBIERR_OSUNKNOWN       ( ERRBASE_OS + ERRCODE_OSUNKNOWN )
#define   DBIERR_OSSHAREVIOL     ( ERRBASE_OS + ERRCODE_OSSHAREVIOL )
#define   DBIERR_OSLOCKVIOL      ( ERRBASE_OS + ERRCODE_OSLOCKVIOL )
#define   DBIERR_OSNETERR        ( ERRBASE_OS + ERRCODE_OSNETERR )
#define   DBIERR_OSINT24FAIL     ( ERRBASE_OS + ERRCODE_OSINT24FAIL )
#define   DBIERR_OSDRIVENOTREADY ( ERRBASE_OS + ERRCODE_OSDRIVENOTREADY )


#define   DBIERR_NOTEXACT        ( ERRBASE_OS + ERRCODE_NOTEXACT )
#define   DBIERR_OSUNKNOWNSRVERR ( ERRBASE_OS + ERRCODE_OSUNKNOWNSRVERR )
#define   DBIERR_SERVERNOMEMORY  ( ERRBASE_OS + ERRCODE_SERVERNOMEMORY )
#define   DBIERR_OSALREADYLOCKED ( ERRBASE_OS + ERRCODE_OSALREADYLOCKED )
#define   DBIERR_OSNOTLOCKED     ( ERRBASE_OS + ERRCODE_OSNOTLOCKED )
#define   DBIERR_NOSERVERSW      ( ERRBASE_OS + ERRCODE_NOSERVERSW)

// ERRCAT_NETWORK
// ==============

#define ERRCODE_NETINITERR       1        // Net init failed
#define ERRCODE_NETUSERLIMIT     2        // Net user limit exceeded
#define ERRCODE_NETFILEVERSION   3        // Wrong net file version
#define ERRCODE_NETFILELOCKED    4        // Not able to lock net file
#define ERRCODE_DIRNOTPRIVATE    5        //
#define ERRCODE_NETMULTIPLE      6        // Multiple net files in use
#define ERRCODE_NETUNKNOWN       7        // Unknown net error
#define ERRCODE_SHAREDFILE       8        // Cannot access a shared file
#define ERRCODE_SHARENOTLOADED   9        // Share not loaded
#define ERRCODE_NOTONANETWORK    10       // 0x0a Not an Network
#define ERRCODE_SQLCOMMLOST      11       // 0x0b Lost Communication with SQL server
#define ERRCODE_SERVERCOMMLOST   12       // 0x0c Lost Communication with IDAPI server
#define ERRCODE_SQLSERVERNOTFOUND 13      // 0x0d SQL Server not found
#define ERRCODE_SERVERNOTFOUND   14       // 0x0e SQL Server not found

#define DBIERR_NETINITERR        (ERRBASE_NETWORK + ERRCODE_NETINITERR)
#define DBIERR_NETUSERLIMIT      (ERRBASE_NETWORK + ERRCODE_NETUSERLIMIT)
#define DBIERR_NETFILEVERSION    (ERRBASE_NETWORK + ERRCODE_NETFILEVERSION)
#define DBIERR_NETFILELOCKED     (ERRBASE_NETWORK + ERRCODE_NETFILELOCKED)
#define DBIERR_DIRNOTPRIVATE     (ERRBASE_NETWORK + ERRCODE_DIRNOTPRIVATE)
#define DBIERR_NETMULTIPLE       (ERRBASE_NETWORK + ERRCODE_NETMULTIPLE)
#define DBIERR_NETUNKNOWN        (ERRBASE_NETWORK + ERRCODE_NETUNKNOWN)
#define DBIERR_SHAREDFILE        (ERRBASE_NETWORK + ERRCODE_SHAREDFILE)
#define DBIERR_SHARENOTLOADED    (ERRBASE_NETWORK + ERRCODE_SHARENOTLOADED)
#define DBIERR_NOTONANETWORK     (ERRBASE_NETWORK + ERRCODE_NOTONANETWORK)
#define DBIERR_SQLCOMMLOST       (ERRBASE_NETWORK + ERRCODE_SQLCOMMLOST)
#define DBIERR_SERVERCOMMLOST    (ERRBASE_NETWORK + ERRCODE_SERVERCOMMLOST)
#define DBIERR_SQLSERVERNOTFOUND (ERRBASE_NETWORK + ERRCODE_SQLSERVERNOTFOUND)
#define DBIERR_SERVERNOTFOUND    (ERRBASE_NETWORK + ERRCODE_SERVERNOTFOUND)

// ERRCAT_DRIVER
// =============

#define ERRCODE_WRONGDRVNAME    1       // Wrong driver name
#define ERRCODE_WRONGSYSVER     2       // Wrong system version
#define ERRCODE_WRONGDRVVER     3       // Wrong driver version
#define ERRCODE_WRONGDRVTYPE    4       // Wrong driver type
#define ERRCODE_CANNOTLOADDRV   5       // Can not load driver
#define ERRCODE_CANNOTLOADLDDRV   6     // Can not load language driver
#define ERRCODE_VENDINITFAIL    7       // Vendor init failure
#define ERRCODE_DRIVERRESTRICTED  8     // Client not enabled for this driver


#define DBIERR_WRONGDRVNAME     (ERRBASE_DRIVER + ERRCODE_WRONGDRVNAME)
#define DBIERR_WRONGSYSVER      (ERRBASE_DRIVER + ERRCODE_WRONGSYSVER)
#define DBIERR_WRONGDRVVER      (ERRBASE_DRIVER + ERRCODE_WRONGDRVVER)
#define DBIERR_WRONGDRVTYPE     (ERRBASE_DRIVER + ERRCODE_WRONGDRVTYPE)
#define DBIERR_CANNOTLOADDRV    (ERRBASE_DRIVER + ERRCODE_CANNOTLOADDRV)
#define DBIERR_CANNOTLOADLDDRV  (ERRBASE_DRIVER + ERRCODE_CANNOTLOADLDDRV)
#define DBIERR_VENDINITFAIL     (ERRBASE_DRIVER + ERRCODE_VENDINITFAIL)
#define DBIERR_DRIVERRESTRICTED (ERRBASE_DRIVER + ERRCODE_DRIVERRESTRICTED)


// ERRCAT_QUERY
// ============



#define DBICODE_AMBJOASY       1 // obsolete
#define DBICODE_AMBJOSYM       2 // obsolete
#define DBICODE_AMBOUTEX       3
#define DBICODE_AMBOUTPR       4 // obsolete
#define DBICODE_AMBSYMAS       5 // obsolete
#define DBICODE_ASETOPER       6
#define DBICODE_AVENUMDA       7
#define DBICODE_BADEXPR1       8
#define DBICODE_BADFLDOR       9
#define DBICODE_BADVNAME      10                              // 0x0a
#define DBICODE_BITMAPER      11                              // 0x0b
#define DBICODE_CALCBADR      12                              // 0x0c
#define DBICODE_CALCTYPE      13                              // 0x0d
#define DBICODE_CHGTO1TI      14                              // 0x0e
#define DBICODE_CHGTOCHG      15                              // 0x0f
#define DBICODE_CHGTOEXP      16                              // 0x10
#define DBICODE_CHGTOINS      17                              // 0x11
#define DBICODE_CHGTONEW      18                              // 0x12
#define DBICODE_CHGTOVAL      19                              // 0x13
#define DBICODE_CHKMRKFI      20                              // 0x14
#define DBICODE_CHNAMBIG      21                              // 0x15
#define DBICODE_CHUNKERR      22                              // 0x16
#define DBICODE_COLUM255      23                              // 0x17
#define DBICODE_CONAFTAS      24                              // 0x18
#define DBICODE_DEL1TIME      25                              // 0x19
#define DBICODE_DELAMBIG      26                              // 0x1a
#define DBICODE_DELFRDEL      27                              // 0x1b
#define DBICODE_EGFLDTYP      28                              // 0x1c
#define DBICODE_EXAMINOR      29                              // 0x1d
#define DBICODE_EXPRTYPS      30                              // 0x1e
#define DBICODE_EXTRACOM      31                              // 0x1f
#define DBICODE_EXTRAORO      32                              // 0x20
#define DBICODE_EXTRAQRO      33                              // 0x21
#define DBICODE_FIND1ATT      34                              // 0x22
#define DBICODE_FINDANST      35                              // 0x23
#define DBICODE_GRPNOSET      36                              // 0x24
#define DBICODE_GRPSTROW      37                              // 0x25
#define DBICODE_IDFINLCO      38                              // 0x26
#define DBICODE_IDFPERLI      39                              // 0x27
#define DBICODE_INANEXPR      40                              // 0x28
#define DBICODE_INS1TIME      41                              // 0x29
#define DBICODE_INSAMBIG      42                              // 0x2a
#define DBICODE_INSDELCH      43                              // 0x2b
#define DBICODE_INSEXPRR      44                              // 0x2c
#define DBICODE_INSTOINS      45                              // 0x2d
#define DBICODE_ISARRAY       46                              // 0x2e
#define DBICODE_LABELERR      47                              // 0x2f
#define DBICODE_LINKCALC      48                              // 0x30
#define DBICODE_LNGVNAME      49                              // 0x31
#define DBICODE_LONGQURY      50                              // 0x32
#define DBICODE_MEMVPROC      51                              // 0x33
#define DBICODE_MISNGCOM      52                              // 0x34
#define DBICODE_MISNGRPA      53                              // 0x35
#define DBICODE_MISSRTQU      54                              // 0x36
#define DBICODE_NAMTWICE      55                              // 0x37
#define DBICODE_NOCHKMAR      56                              // 0x38
#define DBICODE_NODEFOCC      57                              // 0x39
#define DBICODE_NOGROUPS      58                              // 0x3a
#define DBICODE_NONSENSE      59                              // 0x3b
#define DBICODE_NOPATTER      60                              // 0x3c
#define DBICODE_NOSUCHDA      61                              // 0x3d
#define DBICODE_NOVALUE       62                              // 0x3e
#define DBICODE_ONLYCONS      63                              // 0x3f
#define DBICODE_ONLYSETR      64                              // 0x40
#define DBICODE_OUTSENS1      65                              // 0x41
#define DBICODE_OUTTWIC1      66                              // 0x42
#define DBICODE_PAROWCNT      67                              // 0x43
#define DBICODE_PERSEPAR      68                              // 0x44
#define DBICODE_PROCPLSW      69                              // 0x45
#define DBICODE_PWINSRTS      70                              // 0x46
#define DBICODE_PWMODRTS      71                              // 0x47
#define DBICODE_QBEFLDFOUND   72                              // 0x48
#define DBICODE_QBENOFENCE    73                              // 0x49
#define DBICODE_QBENOFENCET   74                              // 0x4a
#define DBICODE_QBENOHEADERT  75                              // 0x4b
#define DBICODE_QBENOTAB      76                              // 0x4c
#define DBICODE_QBENUMCOLS    77                              // 0x4d
#define DBICODE_QBEOPENTAB    78                              // 0x4e
#define DBICODE_QBETWICE      79                              // 0x4f
#define DBICODE_QRYNOANSWER   80                              // 0x50
#define DBICODE_QRYNOTPREP    81                              // 0x51
#define DBICODE_QUAINDEL      82                              // 0x52
#define DBICODE_QUAININS      83                              // 0x53
#define DBICODE_RAGININS      84                              // 0x54
#define DBICODE_RAGINSET      85                              // 0x55
#define DBICODE_ROWUSERR      86                              // 0x56
#define DBICODE_SETEXPEC      87                              // 0x57
#define DBICODE_SETVAMB1      88                              // 0x58
#define DBICODE_SETVBAD1      89                              // 0x59
#define DBICODE_SETVDEF1      90                              // 0x5a
#define DBICODE_SUMNUMBE      91                              // 0x5b
#define DBICODE_TBLISWP3      92                              // 0x5c
#define DBICODE_TOKENNOT      93                              // 0x5d
#define DBICODE_TWOOUTR1      94                              // 0x5e
#define DBICODE_TYPEMISM      95                              // 0x5f
#define DBICODE_UNRELQ1       96                              // 0x60
#define DBICODE_UNUSEDST      97                              // 0x61
#define DBICODE_USEINSDE      98                              // 0x62
#define DBICODE_USEOFCHG      99                              // 0x63
#define DBICODE_VARMUSTF     100                              // 0x64
#define DBICODE_REGISTER     101                              // 0x65
#define DBICODE_LONGEXPR     102                              // 0x66
#define DBICODE_REFRESH      103                              // 0x67
#define DBICODE_CANCEXCEPT   104                              // 0x68
#define DBICODE_DBEXCEPT     105                              // 0x69
#define DBICODE_MEMEXCEPT    106                              // 0x6a
#define DBICODE_FATALEXCEPT  107                              // 0x6b
#define DBICODE_QRYNIY       108                              // 0x6c
#define DBICODE_BADFORMAT    109                              // 0x6d
#define DBICODE_QRYEMPTY     110                              // 0x6e
#define DBICODE_NOQRYTOPREP  111                              // 0x6f
#define DBICODE_BUFFTOOSMALL 112                              // 0x70
#define DBICODE_QRYNOTPARSE  113                              // 0x71
#define DBICODE_NOTHANDLE    114                              // 0x72
#define DBICODE_QRYSYNTERR   115                              // 0x73
#define DBICODE_QXFLDCOUNT   116                              // 0x74
#define DBICODE_QXFLDSYMNOTFOUND 117                          // 0x75
#define DBICODE_QXTBLSYMNOTFOUND 118                          // 0x76
#define DBICODE_BLOBTERM     119                              // 0x77
#define DBICODE_BLOBERR      120                              // 0x78
#define DBICODE_RESTARTQRY   121                              // 0x79
#define DBICODE_UNKNOWNANSTYPE 122                            // 0x7a

// Internal QBE use Only.
   #define DBICODE_SQLG_MDIST 123                           // 0x7b
   #define DBICODE_SQLG_NOARI 124                           // 0x7c
   #define DBICODE_SQLG_LIKEN 125                           // 0x7d
   #define DBICODE_SQLG_ALPHO 126                           // 0x7e
   #define DBICODE_SQLG_DATEO 127                           // 0x7f
   #define DBICODE_SQLG_RELOP 128                           // 0x80
   #define DBICODE_SQLG_ONLYC 129                           // 0x81
   #define DBICODE_SQLG_CNTLN 130                           // 0x82
   #define DBICODE_SQLG_CHINI 131                           // 0x83
   #define DBICODE_SQLG_UNION 132                           // 0x84
   #define DBICODE_SQLG_SLFIN 133                           // 0x85
   #define DBICODE_SQLG_OTJVR 134                           // 0x86
   #define DBICODE_SQLG_STROW 135                           // 0x87
   #define DBICODE_SQLG_QUANT 136                           // 0x88
   #define DBICODE_SQLG_REGSO 137                           // 0x89
   #define DBICODE_SQLG_COUNT 138                           // 0x8a
   #define DBICODE_SQLG_AVERA 139                           // 0x8b
   #define DBICODE_SQLG_DATEA 140                           // 0x8c
   #define DBICODE_SQLG_BADPT 141                           // 0x8d
   #define DBICODE_SQLG_RELPA 142                           // 0x8e
   #define DBICODE_SQLG_PATRN 143                           // 0x8f
   #define DBICODE_SQLG_FNDSU 144                           // 0x90
   #define DBICODE_SQLG_IFDCS 145                           // 0x91
   #define DBICODE_SQLG_IDCCO 146                           // 0x92
   #define DBICODE_SQLG_ONLYI 147                           // 0x93
   #define DBICODE_SQLG_SQLDIALECT 148                      // 0x94
   #define DBICODE_SQLG_NOQUERY 149                         // 0x95
// End of Internal.

#define DBICODE_BLOBGROUP 150                               // 0x96
#define DBICODE_QRYNOPROP  151                              // 0x97
#define DBICODE_ANSTYPNOTSUP  152                           // 0x98
#define DBICODE_ANSALIASNOTSUP  153                         // 0x99
#define DBICODE_INSBLOBREQ      154                         // 0x9a
#define DBICODE_CHGUNIQUENDXREQ 155                         // 0x9b
#define DBICODE_DELUNIQUENDXREQ 156                         // 0x9c
#define DBICODE_SQLNOFULLUPDATE 157                         // 0x9d
#define DBICODE_CANTEXECREMOTE  158                         // 0x9e
#define DBICODE_UNEXPECTEDEOC   159                         // 0x9f
#define DBICODE_SQLPARAMNOTSET  160                         // 0xA0
#define DBICODE_QUERYTOOLONG    161                         // 0xA1


// Errors added for localsql
#define  DBICODE_NOSUCHRELORALIAS    170
#define  DBICODE_TYPEAMBIGUITY       171
#define  DBICODE_ORDERBYNOTAPROJ     172
#define  DBICODE_SQLPARSE            173
#define  DBICODE_CONSTRAINTFAILED    174
#define  DBICODE_NOTGROUPINGFIELD    175
#define  DBICODE_UDFNOTDEFINED       176
#define  DBICODE_UDFERROR            177
#define  DBICODE_SINGLEROWERROR      178
#define  DBICODE_GROUPEXPR           179
#define  DBICODE_QUERYTEXT           180
#define  DBICODE_ANSIJOINSUP         181
#define  DBICODE_DISTUNION           182
#define  DBICODE_GROUPBYREQ          183
#define  DBICODE_INSUPDAUTOIC        184
#define  DBICODE_UPDREFINTSINGLE     185



#define DBIERR_AMBJOASY    (ERRBASE_QUERY+DBICODE_AMBJOASY)
#define DBIERR_AMBJOSYM    (ERRBASE_QUERY+DBICODE_AMBJOSYM)
#define DBIERR_AMBOUTEX    (ERRBASE_QUERY+DBICODE_AMBOUTEX)
#define DBIERR_AMBOUTPR    (ERRBASE_QUERY+DBICODE_AMBOUTPR)
#define DBIERR_AMBSYMAS    (ERRBASE_QUERY+DBICODE_AMBSYMAS)
#define DBIERR_ASETOPER    (ERRBASE_QUERY+DBICODE_ASETOPER)
#define DBIERR_AVENUMDA    (ERRBASE_QUERY+DBICODE_AVENUMDA)
#define DBIERR_BADEXPR1    (ERRBASE_QUERY+DBICODE_BADEXPR1)
#define DBIERR_BADFLDOR    (ERRBASE_QUERY+DBICODE_BADFLDOR)
#define DBIERR_BADVNAME    (ERRBASE_QUERY+DBICODE_BADVNAME)
#define DBIERR_BITMAPER    (ERRBASE_QUERY+DBICODE_BITMAPER)
#define DBIERR_CALCBADR    (ERRBASE_QUERY+DBICODE_CALCBADR)
#define DBIERR_CALCTYPE    (ERRBASE_QUERY+DBICODE_CALCTYPE)
#define DBIERR_CHGTO1TI    (ERRBASE_QUERY+DBICODE_CHGTO1TI)
#define DBIERR_CHGTOCHG    (ERRBASE_QUERY+DBICODE_CHGTOCHG)
#define DBIERR_CHGTOEXP    (ERRBASE_QUERY+DBICODE_CHGTOEXP)
#define DBIERR_CHGTOINS    (ERRBASE_QUERY+DBICODE_CHGTOINS)
#define DBIERR_CHGTONEW    (ERRBASE_QUERY+DBICODE_CHGTONEW)
#define DBIERR_CHGTOVAL    (ERRBASE_QUERY+DBICODE_CHGTOVAL)
#define DBIERR_CHKMRKFI    (ERRBASE_QUERY+DBICODE_CHKMRKFI)
#define DBIERR_CHNAMBIG    (ERRBASE_QUERY+DBICODE_CHNAMBIG)
#define DBIERR_CHUNKERR    (ERRBASE_QUERY+DBICODE_CHUNKERR)
#define DBIERR_COLUM255    (ERRBASE_QUERY+DBICODE_COLUM255)
#define DBIERR_CONAFTAS    (ERRBASE_QUERY+DBICODE_CONAFTAS)
#define DBIERR_DEL1TIME    (ERRBASE_QUERY+DBICODE_DEL1TIME)
#define DBIERR_DELAMBIG    (ERRBASE_QUERY+DBICODE_DELAMBIG)
#define DBIERR_DELFRDEL    (ERRBASE_QUERY+DBICODE_DELFRDEL)
#define DBIERR_EGFLDTYP    (ERRBASE_QUERY+DBICODE_EGFLDTYP)
#define DBIERR_EXAMINOR    (ERRBASE_QUERY+DBICODE_EXAMINOR)
#define DBIERR_EXPRTYPS    (ERRBASE_QUERY+DBICODE_EXPRTYPS)
#define DBIERR_EXTRACOM    (ERRBASE_QUERY+DBICODE_EXTRACOM)
#define DBIERR_EXTRAORO    (ERRBASE_QUERY+DBICODE_EXTRAORO)
#define DBIERR_EXTRAQRO    (ERRBASE_QUERY+DBICODE_EXTRAQRO)
#define DBIERR_FIND1ATT    (ERRBASE_QUERY+DBICODE_FIND1ATT)
#define DBIERR_FINDANST    (ERRBASE_QUERY+DBICODE_FINDANST)
#define DBIERR_GRPNOSET    (ERRBASE_QUERY+DBICODE_GRPNOSET)
#define DBIERR_GRPSTROW    (ERRBASE_QUERY+DBICODE_GRPSTROW)
#define DBIERR_IDFINLCO    (ERRBASE_QUERY+DBICODE_IDFINLCO)
#define DBIERR_IDFPERLI    (ERRBASE_QUERY+DBICODE_IDFPERLI)
#define DBIERR_INANEXPR    (ERRBASE_QUERY+DBICODE_INANEXPR)
#define DBIERR_INS1TIME    (ERRBASE_QUERY+DBICODE_INS1TIME)
#define DBIERR_INSAMBIG    (ERRBASE_QUERY+DBICODE_INSAMBIG)
#define DBIERR_INSDELCH    (ERRBASE_QUERY+DBICODE_INSDELCH)
#define DBIERR_INSEXPRR    (ERRBASE_QUERY+DBICODE_INSEXPRR)
#define DBIERR_INSTOINS    (ERRBASE_QUERY+DBICODE_INSTOINS)
#define DBIERR_ISARRAY     (ERRBASE_QUERY+DBICODE_ISARRAY)
#define DBIERR_LABELERR    (ERRBASE_QUERY+DBICODE_LABELERR)
#define DBIERR_LINKCALC    (ERRBASE_QUERY+DBICODE_LINKCALC)
#define DBIERR_LNGVNAME    (ERRBASE_QUERY+DBICODE_LNGVNAME)
#define DBIERR_LONGQURY    (ERRBASE_QUERY+DBICODE_LONGQURY)
#define DBIERR_MEMVPROC    (ERRBASE_QUERY+DBICODE_MEMVPROC)
#define DBIERR_MISNGCOM    (ERRBASE_QUERY+DBICODE_MISNGCOM)
#define DBIERR_MISNGRPA    (ERRBASE_QUERY+DBICODE_MISNGRPA)
#define DBIERR_MISSRTQU    (ERRBASE_QUERY+DBICODE_MISSRTQU)
#define DBIERR_NAMTWICE    (ERRBASE_QUERY+DBICODE_NAMTWICE)
#define DBIERR_NOCHKMAR    (ERRBASE_QUERY+DBICODE_NOCHKMAR)
#define DBIERR_NODEFOCC    (ERRBASE_QUERY+DBICODE_NODEFOCC)
#define DBIERR_NOGROUPS    (ERRBASE_QUERY+DBICODE_NOGROUPS)
#define DBIERR_NONSENSE    (ERRBASE_QUERY+DBICODE_NONSENSE)
#define DBIERR_NOPATTER    (ERRBASE_QUERY+DBICODE_NOPATTER)
#define DBIERR_NOSUCHDA    (ERRBASE_QUERY+DBICODE_NOSUCHDA)
#define DBIERR_NOVALUE     (ERRBASE_QUERY+DBICODE_NOVALUE)
#define DBIERR_ONLYCONS    (ERRBASE_QUERY+DBICODE_ONLYCONS)
#define DBIERR_ONLYSETR    (ERRBASE_QUERY+DBICODE_ONLYSETR)
#define DBIERR_OUTSENS1    (ERRBASE_QUERY+DBICODE_OUTSENS1)
#define DBIERR_OUTTWIC1    (ERRBASE_QUERY+DBICODE_OUTTWIC1)
#define DBIERR_PAROWCNT    (ERRBASE_QUERY+DBICODE_PAROWCNT)
#define DBIERR_PERSEPAR    (ERRBASE_QUERY+DBICODE_PERSEPAR)
#define DBIERR_PROCPLSW    (ERRBASE_QUERY+DBICODE_PROCPLSW)
#define DBIERR_PWINSRTS    (ERRBASE_QUERY+DBICODE_PWINSRTS)
#define DBIERR_PWMODRTS    (ERRBASE_QUERY+DBICODE_PWMODRTS)
#define DBIERR_QBEFLDFOUND (ERRBASE_QUERY+DBICODE_QBEFLDFOUND)
#define DBIERR_QBENOFENCE  (ERRBASE_QUERY+DBICODE_QBENOFENCE)
#define DBIERR_QBENOFENCET (ERRBASE_QUERY+DBICODE_QBENOFENCET)
#define DBIERR_QBENOHEADERT (ERRBASE_QUERY+DBICODE_QBENOHEADERT)
#define DBIERR_QBENOTAB    (ERRBASE_QUERY+DBICODE_QBENOTAB)
#define DBIERR_QBENUMCOLS  (ERRBASE_QUERY+DBICODE_QBENUMCOLS)
#define DBIERR_QBEOPENTAB  (ERRBASE_QUERY+DBICODE_QBEOPENTAB)
#define DBIERR_QBETWICE    (ERRBASE_QUERY+DBICODE_QBETWICE)
#define DBIERR_QRYNOANSWER (ERRBASE_QUERY+DBICODE_QRYNOANSWER)
#define DBIERR_QRYNOTPREP  (ERRBASE_QUERY+DBICODE_QRYNOTPREP)
#define DBIERR_QUAINDEL    (ERRBASE_QUERY+DBICODE_QUAINDEL)
#define DBIERR_QUAININS    (ERRBASE_QUERY+DBICODE_QUAININS)
#define DBIERR_RAGININS    (ERRBASE_QUERY+DBICODE_RAGININS)
#define DBIERR_RAGINSET    (ERRBASE_QUERY+DBICODE_RAGINSET)
#define DBIERR_ROWUSERR    (ERRBASE_QUERY+DBICODE_ROWUSERR)
#define DBIERR_SETEXPEC    (ERRBASE_QUERY+DBICODE_SETEXPEC)
#define DBIERR_SETVAMB1    (ERRBASE_QUERY+DBICODE_SETVAMB1)
#define DBIERR_SETVBAD1    (ERRBASE_QUERY+DBICODE_SETVBAD1)
#define DBIERR_SETVDEF1    (ERRBASE_QUERY+DBICODE_SETVDEF1)
#define DBIERR_SUMNUMBE    (ERRBASE_QUERY+DBICODE_SUMNUMBE)
#define DBIERR_TBLISWP3    (ERRBASE_QUERY+DBICODE_TBLISWP3)
#define DBIERR_TOKENNOT    (ERRBASE_QUERY+DBICODE_TOKENNOT)
#define DBIERR_TWOOUTR1    (ERRBASE_QUERY+DBICODE_TWOOUTR1)
#define DBIERR_TYPEMISM    (ERRBASE_QUERY+DBICODE_TYPEMISM)
#define DBIERR_UNRELQ1     (ERRBASE_QUERY+DBICODE_UNRELQ1)
#define DBIERR_UNUSEDST    (ERRBASE_QUERY+DBICODE_UNUSEDST)
#define DBIERR_USEINSDE    (ERRBASE_QUERY+DBICODE_USEINSDE)
#define DBIERR_USEOFCHG    (ERRBASE_QUERY+DBICODE_USEOFCHG)
#define DBIERR_VARMUSTF    (ERRBASE_QUERY+DBICODE_VARMUSTF)
#define DBIERR_REGISTER    (ERRBASE_QUERY+DBICODE_REGISTER)
#define DBIERR_LONGEXPR    (ERRBASE_QUERY+DBICODE_LONGEXPR)
#define DBIERR_REFRESH     (ERRBASE_QUERY+DBICODE_REFRESH)
#define DBIERR_CANCEXCEPT  (ERRBASE_QUERY+DBICODE_CANCEXCEPT)
#define DBIERR_DBEXCEPT    (ERRBASE_QUERY+DBICODE_DBEXCEPT)
#define DBIERR_MEMEXCEPT   (ERRBASE_QUERY+DBICODE_MEMEXCEPT)
#define DBIERR_FATALEXCEPT (ERRBASE_QUERY+DBICODE_FATALEXCEPT)
#define DBIERR_QRYNIY      (ERRBASE_QUERY+ DBICODE_QRYNIY)
#define DBIERR_BADFORMAT   (ERRBASE_QUERY+ DBICODE_BADFORMAT)
#define DBIERR_QRYEMPTY    (ERRBASE_QUERY+ DBICODE_QRYEMPTY)
#define DBIERR_NOQRYTOPREP (ERRBASE_QUERY+ DBICODE_NOQRYTOPREP)
#define DBIERR_BUFFTOOSMALL (ERRBASE_QUERY+ DBICODE_BUFFTOOSMALL)
#define DBIERR_QRYNOTPARSE (ERRBASE_QUERY+ DBICODE_QRYNOTPARSE)
#define DBIERR_NOTHANDLE   (ERRBASE_QUERY+ DBICODE_NOTHANDLE)
#define DBIERR_QRYSYNTERR  (ERRBASE_QUERY+ DBICODE_QRYSYNTERR)
#define DBIERR_QXFLDCOUNT  (ERRBASE_QUERY+ DBICODE_QXFLDCOUNT)
#define DBIERR_QXFLDSYMNOTFOUND (ERRBASE_QUERY+ DBICODE_QXFLDSYMNOTFOUND)
#define DBIERR_QXTBLSYMNOTFOUND (ERRBASE_QUERY+ DBICODE_QXTBLSYMNOTFOUND)
#define DBIERR_BLOBTERM    (ERRBASE_QUERY+ DBICODE_BLOBTERM)
#define DBIERR_BLOBERR     (ERRBASE_QUERY+ DBICODE_BLOBERR)
#define DBIERR_RESTARTQRY  (ERRBASE_QUERY+ DBICODE_RESTARTQRY)
#define DBIERR_UNKNOWNANSTYPE (ERRBASE_QUERY+ DBICODE_UNKNOWNANSTYPE)
#define DBIERR_SQLG_MDIST  (ERRBASE_QUERY+ DBICODE_SQLG_MDIST)
#define DBIERR_SQLG_NOARI  (ERRBASE_QUERY+ DBICODE_SQLG_NOARI)
#define DBIERR_SQLG_LIKEN  (ERRBASE_QUERY+ DBICODE_SQLG_LIKEN)
#define DBIERR_SQLG_ALPHO  (ERRBASE_QUERY+ DBICODE_SQLG_ALPHO)
#define DBIERR_SQLG_DATEO  (ERRBASE_QUERY+ DBICODE_SQLG_DATEO)
#define DBIERR_SQLG_RELOP  (ERRBASE_QUERY+ DBICODE_SQLG_RELOP)
#define DBIERR_SQLG_ONLYC  (ERRBASE_QUERY+ DBICODE_SQLG_ONLYC)
#define DBIERR_SQLG_CNTLN  (ERRBASE_QUERY+ DBICODE_SQLG_CNTLN)
#define DBIERR_SQLG_CHINI  (ERRBASE_QUERY+ DBICODE_SQLG_CHINI)
#define DBIERR_SQLG_UNION  (ERRBASE_QUERY+ DBICODE_SQLG_UNION)
#define DBIERR_SQLG_SLFIN  (ERRBASE_QUERY+ DBICODE_SQLG_SLFIN)
#define DBIERR_SQLG_OTJVR  (ERRBASE_QUERY+ DBICODE_SQLG_OTJVR)
#define DBIERR_SQLG_STROW  (ERRBASE_QUERY+ DBICODE_SQLG_STROW)
#define DBIERR_SQLG_QUANT  (ERRBASE_QUERY+ DBICODE_SQLG_QUANT)
#define DBIERR_SQLG_REGSO  (ERRBASE_QUERY+ DBICODE_SQLG_REGSO)
#define DBIERR_SQLG_COUNT  (ERRBASE_QUERY+ DBICODE_SQLG_COUNT)
#define DBIERR_SQLG_AVERA  (ERRBASE_QUERY+ DBICODE_SQLG_AVERA)
#define DBIERR_SQLG_DATEA  (ERRBASE_QUERY+ DBICODE_SQLG_DATEA)
#define DBIERR_SQLG_BADPT  (ERRBASE_QUERY+ DBICODE_SQLG_BADPT)
#define DBIERR_SQLG_RELPA  (ERRBASE_QUERY+ DBICODE_SQLG_RELPA)
#define DBIERR_SQLG_PATRN  (ERRBASE_QUERY+ DBICODE_SQLG_PATRN)
#define DBIERR_SQLG_FNDSU  (ERRBASE_QUERY+ DBICODE_SQLG_FNDSU)
#define DBIERR_SQLG_IFDCS  (ERRBASE_QUERY+ DBICODE_SQLG_IFDCS)
#define DBIERR_SQLG_IDCCO  (ERRBASE_QUERY+ DBICODE_SQLG_IDCCO)
#define DBIERR_SQLG_ONLYI  (ERRBASE_QUERY+ DBICODE_SQLG_ONLYI)
#define DBIERR_SQLG_SQLDIALECT (ERRBASE_QUERY+ DBICODE_SQLG_SQLDIALECT)
#define DBIERR_SQLG_NOQUERY (ERRBASE_QUERY+ DBICODE_SQLG_NOQUERY)
#define DBIERR_BLOBGROUP   (ERRBASE_QUERY+ DBICODE_BLOBGROUP)
#define DBIERR_QRYNOPROP   (ERRBASE_QUERY+DBICODE_QRYNOPROP)
#define DBIERR_ANSTYPNOTSUP   (ERRBASE_QUERY+DBICODE_ANSTYPNOTSUP)
#define DBIERR_ANSALIASNOTSUP (ERRBASE_QUERY+DBICODE_ANSALIASNOTSUP)
#define DBIERR_INSBLOBREQ     (ERRBASE_QUERY+DBICODE_INSBLOBREQ     )                         // 0x9a
#define DBIERR_CHGUNIQUENDXREQ (ERRBASE_QUERY+DBICODE_CHGUNIQUENDXREQ)                         // 0x9b
#define DBIERR_DELUNIQUENDXREQ (ERRBASE_QUERY+DBICODE_DELUNIQUENDXREQ)                         // 0x9c
#define DBIERR_SQLNOFULLUPDATE (ERRBASE_QUERY+DBICODE_SQLNOFULLUPDATE)                         // 0x9d
#define DBIERR_CANTEXECREMOTE  (ERRBASE_QUERY+DBICODE_CANTEXECREMOTE)                          // 0x9e
#define DBIERR_UNEXPECTEDEOC   (ERRBASE_QUERY+DBICODE_UNEXPECTEDEOC)
#define DBIERR_SQLPARAMNOTSET  (ERRBASE_QUERY+DBICODE_SQLPARAMNOTSET)
#define DBIERR_QUERYTOOLONG    (ERRBASE_QUERY+DBICODE_QUERYTOOLONG)

#define   DBIERR_NOSUCHRELORALIAS    (ERRBASE_QUERY+DBICODE_NOSUCHRELORALIAS)
#define  DBIERR_TYPEAMBIGUITY       (ERRBASE_QUERY+DBICODE_TYPEAMBIGUITY)
#define  DBIERR_ORDERBYNOTAPROJ    (ERRBASE_QUERY+DBICODE_ORDERBYNOTAPROJ)
#define  DBIERR_SQLPARSE          (ERRBASE_QUERY+DBICODE_SQLPARSE)
#define  DBIERR_CONSTRAINTFAILED (ERRBASE_QUERY+DBICODE_CONSTRAINTFAILED)
#define  DBIERR_NOTGROUPINGFIELD (ERRBASE_QUERY+DBICODE_NOTGROUPINGFIELD)
#define  DBIERR_UDFNOTDEFINED   (ERRBASE_QUERY+DBICODE_UDFNOTDEFINED)
#define  DBIERR_UDFERROR      (ERRBASE_QUERY+DBICODE_UDFERROR)
#define  DBIERR_SINGLEROWERROR      (ERRBASE_QUERY+DBICODE_SINGLEROWERROR)
#define  DBIERR_GROUPEXPR      (ERRBASE_QUERY+DBICODE_GROUPEXPR)
#define  DBIERR_QUERYTEXT     (ERRBASE_QUERY+DBICODE_QUERYTEXT)
#define  DBIERR_ANSIJOINSUP   ( ERRBASE_QUERY + DBICODE_ANSIJOINSUP)
#define  DBIERR_DISTUNION      ( ERRBASE_QUERY + DBICODE_DISTUNION  )
#define  DBIERR_GROUPBYREQ      ( ERRBASE_QUERY + DBICODE_GROUPBYREQ )
#define  DBIERR_INSUPDAUTOINC   ( ERRBASE_QUERY + DBICODE_INSUPDAUTOIC)
#define  DBIERR_UPDREFINTSINGLE   ( ERRBASE_QUERY + DBICODE_UPDREFINTSINGLE)


// END_OF_QUERY_MESSAGES

// ERRCAT_VERSION
// ===============

#define  ERRCODE_INTERFACEVER    1        // Interface mismatch
#define  ERRCODE_INDEXOUTOFDATE  2        // Index is out of date
#define  ERRCODE_OLDVERSION      3        // Older version (see context)
#define  ERRCODE_VALFILEINVALID  4        // Val. file is out of date
#define  ERRCODE_BLOBVERSION     5        // Old Blob file version
#define  ERRCODE_ENGQRYMISMATCH  6        // Query and IDAPI are mismatched
#define  ERRCODE_SERVERVERSION   7        // Server is incompatible version
#define  ERRCODE_TABLELEVEL      8        // Higher table level required

#define  DBIERR_INTERFACEVER     (ERRBASE_VERSION + ERRCODE_INTERFACEVER)
#define  DBIERR_INDEXOUTOFDATE   (ERRBASE_VERSION + ERRCODE_INDEXOUTOFDATE)
#define  DBIERR_OLDVERSION       (ERRBASE_VERSION + ERRCODE_OLDVERSION)
#define  DBIERR_VALFILEINVALID   (ERRBASE_VERSION + ERRCODE_VALFILEINVALID)
#define  DBIERR_BLOBVERSION      (ERRBASE_VERSION + ERRCODE_BLOBVERSION)
#define  DBIERR_ENGQRYMISMATCH   (ERRBASE_VERSION + ERRCODE_ENGQRYMISMATCH)
#define  DBIERR_SERVERVERSION    (ERRBASE_VERSION + ERRCODE_SERVERVERSION)
#define  DBIERR_TABLELEVEL       (ERRBASE_VERSION + ERRCODE_TABLELEVEL)

// ERRCAT_CAPABILITY
// =================

#define  ERRCODE_NOTSUPPORTED    1        // Capability not supported
#define  ERRCODE_NIY             2        // Not Implemented Yet
#define  ERRCODE_TABLESQL        3        // Cannot access SQL replica
#define  ERRCODE_SEARCHCOLREQD   4        // Searchable (Non-blob column) required
#define  ERRCODE_NOMULTCONNECT   5        // Multiple connections not supported
#define  ERRCODE_NODBASEEXPR     6        // Full dBASE Expressions not supported

#define  DBIERR_NOTSUPPORTED     (ERRBASE_CAPABILITY + ERRCODE_NOTSUPPORTED)
#define  DBIERR_NIY              (ERRBASE_CAPABILITY + ERRCODE_NIY)
#define  DBIERR_TABLESQL         (ERRBASE_CAPABILITY + ERRCODE_TABLESQL)
#define  DBIERR_SEARCHCOLREQD    (ERRBASE_CAPABILITY + ERRCODE_SEARCHCOLREQD)
#define  DBIERR_NOMULTCONNECT    (ERRBASE_CAPABILITY + ERRCODE_NOMULTCONNECT)
#define  DBIERR_NODBASEEXPR      (ERRBASE_CAPABILITY + ERRCODE_NODBASEEXPR)

// ERRCAT_CONFIG
// =============

#define ERRCODE_INVALIDDBSPEC    1
#define ERRCODE_UNKNOWNDBTYPE    2
#define ERRCODE_INVALIDSYSDATA   3
#define ERRCODE_UNKNOWNNETTYPE   4
#define ERRCODE_NOTONTHATNET     5
#define ERRCODE_INVALIDCFGPARAM  6        // Generic invalid config param


#define DBIERR_INVALIDDBSPEC     (ERRBASE_CONFIG + ERRCODE_INVALIDDBSPEC)
#define DBIERR_UNKNOWNDBTYPE     (ERRBASE_CONFIG + ERRCODE_UNKNOWNDBTYPE)
#define DBIERR_INVALIDSYSDATA    (ERRBASE_CONFIG + ERRCODE_INVALIDSYSDATA)
#define DBIERR_UNKNOWNNETTYPE    (ERRBASE_CONFIG + ERRCODE_UNKNOWNNETTYPE)
#define DBIERR_NOTONTHATNET      (ERRBASE_CONFIG + ERRCODE_NOTONTHATNET)
#define DBIERR_INVALIDCFGPARAM   (ERRBASE_CONFIG + ERRCODE_INVALIDCFGPARAM)

// ERRCAT_WARNING  non-fatal warnings:
// warn user of action, or ask for optional behavior
// ==============
#define ERRCODE_OBJIMPLICITLYDROPPED      1
#define ERRCODE_OBJMAYBETRUNCATED         2
#define ERRCODE_OBJIMPLICITLYMODIFIED     3
#define ERRCODE_VALIDATEDATA              4
#define ERRCODE_VALFIELDMODIFIED          5
#define ERRCODE_TABLELEVELCHANGED         6
#define ERRCODE_COPYLINKEDTABLES          7
#define ERRCODE_OTHERSERVERLOADED         8
#define ERRCODE_OBJIMPLICITLYTRUNCATED    9
#define ERRCODE_VCHKMAYNOTBEENFORCED      10
#define ERRCODE_MULTIPLEUNIQRECS          11
#define ERRCODE_FIELDMUSTBETRIMMED        12

#define DBIERR_OBJIMPLICITLYDROPPED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYDROPPED)
#define DBIERR_OBJMAYBETRUNCATED (ERRBASE_WARNING + ERRCODE_OBJMAYBETRUNCATED)
#define DBIERR_OBJIMPLICITLYMODIFIED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYMODIFIED)
#define DBIERR_VALIDATEDATA              \
   ( ERRBASE_WARNING + ERRCODE_VALIDATEDATA)
#define DBIERR_VALFIELDMODIFIED          \
   ( ERRBASE_WARNING + ERRCODE_VALFIELDMODIFIED)
#define DBIERR_TABLELEVELCHANGED         \
   ( ERRBASE_WARNING + ERRCODE_TABLELEVELCHANGED)
#define DBIERR_COPYLINKEDTABLES          \
   ( ERRBASE_WARNING + ERRCODE_COPYLINKEDTABLES)
#define DBIERR_OTHERSERVERLOADED          \
   ( ERRBASE_WARNING + ERRCODE_OTHERSERVERLOADED)
#define DBIERR_OBJIMPLICITLYTRUNCATED  \
   ( ERRBASE_WARNING + ERRCODE_OBJIMPLICITLYTRUNCATED)
#define DBIERR_VCHKMAYNOTBEENFORCED       \
   ( ERRBASE_WARNING + ERRCODE_VCHKMAYNOTBEENFORCED )
#define DBIERR_MULTIPLEUNIQRECS    \
   ( ERRBASE_WARNING + ERRCODE_MULTIPLEUNIQRECS )
#define DBIERR_FIELDMUSTBETRIMMED        \
   ( ERRBASE_WARNING + ERRCODE_FIELDMUSTBETRIMMED )


// ERRCAT_OTHER
// ============

#define  ERRCODE_FILEEXISTS      1        // File already exsits
#define  ERRCODE_BLOBMODIFIED    2        // Another user modified Blob
#define  ERRCODE_UNKNOWNSQL      3        // Unknown SQL error
#define  ERRCODE_TABLEEXISTS     4        // Table already exsits
#define  ERRCODE_PDX10TABLE      5        // Paradox 1.0 tables not supported
#define  ERRCODE_UPDATEABORT     6        // Cached Update aborted

#define  DBIERR_FILEEXISTS       (ERRBASE_OTHER + ERRCODE_FILEEXISTS)
#define  DBIERR_BLOBMODIFIED     (ERRBASE_OTHER + ERRCODE_BLOBMODIFIED)
#define  DBIERR_UNKNOWNSQL       (ERRBASE_OTHER + ERRCODE_UNKNOWNSQL)
#define  DBIERR_TABLEEXISTS      (ERRBASE_OTHER + ERRCODE_TABLEEXISTS)
#define  DBIERR_PDX10TABLE       (ERRBASE_OTHER + ERRCODE_PDX10TABLE)
#define  DBIERR_UPDATEABORT      (ERRBASE_OTHER + ERRCODE_UPDATEABORT)

//

// ERRCAT_COMPATIBILITY
// =====================

#define  ERRCODE_DIFFSORTORDER    1        // Sortorders not compatible
#define  ERRCODE_DIRINUSEBYOLDVER 2        // Directory in use by old version
#define  ERRCODE_PDX35LDDRIVER    3        // Needs Pdox 3.5 compatible language driver

#define  DBIERR_DIFFSORTORDER     (ERRBASE_COMPATIBILITY + ERRCODE_DIFFSORTORDER)
#define  DBIERR_DIRINUSEBYOLDVER  (ERRBASE_COMPATIBILITY + ERRCODE_DIRINUSEBYOLDVER)
#define  DBIERR_PDX35LDDRIVER     (ERRBASE_COMPATIBILITY + ERRCODE_PDX35LDDRIVER)

// ERRCAT_OPTPARAM
// ===============

#define ERRCODE_REQOPTPARAM     1       // Required optional parameter missing
#define ERRCODE_INVALIDOPTPARAM 2       // Optional param out-of-range or bad


#define DBIERR_REQOPTPARAM      (ERRBASE_OPTPARAM + ERRCODE_REQOPTPARAM)
#define DBIERR_INVALIDOPTPARAM  (ERRBASE_OPTPARAM + ERRCODE_INVALIDOPTPARAM)

//  ERRCAT_REPOSITORY
//  =================

#define  ERRCODE_REPOSITORYCORRUPT      1    // Data Repository is corrupt
#define  ERRCODE_INFOBLOBCORRUPT        2    // Info Blob corrupted
#define  ERRCODE_SCHEMACORRUPT          3    // DR Schema is corrupt
#define  ERRCODE_ATTRTYPEEXISTS         4    // Attribute Type exists
#define  ERRCODE_INVALIDOBJTYPE         5    // Invalid Object Type
#define  ERRCODE_INVALIDRELATIONTYPE    6    // Invalid Relation Type
#define  ERRCODE_VIEWEXISTS             7    // View already exists
#define  ERRCODE_NOSUCHVIEW             8    // No such View exists
#define  ERRCODE_INVALIDRECCONSTRAINT   9    // Invalid Record Constraint
#define  ERRCODE_LDBCONNECTION          10   // Object is in a Logical DB
#define  ERRCODE_REPOSITORYEXISTS       11   // Repository already exists
#define  ERRCODE_NOSUCHREPOSITORY       12   // Repository does not exist
#define  ERRCODE_REPOSITORYDBMISSING    13   // Repository database does not exist
#define  ERRCODE_REPOSITORYOUTOFDATE    14   // Repository info is out of date
#define  ERRCODE_REPOSITORYVERSION      15   // DR Version mismatch
#define  ERRCODE_REPOSITORYNAME         16   // Invalid Repository name
#define  ERRCODE_DEPENDENTOBJECTS       17   // Dependent Objects exist
#define  ERRCODE_RELATIONLIMIT          18   // Too many Relationships for this Object Type
#define  ERRCODE_RELATIONSHIPSEXIST     19   // Relationships to the Object exist
#define  ERRCODE_EXCHANGEFILECORRUPT    20   // Exchange File Corrupt
#define  ERRCODE_EXCHANGEFILEVERSION    21   // Exchange File Version Mismatch
#define  ERRCODE_TYPEMISMATCH           22   // Exchange File and Repository Types don't match
#define  ERRCODE_OBJECTEXISTS           23   // Object Exists in the Target Repository
#define  ERRCODE_REPOSITORYACCESS       24   // Access to Repository Denied
#define  ERRCODE_REPOSITORYCREATE       25   // Cannot Create Repository
#define  ERRCODE_DATABASEOPENFAILED     26   // Cannot Open a Database


#define  DBIERR_REPOSITORYCORRUPT    (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYCORRUPT)
#define  DBIERR_INFOBLOBCORRUPT      (ERRBASE_REPOSITORY + ERRCODE_INFOBLOBCORRUPT)
#define  DBIERR_SCHEMACORRUPT        (ERRBASE_REPOSITORY + ERRCODE_SCHEMACORRUPT)
#define  DBIERR_ATTRTYPEEXISTS       (ERRBASE_REPOSITORY + ERRCODE_ATTRTYPEEXISTS)
#define  DBIERR_INVALIDOBJTYPE       (ERRBASE_REPOSITORY + ERRCODE_INVALIDOBJTYPE)
#define  DBIERR_INVALIDRELATIONTYPE  (ERRBASE_REPOSITORY + ERRCODE_INVALIDRELATIONTYPE)
#define  DBIERR_VIEWEXISTS           (ERRBASE_REPOSITORY + ERRCODE_VIEWEXISTS)
#define  DBIERR_NOSUCHVIEW           (ERRBASE_REPOSITORY + ERRCODE_NOSUCHVIEW)
#define  DBIERR_INVALIDRECCONSTRAINT (ERRBASE_REPOSITORY + ERRCODE_INVALIDRECCONSTRAINT)
#define  DBIERR_LDBCONNECTION        (ERRBASE_REPOSITORY + ERRCODE_LDBCONNECTION)
#define  DBIERR_REPOSITORYEXISTS     (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYEXISTS)
#define  DBIERR_NOSUCHREPOSITORY     (ERRBASE_REPOSITORY + ERRCODE_NOSUCHREPOSITORY)
#define  DBIERR_REPOSITORYDBMISSING  (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYDBMISSING)
#define  DBIERR_REPOSITORYOUTOFDATE  (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYOUTOFDATE)
#define  DBIERR_REPOSITORYVERSION    (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYVERSION)
#define  DBIERR_REPOSITORYNAME       (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYNAME)
#define  DBIERR_DEPENDENTOBJECTS     (ERRBASE_REPOSITORY + ERRCODE_DEPENDENTOBJECTS)
#define  DBIERR_RELATIONLIMIT        (ERRBASE_REPOSITORY + ERRCODE_RELATIONLIMIT)
#define  DBIERR_RELATIONSHIPSEXIST   (ERRBASE_REPOSITORY + ERRCODE_RELATIONSHIPSEXIST)
#define  DBIERR_EXCHANGEFILECORRUPT  (ERRBASE_REPOSITORY + ERRCODE_EXCHANGEFILECORRUPT)
#define  DBIERR_EXCHANGEFILEVERSION  (ERRBASE_REPOSITORY + ERRCODE_EXCHANGEFILEVERSION)
#define  DBIERR_TYPEMISMATCH         (ERRBASE_REPOSITORY + ERRCODE_TYPEMISMATCH)
#define  DBIERR_OBJECTEXISTS         (ERRBASE_REPOSITORY + ERRCODE_OBJECTEXISTS)
#define  DBIERR_REPOSITORYACCESS     (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYACCESS)
#define  DBIERR_REPOSITORYCREATE     (ERRBASE_REPOSITORY + ERRCODE_REPOSITORYCREATE)
#define  DBIERR_DATABASEOPENFAILED   (ERRBASE_REPOSITORY + ERRCODE_DATABASEOPENFAILED)

#endif  // NODBIERRS




#ifndef NODBIPROTOTYPES
//============================================================================
//                          DBI prototypes
//============================================================================

//                          Environmental
//----------------------------------------------------------------------------


DBIResult DBIFN DbiInitFn (           // Initialize the Engine
      UINT16          iVer,             // Interface Version
      pDBIEnv         pEnv              // Environment Structure/NULL
   );

    // DO NOT CALL THIS DIRECTLY. Use the 'DbiInit' define below.

    // Initializes the engine environment. Default settings can be overwritten
    // by supplying the appropriate settings. Defaults are read from the system
    // configuration file. pEnv can be NULLP to get the system defautls.

#define DbiInit(pEnv)     DbiInitFn(DBIINTFVER, pEnv)


DBIResult DBIFN DbiExit(VOID);        // Exit engine


//============================================================================
//                         System Level Info
//============================================================================

DBIResult DBIFN DbiGetSysVersion (    // Get system version info
      pSYSVersion    psysVersion
   );

DBIResult DBIFN DbiGetSysConfig (     // System configuration
      pSYSConfig     psysConfig
   );

DBIResult DBIFN DbiGetClientInfo (    // Get Client info
      pCLIENTInfo    pclientInfo
   );

DBIResult DBIFN DbiGetSysInfo (       // Get system status/info
      pSYSInfo       psysInfo
   );

DBIResult DBIFN DbiLoadDriver (       // Load a given driver
      pCHAR          pszDriverType      // Driver name
   );

//============================================================================
//                            Sessions
//============================================================================

DBIResult DBIFN DbiStartSession (     // Start a new session
      pCHAR          pszName,           // Name (Optional)
      phDBISes       phSes  ,           // Session
      pCHAR          pNetDir            // Netfile directory for session (opt)
   );

DBIResult DBIFN DbiGetCurrSession (   // Get the current session
      phDBISes       phSes              // Session
   );

DBIResult DBIFN DbiSetCurrSession (   // Set the current session
      hDBISes        hSes               // Session/NULL
   );

DBIResult DBIFN DbiCloseSession (     // Close the current session
      hDBISes        hSes               // Session
   );

DBIResult DBIFN DbiGetSesInfo (       // Get current session info
      pSESInfo       psesInfo
   );

DBIResult DBIFN DbiSetPrivateDir (    // Set Private Directory for session
      pCHAR          pszDir             // Directory name/NULL
   );


//============================================================================
//                     Datababase, Schema and File inquiry
//============================================================================



DBIResult DBIFN DbiOpenDatabase (     // Open a database
      pCHAR          pszDbName,         // Database name
      pCHAR          pszDbType,         // Database type (NULL: Universal)
      DBIOpenMode    eOpenMode,         // Open type
      DBIShareMode   eShareMode,        // Share type
      pCHAR          pszPassword,       // Password
      UINT16         iOptFlds,          // Number of optional Params
      pFLDDesc       pOptFldDesc,       // Optional Field Descriptors
      pBYTE          pOptParams,        // Optional Params
      phDBIDb        phDb               // Returnd database handle
    );

DBIResult DBIFN DbiSetDirectory (     // Set the current directory
      hDBIDb         hDb,               // Universal database handle
      pCHAR          pszDir             // Directory/NULL
    );

DBIResult DBIFN DbiGetDirectory (     // Get the current/default directory
      hDBIDb         hDb,               // Universal database handle
      BOOL           bDefault,          // True for default
      pCHAR          pszDir             // Returned directory
    );

DBIResult DBIFN DbiOpenTableList (    // Open a cursor on "Tables"
      hDBIDb         hDb,               // Database handle
      BOOL           bExtended,         // True for extended info
      BOOL           bSystem,           // True to include system tables
      pCHAR          pszWild,           // Wild card name
      phDBICur       phCur              // Returned cursor
    );

DBIResult DBIFN DbiOpenFileList (     // Open a cursor on "Files"
      hDBIDb         hDb,               // Universal database handle
      pCHAR          pszWild,           // Wild card name
      phDBICur       phCur              // Returned cursor
    );

DBIResult DBIFN DbiOpenIndexList (    // Return "Indexes" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur);            // Returned cursor on "Indexes"

DBIResult DBIFN DbiOpenFieldList (    // Return "Fields" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      BOOL           bPhyTypes,         // True, for physical types
      phDBICur       phCur);            // Returned cursor on "Fields"

DBIResult DBIFN DbiOpenVchkList (     // Return "Checks" for a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver Type
      phDBICur       phChkCur);         // Returned cursor on "Checks"

DBIResult DBIFN DbiOpenRintList (     // Return Integrity checks
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phChkCur);         // Returned cursor on "Ref Int".

DBIResult DBIFN DbiOpenSecurityList ( // Return security descriptions
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phSecCur           // Returned cursor on sec list
   );

DBIResult DBIFN DbiOpenFamilyList (   // Return family members
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phFmlCur           // Returned cursor on "Family"
   );

DBIResult DBIFN DbiOpenSPList(        // Open a cursor on "Stored Procedures"
      hDBIDb         hDb,               // Universal database handle
      BOOL           bExtended,         // True for extended info (N/A)
      BOOL           bSystem,           // True to include system procedures
      pCHAR          pszQual,           // Qualifier (N/A)
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiOpenSPParamList (    // Return "Parameters" for a stored procedure
      hDBIDb         hDb,               // Database handle
      pCHAR          pszSPName,         // Stored procedure name
      BOOL           bPhyTypes,         // True, for physical types
      UINT16         uOverload,         // Overload number
      phDBICur       phCur              // Returned cursor on "Parameters"
  );

DBIResult DBIFN DbiOpenFunctionList (    // Open a cursor on "Functions"
      hDBIDb         hDb,               // Universal database handle
      DBIFUNCOpts    eoptBits,          // Options for function list
      phDBICur       phCur              // Returned cursor on "Functions"
                                        // Record desc is of type DBIFUNCDesc
   );

DBIResult DBIFN DbiOpenFunctionArgList ( // Return "Arguments" for a function
      hDBIDb         hDb,               // Database handle
      pCHAR          pszFuncName,       // Function name
      UINT16         uOverload,         // Overload number
      phDBICur       phCur              // Returned cursor on "Arguments"
  );

DBIResult DBIFN DbiCloseDatabase (     // Close a database
      phDBIDb        phDb                // Pointer to database handle
    );

//============================================================================
//                                  Capabilities
//============================================================================

DBIResult DBIFN DbiOpenDriverList (   // Get a list of driver names
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetDriverDesc (    // Get description for a given type
      pCHAR          pszDriverType,     // Symbolic name for driver type
      pDRVType       pdrvType           // Driver type description
   );

DBIResult DBIFN DbiOpenDatabaseList ( // Get a list of registered databases
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetDatabaseDesc (  // Get a description of a logical db
      pCHAR          pszName,           // Name of logical database
      pDBDesc        pdbDesc            // Database description
   );

DBIResult DBIFN DbiOpenTableTypesList ( // Get a list of table types
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetTableTypeDesc ( // Get Table capabilities
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTableType,      // Table type
      pTBLType       ptblType           // Table Capabilities
   );


DBIResult DBIFN DbiOpenFieldTypesList ( // Get a list of field types
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTblType,        // Table type (Optional)
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetFieldTypeDesc ( // Get list of field types
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszTableType,      // Table type
      pCHAR          pszFieldType,      // Field type  (Physical only)
      pFLDType       pfldType           // Field type description
   );

DBIResult DBIFN DbiOpenIndexTypesList ( // Get list of index types
      pCHAR          pszDriverType,     // Driver type
      phDBICur       phCur              // Returned cursor
   );

DBIResult DBIFN DbiGetIndexTypeDesc ( // Get description of given idx type
      pCHAR          pszDriverType,     // Driver type
      pCHAR          pszIndexType,      // Index type
      pIDXType       pidxType           // Index description
   );

DBIResult DBIFN DbiOpenLdList (       // Get a list of Lang Drivers
      phDBICur       phCur              // Returned cursor
   );

//===========================================================================
//                      Table Open, Properties & Structure
//===========================================================================

DBIResult DBIFN DbiOpenTable (        // Open a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name or file name
      pCHAR          pszDriverType,     // Driver type                 /NULL
      pCHAR          pszIndexName,      // Index to be used for access /NULL
      pCHAR          pszIndexTagName,   // Index tag name              /NULL
      UINT16         iIndexId,          // Index number                /0
      DBIOpenMode    eOpenMode,         // Read or RW
      DBIShareMode   eShareMode,        // Excl or Share
      XLTMode        exltMode,          // Xlate mode
      BOOL           bUniDirectional,   // Uni or Bi directional
      pBYTE          pOptParams,        // Optional params /NULL
      phDBICur       phCursor           // Returns Cursor handle
    );


DBIResult DBIFN DbiGetCursorProps (   // Get Cursor properties
      hDBICur        hCursor,           // Cursor handle
      pCURProps      pcurProps          // Cursor properties
    );

DBIResult DBIFN DbiGetObjFromName (   // Get object from name
      DBIOBJType     eObjType   ,       // Object handle
      pCHAR          pszObjName ,       // Name of object /NULL
      phDBIObj       phObj              // Returned object handle
   );

DBIResult DBIFN DbiGetObjFromObj (    // Get associated object
      hDBIObj        hObj       ,       // Object handle
      DBIOBJType     eObjType   ,       // Type of associated object
      phDBIObj       phObj              // Returns object of eObjType
   );

DBIResult DBIFN DbiGetProp (          // Get property
      hDBIObj        hObj   ,           // Object handle
      UINT32         iProp  ,           // Property to retrieve
      pVOID          pPropValue,        // == NULL, validate iProp for getting
      UINT16         iMaxLen,           // Length of buffer pPropValue
      pUINT16        piLen              // Returns required length
   );

DBIResult DBIFN DbiSetProp (          // Set property
      hDBIObj        hObj   ,           // Object handle
      UINT32         iProp  ,           // Property to set
      UINT32         iPropValue         // Property value
   );

DBIResult DBIFN DbiValidateProp (     // Validate a property
      hDBIObj        hObj,              // Object handle
      UINT32         iProp,             // property to validate
      BOOL           bSetting           // TRUE:setting, FALSE:getting
  );

DBIResult DBIFN DbiGetFieldDescs (    // Get field descriptions
      hDBICur        hCursor,           // Cursor handle
      pFLDDesc       pfldDesc           // Array of field descriptors
    );

DBIResult DBIFN DbiGetCursorForTable (// Find cursor for a given table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type / NULL
      phDBICur       phCursor           // Returned cursor
   );

DBIResult DBIFN DbiCloneCursor (      // Return a duplicate cursor
      hDBICur        hCurSrc,           // Source cursor
      BOOL           bReadOnly,         // If TRUE, read only mode
      BOOL           bUniDirectional,   // If TRUE, Uni directional
      phDBICur       phCurNew           // Destination cursor address
    );

DBIResult DBIFN DbiCloseCursor (      // Closes cursor
      phDBICur       phCursor           // Pntr to Cursor handle
    );


//============================================================================
//                      Index Manipulation & Usage
//============================================================================

DBIResult DBIFN DbiOpenIndex (        // Open an index
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index Name
      UINT16         iIndexId           // Index number (if applicable)
   );

DBIResult DBIFN DbiCloseIndex (       // Close an index
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index Name
      UINT16         iIndexId           // Index number
   );

DBIResult DBIFN DbiSwitchToIndex (    // Change index order of access
      phDBICur       phCursor,          // Cursor handle (In/Out)
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszTagName,        // Tag name (if applicable)
      UINT16         iIndexId,          // Index number
      BOOL           bCurrRec           // Position at current rec
    );

DBIResult DBIFN DbiGetIndexDesc (     // Get index properties
      hDBICur        hCursor,           // Cursor handle
      UINT16         iIndexSeqNo,       // Index number
      pIDXDesc       pidxDesc           // Returned index description
   );

DBIResult DBIFN DbiGetIndexDescs (    // Get index properties
      hDBICur        hCursor,           // Cursor handle
      pIDXDesc       pidxDesc           // Returned index descriptors
   );

DBIResult DBIFN DbiGetIndexForField ( // Get index desc for given field
      hDBICur        hCursor,           // Cursor handle
      UINT16         iFld,              // Field Number (1..N)
      BOOL           bProdTagOnly,      // If TRUE, only xBASE prod tags will be returned
      pIDXDesc       pidxDesc           // (Optional)
   );

DBIResult DBIFN DbiGetIndexSeqNo(     // Get index sequence number
      hDBICur        hCursor,           // Cursor handle
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszTagName,        // Tag name (if applicable)
      UINT16         iIndexId,          // Index number
      pUINT16        piIndexSeqNo       // Index number
   );

DBIResult DBIFN DbiSetToKey  (        // Set key condition
      hDBICur           hCursor,        // Cursor handle
      DBISearchCond     eSearchCond,    // Search condition (default is =)
      BOOL              bDirectKey,     // Key is supplied directly
      UINT16            iFields,        // No of full fields to match
      UINT16            iLen,           // Partial key len of last field
      pBYTE             pBuff           // Either Record buffer or Key itself
    );

DBIResult DBIFN DbiExtractKey (       // Get the key value of current record
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pRecBuf,        // Record buffer (optional)
      pBYTE             pKeyBuf         // Returned. Key bytes.
   );

DBIResult DBIFN DbiSetRange (         // Set cursor to a range
      hDBICur           hCursor,        // Cursor
      BOOL              bKeyItself,     // Whether Key or Record buffer
      UINT16            iFields1,       // Key fields to be mathced in full
      UINT16            iLen1,          // Key length to compare
      pBYTE             pKey1,          // Top/Left key in Range
      BOOL              bKey1Incl,      // If Inclusive of Key1
      UINT16            iFields2,       // Key fields to be mathced in full
      UINT16            iLen2,          // Key length to compare
      pBYTE             pKey2,          // Bottom/Right key in Range
      BOOL              bKey2Incl       // If Inclusive of Key2
   );

DBIResult DBIFN DbiResetRange (       // Reset range
      hDBICur        hCursor            // cursor handle
   );

DBIResult DBIFN DbiCompareKeys (      // Compare two keys
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pKey1,          // Key buffer 1 to compare
      pBYTE             pKey2,          // Key buffer 2 (Or NULL)
      UINT16            iFields,        // Fields to compare in full
      UINT16            iLen,           // Partial key to compare
      pINT16            piResult        // Compare result
   );

DBIResult DBIFN DbiGetRecordForKey (  // Find a record matching key
      hDBICur           hCursor,        // Cursor handle
      BOOL              bDirectKey,     // Key is supplied directly
      UINT16            iFields,        // No of full fields to match
      UINT16            iLen,           // Partial key len of last field
      pBYTE             pKey,           // Either Record buffer or Key itself
      pBYTE             pRecBuff        // (Optional) Record buffer
   );

//=============================================================================
//                          Validity check and referential integrity
//=============================================================================

DBIResult DBIFN DbiGetVchkDesc (      // Get valcheck descriptor
      hDBICur        hCursor,           // Cursor handle
      UINT16         iValSeqNo,         // Valcheck sequence number
      pVCHKDesc      pvalDesc           // Returned valcheck description
   );

DBIResult DBIFN DbiGetRintDesc (      // Get referential integrity descriptor
      hDBICur        hCursor,           // Cursor handle
      UINT16         iRintSeqNo,        // Rint sequence number
      pRINTDesc      printDesc          // Returned rint description
   );

//=============================================================================
//                              Cursor Maintenance
//=============================================================================


DBIResult DBIFN DbiSetToBegin (       // Reset cursor to beginning
      hDBICur        hCursor            // Cursor handle
   );

DBIResult DBIFN DbiSetToEnd (         // Reset cursor to ending
      hDBICur        hCursor            // Cursor handle
    );

DBIResult DBIFN DbiSetToCursor (      // Set cursor to another cursor position
      hDBICur        hDest,             // Destination cursor
      hDBICur        hSrc               // Source cursor
   );

DBIResult DBIFN DbiGetBookMark (      // Get a book-mark
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark          // Pointer to Book-Mark
   );

DBIResult DBIFN DbiSetToBookMark (    // Position to a Book-Mark
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark          // Pointer to Book-Mark
   );

DBIResult DBIFN DbiCompareBookMarks ( // Compare two Book-marks
      hDBICur        hCur,              // Cursor
      pBYTE          pBookMark1,        // Book mark 1
      pBYTE          pBookMark2,        // Book mark 2
      pCMPBkMkRslt   pCmpBkmkResult     // Compare result
   );

//============================================================================
//                      Data Access: Logical Record Level
//============================================================================


DBIResult DBIFN DbiGetNextRecord (    // Find/Get the next record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetPriorRecord (   // Find/Get the prior record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer (client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetRecord (        // Gets the current record
      hDBICur           hCursor,        // Cursor handle
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiGetRelativeRecord (// Find/Get a record by record number
      hDBICur           hCursor,        // Cursor handle
      INT32             iPosOffset,     // offset from current position
      DBILockType       eLock,          // Optional lock request
      pBYTE             pRecBuff,       // Record buffer(client)
      pRECProps         precProps       // Optional record properties
    );

DBIResult DBIFN DbiInitRecord (       // Initialize record area
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuff           // Record buffer
    );

DBIResult DBIFN DbiInsertRecord (     // Inserts a new record
      hDBICur        hCursor,           // Cursor handle
      DBILockType    eLock,             // Optional lock on this rec
      pBYTE          pRecBuff           // New Record (client)
    );

DBIResult DBIFN DbiModifyRecord (     // Updates the current record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Modified record
      BOOL           bFreeLock          // Free record lock
    );

DBIResult DBIFN DbiDeleteRecord (     // Deletes the current record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf            // Copy of deleted record
    );

DBIResult DBIFN DbiReadBlock (        // Read a block of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecords,         // Number of records to read
      pBYTE          pBuf               // Buffer
   );

DBIResult DBIFN DbiWriteBlock (       // Write a block of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecords,         // Number of records to write/written
      pBYTE          pBuf               // Buffer
   );

DBIResult DBIFN DbiAppendRecord (     // Inserts a new record
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuff           // New Record (client)
    );

DBIResult DBIFN DbiUndeleteRecord (   // Undeletes the current record
      hDBICur        hCursor            // Cursor handle
    );

DBIResult DBIFN DbiGetSeqNo (         // Get logical record number
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piSeqNo            // Pointer to sequence number
   );

DBIResult DBIFN DbiSetToSeqNo (       // Position to a logical record number
      hDBICur        hCursor,           // Cursor handle
      UINT32         iSeqNo             // Sequence number
   );

DBIResult DBIFN DbiGetRecordCount (   // Get the current number of records
      hDBICur        hCursor,           // Cursor handle
      pUINT32        piRecCount         // Number of records
   );


DBIResult DBIFN DbiSetToRecordNo (    // Position to Physical Rec#
      hDBICur        hCursor,           // Cursor handle
      UINT32         iRecNo             // Physical record number
   );

DBIResult DBIFN DbiSaveChanges (      // Flush all buffered changes
      hDBICur        hCursor            // Cursor handle
   );
DBIResult DBIFN DbiForceReread (      // Force Reread of buffers from Disk
      hDBICur        hCursor            // Cursor
   );

DBIResult DBIFN DbiCheckRefresh (VOID); // Check refresh for session

DBIResult DBIFN DbiMakePermanent (    // Make temporary table permanent
      hDBICur     hCursor,              // Cursor handle
      pCHAR       pszName,              // Rename temporary table
      BOOL        bOverWrite            // Overwrite existing file
   );

DBIResult DBIFN DbiForceRecordReread (// Force Reread of current record from Server
      hDBICur     hCursor,              // Cursor handle
      pBYTE       pRecBuff              // Returned : record buffer
   );


//============================================================================
//                            Field Level Access
//============================================================================

DBIResult DBIFN DbiGetField(          // Get Field value
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pRecBuff,          // Record buffer
      pBYTE          pDest,             // Destination field buffer
      pBOOL          pbBlank            // Returned : is field blank
   );

DBIResult DBIFN DbiPutField(          // Put a value in the record buffer
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pRecBuff,          // Record buffer
      pBYTE          pSrc               // Source field buffer
   );

DBIResult DBIFN DbiVerifyField(       // Verifies the field value
      hDBICur        hCursor,           // Cursor
      UINT16         iField,            // Field # (1..n)
      pBYTE          pSrc,              // Field Value
      pBOOL          pbBlank            // Field is Blank (Returned)
   );

DBIResult DBIFN DbiOpenBlob (         // Open a blob for access
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number (1..n)
      DBIOpenMode    eOpenMode          // Open for Read or RW
    );

DBIResult DBIFN DbiGetBlobSize (      // Gets the size of a blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      pUINT32        piSize             // Blob size in bytes
    );

DBIResult DBIFN DbiGetBlob (          // Read bytes from blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iOffSet,           // Starting position
      UINT32         iLen,              // No of bytes to be read
      pBYTE          pDest,             // Destination
      pUINT32        piRead             // Actual no of bytes read
    );

DBIResult DBIFN DbiPutBlob (          // Write bytes to blob
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iOffSet,           // Starting position
      UINT32         iLen,              // No of bytes to put
      pBYTE          pSrc               // pntr to Source
    );

DBIResult DBIFN DbiTruncateBlob (     // Reduces the blob size
      hDBICur        hCursor,           // Cursor handle
      pBYTE          pRecBuf,           // Record Buffer
      UINT16         iField,            // Field number of blob (1..n)
      UINT32         iLen               // New blob length
    );

DBIResult DBIFN DbiFreeBlob (         // Closes the blob
      hDBICur           hCursor,        // Cursor handle
      pBYTE             pRecBuf,        // Record Buffer
      UINT16            iField          // Field number of blob (0..n)
    );

DBIResult DBIFN DbiGetBlobHeading (   // Get Blob Heading
      hDBICur           hCursor,        // Cursor handle
      UINT16            iField,         // Field number of blob (1..n)
      pBYTE             pRecBuf,        // Record buffer of owner record
      pBYTE             pDest           // Destination buffer
   );

DBIResult DBIFN DbiSetFieldMap(       // Set a fieldmap
      hDBICur        hCur,              // Cursor handle
      UINT16         iFields,           // Number of fields
      pFLDDesc       pFldDesc           // Array of field descriptions
   );


//=============================================================================
//                                TRANSACTIONS
//=============================================================================

DBIResult DBIFN DbiBeginTran (        // Begin a transaction
   hDBIDb         hDb,                  // Database handle
   eXILType       eXIL,                 // Transaction isolation level
   phDBIXact      phXact                // Returned Xact handle
);

DBIResult DBIFN DbiEndTran (          // End a transaction
   hDBIDb         hDb,                  // Database handle
   hDBIXact       hXact,                // Xact handle
   eXEnd          eEnd                  // Xact end type
);

DBIResult DBIFN DbiGetTranInfo (      // Get transaction info
   hDBIDb         hDb,                  // Database handle
   hDBIXact       hXact,                // Xact handle
   pXInfo         pxInfo                // Xact info
);

//=============================================================================
//                                  LOCKING
//=============================================================================

DBIResult DBIFN DbiAcqTableLock (     // Lock a table
      hDBICur        hCursor,           // Cursor handle
      DBILockType    eLockType          // Lock type
    );

DBIResult DBIFN DbiAcqPersistTableLock (// Get a persistent lock
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type / NULL
   );

DBIResult DBIFN DbiRelPersistTableLock (// Releases a persistent lock
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type / NULL
   );

DBIResult DBIFN DbiRelTableLock (     // Unlocks Table level locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAll,              // True for all table level locks
      DBILockType    eLockType          // Specific lock type
    );

DBIResult DBIFN DbiRelRecordLock (    // Releases record level locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAll               // True for all. Default Current.
    );

DBIResult DBIFN DbiIsRecordLocked (   // Check if current record is locked
      hDBICur        hCursor,           // Cursor handle
      pBOOL          pbLocked           // Rec lock status
   );

DBIResult DBIFN DbiIsTableLocked (    // Verify if Table is locked
      hDBICur        hCursor,           // Cursor handle
      DBILockType    epdxLock,          // Lock type to verify
      pUINT16        piLocks            // Nbr of locks of the given type
   );

DBIResult DBIFN DbiIsTableShared (    // Verify if this is a shared table
      hDBICur        hCursor,           // Cursor handle
      pBOOL          pbShared           // Shared status
   );

DBIResult DBIFN DbiOpenLockList (     // Get a list of locks
      hDBICur        hCursor,           // Cursor handle
      BOOL           bAllUsers,         // True, for all Users locks
      BOOL           bAllLockTypes,     // True, for all lock types
      phDBICur       phLocks            // Returned cursor on Lock list
   );

DBIResult DBIFN DbiOpenUserList (     // Get a list of users loggedin
      phDBICur       phUsers            // Returned cursor on user list
   );

DBIResult DBIFN DbiSetLockRetry (     // Set Lock wait time
      INT16          iWait              // Time in seconds
   );

//============================================================================
//                              Batch Operations
//============================================================================


DBIResult DBIFN DbiBatchMove (        // Copy records to destination table
   pBATTblDesc    pSrcTblDesc,          // Source table identification,
   hDBICur        hSrcCur,              //  OR source cursor  ( one must be NULL )
   pBATTblDesc    pDstTblDesc ,         // Destination table identification,
   hDBICur        hDstCur,              //  OR destination cursor ( one must be NULL )
   eBATMode       ebatMode,             // Batch mode
   UINT16         iFldCount,            // Size of field maps
   pUINT16        pSrcFldMap,           // Array of source field numbers
   pCHAR          pszIndexName,         // If update mode, used to match records
   pCHAR          pszIndexTagName,      // Index tag name
   UINT16         iIndexId,             // Index  id
   pCHAR          pszKeyviolName,       // Keyviol table name  (optional)
   pCHAR          pszProblemsName,      // Problems table name (optional)
   pCHAR          pszChangedName,       // Changed table name (optional)
   pUINT32        plProbRecs,           // Number records written to problem table
   pUINT32        plKeyvRecs,           // Number records written to keyv table
   pUINT32        plChangedRecs,        // Number records written to changed table
   BOOL           bAbortOnFirstProb,    // If TRUE, abort on first problem rec
   BOOL           bAbortOnFirstKeyviol, // If TRUE, abort on first keyviol rec
   pUINT32        plRecsToMove,         // Number of records to read from source
                                        // Returns number actually read
   BOOL           bTransliterate        // If TRUE, transliterate character data
                                        // in fields between src&dst char sets
);

DBIResult DBIFN DbiCopyTable (        // Copy one table to another
      hDBIDb         hDb,               // Database handle
      BOOL           bOverWrite,        // True, to overwrite existing file
      pCHAR          pszSrcTableName,   // Source table name
      pCHAR          pszSrcDriverType,  // Source driver type
      pCHAR          pszDestTableName   // Destination table name
   );

DBIResult DBIFN DbiEmptyTable (       // Deletes all records
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType      // Driver type /NULL
    );

DBIResult DBIFN DbiPackTable (        // Pack a table
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      BOOL           bRegenIdxs         // Regenerate indexes
    );

DBIResult DBIFN DbiRegenIndex (       // Regenerate an index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszIndexTagName,   // Index tagname (xbase MDX)
      UINT16         iIndexId           // Index number
    );

DBIResult DBIFN DbiRegenIndexes (     // Regenerate all indexes
      hDBICur         hCursor           // Cursor
   );

DBIResult DBIFN DbiSortTable (        // Sort table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name of source
      pCHAR          pszDriverType,     // Driver type /NULL
      hDBICur        hSrcCur,           // OR cursor of table to sort
      pCHAR          pszSortedName,     // Destination table (NULL if sort to self)
      phDBICur       phSortedCur,       // If non-null, return cursor on destination
      hDBICur        hDstCur,           // OR cursor of destination
      UINT16         iSortFields,       // Number of sort fields
      pUINT16        piFieldNum,        // Array of field numbers
      pBOOL          pbCaseInsensitive, // Which fields should sort c-i (Opt)
      pSORTOrder     pSortOrder,        // Array of Sort orders (Opt)
      pfSORTCompFn   *ppfSortFn,        // Array of compare fn pntrs (Opt)
      BOOL           bRemoveDups,       // TRUE : Remove duplicates
      hDBICur        hDuplicatesCur,    // Cursor to duplicates table (Opt)
      pUINT32        plRecsSort         // In/out param. - sort this number
                                        // of records from current position, return
                                        // number actually output to dest.  (Opt)
);


//============================================================================
//                           Create & Restructure
//============================================================================

DBIResult DBIFN DbiCreateTable (      // Create a new table
      hDBIDb         hDb,               // Database handle
      BOOL           bOverWrite,        // True, to overwrite existing file.
      pCRTblDesc     pcrTblDsc          // Table description
   );

DBIResult DBIFN DbiCreateInMemTable ( // Create a temporary table (Logical)
      hDBIDb         hDb,               // Database handle
      pCHAR          pszName,           // Logical Name
      UINT16         iFields,           // No of fields
      pFLDDesc       pfldDesc,          // Array of field descriptors
      phDBICur       phCursor           // Returned cursor handle
   );

DBIResult DBIFN DbiCreateTempTable (  // Create temporary table (Physical)
      hDBIDb         hDb,               // Database handle
      pCRTblDesc     pcrTblDsc,         // Table description
      phDBICur       phCursor           // Returned cursor on table
   );

DBIResult DBIFN DbiDoRestructure (    // Restructure a table
      hDBIDb         hDb,               // Database handle
      UINT16         iTblDescCount,     // Number of table descriptors (1)
      pCRTblDesc     pTblDesc,          // Array of table descs
      pCHAR          pszSaveAs,         // Restructure to this table
      pCHAR          pszKeyviolName,    // Keyviol table name  (optional)
      pCHAR          pszProblemsName,   // Problems table name (optional)
      BOOL           bAnalyzeOnly       // Analyze restructure
   );

DBIResult DBIFN DbiRenameTable (      // Rename table & family
      hDBIDb         hDb,               // Database handle
      pCHAR          pszOldName,        // Old name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszNewName         // New name
   );

DBIResult DBIFN DbiDeleteTable (      // Delete a table
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Name including any path
      pCHAR          pszDriverType      // Driver type /NULL
   );

DBIResult DBIFN DbiAddIndex (         // Add a new index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name including any path
      pCHAR          pszDriverType,     // Driver type /NULL
      pIDXDesc       pIdxDesc    ,      // Description of the index
      pCHAR          pszKeyviolName     // Keyviol table name (optional)
   );

DBIResult DBIFN DbiDeleteIndex (      // Delete index
      hDBIDb         hDb,               // Database handle
      hDBICur        hCursor,           // Cursor (OR)
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszIndexName,      // Index name
      pCHAR          pszIndexTagName,   // Index tagname (xbase MDX)
      UINT16         iIndexId           // Index number
   );

//===========================================================================
//                            Error Info
//===========================================================================

DBIResult DBIFN DbiGetErrorEntry (    // Get error entry
      UINT16         uEntry,            // Error stack entry
      pUINT32        pulNativeError,    // Returned. Native error code, if any
      pCHAR          pszError           // Returned. Error string, if any
   );

DBIResult DBIFN DbiGetErrorInfo (     // Return info on last error
      BOOL           bFull,             // If Full details
      pDBIErrInfo    pErrInfo           // Error Info
   );

DBIResult DBIFN DbiGetErrorString (   // Get message for error code
      DBIResult      rslt,              // Engine error code
      pCHAR          pszError           // Error string for the error
   );

DBIResult DBIFN DbiGetErrorContext (  // Get specific Context if available
      INT16          eContext,          // Context type
      pCHAR          pszContext         // Context string (MAXMSGLEN +1)
   );

//============================================================================
//                              Dbi Services
//============================================================================

DBIResult DBIFN DbiDateEncode (       // Encode Date components into Date
      UINT16         iMon,              // Month    (1..12)
      UINT16         iDay,              // Day      (1..31)
      INT16          iYear,             // Year     (0..2**16-1)
      pDBIDATE       pdateD             // Encoded date
   );

DBIResult DBIFN DbiDateDecode (       // Decode Date into components
      DBIDATE        dateD,             // Encoded Date
      pUINT16        piMon,             // Month
      pUINT16        piDay,             // Day
      pINT16         piYear             // Year
   );

DBIResult DBIFN DbiTimeEncode (       // Encode Time components into TIME
      UINT16         iHour,             // Hours (0..23)
      UINT16         iMin,              // Minutes (0..59)
      UINT16         iMilSec,           // Milli Seconds (0..59999)
      pTIME          ptimeT             // Encoded Time
   );

DBIResult DBIFN DbiTimeDecode (       // Decode TIME into components
      TIME           timeT,             // Encoded Time
      pUINT16        piHour,            // Hours (0..23)
      pUINT16        piMin,             // Minutes (0..59)
      pUINT16        piMilSec           // Milli Seconds (0..59999)
   );

DBIResult DBIFN DbiTimeStampEncode (  // Encode Date & TIme into Date+Time
      DBIDATE        dateD,             // Encoded Date
      TIME           timeT,             // Encoded Time
      pTIMESTAMP     ptsTS              // Encoded Date+Time
   );

DBIResult DBIFN DbiTimeStampDecode (  // Decode Date & Time from Date+Time
      TIMESTAMP      tsTS,              // Encoded Date+Time
      pDBIDATE       pdateD,            // Encoded Date
      pTIME          ptimeT             // Encoded Time
   );


DBIResult DBIFN DbiBcdFromFloat(      // Converts FLOAT number into FMTBcd format
       pDFLOAT     piVal   ,            // Float to convert
       UINT16      iPrecision,          // Precision of BCD
       UINT16      iPlaces ,            // Number of decimals
       pFMTBcd     pBcd                 // returns Bcd number (length = iPrecision +2)
    );

DBIResult DBIFN DbiBcdToFloat(        // Converts FMTBcd number to FLOAT
      pFMTBcd      pBcd,                // Bcd number to convert
      pDFLOAT      piVal                // Returns converted float
   );

//===========================================================================
//                           CallBacks
//===========================================================================

DBIResult DBIFN DbiRegisterCallBack ( // Register a call back fn
      hDBICur        hCursor,           // Cursor (Optional)
      CBType         ecbType,           // Type of call back
      UINT32         iClientData,       // Pass-thru client data
      UINT16         iCbBufLen,         // Callback buffer len
      pVOID          pCbBuf,            // Pointer to callback buffer
      pfDBICallBack  pfCb               // Call back fn being registered
   );

DBIResult DBIFN DbiGetCallBack (      // Register a call back fn
      hDBICur        hCursor,           // Cursor (Optional)
      CBType         ecbType,           // Type of call back
      pUINT32        piClientData,      // Pass-thru client data
      pUINT16        piCbBufLen,        // Callback buffer len
      ppVOID         ppCbBuf,           // Pointer to callback buffer
      ppfDBICallBack ppfCb              // Call back fn being registered
   );

//============================================================================
//                          Date, time formats
//============================================================================


DBIResult DBIFN DbiGetDateFormat (    // Get current date format
      pFMTDate       pfmtDate
   );

DBIResult DBIFN DbiSetDateFormat (    // Set current date format
      pFMTDate       pfmtDate
   );

DBIResult DBIFN DbiGetTimeFormat (    // Get current time format
      pFMTTime       pfmtTime
   );

DBIResult DBIFN DbiSetTimeFormat (    // Set current time format
      pFMTTime       pfmtTime
   );

DBIResult DBIFN DbiGetNumberFormat (  // Get current number format
      pFMTNumber     pfmtNumber
   );

DBIResult DBIFN DbiSetNumberFormat (  // Set current number format
      pFMTNumber     pfmtNumber
   );

//============================================================================
//                      Conversions
//============================================================================

DBIResult DBIFN DbiNativeToAnsi(      // Convert from native to Ansi
      pVOID    pLdObj  ,                // Language driver
      pCHAR    pAnsiStr,                // Destination buffer (opt)
      pCHAR    pNativeStr ,             // Source buffer
      UINT32   iLen    ,                // Length of buffer (opt)
      pBOOL    pbDataLoss               // Returns TRUE if conversion will loose data (opt)
   );

DBIResult DBIFN DbiAnsiToNative(      // Convert from Ansi to native
      pVOID pLdObj  ,                   // Language driver
      pCHAR    pNativeStr ,             // Destination buffer (opt)
      pCHAR    pAnsiStr,                // Source buffer
      UINT32   iLen    ,                // Length of buffer (opt)
      pBOOL    pbDataLoss               // Returns TRUE if conversion will loose data (opt)
   );

//============================================================================
//                            Filters
//============================================================================
#if !defined(MIDL)
DBIResult DBIFN DbiAddFilter(         // Add a filter to the cursor
      hDBICur           hCursor,        // Cursor handle
      UINT32            iClientData,    // Client supplied data      (opt)
      UINT16            iPriority,      // 1..N with 1 being highest (opt)
      BOOL              bCanAbort,      // TRUE if pfFiltercan return ABORT (opt)
      pCANExpr          pcanExpr,       // Expression tree        (opt)
      pfGENFilter       pfFilter,       // ptr to filter function (opt)
      phDBIFilter       phFilter        // Returns filter handle
   );
#endif // !defined(MIDL)

DBIResult DBIFN DbiDropFilter(        // Drop a filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

DBIResult DBIFN DbiActivateFilter(    // Activate a Filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

DBIResult DBIFN DbiDeactivateFilter(  // Deactivate Filter
      hDBICur           hCursor,        // Cursor handle
      hDBIFilter        hFilter         // Filter handle
   );

#if !defined(MIDL)
DBIResult DBIFN  DbiGetFilterInfo(    // Get filter information
      hDBICur           hCur    ,       // Cursor handle
      hDBIFilter        hFilter ,       // Filter handle          /NULL
      UINT16            iFilterId,      // Filter id              /0
      UINT16            iFilterSeqNo,   // Filter sequence number /0
      pFILTERInfo       pFilterinfo     // Returns filter info
  );
#endif // !defined(MIDL)

//============================================================================
//                            Linked Cursors
//============================================================================

DBIResult DBIFN DbiBeginLinkMode(     // Convert cursor to a link cursor
      phDBICur          phCursor        // In/Out : returns new cursor
    );

DBIResult DBIFN DbiEndLinkMode (      // Convert cursor back to normal cursor
      phDBICur       phCursor           // In/Out : returns original cursor
   );

DBIResult DBIFN DbiLinkDetail(        // Link detail to master
      hDBICur        hMstrCursor,       // Master cursor
      hDBICur        hDetlCursor,       // Detail cursor
      UINT16         iLnkFields,        // Number of link fields
      pUINT16        piMstrFields,      // Array of fields in master
      pUINT16        piDetlFields       // Array of fields in detail
    );

DBIResult DBIFN DbiLinkDetailToExp(   // Link detail to a master using exp
        hDBICur         hCursorMstr,    // Master cursor
        hDBICur         hCursorDetl,    // Detail cursor
        UINT16          iKeyLen,        // Key length to match
        pCHAR           pszMstrExp      // Expression string
    );

DBIResult DBIFN DbiUnlinkDetail (     // Unlink detail from master
      hDBICur        hDetlCursor        // Detail cursor to unlink
    );

DBIResult DBIFN DbiGetLinkStatus(     // Query linkage info for table
        hDBICur         hCursor,        // Cursor handle
        phDBICur        phCursorMstr,   // Returns master cursor, if any   (opt)
        phDBICur        phCursorDet,    // Returns first detail cursor, if any (opt)
        phDBICur        phCursorSib     // Returns next sibling detail cursor, if any (opt)
    );


//===========================================================================
//                            Translation
//===========================================================================


DBIResult DBIFN DbiTranslateRecordStructure ( // Translate a record
      pCHAR          pszSrcDriverType,  // Source driver type
      UINT16         iFlds,             // Number of fields
      pFLDDesc       pfldsSrc,          // Array of source fields: logical or physical types
      pCHAR          pszDstDriverType,  // Destination driver type
      pCHAR          pszLangDriver,     // Language driver for destination
      pFLDDesc       pfldsDst     ,     // Array of dest. fields returned
      BOOL           bCreatable         // TRUE -> map to creatable fields only.
   );

DBIResult DBIFN DbiOpenFieldXlt (     // Open translation object
      pCHAR          pszSrcTblType,     // NULL for Logical
      pCHAR          pszSrcLangDrv,     // NULL if no tranliteration
      pFLDDesc       pfldSrc,           // source field descriptor
      pCHAR          pszDestTblType,    // NULL for Logical
      pCHAR          pszDstLangDrv,     // NULL if no tranliteration
      pFLDDesc       pfldDest,          // Source field descriptor
      pBOOL          pbDataLoss,        // Set to TRUE, for data loss
      phDBIXlt       phXlt              // Returned translate handle
   );

DBIResult DBIFN DbiTranslateField (   // Translate a field
      hDBIXlt        hXlt,              // Translation handle
      pBYTE          pSrc,              // Source field
      pBYTE          pDest              // Destination field
   );

DBIResult DBIFN DbiCloseFieldXlt (    // Close translation object
      hDBIXlt        hXlt               // Translation handle
   );

//=========================================================================
//    Delayed Updates
//=========================================================================

DBIResult DBIFN DbiBeginDelayedUpdates(  // put cursor in delayed update mode
      phDBICur         phCursor          // In/Out : returns new Cursor
    );

DBIResult DBIFN DbiEndDelayedUpdates(  // Convert cursor back to normal cursor
      phDBICur         phCursor        // In/Out : returns original Cursor
    );

typedef enum                      // Op types for Delayed Update cursor
   {
      dbiDelayedUpdCommit          = 0,  // Commit the updates
      dbiDelayedUpdCancel          = 1,  // Rollback the updates
      dbiDelayedUpdCancelCurrent   = 2,  // Cancel the Current Rec Change.
      dbiDelayedUpdPrepare         = 3   // Phase1 of two Phase commit.
   }  DBIDelayedUpdCmd;

DBIResult DBIFN DbiApplyDelayedUpdates ( // Perform the specified operation.
    hDBICur            hCursor,          // Delayed update cursor handle
    DBIDelayedUpdCmd   eUpdCmd           // Op Type: Commit or Rollback.
    );

#define DBIDELAYUPD_SHOWMODIFYBIT      (0x1)    // Show only modified records.
#define DBIDELAYUPD_SHOWINSERTBIT      (0x2)    // Show only inserted records.
#define DBIDELAYUPD_SHOWDELETEBIT      (0x4)    // Show only deleted records.
#define DBIDELAYUPD_SHOWNONMODIFYBIT   (0x8)    // Show only unmodified recs.

//===========================================================================
//                                 MISC.
//===========================================================================


DBIResult DBIFN DbiGetTableOpenCount (   //  Get local cursor count
      hDBIDb         hDb,                    // Database
      pCHAR          pszTableName,           // Table name
      pCHAR          pszDriverType,          // Driver type /NULL
      pUINT16        piOpenCount             // returned number of cursors
);

DBIResult DBIFN DbiUseIdleTime (      // Use Idle time
      VOID
   );

DBIResult DBIFN DbiGetLdObj (         // Get language driver
      hDBICur        hCursor,           // Cursor handle
      pVOID         *ppLdObj            // Returned language driver object
   );

DBIResult DBIFN DbiGetLdName (        // Get language driver name from table
      pCHAR         pszDriver,          // Driver name
      pCHAR         pObjName,           // Name of object, i.e. table name
      pCHAR         pLdName             // Returned language driver name
   );

DBIResult DBIFN DbiFormFullName (     // Form Full Name
      hDBIDb         hDb,               // Database handle
      pCHAR          pszTableName,      // Table name
      pCHAR          pszDriverType,     // Driver type /NULL
      pCHAR          pszFullName        // Returns full name
   );

DBIResult DBIFN DbiAddPassword (      // Add a password to current session
      pCHAR          pszPassword        // Password
   );

DBIResult DBIFN DbiDropPassword (     // Drop a password from current session
      pCHAR          pszPassword        // password/NULL
   );

DBIResult DBIFN DbiGetNetUserName (   // Get network username
      pCHAR          pszNetUserName     // Returns username
   );


DBIResult DBIFN DbiDebugLayerOptions (// Get SDK debug layer options
      UINT16         iOption,           // Option
      pCHAR          pDebugFile         //
   );

DBIResult DBIFN DbiOpenCfgInfoList (  // Open a cursor on "Config"
      hDBICfg        hCfg,              // NULL
      DBIOpenMode    eOpenMode,         // ReadWrite or readonly
      CFGMode        eConfigMode,       // Config mode
      pCHAR          pszCfgPath,        // Path
      phDBICur       phCur              // Returned cursor
  );

DBIResult DBIFN DbiAddAlias (         // Add a new alias
      hDBICfg        hCfg,              // NULL
      pCHAR          pszAliasName,      // Alias name
      pCHAR          pszDriverType,     // Driver type for alias
      pCHAR          pszParams,         // Optional parameters
      BOOL           bPersist           // Persistent or session relative
  );

DBIResult DBIFN DbiDeleteAlias (      // Add a new alias
      hDBICfg        hCfg,              // NULL
      pCHAR          pszAliasName       // Alias name
  );



#ifndef NODBIQBE
//===========================================================================
//                      Query Management
//===========================================================================

DBIResult DBIFN DbiQExecDirect (      // Execute query
      hDBIDb         hDb,               // Database handle
      DBIQryLang     eQryLang,          // Query language
      pCHAR          pszQuery,          // Query
      phDBICur       phCur              // Returned cursor on result set
   );

DBIResult DBIFN DbiQAlloc (         // Allocates a statement handle
      hDBIDb         hDb,               // Database handle
      DBIQryLang     eQryLang,          // Query language
      phDBIStmt      phStmt             // Returned statment handle
   );


DBIResult DBIFN DbiQPrepare (         // Prepare a query
      hDBIStmt       hStmt,             // Statment handle
      pCHAR          pszQuery           // Query
   );

DBIResult DBIFN DbiQPrepareExt (      // Prepare a query
      hDBIDb         hDb,               // Database handle
      DBIQryLang     eQryLang,          // Query language
      pCHAR          pszQuery,          // Query
      UINT16         propBits,          // properties for Prepare, e.g. qprepFORUPDATE
      phDBIStmt      phStmt             // Returned statment handle
   );

DBIResult DBIFN DbiQExec (            // Execute prepared query
      hDBIStmt       hStmt,             // Statement handle
      phDBICur       phCur              // Returned handle on result set
   );

DBIResult DBIFN DbiQFree (            // Free statement handle
      phDBIStmt      phStmt             // Statement handle
   );

DBIResult DBIFN DbiQSetParams (       // Set query options
      hDBIStmt       hStmt,             // Statement handle
      UINT16         uFldDescs,         // Number of parameter field descriptors
      pFLDDesc       paFldDescs,        // Array of parameter field descriptors
      pBYTE          pRecBuff           // Record buffer
   );

DBIResult DBIFN DbiQInstantiateAnswer ( // Create answer table
      hDBIStmt   hStmt,                 // Statement Handle
      hDBICur    hCur,                  // Cursor Handle
      pCHAR      pszAnswerName,         // Answer Table Name/NULL
      pCHAR      pszAnswerType,         // Answer Table Type/NULL
      BOOL       bOverWrite,            // Overwrite Flag
      phDBICur   phCur                  // cursor to instantiated table (output)(optional)
   );

DBIResult DBIFN DbiQExecProcDirect (    // Direct execution of stored procedure
      hDBIDb         hDb,               // Database handle
      pCHAR          pszProc,           // Stored procedure name
      UINT16         uParamDescs,       // Number of parameter descriptors
      pSPParamDesc   paParamDescs,      // Array of parameter descriptors
      pBYTE          pRecBuff,          // Record buffer
      phDBICur       phCur              // Returned handle on result set
   );

DBIResult DBIFN DbiQPrepareProc (       // Prepare a stored procedure
      hDBIDb         hDb,               // Database handle
      pCHAR          pszProc,           // Stored procedure name
      UINT16         uParamDescs,       // Number of parameter descriptors
      pSPParamDesc   paParamDescs,      // Array of parameter descriptors
      pBYTE          pRecBuff,          // Record buffer
      phDBIStmt      phStmt             // Returned statment handle
   );

DBIResult DBIFN DbiQSetProcParams (     // Set procedure params
      hDBIStmt       hStmt,             // Statement handle
      UINT16         uParamDescs,       // Number of parameter descriptors
      pSPParamDesc   paParamDescs,      // Array of parameter descriptors
      pBYTE          pRecBuff           // Record buffer
   );

typedef struct
   {
      DBINAME     szDatabase;
      DBITBLNAME  szTableName;
      DBINAME     szFieldName;
      BOOL        bExpression;
      BOOL        bAggregate;
      BOOL        bConstant;
   }  STMTBaseDesc;
typedef STMTBaseDesc far *pSTMTBaseDesc;

DBIResult DBIFN DbiQGetBaseDescs(
      hDBIStmt     hStmt,      // Statement Handle
      phDBICur     phCur       // Cursor of type StatementBaseDesc
      );

#endif  // NODBIQBE

#endif  // NODBIPROTOTYPES

#ifdef __cplusplus
   }
#endif // __cplusplus

#if ! (defined(lint) || defined(_lint))
#  if ( _MSC_VER >= 800 ) || (defined(__BORLANDC__) && defined(__FLAT__))
#    pragma warning(disable:4103)
#    if !(defined( MIDL_PASS )) || defined( __midl )
#      pragma pack(pop)
#    else
#      pragma pack()
#    endif
#  elif defined(__BORLANDC__) && !defined(__FLAT__)
#    pragma option -a.
#  else
#    pragma pack()
#  endif
#endif // ! (defined(lint) || defined(_lint))

#endif


