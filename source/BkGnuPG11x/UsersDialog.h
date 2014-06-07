/************************************************************************/
/*  File name : UsersDialog.h                                           */
/************************************************************************/
/*  Contents  : �����[�g���[�U�ݒ�_�C�A���O                            */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.12          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _USERSDIALOG_H_
#define _USERSDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CUsersDialog                                            */
/* Purpose    : �����[�g���[�U�ݒ�_�C�A���O�N���X                      */
/*----------------------------------------------------------------------*/
class CUsersDialog
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�(static)
    static CUsersDialog* m_Instance; //���̃N���X�̃C���X�^���X
    //�����ϐ�
    CBkGnuPGInfo* m_Info;
    std::string m_UserID;
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CUsersDialog(const CUsersDialog& org); //�R�s�[�R���X�g���N�^
    CUsersDialog& operator=(const CUsersDialog& org); //������Z�q
    //���������֐�
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CUsersDialog(); //�f�t�H���g�R���X�g���N�^
    virtual ~CUsersDialog() {} //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    static CUsersDialog* GetInstatnce(void) { return m_Instance; } //���݋N�����̃C���X�^���X��Ԃ�
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info); //�_�C�A���O�̋N��
    inline const char* GetUserID(void) { return m_UserID.c_str(); } // User ID
    inline void SetUserID(const char* userID) { m_UserID = userID; }
    inline CBkGnuPGInfo* GetBkGnuPGInfo(void) { return m_Info; }
};


#endif // _USERSDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
