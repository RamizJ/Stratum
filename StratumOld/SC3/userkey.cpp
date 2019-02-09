// класс для проверки данных о пользователе по файлу-ключу.
// используется для передачи результатов выполнения пользователем
// цоров на сервер ИВС.

#pragma once
#ifdef WIN32
#undef _WINREG_
#endif

#include <windows.h>
#include <stdlib.h>
#include <shellapi.h>
#include "userkey.h"
#include "dynodata.h"
#include "stream_m.h"
#include "winman.h"
#include "encryption.h"

PUserKey userkey=NULL;
#define slen 1000

bool UserKey::Autorization( bool showmess)
{
	#ifdef LOGON
 	LogMessage("UserKey: Autorization. KeyFullPath:");
 	LogMessage(KeyFullPath);
	#endif
  if(showmess)
  {
	  char 	*capt="Авторизация",
    			*mess="Указан неверный файл-ключ!\nВозможно, истек срок действия. Пожалуйста, получите новый файл-ключ.\nДля этого необходимо войти в систему Электронных дневников и журналов http://web2edu.ru,\nв разделе 'Дополнительное образование' получить файл-ключ,\nсохранить у себя на компьютере и указать путь к этому файлу.";
  	MessageBox(0,mess, capt, 0);
  }
  if(winmanager->mFileDialog(KeyFullPath, "  ", 0))
  {
  	MakeDirectory(temppath);
	  int f=streams->CreateStream("FILE", KeyLinkPath, "CREATE|NOWARNING");
    if(f)
    {
    	streams->Write(f,KeyFullPath,strlen(KeyFullPath));
      streams->CloseStream(f);
	  	return true;
    }
  }
  return false;
}

bool UserKey::ReadAllFile(char* Filename, char* text)
{
	if(!Filename || !text) return false;
  int f=streams->CreateStream("FILE", Filename, "READONLY|NOWARNING");
  if(f)
  {
	  streams->GetAllText(f,text);
	  streams->CloseStream(f);
   	return true;
  }
  return false;
}

bool UserKey::WriteAllFile(char* Filename, char* text)
{
	if(!Filename || !text) return false;
  int f=streams->CreateStream("FILE", Filename, "CREATE|NOWARNING");
  if(f)
  {
	  streams->Write(f,text,strlen(text));
	  streams->CloseStream(f);
   	return true;
  }
  return false;
}


bool UserKey::CheckAutorization(char* text)
{
	IsAutorized=false;
//  MessageBox(0,text,"awef",0);
  if(text)
  if(strlen(text)>0)
  {
	  //дешифруем содержимое файла-ключа
	  char* KeyContent=Encryption::Decrypt(text);
    if(KeyContent)
  	if(strlen(KeyContent)>0)
    {
    	//MessageBox(0,KeyContent,"",0);
		  //создаем список тегов в файле-ключе
		  tags=new CTagsCollection(KeyContent);
      delete[] KeyContent;
		  if(!CheckExpirationDate())
      {
    	//MessageBox(0,KeyContent,"",0);
      	if(tags)
        {
        	tags->FreeAll();
          tags=0;
        }
        return false;
      }
      return IsAutorized=true;
    }
  }
  return false;
}

bool UserKey::CheckExpirationDate()
{
	//MessageBox(0,"111","",0);
	if(!tags)return false;
  char *expdate=tags->GetTagValue("ExpirationDate");
  //MessageBox(0,expdate,"awef",0);
  if(strlen(expdate)<=10) return false;
  char t[10];
	FILETIME ft_cur,ft_exp;
	SYSTEMTIME st_cur, st_exp;
  strncpy(t,expdate,4);	   t[4]='\0';	st_exp.wYear=atoi(t); //MessageBox(0,t,"awef",0);
  strncpy(t,expdate+5,2);	 t[2]='\0';	st_exp.wMonth=atoi(t); //MessageBox(0,t,"awef",0);
  strncpy(t,expdate+8,2);	 t[2]='\0';	st_exp.wDay=atoi(t); //MessageBox(0,t,"awef",0);
  strncpy(t,expdate+11,2); t[2]='\0';	st_exp.wHour=atoi(t); //MessageBox(0,t,"awef",0);
  strncpy(t,expdate+14,2); t[2]='\0';	st_exp.wMinute=atoi(t);// MessageBox(0,t,"awef",0);
  strncpy(t,expdate+17,2); t[2]='\0';	st_exp.wSecond=atoi(t); //MessageBox(0,t,"awef",0);
  strncpy(t,expdate+20,3); t[3]='\0';	st_exp.wMilliseconds=atoi(t); //MessageBox(0,t,"awef",0);

	GetLocalTime(&st_cur);
	SystemTimeToFileTime(&st_cur,&ft_cur);
	SystemTimeToFileTime(&st_exp,&ft_exp);
  if(CompareFileTime(&ft_cur,&ft_exp)<0)
    return true;
 	return false;
};

