/************************************************************************/
/*  File name : Common.cpp                                              */
/************************************************************************/
/*  Contents  : 共通関数群                                              */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include <mbstring.h>
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include "LogFile.h"
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : 例外処理メッセージを表示する                        */
/* Inputs         : Window ハンドル                                     */
/*                  メッセージID                                        */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void ErrorMessage(HWND hWnd, UINT uID)
{
    try {
        char szError[256];
        if((LoadString(g_Info.m_hInstance, uID, szError, 256))>0) {
            g_LogFile.AppendMessage(szError);
            MessageBox(hWnd, szError, "Message from BkGnuPG", MB_OK | MB_ICONWARNING);
            g_LogFile.AppendMessage(szError);
        }
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //エラー内容を出力
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 致命的エラーメッセージを表示する                    */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void FatalErrorMessage(HWND hWnd, const char* fileName, int lineno, const char* pMsg)
{
    try {
        DWORD errNo = GetLastError();
        string ferrMsg;
        if(pMsg==NULL) {
            LPVOID p;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        errNo,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPTSTR)&p, 0, NULL);
            ferrMsg = (char*)p;
            LocalFree(p);
        }
        else {
            ferrMsg = pMsg;
        }
        char szError[256];
        if((LoadString(g_Info.m_hInstance, IDS_GPG_EXCEPTION, szError, 256))>0) {
            string errMsg = szError;
            errMsg += "\n\n\"" + ferrMsg;
            TrimSpace(errMsg);
            errMsg += "\" ";
            if(errNo) { //エラーコードが0以外?
                MessageBox(hWnd, errMsg.c_str(), "Message from BkGnuPG", MB_OK | MB_ICONERROR);
            }
            string file = fileName;
            string::size_type idx = file.find_last_of("\\");
            if(idx!=string::npos) {
                errMsg += file.substr(idx+1);
            }
            else {
                errMsg += file;
            }
            g_LogFile.AppendValue(errMsg.c_str(), lineno); //値を出力する
        }
    }
    catch(...) {
        return ; //どうしようもないので何もしない
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : ファイル名として利用可能な名前に変換する            */
/* Inputs         : 任意の名前                                          */
/*                  ファイル名格納バッファ                              */
/* Ootput         : ファイル名                                          */
/*                                                                      */
/* ※ BkCommon.cpp の同名関数のSTL実装版                                */
/*----------------------------------------------------------------------*/
string& GetLegalFileName(const char* name, string& fileName)
{
    string namebuf = name;
    fileName = "";
    //前後の空白文字を削除
    TrimSpace(namebuf);

    //文字変換
    string::size_type len = namebuf.length();
    if(len==0) { //空白文字しかない
        return fileName;
    }
    for(string::size_type i=0; i<len; i++) {
        unsigned char c = (unsigned char)namebuf[i];
        if ((_ismbblead(c))!=0) { //マルチバイト文字の1byte目
            fileName += (char)c;
            i++;
            fileName += namebuf[i];
        }
        else if(c<' ' || c==0x7f) { //ASCII以外
            c = ' ';
            fileName += (char)c;
        }
        else {
            switch (c) {
            case '*':
            case '<':
            case '>':
            case '?':
            case '\\':
            case '|':
            case '/':
                c = '=';
                break;
            case ':':
                c = ';';
                break;
            case '\"':
                c = '\'';
                break;
            default:
                break;
            }
            fileName += (char)c;
        }
    }

    //前後の空白文字を削除
    TrimSpace(fileName);

    //末尾に'.'があれば削除
    len = fileName.length();
    if(len>0 && fileName[len-1]=='.') {
        fileName = fileName.substr(0, len-1);
    }

    return fileName;
}

/*----------------------------------------------------------------------*/
/* Purpose        : テキストファイル内容を文字列情報としてメモリにロードする*/
/* Inputs         : ファイルへのパス名                                  */
/* Ootput         : ファイル内容(文字列)                                */
/*                                                                      */
/* ※ BkCommon.cpp の同名関数の別実装。メモリの確保に CBeckyAPI::Alloc  */
/*    を使用するので取り扱い注意                                        */
/*----------------------------------------------------------------------*/
char* FileToString(const string& filepath)
{
    //ファイルオープン
    HANDLE hFile = CreateFile(filepath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile==INVALID_HANDLE_VALUE) {
        return NULL;
    }
    DWORD nSize = GetFileSize(hFile, NULL); //4GB以上のサイズには対応しない
    if(nSize==0) {
        CloseHandle(hFile);
        return NULL;
    }

    //ファイル内容のコピー
    char* strBuf = (char*)g_Info.m_BkApi.Alloc(nSize+4);
    if(strBuf!=NULL) {
        DWORD lSize;
        if((ReadFile(hFile, strBuf, nSize, &lSize, NULL))==FALSE) {
            g_Info.m_BkApi.Free(strBuf);
            strBuf = NULL;
        }
        else {
            strBuf[lSize] = '\0';
        }
    }

    //ファイルクローズ
    CloseHandle(hFile);
    return strBuf;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 「name <email>」形式の文字列を要素に分解する        */
/* Inputs         : 元になる文字列                                      */
/*                  名前格納バッファ                                    */
/*                  e-mail格納バッファ                                  */
/* Ootput         : なし                                                */
/*                                                                      */
/* ※ BkCommon.cpp の同名関数のSTL実装版                                */
/*----------------------------------------------------------------------*/
void GetNameAndAddr(const char* src, string& name, string& email)
{
    name = email = "";
    string srcStr = src;
    TrimSpace(srcStr); //前後の空白を除く
    string::size_type len = srcStr.length();
    if(len==0) { //中身がない
        return;
    }

    // name <email> 形式で検索する
    bool bQuote = false;
    string::size_type idx;
    for(idx=0; idx<len; idx++) {
        unsigned char c = (unsigned char)srcStr[idx];
        if ((_ismbblead(c))!=0) { //マルチバイト文字の1byte目
            idx++;
        }
        else if(c=='\"') {
            bQuote = !bQuote;
        }
        else if(c=='<' && bQuote==false) {
            break;
        }
    }
    if(idx!=len) { //デリミタ検出
        string sbuf = srcStr.substr(idx+1);
        string::size_type idx2 = sbuf.find_first_of('>');
        email = sbuf.substr(0, idx2);
        TrimSpace(email); //前後の空白を除く
        if(idx>0) {
            name = srcStr.substr(0, idx);
            TrimSpace(name); //前後の空白を除く
        }
        if((name.length())==0 && idx2!=string::npos) { // <email> name かもしれない
            idx2++;
            if((sbuf.length())<idx2) {
                name = sbuf.substr(idx2);
                TrimSpace(name); //前後の空白を除く
            }
        }
        return;
    }
    else { // email (name) 形式で探してみる
        idx = srcStr.find_first_of('(');
        if(idx==string::npos) { //e-mailのみの記述?
            email = srcStr;
        }
        else {
            string sbuf = srcStr.substr(idx+1);
            string::size_type idx2 = sbuf.find_first_of(')');
            name = sbuf.substr(0, idx2);
            TrimSpace(name); //前後の空白を除く
            if(idx>0) {
                email = srcStr.substr(0, idx);
                TrimSpace(email); //前後の空白を除く
            }
            if((sbuf.length())==0 && idx2!=string::npos) { // (name) email かもしれない
                idx2++;
                if((sbuf.length())<idx2) {
                    email = sbuf.substr(idx2);
                    TrimSpace(email); //前後の空白を除く
                }
            }
        }
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ファイルの有無を問いあわせる                        */
/* Inputs         : ファイル名(フルパス)                                */
/* Ootput         : Boolearn                                            */
/*                                                                      */
/* ※ BkCommon.cpp の同名関数のSTL実装版                                */
/*----------------------------------------------------------------------*/
bool IsFileExist(const string& path)
{
    WIN32_FIND_DATA FindData;
    HANDLE hFind = ::FindFirstFile(path.c_str(), &FindData);
    if (hFind != INVALID_HANDLE_VALUE) {
        ::FindClose(hFind);
        if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0) { //ディレクトリでない
            return true;
        }
    }
    return false;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ファイル削除 (ワイルドカード有効)                   */
/* Inputs         : ファイル名(フルパス)                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void RemoveFiles(const string& path)
{
    string dpath;
    try {
        //ディレクトリ部を取得する
        string::size_type idx = path.find_last_of("\\");
        string dir = "";
        if(idx!=string::npos) {
            dir = path.substr(0, idx+1);
        }
        //ファイル検索と削除
        WIN32_FIND_DATA FindData;
        HANDLE hFind = ::FindFirstFile(path.c_str(), &FindData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0) { //ディレクトリでない
                    dpath = dir + FindData.cFileName; //パス作成
                    DeleteFile(dpath.c_str()); //削除!
#ifdef _DEBUG
                    g_LogFile.AppendValue("delete file", dpath);
#endif
                }
            } while((FindNextFile(hFind, &FindData))!=FALSE);
            ::FindClose(hFind);
        }
    }
    catch(...) {
        g_LogFile.AppendValue("cannot delete file", dpath);
    }
    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 前後の空白文字を取り除く                            */
/* Inputs         : 文字列データ                                        */
/* Ootput         : 処理後の文字列データ                                */
/*----------------------------------------------------------------------*/
string& TrimSpace(string& element)
{
    string::size_type idx = element.find_first_not_of(" \t\r\n");
    string str;
    if(idx!=string::npos) {
        element = str = element.substr(idx); //先頭の空白文字を除く
    }
    else { //空白文字しかない(^^;)
        return element = "";
    }
    idx = str.find_last_not_of(" \t\r\n");
    if(idx!=string::npos) {
        element = str.substr(0, idx+1); //末尾の空白文字を除く
    }

    return element;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
