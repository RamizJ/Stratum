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
  HMODULE Base; //ссылка на библиотеку Base.dll
  HMODULE Analyse; //ссылка на библиотеку Analyse.dll
  HMODULE Morph; //ссылка на библиотеку morph.dll

  bool BaseWasLoaded;
  bool AnalyseWasLoaded;
  bool MorphWasLoaded;

  bool InitBase(char* baseName=0, char* user=0, char* pass=0); //загружаем библиотеку Base.dll и функции из нее
  bool InitAnalyse(); //загружаем библиотеку Analyse.dll и функции из нее
  bool InitMorph(); //загружаем библиотеку Analyse.dll и функции из нее
  char* GetLog(); //загружаем библиотеку Base.dll и функции из нее

  //функции из библиотеки Base
  LONG DLL_CALLCONV (*fGetLog)(char* var); //ссылка на функцию GetLog
  //LONG DLL_CALLCONV (*InitDict)(char* Path); //ссылка на функцию Init
#ifdef TEXTANALYSERSQL
  LONG DLL_CALLCONV (*InitDict)(char* baseName, char* user, char* pass); //ссылка на функцию Init
#else
  LONG DLL_CALLCONV (*InitDict)(char* dictpath); //ссылка на функцию Init
#endif
  LONG DLL_CALLCONV (*fGetWordInfo)(char* Word, char* Info); //ссылка на функцию GetWordInfo
  LONG DLL_CALLCONV (*fAnalyse)(char* Words, char* Sentence); //ссылка на функцию Analyse
  LONG DLL_CALLCONV (*fGetWordProperty)(char* Word, char* Part, char* Name, char* Value);//ссылка на функцию GetWordProperty
  LONG DLL_CALLCONV (*fGetWordForm) (char* Word, int Num, char* Properties, char* Value);//ссылка на функцию GetWordForm
  LONG DLL_CALLCONV (*fMorphDivide ) (char* Word, char* Result, int* ResultSize);//ссылка на функцию MorphDivide
  LONG DLL_CALLCONV (*fSetMorphDivide) (char* Form, char* Divide);

  //функции для поиска слов по критерию
  LONG DLL_CALLCONV (*fSetSearchCriteria) (char* Criteria);
  LONG DLL_CALLCONV (*fGetSearchResult) (long Index, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fClearSearchCriteria) ();
  LONG DLL_CALLCONV (*fFindNextWord) (char* Criteria, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fFindPrevWord) (char* Criteria, char* Buffer, int* BufferSize);
  LONG DLL_CALLCONV (*fAnalyseWord) (char* Word, char* Info);

  //функции из библиотеки Analyse
  LONG DLL_CALLCONV (*InitPlugin)(char* FileName); //ссылка на функцию Init из библиотеки Analyse.dll
  LONG DLL_CALLCONV (*fAnalyseText)(char* Words, char* Question, char* Sentence, char* Answer); //ссылка на функцию AnaliseText
  LONG DLL_CALLCONV (*fGetWordPropertyInSent)(char* Sentence, char* Word, int Pos, char* Name, char* Value); //ссылка на функцию GetWordPropertyInSent
  LONG DLL_CALLCONV (*fGetWordInSentByRole)(char* Sentence, char* Role, char* Value); //ссылка на функцию GetWordInSentByRole

  //функции из библиотеки Morph
  LONG DLL_CALLCONV (*fInitMorph)(char* Root, char* Schema); //ссылка на функцию Init из библиотеки Analyse.dll
  LONG DLL_CALLCONV (*fDivide)(char* Word, char* Buffer, int* Size); //ссылка на функцию AnaliseText

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

  //поиск слов по критерию
  // criteria - критерий поиска - “папу*,часть речи:существительное,падеж:именительный,число:единственное”
  // result - массив с результатами поиска (формируется внутри функции)
  // count - колличество найденных слов (размер массива result)
  // Возвращает
  // 0.. - количество найденных слов
  // -1 база не инициализирована
  // -2 не верный критерий
  // -3 ошибка при получении найденного слова
  // -4 ошибка при очистке результатов поиска
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


//список кодов функции ВМ, можно использовать только 831-840, 891-899

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

//последний номер - 898, можно до 899
