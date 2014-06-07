/************************************************************************/
/*  File name : Decrypt.cpp                                             */
/************************************************************************/
/*  Contents  : 復号化処理 (コールバック関数)                           */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/*  Modified  : Yasuhiro ARAKAWA, Koji Hatanaka                         */
/*                                  Version 1.0.5b3 2005.08.11          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include "CallBacks.h"
#include "MIMEMessage.h"
#include "PassDialog.h"
#include "ViewDialog.h"
#include "LogFile.h"

#include <string>
#include <fstream>
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/
static void ExecDecript(HWND hWnd, LPARAM lParam);
static bool ExecGPG(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode, const char* ascPath, const char* txtPath);
static bool GetMIMEMessage(CMIMEMessage* pTop, CGnuPGModule::EMODE mode, string& ascPath, string& msgPath);
static char* GetMessage(char* lpStr, CGnuPGModule::EMODE& mode, string& ascPath, char* szCharSet);


/*----------------------------------------------------------------------*/
/* Purpose        : 復号化処理 (メニューからの呼び出し)                 */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
void WINAPI Decript(HWND hWnd, LPARAM lParam)
{
    try {
        ExecDecript(hWnd, lParam);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //致命的エラー内容を出力
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : 復号化処理                                          */
/* Inputs         : --                                                  */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
static void ExecDecript(HWND hWnd, LPARAM lParam)
{
    //入出力ファイル作成
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");
    RemoveFiles(ascPath);
    RemoveFiles(msgPath);

    // 現在のメールidを取得
    LPCSTR mailid = NULL;
    mailid = g_Info.m_BkApi.GetCurrentMail();

    //現在のメールからヘッダ部分を抜き出す?
    //char* lpSource = g_Info.m_BkApi.GetSource(NULL); //メール内容 (未デコード) を取り出す
    char* lpSource = g_Info.m_BkApi.GetSource(mailid); //メール内容 (未デコード) を取り出す
    if(lpSource==NULL) {
        ErrorMessage(hWnd, IDS_GPG_ERROR);
        return;
    }
    CMIMEMessage item;
    item.FromString(lpSource);
    g_Info.m_BkApi.Free(lpSource); //もう不要なのでfree

    //PGP/MIMEに関連するフィールドを探す
    CGnuPGModule::EMODE mode = CGnuPGModule::Decript;
    CMIMEMessage* pTop = item.FindCMIMEMessage("multipart", "signed");
    if(pTop!=NULL) { //署名されたメール
        mode = CGnuPGModule::Verify;
    }
    else { //暗号化(PGP/MIME)されたメールか?
        pTop = item.FindCMIMEMessage("multipart", "encrypted");
        if(pTop==NULL) { // Thanx! Koji Hatanaka
            pTop = item.FindCMIMEMessage("application", "pgp-encrypted"); // Illegal type ?
            mode = CGnuPGModule::Decript2;
        }
    }

    if(pTop!=NULL) { //署名(PGP/MIME)された，または暗号化(PGP/MIME)されたメール
        //署名/暗号化されたメッセージを取り出す
        if((GetMIMEMessage(pTop, mode, ascPath, msgPath))==false) {
            ErrorMessage(hWnd, IDS_GPG_ERROR);
            RemoveFiles(ascPath); //念の為の後始末
            return;
        }
        //gpg処理
        g_Info.m_PassPhrase = "";
        if(mode==CGnuPGModule::Verify) { //署名検証
            ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str());
        }
        else if(mode==CGnuPGModule::Decript) { //復号化
            if((ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str()))==true) {
                char* lpArea = FileToString(msgPath);
                if (lpArea!=NULL) {
                    //Header
                    string szData = "multipart/mixed;\r\n boundary=\"";
                    szData += pTop->GetBoundary();
                    szData += "\"; \r\n";
                    pTop->SetHeader("Content-Type", szData.c_str()); //Content-Type を差し替える
                    CMIMEMessage itemDecrypt;
                    itemDecrypt.FromString(lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                    pTop->ClearBody();  //ボディをクリア
                    pTop->ClearChild(); //子パートをクリア
                    //pTop->ClearNext();  //子パートをクリア
                    pTop->AddChild(itemDecrypt); //子パートとしてセットする
                    lpArea = item.ToString();
                    //g_Info.m_BkApi.SetText(-2, lpArea); // for debug
                    g_Info.m_BkApi.SetSource("TEMP", lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                }
            }
        }
        else if(mode==CGnuPGModule::Decript2) { //復号化 (Illegal type ?)
            if((ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str()))==true) {
                char* lpArea = FileToString(msgPath);
                if (lpArea!=NULL) {
                    CMIMEMessage itemDecrypt;
                    itemDecrypt.FromString(lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                    pTop->OverWrite(itemDecrypt);
                    lpArea = item.ToString();
                    //g_Info.m_BkApi.SetText(-2, lpArea); // for debug
                    g_Info.m_BkApi.SetSource("TEMP", lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                }
            }
        }
        else { //公開鍵のインポート?
            ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
        }
        //後始末
        g_Info.m_PassPhrase = "";
        RemoveFiles(ascPath);
        RemoveFiles(msgPath);
    }
    else { //平文 (でももしかしたら署名・暗号化されてるかもしれない)
        char szMimeType[80];
        char* lpData = g_Info.m_BkApi.GetText(szMimeType, 80);
        char* lpStr;
        if(strncmp(lpData, "-----BEGIN PGP ", 15) == 0) { //署名または暗号化されている?
            lpStr = lpData;
        }
        else { //署名または暗号化されている部分がある?
            lpStr = strstr(lpData, "\n-----BEGIN PGP ");
            if(lpStr==NULL) {
                ErrorMessage(hWnd, IDS_GPG_ERROR);
                g_Info.m_BkApi.Free(lpData);
                return;
            }
            lpStr++;
        }
        char szCharSet[80];
        g_Info.m_BkApi.GetCharSet(NULL, szCharSet, 80); //テキストのキャラクタセットを取得する
        g_Info.m_PassPhrase = "";
        int count = 0;
        for(;;) {
            //PGPブロックを取得する
            char* lpNext = GetMessage(lpStr, mode, ascPath, szCharSet);
            if(lpNext==NULL) { //メッセージなし
                RemoveFiles(ascPath); //念の為の後始末
                break;
            }
            //gpg処理
            bool rtn;
            if(mode==CGnuPGModule::Verify) { //署名検証
                rtn = ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
            }
            else if(mode==CGnuPGModule::Decript) { //復号化
                rtn = ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str());
                if(rtn==true) {
                    char* lpArea = FileToString(msgPath);
                    if (lpArea!=NULL) {
                        if (stricmp(szCharSet, "ISO-2022-JP") == 0) {
                            char* lpSJIS = g_Info.m_BkApi.ISO_2022_JP(lpArea, FALSE);
                            g_Info.m_BkApi.SetText(-2, lpSJIS);
                            g_Info.m_BkApi.Free(lpSJIS);
                        }
                        else {
                            g_Info.m_BkApi.SetText(-2, lpArea);
                        }
                        g_Info.m_BkApi.Free(lpArea);
                    }
                }
            }
            else { //公開鍵のインポート?
                rtn = ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
            }
            //後始末
            RemoveFiles(ascPath);
            RemoveFiles(msgPath);
            count++;
            //後処理
            if(rtn==false) { //正常終了しなかった(中止を含む)
                break;
            }
            //他にもPGPブロックがある?
            if(lpNext[0]=='\0') { //メッセージの終端
                break;
            }
            lpStr = strstr(lpNext, "\n-----BEGIN PGP ");
            if(lpStr==NULL) { //これ以上はない
                break;
            }
            lpStr++;
        }
        g_Info.m_PassPhrase = "";
        g_LogFile.AppendValue("number of Decryption and Verification", count);
        if(count==0) {
            ErrorMessage(hWnd, IDS_GPG_ERROR);
        }
        g_Info.m_BkApi.Free(lpData);
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 復号化処理 (メニューからの呼び出し)                 */
/* Inputs         : 処理モード                                          */
/*                  PGP署名/暗号化ブロック格納ファイル                  */
/*                  検証テキスト格納ファイル                            */
/* Ootput         : なし                                                */
/*----------------------------------------------------------------------*/
static bool ExecGPG(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode, const char* ascPath, const char* msgPath)
{
    g_Info.m_UserID = ""; //ユーザIDは空白にしておく
    if((mode==CGnuPGModule::Decript || mode==CGnuPGModule::Decript2) && (g_Info.m_PassPhrase.length())==0) { //復号化を行う かつ パスワード未設定
        CPassPhraseDialog dlg; //PassPhrase ダイアログ
        if((dlg.ExecDialog(hWnd, lParam, g_Info, true))==false) {
            return false;
        }
    }

    bool rtn;
    switch(mode) {
    case CGnuPGModule::Verify:
        rtn = g_Info.m_GPG.ExecVerify(ascPath, msgPath);
        break;
    case CGnuPGModule::Decript:
    case CGnuPGModule::Decript2:
        rtn = g_Info.m_GPG.ExecDecrypt(ascPath, msgPath, g_Info.m_PassPhrase.c_str(), g_Info.m_bUseAgent);
        break;
    case CGnuPGModule::ImpPubKey:
    default :
        rtn = g_Info.m_GPG.ExecImport(ascPath);
        break;
    }
    if(rtn==false) {
        ErrorMessage(hWnd, IDS_GPG_NOTPROCESSED);
        return false;
    }

    CViewDialog vdlg;
    vdlg.ExecDialog(hWnd, lParam, g_Info, g_Info.m_GPG.GetOutputMsg());

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化メッセージ(PGP/MIME)の取り出し                */
/* Inputs         : メールメッセージ (Top)                              */
/*                  処理モード                                          */
/*                  PGP署名/暗号化ブロック格納ファイル                  */
/*                  検証テキスト格納ファイル                            */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static bool GetMIMEMessage(CMIMEMessage* pTop, CGnuPGModule::EMODE mode, string& ascPath, string& msgPath)
{
    if(pTop==NULL) {
        return false;
    }

    if(mode==CGnuPGModule::Verify) { //署名(PGP/MIME)されたメールとして処理
    //署名パートを探す
        CMIMEMessage* pSign = pTop->FindCMIMEMessage("application", "pgp-signature");
        if(pSign==NULL) { //署名パートがない
            g_LogFile.AppendMessage("CMIMEMessage::FindCMIMEMessage(): 署名パートがありませんでした");
            return false;
        }
        //署名パートを格納する
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("ファイルオープンエラー", ascPath);
            return false;
        }
        char* lpSign = pSign->ToString();
        char* lpPGP = strstr(lpSign, "-----BEGIN PGP "); //PGP署名(?)ブロックを格納
        if (lpPGP==NULL) {
            g_LogFile.AppendMessage("PGP署名ブロックがありませんでした");
            g_Info.m_BkApi.Free(lpSign);
            return false;
        }
        ascFile << lpPGP;
        ascFile.close();
        g_Info.m_BkApi.Free(lpSign);
    //被署名パートを探す
        CMIMEMessage* pContent = pTop->GetChild();
        while (stricmp(pContent->GetSubType().c_str(), "pgp-signature") == 0 && pContent) {
            pContent = pContent->GetNext();
        }
        if(pContent==NULL) { //被署名パートがなかった
            g_LogFile.AppendMessage("被署名パートがありませんでした");
            return false;
        }
        //被署名パートを格納する
        ofstream msgFile(msgPath.c_str(), ios_base::binary);
        if((msgFile.is_open())==false) {
            g_LogFile.AppendValue("ファイルオープンエラー", msgPath);
            return false;
        }
        char* lpBody = pContent->ToString();
        msgFile << lpBody;
        msgFile.close();
        g_Info.m_BkApi.Free(lpBody);
    }
    else if(mode==CGnuPGModule::Decript) { //(多分)暗号化(PGP/MIME)されたメール
    //暗号化されたパートを探す
        CMIMEMessage* pContent = pTop->FindCMIMEMessage("application", "octet-stream");
        if(pContent==NULL) { //暗号化されたパートがない
            g_LogFile.AppendMessage("暗号化されたパートがありませんでした");
            return false;
        }
        //暗号化されたブロックを格納
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("ファイルオープンエラー", ascPath);
            return false;
        }
        char* lpContent = pContent->ToString();
        if(lpContent==NULL) {
            g_LogFile.AppendMessage("暗号化されたパートがありませんでした");
            return false;
        }
        char* lpPGP = strstr(lpContent, "-----BEGIN PGP ");
        if(lpPGP==NULL) {
            g_LogFile.AppendMessage("暗号化されたブロックがありませんでした");
            g_Info.m_BkApi.Free(lpContent);
            return false;
        }
        ascFile << lpPGP;
        ascFile.close();
        g_Info.m_BkApi.Free(lpContent);
    }
    else { //CGnuPGModule::Decript2 : 暗号化(PGP/MIME)されたメール (Illegal type ?)
        CMIMEMessage* pContent = pTop;
        //暗号化されたブロックを格納
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("ファイルオープンエラー", ascPath);
            return false;
        }
        char* lpContent = pContent->ToString();
        if(lpContent==NULL) {
            g_LogFile.AppendMessage("暗号化されたパートがありませんでした");
            return false;
        }
        char* lpPGP = strstr(lpContent, "-----BEGIN PGP ");
        if(lpPGP==NULL) {
            g_LogFile.AppendMessage("暗号化されたブロックがありませんでした");
            g_Info.m_BkApi.Free(lpContent);
            return false;
        }
        ascFile << lpPGP;
        ascFile.close();
        g_Info.m_BkApi.Free(lpContent);
    }

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : 暗号化メッセージの取り出し                          */
/* Inputs         : メールメッセージ (本文)                             */
/*                  処理モード(Out)                                     */
/*                  PGP署名/暗号化ブロック格納ファイル                  */
/*                  キャラクタセット                                    */
/* Ootput         : 次のメッセージ                                      */
/*----------------------------------------------------------------------*/
static char* GetMessage(char* lpStr, CGnuPGModule::EMODE& mode, string& ascPath, char* szCharSet)
{
    //署名/暗号ブロックの種類を調べる
    if(strncmp(lpStr, "-----BEGIN PGP PUBLIC KEY BLOCK-----", 36) == 0) { //公開鍵
        mode = CGnuPGModule::ImpPubKey;
    }
    else if(strncmp(lpStr, "-----BEGIN PGP SIGNED MESSAGE-----",34) == 0) { //署名
        mode = CGnuPGModule::Verify;
    }
    else {
        mode = CGnuPGModule::Decript;
    }

    //PGPブロックの終わりを探す
    char* lpEnd = strstr(lpStr, "\n-----END PGP ");
    if(lpEnd==NULL) { //終わりの部分がなかった
        g_LogFile.AppendMessage("不完全なブロック");
        return NULL;
    }
    //行末を探す
    lpEnd++;
    char* lpTmp = strchr(lpEnd,'\n');
    if(lpTmp!=NULL) {
        lpEnd = lpTmp;
    }
    else {
        lpEnd = strchr(lpEnd, '\0');
    }

    //PGPブロックを格納する
    ofstream ascFile(ascPath.c_str(), ios_base::binary);
    if((ascFile.is_open())==false) {
        //ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        g_LogFile.AppendValue("ファイルオープンエラー", ascPath);
        return NULL;
    }
    int nLen = lpEnd - lpStr;
    if(stricmp(szCharSet, "ISO-2022-JP") == 0 || (szCharSet[0] == '\0' && GetACP() == 932)) { //日本語
        char* lpTemp = new char[nLen + 4]; //数字に根拠はない?
        strncpy(lpTemp, lpStr, nLen);
        lpTemp[nLen] = '\0';
        char* lpJIS = g_Info.m_BkApi.ISO_2022_JP(lpTemp, TRUE); //JISに変換する
        delete [] lpTemp;
        ascFile << lpJIS;
        g_Info.m_BkApi.Free(lpJIS);
    }
    else {
        ascFile << lpStr;
    }
    ascFile.close();

    return lpEnd;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
