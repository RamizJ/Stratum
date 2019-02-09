#ifndef SC_TYPES_H_
#define SC_TYPES_H_
typedef INT16 C_TYPE;
#ifdef WIN32
typedef void *  pointer;
typedef void *  Hpointer ;
#else
typedef void far*     pointer;
typedef void _huge * Hpointer ;
#endif
#endif