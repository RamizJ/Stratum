#pragma once
#include "_vmach.h"

#ifdef TEXTANALYSERSQL
	#define DLL_CALLCONV _cdecl
#else
	#define DLL_CALLCONV _stdcall
#endif

void InitTextAnalyserCommands();

class _SCEXPCLASS CTextAnalyser
{
protected:
  HMODULE Base; //������ �� ���������� Base.dll
  HMODULE Analyse; //������ �� ���������� Analyse.dll
  HMODULE Morph; //������ �� ���������� morph.dll

  bool BaseWasLoaded;
  bool AnalyseWasLoaded;
  bool MorphWasLoaded;

  bool InitBase(char* baseName=0, char* user=0, char* pass=0); //��������� ���������� Base.dll � ������� �� ���
  bool InitAnalyse(); //��������� ���������� Analyse.dll � ������� �� ���
  bool InitMorph(); //��������� ���������� Analyse.dll � ������� �� ���
  char* GetLog(); //��������� ���������� Base.dll � ������� �� ���

  //������� �� ���������� Base
  LONG DLL_CALLCONV (*fGetLog)(char* var); //������ �� ������� GetLog
  //LONG DLL_CALLCONV (*InitDict)(char* Path); //������ �� ������� Init
#ifdef TEXTANALYSERSQL
  LONG DLL_CALLCONV (*InitDict)(char* baseName, char* user, char* pass); //������ �� ������� Init
#else
  LONG DLL_CALLCONV (*InitDict)(char* dictpath); //������ �� ������� Init
#endif
  LONG DLL_CALLCONV (*fGetWordInfo)(char* Word, char* Info); //������ �� ������� GetWordInfo
  LONG DLL_CALLCONV (*fAnalyse)(char* Words, char* Sentence); //������ �� ������� Analyse
  LONG DLL_CALLCONV (*fGetWordProperty)(char* Word, char* Part, char* Name, char* Value);//������ �� ������� GetWordProperty
  LONG DLL_CALLCONV (*fGetWordForm) (char* Word, int Num, char* Properties, char* Value);//������ �� ������� GetWordForm
  LONG DLL_CALLCONV (*fMorphDivide ) (char* Word, char* Result, int* ResultSize);//������ �� ������� MorphDivide
  LONG DLL_CALLCONV (*fSetMorphDivide) (char* Form, char* Divide);

  //������� ��� ������ ���� �� ��������
  LONG DLL_CALLCONV (*fSetSearchCriteria) (char* Criteria);
  LONG DLL_CALLCONV (*fGetSearchResult) (long Index, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fClearSearchCriteria) ();
  LONG DLL_CALLCONV (*fFindNextWord) (char* Criteria, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fFindPrevWord) (char* Criteria, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fAnalyseWord) (char* Word, char* Info);

  //������� �� ���������� Analyse
  LONG DLL_CALLCONV (*InitPlugin)(char* FileName); //������ �� ������� Init �� ���������� Analyse.dll
  LONG DLL_CALLCONV (*fAnalyseText)(char* Words, char* Question, char* Sentence, char* Answer); //������ �� ������� AnaliseText
  LONG DLL_CALLCONV (*fGetWordPropertyInSent)(char* Sentence, char* Word, int Pos, char* Name, char* Value); //������ �� ������� GetWordPropertyInSent
  LONG DLL_CALLCONV (*fGetWordInSentByRole)(char* Sentence, char* Role, char* Value); //������ �� ������� GetWordInSentByRole

  //������� �� ���������� Morph
  LONG DLL_CALLCONV (*fInitMorph)(char* Root, char* Schema); //������ �� ������� Init �� ���������� Analyse.dll
  LONG DLL_CALLCONV (*fDivide)(char* Word, char* Buffer, int* Size); //������ �� ������� AnaliseText

public:
  CTextAnalyser();
  CTextAnalyser(char* baseName, char* user, char* pass);
  ~CTextAnalyser();

  char* GetWordInfo(char* Word);
  char* AnalyseSentance(char* Words);
  char* GetWordProperty(char* Word, char* Part, char* Name);
  int   GetWordFormCount(char* Word);
  char* GetWordForm(char* Word, int Num, char* Properties);
  char* MorphDivide(char* Word);
  int SetMorphDivide(char* Form, char* Divide);

  char* WordDivide(char* word);

  //����� ���� �� ��������
  // criteria - �������� ������ - �����*,����� ����:���������������,�����:������������,�����:������������
  // result - ������ � ������������ ������ (����������� ������ �������)
  // count - ����������� ��������� ���� (������ ������� result)
  // ����������
  // 0.. - ���������� ��������� ����
  // -1 ���� �� ����������������
  // -2 �� ������ ��������
  // -3 ������ ��� ��������� ���������� �����
  // -4 ������ ��� ������� ����������� ������
  long SearchWords(char* criteria, char** &result, long begin, long end);

  char* FindNextWord(char* criteria);
  char* FindPrevWord(char* criteria);
  char* AnalyseWord(char* word);

  char* GetSentanceTree(char* Sentance);
  char* GetAnswer(char* Sentance, char* Question);
  char* GetWordPropertyInSent(char* Sentence, char* Word, int Pos, char* Name);
  char* GetWordInSentByRole(char* Sentence, char* Role);

  bool IsInited(){
    if(!BaseWasLoaded)InitBase();
    if(!AnalyseWasLoaded)InitAnalyse();
  	return BaseWasLoaded && AnalyseWasLoaded;
  }
};


template<class T>
void LoadFunctionImpl(HMODULE Module,T& a_Function, const char* a_Name);

#define LoadFunction(module, func, name) LoadFunctionImpl(module, func, name);


//������ ����� ������� ��, ����� ������������ ������ 831-840, 891-899

#define VM_GETWORDINFO		831
#define VM_GETANSWER		832
#define VM_GetSentanceTree		833
#define VM_GETWORDPROPERTY		834
#define VM_GetWordPropertyInSent		835
#define VM_GetWordInSentByRole		836
#define VM_GetWordFormCount		837
#define VM_GetWordForm		838
#define VM_SEARCHWORDS		839
#define VM_GETWORDINFOTOSTREAM		840

#define VM_SEARCHWORDS_END		891
#define VM_SEARCHWORDS_BEGIN_END		892
#define VM_FINDNEXTWORD		893
#define VM_FINDPREVWORD		894
#define VM_ANALYSEWORD		895
#define VM_INITANALYZER		896
#define VM_MORPHDIVIDE		897
#define VM_SETMORPHDIVIDE	898
#define VM_WORDDIVIDE	899

//��������� ����� - 898, ����� �� 899
