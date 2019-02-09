#ifdef DBE
#undef NONLS
#include <windows.h>
#include <bde\idapi.h>
#include "collect.h"
#include "dbe.h"
#include "dbengine.h"
#include <stdio.h>
#include <mem.h>
#include "treeview.h"
#include "tree_itm.h"
//#include <ctype.h>  //isAlpha
#include "class.h"
#include "project.h"

#define IDX_PRIMARY        1
#define IDX_UNIQUE         2
#define IDX_DESCENDING     4
#define IDX_MAINTAINED     8
#define IDX_SUBSET         16
#define IDX_EXPR           32
#define IDX_IDXOOD         64
#define IDX_CASEINSENS     128


int StrToDouble(char*text,double&value);

typedef struct tag_DBTYPE{UINT type;char *name;}_DBTYPE;
#pragma  +wpin

static _DBTYPE TypesName[]={
{fldZSTRING  	,"ZSTRING"   } ,     // Null terminated string
{fldDATE     	,"DATE"      } ,     // Date     (32 bit)
{fldBLOB     	,"BLOB"      } ,     // Blob
{fldBOOL     	,"BOOL"      } ,     // Boolean  (16 bit)
{fldINT16    	,"INT16"     } ,     // 16 bit signed number
{fldINT32    	,"INT32"     } ,     // 32 bit signed number
{fldFLOAT    	,"FLOAT"     } ,     // 64 bit floating point
{fldBCD      	,"BCD"       } ,     // BCD
{fldBYTES    	,"BYTES"     } ,     // Fixed number of bytes
{fldTIME     	,"TIME"      } ,     // Time        (32 bit)
{fldTIMESTAMP	,"TIMESTAMP" } ,     // Time-stamp  (64 bit)
{fldUINT16   	,"UINT16"    } ,     // Unsigned 16 bit integer
{fldUINT32   	,"UINT32"    } ,     // Unsigned 32 bit integer
{fldFLOATIEEE	,"FLOATIEEE" } ,     // 80-bit IEEE float
{fldVARBYTES 	,"VARBYTES"  } ,     // Length prefixed var bytes
{fldLOCKINFO 	,"LOCKINFO"  },

{fldBLOB     	,"MEMO"      },     // Blob
{fldBLOB      	,"BINARY"    },     // Blob
{fldBLOB     	,"FMTMEMO"   },     // Blob
{fldBLOB      	,"OLEOBJ"    },     // Blob
{fldBLOB     	,"GRAPHIC"   },     // Blob
{fldBLOB   	   ,"DBSOLEOBJ" },
{fldBLOB  	   ,"TYPEDBINARY"},

{fldINT32    	,"INTEGER"   }  ,     // 16 bit signed number
{fldINT32       ,"AUTOINC" },

{fldBCD      	,"DECIMAL"  }   ,     // BCD
{fldINT16    	,"SMALLINT" }      ,     // 16 bit signed number
{fldFLOAT    	,"MONEY"    }     // 64 bit floating point

};     // Look for LOCKINFO typedef

static _DBTYPE SubName[]={
{fldstMEMO       	,"MEMO"      },     // Blob
{fldstBINARY     	,"BINARY"    },     // Blob
{fldstFMTMEMO    	,"FMTMEMO"   },     // Blob
{fldstOLEOBJ     	,"OLEOBJ"    },     // Blob
{fldstGRAPHIC    	,"GRAPHIC"   },     // Blob
{fldstDBSOLEOBJ  	,"DBSOLEOBJ" },
{fldstTYPEDBINARY ,"TYPEDBINARY"},
{fldstMONEY    	,"MONEY"},          // 64 bit floating point
{fldstAUTOINC     ,"AUTOINC"}};


UINT GetType(char *name,int t1,int t2,pFLDDesc f=NULL){
 UINT type=0;
 UINT subtype=0;
 for(int i=0;i<sizeof(TypesName)/sizeof(_DBTYPE);i++){
  if(!lstrcmp(name,TypesName[i].name)){type=TypesName[i].type;break;}
 }

 if (type){
 for(int i=0;i<sizeof(SubName)/sizeof(_DBTYPE);i++){
  if(!lstrcmp(name,SubName[i].name)){subtype=SubName[i].type;break;}
 };
 }

 if(((!lstrcmp(name,"CHAR")) || (!lstrcmp(name,"CHARACTER"))
		 ||(!lstrcmp(name,"STRING")) || (!lstrcmp(name,"VARCHAR")) || (!lstrcmp(name,"VARCHAR2"))) && t1)
 {
  type=fldZSTRING;
  if (f)f->iUnits1=(INT16)t1;
 }
 if (!lstrcmp(name,"NUMBER") || !lstrcmp(name,"NUMERIC")){
  if (t1){
	if (t1==6 && t2==10)type=fldINT16;else
	{ if (t1==20 && t2==4)type=fldINT32;else type=fldFLOAT;
	 }}
 };

 if (f && type){
  f->iFldType=(INT16)type;
  f->iSubType=(INT16)subtype;
 }
return type;
};

//#define BDE_DEBUG

DBIResult rslt=0;
BOOL sqlloaded=0;
THandlers * bases=NULL;
THandlers * tables=NULL;

int strpos(char*str,char*fragment);

void DBSQLPParam(int&p){
 p=0;
 if (sqlloaded){
  p|=1;
  for(int i=0;i<bases->count;i++){
	TBase*b=(TBase*)bases->At(i);
	if (b){p|=2;return;}
  }
 }

};

DBIResult CHKERROR(DBIResult _code){
 rslt=(_code);
 if (rslt != DBIERR_NONE){
	char s[256];
	DbiGetErrorString (rslt,s);
   AddObjectName(s);
	_Message(s);
 }
 return rslt;
};
int RETURN(){
 return rslt==0;
}

#define CharNULL(s)   (s?(s[0]?s:NULL):NULL)
void GetModes(char *flags,DBIOpenMode&om,DBIShareMode&sm,BOOL&closeonwin)
{
	om=dbiREADWRITE;
	sm=dbiOPENSHARED;
	closeonwin=0;
	char _flags[256];
	lstrcpy(_flags,flags);
	AnsiUpper(_flags);
	if (strpos(_flags,"DBIREADONLY")>-1)om=dbiREADWRITE;
	if (strpos(_flags,"DBIOPENEXCL")>-1)sm=dbiOPENEXCL;
	if (strpos(_flags,"DBIAUTOCLOSE")>-1)closeonwin=TRUE;
}

TTable::TTable(hDBICur  hCur,TBase* _base){
memset(this,0,sizeof(TTable));
um=UM_NONE;
base=_base;
hCursor=hCur;
 if (CHKERROR(DbiGetCursorProps(hCursor, &curProps)) == DBIERR_NONE)
 {
 // Allocate the record buffer
	pRecBuf = new BYTE[curProps.iRecBufSize];
 // Check result of malloc
  pFldArray = (pFLDDesc) new BYTE[sizeof(FLDDesc) *  curProps.iFields];
  CHKERROR(DbiGetFieldDescs(hCur, pFldArray));
  CHKERROR(DbiGetLdObj (hCursor,&ppLdObj));

  DBIResult rslt1=DbiGetNextRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps);
	position=0;
  if (!rslt && !rslt1){ posvalid=1; }else {posvalid=0;}
  if (rslt1){empty=TRUE;CHKERROR(DbiInitRecord (hCursor,pRecBuf));}
 }
};
int TTable::PackBase(){
 Flush();
 CHKERROR(DbiPackTable(base->hDb,hCursor,NULL,NULL,1));
  GetTop();
 NotifyWindows(U_ALL,0,0);
 return TRUE;
};

BOOL DbPackTable(int table){
TTable*t=GetCursor(table);
if(t)return t->PackBase();
return FALSE;
};

BOOL DBSort(int table,UINT16 flags,int count,char **names){

TTable*t=GetCursor(table);
if(t && count){
pUINT16       rec = new UINT16[count];
pSORTOrder sorder = new SORTOrder[count];
pBOOL      scase  = new BOOL[count];

 memset(sorder,0,sizeof(SORTOrder)*count);
 memset(scase,0 ,sizeof(BOOL)*count);

 for(int i=0;i<count;i++){
  char s[256];
  lstrcpy(s,names[i]);
  char name[256];
  char *ps=s;
   while(*ps && *ps!='/')ps++;
   lstrcpyn(name,s,ps-s+1);
    rec[i]=t->GetFId(name);
 if(*ps=='/'){
   ps++;
  switch(*ps){
   case 'c':scase[i]=0;break;
   case 'i':scase[i]=1;break;
   case 'd':sorder[i]=1;break;
   case 'a':sorder[i]=0;break;
  }
 }
}
t->Flush();
   char fullname[1024];
   UINT16 size;
   CHKERROR(DbiGetProp(t->hCursor,curFILENAME, (pVOID)fullname, sizeof(fullname), &size));

//   CHKERROR(DbiCloseCursor (&(t->hCursor)));
//   t->hCursor=0;

HCURSOR old=SetCursor(LoadCursor(0,IDC_WAIT));
CHKERROR(DbiSortTable(t->base->hDb,fullname,
                                   t->curProps.szTableType,
                                   t->hCursor,    // hSrcCur
                                   NULL,    // pszSortedName "c:\\test.dbf"
                                   NULL,    // phSortedCur
                                   NULL,    // hDstCur
                                   count,   // iSortFields
                                   rec,     // piFieldNum
                                   scase,
                                   sorder,
                                   NULL,(flags&1)!=0,NULL,0));

if(!t->hCursor){
t->ReOpen();
}
t->GetTop();
t->NotifyWindows(U_ALL,0,0);

SetCursor(old);

delete rec;
delete sorder;
delete scase;
return RETURN();
}
return FALSE;
};
void TTable::ReOpen(){
/*
	  CHKERROR(DbiOpenTable(base->hDb,
					curProps.szTableType,
					NULL,
					NULL,NULL,
					openmode, sharemode,
					xltFIELD,
					FALSE,
					NULL,
					&hCursor));*/
};

