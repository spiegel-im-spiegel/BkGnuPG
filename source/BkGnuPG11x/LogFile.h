/************************************************************************/
/*  File name : LogFile.h                                               */
/************************************************************************/
/*  Contents  : デバッグ用ログファイル                                  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _LOGFILE_H_
#define _LOGFILE_H_

/**** Incude Files ****/
#include <string>
#include <fstream>


/**** Global Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CLogFile                                                */
/* Purpose    : デバッグ用ログファイル                                  */
/*----------------------------------------------------------------------*/
class CLogFile
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
    bool m_bWrite; //書き込み許可フラグ
    std::string m_LogPath; //ログファイルへのパス
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CLogFile(const CLogFile& org); //コピーコンストラクタ
    CLogFile& operator=(const CLogFile& org); //代入演算子
    //内部処理関数
    std::string TimeString(void); //現在時刻を文字列変換する
public:
    //コンストラクタ・デストラクタ
    CLogFile(const char* path=NULL) : m_bWrite(false) { Reset(path); } //デフォルトコンストラクタ
    virtual ~CLogFile() {} //デストラクタ
    //演算子
    //インタフェース関数
    void Reset(const char* path); //初期化処理
    void AppendMessage(const char* msg); //メッセージの出力
    template<class T> void AppendValue(const char* caption, const T& val) //値を出力する
    {
        using namespace std;
        if(m_bWrite==true) {
            try {
                //メッセージを追記する
                ofstream of(m_LogPath.c_str(), ios_base::app); //追記モードでオープン
                //時刻とともにメッセージを出力
                of << TimeString() << ": " << caption << " = " << val << endl;
                of.close();
            }
            catch(...) {
                return; //どうしようもないので何もしない
            }
        }
    }
};

/**** External Declarelation ****/
extern CLogFile g_LogFile; //ログファイル 実装


#endif // _LOGFILE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
