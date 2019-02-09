#include "TextAnalyser\TextAnalyser.h"
#include "twindow.h"
#include "dynodata.h"
#include "stream_m.h"

//шаблон функции для загрузки функций из библиотек
template<class T>
void LoadFunctionImpl(HMODULE Module, T& a_Function, const char* a_Name)
{
	a_Function = (T)GetProcAddress(Module, a_Name);
}


CTextAnalyser* TA;

CTextAnalyser::CTextAnalyser()
{
  Base=NULL;
  Analyse=NULL;
  BaseWasLoaded=false;
  AnalyseWasLoaded=false;
  MorphWasLoaded=false;
}
CTextAnalyser::CTextAnalyser(char* baseName, char* user, char* pass)
{
  Base=NULL;
  Analyse=NULL;
  BaseWasLoaded=false;
  AnalyseWasLoaded=false;
  MorphWasLoaded=false;
#ifdef TEXTANALYSERSQL
  InitBase(baseName,user,pass);
  InitAnalyse();
#endif
}

CTextAnalyser::~CTextAnalyser()
{
  FreeLibrary(Base);
  FreeLibrary(Analyse);
}

char* CTextAnalyser::GetLog()
{
  char* result=new char[1];
  if(!fGetLog)return result;
  long r = fGetLog(NULL);
  wsprintf(result,"%d",r);
  if(r > 0)
  {
    char* result1 = new char[r+1];
    fGetLog(result1);
    return result1;
  }
  else
    return result;
}

bool CTextAnalyser::InitBase(char* baseName, char* user, char* pass)
{
  if(BaseWasLoaded)return true;

  bool showMess=false;
if(showMess)MessageBox(0,"init begin","1",0);
  char s[1000];
  //загружаем библиотеку base.dll
#ifdef TEXTANALYSERSQL
  wsprintf(s,"%s\\plugins\\TextAnalyser\\Base_sql.dll",sc_path);
#else
  wsprintf(s,"%s\\plugins\\TextAnalyser\\Base.dll",sc_path);
#endif
  Base = LoadLibrary(s); if(!Base)return false;
if(showMess)MessageBox(0,s,"2",0);

  //получаем адреса функций из библиотеки base.dll
//  LoadFunction(Base, fGetLog, "GetLog"); if(!fGetLog)return false;
//if(showMess)MessageBox(0,"GetLog","3",0);
  LoadFunction(Base, InitDict, "Init"); if(!InitDict)return false;
if(showMess)MessageBox(0,"Init","4",0);
  LoadFunction(Base, fGetWordInfo, "GetWordInfo"); if(!fGetWordInfo)return false;
if(showMess)MessageBox(0,"GetWordInfo","5",0);
  LoadFunction(Base, fAnalyse, "Analyse"); if(!fAnalyse)return false;
if(showMess)MessageBox(0,"Analyse","6",0);
  LoadFunction(Base, fGetWordProperty, "GetWordProperty"); if(!fGetWordProperty)return false;
if(showMess)MessageBox(0,"GetWordProperty","7",0);
  LoadFunction(Base, fGetWordForm, "GetWordForm"); if(!fGetWordForm)return false;
if(showMess)MessageBox(0,"GetWordForm","8",0);

  LoadFunction(Base, fSetSearchCriteria, "SetSearchCriteria"); if(!fSetSearchCriteria)return false;
if(showMess)MessageBox(0,"SetSearchCriteria","9",0);
  LoadFunction(Base, fGetSearchResult, "GetSearchResult"); if(!fGetSearchResult)return false;
if(showMess)MessageBox(0,"GetSearchResult","10",0);
  LoadFunction(Base, fClearSearchCriteria, "ClearSearchCriteria"); if(!fClearSearchCriteria)return false;
if(showMess)MessageBox(0,"ClearSearchCriteria","11",0);
  LoadFunction(Base, fFindNextWord, "FindNextWord"); if(!fFindNextWord)return false;
if(showMess)MessageBox(0,"FindNextWord","11",0);
  LoadFunction(Base, fFindPrevWord, "FindPrevWord"); if(!fFindPrevWord)return false;
if(showMess)MessageBox(0,"FindPrevWord","11",0);
  LoadFunction(Base, fAnalyseWord, "AnalyseWord"); if(!fAnalyseWord)return false;
if(showMess)MessageBox(0,"AnalyseWord","11",0);

#ifdef TEXTANALYSERSQL
  LoadFunction(Base, fMorphDivide, "MorphDivide"); if(!fMorphDivide)return false;
if(showMess)MessageBox(0,"MorphDivide","8",0);
  LoadFunction(Base, fSetMorphDivide, "SetMorphDivide"); if(!fSetMorphDivide)return false;
if(showMess)MessageBox(0,"SetMorphDivide","8",0);
#endif


if(showMess)MessageBox(0,"InitDict execute","12",0);
  //загружаем словарь base.dic
#ifdef TEXTANALYSERSQL
  //InitDict("ta_russian","root","root");
 	InitDict(baseName,user,pass);
#else
  //wsprintf(s,"%s\\plugins\\TextAnalyser\\base.dic",sc_path);
  wsprintf(s,"%s\\plugins\\TextAnalyser\\base_fast.dic",sc_path);
  if(InitDict(s)!=0)return false;
#endif
if(showMess)MessageBox(0,"InitDict complete","13",0);

  return BaseWasLoaded=true;
}

