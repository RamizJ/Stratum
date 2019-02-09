/*
Copyright (c) 1995  Virtual World
Module Name:
	 error_h.h
Author:
	 Alexander Shelemekhov
----------------------------------------------
Error handler functions and defines
*/

#ifndef __ERROR_HANDLER
#define __ERROR_HANDLER
#include "errors.rh"
HINSTANCE extern hInstance; 
int _Error(int);
int _abstract();
int _notrealized();
int SetErrorLogFile(char* );
#endif
