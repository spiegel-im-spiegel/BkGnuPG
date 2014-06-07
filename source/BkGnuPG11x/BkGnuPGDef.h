/************************************************************************/
/*  File name : BkGnuPGDef.h                                            */
/************************************************************************/
/*  Contents  : Becky! GNU Privacy Guard �v���O�C�� ���ʒ�`            */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _BKGNUPGDEF_H_
#define _BKGNUPGDEF_H_

/**** Incude Files ****/
#define WIN32_LEAN_AND_MEAN // Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂�
#include <windows.h>
#include "resource.h"

#include <string>
#include <vector>


/**** Global Macro ****/
#define WM_SET_TRANSFER_SAFE    (WM_USER+300) // Makes compose window to encode text in transfer safe form.


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/
    //Common.cpp
extern void ErrorMessage(HWND hWnd, UINT uID); //�G���[���b�Z�[�W��\������
extern void FatalErrorMessage(HWND hWnd, const char* fileName, int lineno, const char* pMsg=NULL); //�v���I�G���[���b�Z�[�W��\������
extern std::string& GetLegalFileName(const char* name, std::string& fileName); //�t�@�C�����Ƃ��ė��p�\�Ȗ��O�ɕϊ�����
extern char* FileToString(const std::string& fileName); //�e�L�X�g�t�@�C�����e�𕶎�����Ƃ��ă������Ƀ��[�h����
extern void GetNameAndAddr(const char* src, std::string& name, std::string& email); //�uname <email>�v�`���̕������v�f�ɕ�������
extern bool IsFileExist(const std::string& path); //�t�@�C���̗L����₢���킹��
extern void RemoveFiles(const std::string& path); //�t�@�C���폜 (���C���h�J�[�h�L��)
extern std::string& TrimSpace(std::string& element); //�O��̋󔒕�������菜��


#endif // _BKGNUPGDEF_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
