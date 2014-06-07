/************************************************************************/
/*  File name : LogFile  .cpp                                           */
/************************************************************************/
/*  Contents  : デバッグ用ログファイル                                  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "LogFile.h"

#include <ctime>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : 初期化処理                                          */
/* Inputs         : ログファイルへのパス                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CLogFile::Reset(const char* path)
{
    if(path==NULL) {
        m_bWrite = false;
        m_LogPath = "";
    }
    else {
        m_bWrite = true;
        m_LogPath = path;
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : メッセージの出力                                    */
/* Inputs         : メッセージ文字列                                    */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void CLogFile::AppendMessage(const char* msg)
{
    if(m_bWrite==false) {
        return;
    }

    //メッセージを追記する
    try {
        ofstream of(m_LogPath.c_str(), ios_base::app); //追記モードでオープン
        //時刻とともにメッセージを出力
        of << TimeString() << ": " << msg << endl;
        of.close();
    }
    catch(...) {
        return;
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 現在時刻を文字列変換する                            */
/* Inputs         : なし                                                */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
string CLogFile::TimeString(void)
{
    time_t sysTm;
    time(&sysTm); //システムタイマ値取得
    struct tm *t = localtime(&sysTm);
    string s = asctime(t);
    string::size_type len = s.find_first_of("\n");
    if(len!=string::npos) {
        s = s.substr(0, len);
    }

    return s;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
