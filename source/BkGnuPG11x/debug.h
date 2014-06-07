/************************************************************************/
/*  File name : debug.h                                                 */
/************************************************************************/
/*  Contents  : デバッグルーチン定義                                    */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.20    2000.10.02          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _DEBUG_H_
#define _DEBUG_H_
#ifdef _DEBUG //デバッグリリース時にのみ定義

/**** Incude Files ****/
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>


/**** Global Macro ****/
//間抜けなことにデバッグモード時はinline関数は展開されない(^^;) ので，#defineで対処
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__ )


#endif // _DEBUG
#endif // _DEBUG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
