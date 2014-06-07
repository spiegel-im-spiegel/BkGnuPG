/************************************************************************/
/*  File name : debug.h                                                 */
/************************************************************************/
/*  Contents  : �f�o�b�O���[�`����`                                    */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.20    2000.10.02          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _DEBUG_H_
#define _DEBUG_H_
#ifdef _DEBUG //�f�o�b�O�����[�X���ɂ̂ݒ�`

/**** Incude Files ****/
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>


/**** Global Macro ****/
//�Ԕ����Ȃ��ƂɃf�o�b�O���[�h����inline�֐��͓W�J����Ȃ�(^^;) �̂ŁC#define�őΏ�
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__ )


#endif // _DEBUG
#endif // _DEBUG_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
