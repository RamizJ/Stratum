/*
Класс для шифрования и дешифрования текстов.
Для шифрования использовать статический метод Encrypt.
Для дешифрования использовать статический метод Decrypt.

encryption.cpp
*/
#pragma once

#define KEYLENGTH 10
#define WIDTHRECT 7

class Encryption
{
private:
	static char* KeyGenerator(int Len);
  static char* ReplaceDecrypt(char* EncMessage, char* KeyMessage);
  static char* ExchangeDecrypt(char* EncMessage, int WidthRect);
  static char* ReplaceEncrypt(char* OrigMessage, char* KeyMessage);
  static char* ExchangeEncrypt(char* OrigMessage, int WidthRect);

public:
	static char* Encrypt(char* OrigMessage);
  static char* Decrypt(char* EncMessage);
};
