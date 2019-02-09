/*
Класс UserKey для проверки данных о пользователе по файлу-ключу.
Используется для передачи результатов выполнения пользователем
цоров на сервер ИВС.

userkey.cpp
*/
#pragma once
//#ifndef userkey_h
//#define userkey_h
#include "collect.h"

class UserKey
{
protected:
  char* temppath;

	char* KeyLinkPath;
	char* KeyFullPath;
	char* projectFullPath;
  char* EncKeyContent;
  char* fileresult;

  void Init();

  bool Autorization(bool);
  bool ReadAllFile(char* Filename, char* text);
  bool WriteAllFile(char* Filename, char* text);
  //проверить авторизацию пользователя по файлу-ключу
  bool CheckAutorization(char* text);
	bool CheckExpirationDate();
  bool FindUserKeyWithRun();
public:
  PTagsCollection tags;
  bool IsAutorized;

  UserKey();
  UserKey(char* filename);

  //чтение файла-ключа с идентификатором проекта
  bool ReadProjectKey();
  bool ReadProjectKey(char* ProjectFullPath);
  //Отправка результата выполнения проекта
	int SendResult(int hstream);
  //копируем файл результата в заданную папку
  int CopyResult();
};
typedef UserKey* PUserKey;

extern PUserKey userkey;

//#endif
