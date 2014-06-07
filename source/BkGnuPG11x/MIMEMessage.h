/************************************************************************/
/*  File name : MIMEMessage.h                                           */
/************************************************************************/
/*  Contents  : MIME メールメッセージ                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _MIMEMESSAGE_H_
#define _MIMEMESSAGE_H_

/**** Incude Files ****/
#include <string>
#include <vector>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CBigBuffer                                              */
/* Purpose    : 巨大文字列バッファ操作クラス                            */
/*                                                                      */
/* ※ メモリの確保に CBeckyAPI::Alloc を使用するので取り扱い注意        */
/*----------------------------------------------------------------------*/
class CBigBuffer
{
// メンバインスタンス
private:
    //定数定義
    static const int m_Capacity; //バッファのブロック単位
    //内部変数
    bool m_bBkAPI; //メモリ操作に Becky! API を使うかどうか
    char* m_Buffer; //バッファへのポインタ
    int m_nBlock; //ヒープメモリのブロック数
    int m_nSize; //文字列サイズ
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CBigBuffer(const CBigBuffer& org); //コピーコンストラクタ
    CBigBuffer& operator=(const CBigBuffer& org); //代入演算子
    //内部処理関数
    inline void Init(void) { m_Buffer=NULL; m_nBlock=0; m_nSize=0; } //内部状態の初期化
public:
    //コンストラクタ・デストラクタ
    CBigBuffer(bool bBkAPI=true) : m_Buffer(NULL), m_nBlock(0), m_nSize(0), m_bBkAPI(bBkAPI) {} //デフォルトコンストラクタ
    virtual ~CBigBuffer() { Reset(); } //デストラクタ
    //演算子
    //インタフェース関数
    void Reset(bool bBkAPI=true); //内部バッファを解放し初期化する
    void AddStr(const char* str); //バッファに文字列を追加する
    void RollBack(void); //末尾の改行コードを削除する
    const char* Reference(void) const { return m_Buffer; } //バッファの表示(推奨しない)
    char* Export(void); //バッファのエクスポート
};

/*----------------------------------------------------------------------*/
/* Class Name : CMIMEMessage                                            */
/* Purpose    : MIME メールメッセージクラス                             */
/*----------------------------------------------------------------------*/
class CMIMEMessage
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
    //関連するメッセージ
    CMIMEMessage* m_pNext; //次のMIMEメッセージ
    CMIMEMessage* m_pChild; //カプセル化された子MIMEメッセージ
    //ヘッダ情報
    std::string m_Type;
    std::string m_SubType;
    std::string m_Boundary;
    std::vector<std::string> m_lstHeaders;
    //std::vector<std::string> m_lstBody;
    //std::vector<std::string> m_lstTrail;
    CBigBuffer m_Body;
    CBigBuffer m_Trail;
public:
    //定数定義

// メンバメソッド
private:
    //内部処理関数
    void Init(void); //初期化
    void Copy(const CMIMEMessage& org); //インスタンスのコピー
    const char* GetLine(const char* src, std::string& lineBuf); //一行分の文字列を取得
    std::string& GetOptValue(std::string& option, std::string& opt, std::string& val); // 文字列から opt=val の情報を抜き出す
    void SetNext(const CMIMEMessage& next);
    void SetChild(const CMIMEMessage& child);
public:
    //コンストラクタ・デストラクタ
    CMIMEMessage() : m_pNext(NULL), m_pChild(NULL) { Init(); } //デフォルトコンストラクタ
    CMIMEMessage(const CMIMEMessage& org) : m_pNext(NULL), m_pChild(NULL) { Copy(org); } //コピーコンストラクタ
    virtual ~CMIMEMessage() { Init(); } //デストラクタ
    //演算子
    CMIMEMessage& operator=(const CMIMEMessage& org) { Copy(org); return *this; } //代入演算子
    //インタフェース関数
    inline const std::string& GetType(void) const { return m_Type; }
    inline const std::string& SetType(const std::string& szType) { return m_Type = szType; }
    inline const std::string& GetSubType(void) const { return m_SubType; }
    inline const std::string& SetSubType(const std::string& szSubType) { return m_SubType = szSubType; }
    inline const std::string& GetBoundary(void) const { return m_Boundary; }
    inline const std::string& SetBoundary(const std::string& szBoundary) { return m_Boundary = szBoundary; }
    CMIMEMessage& GetBody(CMIMEMessage& item); //Body関連の情報(Header含む)を抜き出す
    inline void ClearBody(void) { m_Body.Reset(); }
    inline void AddBody(const std::string& body) { m_Body.AddStr(body.c_str()); }
    inline CMIMEMessage* GetNext(void) const { return m_pNext; }
    inline void ClearNext(void) { delete m_pNext; m_pNext=NULL; }
    inline CMIMEMessage* GetChild(void) const { return m_pChild; }
    inline void ClearChild(void) { delete m_pChild; m_pChild=NULL; }
    void AddChild(const CMIMEMessage& child);
    void OverWrite(CMIMEMessage& item); //MIMEメッセージを上書きコピーする
    const char* FromString(const char* src, const char* boundParent=NULL);
    char* ToString(void);
    std::string& GetHeader(const char* header, std::string& dataBuf);
    bool SetHeader(const char* header, const char* data);
    CMIMEMessage* FindCMIMEMessage(const char* type, const char* subType);
#if 0
    std::string& SepareteHeaderValue(std::string& value);
#endif
};


#endif // _MIMEMESSAGE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
