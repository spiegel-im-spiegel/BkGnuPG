/************************************************************************/
/*  File name : SetupDialog.h                                           */
/************************************************************************/
/*  Contents  : Setting ダイアログ                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _SETUPDIALOG_H_
#define _SETUPDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CSetupDialog                                            */
/* Purpose    : Setting ダイアログクラス                                */
/*----------------------------------------------------------------------*/
class CSetupDialog
{
// メンバインスタンス
private:
    //定数定義
    //内部変数(static)
    static CSetupDialog* m_Instance; //このクラスのインスタンス
    //内部変数
    std::string m_CmdPath;
    std::string m_DefUserID; //デフォルトユーザID
    bool m_bPGPMIME; //PGP/MIME対応
    bool m_bPGPCompatible; //PGP互換の署名・暗号化を行う (m_bOpenPGP, m_ThrowKey は無効になる)
    bool m_bOpenPGP; //OpenPGPに準拠した署名暗号化を行う (m_bPGPCompatible 優先)
    bool m_bThrowKeyID; //受信者を匿名にする (m_bPGPMIME==true では無効)
    bool m_bRFC1991; //RFC1991に従う (m_bPGPCompatible==true でのみ有効)
    bool m_bMD5; //メッセージダイジェストにMD5を使用する
    bool m_bEnableCcUser; //暗号化時の公開鍵IDにCcアドレスを含める
    bool m_bEnableBccUser; //暗号化時の公開鍵IDにBccアドレスを含める
    bool m_bEnableFromUser; //暗号化時の公開鍵IDにFromアドレスを含める
    bool m_bEnableDefUser; //暗号化時の公開鍵IDにデフォルトユーザを含める
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CSetupDialog(const CSetupDialog& org); //コピーコンストラクタ
    CSetupDialog& operator=(const CSetupDialog& org); //代入演算子
    //内部処理関数
public:
    //コンストラクタ・デストラクタ
    CSetupDialog(); //デフォルトコンストラクタ
    virtual ~CSetupDialog() {} //デストラクタ
    //演算子
    //インタフェース関数
    static CSetupDialog* GetInstatnce(void) { return m_Instance; } //現在起動中のインスタンスを返す
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info); //ダイアログの起動
    inline const char* GetCmdPath(void) { return m_CmdPath.c_str(); } //GnuPG モジュールへのパス
    inline void SetCmdPath(const char* cmdPath) { m_CmdPath = cmdPath; }
    inline const char* GetDefUserID(void) { return m_DefUserID.c_str(); } //デフォルトユーザID
    inline void SetDefUserID(const char* defUserID) { m_DefUserID = defUserID; }
    inline bool IsPGPMIME(void) { return m_bPGPMIME; } //PGP/MIME 対応
    inline void SetOptPGPMIME(bool bPGPMIME) { m_bPGPMIME = bPGPMIME; }
    inline bool IsPGPCompatible(void) { return m_bPGPCompatible; } //PGP互換
    inline void SetOptPGPCompatible(bool bPGPCompatible) { m_bPGPCompatible = bPGPCompatible; }
    inline bool IsOpenPGP(void) { return m_bOpenPGP; } //OpenPGP互換
    inline void SetOptOpenPGP(bool bOpenPGP) { m_bOpenPGP = bOpenPGP; }
    inline bool IsThrowKeyID(void) { return m_bThrowKeyID; } //受信者を匿名にする
    inline void SetOptThrowKeyID(bool bThrowKeyID) { m_bThrowKeyID = bThrowKeyID; }
    inline bool IsRFC1991(void) const { return m_bRFC1991; }  //RFC1991に従う?
    inline void SetOptRFC1991(const bool bRFC1991) { m_bRFC1991 = bRFC1991; return; } //RFC1991条件設定
    inline bool IsMD5(void) const { return m_bMD5; }  //メッセージダイジェストにMD5を使用する
    inline void SetOptMD5(const bool bMD5) { m_bMD5 = bMD5; return; } //MD5条件設定
    inline bool IsEnableCcUser(void) { return m_bEnableCcUser; } //暗号化時の公開鍵IDにCcアドレスを含める
    inline void SetOptEnableCcUser(bool bEnableCcUser) { m_bEnableCcUser = bEnableCcUser; }
    inline bool IsEnableBccUser(void) { return m_bEnableBccUser; } //暗号化時の公開鍵IDにBccアドレスを含める
    inline void SetOptEnableBccUser(bool bEnableBccUser) { m_bEnableBccUser = bEnableBccUser; }
    inline bool IsEnableFromUser(void) { return m_bEnableFromUser; } //暗号化時の公開鍵IDにFromアドレスを含める
    inline void SetOptEnableFromUser(bool bEnableFromUser) { m_bEnableFromUser = bEnableFromUser; }
    inline bool IsEnableDefUser(void) { return m_bEnableDefUser; } //暗号化時の公開鍵IDにデフォルトユーザを含める
    inline void SetOptEnableDefUser(bool bEnableDefUser) { m_bEnableDefUser = bEnableDefUser; }
};


#endif // _SETUPDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
