/************************************************************************/
/*  File name : prototype.h                                             */
/************************************************************************/
/*  Contents  : ���I�R�[�f�B���O�K��Ɋ�Â��v���g�^�C�v                */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _PROTOTYPE_H_
#define _PROTOTYPE_H_

/**** Incude Files ****/


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CExsample                                               */
/* Purpose    : �N���X��`��                                            */
/*----------------------------------------------------------------------*/
class CExsample
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
    int m_val; //�����ϐ�
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CExsample(const CExsample& org); //�R�s�[�R���X�g���N�^
    CExsample& operator=(const CExsample& org); //������Z�q
    //���������֐�
    void PMetod(void); //����J���\�b�h
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CExsample(); //�f�t�H���g�R���X�g���N�^
    virtual ~CExsample(); //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    void GMetod(void); //���J���\�b�h
};


#endif // _PROTOTYPE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