bool CTextAnalyser::InitAnalyse()
{
  if(AnalyseWasLoaded)  return true;

  if(!BaseWasLoaded) return false;
  //загружаем библиотеку Analyse.dll
  char s[1000];
  wsprintf(s,"%s\\plugins\\TextAnalyser\\Analise.dll",sc_path);
  Analyse = LoadLibrary(s); if(!Analyse)return false;

  //получаем адреса функций из библиотеки Analise.dll
  LoadFunction(Analyse, InitPlugin, "Init"); if(!InitPlugin)return false;
  LoadFunction(Analyse, fAnalyseText, "AnaliseText"); if(!fAnalyseText)return false;
  LoadFunction(Analyse, fGetWordPropertyInSent, "GetWordPropertyInSent"); if(!fGetWordPropertyInSent)return false;
  LoadFunction(Analyse, fGetWordInSentByRole, "GetWordInSentByRole"); if(!fGetWordInSentByRole)return false;

  //загружаем плагины
  wsprintf(s,"%s\\plugins\\TextAnalyser\\",sc_path);
  if(InitPlugin(s)!=0)return false;
  return AnalyseWasLoaded=true;
}

bool CTextAnalyser::InitMorph()
{
	//MessageBox(0,"InitMorph0","",0);
  if(MorphWasLoaded){ return true;}

	//MessageBox(0,"InitMorph","",0);
  //загружаем библиотеку
  char s[1000];
  wsprintf(s,"%s\\plugins\\TextAnalyser\\morph.dll",sc_path);
  Morph = LoadLibrary(s); if(!Morph)return false;
	//MessageBox(0,"InitMorph1","",0);

  //получаем адреса функций из библиотеки Analise.dll
  LoadFunction(Morph, fInitMorph, "Init"); if(!fInitMorph)return false;
  LoadFunction(Morph, fDivide, "Divide"); if(!fDivide)return false;
	//MessageBox(0,"InitMorph2","",0);

  //загружаем плагины
  wsprintf(s,"%s\\plugins\\TextAnalyser\\root.txt",sc_path);
  char s1[1000];
  wsprintf(s1,"%s\\plugins\\TextAnalyser\\schema.txt",sc_path);
  if(fInitMorph(s,s1)!=0)return false;
	//MessageBox(0,"InitMorph3","",0);

  return MorphWasLoaded=true;
}


char* CTextAnalyser::GetWordInfo(char* Word)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited()) //проверяем инициализирован ли словарь
  {
    LONG size = fGetWordInfo(Word,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      fGetWordInfo(Word,result);
    }
  }
  return result;
}

char* CTextAnalyser::AnalyseSentance(char* Words)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited()) //проверяем инициализирован ли словарь
  {
    LONG size = fAnalyse(Words,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      fAnalyse(Words,result);
    }
  }
  return result;
}

char* CTextAnalyser::GetWordProperty(char* Word, char* Part, char* Name)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited()) //проверяем инициализирован ли словарь
  {
    LONG size=0;
    if(strlen(Part)==0)
      size = fGetWordProperty(Word,NULL,Name,NULL);
    else
      size = fGetWordProperty(Word,Part,Name,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      if(strlen(Part)==0)
        fGetWordProperty(Word,NULL,Name,result);
      else
        fGetWordProperty(Word,Part,Name,result);
    }
  }
  return result;
}

