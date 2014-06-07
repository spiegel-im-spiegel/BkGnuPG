/************************************************************************/
/*  File name : PassDialog.h                                            */
/************************************************************************/
/*  Contents  : PassPhrase �_�C�A���O                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, hex                                   */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _PASSDIALOG_H_
#define _PASSDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CPassPhraseDialog                                       */
/* Purpose    : PassPhrase �_�C�A���O�N���X                             */
/*----------------------------------------------------------------------*/
class CPassPhraseDialog
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�(static)
    static CPassPhraseDialog* m_Instance; //���̃N���X�̃C���X�^���X
    //�����ϐ�
    CBkGnuPGInfo* m_Info;
    std::string m_UserID;
    std::string m_PassPhrase;
    BOOL m_bFixed; //���[�UID���̓{�b�N�X�̏��
    bool m_bShowText; //�p�X�t���[�Y�\��
    bool m_bUseAgent; // Use GPG Agent
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CPassPhraseDialog(const CPassPhraseDialog& org); //�R�s�[�R���X�g���N�^
    CPassPhraseDialog& operator=(const CPassPhraseDialog& org); //������Z�q
    //���������֐�
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CPassPhraseDialog(); //�f�t�H���g�R���X�g���N�^
    virtual ~CPassPhraseDialog() {} //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    static CPassPhraseDialog* GetInstatnce(void) { return m_Instance; } //���݋N�����̃C���X�^���X��Ԃ�
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, bool bFixedID); //�_�C�A���O�̋N��
    inline const BOOL IsFixed(void) const { return m_bFixed; } //���[�UID���̓{�b�N�X�̏�Ԃ�Ԃ�
    inline const bool IsShowText(void) const { return m_bShowText; } //�p�X�t���[�Y�\���̏�Ԃ�Ԃ�
    inline const bool UseAgent(void) const { return m_bUseAgent; }
    inline void SetOptShowText(bool bShowText) { m_bShowText = bShowText; } //�p�X�t���[�Y�\��
    inline const char* GetUserID(void) const { return m_UserID.c_str(); } // User ID
    inline void SetUserID(const char* userID) { m_UserID = userID; }
    inline const char* GetPassPhrase(void) const { return m_PassPhrase.c_str(); } //PassPhrase
    inline void SetPassPhrase(const char* passPhrase) { m_PassPhrase = passPhrase; }
    inline void SetUseAgent(bool value) { m_bUseAgent = value; }
    inline CBkGnuPGInfo* GetBkGnuPGInfo(void) { return m_Info; }
};


#endif // _PASSDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
