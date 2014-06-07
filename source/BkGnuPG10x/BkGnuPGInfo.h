/************************************************************************/
/*  File name : BkGnuPGInfo.h                                           */
/************************************************************************/
/*  Contents  : BkGnuPG 制御情報アクセス                                */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _BKGNUPGINFO_H_
#define _BKGNUPGINFO_H_

/**** Incude Files ****/
#include <windows.h>
#include "../BeckyAPI.h"
#include "GnuPGModule.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CBkGnuPGInfo                                            */
/* Purpose    : BkGnuPG 制御情報アクセスクラス                          */
/*               ※ このクラスでは内部変数を全て公開し双方向アクセス可能*/
/*                  にする                                              */
/*----------------------------------------------------------------------*/
class CBkGnuPGInfo
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
public:
    //定数定義
    enum MENUKIND {
        Setup = 0,      //設定
        Encrypt,        //暗号化
        Sign,           //署名
        EncryptSign,    //暗号化＆署名
        ExpPubKey,      //公開鍵のエクスポート
        Decript,        //復号化(含検証&インポート)
        MenuMax
    };
    //変数
    //システム変数(インスタンス)
    static CBeckyAPI m_BkApi; //Becky! API インタフェースクラス (You can have only one instance in a project.)
    static bool m_InitFlag; //初期化完了フラグ
    CGnuPGModule m_GPG; //GnuPGアクセスクラス
    UINT m_MainMenuID[MenuMax];
    UINT m_LVMenuID[MenuMax];
    UINT m_CompMenuID[MenuMax];
    //環境変数
    HINSTANCE m_hInstance;
    std::string m_BaseDir;
    std::string m_DataDir; //データフォルダ
    std::string m_TempDir;
    std::string m_IniPath;
    bool m_bEnableCcUser; //暗号化時の公開鍵IDにCcアドレスを含める
    bool m_bEnableBccUser; //暗号化時の公開鍵IDにBccアドレスを含める
    bool m_bEnableFromUser; //暗号化時の公開鍵IDにFromアドレスを含める
    bool m_bEnableDefUser; //暗号化時の公開鍵IDにデフォルトユーザを含める
    //PassPhrase ダイアログ設定用
    std::string m_UserID;
    std::string m_PassPhrase;
    bool m_bShowText; //パスフレーズ表示
    bool m_bUseAgent; // Use GPG Agent
    //Viewer ダイアログ設定用
    CGnuPGModule::CHARSET m_CharSet;
    //デバッグ用
    bool m_bDebugLog; //デバッグ用のログを出力する
    std::string m_LogPath;

// メンバメソッド
private:
    //内部処理関数
    void Copy(const CBkGnuPGInfo& org); //インスタンスのコピー
    void SetFilePath(const char* modulePath); //モジュールへのパスを設定する
    CGnuPGModule::CHARSET GetCharSet(const char* charset); //キャラクタセットの識別
public:
    //コンストラクタ・デストラクタ
    CBkGnuPGInfo(); //デフォルトコンストラクタ
    CBkGnuPGInfo(const CBkGnuPGInfo& org) { Copy(org); } //コピーコンストラクタ
    virtual ~CBkGnuPGInfo(); //デストラクタ
    //演算子
    CBkGnuPGInfo& operator=(const CBkGnuPGInfo& org) { Copy(org); return *this; } //代入演算子
    //インタフェース関数
    bool InitInstance(HINSTANCE hModule); //情報初期化
    void SetTempPath(const char* tempPath); //テンポラリディレクトリを設定する
    void GetEnv(void); //環境変数の取得
    void SetEnv(void); //環境変数の設定
    void SetUseAgentFlag(void); //環境変数 (UseAgent) の設定
};

/**** External Declarelation ****/
extern CBkGnuPGInfo g_Info; //BkGnuPG 制御情報クラス 実装


#endif // _BKGNUPGINFO_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
