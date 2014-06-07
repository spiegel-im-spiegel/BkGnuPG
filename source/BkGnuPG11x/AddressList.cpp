/************************************************************************/
/*  File name : AddressList.cpp                                         */
/************************************************************************/
/*  Contents  : メールアドレスリスト                                    */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include <mbstring.h>
#include "AddressList.h"
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include <algorithm>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : メールからアドレス一覧を追加する                    */
/* Inputs         : ウィンドウハンドル                                  */
/*                  アドレスフィールド種別                              */
/* Ootput         : 追加数                                              */
/*----------------------------------------------------------------------*/
int CAddressList::AddAddressFrom(HWND hWnd, FID field)
{
    char* fieldName;
    switch(field) {
    case FROM_FIELD:
        fieldName = "From";
        break;
    case REPLY_TO_FILED:
        fieldName = "Reply-To";
        break;
    case TO_FIELD:
        fieldName = "To";
        break;
    case CC_FIELD:
        fieldName = "Cc";
        break;
    case BCC_FIELD:
        fieldName = "Bcc";
        break;
    default :
        return 0;
    }

    char szAddr[32768];
    g_Info.m_BkApi.CompGetSpecifiedHeader(hWnd, fieldName, szAddr, 32768);
    char* lpAddr = g_Info.m_BkApi.SerializeRcpts(szAddr);
    return AddAddressFrom(lpAddr);
}

/*----------------------------------------------------------------------*/
/* Purpose        : シリアライズされたアドレス一覧情報を追加する        */
/* Inputs         : シリアライズされたアドレス一覧                      */
/* Ootput         : 追加数                                              */
/*----------------------------------------------------------------------*/
int CAddressList::AddAddressFrom(const char* list)
{
    if(list==NULL || (*list)=='\0') {
        return 0;
    }

    vector<string> listList;
    if((ImportList(list, listList))==0) {
        return 0;
    }

    int ct = 0;
    for(vector<string>::iterator p=listList.begin(); p!=listList.end(); p++) {
        string name, email;
        GetNameAndAddr(p->c_str(), name, email);
        if((email.length())>0 && (find(m_List.begin(), m_List.end(), email))==(m_List.end())) {
            m_List.push_back(email);
            ct++;
        }
    }

    return ct;
}

/*----------------------------------------------------------------------*/
/* Purpose        : メールアドレス(=UserID)リストをシリアライズする     */
/* Inputs         : なし                                                */
/* Ootput         : シリアライズされたアドレスリスト                    */
/*----------------------------------------------------------------------*/
const char* CAddressList::Serialize(void)
{
    m_SirialList = "";
    for(vector<string>::iterator p=m_List.begin(); p!=m_List.end(); p++) {
        if((m_SirialList.length())!=0) {
            m_SirialList += ",";
        }
        m_SirialList += *p;
    }

    return m_SirialList.c_str();
}

/*----------------------------------------------------------------------*/
/* Purpose        : シリアライズされた一覧をリストに格納する            */
/* Inputs         : シリアライズされたアドレス一覧                      */
/*                  格納先バッファ                                      */
/* Ootput         : 格納数                                              */
/*----------------------------------------------------------------------*/
int CAddressList::ImportList(const char* src, vector<string>& list)
{
    list.clear(); //リストのクリア
    string srcStr = src;
    string::size_type len = srcStr.length();
    if(len==0) { //中身がない
        return 0;
    }

    int ct = 0;
    for(;;) {
        bool bQuote = false;
        string::size_type idx = 0;
        for(; idx<len; idx++) {
            unsigned char c = (unsigned char)srcStr[idx];
            if ((_ismbblead(c))!=0) { //マルチバイト文字の1byte目
                idx++;
            }
            else if(c=='\"') {
                bQuote = !bQuote;
            }
            else if((c==',' || c==';') && bQuote==false) {
                break;
            }
        }
        if(idx==len) {
            list.push_back(srcStr);
            ct++;
            break;
        }
        else if(idx>0) {
            list.push_back(srcStr.substr(0, idx));
            ct++;
        }
        srcStr = srcStr.substr(idx+1);
        len = srcStr.length();
        if(len==0) { //中身がない
            break;
        }
    }

    return ct;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
