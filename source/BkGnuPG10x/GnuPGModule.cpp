/************************************************************************/
/*  File name : GnuPGModule.cpp                                         */
/************************************************************************/
/*  Contents  : gpg.exe Control Class                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.31    2000.10.19          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "GnuPGModule.h"
#include "LogFile.h"

#include <string>
#include <fstream>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Constants ****/
const DWORD CGnuPGModule::m_PipeSize = 32767; //パイプのバッファサイズ
const char* const CGnuPGModule::m_DefaultModuleName = "gpg.exe";  //デフォルトのモジュール名
const char* const CGnuPGModule::m_DefaultOption = "--logger-fd 2 --batch --yes --verbose";  //デフォルトオプション
    //   --logger-fd 2: ログの出力を標準エラー出力にする
    //         --batch: バッチモードで処理する
    //           --yes: 問い合わせに対して全て「Yes」で自動処理する
    //       --verbose: 詳細表示
const char* const CGnuPGModule::m_SignPGPOption = "--force-v3-sigs"; //署名処理用PGP対応オプション
    // --force-v3-sigs: Version 3 で署名する (gpg のデフォルトは Version 4)
const char* const CGnuPGModule::m_OpenPGPOption = "--openpgp"; //OpenPGP対応オプション
const char* const CGnuPGModule::m_ThrowKeyIDOption = "--throw-keyid"; //匿名受信者対応オプション
const char* const CGnuPGModule::m_RFC1991Option = "--rfc1991"; //RFC1991準拠オプション
const char* const CGnuPGModule::m_MdMD5Option = "--digest-algo MD5"; //メッセージダイジェストにMD5を使うオプション
const char* const CGnuPGModule::m_MdSHA1Option = "--digest-algo SHA1"; //メッセージダイジェストにSHA-1を使うオプション


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : デフォルトコンストラクタ                            */
/* Inputs         : モジュールへのパス                                  */
/*----------------------------------------------------------------------*/
CGnuPGModule::CGnuPGModule(const char* moduleName)
    : m_hStdoutRead(NULL), m_hStdoutWrite(NULL),
        m_hStderrRead(NULL), m_hStderrWrite(NULL),
        m_hStdinRead(NULL), m_hStdinWrite(NULL),
        m_hProcess(NULL),
        m_bPGPMIME(true), m_bPGPCompatible(true),
        m_bOpenPGP(false), m_bThrowKeyID(false),
        m_bRFC1991(false), m_bMD5(false),
        m_bTextModeSig(false)
{
    if(moduleName==NULL) {
        m_ModuleName = m_DefaultModuleName;
    }
    else {
        m_ModuleName = moduleName;
    }
    m_DefUserID = ""; //デフォルトユーザID
    m_MDalgo = "SHA1"; //デフォルトのメッセージダイジェストアルゴリズム名
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化処理                                          */
/* Inputs         : 暗号化したメッセージを格納するファイルへのパス      */
/*                  暗号対象メッセージを格納したファイルへのパス        */
/*                  メールアドレス(=UserID)リスト                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecEncrypt(const char* ascPath, const char* txtPath, vector<string>& addrList)
{
    //初期処理
    if(ascPath==NULL || txtPath==NULL || (addrList.size())==0) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "-ea ";
    m_CommandStr += Quotation(string(txtPath));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット
    if(m_bPGPCompatible==true) {
        if(m_bRFC1991==true) {
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath));
    for(vector<string>::iterator p=addrList.begin(); p!=addrList.end(); p++) {
        m_OptionStr += " -r ";
        m_OptionStr += Quotation(*p);
    }

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 署名処理                                            */
/* Inputs         : 署名ブロックを格納するファイルへのパス              */
/*                  署名対象メッセージを格納したファイルへのパス        */
/*                  署名に使う User ID                                  */
/*                  パスフレーズ                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecSign(const char* ascPath, const char* txtPath, const char* userID, const char* passPhrase, bool UseAgent)
{
    //初期処理
    if(ascPath==NULL || txtPath==NULL || passPhrase==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    if(m_bPGPMIME==true) {
        m_CommandStr = "-a "; //ASCII 出力
        if(m_bTextModeSig==true) { //分離署名の際にテキストモードで署名する
            m_CommandStr += "--textmode ";
        }
        m_CommandStr += "--detach-sign "; //署名パートを分離する
    }
    else {
        m_CommandStr = "--clearsign "; //クリア署名
    }
    m_CommandStr += Quotation(string(txtPath));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP互換のためのオプション
        if(m_bRFC1991==true) { //RFC1991に従う
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //署名アルゴリズム(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //署名アルゴリズム(SHA-1)
    //}
    m_OptionStr += " --digest-algo ";
    m_OptionStr += GetOptMDalgo();
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0"; //パスフレーズの入力を標準入力で行う
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent";
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)) + " "; //署名ブロックの出力先
    if(userID!=NULL) {
        m_OptionStr += "-u "; //ユーザIDの指定
        m_OptionStr += Quotation(string(userID)) + " ";
    }
    else if((m_DefUserID.length())>0) {
        m_OptionStr += "--default-key "; //デフォルトユーザIDの指定
        m_OptionStr += Quotation(m_DefUserID) + " ";
    }

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    if (UseAgent==false) { // not Use GPG Agent
        Sleep(10); //起動後10msほど待ってみる
        //パスフレーズ入力
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化と署名処理                                    */
/* Inputs         : 署名・暗号化ブロックを格納するファイルへのパス      */
/*                  署名・暗号化対象メッセージを格納したファイルへのパス*/
/*                  署名に使う User ID                                  */
/*                  暗号に使うメールアドレス(=UserID)リスト             */
/*                  パスフレーズ                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecEncryptSign(const char* ascPath, const char* txtPath, const char* userID, vector<string>& addrList, const char* passPhrase, bool UseAgent)
{
    //初期処理
    if(ascPath==NULL || txtPath==NULL || (addrList.size())==0 || passPhrase==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "-esa "; //暗号化 & 署名
    m_CommandStr += Quotation(string(txtPath));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP互換のためのオプション
        if(m_bRFC1991==true) { //RFC1991に従う
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //署名アルゴリズム(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //署名アルゴリズム(SHA-1)
    //}
    m_OptionStr += " --digest-algo ";
    m_OptionStr += GetOptMDalgo();
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0"; //パスフレーズの入力を標準入力で行う
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent";
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)) + " "; //署名・暗号化パケットの出力先
    if(userID!=NULL) {
        m_OptionStr += "-u "; //ユーザIDの指定
        m_OptionStr += Quotation(string(userID));
    }
    else if((m_DefUserID.length())>0) {
        m_OptionStr += "--default-key "; //デフォルトユーザIDの指定
        m_OptionStr += Quotation(m_DefUserID);
    }
    for(vector<string>::iterator p=addrList.begin(); p!=addrList.end(); p++) {
        m_OptionStr += " -r ";
        m_OptionStr += Quotation(*p);
    }

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    if(UseAgent==false) { // not Use GPG Agent
        Sleep(10); //起動後10msほど待ってみる
        //パスフレーズ入力
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 公開鍵のエクスポート                                */
/* Inputs         : 公開鍵を格納するファイルへのパス                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecExport(const char* ascPath, const char* userID)
{
    //初期処理
    if(ascPath==NULL || userID==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "-a --export ";
    m_CommandStr += Quotation(string(userID));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP互換のためのオプション
        if(m_bRFC1991==true) { //RFC1991に従う
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //署名アルゴリズム(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //署名アルゴリズム(SHA-1)
    //}
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)); //復号化したメッセージの出力先

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 復号化処理                                          */
/* Inputs         : 暗号化メッセージを格納したファイルへのパス          */
/*                  復号化メッセージを格納するファイルへのパス          */
/*                  パスフレーズ                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecDecrypt(const char* ascPath, const char* txtPath, const char* passPhrase, bool UseAgent)
{
    //初期処理
    if(ascPath==NULL || txtPath==NULL || passPhrase==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "-d ";
    m_CommandStr += Quotation(string(ascPath));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0 "; //パスフレーズの入力を標準入力で行う
        if((m_DefUserID.length())>0) {
            m_OptionStr += "--default-key "; //デフォルトユーザIDの指定
            m_OptionStr += Quotation(m_DefUserID) + " ";
        }
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent ";
    }
    m_OptionStr += "-o ";
    m_OptionStr += Quotation(string(txtPath)) + " "; //復号化したメッセージの出力先

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    if(UseAgent==false) { // not Use GPG Agent
        Sleep(10); //起動後10msほど待ってみる
        //パスフレーズ入力
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 署名の検証                                          */
/* Inputs         : 署名を格納したファイルへのパス                      */
/*                  被署名メッセージを格納したファイルへのパス          */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecVerify(const char* ascPath, const char* txtPath)
{
    //初期処理
    if(ascPath==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "--verify ";
    m_CommandStr += Quotation(string(ascPath));
    if(txtPath!=NULL) {
        m_CommandStr += " ";
        m_CommandStr += Quotation(string(txtPath));
    }
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 公開鍵のインポート                                  */
/* Inputs         : 公開鍵を格納したファイルへのパス                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecImport(const char* ascPath)
{
    //初期処理
    if(ascPath==NULL) { //必須パラメータがない
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //標準出力バッファをクリア

    //コマンド設定
    m_CommandStr = "--import ";
    m_CommandStr += Quotation(string(ascPath));
    //オプション設定
    m_OptionStr = m_DefaultOption; //デフォルトのオプションをセット

    //コマンド起動
    if((Execute())==false) {
        return false; //起動失敗
    }

    //プロセス終了まで待つ
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : GnuPGのホームディレクトリを取得する                 */
/* Inputs         : なし                                                */
/* Ootput         : GnuPGのホームディレクトリ                           */
/*----------------------------------------------------------------------*/
string CGnuPGModule::GetGPGHomeDir(void)
{
    HKEY hKey;
    string dir = "";
    if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GNU\\GnuPG", 0, KEY_QUERY_VALUE, &hKey))!=ERROR_SUCCESS) { //キーがない
        return dir;
    }

    DWORD len;
    if((RegQueryValueEx(hKey, "HomeDir", NULL, NULL, NULL, &len))==ERROR_SUCCESS) {
        if(len>0) {
            len++;
            char* path = new char[len];
            if((RegQueryValueEx(hKey, "HomeDir", NULL, NULL, (LPBYTE)path, &len))==ERROR_SUCCESS) {
                dir = path;
                if(path[strlen(path)-1]!='\\') {
                    dir += "\\";
                }
            }
            delete [] path;
        }
    }
    RegCloseKey(hKey);

    return dir;
}

/*----------------------------------------------------------------------*/
/* Purpose        : GnuPGのプログラム名を取得する                       */
/* Inputs         : なし                                                */
/* Ootput         : GnuPGのプログラム名                                 */
/*                                                                      */
/* Auther         : Canalian    Version 1.0.4    2003.03.23             */
/*----------------------------------------------------------------------*/
string CGnuPGModule::GetGPGModuleName(void)
{
    HKEY hKey;
    string dir = m_DefaultModuleName;
    if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GNU\\GnuPG", 0, KEY_QUERY_VALUE, &hKey))!=ERROR_SUCCESS) { //キーがない
        return dir;
    }

    DWORD len;
    if((RegQueryValueEx(hKey, "gpgProgram", NULL, NULL, NULL, &len))==ERROR_SUCCESS) {
        if(len>0) {
            len++;
            char* path = new char[len];
            if((RegQueryValueEx(hKey, "gpgProgram", NULL, NULL, (LPBYTE)path, &len))==ERROR_SUCCESS) {
                dir = path;
            }
            delete [] path;
        }
    }
    RegCloseKey(hKey);

    return dir;
}

/*----------------------------------------------------------------------*/
/* Purpose        : メッセージダイジェストアルゴリズム名を取得する      */
/* Inputs         : なし                                                */
/* Ootput         : メッセージダイジェストアルゴリズム名                */
/*----------------------------------------------------------------------*/
const char* CGnuPGModule::GetOptMDalgo(void)
{
    if(m_bMD5==false) {
        return m_MDalgo.c_str();
    }
    else {
        return "MD5";
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : インスタンスのコピー                                */
/* Inputs         : コピー元インスタンス                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CGnuPGModule::Copy(const CGnuPGModule& org)
{
    //動作用各種変数
    m_ModuleName = org.m_ModuleName;   //モジュール名
    m_OptionStr = org.m_OptionStr;    //起動オプション文字列
    m_CommandStr = org.m_CommandStr;   //起動コマンド文字列
    m_StdoutBuffer = org.m_StdoutBuffer; //標準出力バッファ (表示用)
    m_StderrBuffer = org.m_StderrBuffer; //標準エラー出力バッファ (表示用)
    //オプション
    m_DefUserID = org.m_DefUserID; //デフォルトユーザID
    m_bPGPMIME = org.m_bPGPMIME; //PGP/MIME対応
    m_bPGPCompatible = org.m_bPGPCompatible; //PGP互換の署名・暗号化を行う
    m_bOpenPGP = org.m_bOpenPGP; //OpenPGPに準拠した署名暗号化を行う
    m_bThrowKeyID = org.m_bThrowKeyID; //受信者を匿名にする
    m_bRFC1991 = org.m_bRFC1991; //RFC1991に従う
    m_bMD5 = org.m_bMD5; //MD5を使用する
    m_bTextModeSig = org.m_bTextModeSig; //分離署名の際にテキストモードで署名する
}

/*----------------------------------------------------------------------*/
/* Purpose        : プロセスを起動する                                  */
/* Inputs         : なし                                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Execute(void)
{
    if((IsActive())==true) { // プロセスが終了していなければ強制終了
        Terminate();
    }

    //コマンドラインの整形
    //char szCom[_MAX_PATH];
    //GetEnvironmentVariable("COMSPEC" , szCom, _MAX_PATH);
    //string cmdline = szCom;
    //cmdline += " /C ";
    //cmdline += Quotation(m_ModuleName);
    string cmdline = m_ModuleName;
    //Quotation(cmdline);
    if((m_OptionStr.length())>0) {
        cmdline += " ";
        cmdline += m_OptionStr;
    }
    if((m_CommandStr.length())>0) {
        cmdline += " ";
        cmdline += m_CommandStr;
    }

    //パラメータの初期化
    SECURITY_ATTRIBUTES satt;
    satt.nLength = sizeof(SECURITY_ATTRIBUTES);
    satt.lpSecurityDescriptor = NULL;
    satt.bInheritHandle = TRUE;

    STARTUPINFO si;
    ZeroMemory( &si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
#ifdef _DEBUG
    si.wShowWindow = SW_MINIMIZE;
    //si.wShowWindow = SW_HIDE;
#else
    si.wShowWindow = SW_HIDE;
#endif
    CreatePipe(&m_hStdinRead,  &m_hStdinWrite,  &satt, m_PipeSize);
    CreatePipe(&m_hStdoutRead, &m_hStdoutWrite, &satt, m_PipeSize);
    CreatePipe(&m_hStderrRead, &m_hStderrWrite, &satt, m_PipeSize);
    si.hStdOutput = m_hStdoutWrite;
    si.hStdError  = m_hStderrWrite;
    si.hStdInput  = m_hStdinRead;

    PROCESS_INFORMATION pi;
    ZeroMemory( &pi, sizeof(PROCESS_INFORMATION));

    //プロセスの起動
    g_LogFile.AppendMessage(cmdline.c_str());
    if((CreateProcess(NULL, (LPTSTR)cmdline.c_str(), NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | HIGH_PRIORITY_CLASS, NULL, NULL, &si, &pi))==FALSE) {
        Sleep(10); // クッション？
        CloseHandle(pi.hThread);
        Terminate(); //パラメータを戻す
        return false;
    }
    else {
        m_hProcess = pi.hThread;
        return true;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : プロセスを終了する                                  */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CGnuPGModule::Terminate(void)
{
    //プロセスの終了
    if(m_hProcess!=NULL) { //動作中
        if((IsActive())==true) { // プロセスが終了していなければ強制終了
            TerminateProcess(m_hProcess, 0);
            WaitForSingleObject(m_hProcess, 1000); //完了するまで1秒くらい待ってみる
        }
        CloseHandle(m_hProcess);
        m_hProcess = NULL;
    }

    //パイプを閉じる
    if(m_hStdoutRead!=NULL) {
        CloseHandle(m_hStdoutRead);
        m_hStdoutRead = NULL;
    }
    if(m_hStdoutWrite!=NULL) {
        CloseHandle(m_hStdoutWrite);
        m_hStdoutWrite = NULL;
    }
    if(m_hStderrRead!=NULL) {
        CloseHandle(m_hStderrRead);
        m_hStderrRead = NULL;
    }
    if(m_hStderrWrite!=NULL) {
        CloseHandle(m_hStderrWrite);
        m_hStderrWrite = NULL;
    }
    if(m_hStdinRead!=NULL) {
        CloseHandle(m_hStdinRead);
        m_hStdinRead = NULL;
    }
    if(m_hStdinWrite!=NULL) {
        CloseHandle(m_hStdinWrite);
        m_hStdinWrite = NULL;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 文字列が空白文字を含む場合はダブルクォーテションで囲む*/
/* Inputs         : 文字列                                              */
/* Ootput         : 変換文字列                                          */
/*----------------------------------------------------------------------*/
string& CGnuPGModule::Quotation(string& str)
{
    string::size_type len = str.find_first_of(" \t");
    if(len!=string::npos) { //空白を含む文字列
        Dequotation(str); //いったんダブルクォーテションを外す
        string sbuf = "\"";
        sbuf += str + "\"";
        str = sbuf;
    }

    return str;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ダブルクォーテションの囲みを外す                    */
/* Inputs         : 文字列                                              */
/* Ootput         : 変換文字列                                          */
/*----------------------------------------------------------------------*/
string& CGnuPGModule::Dequotation(string& str)
{
    for(;;) {
        string::size_type len = str.length();
        if(len>=2 && str[0]=='\"' && str[len-1]=='\"') {
            str = str.substr(1, len-2);
        }
        else {
            break;
        }
    }

    return str;
}

/*----------------------------------------------------------------------*/
/* Purpose        : プロセスは稼働中か?                                 */
/* Inputs         : タイムアウト時間                                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::IsActive(DWORD tout)
{
    if(m_hProcess!=NULL) { //動作中
        if(WaitForSingleObject(m_hProcess, tout) != WAIT_OBJECT_0) { //指定時間待ってもシグナルなし
            return true;
        }
        //DWORD dwStatus;
        //if((GetExitCodeProcess(m_hProcess, &dwStatus))!=FALSE) {
        //    if(dwStatus==STILL_ACTIVE) {
        //        return true;
        //    }
        //}
    }

    return false;
}

/*----------------------------------------------------------------------*/
/* Purpose        : プロセスが終了するまで待つ                          */
/* Inputs         : なし                                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
void CGnuPGModule::WaitforTerminate(void)
{
    IsActive(30000); //最大30秒まで待つ?
    //for(int i=0; i<100 && (IsActive())==true; i++) { //最大10秒まで待つ
    //for(int i=0; i<300 && (IsActive())==true; i++) { //最大30秒まで待つ?
    //    Sleep(100);
    //}
    //Read(m_StdoutBuffer, true);
    Read(m_StderrBuffer, false);
    Terminate();
}

/*----------------------------------------------------------------------*/
/* Purpose        : 標準入力への入力                                    */
/* Inputs         : インプットメッセージ                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Write(string& inpMsg)
{
    string::size_type len = inpMsg.length();
    if(len<=0) { //入力メッセージがない
        return true;
    }
    char* msg = new char[len+1];
    strncpy(msg, inpMsg.c_str(), len);
    msg[len] = '\0';

    char* p = msg;
    DWORD dwRest = len;
    for(;;) {
        DWORD dwLen = 0;
        //if((IsActive())==false) { //プロセスは稼働していない
        //    delete [] msg;
        //    return false;
        //}
        if((WriteFile(m_hStdinWrite, p, dwRest, &dwLen, NULL))==FALSE) {
            delete [] msg;
            return false;
        }
        if(dwLen>=dwRest) { //全て書き込み完了
            break;
        }
        p += dwLen;
        dwRest -= dwLen;
    }

    delete [] msg;
    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 標準出力からのメッセージ                            */
/* Inputs         : 出力メッセージ格納先                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Read(string& outpMsg, bool bBlock)
{
    outpMsg = "";

    //パイプの状態を取得する
    DWORD dwSize;
    if(bBlock==false) {
        DWORD dwLen;
        if((PeekNamedPipe(m_hStderrRead, NULL, 0, &dwLen, &dwSize, NULL))==FALSE) { //読み取り失敗
            return false;
        }
        else if(dwSize<=0) { //何もなかった
            return true;
        }
        dwSize += 8; //strstr() 関数対策?
    }
    else {
        dwSize = m_PipeSize;
    }

    char* buf = new char[dwSize];
    if((ReadFile(m_hStderrRead, buf, dwSize, &dwSize, NULL))==FALSE) {
        delete [] buf;
        return false;
    }
    buf[dwSize] = '\0';
    outpMsg = buf;
    delete [] buf;

#ifdef _DEBUG
    g_LogFile.AppendMessage(outpMsg.c_str());
#endif
    return true;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
