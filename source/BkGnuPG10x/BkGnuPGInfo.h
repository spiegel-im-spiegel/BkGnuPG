/************************************************************************/
/*  File name : BkGnuPGInfo.h                                           */
/************************************************************************/
/*  Contents  : BkGnuPG ������A�N�Z�X                                */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _BKGNUPGINFO_H_
#define _BKGNUPGINFO_H_

/**** Incude Files ****/
#include <windows.h>
#include "../BeckyAPI.h"
#include "GnuPGModule.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CBkGnuPGInfo                                            */
/* Purpose    : BkGnuPG ������A�N�Z�X�N���X                          */
/*               �� ���̃N���X�ł͓����ϐ���S�Č��J���o�����A�N�Z�X�\*/
/*                  �ɂ���                                              */
/*----------------------------------------------------------------------*/
class CBkGnuPGInfo
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
public:
    //�萔��`
    enum MENUKIND {
        Setup = 0,      //�ݒ�
        Encrypt,        //�Í���
        Sign,           //����
        EncryptSign,    //�Í���������
        ExpPubKey,      //���J���̃G�N�X�|�[�g
        Decript,        //������(�܌���&�C���|�[�g)
        MenuMax
    };
    //�ϐ�
    //�V�X�e���ϐ�(�C���X�^���X)
    static CBeckyAPI m_BkApi; //Becky! API �C���^�t�F�[�X�N���X (You can have only one instance in a project.)
    static bool m_InitFlag; //�����������t���O
    CGnuPGModule m_GPG; //GnuPG�A�N�Z�X�N���X
    UINT m_MainMenuID[MenuMax];
    UINT m_LVMenuID[MenuMax];
    UINT m_CompMenuID[MenuMax];
    //���ϐ�
    HINSTANCE m_hInstance;
    std::string m_BaseDir;
    std::string m_DataDir; //�f�[�^�t�H���_
    std::string m_TempDir;
    std::string m_IniPath;
    bool m_bEnableCcUser; //�Í������̌��J��ID��Cc�A�h���X���܂߂�
    bool m_bEnableBccUser; //�Í������̌��J��ID��Bcc�A�h���X���܂߂�
    bool m_bEnableFromUser; //�Í������̌��J��ID��From�A�h���X���܂߂�
    bool m_bEnableDefUser; //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
    //PassPhrase �_�C�A���O�ݒ�p
    std::string m_UserID;
    std::string m_PassPhrase;
    bool m_bShowText; //�p�X�t���[�Y�\��
    bool m_bUseAgent; // Use GPG Agent
    //Viewer �_�C�A���O�ݒ�p
    CGnuPGModule::CHARSET m_CharSet;
    //�f�o�b�O�p
    bool m_bDebugLog; //�f�o�b�O�p�̃��O���o�͂���
    std::string m_LogPath;

// �����o���\�b�h
private:
    //���������֐�
    void Copy(const CBkGnuPGInfo& org); //�C���X�^���X�̃R�s�[
    void SetFilePath(const char* modulePath); //���W���[���ւ̃p�X��ݒ肷��
    CGnuPGModule::CHARSET GetCharSet(const char* charset); //�L�����N�^�Z�b�g�̎���
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CBkGnuPGInfo(); //�f�t�H���g�R���X�g���N�^
    CBkGnuPGInfo(const CBkGnuPGInfo& org) { Copy(org); } //�R�s�[�R���X�g���N�^
    virtual ~CBkGnuPGInfo(); //�f�X�g���N�^
    //���Z�q
    CBkGnuPGInfo& operator=(const CBkGnuPGInfo& org) { Copy(org); return *this; } //������Z�q
    //�C���^�t�F�[�X�֐�
    bool InitInstance(HINSTANCE hModule); //��񏉊���
    void SetTempPath(const char* tempPath); //�e���|�����f�B���N�g����ݒ肷��
    void GetEnv(void); //���ϐ��̎擾
    void SetEnv(void); //���ϐ��̐ݒ�
    void SetUseAgentFlag(void); //���ϐ� (UseAgent) �̐ݒ�
};

/**** External Declarelation ****/
extern CBkGnuPGInfo g_Info; //BkGnuPG ������N���X ����


#endif // _BKGNUPGINFO_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
