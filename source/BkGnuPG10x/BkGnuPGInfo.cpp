/************************************************************************/
/*  File name : BkGnuPGInfo.cpp                                         */
/************************************************************************/
/*  Contents  : BkGnuPG 制御情報アクセスクラス                          */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGInfo.h"
#include "BkGnuPGDef.h"

#include <string>
#include <sstream>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Constants ****/


/**** Prototyping ****/
CBeckyAPI CBkGnuPGInfo::m_BkApi; //Becky! API インタフェースクラス (You can have only one instance in a project.)
bool CBkGnuPGInfo::m_InitFlag = false; //初期化完了フラグ


/*----------------------------------------------------------------------*/
/* Purpose        : デフォルトコンストラクタ                            */
/* Inputs         : なし                                                */
/*----------------------------------------------------------------------*/
CBkGnuPGInfo::CBkGnuPGInfo(void)
    : m_bEnableCcUser(false), m_bEnableBccUser(false), m_bEnableFromUser(false), m_bEnableDefUser(false), m_bShowText(false),
        m_CharSet(CGnuPGModule::DEFAULT), m_bDebugLog(false), m_bUseAgent(false)
{
    m_BaseDir = ".\\"; //取り敢えずカレントディレクトリにしておく
    m_DataDir = ".\\"; //取り敢えずカレントディレクトリにしておく
    m_TempDir = ".\\"; //取り敢えずカレントディレクトリにしておく
    m_IniPath = m_BaseDir + "BkGnuPG.ini";
    m_LogPath = m_BaseDir + "BkGnuPG.log";
    m_UserID = m_PassPhrase = "";
    for(int i=0; i<MenuMax; i++) { //Menu ID
        m_MainMenuID[i] = m_LVMenuID[i] = m_CompMenuID[i] = 0;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : デストラクタ                                        */
/* Inputs         : なし                                                */
/*----------------------------------------------------------------------*/
CBkGnuPGInfo::~CBkGnuPGInfo(void)
{
    //TODO
}

/*----------------------------------------------------------------------*/
/* Purpose        : インスタンスのコピー                                */
/* Inputs         : コピー元インスタンス                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::Copy(const CBkGnuPGInfo& org)
{
    m_GPG = org.m_GPG;
    m_hInstance = org.m_hInstance;
    m_BaseDir = org.m_BaseDir;
    m_DataDir = org.m_DataDir;
    m_TempDir = org.m_TempDir;
    m_IniPath = org.m_IniPath;
    m_LogPath = org.m_LogPath;
    m_bEnableCcUser = org.m_bEnableCcUser;
    m_bEnableBccUser = org.m_bEnableBccUser;
    m_bEnableFromUser = org.m_bEnableFromUser; //暗号化時の公開鍵IDにFromアドレスを含める
    m_bEnableDefUser = org.m_bEnableDefUser; //暗号化時の公開鍵IDにデフォルトユーザを含める
    m_bShowText = org.m_bShowText; //パスフレーズ表示
    m_bUseAgent = org.m_bUseAgent; // Use GPG Agent
    m_UserID = org.m_UserID;
    m_PassPhrase = org.m_PassPhrase;
    m_CharSet = org.m_CharSet;
    m_bDebugLog = org.m_bDebugLog;
    for(int i=0; i<MenuMax; i++) { //Menu ID
        m_MainMenuID[i] = org.m_MainMenuID[i];
        m_LVMenuID[i] = org.m_LVMenuID[i];
        m_CompMenuID[i] = org.m_CompMenuID[i];
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 情報初期化                                          */
/* Inputs         : モジュールインスタンスのハンドル                    */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
bool CBkGnuPGInfo::InitInstance(HINSTANCE hModule)
{
    if(m_InitFlag==true) { //既に初期化済み
        return true;
    }

    //Becky! API インタフェースクラスの初期化
    if (!m_BkApi.InitAPI()) {
        return false;
    }
    m_InitFlag = true;

    //パス情報の取得
    m_hInstance = hModule;
    char szPath[_MAX_PATH+2]; // Ini file to save your plugin settings.
    GetModuleFileName((HINSTANCE)hModule, szPath, _MAX_PATH);
    SetFilePath(szPath); //各種ファイルへのパスを設定する

    //環境変数の取得
    GetEnv(); //環境変数の取得

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 各種ファイルへのパスを設定する                      */
/* Inputs         : モジュールへのパス                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetFilePath(const char* modulePath)
{
    //起動ディレクトリ取得
    string path = modulePath;
    string::size_type len = path.find_last_of("\\");
    string fname;
    if(len!=string::npos) {
        m_BaseDir = path.substr(0, len+1);
        fname = path.substr(len+1);
    }
    else {
        m_BaseDir = ".\\";
        fname = path;
    }
    //ファイル名(ベース名)取得
    len = fname.find_last_of(".");
    if(len!=string::npos) {
        fname = fname.substr(0, len);
    }
    //データフォルダ取得
    m_DataDir = m_BkApi.GetDataFolder();
    len = m_DataDir.length();
    if(len>0 && m_DataDir[len-1]!='\\') {
        m_DataDir += '\\';
    }
    //GnuPGのホームディレクトリを取得する
    string gpgHome = CGnuPGModule::GetGPGHomeDir();
    len = gpgHome.length();
    if(len>0 && gpgHome[len-1]!='\\') {
        gpgHome += '\\';
    }

    //INIファイル検索
    if((IsFileExist(m_DataDir+"PlugIns\\"+fname+".ini"))==true) { //データフォルダにINIファイルがある
        m_IniPath = m_DataDir + "PlugIns\\";
    }
    else if((gpgHome.length())>0 && (IsFileExist(gpgHome+fname+".ini"))==true) { //GnuPGのホームディレクトリにINIファイルがある
        m_IniPath = gpgHome;
    }
    else { //起動ディレクトリにINIファイルを置く
        m_IniPath = m_BaseDir;
    }

    //ファイルパス設定
    m_IniPath += fname;
    m_LogPath = m_IniPath;
    m_IniPath += ".ini";
    m_LogPath += ".log";

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : テンポラリディレクトリを設定する                    */
/* Inputs         : テンポラリディレクトリ                              */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetTempPath(const char* tempPath)
{
    if(tempPath==NULL) {
        return;
    }
    int len = strlen(tempPath);
    if(len==0) {
        return;
    }
    m_TempDir = tempPath;
    if(tempPath[len-1]!='\\') {
        m_TempDir += "\\";
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 環境変数の取得                                      */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::GetEnv(void)
{
    //GPGコマンド名
    char path[_MAX_PATH+2];
    string gpgPath = CGnuPGModule::GetGPGModuleName();
    //GetPrivateProfileString("Settings", "CmdName", m_GPG.GetDefModuleName(), path, _MAX_PATH, m_IniPath.c_str());
    GetPrivateProfileString("Settings", "CmdName", gpgPath.c_str(), path, _MAX_PATH, m_IniPath.c_str());
    if((strlen(path))>0) {
        m_GPG.SetModuleName(path); //GnuPGモジュール名を設定する
    }
    else {
        //m_GPG.SetModuleName(m_GPG.GetDefModuleName()); //デフォルトのGnuPGモジュール名を設定する
        m_GPG.SetModuleName(gpgPath.c_str()); //レジストリで定義されたGnuPGへのパスを設定する
    }
    //デフォルトユーザID
    GetPrivateProfileString("Settings", "DefUserID", m_GPG.GetDefUserID(), path, _MAX_PATH, m_IniPath.c_str());
    m_GPG.SetDefUserID(path); //デフォルトユーザIDを設定する
    //メッセージダイジェストアルゴリズ名
    GetPrivateProfileString("Settings", "MDalgo", m_GPG.IsMDalgo(), path, _MAX_PATH, m_IniPath.c_str());
    m_GPG.SetOptMDalgo(path); //メッセージダイジェストアルゴリズ名を設定する
    //PGP/MIME
    int chk = m_GPG.IsPGPMIME() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "PGPMIME", chk, m_IniPath.c_str());
    m_GPG.SetOptPGPMIME(chk!=0);
    //PGP互換
    chk = m_GPG.IsPGPCompatible() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "PGPCompatible", chk, m_IniPath.c_str());
    m_GPG.SetOptPGPCompatible(chk!=0);
    //OpenPGP
    chk = m_GPG.IsOpenPGP() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "OpenPGP", chk, m_IniPath.c_str());
    m_GPG.SetOptOpenPGP(chk!=0);
    //匿名受信者
    chk = m_GPG.IsThrowKeyID() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "AnonymousRecipient", chk, m_IniPath.c_str());
    m_GPG.SetOptThrowKeyID(chk!=0);
    //RFC1991に従う
    chk = m_GPG.IsRFC1991() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "RFC1991", chk, m_IniPath.c_str());
    m_GPG.SetOptRFC1991(chk!=0);
    //メッセージダイジェストにMD5を使う
    chk = m_GPG.IsMD5() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "UseMD5", chk, m_IniPath.c_str());
    m_GPG.SetOptMD5(chk!=0);
    //暗号化時の公開鍵IDにCcアドレスを含める
    chk = m_bEnableCcUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableCcUser", chk, m_IniPath.c_str());
    m_bEnableCcUser = (chk!=0);
    //暗号化時の公開鍵IDにBccアドレスを含める
    chk = m_bEnableBccUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableBccUser", chk, m_IniPath.c_str());
    m_bEnableBccUser = (chk!=0);
    //暗号化時の公開鍵IDにFromアドレスを含める
    chk = m_bEnableFromUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableFromUser", chk, m_IniPath.c_str());
    m_bEnableFromUser = (chk!=0);
    //暗号化時の公開鍵IDにデフォルトユーザを含める
    chk = m_bEnableDefUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableDefUser", chk, m_IniPath.c_str());
    m_bEnableDefUser = (chk!=0);
    //分離署名の際にテキストモードで署名する
    chk = m_GPG.IsTextModeSig() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableTextModeSig", chk, m_IniPath.c_str());
    m_GPG.SetOptTextModeSig(chk!=0);
    //パスフレーズ表示
    chk = m_bShowText ? 1:0;
    chk = GetPrivateProfileInt("Settings", "ShowPassphrase", chk, m_IniPath.c_str());
    m_bShowText = (chk!=0);
    // Agent settings
    chk = m_bUseAgent ? 1:0;
    chk = GetPrivateProfileInt("Settings", "UseAgent", chk, m_IniPath.c_str());
    m_bUseAgent = (chk != 0);
    //GnuPG出力
    GetPrivateProfileString("Output", "Charset", "", path, _MAX_PATH, m_IniPath.c_str());
    m_CharSet = GetCharSet(path);
    //ログ情報出力
    chk = m_bDebugLog ? 1:0;
    chk = GetPrivateProfileInt("Debug", "LogFile", chk, m_IniPath.c_str());
    m_bDebugLog = (chk!=0);

}

