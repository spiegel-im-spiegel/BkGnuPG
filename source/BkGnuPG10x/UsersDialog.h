/************************************************************************/
/*  File name : UsersDialog.h                                           */
/************************************************************************/
/*  Contents  : リモートユーザ設定ダイアログ                            */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.12          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _USERSDIALOG_H_
#define _USERSDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CUsersDialog                                            */
/* Purpose    : リモートユーザ設定ダイアログクラス                      */
/*----------------------------------------------------------------------*/
class CUsersDialog
{
// メンバインスタンス
private:
    //定数定義
    //内部変数(static)
    static CUsersDialog* m_Instance; //このクラスのインスタンス
    //内部変数
    CBkGnuPGInfo* m_Info;
    std::string m_UserID;
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CUsersDialog(const CUsersDialog& org); //コピーコンストラクタ
    CUsersDialog& operator=(const CUsersDialog& org); //代入演算子
    //内部処理関数
public:
    //コンストラクタ・デストラクタ
    CUsersDialog(); //デフォルトコンストラクタ
    virtual ~CUsersDialog() {} //デストラクタ
    //演算子
    //インタフェース関数
    static CUsersDialog* GetInstatnce(void) { return m_Instance; } //現在起動中のインスタンスを返す
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info); //ダイアログの起動
    inline const char* GetUserID(void) { return m_UserID.c_str(); } // User ID
    inline void SetUserID(const char* userID) { m_UserID = userID; }
    inline CBkGnuPGInfo* GetBkGnuPGInfo(void) { return m_Info; }
};


#endif // _USERSDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
