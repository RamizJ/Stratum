/*
����� UserKey ��� �������� ������ � ������������ �� �����-�����.
������������ ��� �������� ����������� ���������� �������������
����� �� ������ ���.

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
  //��������� ����������� ������������ �� �����-�����
  bool CheckAutorization(char* text);
	bool CheckExpirationDate();
  bool FindUserKeyWithRun();
public:
  PTagsCollection tags;
  bool IsAutorized;

  UserKey();
  UserKey(char* filename);

  //������ �����-����� � ��������������� �������
  bool ReadProjectKey();
  bool ReadProjectKey(char* ProjectFullPath);
  //�������� ���������� ���������� �������
	int SendResult(int hstream);
  //�������� ���� ���������� � �������� �����
  int CopyResult();
};
typedef UserKey* PUserKey;

extern PUserKey userkey;

//#endif
