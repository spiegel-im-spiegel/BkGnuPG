/************************************************************************/
/*  File name : ViewDialog.h                                            */
/************************************************************************/
/*  Contents  : 結果出力ダイアログ                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _VIEWDIALOG_H_
#define _VIEWDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CViewDialog                                             */
/* Purpose    : 結果出力ダイアログクラス                                */
/*----------------------------------------------------------------------*/
class CViewDialog
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
    std::string m_Message;
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CViewDialog(const CViewDialog& org); //コピーコンストラクタ
    CViewDialog& operator=(const CViewDialog& org); //代入演算子
    //内部処理関数
public:
    //コンストラクタ・デストラクタ
    CViewDialog(); //デフォルトコンストラクタ
    virtual ~CViewDialog() {} //デストラクタ
    //演算子
    //インタフェース関数
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, const char* outputMsg); //ダイアログの起動
    inline const char* GetOutputMsg(void) { return m_Message.c_str(); }
};


#endif // _VIEWDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