void UserKey::Init()
{
	#ifdef LOGON
 	LogMessage("UserKey: Init");
	#endif
	tags=NULL;
  IsAutorized=false;
  fileresult=new char[slen];

  EncKeyContent= new char[MAX_CLASSTEXTSIZE];
  EncKeyContent[0]=0;

  temppath=new char[slen];
  GetTempPath(slen,temppath);
  AddSlash(temppath);
  strcat(temppath,"stratum");
	MakeDirectory(temppath);

  //определяем файл с ссылкой на файл-ключ
  KeyLinkPath = new char[slen];
  strcpy(KeyLinkPath," ");
  strcpy(KeyLinkPath,temppath);
  AddSlash(KeyLinkPath);
  strcat(KeyLinkPath,"path.txt");

  KeyFullPath= new char[MAX_CLASSTEXTSIZE];
  strcpy(KeyFullPath," ");

  projectFullPath = new char[slen];
  strcpy(projectFullPath," ");
}

UserKey::UserKey(char* filename)
{
	#ifdef LOGON
 	LogMessage("UserKey: Create instance. filename:");
 	LogMessage(filename);
	#endif
  if(!filename)return;
  if(!ScFileExist(filename)) return;

  Init();

  strcpy(KeyFullPath,filename);

  if(ReadAllFile(KeyFullPath,EncKeyContent))
    CheckAutorization(EncKeyContent);
}

UserKey::UserKey()
{
	#ifdef LOGON
 	LogMessage("UserKey: Create instance without filename");
	#endif
  Init();

  //ищем файл-ключ в папке проекта
  GetProjectPath(KeyFullPath);
  AddSlash(KeyFullPath);
  strcat(KeyFullPath,"user.key");
  // || true - hack - если в папке проекта нет user.key,
  // то не ищем его больше
  if(ReadAllFile(KeyFullPath,EncKeyContent))
    if(CheckAutorization(EncKeyContent)) goto end;

  //ищем user.key рядом с run.exe
  if(FindUserKeyWithRun())
  {
  	if(ReadAllFile(KeyFullPath,EncKeyContent))
    {
    	if(CheckAutorization(EncKeyContent)) goto end;
		}
  }

	if(!ReadAllFile(KeyLinkPath,KeyFullPath))
  {
  	if(Autorization(true)) ReadAllFile(KeyFullPath,EncKeyContent);
    else goto end;
  }
  else
  {
  	if(!ReadAllFile(KeyFullPath,EncKeyContent))
    {
	  	if(Autorization(true)) ReadAllFile(KeyFullPath,EncKeyContent);
  	  else goto end;
    }
  }

  while(!CheckAutorization(EncKeyContent))
  {
  	MessageBox(0,"Неверный файл-ключ","Авторизация",0);
 	  if(Autorization(false)) ReadAllFile(KeyFullPath,EncKeyContent);
   	else goto end;
  }
end:

  //hack по просьбам трудящихся (если не найден user.key)
  //чтобы userkey.cls не выходил если ключ не найден
  //if(!tags) tags=new CTagsCollection("<SEDIGInfo>Invalid</SEDIGInfo>");

  ReadProjectKey();
};

bool UserKey::FindUserKeyWithRun()
{
  if(!KeyFullPath) return FALSE;
  if(strlen(KeyFullPath)<4) return FALSE;

  char dir[slen],temp[slen];
  GetPath(KeyFullPath,dir);
  int i=strlen(dir);
  if(i<2) return FALSE;

  WIN32_FIND_DATA fdata;
  memset(&fdata, 0, sizeof(fdata));
  fdata.dwFileAttributes=FILE_ATTRIBUTE_NORMAL;
	HANDLE fhandle=0;

  while(i>1)
  {
    strcpy(temp,dir);
    AddSlash(temp);
    fhandle=FindFirstFile(strcat(temp,"run.exe"), &fdata);
 		if(fhandle!=INVALID_HANDLE_VALUE) //нашли run.exe
    {
      char path[slen];
      strcpy(path,dir);
      AddSlash(path);
      strcat(path,"user.key");
	    FindClose(fhandle);
      if(!ScFileExist(path)) return FALSE;
      strcpy(KeyFullPath,path);
    	return TRUE;
    }
    FindClose(fhandle);
    GetPath(dir,dir);
    i=strlen(dir);
  }
  return FALSE;
}

bool UserKey::ReadProjectKey()
{
	#ifdef LOGON
 	LogMessage("UserKey: ReadProjectKey");
	#endif
  if(!IsAutorized) return 0;
  if(!ScFileExist(KeyFullPath)) return 0;

  //ищем файл с номером проекта
  GetPath(KeyFullPath,projectFullPath);
  AddSlash(projectFullPath);
  strcat(projectFullPath,"project.key");
  return ReadProjectKey(projectFullPath);
}

