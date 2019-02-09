/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			collect.cpp
Author:
			Alexander Shelemekhov
*/
#define EM_coindexerror     3000
#define EM_cooverflow       3001
#define EM_hglobal          3002
#include "gwin2d3d\gwintype.h"
#include "video\collect.h"


int _Error(int,int){
return MessageBox(0,"Error","Error in scvideo",MB_TASKMODAL|MB_OK|MB_ICONSTOP);
};
//#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
//#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
//#define FP_OFF( fp )( (unsigned )( fp ))

TCollection::TCollection(WORD asize,WORD adelta)
	{  if (asize)
		{ items = new pointer [asize];
		  int i;
		  for(i=0;i<asize;i++)items[i]=NULL;
		 }else  items=NULL;
	  delta=adelta;limit=asize;count=0;
	};
TCollection::~TCollection(){/*FreeAll();*/delete []items ;};
pointer TCollection::At(register index){ if (IsIndexValid(index))return items[index];
							 else {_Error(EM_coindexerror,index);return NULL;} };
void TCollection::AtDelete(C_TYPE index){
		 if (IsIndexValid(index)){
			for (index=index;index+1<count;index++)items[index]=items[index+1];
			count--;
		 }else _Error(EM_coindexerror,index);
		 };
void TCollection::AtInsert(C_TYPE index,pointer item){
		 if ((index>=0)&&(index<=count)){
			if (count==limit){
			 int l=limit;
			 limit+=delta;
			 if (limit>max_size)limit=max_size;
			 if (l==limit)_Error(EM_cooverflow,0);
			 pointer * _items=new pointer[limit];
			 int i;
			 for(i=0;i<count;i++)_items[i]=items[i];
			 delete [] items;
			 items=_items;
			}
		 int i;   for(i=count;i>index;i--)items[i]=items[i-1];
//       _fmemmove (void far *dest, const void far *src, size_t n)
/*
 int mc=(count-index)*sizeof(pointer);
if(mc)
memmove (MK_FP(FP_SEG(items),FP_OFF(items)+index*4+4),
MK_FP(FP_SEG(items),FP_OFF(items)+index*4),mc);
*/
		 items[index]=item;
		 count++;
		 }else _Error(EM_coindexerror,index);
		};
void TCollection::AtPut(C_TYPE index,pointer item){
		 if (IsIndexValid(index)) items[index]=item;
			else _Error(EM_coindexerror,index);
	  };
void TCollection::FreeAll(){int i;
 for (i=0;i<count;i++)FreeItem(items[i]);
 count=0;
};
C_TYPE TCollection::IndexOf(pointer item){
  C_TYPE i;
  for(i=0;(i<count)&&(items[i]!=item);i++);
  if (i<count) return i;else
  return -1;
};
void TCollection::Swap(C_TYPE i,C_TYPE j)
 {
	 pointer p1,p2;p1=At(i);p2=At(j);items[i]=p2;items[j]=p1;
 };

/*	TCollection::TCollection(PStream ps)
 {
  count=ps->ReadWord();
  limit=ps->ReadWord();
  delta=ps->ReadWord();
  items = new pointer [limit];
  int i;
  if (limit==0)limit=count;
  for(i=0;i<limit;i++)items[i]=NULL;
 };
void TCollection::LoadItems(PStream ps)
 { int i;
	for (i=0;i<count;i++)items[i]=GetItem(ps);
 };
void 		TCollection::Store(PStream ps)
 {
  ps->WriteWord(GetCountToSave());
  ps->WriteWord(limit);
  ps->WriteWord(delta);
  _store(ps);
  int i;
	 for (i=0;i<count;i++)
	  {pointer item=At(i);
	  if ((fullstore)||(IsNeedToSave(item)))PutItem(ps,item);
	  }
 };
int TCollection::IsNeedToSave(pointer){return 1;};
pointer  TCollection::GetItem(PStream ps)
 {
	return ps->Get();
 };
int TCollection::GetCountToSave()
 {if (fullstore)return count;
  int c=0,i;
  for(i=0;i<count;i++)if (IsNeedToSave(At(i)))c++;
  return c;
 };
void     TCollection::PutItem(PStream ps,pointer item)
 {
  ps->Put(item);
 }; */
void 		TCollection::FreeItem(pointer)
 { };
int  TCollection::IsIndexValid(register Index)
 {return ((Index>-1)&&(Index<count));

 }
//*---------------------------------------------------*
//TSorted
/*
TSortedCollection::TSortedCollection(PStream ps):
 TCollection(ps)
 {
  ps->Read(&Duplicates,1);
 };
void TSortedCollection::_store(PStream ps){
  ps->Write(&Duplicates,1);
} */

void DeleteCollection(TCollection*p)
 {if (p){p->FreeAll();
		  p->DeleteAll();
		  delete p;
		  }
 };

