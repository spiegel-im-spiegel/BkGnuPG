/************************************************************************/
/*  File name : PubKey.cpp                                              */
/************************************************************************/
/*  Contents  : ���J������ (�R�[���o�b�N�֐�)                           */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA    Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGDef.h"
#include "BkGnuPGInfo.h"
#include "CallBacks.h"
//#include "../BkCommon.h"
#include "AddressList.h"
#include "ViewDialog.h"
#include "LogFile.h"

#include <string>
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/
static void ExecPubKeyExport(HWND hWnd, LPARAM lParam);


/*----------------------------------------------------------------------*/
/* Purpose        : ���J���̓Y�t���� (���j���[����̌Ăяo��)           */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void WINAPI PubKeyExport(HWND hWnd, LPARAM lParam)
{
    try {
        ExecPubKeyExport(hWnd, lParam);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�v���I�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���J���̓Y�t����                                    */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
static void ExecPubKeyExport(HWND hWnd, LPARAM lParam)
{
    //���J�L�[�����o�����[�U���擾����
    CAddressList localUser;
    localUser.Clear();
    if((localUser.AddAddressFrom(hWnd, CAddressList::FROM_FIELD))==0) { //From�t�B�[���h�̒��g���Ȃ�
        g_LogFile.AppendMessage("���[�UID���ЂƂ�����܂���");
        return;
    }
    vector<string>& rlist = localUser.GetList();
    vector<string>::iterator s = rlist.begin();
    g_Info.m_UserID = (*s); // From �t�B�[���h����擾

    //���o�̓t�@�C���쐬
    string ascPath;
    string ascFile;
    GetLegalFileName(g_Info.m_UserID.c_str(), ascFile);
    if((ascFile.length())>0) {
        g_Info.SetTempPath(g_Info.m_BkApi.GetTempFolder());
        ascPath = g_Info.m_TempDir + ascFile + ".asc";
    }
    else {
        ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    }

    //���J���擾����
    RemoveFiles(ascPath);
    if((g_Info.m_GPG.ExecExport(ascPath.c_str(), g_Info.m_UserID.c_str()))==false) {
        ErrorMessage(hWnd, IDS_GPG_NOTPROCESSED);
        g_Info.m_PassPhrase = "";
        RemoveFiles(ascPath);
        return;
    }
    else {
        g_Info.m_PassPhrase = "";
        CViewDialog vdlg;
        vdlg.ExecDialog(hWnd, lParam, g_Info, g_Info.m_GPG.GetOutputMsg());
    }

    //�Í������b�Z�[�W��ݒ肷��
    if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
        g_Info.m_BkApi.CompAttachFile(hWnd, ascPath.c_str(), "application/pgp-keys");
    }
    else {
        LPSTR lpArea = FileToString(ascPath);
        if(lpArea!=NULL) {
            g_Info.m_BkApi.CompSetText(hWnd, 1, lpArea);
            g_Info.m_BkApi.Free(lpArea);
        }
        else {
            char buf[256];
            LoadString(g_Info.m_hInstance, IDS_GPG_FAILATTACHPUBKEY, buf, 255);
            g_LogFile.AppendMessage(buf);
        }
        g_Info.m_BkApi.Command(hWnd, "TextTop");
    }
    RemoveFiles(ascPath);
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
