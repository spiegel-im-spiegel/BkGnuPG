/************************************************************************/
/*  File name : Encrypt.cpp                                             */
/************************************************************************/
/*  Contents  : 暗号化処理 (コールバック関数)                           */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.21    2000.10.04          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include "CallBacks.h"
#include "MIMEMessage.h"
#include "AddressList.h"
#include "UsersDialog.h"
#include "PassDialog.h"
#include "ViewDialog.h"
#include "LogFile.h"

#include <ctime>
#include <string>
#include <fstream>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/
static void ExecEncrypt(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode);
static void ExecSign(HWND hWnd, LPARAM lParam);
static bool GetMessage(HWND hWnd, string& msgPath, CMIMEMessage& item, CGnuPGModule::EMODE mode);


/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化処理 (メニューからの呼び出し)                 */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void WINAPI Encrypt(HWND hWnd, LPARAM lParam)
{
    try {
        ExecEncrypt(hWnd, lParam, CGnuPGModule::Encrypt);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化処理 (メニューからの呼び出し)                 */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void WINAPI EncryptSign(HWND hWnd, LPARAM lParam)
{
    try {
        ExecEncrypt(hWnd, lParam, CGnuPGModule::EncryptSign);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 署名処理 (メニューからの呼び出し)                   */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void WINAPI Sign(HWND hWnd, LPARAM lParam)
{
    try {
        ExecSign(hWnd, lParam);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 署名処理                                            */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
static void ExecSign(HWND hWnd, LPARAM lParam)
{
    //if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
    //    SendMessage(hWnd, WM_SET_TRANSFER_SAFE, (WPARAM)TRUE, 0);
    //}

    //入出力ファイル作成
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");

    //署名するユーザを取得する
    CAddressList localUser;
    localUser.Clear();
    if((localUser.AddAddressFrom(hWnd, CAddressList::FROM_FIELD))==0) { //Fromフィールドの中身がない
        ErrorMessage(hWnd, IDS_GPG_NOLOCALUSER);
        return;
    }
    vector<string>& rlist = localUser.GetList();
    vector<string>::iterator s = rlist.begin();
    g_Info.m_UserID = (*s); // From フィールドから取得

    //パスフレーズの入力
    g_Info.m_PassPhrase = "";
    CPassPhraseDialog dlg; //PassPhrase ダイアログ
    if((dlg.ExecDialog(hWnd, lParam, g_Info, false))==false) {
        return; //ユーザによる中止
    }

    //暗号化メッセージの取り出し
    CMIMEMessage item;
    if((GetMessage(hWnd, msgPath, item, CGnuPGModule::Sign))==false) {
        ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        return;
    }

    //暗号化処理
    RemoveFiles(ascPath);
    if((g_Info.m_GPG.ExecSign(ascPath.c_str(), msgPath.c_str(), g_Info.m_UserID.c_str(), g_Info.m_PassPhrase.c_str(), g_Info.m_bUseAgent))==false) {
        ErrorMessage(hWnd, IDS_GPG_NOTPROCESSED);
        g_Info.m_PassPhrase = "";
        RemoveFiles(msgPath);
        return;
    }
    else {
        g_Info.m_PassPhrase = "";
        CViewDialog vdlg;
        vdlg.ExecDialog(hWnd, lParam, g_Info, g_Info.m_GPG.GetOutputMsg());
    }

    //暗号化メッセージを設定する
    char* lpArea = FileToString(ascPath);
    if(lpArea!=NULL) {
        if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
            //MIMEバウンダリ文字列作成
            char szBoundary[1024];
            time_t t;
            time(&t);
            sprintf(szBoundary, "===[PGP/MIME_RFC2015]===%08X.%04X===", (DWORD)t, rand());
            //MIMEメッセージを作成する
            CBigBuffer Buffer; //巨大文字列バッファ
            //Header
            string szData = "Content-Type: multipart/signed;\r\n boundary=\"";
            szData += szBoundary;
            szData += "\";\r\n protocol=\"application/pgp-signature\"";
            //if((g_Info.m_GPG.IsMD5())==true) {
            //    szData += "; micalg=pgp-md5\r\n"; //MD5
            //}
            //else {
            //    szData += "; micalg=pgp-sha1\r\n"; //SHA-1
            //}
            szData += "; micalg=pgp-";
            szData += g_Info.m_GPG.GetOptMDalgo();
            szData += "; \r\n";
            Buffer.AddStr(szData.c_str());
            item.GetHeader("Content-Transfer-Encoding", szData);
            if(stricmp(szData.c_str(), "8bit") == 0 || stricmp(szData.c_str(), "binary") == 0) {
                //なにもしない?
            }
            else {
                Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n"); //7bitにセット
            }
            //Body
            //Buffer.AddStr("\r\n");
            //メッセージ パート
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            char* lpMsg = FileToString(msgPath);
            if(lpMsg!=NULL) {
                Buffer.AddStr(lpMsg); //メッセージブロックをセット
                g_Info.m_BkApi.Free(lpMsg);
            }
            //署名パート
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Type: application/pgp-signature\r\n");
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n\r\n");
            char* lpSig = strstr(lpArea, "-----BEGIN PGP SIGNATURE-----");
            if(lpSig==NULL) {
                lpSig = strstr(lpArea, "-----BEGIN PGP MESSAGE-----");
            }
            if(lpSig!=NULL) {
                Buffer.AddStr(lpSig);
            }
            //マルチパートの終わり
            szData = "\r\n--";
            szData += szBoundary;
            szData += "--\r\n";
            Buffer.AddStr(szData.c_str());
            //Import
            CMIMEMessage EncItem;
            EncItem.FromString(Buffer.Reference());
            Buffer.Reset(); //もう不要
            item.OverWrite(EncItem); //もとのMIMEメッセージに上書きする
            //Export
            char* lpSource = item.ToString();
            //g_Info.m_BkApi.CompSetText(hWnd, 0, lpSource); // for debug
            g_Info.m_BkApi.CompSetSource(hWnd, lpSource);
            g_Info.m_BkApi.Free(lpSource);
        }
        else {
            g_Info.m_BkApi.CompSetText(hWnd, 0, lpArea);
        }
        g_Info.m_BkApi.Command(hWnd, "TextTop");
        g_Info.m_BkApi.Free(lpArea);
    }
    RemoveFiles(ascPath);
    RemoveFiles(msgPath);
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化処理                                          */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
static void ExecEncrypt(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode)
{
    //if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
    //    SendMessage(hWnd, WM_SET_TRANSFER_SAFE, (WPARAM)TRUE, 0);
    //}

    //入出力ファイル作成
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");

    //暗号化する公開鍵のユーザを取得する
    CAddressList recipient;
    recipient.Clear();
    recipient.AddAddressFrom(hWnd, CAddressList::TO_FIELD); //To フィールドのアドレスリストを取得
    if(g_Info.m_bEnableCcUser==true) { //Cc フィールドのアドレスも含める
        recipient.AddAddressFrom(hWnd, CAddressList::CC_FIELD);
    }
    if(g_Info.m_bEnableBccUser==true) { //Bcc フィールドのアドレスも含める
        recipient.AddAddressFrom(hWnd, CAddressList::BCC_FIELD);
    }
    if(g_Info.m_bEnableFromUser==true) { //From フィールドのアドレスも含める
        recipient.AddAddressFrom(hWnd, CAddressList::FROM_FIELD);
    }
    if(g_Info.m_bEnableDefUser==true) { //デフォルトユーザも含める
        recipient.AddAddressFrom(g_Info.m_GPG.GetDefUserID());
    }
    g_Info.m_UserID = recipient.GetSerialList();
    g_Info.m_PassPhrase = "";
    CUsersDialog dlg; //Users ダイアログ
    if((dlg.ExecDialog(hWnd, lParam, g_Info))==false) {
        return; //ユーザによる中止
    }
    recipient.Clear();
    if((recipient.AddAddressFrom(g_Info.m_UserID.c_str()))==0) { //ひとつもユーザIDがない
        ErrorMessage(hWnd, IDS_GPG_NORECIPIENT);
        return;
    }

    if(mode==CGnuPGModule::EncryptSign) {
        //署名するユーザを取得する
        CAddressList localUser;
        localUser.Clear();
        if((localUser.AddAddressFrom(hWnd, CAddressList::FROM_FIELD))==0) { //Fromフィールドの中身がない
            ErrorMessage(hWnd, IDS_GPG_NOLOCALUSER);
            return;
        }
        vector<string>& rlist = localUser.GetList();
        vector<string>::iterator s = rlist.begin();
        g_Info.m_UserID = (*s); // From フィールドから取得
        //パスフレーズの入力
        g_Info.m_PassPhrase = "";
        CPassPhraseDialog dlg; //PassPhrase ダイアログ
        if((dlg.ExecDialog(hWnd, lParam, g_Info, false))==false) {
            return; //ユーザによる中止
        }
    }

    //暗号化メッセージの取り出し
    CMIMEMessage item;
    if((GetMessage(hWnd, msgPath, item, mode))==false) {
        ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        return;
    }

    //暗号化処理
    RemoveFiles(ascPath);
    bool rtn;
    if(mode==CGnuPGModule::Encrypt) {
        rtn = g_Info.m_GPG.ExecEncrypt(ascPath.c_str(), msgPath.c_str(), recipient.GetList());
    }
    else { //CGnuPGModule::EncryptSign
        rtn = g_Info.m_GPG.ExecEncryptSign(ascPath.c_str(), msgPath.c_str(), g_Info.m_UserID.c_str(), recipient.GetList(), g_Info.m_PassPhrase.c_str(), g_Info.m_bUseAgent);
        g_Info.m_PassPhrase = "";
    }
    if(rtn==false) {
        ErrorMessage(hWnd, IDS_GPG_NOTPROCESSED);
        RemoveFiles(msgPath);
        return;
    }
    else {
        CViewDialog vdlg;
        vdlg.ExecDialog(hWnd, lParam, g_Info, g_Info.m_GPG.GetOutputMsg());
    }

    //暗号化メッセージを設定する
    char* lpArea = FileToString(ascPath);
    if(lpArea!=NULL) {
        if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
            //MIMEバウンダリ文字列作成
            char szBoundary[1024];
            time_t t;
            time(&t);
            sprintf(szBoundary, "===[PGP/MIME_RFC2015]===%08X.%04X===", (DWORD)t, rand());
            //MIMEメッセージを作成する
            CBigBuffer Buffer; //巨大文字列バッファ
            //ヘッダ
            string szData = "Content-Type: multipart/encrypted;\r\n boundary=\"";
            szData += szBoundary;
            szData += "\";\r\n protocol=\"application/pgp-encrypted\"\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n");
            //Body
            //Buffer.AddStr("\r\n");
            //encrypted パート
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Type: application/pgp-encrypted\r\n");
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n\r\n");
            Buffer.AddStr("Version: 1\r\n"); //取り敢えず固定値
            //暗号化ブロック
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Type: application/octet-stream\r\n");
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n\r\n");
            char* lpMsg = strstr(lpArea, "-----BEGIN PGP MESSAGE-----");
            if (lpMsg) {
                Buffer.AddStr(lpMsg);
            }
            //マルチパートの終わり
            szData = "\r\n--";
            szData += szBoundary;
            szData += "--\r\n";
            Buffer.AddStr(szData.c_str());
            //Import
            CMIMEMessage EncItem;
            EncItem.FromString(Buffer.Reference());
            Buffer.Reset(); //もう不要
            item.OverWrite(EncItem); //もとのMIMEメッセージに上書きする
            //Export
            char* lpSource = item.ToString();
            //g_Info.m_BkApi.CompSetText(hWnd, 0, lpSource); // for debug
            g_Info.m_BkApi.CompSetSource(hWnd, lpSource);
            g_Info.m_BkApi.Free(lpSource);
        }
        else {
            g_Info.m_BkApi.CompSetText(hWnd, 0, lpArea);
        }
        g_Info.m_BkApi.Command(hWnd, "TextTop");
        g_Info.m_BkApi.Free(lpArea);
    }
    RemoveFiles(ascPath);
    RemoveFiles(msgPath);
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化メッセージの取り出し                          */
/* Inputs         : 暗号化対象のメッセージを格納するファイルへのパス    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static bool GetMessage(HWND hWnd, string& msgPath, CMIMEMessage& item, CGnuPGModule::EMODE mode)
{
    //ファイルオープン
    RemoveFiles(msgPath); //以前のファイルは削除する
    ofstream msgFile(msgPath.c_str(), ios_base::binary);
    if((msgFile.is_open())==false) {
        g_LogFile.AppendValue("Cannot file Open", msgPath);
        return false;
    }

    //暗号化対象のメッセージをファイルに格納する
    if(g_Info.m_GPG.IsPGPMIME()==true) { //PGP/MIME対応
        char* lpSrc = g_Info.m_BkApi.CompGetSource(hWnd); //作成中のメッセージを取得
        if(lpSrc==NULL) {
            g_LogFile.AppendMessage("No MIME message");
            msgFile.close();
            RemoveFiles(msgPath); //念の為の後始末
            return false;
        }
        item.FromString(lpSrc);
        g_Info.m_BkApi.Free(lpSrc);
        //Body Part の取り出し
        CMIMEMessage BodyPart;
        item.GetBody(BodyPart);
        msgFile << BodyPart.ToString();
    }
    else { //PGP
        char szMimeType[80];
        char* lpBody = g_Info.m_BkApi.CompGetText(hWnd, szMimeType, 80); //作成中のメッセージ(Text)を取得
        char szCharSet[82];
        g_Info.m_BkApi.CompGetCharSet(hWnd, szCharSet, 80); //キャラクタセットを取得

        if(stricmp(szCharSet, "ISO-2022-JP") == 0 || (szCharSet[0] == '\0' && GetACP() == 932)) { //日本語の平文
            char* lpJIS = g_Info.m_BkApi.ISO_2022_JP(lpBody, TRUE); //JISコードに変換する
            msgFile << lpJIS;
            g_Info.m_BkApi.Free(lpJIS);
            //if(g_Info.m_GPG.IsPGPCompatible()==true && mode==CGnuPGModule::Sign) { //PGP互換のクリア署名 ← GnuPG 1.0.4 で改善?
            //    msgFile << "\r\n";
            //}
        }
        else { //日本語以外の平文
            msgFile << lpBody; //そのまま格納
        }
        g_Info.m_BkApi.Free(lpBody);
    }
    msgFile.close();

    return true;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
