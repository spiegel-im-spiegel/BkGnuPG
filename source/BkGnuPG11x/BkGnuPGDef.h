/************************************************************************/
/*  File name : BkGnuPGDef.h                                            */
/************************************************************************/
/*  Contents  : Becky! GNU Privacy Guard プラグイン 共通定義            */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _BKGNUPGDEF_H_
#define _BKGNUPGDEF_H_

/**** Incude Files ****/
#define WIN32_LEAN_AND_MEAN // Windows ヘッダーから殆ど使用されないスタッフを除外します
#include <windows.h>
#include "resource.h"

#include <string>
#include <vector>


/**** Global Macro ****/
#define WM_SET_TRANSFER_SAFE    (WM_USER+300) // Makes compose window to encode text in transfer safe form.


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/
    //Common.cpp
extern void ErrorMessage(HWND hWnd, UINT uID); //エラーメッセージを表示する
extern void FatalErrorMessage(HWND hWnd, const char* fileName, int lineno, const char* pMsg=NULL); //致命的エラーメッセージを表示する
extern std::string& GetLegalFileName(const char* name, std::string& fileName); //ファイル名として利用可能な名前に変換する
extern char* FileToString(const std::string& fileName); //テキストファイル内容を文字列情報としてメモリにロードする
extern void GetNameAndAddr(const char* src, std::string& name, std::string& email); //「name <email>」形式の文字列を要素に分解する
extern bool IsFileExist(const std::string& path); //ファイルの有無を問いあわせる
extern void RemoveFiles(const std::string& path); //ファイル削除 (ワイルドカード有効)
extern std::string& TrimSpace(std::string& element); //前後の空白文字を取り除く


#endif // _BKGNUPGDEF_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
