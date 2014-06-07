/************************************************************************/
/*  File name : PassDialog.h                                            */
/************************************************************************/
/*  Contents  : PassPhrase ダイアログ                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _PASSDIALOG_H_
#define _PASSDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CPassPhraseDialog                                       */
/* Purpose    : PassPhrase ダイアログクラス                             */
/*----------------------------------------------------------------------*/
class CPassPhraseDialog
{
// メンバインスタンス
private:
    //定数定義
    //内部変数(static)
    static CPassPhraseDialog* m_Instance; //このクラスのインスタンス
    //内部変数
    CBkGnuPGInfo* m_Info;
    std::string m_UserID;
    std::string m_PassPhrase;
    BOOL m_bFixed; //ユーザID入力ボックスの状態
    bool m_bShowText; //パスフレーズ表示
    bool m_bUseAgent; // Use GPG Agent
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CPassPhraseDialog(const CPassPhraseDialog& org); //コピーコンストラクタ
    CPassPhraseDialog& operator=(const CPassPhraseDialog& org); //代入演算子
    //内部処理関数
public:
    //コンストラクタ・デストラクタ
    CPassPhraseDialog(); //デフォルトコンストラクタ
    virtual ~CPassPhraseDialog() {} //デストラクタ
    //演算子
    //インタフェース関数
    static CPassPhraseDialog* GetInstatnce(void) { return m_Instance; } //現在起動中のインスタンスを返す
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, bool bFixedID); //ダイアログの起動
    inline const BOOL IsFixed(void) const { return m_bFixed; } //ユーザID入力ボックスの状態を返す
    inline const bool IsShowText(void) const { return m_bShowText; } //パスフレーズ表示の状態を返す
    inline const bool UseAgent(void) const { return m_bUseAgent; }
    inline void SetOptShowText(bool bShowText) { m_bShowText = bShowText; } //パスフレーズ表示
    inline const char* GetUserID(void) const { return m_UserID.c_str(); } // User ID
    inline void SetUserID(const char* userID) { m_UserID = userID; }
    inline const char* GetPassPhrase(void) const { return m_PassPhrase.c_str(); } //PassPhrase
    inline void SetPassPhrase(const char* passPhrase) { m_PassPhrase = passPhrase; }
    inline void SetUseAgent(bool value) { m_bUseAgent = value; }
    inline CBkGnuPGInfo* GetBkGnuPGInfo(void) { return m_Info; }
};


#endif // _PASSDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
