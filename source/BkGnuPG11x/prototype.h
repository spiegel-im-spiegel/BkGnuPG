/************************************************************************/
/*  File name : prototype.h                                             */
/************************************************************************/
/*  Contents  : 私的コーディング規約に基づくプロトタイプ                */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

/**** Incude Files ****/


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CExsample                                               */
/* Purpose    : クラス定義例                                            */
/*----------------------------------------------------------------------*/
class CExsample
{
// メンバインスタンス
private:
    //定数定義
    //内部変数
    int m_val; //内部変数
public:
    //定数定義

// メンバメソッド
private:
    //インスタンスのコピーを作らせないための措置
    CExsample(const CExsample& org); //コピーコンストラクタ
    CExsample& operator=(const CExsample& org); //代入演算子
    //内部処理関数
    void PMetod(void); //非公開メソッド
public:
    //コンストラクタ・デストラクタ
    CExsample(); //デフォルトコンストラクタ
    virtual ~CExsample(); //デストラクタ
    //演算子
    //インタフェース関数
    void GMetod(void); //公開メソッド
};


#endif // _PROTOTYPE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
