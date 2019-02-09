/*
Класс для шифрования и дешифрования текстов.
Для шифрования использовать статический метод Encrypt.
Для дешифрования использовать статический метод Decrypt.
*/
#pragma once

#include "encryption.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>

/// Генерирует ключ для шифрования методом сложной замены Гронсфельда
/// <param name="Len">Размер ключа</param>
/// <returns>Ключ для шифрования</returns>
char* Encryption::KeyGenerator(int Len)
{
	char *key = new char[Len+1];
  char c[1];
  randomize();
	for (int i = 0; i < Len; i++)
  {
    gcvt(max(1,min(7,random(6))),1,c);
    key[i]=c[0];
  }
  key[Len]=0;
	return key;
}

/// Шифрование текста методом сложной замены Гронсфельда
/// <param name="OrigMessage">Текст, который нужно зашифровать</param>
/// <param name="KeyMessage">Ключ для шифрования, например, 535821356</param>
/// <returns>Зашифрованный текст</returns>
char* Encryption::ReplaceEncrypt(char* OrigMessage, char* KeyMessage)
{
	int r = 0;
	char *tmp = new char[strlen(OrigMessage)+1],*c=new char[2];
	strcpy(tmp,OrigMessage);
	for (int i = 0; i < strlen(OrigMessage); i++)
	{
    strncpy(c,KeyMessage+r,1);
    c[1]=0;
  	int t=tmp[i] - atoi(c);
    //if(t<=0)t+=256;
		tmp[i] = char(t);
		r++;
		if (r >= strlen(KeyMessage)) r = 0;
	}
	return tmp;
}

/// Дешифрование текста, зашифрованного методом Гронсфельда
/// <param name="EncMessage">Зашифрованный текст</param>
/// <param name="KeyMessage">Ключ для дешифрования, например, 535821356</param>
/// <returns>Дешифрованный текст</returns>
char* Encryption::ReplaceDecrypt(char* EncMessage, char* KeyMessage)
{
  for(int i=0;i<KEYLENGTH;i++)
    if(KeyMessage[i]<'0' || KeyMessage[i] > '9') return "";

	int r = 0;
	char* tmp = new char[strlen(EncMessage)+1],*c=new char[2];
  strcpy(tmp,EncMessage);
	for (i = 0; i < strlen(EncMessage); i++)
	{
    strncpy(c,KeyMessage+r,1);
  	int t=int(tmp[i]) + atoi(c);
    //if(t>256)t-=255;
		tmp[i] = char(t);
		r++;
		if (r >= strlen(KeyMessage)) r = 0;
	}
	return tmp;
}


/// Шифрование текста методом прямоугольника
/// <param name="OrigMessage">Текст, который нужно зашифровать</param>
/// <param name="WidthRect">Ширина прямоугольника, например, 5</param>
/// <returns>Зашифрованный текст</returns>
char* Encryption::ExchangeEncrypt(char* OrigMessage, int WidthRect)
{
	int r = strlen(OrigMessage) / WidthRect;
  int len=(r + 1) * WidthRect+1;
  char* tmp=new char[len];
  for(int i=0; i<len-1;i++) tmp[i]=' ';
  tmp[i]='\0';
  for(i=0;i<strlen(OrigMessage);i++) tmp[i]=OrigMessage[i];

  char* result=new char[len];
  strcpy(result,tmp);
  int s=0;
	for(i = WidthRect; i > 0; i--)
		for(int j = 0; j <= r; j++)
		{
      result[s]=tmp[j * WidthRect + i - 1];
      s++;
    }
	return result;
}

/// Дешифрование текста, зашифрованного методом прямоугольника
/// <param name="EncMessage">Зашифрованный текст</param>
/// <param name="WidthRect">Ширина прямоугольника, например, 5</param>
/// <returns>Дешифрованный текст</returns>
char* Encryption::ExchangeDecrypt(char* EncMessage, int WidthRect)
{
	int r = strlen(EncMessage) / WidthRect;
	char* result = new char[strlen(EncMessage)+1];
  strcpy(result,EncMessage);
  int s=0;
	for (int i = 0; i < r; i++)
		for (int j = WidthRect - 1; j >= 0; j--)
		{
      result[s]=EncMessage[j * r + i];
      s++;
    }
  result[s]='\0';
	return result;
}

/// Комплексное шифрование текста
/// <param name="OrigMessage">Текст, который нужно зашифровать</param>
/// <returns>Зашифрованный текст</returns>
char* Encryption::Encrypt(char* OrigMessage)
{
 	char* Key = KeyGenerator(KEYLENGTH);
	char* encmess = ReplaceEncrypt(OrigMessage, Key);
  char* result=new char[strlen(encmess)+strlen(Key)+1];
	strcpy(result,encmess);
  strcat(result,Key);
	result = ExchangeEncrypt(result, WIDTHRECT);
	return result;
}

/// Комплексное дешифрование текста
/// <param name="EncMessage">Текст, который нужно дешифровать</param>
/// <returns>Дешифрованный текст</returns>
char* Encryption::Decrypt(char* EncMessage)
{
	char* origmess = ExchangeDecrypt(EncMessage, WIDTHRECT);
  for(int i=strlen(origmess)-1;origmess[i]==' ' && i>=0;i--)
    origmess[i]='\0';
  int len=strlen(origmess)-KEYLENGTH;
  if(len<1)return"";
  char* Key=origmess+len;
	char* origm=new char[len+1];
  strncpy(origm,origmess,len);
  origm[len]='\0';
	origmess = ReplaceDecrypt(origm, Key);
	return origmess;
}