int CTextAnalyser::GetWordFormCount(char* word)
{
  if(IsInited()) //проверяем инициализирован ли словарь
  {
    return fGetWordForm(word,0,NULL,NULL);
  }
  return 0;
}

char* CTextAnalyser::GetWordForm(char* Word, int Num, char* Properties)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited() && Num>=0) //проверяем инициализирован ли словарь
  {
    LONG size = fGetWordForm(Word,Num,Properties,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      fGetWordForm(Word,Num,Properties,result);
    }
  }
  return result;
}

char* CTextAnalyser::WordDivide(char* Word)
{
//	MessageBox(0,"WordDivide",Word,0);
  char* result=new char[1];
  wsprintf(result,"");
  if(!InitMorph()/*||!fDivide||!Word*/) return result;
//	MessageBox(0,"WordDivide1","",0);
  int size = 0;
  fDivide(Word,0,&size);
//	MessageBox(0,"WordDivide2","",0);
  char er[100];
  wsprintf(er,"%d",size);
//	MessageBox(0,Word,"",0);
// 	MessageBox(0,er,"size",0);
  if(size>0)
	{
    result = new char[size+1];
	  fDivide(Word,result,&size);
	//MessageBox(0,"WordDivide4","",0);
    result[size]='\0';
    //MessageBox(0,result,"",0);
  }
  return result;
}

char* CTextAnalyser::MorphDivide(char* Word)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(!IsInited() || !fMorphDivide) return result;
  if(Word)
  {
    int size = 0;
    fMorphDivide(Word,0,&size);
    if(size>0)
    {
      result = new char[size+1];
	    fMorphDivide(Word,result,&size);
    }
  }
  return result;
}

int CTextAnalyser::SetMorphDivide(char* Form, char* Divide)
{
  if(!IsInited() || !fSetMorphDivide || !Form || !Divide) return 0;
  return fSetMorphDivide(Form,Divide);
}


long CTextAnalyser::SearchWords(char* criteria, char** &result, long begin, long count)
{
  if(!IsInited()) return -1;
  bool showMessage=false;
if(showMessage)MessageBox(0,criteria,"1",0);
  LONG foundedCount = fSetSearchCriteria(criteria);
  if(foundedCount<0) return -2;
  if(foundedCount==0) return 0;
if(showMessage)MessageBox(0,"","2",0);
  long end = min(begin+count,min(foundedCount,max_size+begin));
 	count = end-begin;//min(foundedCount-begin,count);
if(showMessage)
{
char s[100];
wsprintf(s,"Найдено %d слов. begin=%d, end=%d, count=%d",foundedCount,begin,end,count);
MessageBox(0,s,"3",0);
}
  if(count<=0)return 0;
  result=new char*[count];
  int wordLen=0;
  for(long i=begin; i<end; i++)
  {
  	wordLen=0;
//if(showMessage)MessageBox(0,"","4.1",0);
    fGetSearchResult(i,0,&wordLen);
    if(wordLen<1) return -3;
//if(showMessage)MessageBox(0,"","4.2",0);
  	char* word = new char[wordLen];
    if(fGetSearchResult(i,word,&wordLen)<0) return -3;
    result[i-begin]=word;
//    word=result[i];
if(showMessage)MessageBox(0,word,"4.3",0);
  }
if(showMessage)MessageBox(0,"","5",0);
  if(fClearSearchCriteria()<0)return -4;
if(showMessage)MessageBox(0,"","6",0);
  return count;
}

char* CTextAnalyser::FindNextWord(char* criteria)
{
  if(!IsInited()) return 0;
  bool showMessage=false;
  if(criteria)
  {
    int bufferSize = 0;
    int ret = fFindNextWord(criteria,0,&bufferSize);
if(showMessage)MessageBox(0,criteria,"",0);
    if(bufferSize>0)
    {
      char* result = new char[bufferSize+1];
	    int ret1 = fFindNextWord(criteria,result,&bufferSize);
      result[bufferSize]=0;
if(showMessage)
{
char s[100];
wsprintf(s,"bufferSize =%d, ret = %d",bufferSize,ret);
MessageBox(0,s,"",0);
}
//if(showMessage)MessageBox(0,GetLog(),result,0);
      return result;
    }
  }
	return 0;
}

