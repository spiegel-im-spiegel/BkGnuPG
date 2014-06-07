/************************************************************************/
/*  File name : ViewDialog.h                                            */
/************************************************************************/
/*  Contents  : ���ʏo�̓_�C�A���O                                      */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _VIEWDIALOG_H_
#define _VIEWDIALOG_H_

/**** Incude Files ****/
#include <windows.h>
#include "BkGnuPGInfo.h"

#include <string>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CViewDialog                                             */
/* Purpose    : ���ʏo�̓_�C�A���O�N���X                                */
/*----------------------------------------------------------------------*/
class CViewDialog
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
    std::string m_Message;
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CViewDialog(const CViewDialog& org); //�R�s�[�R���X�g���N�^
    CViewDialog& operator=(const CViewDialog& org); //������Z�q
    //���������֐�
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CViewDialog(); //�f�t�H���g�R���X�g���N�^
    virtual ~CViewDialog() {} //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    bool ExecDialog(HWND hWnd, LPARAM lParam, CBkGnuPGInfo& info, const char* outputMsg); //�_�C�A���O�̋N��
    inline const char* GetOutputMsg(void) { return m_Message.c_str(); }
};


#endif // _VIEWDIALOG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
