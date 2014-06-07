/************************************************************************/
/*  File name : Encrypt.cpp                                             */
/************************************************************************/
/*  Contents  : �Í������� (�R�[���o�b�N�֐�)                           */
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

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/
static void ExecEncrypt(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode);
static void ExecSign(HWND hWnd, LPARAM lParam);
static bool GetMessage(HWND hWnd, string& msgPath, CMIMEMessage& item, CGnuPGModule::EMODE mode);


/*----------------------------------------------------------------------*/
/* Purpose        : �Í������� (���j���[����̌Ăяo��)                 */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void WINAPI Encrypt(HWND hWnd, LPARAM lParam)
{
    try {
        ExecEncrypt(hWnd, lParam, CGnuPGModule::Encrypt);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�v���I�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �Í������� (���j���[����̌Ăяo��)                 */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void WINAPI EncryptSign(HWND hWnd, LPARAM lParam)
{
    try {
        ExecEncrypt(hWnd, lParam, CGnuPGModule::EncryptSign);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�v���I�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �������� (���j���[����̌Ăяo��)                   */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void WINAPI Sign(HWND hWnd, LPARAM lParam)
{
    try {
        ExecSign(hWnd, lParam);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�v���I�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : ��������                                            */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
static void ExecSign(HWND hWnd, LPARAM lParam)
{
    //if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
    //    SendMessage(hWnd, WM_SET_TRANSFER_SAFE, (WPARAM)TRUE, 0);
    //}

    //���o�̓t�@�C���쐬
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");

    //�������郆�[�U���擾����
    CAddressList localUser;
    localUser.Clear();
    if((localUser.AddAddressFrom(hWnd, CAddressList::FROM_FIELD))==0) { //From�t�B�[���h�̒��g���Ȃ�
        ErrorMessage(hWnd, IDS_GPG_NOLOCALUSER);
        return;
    }
    vector<string>& rlist = localUser.GetList();
    vector<string>::iterator s = rlist.begin();
    g_Info.m_UserID = (*s); // From �t�B�[���h����擾

    //�p�X�t���[�Y�̓���
    g_Info.m_PassPhrase = "";
    CPassPhraseDialog dlg; //PassPhrase �_�C�A���O
    if((dlg.ExecDialog(hWnd, lParam, g_Info, false))==false) {
        return; //���[�U�ɂ�钆�~
    }

    //�Í������b�Z�[�W�̎��o��
    CMIMEMessage item;
    if((GetMessage(hWnd, msgPath, item, CGnuPGModule::Sign))==false) {
        ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        return;
    }

    //�Í�������
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

    //�Í������b�Z�[�W��ݒ肷��
    char* lpArea = FileToString(ascPath);
    if(lpArea!=NULL) {
        if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
            //MIME�o�E���_��������쐬
            char szBoundary[1024];
            time_t t;
            time(&t);
            sprintf(szBoundary, "===[PGP/MIME_RFC2015]===%08X.%04X===", (DWORD)t, rand());
            //MIME���b�Z�[�W���쐬����
            CBigBuffer Buffer; //���啶����o�b�t�@
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
                //�Ȃɂ����Ȃ�?
            }
            else {
                Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n"); //7bit�ɃZ�b�g
            }
            //Body
            //Buffer.AddStr("\r\n");
            //���b�Z�[�W �p�[�g
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            char* lpMsg = FileToString(msgPath);
            if(lpMsg!=NULL) {
                Buffer.AddStr(lpMsg); //���b�Z�[�W�u���b�N���Z�b�g
                g_Info.m_BkApi.Free(lpMsg);
            }
            //�����p�[�g
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
            //�}���`�p�[�g�̏I���
            szData = "\r\n--";
            szData += szBoundary;
            szData += "--\r\n";
            Buffer.AddStr(szData.c_str());
            //Import
            CMIMEMessage EncItem;
            EncItem.FromString(Buffer.Reference());
            Buffer.Reset(); //�����s�v
            item.OverWrite(EncItem); //���Ƃ�MIME���b�Z�[�W�ɏ㏑������
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
/* Purpose        : �Í�������                                          */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
static void ExecEncrypt(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode)
{
    //if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
    //    SendMessage(hWnd, WM_SET_TRANSFER_SAFE, (WPARAM)TRUE, 0);
    //}

    //���o�̓t�@�C���쐬
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");

    //�Í���������J���̃��[�U���擾����
    CAddressList recipient;
    recipient.Clear();
    recipient.AddAddressFrom(hWnd, CAddressList::TO_FIELD); //To �t�B�[���h�̃A�h���X���X�g���擾
    if(g_Info.m_bEnableCcUser==true) { //Cc �t�B�[���h�̃A�h���X���܂߂�
        recipient.AddAddressFrom(hWnd, CAddressList::CC_FIELD);
    }
    if(g_Info.m_bEnableBccUser==true) { //Bcc �t�B�[���h�̃A�h���X���܂߂�
        recipient.AddAddressFrom(hWnd, CAddressList::BCC_FIELD);
    }
    if(g_Info.m_bEnableFromUser==true) { //From �t�B�[���h�̃A�h���X���܂߂�
        recipient.AddAddressFrom(hWnd, CAddressList::FROM_FIELD);
    }
    if(g_Info.m_bEnableDefUser==true) { //�f�t�H���g���[�U���܂߂�
        recipient.AddAddressFrom(g_Info.m_GPG.GetDefUserID());
    }
    g_Info.m_UserID = recipient.GetSerialList();
    g_Info.m_PassPhrase = "";
    CUsersDialog dlg; //Users �_�C�A���O
    if((dlg.ExecDialog(hWnd, lParam, g_Info))==false) {
        return; //���[�U�ɂ�钆�~
    }
    recipient.Clear();
    if((recipient.AddAddressFrom(g_Info.m_UserID.c_str()))==0) { //�ЂƂ����[�UID���Ȃ�
        ErrorMessage(hWnd, IDS_GPG_NORECIPIENT);
        return;
    }

    if(mode==CGnuPGModule::EncryptSign) {
        //�������郆�[�U���擾����
        CAddressList localUser;
        localUser.Clear();
        if((localUser.AddAddressFrom(hWnd, CAddressList::FROM_FIELD))==0) { //From�t�B�[���h�̒��g���Ȃ�
            ErrorMessage(hWnd, IDS_GPG_NOLOCALUSER);
            return;
        }
        vector<string>& rlist = localUser.GetList();
        vector<string>::iterator s = rlist.begin();
        g_Info.m_UserID = (*s); // From �t�B�[���h����擾
        //�p�X�t���[�Y�̓���
        g_Info.m_PassPhrase = "";
        CPassPhraseDialog dlg; //PassPhrase �_�C�A���O
        if((dlg.ExecDialog(hWnd, lParam, g_Info, false))==false) {
            return; //���[�U�ɂ�钆�~
        }
    }

    //�Í������b�Z�[�W�̎��o��
    CMIMEMessage item;
    if((GetMessage(hWnd, msgPath, item, mode))==false) {
        ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        return;
    }

    //�Í�������
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

    //�Í������b�Z�[�W��ݒ肷��
    char* lpArea = FileToString(ascPath);
    if(lpArea!=NULL) {
        if(g_Info.m_GPG.IsPGPMIME()) { //PGP/MIME
            //MIME�o�E���_��������쐬
            char szBoundary[1024];
            time_t t;
            time(&t);
            sprintf(szBoundary, "===[PGP/MIME_RFC2015]===%08X.%04X===", (DWORD)t, rand());
            //MIME���b�Z�[�W���쐬����
            CBigBuffer Buffer; //���啶����o�b�t�@
            //�w�b�_
            string szData = "Content-Type: multipart/encrypted;\r\n boundary=\"";
            szData += szBoundary;
            szData += "\";\r\n protocol=\"application/pgp-encrypted\"\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n");
            //Body
            //Buffer.AddStr("\r\n");
            //encrypted �p�[�g
            szData = "\r\n--";
            szData += szBoundary;
            szData += "\r\n";
            Buffer.AddStr(szData.c_str());
            Buffer.AddStr("Content-Type: application/pgp-encrypted\r\n");
            Buffer.AddStr("Content-Transfer-Encoding: 7bit\r\n\r\n");
            Buffer.AddStr("Version: 1\r\n"); //��芸�����Œ�l
            //�Í����u���b�N
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
            //�}���`�p�[�g�̏I���
            szData = "\r\n--";
            szData += szBoundary;
            szData += "--\r\n";
            Buffer.AddStr(szData.c_str());
            //Import
            CMIMEMessage EncItem;
            EncItem.FromString(Buffer.Reference());
            Buffer.Reset(); //�����s�v
            item.OverWrite(EncItem); //���Ƃ�MIME���b�Z�[�W�ɏ㏑������
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
/* Purpose        : �Í������b�Z�[�W�̎��o��                          */
/* Inputs         : �Í����Ώۂ̃��b�Z�[�W���i�[����t�@�C���ւ̃p�X    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static bool GetMessage(HWND hWnd, string& msgPath, CMIMEMessage& item, CGnuPGModule::EMODE mode)
{
    //�t�@�C���I�[�v��
    RemoveFiles(msgPath); //�ȑO�̃t�@�C���͍폜����
    ofstream msgFile(msgPath.c_str(), ios_base::binary);
    if((msgFile.is_open())==false) {
        g_LogFile.AppendValue("Cannot file Open", msgPath);
        return false;
    }

    //�Í����Ώۂ̃��b�Z�[�W���t�@�C���Ɋi�[����
    if(g_Info.m_GPG.IsPGPMIME()==true) { //PGP/MIME�Ή�
        char* lpSrc = g_Info.m_BkApi.CompGetSource(hWnd); //�쐬���̃��b�Z�[�W���擾
        if(lpSrc==NULL) {
            g_LogFile.AppendMessage("No MIME message");
            msgFile.close();
            RemoveFiles(msgPath); //�O�ׂ̈̌�n��
            return false;
        }
        item.FromString(lpSrc);
        g_Info.m_BkApi.Free(lpSrc);
        //Body Part �̎��o��
        CMIMEMessage BodyPart;
        item.GetBody(BodyPart);
        msgFile << BodyPart.ToString();
    }
    else { //PGP
        char szMimeType[80];
        char* lpBody = g_Info.m_BkApi.CompGetText(hWnd, szMimeType, 80); //�쐬���̃��b�Z�[�W(Text)���擾
        char szCharSet[82];
        g_Info.m_BkApi.CompGetCharSet(hWnd, szCharSet, 80); //�L�����N�^�Z�b�g���擾

        if(stricmp(szCharSet, "ISO-2022-JP") == 0 || (szCharSet[0] == '\0' && GetACP() == 932)) { //���{��̕���
            char* lpJIS = g_Info.m_BkApi.ISO_2022_JP(lpBody, TRUE); //JIS�R�[�h�ɕϊ�����
            msgFile << lpJIS;
            g_Info.m_BkApi.Free(lpJIS);
            //if(g_Info.m_GPG.IsPGPCompatible()==true && mode==CGnuPGModule::Sign) { //PGP�݊��̃N���A���� �� GnuPG 1.0.4 �ŉ��P?
            //    msgFile << "\r\n";
            //}
        }
        else { //���{��ȊO�̕���
            msgFile << lpBody; //���̂܂܊i�[
        }
        g_Info.m_BkApi.Free(lpBody);
    }
    msgFile.close();

    return true;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
