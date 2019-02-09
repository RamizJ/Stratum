#include <stratum\sccall.h>
typedef void* NETAREA;
typedef struct tagNETADDR{
 char*addr;
 int size;
}NETADDR;

#define AREA_SERVER      0x01
#define AREA_CLIENT      0x0
#define AREA_SENDER      0x02
#define AREA_RECEIVER    0x0
#define AREA_AUTORECEIVE 0x04
#define AREA_SYNCHRONE   0x08
#define AREA_TWOPHASE    0x800

NETAREA SDKPROC CreateAreaNET(DWORD data,DWORD data1,char*protocol,char*,int id,int type);
NETAREA SDKPROC GetNetArea(DWORD data1);
BOOL    SDKPROC AddToAreaNET(NETAREA,void * addr,int size);// Добавить в область новые адреса
BOOL    SDKPROC  AddToAreaNETEx(NETAREA _a,void * addr,int size,char *n,char * t);
BOOL    SDKPROC ChangeAreaNET(NETAREA,NETADDR*,int count);

BOOL    SDKPROC SubmitAreaNET(NETAREA);                    // Создание области завершено
BOOL    SDKPROC RemoveAreaNET(NETAREA);                    // Удалить одну область
BOOL    SDKPROC RemoveAreasNET(DWORD data);                // Удалить сразу кучу областей
BOOL    SDKPROC AreasChanged();

BOOL    SDKPROC AddSendAddressNET(NETAREA,char*,int id,int type);

BOOL    SDKPROC SetAreaStatusNET(NETAREA,int id,int flag);
BOOL    SDKPROC NetworkProc();


/*
1. Передача всем перечисленным
 2. Передача одному

Все сводится на прием от одного.
3. Прием от одного.

4. Посылка сетевого сообщения

*/
