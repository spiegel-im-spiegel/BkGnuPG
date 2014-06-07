/************************************************************************/
/*  File name : UsersDialog.cpp                                         */
/************************************************************************/
/*  Contents  : PassPhrase �_�C�A���O                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.12          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/

/**** Incude Files ****/
#include "UsersDialog.h"
#include "resource.h"
#include "CallBacks.h"
#include "SetupDialog.h"

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Internal Valiable ****/
//static CUsersDialog* usersInfo = NULL;
CUsersDialog* CUsersDialog::m_Instance = NULL; //���̃N���X�̃C���X�^���X


/**** Prototyping ****/
static BOOL CALLBACK GPGUsersProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/*----------------------------------------------------------------------*/
/* Purpose        : �f�t�H���g�R���X�g���N�^                            */
/* Inputs         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
CUsersDialog::CUsersDialog(void) : m_Info(NULL)
{
    m_UserID = "";
}

/*----------------------------------------------------------------------*/
/* Purpose        : �_�C�A���O�̋N��                                    */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CUsersDialog::ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info)
{
    if(m_Instance!=NULL) { //�������
        return false;
    }
    else {
        m_Instance = this; //�V�r�A�ȃ^�C�~���O�ł͂Ȃ��̂ŁC����ł��悢
    }
    //����������
    m_Info = &info;
    m_UserID = info.m_UserID;
    //�_�C�A���O�N��(���[�_���_�C�A���O)
    //usersInfo = this;
    int rtn = DialogBox(info.m_hInstance, MAKEINTRESOURCE(IDD_GPG_RUSERSDIALOG), hWnd, (DLGPROC)GPGUsersProc);
    m_Instance = NULL;
    //usersInfo = NULL;
    //���ʏ���
    if(rtn<0) {
        return false;
    }
    else if(rtn==IDOK) {
        //���ʂ̏����߂�
        info.m_UserID = m_UserID;
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : Users Dialog Callback                               */
/* Inputs         : --                                                  */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static BOOL CALLBACK GPGUsersProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CUsersDialog* usersInfo = CUsersDialog::GetInstatnce();
    switch (msg){
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            if(usersInfo!=NULL) {
                char buf[8192];
                if((GetDlgItemText(hWnd, IDC_EDIT_USERID, buf, 8192))>0) {
                    usersInfo->SetUserID(buf);
                }
                else {
                    usersInfo->SetUserID("");
                }
            }
            EndDialog(hWnd, IDOK);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hWnd, IDCANCEL);
            return TRUE;
        }
        else if (LOWORD(wParam) == IDC_BUTTON_SETTING) {
            if(HIWORD(wParam) == BN_CLICKED) { //�{�^���������ꂽ
                CSetupDialog dlg;
                dlg.ExecDialog(hWnd, 0, *(usersInfo->GetBkGnuPGInfo())); //Setup Dialog ���N��
            }
        }
        break;

    case WM_INITDIALOG:
        if(usersInfo!=NULL) {
            SetDlgItemText(hWnd, IDC_EDIT_USERID, usersInfo->GetUserID());
        }
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
