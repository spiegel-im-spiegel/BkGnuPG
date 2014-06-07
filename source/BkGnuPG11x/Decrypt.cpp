/************************************************************************/
/*  File name : Decrypt.cpp                                             */
/************************************************************************/
/*  Contents  : ���������� (�R�[���o�b�N�֐�)                           */
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

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Prototyping ****/
static void ExecDecript(HWND hWnd, LPARAM lParam);
static bool ExecGPG(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode, const char* ascPath, const char* txtPath);
static bool GetMIMEMessage(CMIMEMessage* pTop, CGnuPGModule::EMODE mode, string& ascPath, string& msgPath);
static char* GetMessage(char* lpStr, CGnuPGModule::EMODE& mode, string& ascPath, char* szCharSet);


/*----------------------------------------------------------------------*/
/* Purpose        : ���������� (���j���[����̌Ăяo��)                 */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void WINAPI Decript(HWND hWnd, LPARAM lParam)
{
    try {
        ExecDecript(hWnd, lParam);
    }
    catch(...) {
        FatalErrorMessage(hWnd, __FILE__, __LINE__); //�v���I�G���[���e���o��
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : ����������                                          */
/* Inputs         : --                                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
static void ExecDecript(HWND hWnd, LPARAM lParam)
{
    //���o�̓t�@�C���쐬
    string ascPath = g_Info.m_BkApi.GetTempFileName("asc");
    string msgPath = g_Info.m_BkApi.GetTempFileName("msg");
    RemoveFiles(ascPath);
    RemoveFiles(msgPath);

    // ���݂̃��[��id���擾
    LPCSTR mailid = NULL;
    mailid = g_Info.m_BkApi.GetCurrentMail();

    //���݂̃��[������w�b�_�����𔲂��o��?
    //char* lpSource = g_Info.m_BkApi.GetSource(NULL); //���[�����e (���f�R�[�h) �����o��
    char* lpSource = g_Info.m_BkApi.GetSource(mailid); //���[�����e (���f�R�[�h) �����o��
    if(lpSource==NULL) {
        ErrorMessage(hWnd, IDS_GPG_ERROR);
        return;
    }
    CMIMEMessage item;
    item.FromString(lpSource);
    g_Info.m_BkApi.Free(lpSource); //�����s�v�Ȃ̂�free

    //PGP/MIME�Ɋ֘A����t�B�[���h��T��
    CGnuPGModule::EMODE mode = CGnuPGModule::Decript;
    CMIMEMessage* pTop = item.FindCMIMEMessage("multipart", "signed");
    if(pTop!=NULL) { //�������ꂽ���[��
        mode = CGnuPGModule::Verify;
    }
    else { //�Í���(PGP/MIME)���ꂽ���[����?
        pTop = item.FindCMIMEMessage("multipart", "encrypted");
        if(pTop==NULL) { // Thanx! Koji Hatanaka
            pTop = item.FindCMIMEMessage("application", "pgp-encrypted"); // Illegal type ?
            mode = CGnuPGModule::Decript2;
        }
    }

    if(pTop!=NULL) { //����(PGP/MIME)���ꂽ�C�܂��͈Í���(PGP/MIME)���ꂽ���[��
        //����/�Í������ꂽ���b�Z�[�W�����o��
        if((GetMIMEMessage(pTop, mode, ascPath, msgPath))==false) {
            ErrorMessage(hWnd, IDS_GPG_ERROR);
            RemoveFiles(ascPath); //�O�ׂ̈̌�n��
            return;
        }
        //gpg����
        g_Info.m_PassPhrase = "";
        if(mode==CGnuPGModule::Verify) { //��������
            ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str());
        }
        else if(mode==CGnuPGModule::Decript) { //������
            if((ExecGPG(hWnd, lParam, mode, ascPath.c_str(), msgPath.c_str()))==true) {
                char* lpArea = FileToString(msgPath);
                if (lpArea!=NULL) {
                    //Header
                    string szData = "multipart/mixed;\r\n boundary=\"";
                    szData += pTop->GetBoundary();
                    szData += "\"; \r\n";
                    pTop->SetHeader("Content-Type", szData.c_str()); //Content-Type �������ւ���
                    CMIMEMessage itemDecrypt;
                    itemDecrypt.FromString(lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                    pTop->ClearBody();  //�{�f�B���N���A
                    pTop->ClearChild(); //�q�p�[�g���N���A
                    //pTop->ClearNext();  //�q�p�[�g���N���A
                    pTop->AddChild(itemDecrypt); //�q�p�[�g�Ƃ��ăZ�b�g����
                    lpArea = item.ToString();
                    //g_Info.m_BkApi.SetText(-2, lpArea); // for debug
                    g_Info.m_BkApi.SetSource("TEMP", lpArea);
                    g_Info.m_BkApi.Free(lpArea);
                }
            }
        }
        else if(mode==CGnuPGModule::Decript2) { //������ (Illegal type ?)
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
        else { //���J���̃C���|�[�g?
            ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
        }
        //��n��
        g_Info.m_PassPhrase = "";
        RemoveFiles(ascPath);
        RemoveFiles(msgPath);
    }
    else { //���� (�ł������������珐���E�Í�������Ă邩������Ȃ�)
        char szMimeType[80];
        char* lpData = g_Info.m_BkApi.GetText(szMimeType, 80);
        char* lpStr;
        if(strncmp(lpData, "-----BEGIN PGP ", 15) == 0) { //�����܂��͈Í�������Ă���?
            lpStr = lpData;
        }
        else { //�����܂��͈Í�������Ă��镔��������?
            lpStr = strstr(lpData, "\n-----BEGIN PGP ");
            if(lpStr==NULL) {
                ErrorMessage(hWnd, IDS_GPG_ERROR);
                g_Info.m_BkApi.Free(lpData);
                return;
            }
            lpStr++;
        }
        char szCharSet[80];
        g_Info.m_BkApi.GetCharSet(NULL, szCharSet, 80); //�e�L�X�g�̃L�����N�^�Z�b�g���擾����
        g_Info.m_PassPhrase = "";
        int count = 0;
        for(;;) {
            //PGP�u���b�N���擾����
            char* lpNext = GetMessage(lpStr, mode, ascPath, szCharSet);
            if(lpNext==NULL) { //���b�Z�[�W�Ȃ�
                RemoveFiles(ascPath); //�O�ׂ̈̌�n��
                break;
            }
            //gpg����
            bool rtn;
            if(mode==CGnuPGModule::Verify) { //��������
                rtn = ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
            }
            else if(mode==CGnuPGModule::Decript) { //������
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
            else { //���J���̃C���|�[�g?
                rtn = ExecGPG(hWnd, lParam, mode, ascPath.c_str(), NULL);
            }
            //��n��
            RemoveFiles(ascPath);
            RemoveFiles(msgPath);
            count++;
            //�㏈��
            if(rtn==false) { //����I�����Ȃ�����(���~���܂�)
                break;
            }
            //���ɂ�PGP�u���b�N������?
            if(lpNext[0]=='\0') { //���b�Z�[�W�̏I�[
                break;
            }
            lpStr = strstr(lpNext, "\n-----BEGIN PGP ");
            if(lpStr==NULL) { //����ȏ�͂Ȃ�
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
/* Purpose        : ���������� (���j���[����̌Ăяo��)                 */
/* Inputs         : �������[�h                                          */
/*                  PGP����/�Í����u���b�N�i�[�t�@�C��                  */
/*                  ���؃e�L�X�g�i�[�t�@�C��                            */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
static bool ExecGPG(HWND hWnd, LPARAM lParam, CGnuPGModule::EMODE mode, const char* ascPath, const char* msgPath)
{
    g_Info.m_UserID = ""; //���[�UID�͋󔒂ɂ��Ă���
    if((mode==CGnuPGModule::Decript || mode==CGnuPGModule::Decript2) && (g_Info.m_PassPhrase.length())==0) { //���������s�� ���� �p�X���[�h���ݒ�
        CPassPhraseDialog dlg; //PassPhrase �_�C�A���O
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
/* Purpose        : �Í������b�Z�[�W(PGP/MIME)�̎��o��                */
/* Inputs         : ���[�����b�Z�[�W (Top)                              */
/*                  �������[�h                                          */
/*                  PGP����/�Í����u���b�N�i�[�t�@�C��                  */
/*                  ���؃e�L�X�g�i�[�t�@�C��                            */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
static bool GetMIMEMessage(CMIMEMessage* pTop, CGnuPGModule::EMODE mode, string& ascPath, string& msgPath)
{
    if(pTop==NULL) {
        return false;
    }

    if(mode==CGnuPGModule::Verify) { //����(PGP/MIME)���ꂽ���[���Ƃ��ď���
    //�����p�[�g��T��
        CMIMEMessage* pSign = pTop->FindCMIMEMessage("application", "pgp-signature");
        if(pSign==NULL) { //�����p�[�g���Ȃ�
            g_LogFile.AppendMessage("CMIMEMessage::FindCMIMEMessage(): �����p�[�g������܂���ł���");
            return false;
        }
        //�����p�[�g���i�[����
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("�t�@�C���I�[�v���G���[", ascPath);
            return false;
        }
        char* lpSign = pSign->ToString();
        char* lpPGP = strstr(lpSign, "-----BEGIN PGP "); //PGP����(?)�u���b�N���i�[
        if (lpPGP==NULL) {
            g_LogFile.AppendMessage("PGP�����u���b�N������܂���ł���");
            g_Info.m_BkApi.Free(lpSign);
            return false;
        }
        ascFile << lpPGP;
        ascFile.close();
        g_Info.m_BkApi.Free(lpSign);
    //�폐���p�[�g��T��
        CMIMEMessage* pContent = pTop->GetChild();
        while (stricmp(pContent->GetSubType().c_str(), "pgp-signature") == 0 && pContent) {
            pContent = pContent->GetNext();
        }
        if(pContent==NULL) { //�폐���p�[�g���Ȃ�����
            g_LogFile.AppendMessage("�폐���p�[�g������܂���ł���");
            return false;
        }
        //�폐���p�[�g���i�[����
        ofstream msgFile(msgPath.c_str(), ios_base::binary);
        if((msgFile.is_open())==false) {
            g_LogFile.AppendValue("�t�@�C���I�[�v���G���[", msgPath);
            return false;
        }
        char* lpBody = pContent->ToString();
        msgFile << lpBody;
        msgFile.close();
        g_Info.m_BkApi.Free(lpBody);
    }
    else if(mode==CGnuPGModule::Decript) { //(����)�Í���(PGP/MIME)���ꂽ���[��
    //�Í������ꂽ�p�[�g��T��
        CMIMEMessage* pContent = pTop->FindCMIMEMessage("application", "octet-stream");
        if(pContent==NULL) { //�Í������ꂽ�p�[�g���Ȃ�
            g_LogFile.AppendMessage("�Í������ꂽ�p�[�g������܂���ł���");
            return false;
        }
        //�Í������ꂽ�u���b�N���i�[
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("�t�@�C���I�[�v���G���[", ascPath);
            return false;
        }
        char* lpContent = pContent->ToString();
        if(lpContent==NULL) {
            g_LogFile.AppendMessage("�Í������ꂽ�p�[�g������܂���ł���");
            return false;
        }
        char* lpPGP = strstr(lpContent, "-----BEGIN PGP ");
        if(lpPGP==NULL) {
            g_LogFile.AppendMessage("�Í������ꂽ�u���b�N������܂���ł���");
            g_Info.m_BkApi.Free(lpContent);
            return false;
        }
        ascFile << lpPGP;
        ascFile.close();
        g_Info.m_BkApi.Free(lpContent);
    }
    else { //CGnuPGModule::Decript2 : �Í���(PGP/MIME)���ꂽ���[�� (Illegal type ?)
        CMIMEMessage* pContent = pTop;
        //�Í������ꂽ�u���b�N���i�[
        ofstream ascFile(ascPath.c_str(), ios_base::binary);
        if((ascFile.is_open())==false) {
            g_LogFile.AppendValue("�t�@�C���I�[�v���G���[", ascPath);
            return false;
        }
        char* lpContent = pContent->ToString();
        if(lpContent==NULL) {
            g_LogFile.AppendMessage("�Í������ꂽ�p�[�g������܂���ł���");
            return false;
        }
        char* lpPGP = strstr(lpContent, "-----BEGIN PGP ");
        if(lpPGP==NULL) {
            g_LogFile.AppendMessage("�Í������ꂽ�u���b�N������܂���ł���");
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
/* Purpose        : �Í������b�Z�[�W�̎��o��                          */
/* Inputs         : ���[�����b�Z�[�W (�{��)                             */
/*                  �������[�h(Out)                                     */
/*                  PGP����/�Í����u���b�N�i�[�t�@�C��                  */
/*                  �L�����N�^�Z�b�g                                    */
/* Ootput         : ���̃��b�Z�[�W                                      */
/*----------------------------------------------------------------------*/
static char* GetMessage(char* lpStr, CGnuPGModule::EMODE& mode, string& ascPath, char* szCharSet)
{
    //����/�Í��u���b�N�̎�ނ𒲂ׂ�
    if(strncmp(lpStr, "-----BEGIN PGP PUBLIC KEY BLOCK-----", 36) == 0) { //���J��
        mode = CGnuPGModule::ImpPubKey;
    }
    else if(strncmp(lpStr, "-----BEGIN PGP SIGNED MESSAGE-----",34) == 0) { //����
        mode = CGnuPGModule::Verify;
    }
    else {
        mode = CGnuPGModule::Decript;
    }

    //PGP�u���b�N�̏I����T��
    char* lpEnd = strstr(lpStr, "\n-----END PGP ");
    if(lpEnd==NULL) { //�I���̕������Ȃ�����
        g_LogFile.AppendMessage("�s���S�ȃu���b�N");
        return NULL;
    }
    //�s����T��
    lpEnd++;
    char* lpTmp = strchr(lpEnd,'\n');
    if(lpTmp!=NULL) {
        lpEnd = lpTmp;
    }
    else {
        lpEnd = strchr(lpEnd, '\0');
    }

    //PGP�u���b�N���i�[����
    ofstream ascFile(ascPath.c_str(), ios_base::binary);
    if((ascFile.is_open())==false) {
        //ErrorMessage(hWnd, IDS_GPG_FATALERROR);
        g_LogFile.AppendValue("�t�@�C���I�[�v���G���[", ascPath);
        return NULL;
    }
    int nLen = lpEnd - lpStr;
    if(stricmp(szCharSet, "ISO-2022-JP") == 0 || (szCharSet[0] == '\0' && GetACP() == 932)) { //���{��
        char* lpTemp = new char[nLen + 4]; //�����ɍ����͂Ȃ�?
        strncpy(lpTemp, lpStr, nLen);
        lpTemp[nLen] = '\0';
        char* lpJIS = g_Info.m_BkApi.ISO_2022_JP(lpTemp, TRUE); //JIS�ɕϊ�����
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
