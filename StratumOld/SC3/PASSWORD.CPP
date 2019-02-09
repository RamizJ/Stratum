#ifdef PROTECT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//------------------------------------------
void _CharLower(char*s){
 char*_ps=s;
 while(*_ps){
  if(*_ps>='A' && (*_ps<='Z'))(*_ps)-=('A'-'a');
  _ps++;
 }

};


int test1(char *password)
{char chk[10];
 int i,j;
 for (i = 0; i < 10; i++)chk[i]=0;
 if (strlen(password) != 12) return 0;
 for (i = 0; i < 12; i++){
                       if ((password[i] < '0' || password[i] > '9')) return 0;
                        chk[password[i]-'0']++;
                       }
 for (i = 0; i < 10; i++)if(chk[i])j++;
 return j>1;
}

int test2(char *password)
{
 unsigned long keyval;
 char ch,str[10];
 int i,code;

 ch = password[10];
 password[10] = 0;
 keyval = atol(password);
 password[10] = ch;
 code = 0;
 for (i = 0; i < 10; i++)
 {
  code += keyval%10;
  keyval /= 10;
 }
 if (code != atoi(password+10)) return 0;
 return 1;
}

int test3(char *password, char *key_string)
{
 _CharLower(key_string);
 unsigned long keyval;
 int i,element,code;
 unsigned char ch,*byte_array;
 byte_array =(unsigned char*) &keyval;

 code = 0;
 for (i = 0; i < strlen(key_string); i++)
 {
  element = key_string[i];
  if ((element > 96 && element < 123)||(element > 47 && element < 58))
   code += element;
 }

 ch = password[10];
 password[10] = 0;
 keyval = atol(password);
 password[10] = ch;
 for (i = 2; i >= 0; i--) byte_array[i+1] ^= byte_array[i];
 keyval -= code;
 if (keyval > 2176782335) return 0;
 return 1;
}

int test4(char *password, char *key_string)
{
  _CharLower(key_string);
 unsigned long keyval;
 int i,element,code;
 unsigned char ch,*byte_array;
 byte_array =(unsigned char*) &keyval;

 code = 0;
 for (i = 0; i < strlen(key_string); i++)
 {
  element = key_string[i];
  if ((element > 96 && element < 123)||(element > 47 && element < 58))
   code += element;
 }

 ch = password[10];
 password[10] = 0;
 keyval = atol(password);
 password[10] = ch;
 for (i = 2; i >= 0; i--) byte_array[i+1] ^= byte_array[i];
 keyval -= code;
 if (keyval < 604661760) return 0;
 return 1;
}


int test5(char *password, char *key_string)
{
 _CharLower(key_string);
 char buffer[6];
 unsigned long keyval;
 int i,element,code,j;
 unsigned char ch,*byte_array;
 byte_array =(unsigned char*) &keyval;

 code = 0; j = 0;
 for ( i = 0; i < strlen(key_string); i++)
 {
  element = key_string[i];
  if ((element > 96 && element < 123)||(element > 47 && element < 58))
   {
    code += element;
    if (j < 6)
    { buffer[j] = element; j++; }
   }
 }

 ch = password[10];
 password[10] = 0;
 keyval = atol(password);
 password[10] = ch;
 for (i = 2; i >= 0; i--) byte_array[i+1] ^= byte_array[i];
 keyval -= code;
 for (i = 0; keyval > 0; i++)
 {
  if (i == strlen(key_string)) return 0;
  element = key_string[i];
  code = keyval%36;
  keyval /= 36;
  if (code < 10) code += 48;
   else code += 87;
  if (code != buffer[5-i]) return 0;
 }
 return 1;
}


#endif


