/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			media.h
Author:
			Alexander Shelemekhov
*/

void InitInput();
void InitMedia();

int PlayWAV(char* lpszSoundName, UINT uFlags);
double GetJoyX(int id);
double GetJoyY(int id);
double GetJoyZ(int id);
BYTE   GetJoyBtn(int id);

// MCI interface

double GetLastMCIrez();
DWORD V_mcisendstring(char *cmd,char*ret,int hsp);
UINT _mcigeterrorestring(double error,char*buf);
BOOL CameraProc3d(PObject po,HSP3D hsp3d,HOBJ2D obj,BOOL set);