bool UserKey::ReadProjectKey(char* ProjectFullPath)
{
	#ifdef LOGON
 	LogMessage("UserKey: ReadProjectKey with ProjectFullPath");
 	LogMessage(ProjectFullPath);
	#endif
  if(!IsAutorized) return 0;
  if(!ScFileExist(KeyFullPath)) return 0;

  if(ScFileExist(ProjectFullPath) && tags && tags->items)
  {
  	char *encProjectContent= new char[MAX_CLASSTEXTSIZE];
	  encProjectContent[0]=0;
  	if(ReadAllFile(ProjectFullPath,encProjectContent))
    {
  		if(encProjectContent)
		  if(strlen(encProjectContent)>0)
		  {
	  		//дешифруем содержимое файла-ключа
			  char* projectContent=Encryption::Decrypt(encProjectContent);
    		if(projectContent)
		  	if(strlen(projectContent)>0)
    		{
          tags->Add(projectContent);
          if(projectContent) delete[] projectContent;
          return 1;
	    	}
  		}
    }
		if(encProjectContent) delete[] encProjectContent;
  }
  return 0;
};

int UserKey::SendResult(int hstream)
{
	#ifdef LOGON
 	LogMessage("UserKey: SendResult");
	#endif
  // hack
  //return 1;

  if(!IsAutorized && !hstream)return 0;
  //MessageBox(0,"0","awef",0);
  //читаем содержимое результата из потока
  char* ResultContent=new char[MAX_CLASSTEXTSIZE];
  if(!streams->GetAllText(hstream,ResultContent)) return 0;
  //MessageBox(0,"1","awef",0);

  //шифруем результат
  char* EncResult=Encryption::Encrypt(ResultContent);
  if(!EncResult) return 0;
  if(!strlen(EncResult)) return 0;
  //MessageBox(0,"2","awef",0);

  //сохраняем результат в файл
  strcpy(fileresult,temppath);  	AddSlash(fileresult);
  strcat(fileresult,"result.dat");
  //MessageBox(0,fileresult,"awef",0);
 	if(!WriteAllFile(fileresult,EncResult))return 0;
  //MessageBox(0,"3","awef",0);
  //формируем файл письма
 	char mail[slen];
  strcpy(mail,temppath); 	AddSlash(mail);  strcat(mail,"mail");
  int f=streams->CreateStream("FILE", mail, "CREATE|NOWARNING");
  if(!f) return 0;
  //MessageBox(0,"4","awef",0);

 	streams->SWrite(f,"<send_email>");

 	//streams->SWrite(f,"<receiver>marchenko@stratum.ac.ru</receiver>");
 	//streams->SWrite(f,"<receiver>overinanton@mail.ru</receiver>");
 	//streams->SWrite(f,"<receiver>gp@stratum.ac.ru</receiver>");
 	//streams->SWrite(f,"<receiver>travis@stratum.ac.ru</receiver>");
 	//streams->SWrite(f,"<receiver>kuzaev@stratum.ac.ru</receiver>");
 	//streams->SWrite(f,"<receiver>gadiatulin@ics.perm.ru</receiver>");

	streams->SWrite(f,"<receiver>results@web2edu.ru</receiver>");
 	streams->SWrite(f,"<sender>admin@stratum.ac.ru</sender>");
 	streams->SWrite(f,"<title>Results</title>");
 	streams->SWrite(f,"<text>Stratum DER Results</text>");
 	streams->SWrite(f,"</send_email>");
  char temp[slen]; wsprintf(temp,"<attach>%s</attach>",fileresult);
 	streams->SWrite(f,temp);
	streams->CloseStream(f);
  //MessageBox(0,"5","awef",0);

  //Отправляем файл-результат по почте
  char exefile[slen]=""; char params[100]="";
  wsprintf(exefile,"\"%s\\plugins\\sendmail\\sendmail.exe\"",sc_path);
  wsprintf(params,"\"%s\"",mail);
  SHELLEXECUTEINFO sei;
  memset(&sei, 0, sizeof(sei));
 	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.hwnd = mainwindow->HWindow;
	sei.lpFile = exefile;
  sei.lpVerb = "open";
	sei.lpParameters = params;
 	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.nShow = SW_HIDE;
  //MessageBox(0,"6","awef",0);

	if(!ShellExecuteEx(&sei)) return 0;
  //MessageBox(0,"7","awef",0);

	if(sei.hProcess == 0) return 0;
  //MessageBox(0,"8","awef",0);

  WaitForSingleObjectEx(sei.hProcess, INFINITE, false);
  //MessageBox(0,"9","awef",0);
  CloseHandle(sei.hProcess);

  //if(!ShellExecute(mainwindow->HWindow,NULL,exefile,params,NULL,SW_HIDE))return 0;
	return 1;
}

int UserKey::CopyResult()
{
  if(!IsAutorized && !fileresult)return 0;
  int f=streams->CreateStream("FILE", fileresult, "READONLY|NOWARNING");
  if(!f)return 0;
	streams->CloseStream(f);
  char* newfileresult=new char[slen];
  strcpy(newfileresult,"|result.dat");
  if(!winmanager->mFileDialog(newfileresult, "  ", 1))return 0;
  if(!ScCopyFile(fileresult, newfileresult))return 0;
  return 1;
}

