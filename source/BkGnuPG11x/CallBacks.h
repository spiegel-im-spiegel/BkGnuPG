/************************************************************************/
/*  File name : CallBacks.h                                             */
/************************************************************************/
/*  Contents  : 各種 CallProc 関数定義                                  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

/**** Incude Files ****/
#include <windows.h>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/
//「署名・暗号化」メニューの処理
extern void WINAPI Encrypt(HWND hWnd, LPARAM lParam);
extern void WINAPI EncryptSign(HWND hWnd, LPARAM lParam);
extern void WINAPI Sign(HWND hWnd, LPARAM lParam);
//「公開鍵」メニューの処理
extern void WINAPI PubKeyExport(HWND hWnd, LPARAM lParam);
//「復号化」メニューの処理
extern void WINAPI Decript(HWND hWnd, LPARAM lParam);


#endif // _CALLBACKS_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