char* CTextAnalyser::FindPrevWord(char* criteria)
{
  if(!IsInited()) return 0;
  if(criteria)
  {
    int bufferSize = 0;
    fFindPrevWord(criteria,0,&bufferSize);
    if(bufferSize>0)
    {
      char* result = new char[bufferSize+1];
	    fFindPrevWord(criteria,result,&bufferSize);
      return result;
    }
  }
	return 0;
}

char* CTextAnalyser::AnalyseWord(char* word)
{
  if(!IsInited() || !word) return 0;
  int bufferSize = fAnalyseWord(word,0);;
  if(bufferSize>0)
  {
    char* result = new char[bufferSize+1];
	  fAnalyseWord(word,result);
    return result;
  }
	return 0;
}

char* CTextAnalyser::GetAnswer(char* Sentance, char* Question)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited())
  {
    char* AnalyseSent=AnalyseSentance(Sentance);
    char* AnalyseQuestion=AnalyseSentance(Question);
    LONG size = fAnalyseText(AnalyseSent,AnalyseQuestion,NULL,NULL);
    if(size>0)
    {
      char* Sentance=new char[size];
      size = fAnalyseText(AnalyseSent,AnalyseQuestion,Sentance,NULL);
      if(size>0)
      {
        delete result;
        char* Answer=new char[size];
        fAnalyseText(AnalyseSent,AnalyseQuestion,Sentance,Answer);
        result=Answer;
      }
    }
  }
  return result;
}

char* CTextAnalyser::GetSentanceTree(char* Sentance)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited())
  {
    char* AnalysedSent=AnalyseSentance(Sentance);
    LONG size = fAnalyseText(AnalysedSent,NULL,NULL,NULL);
    if(size>0)
    {
    	//MessageBox(0,Sentance,"awef",0);
      char* Tree=new char[size];
      fAnalyseText(AnalysedSent,NULL,Tree,NULL);
      result=Tree;
    }
  }
  return result;
}

char* CTextAnalyser::GetWordPropertyInSent(char* Sentence, char* Word, int Pos, char* Name)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited() && Pos>=0) //проверяем инициализирован ли словарь
  {
    char* AnalyseSent=AnalyseSentance(Sentence);
    LONG size=fGetWordPropertyInSent(AnalyseSent,Word,Pos,Name,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      fGetWordPropertyInSent(AnalyseSent,Word,Pos,Name,result);
    }
  }
  return result;
}

char* CTextAnalyser::GetWordInSentByRole(char* Sentence, char* Role)
{
  char* result=new char[1];
  wsprintf(result,"");
  if(IsInited()) //проверяем инициализирован ли словарь
  {
    char* AnalyseSent=AnalyseSentance(Sentence);
    LONG size=fGetWordInSentByRole(AnalyseSent,Role,NULL);
    if(size>0)
    {
      delete result;
      result=new char[size];
      fGetWordInSentByRole(AnalyseSent,Role,result);
    }
  }
  return result;
}


//Создаем функции для виртуальной машины Стратума

void vm_getwordinfo()
{
  char *wordstr=POP_PTR;
  PUSH_PTR(TA->GetWordInfo(wordstr));
}

void vm_getwordinfotostream()
{
  int hStream=POP_LONGi;
  char *wordstr=POP_PTR;
  if(hStream>0)
  {
  	streams->SWrite(hStream, TA->GetWordInfo(wordstr));
  	streams->Seek(hStream, 0);
  }
}

void vm_getwordproperty()
{
  char *Name=POP_PTR;
  char *Part=POP_PTR;
  char *Word=POP_PTR;
  PUSH_PTR(TA->GetWordProperty(Word,Part,Name));
}

void vm_getanswer()
{
  char *question=POP_PTR;
  char *Sentence=POP_PTR;
  PUSH_PTR(TA->GetAnswer(Sentence,question));
}

void vm_GetSentanceTree()
{
  char *Sentence=POP_PTR;
  PUSH_PTR(TA->GetSentanceTree(Sentence));
}

void vm_GetWordPropertyInSent()
{
  char *Name=POP_PTR;
  int  Pos=POP_DOUBLE;
  char *Word=POP_PTR;
  char *Sentence=POP_PTR;
  PUSH_PTR(TA->GetWordPropertyInSent(Sentence,Word,Pos,Name));
}

void vm_GetWordInSentByRole()
{
  char *Role=POP_PTR;
  char *Sentence=POP_PTR;
  PUSH_PTR(TA->GetWordInSentByRole(Sentence,Role));
}

