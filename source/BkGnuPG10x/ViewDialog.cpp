/************************************************************************/
/*  File name : ViewDialog.cpp                                          */
/************************************************************************/
/*  Contents  : 結果出力ダイアログ                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "ViewDialog.h"
#include "resource.h"
#include "CallBacks.h"

#include "debug.h" //最後に定義


/**** Internal Macro ****/


/**** Typedef ****/


/**** Internal Valiable ****/
static CViewDialog* viewIndo = NULL;


/**** Prototyping ****/
static BOOL CALLBACK GPGViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*----------------------------------------------------------------------*/
/* Purpose        : デフォルトコンストラクタ                            */
/* Inputs         : なし                                                */
/*----------------------------------------------------------------------*/
CViewDialog::CViewDialog(void)
{
    m_Message = "";
}

/*----------------------------------------------------------------------*/
/* Purpose        : ダイアログの起動                                    */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CViewDialog::ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, const char* outputMsg)
{
    //初期化処理
    if(g_Info.m_CharSet==CGnuPGModule::ISO_2022_IP) { //JISコードで出力
        char* lpSJIS = g_Info.m_BkApi.ISO_2022_JP(outputMsg, FALSE); //JIS→SJIS変換
        m_Message = lpSJIS;
        g_Info.m_BkApi.Free(lpSJIS);
    }
    else if(g_Info.m_CharSet==CGnuPGModule::EUC_JP) { //EUCコードで出力
        char* lpSJIS = g_Info.m_BkApi.EUC_JP(outputMsg, FALSE); //EUC→SJIS変換
        m_Message = lpSJIS;
        g_Info.m_BkApi.Free(lpSJIS);
    }
    else if(g_Info.m_CharSet==CGnuPGModule::UTF_8) { //UTF-8コードで出力
        char* lpSJIS = g_Info.m_BkApi.UTF_8(outputMsg, FALSE); //UTF8→SJIS変換
        m_Message = lpSJIS;
        g_Info.m_BkApi.Free(lpSJIS);
    }
    else { //変換しない
        m_Message = outputMsg;
    }
    //ダイアログ起動(モーダルダイアログ)
    viewIndo = this;
    int rtn = DialogBox(info.m_hInstance, MAKEINTRESOURCE(IDD_VIEWDIALOG), hWnd, (DLGPROC)GPGViewProc);
    viewIndo = NULL;
    //結果処理
    if(rtn<0) {
        return false;
    }
    else if(rtn==IDOK) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : View Dialog Callback                                */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static BOOL CALLBACK GPGViewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg){
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hWnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
        }
        break;

    case WM_INITDIALOG:
        if(viewIndo!=NULL) {
            SetDlgItemText(hWnd, IDC_EDIT_VIEW, viewIndo->GetOutputMsg());
        }
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