void TTable::NotifyWindows(UPDATE_BROWSE code,long y,long x){
if(!browsers)return;
TTableWnd *b=browsers;
 TCursorChangeInfo c;
 c.code=code;
 c.x=x;
 c.y=y;
 while(b){
  SendMessage(b->hwnd,TB_CURSORCHANGE,0,(LPARAM)&c);
  b=b->next;
 }

};

BOOL TTable::InsertWindow(HWND HWindow){
TTableWnd *b=browsers;
 while(b){
  if(b->hwnd==HWindow)return FALSE;
  b=b->next;
 }
TTableWnd*t=new TTableWnd();
t->next=NULL;
t->hwnd=HWindow;
if(browsers){
 b=browsers;
 while(b->next){
  b=b->next;
}
b->next=t;
}else browsers=t;
return TRUE;
};

BOOL TTable::RemoveWindow(HWND HWindow){
TTableWnd *b=browsers;
TTableWnd *_b=NULL;
 while(b){
  if(b->hwnd==HWindow){
   if(b==browsers){browsers=b->next; }else {_b->next=b->next;}
   delete b;
   return TRUE;
  }
  _b=b;
  b=b->next;
 }
return FALSE;
};

void TTable::GetNext(){
 Flush();
 CHKERROR(DbiGetNextRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
 if (!rslt && posvalid)position++;else posvalid=0;
};
void TTable::GetPrior(){
 Flush();
 CHKERROR(DbiGetPriorRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
 if (!rslt && posvalid)position--;else posvalid=0;
};
void TTable::GetRelative(long delta){
 if (delta){
  Flush();
  if (delta<2 && delta>-2){
	 if (delta==1){
      rslt=DbiGetNextRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps);
		if(!((rslt==DBIERR_EOF)&&EXEC_FLAGS&EF_STEPACTIVE))  CHKERROR(rslt);
      }else
		CHKERROR(DbiGetPriorRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
  }else{
	CHKERROR(DbiGetRelativeRecord (hCursor,delta,dbiNOLOCK,pRecBuf,&recProps));
  }
 if (!rslt && posvalid)position+=delta;else posvalid=0;
 }
};
void TTable::GetTop(){
 Flush();
 CHKERROR(DbiSetToBegin(hCursor));
 CHKERROR(DbiGetNextRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
 position=0;
 if (!rslt){posvalid=1;}
};
void TTable::GetBottom(){
 Flush();
 CHKERROR(DbiSetToEnd(hCursor));
 CHKERROR(DbiGetPriorRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
 UINT32 count;
 if (!rslt && (CHKERROR(DbiGetRecordCount (hCursor,&count))==DBIERR_NONE)){
 posvalid=1;
 position=count-1;
 }else posvalid=0;
};

int DBSkip(int table,long delta){
 TTable*t=GetCursor(table);
 if (t){ t->GetRelative(delta);
		 if(t->posvalid)t->NotifyWindows(U_CURPOS,t->position,0);
 }return (rslt==0);
};

int DBSkipTo(int table,long y){
TTable*t=GetCursor(table);
 if (t){
  if(!t->posvalid){
   t->GetTop();
  }
  if (y!=t->position){
    long delta=y-t->position;
	  t->GetRelative(delta);
    }
   return (rslt==0);
  }
  return FALSE;
};
long DBRecno(int table){
TTable*t=GetCursor(table);
 if (t){
  return t->position;
 }
 return -1;
};
TTable::~TTable(){
 Flush();
 CHKERROR(DbiCloseCursor (&hCursor));
 if(pRecBuf)delete pRecBuf;
 if(pFldArray) delete pFldArray;
 if (mark)delete mark;
 NotifyWindows(U_REMOVE,0,0);

 while(browsers){
  TTableWnd *b=browsers;
   browsers=b->next;
   delete b;
 }
};
void TTable::Flush(){
 if(um!=UM_NONE){
 UpdateMode _um=um;
  um=UM_NONE;
 switch (_um){
 case UM_MODIFY:CHKERROR(DbiModifyRecord (hCursor,pRecBuf, dbiNOLOCK));
				 NotifyWindows(U_RECORD,position,0);
				 break;
 case UM_INSERT:CHKERROR(DbiInsertRecord (hCursor, dbiNOLOCK,pRecBuf));
				 NotifyWindows(U_INSERT,position,0);
				 break;
 case UM_APPEND:CHKERROR(DbiAppendRecord (hCursor,pRecBuf));
				 NotifyWindows(U_APPEND,0,0);
				 break;
 }
 }
};

BOOL TTable::IsDBASE()
{
	return (!lstrcmp(curProps.szTableType,szDBASE));
};

int TTable::DeleteRecord(BOOL pack)
{
	switch (um)
  {
 	case UM_APPEND:
	case UM_INSERT:um=UM_NONE;break;
	case UM_MODIFY:um=UM_NONE;
	case UM_NONE:
  	DbiDeleteRecord (hCursor,NULL);
		UINT32 count;
		if(pack && IsDBASE())
    {
			CHKERROR(DbiPackTable(base->hDb,hCursor,NULL,NULL,1));
    };
		GetTop();
		CHKERROR(DbiGetRecordCount (hCursor,&count));
		if(position>=count)position=count-1;
		posvalid=0;
		NotifyWindows(U_DELETE,position,0);
  }
 	return rslt;
};

BOOL TTable::UnDeleteRecord(){
  if(CHKERROR(DbiUndeleteRecord(hCursor)==DBIERR_NONE)){
   NotifyWindows(U_ALL,position,0);
   return TRUE;
  }
  return FALSE;
};


BOOL FormatDate(DBIDATE*date,char*out){
	INT16 piYear;
	UINT16 piMon, piDay;
	if(CHKERROR(DbiDateDecode (*date,&piMon,&piDay,&piYear)) == DBIERR_NONE){
#ifdef WIN32
   SYSTEMTIME st;
	st.wYear=piYear;
	st.wMonth=piMon;
	st.wDayOfWeek=0;
	st.wDay=piDay;
   GetDateFormat(0,
	 DATE_SHORTDATE	,	// flags specifying function options
	&st,	// date to be formatted
	NULL,	// date format string
	out,	// buffer for storing formatted string
	255 	// size of buffer
   );
#else
  wsprintf(out,"%d,%d,%d",piDay,piMon, piYear);
#endif
   return TRUE;
   }else lstrcpy(out,"--:--:--");
  return FALSE;
};
BOOL FormatTime(TIME *timeT,char *out){
	 UINT16 piHour, piMin, piMilSec;
	if(CHKERROR(DbiTimeDecode (*timeT,&piHour,&piMin,&piMilSec)) == DBIERR_NONE){
   UINT16 sec=piMilSec/1000;
   UINT16 msec=piMilSec%1000;

#ifdef WIN32
   SYSTEMTIME st;
	st.wHour=piHour;
	st.wMinute=piMin;
	st.wSecond=sec;
	st.wMilliseconds=msec;

   GetTimeFormat(0,
	 0	,	// flags specifying function options
	&st,	// date to be formatted
	NULL,	// date format string
	out,	// buffer for storing formatted string
	255 	// size of buffer
   );
#else
  wsprintf(out,"%d:%d,%d.%d",piHour,piMin,piMin,msec);
#endif
  return TRUE;
	  }else lstrcpy(out,"-------");
  return FALSE;
};

void TTable::GetText(INT16 id,char *out){
#ifdef LOGON
LogMessage("Вошел в TTable::GetText");
#endif
	if(id==-2)
 	{
#ifdef LOGON
LogMessage("id==-2");
#endif
  	if(recProps.bDeleteFlag)lstrcpy(out,"1");
    else *out=0;
 	};
 	if (id>0 && id<=curProps.iFields)
  {
#ifdef LOGON
LogMessage("id>0");
#endif
  	out[0]=0;
  	union{
   		TIME    	*timeT;
   		DBIDATE 	*date;
   		TIMESTAMP	*timestamp;
			BYTE   		*in;
   		BOOL16 		*InBool16;
			INT16  		*InInt16;
   		INT32  		*InInt32;
   		UINT16 		*UInInt16;
   		UINT32 		*UInInt32;
			double 		*InDouble;
			long double *InLD;
  	};
  	int i=id-1;
  	BYTE buf[256];
  	in=buf;
  	BOOL blank;
  	UINT16 type=pFldArray[i].iFldType;
    // lstrcpy(out,"tst");return;
#ifdef LOGON
LogMessage("id>0");
#endif
    if (type!=fldBLOB)
    {
#ifdef LOGON
LogMessage("type!=fldBLOB");
#endif
	 		CHKERROR(DbiGetField(hCursor,id,pRecBuf,buf,&blank));
  	}
 		if(blank)
    {
#ifdef LOGON
LogMessage("blank");
#endif
    *out=0;  return;
    };

#ifdef LOGON
LogMessage("switch (type)");
#endif
  	switch (type)
    {
  		case fldUNKNOWN    :break;
  		case fldZSTRING    :// Null terminated string
			{
#ifdef LOGON
LogMessage("fldZSTRING begin");
#endif
				BOOL pbDataLoss;
				if((ppLdObj==NULL)
        		|| CHKERROR(DbiNativeToAnsi(ppLdObj,out,(char*)in,0,&pbDataLoss))
						|| pbDataLoss)
        {
#ifdef LOGON
LogMessage("lstrcpy");
#endif
						lstrcpy(out,(char*)in);
        }
#ifdef LOGON
LogMessage("fldZSTRING end");
#endif
      }break;
 			case fldDATE       :// Date     (32 bit)
        FormatDate(date,out);
        break;
  		case fldBLOB       :// Blob
				lstrcpy(out,"MEMO");break;
  		case fldBOOL       :// Boolean  (16 bit)
				if (*InBool16)lstrcpy(out,"TRUE");else lstrcpy(out,"FALSE");
				break;
  		case fldINT16      :// 16 bit signed number
				wsprintf(out,"%d",*InInt16);
				break;
  		case fldINT32      :// 32 bit signed number
				wsprintf(out,"%ld",*InInt32);
				break;
  		case fldFLOAT      :// 64 bit floating point
				sprintf(out,"%G",*InDouble);
				break;
  		case fldBCD        :// BCD
				lstrcpy(out,"BCD");break;
  		case fldBYTES      :// Fixed number of bytes
				lstrcpy(out,"Bytes");break;
      case fldTIME       :// Time        (32 bit)
				FormatTime(timeT,out);
				break;
		  case fldTIMESTAMP  :// Time-stamp  (64 bit)
				DBIDATE d;TIME t;
				CHKERROR(DbiTimeStampDecode(*timestamp,&d,&t));
				FormatTime(timeT,out);
				lstrcat(out," ");
				FormatDate(date,out+lstrlen(out));
        break;
  		case fldUINT16     :// Unsigned 16 bit integer
				wsprintf(out,"%u",*UInInt16);
				break;
  		case fldUINT32     :// Unsigned 32 bit integer
				wsprintf(out,"%lu",*UInInt32);
				break;
  		case fldFLOATIEEE  :// 80-bit IEEE float
				sprintf(out,"%G",*InLD);
				break;
 			case fldVARBYTES   :// Length prefixed var bytes
  		case fldLOCKINFO   :// Look for LOCKINFO typedef
				break;
  	};
 	}
  else *out=0;
	#ifdef LOGON
	LogMessage("Вышел из TTable::GetText");
	#endif
};


double TTable::GetDouble(INT16 id){
 if (id>0 && id<=curProps.iFields){
 union{
	char   buf[256];
	long double dieee;
	double ddouble;
//	float  dfloat;
	INT16  dint16;
   INT32  dint32;
	WORD   dword;
	BOOL16 dbool;
	long   dlong;
	UINT32 duint32;
  };

  double ret=0;
  int i=id-1;
  BOOL blank;

  CHKERROR(DbiGetField(hCursor,id, pRecBuf,(BYTE*) buf, &blank));
  if(blank)return 0;

  switch (pFldArray[i].iFldType){
  case fldUNKNOWN    :break;
  case fldZSTRING    :// Null terminated string
							 {StrToDouble(buf,ret); }break;
  case fldDATE       :// Date     (32 bit)
							 ;break;
  case fldBLOB       :// Blob
							 ;break;
  case fldBOOL       :// Boolean  (16 bit)
							 if (dbool)ret=1.0;else ret=0.0;	 break;
  case fldINT16      :// 16 bit signed number
							  ret=dint16;  break;
  case fldINT32      :// 32 bit signed number
							  ret=dlong;
							 break;
  case fldFLOAT      :// 64 bit floating point
							  ret=ddouble;
                      break;
  case fldBCD        :// BCD
							 break;
  case fldBYTES      :// Fixed number of bytes
							 break;
  case fldTIME       :// Time        (32 bit)
							 break;
  case fldTIMESTAMP  :// Time-stamp  (64 bit)
							 break;
  case fldUINT16     :// Unsigned 16 bit integer
							 ret=dword; break;
  case fldUINT32     :// Unsigned 32 bit integer
							 ret=duint32;
							 break;
  case fldFLOATIEEE  :// 80-bit IEEE float
							 ret=dieee;	 break;
  case fldVARBYTES   :// Length prefixed var bytes
  case fldLOCKINFO   :// Look for LOCKINFO typedef
							 break;
  };
  return ret;
 }
return 0;
};

BOOL  TTable::SetText(INT16 id,char *in){
 if (id>0 && id<=curProps.iFields){
	// int i=id-1;
   _SetText(id,pRecBuf,in);
   if(indexactive)posvalid=FALSE;
 if (um!=UM_INSERT && um!=UM_APPEND){um=UM_MODIFY; }
 if(browsers)Flush();
 }
return RETURN();
}

BOOL  TTable::_SetText(INT16 id,pBYTE _pRecBuf,char *in){
  switch (pFldArray[id-1].iFldType){
  case fldUNKNOWN    :break;
  case fldFLOAT      :{double i;
                      if(StrToDouble(in,i)){
                       CHKERROR(DbiPutField (hCursor,id, _pRecBuf,(BYTE*)(&i))); break;
							 }else return FALSE;
                      }
					   break;
  case fldINT16      :{int i;
                     if(sscanf(in,"%d",&i)){
                       CHKERROR(DbiPutField (hCursor,id, _pRecBuf,(BYTE*)(&i))); break;
							 }else return FALSE;
                      }
					   break;
  case fldUINT16     :{int i;
                     if(sscanf(in,"%u",&i)){
                       CHKERROR(DbiPutField (hCursor,id, _pRecBuf,(BYTE*)(&i))); break;
							 }else return FALSE;
                      }
					   break;

  case fldZSTRING    :{// Null terminated string
                      BOOL pbDataLoss;
                      char out[256];
							 if((ppLdObj==NULL) || CHKERROR(DbiAnsiToNative(ppLdObj,out,(char*)in, 0,&pbDataLoss ))==0)
							 {
                       CHKERROR(DbiPutField (hCursor,id, _pRecBuf,(BYTE*)out));
                       }
                      }break;
  case fldBOOL       :{BOOL b=0;
                      if ((lstrcmpi(in,"TRUE")==0)||
                          (lstrcmpi(in,"YES")==0)||
                          (lstrcmpi(in,"1")==0)||
                          (lstrcmpi(in,"ДА")==0)
                         )b=TRUE;else
                         {
                          if ((lstrcmpi(in,"FALSE")==0)||
                          (lstrcmpi(in,"NO")==0)||
                          (lstrcmpi(in,"0")==0)||
                          (lstrcmpi(in,"НЕТ")==0)
						 );else return -1;
                         }
                      CHKERROR(DbiPutField (hCursor,id, _pRecBuf,(BYTE*)(&b))); break;
                      }
  default: return FALSE;
  }
return TRUE;
};

BOOL TTable::SetDouble(INT16 _id,double d){
 if ((_id>0) && (_id<=curProps.iFields)){
   if(indexactive)posvalid=FALSE;
	 int i=_id-1;
  union{
  char   dc[256];
  double dd;
  int    id;
  long   ld;
  UINT32 duint32;
  UINT16 duint16;
  long double  ldd;
  };
  switch (pFldArray[i].iFldType){
  case fldUNKNOWN    :break;
  case fldZSTRING    :// Null terminated string
							 sprintf(dc,"%G",d);
							 break;
  case fldINT16      :// 16 bit signed number
							 id=d;break;
  case fldINT32      :// 32 bit signed number
							 ld=d;
							 break;
  case fldFLOAT      :// 64 bit floating point
							 dd=d; break;
  case fldUINT16     :// Unsigned 16 bit integer
							 duint16=d; break;
  case fldUINT32     :// Unsigned 32 bit integer
							 duint32=d;
							 break;
  case fldFLOATIEEE  :// 80-bit IEEE float
							 ldd=d;	 break;
  default: return -1;
  }
 CHKERROR(DbiPutField (hCursor,_id, pRecBuf,(pBYTE)dc));
 if (um!=UM_INSERT && um!=UM_APPEND){um=UM_MODIFY; }
 if (browsers)Flush();
 }
 return RETURN();
};
C_TYPE THandlers::Insert(pointer item){
 for(C_TYPE i=0;i<count;i++){
				if (!_At(i)){ items[i]=item;return (C_TYPE)HbyI(i); }
  }
 return (C_TYPE)HbyI(TCollection::Insert(item));
};

pointer THandlers::Get(int handle){
 int i=IbyH(handle);
 if (i>=0 && i<count)return _At(i);
 return 0;
};

void 	THandlers::FreeItem(pointer p)
{
	if (!p)return;
	if (table)
  {
		delete (TTable*)p;
	}
  else
  {
		TBase* base=(TBase*)p;
		base->autoclose=FALSE;
		for(C_TYPE i=0;i<tables->count;i++)
    {
			TTable*t=(TTable*)tables->At(i);
 			if (t && t->base==base)
      {
 				t->autoclose=0;
				tables->FreeItem(t);
				tables->AtPut(i,NULL);
  		}
    }
  	delete base;
  }
};

int THandlers::DBEdel(int h)
{
  C_TYPE i=IbyH(h);
  if (i>=0 && i<count)
  {
		pointer p=At(i);
		if (p)
    {
    	FreeItem(p);
      AtPut(i,NULL);
      return 1;
    }
  }
 	return 0;
};

/*
int THandlers::DBEdel(int h){
 int i=IbyH(h);
  if (i>=0 && i<count){
			pointer p=tables->At(i);
			if (p){tables->FreeItem(p);tables->AtPut(i,NULL);return 1;}
		  };
 return 0;
};
*/
void AddMsgStr(char *s,long t);

BOOL DbiGetSysVersion(char* SysVer)
{
   SYSVersion SysVersion;
   pSYSVersion pSysVersion=&SysVersion;
   DBIResult   rslt;
   UINT16      Mo, Da, H, M, Ms;
   INT16       Yr;
   rslt = CHKERROR(DbiGetSysVersion(pSysVersion));
   if (rslt == DBIERR_NONE)
   {
      if (SysVer != NULL)
      {
         rslt = CHKERROR(DbiDateDecode(pSysVersion->dateVer, &Da, &Mo, &Yr));
         if (rslt != DBIERR_NONE)
            return rslt;
         rslt = CHKERROR(DbiTimeDecode(pSysVersion->timeVer, &H, &M, &Ms));
         if (rslt != DBIERR_NONE)

            return rslt;
         wsprintf(SysVer, "Engine: %d, Interface Level: %d, Date: %d/%d/%d, "
            "Time: %d:%d:%d", pSysVersion->iVersion, pSysVersion->iIntfLevel,
            Mo, Da, Yr, H, M, (Ms / 1000));
            return TRUE;
      }
   }
 return FALSE;
}


int DBInitSQL(){
  DWORD tic,tic1;
  tic=GetTickCount();

int rez=0;
HCURSOR old=SetCursor(LoadCursor(0,IDC_WAIT));
if (!sqlloaded){
int	HandlesAvail, HandlesWanted = 68;
  HandlesAvail = SetHandleCount(HandlesWanted);
	if (HandlesAvail < HandlesWanted)
	 { // Display message re: not enough available file handles
		_Error(EM_NOTHANDLES,0,0);
		goto _quit;
	 };
	rslt=0;
	 if ((rslt = DbiInit(NULL)) != DBIERR_NONE)
	 {	  if (rslt == DBIERR_CANTFINDODAPI)
		  {MessageBox(GetFocus(), "DbiInit( NULL ) failed - DLL's not found."
			  "\r\nCheck your path.", "SC3", MB_OK);
		  }
		  else
		  {MessageBox(GetFocus(), "DbiInit( NULL ) failed!\nTry to install Borland Database engine!", "SC3",  MB_OK);
		  }
		  goto _quit;

	 // Enable trace info if the debugging layer is enabled.
	}
	(bases=new THandlers())->table=0;
	(tables=new THandlers())->table=TRUE;
	sqlloaded=1;

#ifdef BDE_DEBUG
	CHKERROR(DbiDebugLayerOptions(DEBUGON | OUTPUTTOFILE, "DBPING.INF"));
#endif  //BDE_DEBUG
	rez=1;
	 }
_quit:
	if (rez){
	  tic1=GetTickCount();
     char s[256];
     lstrcpy(s,"Подключенно BDE (");
     DbiGetSysVersion(s+lstrlen(s));
     lstrcat(s," )");
	  AddMsgStr(s,tic1-tic);
	 }
	SetCursor(old);
 return sqlloaded;
};

int DoneSQL()
{
	if (sqlloaded)
  {
		DeleteCollection(bases);
		DeleteCollection(tables);
#ifdef BDE_DEBUG
		CHKERROR(DbiDebugLayerOptions(0, NULL));
#endif
		CHKERROR(DbiExit ());
		sqlloaded=0;
		return 1;
	}
  return 0;
};

int DBOpenDataBase(char *name,char*type,char * password,char * flags)
{
	if(!sqlloaded)DBInitSQL();

	hDBIDb hDb;
	DBIOpenMode om;
  DBIShareMode sm;
  BOOL autclose;
  GetModes(flags,om,sm,autclose);

  //http://www.progtown.com/topic204767-connection-through-bde.html
  /*char* UserName="root";
  FLDDesc UserNameDesc;
  memset(&UserNameDesc, 0, sizeof(UserNameDesc));
	UserNameDesc.iOffset = 0;
	UserNameDesc.iLen = (UINT16)(strlen(UserName) + 1);
	strcpy(UserNameDesc.szName, "USER NAME");*/

	CHKERROR(DbiOpenDatabase(
			  CharNULL(name),  // Database name - NULL for standard Database
			  CharNULL(type),  // Database type - NULL for standard Database
			  om,    // Open mode - Read/Write or Read only
			  sm,   // Share mode - Shared or Exclusive
			  CharNULL(password),  // Password - not needed for the STANDARD database
			  0,            // Number of Optional Parameters
			  NULL,          // Field Desc for Optional Parameters
			  NULL,          // Values for the optional parameters
        //1, &UserNameDesc,(pBYTE)UserName,
			  &hDb));          // Handle to the database
 	if (rslt == DBIERR_NONE)
 	{
  	C_TYPE h=bases->Insert(new TBase(hDb));
   	TBase *_b=GetBase(h);
		_b->autoclose=autclose;
		_b->_handle=h;
		return h;
 	}
 	return 0;
};

int DBSetDir(int _base,char*dir){
TBase *base= GetBase(_base);
 if (base){
  char s[260];
  GetFullFileNameEx((TProject*)base->project,dir,s);
  CHKERROR(DbiSetDirectory(base->hDb,s));
 }
 return RETURN();
};
int DBGoTop(int table){
TTable*t=GetCursor(table);
if (t){
t->GetTop();
}return RETURN();
};
int DBGoBottom(int table){
TTable*t=GetCursor(table);
if (t){
t->GetBottom();
}return RETURN();
};

int DBGetField(int table,INT16 id,char*field)
{
	#ifdef LOGON
	LogMessage("Вошел в DBGetField");
	#endif
  TTable*t=GetCursor(table);
	#ifdef LOGON
	LogMessage("после в GetCursor");
	#endif
  if (t)
  {
	  t->GetText((C_TYPE)id,field);
		#ifdef LOGON
		LogMessage("после в GetText");
		#endif
  }
  else *field=0;
	#ifdef LOGON
	LogMessage("Выход из DBGetField");
	#endif
 	return RETURN();
};

int DBGetField(int table,INT16 id,double*rez)
{
 TTable*t=GetCursor(table);
  if (t){*rez=t->GetDouble(id);}else *rez=0;
 return RETURN();
};
int DBGetField(int table,char *name,char*field){
 TTable*t=GetCursor(table);
  if (t){
  t->GetText(t->GetFId(name),field);
  }else *field=0;
 return RETURN();
};
int DBGetField(int table,char *name,double*rez){
 TTable*t=GetCursor(table);
  if (t){
  INT16 id=t->GetFId(name);
  *rez=t->GetDouble(id);
  }else *rez=0;
 return RETURN();
};
int DBSetField(int table,INT16 id,char*field){
 TTable*t=GetCursor(table);
 if (t)return t->SetText(id,field);
 return -1;
};
int DBSetField(int table,INT16 id,double field){
 TTable*t=GetCursor(table);
 if (t)return t->SetDouble(id,field);
 return -1;
};

int DBSetField(int table,char *name,char*field){
 TTable*t=GetCursor(table);
 if (t){
 INT16 id=t->GetFId(name);
 return t->SetText(id,field);}
 return -1;
};
int DBSetField(int table,char *name,double field){
 TTable*t=GetCursor(table);

 if (t){INT16 id=t->GetFId(name);
 return t->SetDouble(id,field);}
 return -1;
};

BOOL IsTableExist(int table){return GetCursor(table)!=0;};

void DBFieldName(int table,INT16 id,char*name){
 TTable*t=GetCursor(table);
 if (t && id>0 && id<=t->curProps.iFields){
  lstrcpy(name,t->pFldArray[id-1].szName);
 }else *name=0;
};
int  DBFieldType(int table,INT16 id){
 TTable*t=GetCursor(table);
 if (t && id>0 && id<=t->curProps.iFields){
  return t->pFldArray[id-1].iFldType;
 };return 0;
};
int  DBGetFLen(int table,INT16 id){
 TTable*t=GetCursor(table);
 if (t && id>0 && id<=t->curProps.iFields){
 int l=50;
  int type=t->pFldArray[id-1].iFldType;
  switch(type){
	case fldZSTRING:l= t->pFldArray[id-1].iUnits1*7;break;
	case fldINT16:l=50;
  }
	if (l<50)l=50;
	l*=1.5;
	return l;

 };return 0;
};
int DBInsertRecord(int table){
TTable*t=GetCursor(table);
 if (t){
  t->Flush();
  CHKERROR(DbiInitRecord (t->hCursor,t->pRecBuf));
  t->um=TTable::UM_INSERT;
  t->posvalid=FALSE;
  if(t->autoflush || t->browsers)t->Flush();
 }
 return RETURN();
};
int DBAppendRecord(int table){
TTable*t=GetCursor(table);
 if (t){
  t->Flush();
  CHKERROR(DbiInitRecord (t->hCursor,t->pRecBuf));
  t->posvalid=FALSE;
  t->um=TTable::UM_APPEND;
  if(t->autoflush || t->browsers || 1)t->Flush();
 }
 return RETURN();
};

int DBDeleteRecord(int table,BOOL pack)
{
	TTable*t=GetCursor(table);
 	if (t)return t->DeleteRecord(pack);
 	return RETURN();
};

UINT DBFieldCount(int table){
TTable*t=GetCursor(table);
 if (t)return t->curProps.iFields;
 return 0;
};

int DBOpenTable(int base,char*name,char*type,char *index,char*indextag,
				 UINT indexId,char * flags)
{
  TBase *_base=GetBase(base);
  if (_base)
  {
  	hDBICur hCur;
  	DBIOpenMode om;
  	DBIShareMode sm;
	  BOOL autoclose;
  	GetModes(flags,om,sm,autoclose);
	  char s[260],tmp_type[256];
  	GetFullFileNameEx((TProject*)_base->project,name,s);

	  char *_type=CharNULL(type);
  	if(_type)
    {
   		if(GetPrivateProfileString("table_types","_type",_type,tmp_type,sizeof(tmp_type),ini_file))
	    	_type=tmp_type;
  	}
		CHKERROR(DbiOpenTable(_base->hDb,s,
								_type,
								CharNULL(index),
								CharNULL(indextag),(UINT16)indexId,
								om, sm,
								xltFIELD,
								FALSE,
								NULL,
								&hCur));
	  if (rslt == DBIERR_NONE)
  	{
			TTable * t=new TTable(hCur,_base);
	   	t->openmode=om;
  	 	t->sharemode=sm;
   		t->autoclose=autoclose;
	    if (rslt == DBIERR_NONE)
		  	return tables->Insert(t);
		  else delete t;
		}
	}
 	return 0;
};

int DBGetLastError(){
 int r=rslt;rslt=0;
 return r;
};
int DBGetTextError(int r,char *txt){
 DBIResult rez=(DBIResult)r;
 DbiGetErrorString (rez,txt);
 return 1;
};

int DBCloseTable(int h)
{
 	BOOL ok=FALSE;
 	TTable*t=GetCursor(h);
 	if(t)
  {
  	BOOL ac=t->autoclose;
  	TBase* p=t->base;
  	ok=tables->DBEdel(h);
  	/*if(ok)
    {
   		BOOL canclose=TRUE;
    	for(C_TYPE i=0;i<tables->count;i++)
      {
     		TTable* _t=(TTable*)tables->At(i);
        if (_t && _t->base==p)
        {
        	canclose=FALSE;break;
        }
    	}
   		if(canclose)
     	{
      	DBCloseBase(p->_handle);
     	}
   	}*/
  }
 	return ok;
};

void DBCloseAll(void * project)
{
 	if (sqlloaded && bases)
 	{
 		for(C_TYPE i=0;i<bases->count;i++)
    {
  		TBase* p=(TBase*)bases->At(i);
  		if (p && ((p->project==project)||(project==NULL)))
      {
        DBCloseBase(i);
	 			/*bases->FreeItem(p);
	 			bases->AtPut(i,NULL);*/
  		}
 		}
 	}
}

int DBCloseBase(int h)
{
	return bases->DBEdel(h);
};


INT16 TTable::GetFId(char * Fname){
 if(*Fname=='#'){
  int num;
  if(sscanf(Fname+1,"%d",&num)==1){
   if(num>0 && num <=curProps.iFields)return num;
  }
 }else{
 for(INT16 i=0;i<curProps.iFields;i++){
  if (lstrcmpi(pFldArray[i].szName,Fname)==0)return (pFldArray[i].iFldNum);
  }
  return 0;
};
char s[256];
wsprintf(s,"Field %s not found in table",Fname);
 AddObjectName(s);
_Message(s);
return 0;
}

int DBGetFId(int table,char*field){
TTable*t=GetCursor(table);
if (t){return t->GetFId(field);}
  return -1;
};

int DBSQL(int base,char *query,int type)
{
 	TBase *_base=GetBase(base);
 	if (_base)
  {
		char _query[MAX_CLASSTEXTSIZE];
 		{ // пытаемся преобразовать в DOS строку
  		BOOL pbDataLoss;
  		pVOID ppLdObj; //Language driver
  		if(CHKERROR(DbiGetLdObj(0,&ppLdObj))==0)
      {
  			if(CHKERROR(DbiAnsiToNative(ppLdObj,_query,(char*)query, 0,&pbDataLoss ))==DBIERR_NONE)
        {
    			query=_query;
  			}
  		}
 		}
 		hDBICur hCur=0;

 		if(CHKERROR(DbiQExecDirect (_base->hDb,(DBIQryLang)type,query,&hCur))==DBIERR_NONE)
    {
			if (hCur)
      {
				TTable * t=new TTable(hCur,_base);
	 			if (rslt == DBIERR_NONE)
        {
	  			return tables->Insert(t);
	 			}
        else delete t;
  		}
    }
 	}
 	return 0;
};

double DBGetCount(int h)
{
	TTable*t=GetCursor(h);
	if (t)
  {
 		UINT32 count;
 		t->Flush();
 		if (CHKERROR(DbiGetRecordCount (t->hCursor,&count))==DBIERR_NONE)
    	return count;
 	}
 	return -1;
}

int DBAttachWindow(int table,HWND hwnd){
TTable*t=GetCursor(table);
if (t){
 t->InsertWindow(hwnd);
return TRUE;
}
 return 0;
};
int DBDeAttachWindow(int table,HWND hwnd){
TTable*t=GetCursor(table);
if (t){
 t->RemoveWindow(hwnd);
 if((t->browsers==0) && t->autoclose){
  DBCloseTable(table);
 }
return TRUE;
}
 return 0;
};

int DBDraw(int table,long y,long x,char *txt,COLORREF&color){
TTable*t=GetCursor(table);
if (t){
if(t->position==y && t->posvalid){
 t->GetText((INT16)(x+1),txt);
 return TRUE;
}
t->Flush();
BOOL marked=t->wasbegin;
if (!marked){ DBBeginDraw(table); }
  if (t->lastok){
	while(t->last<y){  t->GetNext();
	if(rslt==DBIERR_EOF)return FALSE;
	t->last++; }
	 t->GetText((INT16)(x+1),txt);
  }else{
		 if (t->posvalid)
	   {
			  if (y!=t->position){
			   long delta=y-t->position;
			   t->GetRelative(delta);
			if(rslt==DBIERR_EOF)return FALSE;
			  }
		  t->GetText((INT16)(x+1),txt);
		 }else
		 {
			  t->GetTop();
			  t->GetRelative(y);
		   if(rslt==DBIERR_EOF)return FALSE;
			  t->GetText((INT16)(x+1),txt);
		   }
	 t->lastok=1;
	 t->last=y;

  }
 if (t->recProps.bDeleteFlag && t->curProps.bSoftDeletes)color=RGB(192,192,192);
 if (!marked){ DBEndDraw(table); }
 return TRUE;
 }
 return 0;
};

int DBEndDraw(int table){
TTable*t=GetCursor(table);
if (t){
 if (t->wasbegin){
//  CHKERROR(DbiSetToBookMark (t->hCursor,(t->mark))); //
//  delete t->mark;                                    //
  t->wasbegin=0;
  t->mark=0;
  t->lastok=0;
  t->GetRelative(t->posbefore-t->position);
//  t->position=t->posbefore;                          //
  }
 }
 return 0;
};

int DBBeginDraw(int table){
TTable*t=GetCursor(table);
if (t){
 if(t->mark){
  MessageBox(mainwindow->HWindow,"","",0);
 }
// t->mark=new BYTE[t->curProps.iBookMarkSize];
// CHKERROR(DbiGetBookMark(t->hCursor,(t->mark)));
 t->wasbegin=1;
 t->posbefore=t->position;
 t->last=0;
 t->lastok=0;
 return TRUE;
 }
 return 0;
};

int DBSetFlushMode(int table,int fm){
TTable*t=GetCursor(table);
if(t){
 int _fm=t->autoflush;
 t->autoflush=fm;
 return _fm;
}
return 0;
};
int DBZap(int table){
TTable*t=GetCursor(table);
if (t){
 CHKERROR(DbiEmptyTable (t->base->hDb,t->hCursor,NULL,NULL))==DBIERR_NONE;
 t->NotifyWindows(U_ALL,t->position,0);
 return TRUE;
}return 1;
}
int DBCreateTable(int base,char *name,char * driver,char *desc,int Overwrite){
#define _error _Message("Syntax Error in create table");goto m1;
  TBase *_base=GetBase(base);
  if (_base && name && desc){
  CRTblDesc table;
	memset(&table,0,sizeof(table));
	lstrcpy(table.szTblName,name);   // Table name
	lstrcpy(table.szTblType,driver); // Table Type
	char *ps=desc;
	int fcount=1;
	while (*ps){if (*ps==',')fcount++;ps++;}
	if (fcount*sizeof(FLDDesc)>32768l)return 0;

	ps=desc;
	int state=0;
	table.pfldDesc=new FLDDesc[fcount];
	memset(table.pfldDesc,0,fcount*sizeof(FLDDesc));
	char fname[64]="";
	char ftype[64]="";
	int  index=0;
	int  fsize=0;
	int  fsize1=0;

	while (TRUE){
	 char c=*ps;
	 AnsiUpperBuff(&c,1);
	 switch (state){
	  case 0:if (c==' ')break;else state=1;
	  case 1:if ((c>='A' && c<='Z' || c>='1' && c<='9')
							 || c=='_' && index<64){fname[index]=c;index++;break;}
				if (c==' '){state=19;index=0;*ftype=0;break;}
				_error;
	  case 19:if (c==' ')break;else state=20;

// type Если ( то 29 иначе 90 или 100
	  case 20:if ((c>='A' && c<='Z' || c>='1' && c<='9')
							 || c=='_' && index<64){ftype[index]=c;index++;break;}
				if (c==' '){state=90;break;}
				if (c==','|| c==0){state=100;break;}

				if (c=='('){state=29;fsize=0;fsize1=0;break;}
				_error;
// ('XXX'

	  case 29:if (c==' ')break;else state=30;
	  case 30:if (c>='0' && c<'9'){fsize=fsize*10+(c-'0');
										  if (fsize>4096){_error;}
										  break;}
				if (c==' '){state=35;break;}
				if (c==','){state=39;break;}
				if (c==')'){state=90;break;}
				_error;
// XXX ',' XXX
	  case 35:if (c==' '){break;}
				 if (c==','){state=39;break;}
// 'XXX')
	  case 39:if (c==' ')break;else {state=40;break;}
	  case 40:if (c>='0' && c<'9'){fsize1=fsize1*10+(c-'0');
										  if (fsize1>4096){_error;}
										  break;}
				if (c==' '){state=49;break;}
				if (c==')'){state=90;break;}
				_error;
	  case 49:if (c==' ')break;
				 if (c==')'){state=90;break;}
					_error;
	  case 90:if (c==' ')break;
				 if (c==','||c==0){state=100;}else {_error;}

	  case 100:{
			  INT16 i=table.iFldCount;
			  table.pfldDesc[i].iFldNum=(INT16)(i+1);
			  lstrcpy(table.pfldDesc[i].szName,fname);
			  if (!GetType(ftype,fsize,fsize1,&(table.pfldDesc[i]))){
					_error;
			  }
			  index=0;
			  *fname=0;
			  *ftype=0;
			  table.iFldCount++;
			  state=0;
			  break;
			}
	 }
	if (*ps)ps++;else break;
	}
  CHKERROR(DbiCreateTable(_base->hDb,Overwrite,&table));
m1:
  delete table.pfldDesc;
  return rslt;
  }
 return -1;
};


hDBIDb _GeBase(int h){
TBase*b=GetBase(h);
 if (b)return b->hDb;return NULL;
};
hDBICur  _GetCursor(int h){
TTable*t=GetCursor(h);
 if (t)return t->hCursor;return NULL;
};

TBase*  GetBase(int h)
{
 return (TBase*)bases->Get(h);
};
TTable* GetCursor(int h){
 return (TTable*)tables->Get(h);
};


int  DBGetNextBase(int b){
 C_TYPE i=-1;
 if(b)i=bases->IbyH(b);
 i++;
 pointer p=NULL;
 while (i>=0 && i<bases->count){
   p=bases->At(i);
   if(p)return bases->HbyI(i);
   i++;
 }
 return 0;
};
int DBGetNextTable(int h,int t){
TBase*b= GetBase(h);
 if(b){
  C_TYPE i=0;
  if (t)i=tables->IbyH(t)+1;

 while (i>=0 && i<tables->count){
   TTable*p=(TTable*)tables->At(i);
   if(p && p->base==b)return tables->HbyI(i);
   i++;
 }}
 return 0;
};
BOOL DBGetBaseDesc(int h,char*s){
 TBase*b = GetBase(h);
 if(b){
  char ss1[1024];
  char ss2[1024];
  UINT16      iLen;
   CHKERROR(DbiGetProp(b->hDb,dbDATABASENAME , (pVOID)ss1, sizeof(ss1), &iLen));
   CHKERROR(DbiGetProp(b->hDb,dbDATABASETYPE , (pVOID)ss2, sizeof(ss2), &iLen));
 wsprintf(s,"%d %s (%s)",h,ss1,ss2);
 return TRUE;
 }
 *s=0;
 return 0;
}
BOOL DBGetTableDesc(int h,char*s){
 TTable*t=GetCursor(h);
 if(t){

  wsprintf(s,"%d %s %s",h,t->curProps.szName,t->curProps.szTableType);

  return TRUE;
 }
 return FALSE;
};


TBase::BeginTran(eXILType eXIL){
 if (hXact)EndTran(xendABORT);
 return CHKERROR(DbiBeginTran (hDb, eXIL,&hXact));
};
TBase::EndTran(eXEnd eEnd){
if (hXact){
  CHKERROR(DbiEndTran (hDb,hXact, eEnd));
  hXact=0;
  return rslt;
 }return 0;
};
TBase::TBase(hDBIDb _hDb){
 memset(this,0,sizeof(TBase));
 project=GetCurrentProject();
 hDb=_hDb;
};
TBase::~TBase(){
 if (hDb != 0) {DbiCloseDatabase(&hDb);}
};
int __InsertBDEObj(HWND tree,int type,DWORD parent){

  TreeItemData td;
  td.parent = parent;
  td.IsImage = 0;
 switch(type){
  case TREE_DBE:{
	for(C_TYPE i=0;i<bases->count;i++){
	  TBase*b=(TBase*)bases->At(i);
	  if (b){
	  td.data =(DWORD)b;
	  char s[128];
	  wsprintf(s,"BASE (#%d) BDE handle: 0x%lX",bases->HbyI(i),b->hDb);
	  td.text=s;
	  td.IsLeaf  = TRUE;
	  for(int j=0;j<tables->count;j++){
		TTable *t=(TTable *)tables->At(j);
		if (t&& t->base==b){ td.IsLeaf = FALSE;break; }

	  }
	  td.datatype=TREE_BASE;
	  ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
	 }}
	}break;
  case TREE_BASE:{
	  TBase*b=(TBase*)parent;
	  td.IsLeaf  = TRUE;
	  for(C_TYPE j=0;j<tables->count;j++){
		TTable *t=(TTable *)tables->At(j);
		if (t&& t->base==b){
		 td.datatype=TREE_TABLE;
		 td.data =(DWORD)t;
		 td.IsImage = TABLE_IMG;
		 td.IsLeaf  = TRUE;
		char s[128];
		 wsprintf(s,"%s table: %s #%d  (%lx)",t->curProps.szTableType,t->curProps.szName,tables->HbyI(j),t->hCursor);
		 td.text=s;
		 ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
		}
	  }
  }break;

 }
return 0;
};
int  DBhandleByPointer(UINT32 _d){
   for(C_TYPE j=0;j<tables->count;j++){
	 UINT32 d=(UINT32)tables->At(j);
    if(d==_d)return tables->HbyI(j);
    }
    return 0;
};

/*.............................Writed by AK{.............................*/
int TTable::CloseIndex(char  * name){
int er=CHKERROR(DbiCloseIndex(hCursor, name, NULL/*Currently not used in BDE*/));
return er;};

int TTable::OpenIndex(char * name, UINT16 IndexId)
{
  char s[260];
  GetFullFileNameEx((TProject*)base->project,name,s);
int er=CHKERROR(DbiOpenIndex( hCursor,s, IndexId));
return er;};

int TTable::DeleteIndex(char * name, UINT16 IndexId, char * tag)
{
  char s[260];
  GetFullFileNameEx((TProject*)base->project,name,s);
int er=CHKERROR(DbiDeleteIndex(base->hDb, hCursor, curProps.szName, curProps.szTableType,s, tag, IndexId));
return er;};

int TTable::AddIndex(char * name, UINT16 IndexId, char * tag, UINT16 Props, char ** Fields,int count, UINT16 keytype, char* keyexpr,char* keycond)
{
  char s[260];
  GetFullFileNameEx((TProject*)base->project,name,s);

//MessageBox(GetFocus(),name,"Okay?",MB_ICONEXCLAMATION|MB_OK);
	DBIKEY flds;
	setmem(&flds,sizeof(DBIKEY),NULL);
	int i;
	for (i=0; i<count; i++)
	{
	 flds[i]=GetFId(Fields[i]);
	}

	IDXDesc idx={
	{NULL},  // Name
	IndexId,// Number
	{NULL},   // Tag Name ( for dBase only)
	{NULL}, // Optional Format
	(Props&IDX_PRIMARY)!=0,   // Primary?
	(Props&IDX_UNIQUE)!=0,   // Unique?
	(Props&IDX_DESCENDING)!=0,  // Descending?
	(Props&IDX_MAINTAINED)!=0,   // Maintained?
	(lstrlen(keycond))>0,  // SubSet?
	(lstrlen(keyexpr))>0,  // Expression index?
	NULL,   // for QBE only
	count,      // Fields in key
	1/*???*/,      // Length of key in bytes(?)
	(Props&IDX_IDXOOD)!=0,  // Index out of date?
	keytype,      // Key Type of Expression
	{NULL},    // Array of field numbers
	{0},    // Key expression
	{0},    // Key Condition
	(Props&IDX_CASEINSENS)!=0,  // Case insensitive
	0,      // Block size in bytes
	0};     // Restructure number

	lstrcpy(idx.szName,s);
	lstrcpy(idx.szTagName,tag);
	lstrcpy(idx.szKeyExp,keyexpr);
	lstrcpy(idx.szKeyCond,keycond);
	memcpy(idx.aiKeyFld,flds,sizeof(DBIKEY));

int er=CHKERROR(DbiAddIndex(base->hDb, hCursor, curProps.szName, curProps.szTableType, &idx, NULL));
return er==0;
};

int TTable::SwitchToIndex(char  * name, UINT16 IndexId, char *tag, BOOL bCurrRec)
{
//  if (lstrlen(name)>127) lstrcpyn(name, name, 127);
  char s[260];
  GetFullFileNameEx((TProject*)base->project,name,s);

  if(CHKERROR(DbiSwitchToIndex (&hCursor,s, tag, IndexId, bCurrRec))==DBIERR_NONE){
	 posvalid=FALSE;
    if ((name && *name)||(tag && *tag)){indexactive=1;}else {indexactive=1;}
	 if(bCurrRec)CHKERROR(DbiGetRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
	 else GetTop();
	 NotifyWindows(U_ALL,0,0);
  }
  return rslt=DBIERR_NONE;
};

int TTable::RegenIndex(char* name,char* tag, UINT16 IndexId){
  char s[260];
  GetFullFileNameEx((TProject*)base->project,name,s);
 int er=CHKERROR(DbiRegenIndex(base->hDb, hCursor, curProps.szName, curProps.szTableType,s, tag, IndexId));
 return er==0;
};

BOOL TTable::SetToKey(char *key){
 //	 int i=id-1;
//  _SetText(id,pRecBuf,key);

//  pBYTE    pTmpRecBuf=new BYTE[curProps.iRecBufSize];
//  RECProps RecProp;

//  CHKERROR(DbiInitRecord(hCursor, pTmpRecBuf));
//  _SetText(id,pTmpRecBuf,key);
  CHKERROR(DbiSetToKey(hCursor, keySEARCHEQ,TRUE, 0, 0,key));
  Flush();
  CHKERROR(DbiGetNextRecord(hCursor, dbiNOLOCK,pRecBuf,&recProps));
  //if (!rslt && posvalid)position++;else
  posvalid=0;
  return (rslt==DBIERR_NONE);
  return 0;
};

int DBSetToKey(int table,char*key){
int er;
TTable*t=GetCursor(table);
if (t){
 er=t->SetToKey(key);
}else er=0;
return er;

};

BOOL TTable::SetDeleteMode(BOOL mode){
 if(curProps.bSoftDeletes){
   if(curProps.bDeletedOn!=mode){
	if(CHKERROR(DbiSetProp (hCursor,curSOFTDELETEON,mode))==DBIERR_NONE)
	 curProps.bDeletedOn=mode;
	 NotifyWindows(U_ALL,0,0);
   }
   return curProps.bDeletedOn;
 }
 return 0;
};

BOOL TTable::ReadBlob(int id,TStream*st){
  if (id>0 && id<=curProps.iFields){
   char      *BlobInfo;  // Holds Blob information
   UINT32    BlobSize; // Input / Output Blob size in Bytes

   rslt = CHKERROR(DbiOpenBlob(hCursor,pRecBuf,id, dbiREADONLY));
   if (rslt != DBIERR_NONE) return 0;

   rslt = CHKERROR(DbiGetBlobSize(hCursor,pRecBuf,id, &BlobSize));
   if (rslt != DBIERR_NONE) return 0;

	HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE,BlobSize);
	BlobInfo =(char*) GlobalLock(hglb);

   rslt = CHKERROR(DbiGetBlob(hCursor,pRecBuf,id, 0, BlobSize,
					 (pBYTE)BlobInfo, &BlobSize));
#ifdef WIN32
   st->Write(BlobInfo,BlobSize);
#else
   st->Write((void __huge *)BlobInfo,BlobSize);
#endif
   GlobalUnlock(hglb); GlobalFree(hglb);

   rslt = CHKERROR(DbiFreeBlob(hCursor,pRecBuf,id));

   return rslt==DBIERR_NONE;
  }
  return  FALSE;
};

BOOL TTable::WriteBlob(int id,TStream*st){
  if (id>0 && id<=curProps.iFields){
   char      *BlobInfo;  // Holds Blob information
   UINT32    BlobSize; // Input / Output Blob size in Bytes

   BlobSize=st->GetSize();
   st->Seek(0);
   rslt = CHKERROR(DbiOpenBlob(hCursor,pRecBuf,id, dbiREADONLY));
   if (rslt != DBIERR_NONE) return 0;

	HGLOBAL hglb = GlobalAlloc(GMEM_MOVEABLE,BlobSize);
	BlobInfo = (char*)GlobalLock(hglb);
#ifdef WIN32
   st->Read(BlobInfo,BlobSize);
#else
   st->Read((void __huge *)BlobInfo,BlobSize);
#endif

   rslt = CHKERROR(DbiPutBlob(hCursor,pRecBuf,id, 0, BlobSize,
					 (pBYTE)BlobInfo));

   GlobalUnlock(hglb); GlobalFree(hglb);
   CHKERROR(DbiTruncateBlob (hCursor, pRecBuf, id, BlobSize));
   if (um!=UM_INSERT && um!=UM_APPEND){um=UM_MODIFY; }

   rslt = CHKERROR(DbiFreeBlob(hCursor,pRecBuf,id));

   return rslt==DBIERR_NONE;
  }
  return  FALSE;
};
BOOL TTable::FreeBlob(int id){
   if (id>0 && id<=curProps.iFields){
   rslt = CHKERROR(DbiOpenBlob(hCursor,pRecBuf,id, dbiREADONLY));
   if (rslt != DBIERR_NONE) return 0;
	CHKERROR(DbiTruncateBlob (hCursor, pRecBuf, id,0));
	rslt =CHKERROR(DbiFreeBlob(hCursor,pRecBuf,id));
    if (um!=UM_INSERT && um!=UM_APPEND){um=UM_MODIFY; }
   }
}
int DBReadBlob(int table,int id,int stream){
 TTable*t=GetCursor(table);
 if(t){
	TStream * st=streams->GetStream(stream);
	if(st){
	   return t->ReadBlob(id,st);
	}
  }
 return 0;
};

int DBWriteBlob(int table,int id,int stream){
 TTable*t=GetCursor(table);
 if(t){
	TStream * st=streams->GetStream(stream);
	if(st){
	   return t->WriteBlob(id,st);
	}
  }
 return 0;
};
int DBFreeBlob(int table,int id){
TTable*t=GetCursor(table);
 if(t){
  t->FreeBlob(id);
  return TRUE;
 }
return 0;
};

/*.............................Writed by AK}.............................*/


/*.............................Writed by AK.............................*/



int DBAddIndex(int table, char * name, UINT16 IndexId, char * tag, UINT16 Props, char ** Fields,int count, UINT16 keytype, char* keyexpr,char* keycond)
{
TTable*t=GetCursor(table);
if (t){
return (t->AddIndex(name, IndexId, tag, Props, Fields,count, keytype, keyexpr, keycond));
}
return FALSE;
};

int DBDeleteIndex(int table, char * name, UINT16 IndexId, char * tag)
{
 int er;
 TTable*t=GetCursor(table);
 if (t){
  er=t->DeleteIndex(name, IndexId, tag);
 }else er=-1;
 return er==DBIERR_NONE;
};

int DBOpenIndex(int table,char * name, UINT16 IndexId)
{
 int er;
 TTable*t=GetCursor(table);
 if (t){
  er=t->OpenIndex(name, IndexId);
  }else er=-1;
 return er==DBIERR_NONE;
};

int DBSwitchToIndex(int table, char * name, UINT16 IndexId, char *tag, BOOL bCurrRec)
{
int er;
TTable*t=GetCursor(table);
if (t){
er=t->SwitchToIndex(name, IndexId, tag, bCurrRec);
}else er=-1;
return er==DBIERR_NONE;};

int DBCloseIndex(int table, char * name){
int er;
TTable*t=GetCursor(table);
if (t){
er=t->CloseIndex(name);
}else er=-1;
return er==DBIERR_NONE;};

int DBRegenIndex(int table,char  * name,char *tag, UINT16 IndexId){
int er;
TTable*t=GetCursor(table);
if (t){
er=t->RegenIndex(name, tag, IndexId);
}else er=-1;
return er==DBIERR_NONE;};

int DBAddFilter (int table, char* fExpr){// Writed by AK
TTable*t=GetCursor(table);
if (t){
//return t->AddFilter(fExpr);
}
return 0;};
/*
#include "dialogs.rh"
#include <stratum\sccall.h>
#include "browser.h"
#include "systab.h"
*/
#define __MINMAX_DEFINED
#include "_dialogs.h"
#include "winman.h"

BOOL scCALLBACK TableMainProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
switch(msg){
   case WM_COMMAND:{
 	  TPropertyButton*b=GetDlgPropertyButton(hwnd);
    if(b){
	 int table=(int)b->data;
	 TTable*t=GetCursor(table);
    EV_COMMANDsimply
    switch(command){
     case IDC_BROWSE:if(!t->browsers){
                      winmanager->OpenBrowseWindow("@BROWSE@def",table,"");
                      if(t->browsers)EnableWindow(GetDlgItem(hwnd,IDC_BROWSE),0);
                    }break;

    }} };break;
	case PSP_INITPAGE:{
	 TPropertyButton*b=(TPropertyButton*)lParam;
	 int table=(int)b->data;
	 TTable*t=GetCursor(table);
		   SetDlgItemText(hwnd,IDC_BASENAME,t->curProps.szName);
		   SetDlgItemText(hwnd,IDC_BASETYPE,t->curProps.szTableType);
		   SetDlgItemInt(hwnd,IDC_BASERECORDS,DBGetCount(table),0);
		   char s[1024];
		   UINT16 size;
         if(t){
		   CHKERROR(DbiGetProp(t->hCursor,curFILENAME, (pVOID)s, sizeof(s), &size));
		   SetDlgItemText(hwnd,IDC_BASEFILE,s);
		   SetDlgItemText(hwnd,IDC_LANGDRV,t->curProps.szLangDriver);
         if(t->browsers)ShowWindow(GetDlgItem(hwnd,IDC_BROWSE),SW_HIDE);
         }
	}return TRUE;
 }
  return DefPropertyItemProc(hwnd,msg,wParam,lParam);
};
BOOL scCALLBACK TableRecProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
switch(msg){
	case TBL_GETTEXT:{
	 TLABELINFO* info=(TLABELINFO*)lParam;
	  if (info && info->id==IDC_HLABEL && info->pos>=0 && info->pos<4){
	  static char * fields[]={
	 "Name","Type/sutype","Size","Rights"
//	 "Имя","Тип","Размер","Права"
	 };
	  lstrcpy(info->text,fields[(int)info->pos]);
	  return TRUE;
	  }
	 if (info->id==IDC_VLABEL){
	   sprintf(info->text,"%d",(int)info->pos+1);
	 }

	};break;
   case TBN_GETTEXT:{
	  CELLINFO* ci=(CELLINFO*)lParam;
	  TPropertyButton*b=GetDlgPropertyButton(hwnd);
	 TTable*t=(TTable*)b->user;
	 if(t && ci->x<4 && ci->y<t->curProps.iFields && ci->x>=0 && ci->y>=0){
	 ci->text[0]=0;
	  switch(ci->x){
	   case 0:
			 lstrcpy(ci->text,t->pFldArray[ci->y].szName);
			 break;
	   case 1:{
			  for(int j=0;j<sizeof(TypesName)/sizeof(TypesName[0]);j++){
				  if(TypesName[j].type==t->pFldArray[ci->y].iFldType){lstrcpy(ci->text,TypesName[j].name);
				  for(j=0;j<sizeof(SubName)/sizeof(SubName[0]);j++){
					 if(SubName[j].type==t->pFldArray[ci->y].iSubType ){
				  lstrcat(ci->text,",");lstrcat(ci->text,SubName[j].name);
				  return TRUE;
				  }return TRUE;
			  }
			   };
				}}break;
	   case 2:
			   sprintf(ci->text,"%d",t->pFldArray[ci->y].iUnits1);
			   if(t->pFldArray[ci->y].iUnits2){
			   sprintf(ci->text+lstrlen(ci->text),",%d",t->pFldArray[ci->y].iUnits2);
			   }
			  break;
	   case 3:
	   switch(t->pFldArray[ci->y].efldrRights){
		case fldrREADWRITE:lstrcpy(ci->text,"readwrite");break;
		case fldrREADONLY:lstrcpy(ci->text,"readonly");break;
		case fldrNONE:    lstrcpy(ci->text,"none");break;
		case fldrUNKNOWN: lstrcpy(ci->text,"unknown");break;
	   break;
	  }
	 }
   }};return TRUE;
	case PSP_INITPAGE:{
	HWND hl=GetDlgItem(hwnd,IDC_HLABEL);
	HWND vl=GetDlgItem(hwnd,IDC_VLABEL);
	 HWND br=GetDlgItem(hwnd,IDC_BASEFIELDSDESC);
	  SendMessage(br,TB_SETLABEL,(WPARAM)hl,(LPARAM)vl);

	   TPropertyButton*b=(TPropertyButton*)lParam;
	  TTable*t=GetCursor(b->data);
	  if(!t)return 0;
	  b->user=t;
	  SetDlgItemInt(hwnd,IDC_BASERECSIZE,t->curProps.iRecSize,0);
		SetDlgItemInt(hwnd,IDC_BASEFIELDS,t->curProps.iFields ,0);

	  SendMessage(br,TB_SETRANGEY,0,t->curProps.iFields);
	  TCollumn c[4]={TCollumn(0,80,CF_READONLY),TCollumn(0,65,CF_READONLY)
						  ,TCollumn(0,55,CF_READONLY),TCollumn(0,65,CF_READONLY)};

		  for(int i=0;i<sizeof(c)/sizeof(TCollumn);i++)
		 SendMessage(br,TB_ADDCOLLUMN,-1,LPARAM(c+i));

   }return TRUE;
}
  return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}
BOOL scCALLBACK TableIdxProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
switch(msg){
	case PSP_INITPAGE:{
	TPropertyButton*b=(TPropertyButton*)lParam;
	TTable*t=GetCursor(b->data);
	if(!t)return 0;
	hDBICur  hCur;
   BYTE mode=(BYTE)b->dwuser1;
   if(mode){
    CHKERROR(DbiOpenFieldList(t->base->hDb,t->curProps.szName, NULL, FALSE,&hCur));
    SetDlgItemInt(hwnd,IDC_BASERECSIZE,t->curProps.iRecSize,0);
    }
    else
	CHKERROR(DbiOpenIndexList(t->base->hDb,t->curProps.szName,NULL,&hCur));

	if (hCur){
	TTable * _t=new TTable(hCur,t->base);
	 if (rslt == DBIERR_NONE){
	  b->dwuser1=tables->Insert(_t);
	HWND idx=GetDlgItem(hwnd,IDC_BASEIDXDESC);
	TCursorWindow *cw=(TCursorWindow *)GetWindowLong(idx,0);
   char * _ps=NULL;
   if(!mode)_ps="Name:100,IndexId:50,TagName:60,Primary:60,UNIQUE:50,DESCENDING:80,CASEINSENSITIVE:90";
	if(cw)cw->SetTable(b->dwuser1,_ps);
	  }else delete t;
	}
   }return TRUE;
   case PSP_CLOSE:{
	  TPropertyButton*b=(TPropertyButton*)lParam;
	 if(b->flags&PSP_INITIALIZE){
	  if(b->dwuser1)DBCloseTable(b->dwuser1);
	  }
   };return TRUE;

}
  return DefPropertyItemProc(hwnd,msg,wParam,lParam);
}

// Новые функции
int DBSys(int i,int j){
if(!sqlloaded)DBInitSQL();
 switch(i){
 case 1:{ hDBICur  hCur=0;
         TBase *base=j?GetBase(j):NULL;
          if(base){
          CHKERROR(DbiOpenLdList(&hCur));
          if (hCur){
           TTable * _t=new TTable(hCur,base);
            if (rslt == DBIERR_NONE){
           return tables->Insert(_t);
           }else delete _t;
           }}
        }break;
 }
return 0;
}


UINT16  DBSetCodePage(int table,UINT16 page){
 TTable*t=GetCursor(table);
 if(t){
    if(page){
      switch(page){
       case 0:t->ppLdObj=0;break;
       case 1:CHKERROR(DbiGetLdObj (t->hCursor,&(t->ppLdObj)));break;
       case 866:CHKERROR(DbiSetProp(t->hCursor,curLANGDRVNAME, (UINT32)"db866ru0"));break;
      }
    }
  }
  return 0;
};

UINT16 DBGetCodePage(int table){
 TTable*t=GetCursor(table);
 if(t){
   LDDesc *ld=(LDDesc *)t->ppLdObj;

   if(ld){
     UINT16 *cp=(UINT16 *)(((char*)t->ppLdObj)+78); //???
     return *cp;
   }//return ld->iCodePage;
   /*
   if(CHKERROR(DbiGetLdObj(t->hCursor,(void**)&ld))==DBIERR_NONE){
	return ld->iCodePage;
   }
   */
 }
 return FALSE;
};

BOOL DBSetDeleteMode(int table,BOOL mode){
 TTable*t=GetCursor(table);
 if(t)return t->SetDeleteMode(mode);
};

BOOL DBGetDeleteMode(int table){
 TTable*t=GetCursor(table);
 if(t && t->curProps.bSoftDeletes)return t->curProps.bDeletedOn;
 return FALSE;
};
BOOL DBUndeleteRecord(int table){
 TTable*t=GetCursor(table);
 if(t)return t->UnDeleteRecord();
 return FALSE;
};

BOOL DBLock(int table,BYTE m){
 TTable*t=GetCursor(table);
 if(t)return (CHKERROR(DbiAcqTableLock(t->hCursor,m?dbiWRITELOCK:dbiREADLOCK))==DBIERR_NONE);
 return FALSE;
}
BOOL DBUnLock(int table,BYTE m){
 TTable*t=GetCursor(table);
 if(t){
  BOOL m1=m>=2;
  DBILockType eLockType=m?dbiWRITELOCK:dbiREADLOCK;
  return(CHKERROR(DbiRelTableLock(t->hCursor,m1,eLockType))==DBIERR_NONE);
 }
 return FALSE;
}
BYTE DBIsFieldReadOnly(int table,int id){
 TTable*t=GetCursor(table);
 if(t){
 if (id>0 && id<=t->curProps.iFields)
   {
     int i=id-1;
     if(t->pFldArray[i].efldrRights==fldrREADWRITE)
        {
          if(t->pFldArray[i].iFldType==fldBLOB)return 2;
          return 0;
        }
     return 1;
   }
  }
 return 0xff;
};
BOOL DBCopyTo(int table,char *file,char *type){
   TTable*t=GetCursor(table);
   if(!t)return FALSE;

   if(!lstrcmpi(type,"TEXT") )
    {
   t->GetTop();
   TDOSStream st(file,TDOSStream::stCreate);
   char s[256];
   if(st.status==0)
    {
     for(int i=0;i<t->curProps.iFields;i++)
      {
       DBFieldName(table,i+1,s);
         if(i<t->curProps.iFields-1)lstrcat(s,"\t");else lstrcat(s,"\r\n");
         st.Write(s,lstrlen(s));
      }

     do{
        for(int i=0;i<t->curProps.iFields;i++)
        {
         t->GetText(i,s);
         if(i<t->curProps.iFields-1)lstrcat(s,"\t");else lstrcat(s,"\r\n");
         st.Write(s,lstrlen(s));
        }
       t->GetRelative(1);
      }while(rslt==0);
    }
   t->GetTop();
   t->posvalid=0;
    return TRUE;
    }

   CURProps     CurProps;
   BATTblDesc   TblDesc;
   memset(&TblDesc,0,sizeof(TblDesc));
   // Get source cursor properties
   if(CHKERROR(DbiGetCursorProps(t->hCursor, &CurProps))!=DBIERR_NONE)return FALSE;

   TblDesc.hDb = t->base->hDb;
   lstrcpy(TblDesc.szTblName, file );
   lstrcpy(TblDesc.szTblType, type );

  // Delete table if it exists
   rslt = DbiDeleteTable(t->base->hDb,TblDesc.szTblName,CharNULL(TblDesc.szTblType));
   if ((rslt != DBIERR_NOSUCHFILE) && (rslt != DBIERR_NOSUCHTABLE))
      CHKERROR(rslt);
   t->GetTop();

   // Copy the information from the dBASE table to the Paradox table
   BOOL rez=(CHKERROR(DbiBatchMove(NULL,t->hCursor, &TblDesc, NULL, batCOPY,
					  0, 0, NULL, NULL, NULL,
					  NULL/* "KEYVIOL"*/,NULL/* "PROBLEMS"*/,NULL /*"CHANGED"*/,
					  NULL, NULL, NULL, FALSE, FALSE, NULL, FALSE))==DBIERR_NONE);
   t->GetTop();
   t->posvalid=0;

   return rez;
};
#endif