void vm_GetWordFormCount()
{
  char *Word=POP_PTR;
  PUSH_DOUBLE(TA->GetWordFormCount(Word));
}

void vm_GetWordForm()
{
  char* Properties=POP_PTR;
  int   Num=POP_DOUBLE;
  char* Word=POP_PTR;
  PUSH_PTR(TA->GetWordForm(Word,Num,Properties));
}

dyno_t GetSerchWords(char* criteria, long begin, long end)
{
  char** results;
  long count=0;
  dyno_t h=0;
  if(count = TA->SearchWords(criteria,results,begin,end))
  {
	  h=dyno_mem->NewVar();
    for(long i=0; i<count && h>0 && i<max_size;i++)
    {
	  	dyno_mem->InsertInVar(h, "STRING");
			dyno_mem->SetVar(h, i, "STRING", 3, results[i]);
    }
  }
  return h;
}

void vm_searchwords()
{
  bool showMessage=false;

  char* criteria=POP_PTR;
  PUSH_LONG(GetSerchWords(criteria,0,max_size));
}

void vm_searchwords_end()
{
  bool showMessage=false;

  long end = (int)POP_DOUBLE;
  char* criteria=POP_PTR;
  PUSH_LONG(GetSerchWords(criteria,0,end));
}

void vm_searchwords_begin_end()
{
  bool showMessage=false;

  long end = (int)POP_DOUBLE;
  long begin = (int)POP_DOUBLE;
  char* criteria=POP_PTR;
  PUSH_LONG(GetSerchWords(criteria,begin,end));
}

void vm_findnextword()
{
  char* criteria=POP_PTR;
  PUSH_PTR(TA->FindNextWord(criteria));
}
void vm_findprevword()
{
  char* criteria=POP_PTR;
  PUSH_PTR(TA->FindPrevWord(criteria));
}

void vm_analyseword()
{
  char* word=POP_PTR;
  PUSH_PTR(TA->AnalyseWord(word));
}

void vm_initanalyzer()
{
  char* password=POP_PTR;
  char* username=POP_PTR;
  char* schemaname=POP_PTR;

  TA=new CTextAnalyser(schemaname,username,password);
  PUSH_DOUBLE(TA && TA->IsInited());
}

void vm_worddivide()
{
  char* word=POP_PTR;
  PUSH_PTR(TA->WordDivide(word));
}

void vm_morphdivide()
{
  char* word=POP_PTR;
  PUSH_PTR(TA->MorphDivide(word));
}

void vm_setmorphdivide()
{
  char* divide=POP_PTR;
  char* form=POP_PTR;
  PUSH_DOUBLE(TA->SetMorphDivide(form,divide));
}

//Добавляем созданные функции виртуальной машины в массив команд ВМ
void InitTextAnalyserCommands()
{
  TA=new CTextAnalyser();

	commands[VM_GETWORDINFO] = vm_getwordinfo;
	commands[VM_GETWORDINFOTOSTREAM] = vm_getwordinfotostream;
	commands[VM_GETANSWER] = vm_getanswer;
	commands[VM_GetSentanceTree] = vm_GetSentanceTree;
	commands[VM_GETWORDPROPERTY] = vm_getwordproperty;
	commands[VM_GetWordPropertyInSent] = vm_GetWordPropertyInSent;
	commands[VM_GetWordInSentByRole] = vm_GetWordInSentByRole;
	commands[VM_GetWordFormCount] = vm_GetWordFormCount;
	commands[VM_GetWordForm] = vm_GetWordForm;
	commands[VM_SEARCHWORDS] = vm_searchwords;
	commands[VM_SEARCHWORDS_END] = vm_searchwords_end;
	commands[VM_SEARCHWORDS_BEGIN_END] = vm_searchwords_begin_end;
	commands[VM_FINDNEXTWORD] = vm_findnextword;
	commands[VM_FINDPREVWORD] = vm_findprevword;
	commands[VM_ANALYSEWORD] = vm_analyseword;
	commands[VM_INITANALYZER] = vm_initanalyzer;
	commands[VM_MORPHDIVIDE] = vm_morphdivide;
	commands[VM_SETMORPHDIVIDE] = vm_setmorphdivide;
	commands[VM_WORDDIVIDE] = vm_worddivide;
}

