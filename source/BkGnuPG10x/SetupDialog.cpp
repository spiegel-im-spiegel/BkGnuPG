/************************************************************************/
/*  File name : SetupDialog.cpp                                         */
/************************************************************************/
/*  Contents  : Setting ダイアログ                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "SetupDialog.h"
#include "resource.h"
#include "GnuPGModule.h"
using namespace std;

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Internal Valiable ****/
CSetupDialog* CSetupDialog::m_Instance = NULL; //このクラスのインスタンス


/**** Prototyping ****/
static BOOL CALLBACK GPGSetupProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*----------------------------------------------------------------------*/
/* Purpose        : デフォルトコンストラクタ                            */
/* Inputs         : なし                                                */
/*----------------------------------------------------------------------*/
CSetupDialog::CSetupDialog(void)
    : m_bPGPMIME(true), m_bPGPCompatible(true),
        m_bOpenPGP(false), m_bThrowKeyID(false),
        m_bEnableCcUser(false), m_bEnableBccUser(false)
{
    m_CmdPath = CGnuPGModule::GetDefModuleName();
}

/*----------------------------------------------------------------------*/
/* Purpose        : ダイアログの起動                                    */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CSetupDialog::ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info)
{
    if(m_Instance!=NULL) { //競合状態
        return false;
    }
    else {
        m_Instance = this; //シビアなタイミングではないので，これでもよい
    }

    //初期化処理
    //m_CmdPath = info.m_GPG.GetModuleName();
    m_DefUserID = info.m_GPG.GetDefUserID();
    m_bPGPMIME = info.m_GPG.IsPGPMIME();
    m_bPGPCompatible = info.m_GPG.IsPGPCompatible();
    m_bOpenPGP = info.m_GPG.IsOpenPGP();
    m_bThrowKeyID = info.m_GPG.IsThrowKeyID();
    m_bRFC1991 = info.m_GPG.IsRFC1991();
    m_bMD5 = info.m_GPG.IsMD5();
    m_bEnableCcUser = info.m_bEnableCcUser;
    m_bEnableBccUser = info.m_bEnableBccUser;
    m_bEnableFromUser = info.m_bEnableFromUser;
    m_bEnableDefUser = info.m_bEnableDefUser;
    //ダイアログ起動(モーダルダイアログ)
    int rtn = DialogBox(info.m_hInstance, MAKEINTRESOURCE(IDD_GPG_SETUPDIALOG), hWnd, (DLGPROC)GPGSetupProc);
    m_Instance = NULL;
    //結果処理
    if(rtn<0) {
        return false;
    }
    else if(rtn==IDOK) {
        //結果の書き戻し
        //info.m_GPG.SetModuleName(m_CmdPath.c_str());
        info.m_GPG.SetDefUserID(m_DefUserID.c_str());
        info.m_GPG.SetOptPGPMIME(m_bPGPMIME);
        info.m_GPG.SetOptPGPCompatible(m_bPGPCompatible);
        info.m_GPG.SetOptOpenPGP(m_bOpenPGP);
        info.m_GPG.SetOptThrowKeyID(m_bThrowKeyID);
        info.m_GPG.SetOptRFC1991(m_bRFC1991);
        info.m_GPG.SetOptMD5(m_bMD5);
        info.m_bEnableCcUser = m_bEnableCcUser;
        info.m_bEnableBccUser = m_bEnableBccUser;
        info.m_bEnableFromUser = m_bEnableFromUser;
        info.m_bEnableDefUser = m_bEnableDefUser;
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : Setup Dialog Callback                               */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static BOOL CALLBACK GPGSetupProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CSetupDialog* setupInfo = CSetupDialog::GetInstatnce();
    switch (msg){
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            if(setupInfo!=NULL) {
                char buf[8192];
                //if((GetDlgItemText(hWnd, IDC_EDIT_CMDPATH, buf, 8192))>0) {
                //    setupInfo->SetCmdPath(buf);
                //}
                //else {
                //    setupInfo->SetCmdPath(CGnuPGModule::GetDefModuleName());
                //}
                if((GetDlgItemText(hWnd, IDC_EDIT_DEFUSERID, buf, 8192))>0) {
                    setupInfo->SetDefUserID(buf);
                }
                else {
                    setupInfo->SetDefUserID("");
                }
                setupInfo->SetOptPGPMIME( (SendDlgItemMessage(hWnd, IDC_CHECK_PGPMIME, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                if(SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    setupInfo->SetOptRFC1991( (SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                    setupInfo->SetOptPGPCompatible(true);
                    setupInfo->SetOptOpenPGP(false);
                    setupInfo->SetOptThrowKeyID(false);
                    setupInfo->SetOptMD5( (SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                }
                else {
                    setupInfo->SetOptPGPCompatible(false);
                    setupInfo->SetOptRFC1991(false);
                    setupInfo->SetOptOpenPGP( (SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                    if(SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                        setupInfo->SetOptMD5(false);
                    }
                    else {
                        setupInfo->SetOptMD5( (SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                    }
                    setupInfo->SetOptThrowKeyID( (SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                }
                setupInfo->SetOptEnableCcUser( (SendDlgItemMessage(hWnd, IDC_CHECK_CCENABLE, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                setupInfo->SetOptEnableBccUser( (SendDlgItemMessage(hWnd, IDC_CHECK_BCCENABLE, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                setupInfo->SetOptEnableFromUser( (SendDlgItemMessage(hWnd, IDC_CHECK_FROMENABLE, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
                setupInfo->SetOptEnableDefUser( (SendDlgItemMessage(hWnd, IDC_CHECK_DEFUSRENABLE, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true:false );
            }
            EndDialog(hWnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_CHECK_CMP_PGP) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_SETCHECK, BST_UNCHECKED, 0);
                    SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_SETCHECK, BST_UNCHECKED, 0);
                }
                else { //チェックが外れている
                    SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_RFC1991) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_SETCHECK, BST_CHECKED, 0);
                    SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_SETCHECK, BST_UNCHECKED, 0);
                    SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_OpenPGP) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_SETCHECK, BST_UNCHECKED, 0);
                    SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_SETCHECK, BST_UNCHECKED, 0);
                    SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_THROWKEYID) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_SETCHECK, BST_UNCHECKED, 0); //チェックを外す
                    SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        else if (LOWORD(wParam) == IDC_CHECK_MD5) {
            if(HIWORD(wParam) == BN_CLICKED) { //クリックされた
                if(SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_SETCHECK, BST_UNCHECKED, 0);
                }
            }
        }
        break;

    case WM_INITDIALOG:
        if(setupInfo!=NULL) {
            SetDlgItemText(hWnd, IDC_EDIT_CMDPATH, setupInfo->GetCmdPath());
            SetDlgItemText(hWnd, IDC_EDIT_DEFUSERID, setupInfo->GetDefUserID());
            WPARAM chk = (setupInfo->IsPGPMIME()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_PGPMIME, BM_SETCHECK, chk, 0);
            chk = (setupInfo->IsPGPCompatible()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_CMP_PGP, BM_SETCHECK, chk, 0);
            if(chk==BST_CHECKED) {
                chk = (setupInfo->IsRFC1991()) ? BST_CHECKED:BST_UNCHECKED;
                SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_SETCHECK, chk, 0);
                SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_SETCHECK, BST_UNCHECKED, 0);
                SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_SETCHECK, BST_UNCHECKED, 0);
                chk = (setupInfo->IsMD5()) ? BST_CHECKED:BST_UNCHECKED;
                SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_SETCHECK, chk, 0);
            }
            else {
                chk = (setupInfo->IsOpenPGP()) ? BST_CHECKED:BST_UNCHECKED;
                SendDlgItemMessage(hWnd, IDC_CHECK_OpenPGP, BM_SETCHECK, chk, 0);
                if(chk==BST_CHECKED) {
                    SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_SETCHECK, BST_UNCHECKED, 0);
                }
                else {
                    chk = (setupInfo->IsMD5()) ? BST_CHECKED:BST_UNCHECKED;
                    SendDlgItemMessage(hWnd, IDC_CHECK_MD5, BM_SETCHECK, chk, 0);
                }
                chk = (setupInfo->IsThrowKeyID()) ? BST_CHECKED:BST_UNCHECKED;
                SendDlgItemMessage(hWnd, IDC_CHECK_THROWKEYID, BM_SETCHECK, chk, 0);
                SendDlgItemMessage(hWnd, IDC_CHECK_RFC1991, BM_SETCHECK, BST_UNCHECKED, 0);
            }
            chk = (setupInfo->IsEnableCcUser()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_CCENABLE, BM_SETCHECK, chk, 0);
            chk = (setupInfo->IsEnableBccUser()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_BCCENABLE, BM_SETCHECK, chk, 0);
            chk = (setupInfo->IsEnableFromUser()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_FROMENABLE, BM_SETCHECK, chk, 0);
            chk = (setupInfo->IsEnableDefUser()) ? BST_CHECKED:BST_UNCHECKED;
            SendDlgItemMessage(hWnd, IDC_CHECK_DEFUSRENABLE, BM_SETCHECK, chk, 0);
        }
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
