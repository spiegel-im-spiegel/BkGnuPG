/************************************************************************/
/*  File name : SetupDialog.h                                           */
/************************************************************************/
/*  Contents  : Setting �_�C�A���O                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _SETUPDIALOG_H_
#define _SETUPDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CSetupDialog                                            */
/* Purpose    : Setting �_�C�A���O�N���X                                */
/*----------------------------------------------------------------------*/
class CSetupDialog
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�(static)
    static CSetupDialog* m_Instance; //���̃N���X�̃C���X�^���X
    //�����ϐ�
    std::string m_CmdPath;
    std::string m_DefUserID; //�f�t�H���g���[�UID
    bool m_bPGPMIME; //PGP/MIME�Ή�
    bool m_bPGPCompatible; //PGP�݊��̏����E�Í������s�� (m_bOpenPGP, m_ThrowKey �͖����ɂȂ�)
    bool m_bOpenPGP; //OpenPGP�ɏ������������Í������s�� (m_bPGPCompatible �D��)
    bool m_bThrowKeyID; //��M�҂𓽖��ɂ��� (m_bPGPMIME==true �ł͖���)
    bool m_bRFC1991; //RFC1991�ɏ]�� (m_bPGPCompatible==true �ł̂ݗL��)
    bool m_bMD5; //���b�Z�[�W�_�C�W�F�X�g��MD5���g�p����
    bool m_bEnableCcUser; //�Í������̌��J��ID��Cc�A�h���X���܂߂�
    bool m_bEnableBccUser; //�Í������̌��J��ID��Bcc�A�h���X���܂߂�
    bool m_bEnableFromUser; //�Í������̌��J��ID��From�A�h���X���܂߂�
    bool m_bEnableDefUser; //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CSetupDialog(const CSetupDialog& org); //�R�s�[�R���X�g���N�^
    CSetupDialog& operator=(const CSetupDialog& org); //������Z�q
    //���������֐�
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CSetupDialog(); //�f�t�H���g�R���X�g���N�^
    virtual ~CSetupDialog() {} //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    static CSetupDialog* GetInstatnce(void) { return m_Instance; } //���݋N�����̃C���X�^���X��Ԃ�
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info); //�_�C�A���O�̋N��
    inline const char* GetCmdPath(void) { return m_CmdPath.c_str(); } //GnuPG ���W���[���ւ̃p�X
    inline void SetCmdPath(const char* cmdPath) { m_CmdPath = cmdPath; }
    inline const char* GetDefUserID(void) { return m_DefUserID.c_str(); } //�f�t�H���g���[�UID
    inline void SetDefUserID(const char* defUserID) { m_DefUserID = defUserID; }
    inline bool IsPGPMIME(void) { return m_bPGPMIME; } //PGP/MIME �Ή�
    inline void SetOptPGPMIME(bool bPGPMIME) { m_bPGPMIME = bPGPMIME; }
    inline bool IsPGPCompatible(void) { return m_bPGPCompatible; } //PGP�݊�
    inline void SetOptPGPCompatible(bool bPGPCompatible) { m_bPGPCompatible = bPGPCompatible; }
    inline bool IsOpenPGP(void) { return m_bOpenPGP; } //OpenPGP�݊�
    inline void SetOptOpenPGP(bool bOpenPGP) { m_bOpenPGP = bOpenPGP; }
    inline bool IsThrowKeyID(void) { return m_bThrowKeyID; } //��M�҂𓽖��ɂ���
    inline void SetOptThrowKeyID(bool bThrowKeyID) { m_bThrowKeyID = bThrowKeyID; }
    inline bool IsRFC1991(void) const { return m_bRFC1991; }  //RFC1991�ɏ]��?
    inline void SetOptRFC1991(const bool bRFC1991) { m_bRFC1991 = bRFC1991; return; } //RFC1991�����ݒ�
    inline bool IsMD5(void) const { return m_bMD5; }  //���b�Z�[�W�_�C�W�F�X�g��MD5���g�p����
    inline void SetOptMD5(const bool bMD5) { m_bMD5 = bMD5; return; } //MD5�����ݒ�
    inline bool IsEnableCcUser(void) { return m_bEnableCcUser; } //�Í������̌��J��ID��Cc�A�h���X���܂߂�
    inline void SetOptEnableCcUser(bool bEnableCcUser) { m_bEnableCcUser = bEnableCcUser; }
    inline bool IsEnableBccUser(void) { return m_bEnableBccUser; } //�Í������̌��J��ID��Bcc�A�h���X���܂߂�
    inline void SetOptEnableBccUser(bool bEnableBccUser) { m_bEnableBccUser = bEnableBccUser; }
    inline bool IsEnableFromUser(void) { return m_bEnableFromUser; } //�Í������̌��J��ID��From�A�h���X���܂߂�
    inline void SetOptEnableFromUser(bool bEnableFromUser) { m_bEnableFromUser = bEnableFromUser; }
    inline bool IsEnableDefUser(void) { return m_bEnableDefUser; } //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
    inline void SetOptEnableDefUser(bool bEnableDefUser) { m_bEnableDefUser = bEnableDefUser; }
};


#endif // _SETUPDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
