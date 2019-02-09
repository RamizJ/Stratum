#ifndef __SC_CURSOR_j89_H_
#define __SC_CURSOR_j89_H_

enum UPDATE_BROWSE{U_ALL,U_REMOVE,U_RECORD,U_INSERT,U_APPEND,U_CURPOS,U_DELETE};
struct TCursorChangeInfo{
 UPDATE_BROWSE code;
 long y,x;
};

#endif
