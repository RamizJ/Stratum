#ifndef __TSCHEME_H_
#define __TSCHEME_H_
#include <math.h>
#include <mem.h>
#include <stdio.h>

#include "class.h"
#include "twindow.h"
#include "dialogs.h"
#include "sc3.rh"
#include "messages.h"
#include "state.h"
#include "stream.h"
#include "grtools.h"
#include "tscheme.h"
#include "video\scvideo.h"
#include  "graphic\3dtools.h"
#include "filetype.h"
#ifdef CAN_EDIT_BITMAP
#include "project.h"
#include "winman.h"
#include "bmpwnd.h"
#endif
#include "hyper.h"
#include <stratum\stratum.h>
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>

#define IsChecked(_MODE,MODE) ((_MODE==MODE)?MF_CHECKED:0)

#define sign(a) (abs(a)==(a))

#define __PrepareToDrag \
	 GetObjectOrgSize2d(hsp,object2d,&delta,NULL);\
	 delta.x=point2d.x-delta.x;delta.y=point2d.y-delta.y;\
	  lastpoint=point2d;\
	  SetCapture();\
	  if ((modKeys & MK_CONTROL)&&(object2d<OID_RESERVED))SetState(sfctrllbtndown2d);\
		 else  SetState(sflbuttondown2d);\
	  SetCurrentObject(object2d);


#ifdef ENGLISH
#define _varplace "Var place"
#else
#define _varplace "Контактная площадка"
#endif
#include "strings.rh"
// для ConvertToOne
#include "autosntz.h"
#include "memmgr.h"

struct LINKCLIP{
 PLinkInfo plink;
 INT16     width;
 INT16     flags;
 COLORREF  color;
 INT16     layer;
 char      obj1[64];
 char      obj2[64];
};

extern UINT CF_2D;
extern UINT CF_3D;
extern UINT CF_RTF;


#endif  //__TSCHEME_H_