/*----------------------------------------------------------------------*/
/* Purpose        : 環境変数の設定                                      */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetEnv(void)
{
    //GPGコマンド名
    //WritePrivateProfileString("Settings", "CmdName", m_GPG.GetModuleName(), m_IniPath.c_str());
    //デフォルトユーザID
    WritePrivateProfileString("Settings", "DefUserID", m_GPG.GetDefUserID(), m_IniPath.c_str());
    //PGP/MIME
    char* s = m_GPG.IsPGPMIME() ? "1":"0";
    WritePrivateProfileString("Settings", "PGPMIME", s, m_IniPath.c_str());
    //PGP互換
    s = m_GPG.IsPGPCompatible() ? "1":"0";
    WritePrivateProfileString("Settings", "PGPCompatible", s, m_IniPath.c_str());
    //OpenPGP
    s = m_GPG.IsOpenPGP() ? "1":"0";
    WritePrivateProfileString("Settings", "OpenPGP", s, m_IniPath.c_str());
    //匿名受信者
    s = m_GPG.IsThrowKeyID() ? "1":"0";
    WritePrivateProfileString("Settings", "AnonymousRecipient", s, m_IniPath.c_str());
    //RFC1991に従う
    s = m_GPG.IsRFC1991() ? "1":"0";
    WritePrivateProfileString("Settings", "RFC1991", s, m_IniPath.c_str());
    //メッセージダイジェストにMD5を使う
    s = m_GPG.IsMD5() ? "1":"0";
    WritePrivateProfileString("Settings", "UseMD5", s, m_IniPath.c_str());
    //Use GPG Agent
    SetUseAgentFlag();
    //暗号化時の公開鍵IDにCcアドレスを含める
    s = m_bEnableCcUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableCcUser", s, m_IniPath.c_str());
    //暗号化時の公開鍵IDにBccアドレスを含める
    s = m_bEnableBccUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableBccUser", s, m_IniPath.c_str());
    //暗号化時の公開鍵IDにFromアドレスを含める
    s = m_bEnableFromUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableFromUser", s, m_IniPath.c_str());
    //暗号化時の公開鍵IDにデフォルトユーザを含める
    s = m_bEnableDefUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableDefUser", s, m_IniPath.c_str());
}

/*----------------------------------------------------------------------*/
/* Purpose        : 環境変数 (UseAgent) の設定                          */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetUseAgentFlag(void)
{
    //Use GPG Agent
    char* s = m_bUseAgent ? "1":"0";
    WritePrivateProfileString("Settings", "UseAgent", s, m_IniPath.c_str());
}

/*----------------------------------------------------------------------*/
/* Purpose        : キャラクタセットの識別                              */
/* Inputs         : キャラクタセットを示す文字列                        */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
CGnuPGModule::CHARSET CBkGnuPGInfo::GetCharSet(const char* charset)
{
    if(charset==NULL || *charset=='\0') {
        return CGnuPGModule::DEFAULT;
    }

    if((stricmp(charset, "iso-2022-jp"))==0) {
        return CGnuPGModule::ISO_2022_IP;
    }
    else if((stricmp(charset, "euc-jp"))==0) {
        return CGnuPGModule::EUC_JP;
    }
    else if((stricmp(charset, "utf-8"))==0) {
        return CGnuPGModule::UTF_8;
    }
    else {
        return CGnuPGModule::DEFAULT;
    }
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
