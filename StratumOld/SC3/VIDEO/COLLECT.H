/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			collect.h
Author:
			Alexander Shelemekhov
*/

#ifndef _collection_h
#define _collection_h
#define _fast_non_check

//#include "errors.h"
const max_size=65520u /4;
typedef void*pointer;
typedef INT16 C_TYPE;
//*----------------------------------------------------*

class TCollection{
public:
C_TYPE count,delta,limit;
pointer * items;
		TCollection()
			 {
				 items=NULL;count=0;limit=0;delta=0;
			 }
  TCollection(WORD asize,WORD adelta);
//  TCollection(PStream);
 ~TCollection();
pointer           At(register index);
#ifndef _fast_non_check
#define _At(index) At(index)
#else
inline pointer 	_At(register index){return items[index];};
#endif
void 					AtDelete(C_TYPE index);
void 					AtFree(C_TYPE index)
			  { FreeItem(At(index));
				 AtDelete(index);
			  };
void 					AtInsert(C_TYPE index,pointer item);
void 					AtPut(C_TYPE index,pointer item);
void 					Delete(pointer item)
			 {C_TYPE index=IndexOf(item);
			  if (index>-1) AtDelete(index); };
void 		 DeleteAll(){count=0;};
void 		 FreeAll();
void 		 Free(pointer item)
			 {
				 Delete(item);
				 FreeItem(item);
			 };
virtual C_TYPE 		IndexOf(pointer item);
virtual C_TYPE 		Insert(pointer item)
			 {
				 AtInsert(count,item);
				 return (INT16)(count-1);
			 };
void 					Pack();
void 					SetLimit(C_TYPE );
void 					Swap(C_TYPE i,C_TYPE j);
inline BOOL        IsIndexValid(register Index);
virtual void 		FreeItem(pointer );
};

void DeleteCollection(TCollection* p);
#endif // _collection_h
