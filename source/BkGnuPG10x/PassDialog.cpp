/************************************************************************/
/*  File name : PassDialog.cpp                                          */
/************************************************************************/
/*  Contents  : PassPhrase ダイアログ                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "PassDialog.h"
#include "resource.h"
#include "CallBacks.h"
#include "SetupDialog.h"

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Internal Valiable ****/
CPassPhraseDialog* CPassPhraseDialog::m_Instance = NULL; //このクラスのインスタンス


/**** Prototyping ****/
static BOOL CALLBACK GPGPassPhraseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*----------------------------------------------------------------------*/
/* Purpose        : デフォルトコンストラクタ                            */
/* Inputs         : なし                                                */
/*----------------------------------------------------------------------*/
CPassPhraseDialog::CPassPhraseDialog(void) : m_Info(NULL), m_bFixed(TRUE)
{
    m_UserID = "";
    m_PassPhrase = "";
}

/*----------------------------------------------------------------------*/
/* Purpose        : ダイアログの起動                                    */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CPassPhraseDialog::ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, bool bFixedID)
{
    bool tmpUseAgent = info.m_bUseAgent;

    if(m_Instance!=NULL) { //競合状態
        return false;
    }
    m_Instance = this; //シビアなタイミングではないので，これでもよい
    //初期化処理
    m_Info = &info;
    m_UserID = info.m_UserID;
    m_PassPhrase = info.m_PassPhrase;
    m_bShowText = info.m_bShowText;
    m_bUseAgent = info.m_bUseAgent;
    m_bFixed = bFixedID ? TRUE:FALSE;
    //ダイアログ起動(モーダルダイアログ)
    int rtn = DialogBox(info.m_hInstance, MAKEINTRESOURCE(IDD_GPG_PASSPHRASEDIALOG), hWnd, (DLGPROC)GPGPassPhraseProc);
    m_Instance = NULL;
    //結果処理
    if(rtn<0) {
        return false;
    }
    if(tmpUseAgent != m_bUseAgent) {
        info.m_bUseAgent = m_bUseAgent;
        info.SetUseAgentFlag(); // Write INI File
    }
    if(rtn==IDOK) {
        //結果の書き戻し
        if(bFixedID==false) {
            info.m_UserID = m_UserID;
        }
        info.m_PassPhrase = m_PassPhrase;
        info.m_bShowText = m_bShowText;
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : PassPhrase Dialog Callback                          */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static BOOL CALLBACK GPGPassPhraseProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CPassPhraseDialog* passInfo = CPassPhraseDialog::GetInstatnce();
    switch (msg){
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            if(passInfo!=NULL) {
                char buf[8192];
                if((GetDlgItemText(hWnd, IDC_EDIT_PASSPHRASE, buf, 8192))>0) {
                    passInfo->SetPassPhrase(buf);
                }
                else {
                    passInfo->SetPassPhrase("");
                }
                if((GetDlgItemText(hWnd, IDC_EDIT_USERID, buf, 8192))>0) {
                    passInfo->SetUserID(buf);
                }
                else {
                    passInfo->SetUserID("");
                }
                passInfo->SetOptShowText( (SendDlgItemMessage(hWnd, IDC_CHECK_SHOWTEXT, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                passInfo->SetUseAgent( (SendDlgItemMessage(hWnd, IDC_CHECK_USEAGENT, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
            }
            EndDialog(hWnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON_SETTING) {
            if(HIWORD(wParam) == BN_CLICKED) { //ボタンが押された
                CSetupDialog dlg;
                dlg.ExecDialog(hWnd, 0, *(passInfo->GetBkGnuPGInfo())); //Setup Dialog を起動
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_SHOWTEXT) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_SHOWTEXT, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_EDIT_PASSPHRASE, EM_SETPASSWORDCHAR, 0, 0);
                    SetDlgItemText(hWnd, IDC_EDIT_PASSPHRASE, passInfo->GetPassPhrase());
                }
                else {
                    SendDlgItemMessage(hWnd, IDC_EDIT_PASSPHRASE, EM_SETPASSWORDCHAR, '*', 0);
                    SetDlgItemText(hWnd, IDC_EDIT_PASSPHRASE, passInfo->GetPassPhrase());
                }
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_USEAGENT) {
            if (HIWORD(wParam) == BN_CLICKED) {
                BOOL bOn = (SendDlgItemMessage(hWnd, IDC_CHECK_USEAGENT, BM_GETCHECK, 0, 0) == BST_CHECKED);
                passInfo->SetUseAgent(bOn ? true : false);
                EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PASSPHRASE), !bOn);
                EnableWindow(GetDlgItem(hWnd, IDC_CHECK_SHOWTEXT), !bOn);
            }
        }
        break;

    case WM_INITDIALOG:
        if(passInfo!=NULL) {
            SetDlgItemText(hWnd, IDC_EDIT_PASSPHRASE, passInfo->GetPassPhrase());
            SetDlgItemText(hWnd, IDC_EDIT_USERID, passInfo->GetUserID());
            SendDlgItemMessage(hWnd, IDC_EDIT_USERID, EM_SETREADONLY, passInfo->IsFixed(), 0);
            EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_SETTING), !(passInfo->IsFixed()));
            if((passInfo->IsShowText())==true) {
                SendDlgItemMessage(hWnd, IDC_CHECK_SHOWTEXT, BM_SETCHECK, BST_CHECKED, 0);
                SendDlgItemMessage(hWnd, IDC_EDIT_PASSPHRASE, EM_SETPASSWORDCHAR, 0, 0);
            }
            else {
                SendDlgItemMessage(hWnd, IDC_CHECK_SHOWTEXT, BM_SETCHECK, BST_UNCHECKED, 0);
                SendDlgItemMessage(hWnd, IDC_EDIT_PASSPHRASE, EM_SETPASSWORDCHAR, '*', 0);
            }
            if (passInfo->UseAgent()) {
                EnableWindow(GetDlgItem(hWnd, IDC_EDIT_PASSPHRASE), FALSE);
                EnableWindow(GetDlgItem(hWnd, IDC_CHECK_SHOWTEXT), FALSE);
                SendDlgItemMessage(hWnd, IDC_CHECK_USEAGENT, BM_SETCHECK, BST_CHECKED, 0);
            }
        }
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
