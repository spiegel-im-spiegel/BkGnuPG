/************************************************************************/
/*  File name : GnuPGModule.h                                           */
/************************************************************************/
/*  Contents  : GnuPG モジュールをラッピングする                        */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.21    2000.10.04          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _GNUPGMODULE_H_
#define _GNUPGMODULE_H_

/**** Incude Files ****/
#include <windows.h>

#include <string>
#include <vector>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CGnuPGModule                                            */
/* Purpose    : GnuPG モジュールをラッピングするクラス                  */
/*----------------------------------------------------------------------*/
class CGnuPGModule
{
// メンバインスタンス
private:
    //定数定義
    static const DWORD m_PipeSize; //パイプのバッファサイズ
    static const char* const m_DefaultModuleName; //デフォルトのモジュール名
    static const char* const m_DefaultOption; //デフォルトオプション
    static const char* const m_SignPGPOption; //署名処理用PGP対応オプション
    static const char* const m_OpenPGPOption; //OpenPGP対応オプション
    static const char* const m_ThrowKeyIDOption; //匿名受信者対応オプション
    static const char* const m_RFC1991Option; //クリア署名処理用PGP対応オプション
    static const char* const m_MdMD5Option; //メッセージダイジェストにMD5を使うオプション
    static const char* const m_MdSHA1Option; //メッセージダイジェストにSHA-1を使うオプション
    //内部変数
    //入出力リダイレクト
    HANDLE m_hStdoutRead;       //標準出力読み込みパイプ
    HANDLE m_hStdoutWrite;      //標準出力書き込みパイプ
    HANDLE m_hStderrRead;       //標準エラー出力読み込みパイプ
    HANDLE m_hStderrWrite;      //標準エラー出力書き込みパイプ
    HANDLE m_hStdinRead;        //標準入力読み込みパイプ
    HANDLE m_hStdinWrite;       //標準入力書き込みパイプ
    //動作用各種変数
    HANDLE m_hProcess;          //プロセスへのハンドル
    std::string m_ModuleName;   //モジュール名
    std::string m_OptionStr;    //起動オプション文字列
    std::string m_CommandStr;   //起動コマンド文字列
    std::string m_StdoutBuffer; //標準出力バッファ (表示用)
    std::string m_StderrBuffer; //標準エラー出力バッファ (表示用)
    //オプション
    std::string m_DefUserID; //デフォルトユーザID
    std::string m_MDalgo; //メッセージダイジェストアルゴリズム名
    bool m_bPGPMIME; //PGP/MIME対応
    bool m_bPGPCompatible; //PGP互換の署名・暗号化を行う (m_bOpenPGP, m_ThrowKey は無効になる)
    bool m_bOpenPGP; //OpenPGPに準拠した署名暗号化を行う (m_bPGPCompatible 優先)
    bool m_bThrowKeyID; //受信者を匿名にする (m_bPGPMIME==true では無効)
    bool m_bRFC1991; //RFC1991に従う (m_bPGPCompatible==true でのみ有効)
    bool m_bMD5; //メッセージダイジェストにMD5を使用する
    bool m_bTextModeSig; //分離署名の際にテキストモードで署名する
public:
    //定数定義
    enum EMODE {    // 動作モード
        Null = 0,       //未定義
        Encrypt,        //暗号化
        Sign,           //署名
        EncryptSign,    //暗号化＆署名
        ExpPubKey,      //公開鍵のエクスポート
        Decript,        //復号化
        Verify,         //署名の検証
        ImpPubKey       //公開鍵のインポート
    };
    enum CHARSET {  //GnuPG出力キャラクタセット
        DEFAULT = 0,    //デフォルト(Shift-JIS)
        ISO_2022_IP,    //iso-2022-jp (JIS)
        EUC_JP,         // EUC-JP
        UTF_8           // UTF-8
    };

// メンバメソッド
private:
    //内部処理関数
    void Copy(const CGnuPGModule& org); //インスタンスのコピー
    bool Execute(void); //プロセスを起動する
    std::string& Quotation(std::string& str); //文字列が空白文字を含む場合はダブルクォーテションで囲む
    std::string& Dequotation(std::string& str); //ダブルクォーテションの囲みを外す
    void Terminate(void); //プロセスを終了する
    bool IsActive(DWORD tout=0); //プロセスは稼働中か?
    void WaitforTerminate(void); //プロセスが終了するまで待つ
    bool Write(std::string& inpMsg); //標準入力への入力
    bool Read(std::string& outpMsg, bool bBlock); //標準出力からのメッセージ
public:
    //コンストラクタ・デストラクタ
    CGnuPGModule(const char* moduleName=NULL); //デフォルトコンストラクタ
    CGnuPGModule(const CGnuPGModule& org) { Copy(org); } //コピーコンストラクタ
    virtual ~CGnuPGModule() { Terminate(); } //デストラクタ
    //演算子
    CGnuPGModule& operator=(const CGnuPGModule& org) { Copy(org); return *this; } //代入演算子
    //インタフェース関数
    bool ExecEncrypt(const char* ascPath, const char* txtPath, std::vector<std::string>& addrList); //暗号化処理
    bool ExecSign(const char* ascPath, const char* txtPath, const char* userID, const char* passPhrase, bool UseAgent); //署名処理
    bool ExecEncryptSign(const char* ascPath, const char* txtPath, const char* userID, std::vector<std::string>& addrList, const char* passPhrase, bool UseAgent); //暗号化と署名処理
    bool ExecDecrypt(const char* ascPath, const char* txtPath, const char* passPhrase, bool UseAgent); //復号化処理
    bool ExecVerify(const char* ascPath, const char* txtPath); //署名の検証
    bool ExecImport(const char* ascPath); //公開鍵のインポート
    bool ExecExport(const char* ascPath, const char* userID); //公開鍵のエクスポート
    inline const char* GetOutputMsg(void) { return m_StderrBuffer.c_str(); }
    inline const char* GetStatusMsg(void) { return m_StdoutBuffer.c_str(); }
    inline static const char* GetDefModuleName(void) { return m_DefaultModuleName; } //モジュール名を取得する
    inline const char* GetModuleName(void) const { return m_ModuleName.c_str(); } //モジュール名を取得する
    inline void SetModuleName(const char* moduleName) { m_ModuleName = moduleName; Quotation(m_ModuleName); return; } //モジュール名を設定する
    inline const char* GetDefUserID(void) const { return m_DefUserID.c_str(); } //デフォルトユーザIDを取得する
    inline void SetDefUserID(const char* userID) { m_DefUserID = userID; return; } //デフォルトユーザIDを設定する
    inline bool IsPGPMIME(void) const { return m_bPGPMIME; } //PGP/MIME 対応か?
    inline void SetOptPGPMIME(const bool bPGPMIME) { m_bPGPMIME = bPGPMIME; return; } //PGP/MIME 対応条件設定
    inline bool IsPGPCompatible(void) const { return m_bPGPCompatible; }  //PGP互換?
    inline void SetOptPGPCompatible(const bool bPGPCompatible) { m_bPGPCompatible = bPGPCompatible; return; } //PGP互換条件設定
    inline bool IsOpenPGP(void) const { return m_bOpenPGP; }  //OpenPGP?
    inline void SetOptOpenPGP(const bool bOpenPGP) { m_bOpenPGP = bOpenPGP; return; } //OpenPGP条件設定
    inline bool IsThrowKeyID(void) const { return m_bThrowKeyID; }  //受信者と匿名にする?
    inline void SetOptThrowKeyID(const bool bThrowKeyID) { m_bThrowKeyID = bThrowKeyID; return; } //匿名受信者条件設定
    inline bool IsRFC1991(void) const { return m_bRFC1991; }  //RFC1991に従う?
    inline void SetOptRFC1991(const bool bRFC1991) { m_bRFC1991 = bRFC1991; return; } //RFC1991条件設定
    inline bool IsMD5(void) const { return m_bMD5; }  //メッセージダイジェストにMD5を使用する
    inline void SetOptMD5(const bool bMD5) { m_bMD5 = bMD5; return; } //MD5条件設定

    inline const char* IsMDalgo(void) const { return m_MDalgo.c_str(); }  //メッセージダイジェストアルゴリズム名
    inline void SetOptMDalgo(const char* strMDalgo) { m_MDalgo = strMDalgo; return; } //メッセージダイジェストアルゴリズム名設定
    const char* GetOptMDalgo(void); //メッセージダイジェストアルゴリズム名を取得する

    inline bool IsTextModeSig(void) const { return m_bTextModeSig; }  //分離署名の際にテキストモードで署名する
    inline void SetOptTextModeSig(const bool bTextModeSig) { m_bTextModeSig = bTextModeSig; return; } //分離署名条件設定
    //インタフェース関数 (static)
    static std::string GetGPGHomeDir(void); //GnuPGのホームディレクトリを取得する
    static std::string GetGPGModuleName(void); //GnuPGのプログラム名を取得する (by Canalian)

};


#endif // _GNUPGMODULE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
