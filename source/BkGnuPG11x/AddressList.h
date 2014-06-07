/************************************************************************/
/*  File name : AddressList.h                                           */
/************************************************************************/
/*  Contents  : メールアドレスリスト                                    */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _ADDRESSLIST_H_
#define _ADDRESSLIST_H_

/**** Incude Files ****/
#include <windows.h>

#include <string>
#include <vector>

/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CAddressList                                            */
/* Purpose    : アドレスリストクラス                                    */
/*----------------------------------------------------------------------*/
class CAddressList
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
    std::vector<std::string> m_List; //メールアドレス(=UserID)リスト
    std::string m_SirialList; //メールアドレス(=UserID)リスト(シリアライズしたもの)
public:
    //定数定義
    enum FID { //アドレスフィールド種別
        NULL_FIELD = 0,
        FROM_FIELD,
        REPLY_TO_FILED,
        TO_FIELD,
        CC_FIELD,
        BCC_FIELD
    };

// メンバメソッド
private:
    //内部処理関数
    void Copy(const CAddressList& org) { m_List = org.m_List; } //インスタンスのコピー
    const char* Serialize(void); //メールアドレス(=UserID)リストをシリアライズする
    int ImportList(const char* src, std::vector<std::string>& list); //シリアライズされた一覧をリストに格納する
public:
    //コンストラクタ・デストラクタ
    CAddressList() { Clear(); } //デフォルトコンストラクタ
    CAddressList(const CAddressList& org) { Copy(org); } //コピーコンストラクタ
    virtual ~CAddressList() {} //デストラクタ
    //演算子
    CAddressList& operator=(const CAddressList& org) { Copy(org); return *this; } //代入演算子
    //インタフェース関数
    inline void Clear(void) { m_List.clear(); } //一覧をクリアする
    int AddAddressFrom(HWND hWnd, FID field); //メールからアドレス一覧を追加する
    int AddAddressFrom(const char* list); //シリアライズされたアドレス一覧情報を追加する
    inline std::vector<std::string>& GetList(void) { return m_List; } //アドレス一覧取得
    inline const char* GetSerialList(void) { return Serialize(); } //アドレス一覧取得
};


#endif // _ADDRESSLIST_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
