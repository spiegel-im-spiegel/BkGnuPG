/************************************************************************/
/*  File name : BkGnuPG.h                                               */
/************************************************************************/
/*  Contents  : Becky! GNU Privacy Guard �v���O�C�� �C���^�t�F�[�X��`  */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _BKGNUPG_H_
#define _BKGNUPG_H_

// �ȉ��� ifdef �u���b�N�� DLL ����ȒP�ɃG�N�X�|�[�g������}�N�����쐬����W���I�ȕ��@�ł��B
// ���� DLL ���̂��ׂẴt�@�C���̓R�}���h���C���Œ�`���ꂽ BKGNUPG_EXPORTS �V���{��
// �ŃR���p�C������܂��B���̃V���{���͂��� DLL ���g�p����ǂ̃v���W�F�N�g��ł�����`�łȂ�
// ��΂Ȃ�܂���B���̕��@�ł̓\�[�X�t�@�C���ɂ��̃t�@�C�����܂ނ��ׂẴv���W�F�N�g�� DLL
// ����C���|�[�g���ꂽ���̂Ƃ��� BKGNUPG_API �֐����Q�Ƃ��A���̂��߂��� DLL �͂��̃}�N
// ���Œ�`���ꂽ�V���{�����G�N�X�|�[�g���ꂽ���̂Ƃ��ĎQ�Ƃ��܂��B
#ifdef BKGNUPG_EXPORTS
#define BKGNUPG_API __declspec(dllexport)
#else
#define BKGNUPG_API __declspec(dllimport)
#endif


//�G�N�X�|�[�g�֐��ɂĂ� BkGnuPG.def �t�@�C�����Q�Ƃ̂���

#endif // _BKGNUPG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
