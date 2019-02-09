/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			collect.h
      collect.cpp
Author:
			Alexander Shelemekhov
*/

#ifndef _collection_h
#define _collection_h
#define _fast_non_check
const max_size=65520u/2;
#include <stratum\sccall.h>
#include <stratum\sc_type.h>

// Collection
class _SCEXPCLASS TCollection
{
public:
	C_TYPE count,delta,limit;
	pointer * items;

	TCollection();
	TCollection(C_TYPE asize,C_TYPE adelta);
	~TCollection();

	pointer At(register index);
	#ifndef _fast_non_check
	#define _At(index) At(index)
	#else
	inline pointer 	_At(register index){return items[index];};
	#endif
	void 					AtDelete(C_TYPE index);
	void 					AtFree(C_TYPE index);
	void 					AtInsert(C_TYPE index,pointer item);
	void 					AtPut(C_TYPE index,pointer item);
	void 					Delete(pointer item);
	void 					DeleteAll(){count=0;};
	void 					FreeAll();
	void 					Free(pointer item);
	virtual C_TYPE  IndexOf(pointer item);
	virtual C_TYPE 	Insert(pointer item);
	void 					Pack();
	void 					SetLimit(C_TYPE);
	void 					Swap(C_TYPE i,C_TYPE j);
	BOOL          IsIndexValid(register Index);
	virtual void 	FreeItem(pointer item);
};

// Sorted Collection
class _SCEXPCLASS TSortedCollection:public TCollection
{
public:
	unsigned char Duplicates;
 	TSortedCollection::TSortedCollection(C_TYPE asize,C_TYPE adelta);
  virtual C_TYPE    Insert(pointer item);
	virtual C_TYPE    IndexOf(pointer item);
	virtual BOOL      Search(pointer key,C_TYPE &index);
	virtual int       Compare(pointer key1,pointer key2);
};


class _SCEXPCLASS TStringCollection:public TSortedCollection
{
public:
  TStringCollection(WORD asize,WORD adelta);
	virtual int Compare(pointer key1,pointer key2);
};

class CTag
{
public:
	char	*name,			 //имя тега
  			*value,      //значение тега
        *fullvalue;  //исходная строка тега

  CTag(char* _fullvalue);
  void Free();
  bool IsValid();
};
typedef CTag* PTag;


class CTagsCollection : public TCollection
{
public:
	CTagsCollection(char* tags,C_TYPE asize=10,C_TYPE adelta=5);
  void FreeItem(pointer item);
	void Add(char* tags);

	PTag SearchTag(char* tagname);
  //получить значение
  char* GetTagValue(char* tagname);
  char* GetTagFullValue(char* tagname);
};
typedef CTagsCollection* PTagsCollection;


void SDKPROC DeleteCollection(TCollection* p);
#endif // _collection_h


