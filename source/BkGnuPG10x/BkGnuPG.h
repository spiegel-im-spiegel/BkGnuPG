/************************************************************************/
/*  File name : BkGnuPG.h                                               */
/************************************************************************/
/*  Contents  : Becky! GNU Privacy Guard プラグイン インタフェース定義  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _BKGNUPG_H_
#define _BKGNUPG_H_

// 以下の ifdef ブロックは DLL から簡単にエクスポートさせるマクロを作成する標準的な方法です。
// この DLL 内のすべてのファイルはコマンドラインで定義された BKGNUPG_EXPORTS シンボル
// でコンパイルされます。このシンボルはこの DLL が使用するどのプロジェクト上でも未定義でなけ
// ればなりません。この方法ではソースファイルにこのファイルを含むすべてのプロジェクトが DLL
// からインポートされたものとして BKGNUPG_API 関数を参照し、そのためこの DLL はこのマク
// ロで定義されたシンボルをエクスポートされたものとして参照します。
#ifdef BKGNUPG_EXPORTS
#define BKGNUPG_API __declspec(dllexport)
#else
#define BKGNUPG_API __declspec(dllimport)
#endif


//エクスポート関数につては BkGnuPG.def ファイルを参照のこと

#endif // _BKGNUPG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
