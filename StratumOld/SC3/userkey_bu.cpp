// класс для проверки данных о пользователе по файлу-ключу.
// используется для передачи результатов выполнения пользователем
// цоров на сервер ИВС.

#pragma once
#include "time.h"
#include "userkey.h"
#include "encryption.h"
#include "dynodata.h"
#include "winman.h"
#include "stream_m.h"

PUserKey userkey=NULL;
#define slen 1000

bool UserKey::Autorization( bool showmess)
{
  if(showmess)
  {
	  char 	*capt="Авторизация",
    			*mess="Вам необходимо авторизоваться на сайте\nи указать путь к полученному там файлу-ключу!";
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
//char s[20];
//wsprintf(s,"%d",f);
//MessageBox(0,text,Filename,0);
	  streams->CloseStream(f);
   	return true;
  }
  return false;
}

bool UserKey::CheckAutorization(char* text)
{
  //MessageBox(0,text,"awef",0);
  if(text)
  if(strlen(text)>0)
  {
  	//MessageBox(0,text,"awef",0);
	  //дешифруем содержимое файла-ключа
	  char* KeyContent=Encryption::Decrypt(text);
  	if(strlen(KeyContent)>0)
    {
		  //создаем список тегов в файле-ключе
		  tags=new CTagsCollection(KeyContent);
		  if(!CheckExpirationDate())
      {if(tags)tags->FreeAll(); return false;}
      IsAutorized=true;
      return true;
    }
  }
  return false;
}

UserKey::UserKey()
{
	tags=NULL;
  IsAutorized=false;

  char *EncKeyContent= new char[slen];
  EncKeyContent[0]=0;

  //определяем файл с ссылкой на файл-ключ
  temppath=new char[100];
  GetTempPath(slen,temppath);
  AddSlash(temppath);
  strcat(temppath,"stratum");

  KeyLinkPath = new char[100];
  strcpy(KeyLinkPath," ");
  strcpy(KeyLinkPath,temppath);
  AddSlash(KeyLinkPath);
  strcat(KeyLinkPath,"path.txt");

  //ищем файл-ключ в папке проекта
  char projpath[slen];
  GetProjectPath(projpath);
  AddSlash(projpath);
  KeyFullPath= new char[100];
  strcpy(KeyFullPath," ");
  wsprintf(KeyFullPath,"%suser.key",projpath);

  //bool isdefpath=true;
  if(ReadAllFile(KeyFullPath,EncKeyContent))
    if(CheckAutorization(EncKeyContent)) return;
  //MessageBox(0,KeyFullPath,"awef",0);
  //int f=streams->CreateStream("FILE", KeyFullPath, "READWRITE|NOWARNING");
  //if(f) goto readkey;

//readlink:
	//isdefpath=false;
//  f=streams->CreateStream("FILE", KeyLinkPath, "READWRITE|NOWARNING");
//  if(!f)
//  {
//    if(!Autorization(true))	return;
//  }
//  else
//  {
//	  streams->GetLine(f,KeyFullPath);
//	  streams->CloseStream(f);
//	  //MessageBox(0,KeyFullPath,"awef",0);
//  }
//  if(strlen(KeyFullPath)<1)return;

	if(!ReadAllFile(KeyLinkPath,KeyFullPath))
  {
  	if(Autorization(true)) ReadAllFile(KeyFullPath,EncKeyContent);
    else return;
  }
  else ReadAllFile(KeyFullPath,EncKeyContent);

  while(!CheckAutorization(EncKeyContent))
  {
  	MessageBox(0,"Неверный файл-ключ","Авторизация",0);
 	  if(Autorization(false)) ReadAllFile(KeyFullPath,EncKeyContent);
   	else return;
  }

//readfile:
//  f=streams->CreateStream("FILE", KeyFullPath, "READONLY|NOWARNING");
//  if(!f)
//    if(!Autorization(true))	return;

//readkey:
  //читаем содержимое файла-ключа
//  MessageBox(0,KeyLinkPath,"",0);
//  streams->GetAllText(f,EncKeyContent);
//  streams->CloseStream(f);
//  if(strlen(EncKeyContent)<1)
  {
//wrongformat:
//    if(isdefpath) goto readlink;
//  	MessageBox(0,"Неверный файл-ключ","Авторизация",0);
// 	  if(Autorization(false)) goto readfile;
//   	else return;
  }

//  char* KeyContent=Encryption::Decrypt(EncKeyContent);
//  if(strlen(KeyContent)<1) goto wrongformat;
  //MessageBox(0,KeyContent,"",0);

//  tags=new CTagsCollection(KeyContent);
//  if(!IsUserAutorized()){ if(tags)tags->FreeAll(); goto wrongformat;}

  IsAutorized=true;
};

bool UserKey::CheckExpirationDate()
{
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

bool UserKey::SendResult(int hstream)
{
  if(!IsAutorized)return false;

  //читаем содержимое результата из потока
  char ResultContent[slen];

  //шифруем результат

  //сохраняем результат в файл

  //Отправляем файл-результат по почте

	return false;
}
