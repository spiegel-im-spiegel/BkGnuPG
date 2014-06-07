/************************************************************************/
/*  File name : AddressList.h                                           */
/************************************************************************/
/*  Contents  : ���[���A�h���X���X�g                                    */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _ADDRESSLIST_H_
#define _ADDRESSLIST_H_

/**** Incude Files ****/
#include <windows.h>

#include <string>
#include <vector>

/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CAddressList                                            */
/* Purpose    : �A�h���X���X�g�N���X                                    */
/*----------------------------------------------------------------------*/
class CAddressList
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
    std::vector<std::string> m_List; //���[���A�h���X(=UserID)���X�g
    std::string m_SirialList; //���[���A�h���X(=UserID)���X�g(�V���A���C�Y��������)
public:
    //�萔��`
    enum FID { //�A�h���X�t�B�[���h���
        NULL_FIELD = 0,
        FROM_FIELD,
        REPLY_TO_FILED,
        TO_FIELD,
        CC_FIELD,
        BCC_FIELD
    };

// �����o���\�b�h
private:
    //���������֐�
    void Copy(const CAddressList& org) { m_List = org.m_List; } //�C���X�^���X�̃R�s�[
    const char* Serialize(void); //���[���A�h���X(=UserID)���X�g���V���A���C�Y����
    int ImportList(const char* src, std::vector<std::string>& list); //�V���A���C�Y���ꂽ�ꗗ�����X�g�Ɋi�[����
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CAddressList() { Clear(); } //�f�t�H���g�R���X�g���N�^
    CAddressList(const CAddressList& org) { Copy(org); } //�R�s�[�R���X�g���N�^
    virtual ~CAddressList() {} //�f�X�g���N�^
    //���Z�q
    CAddressList& operator=(const CAddressList& org) { Copy(org); return *this; } //������Z�q
    //�C���^�t�F�[�X�֐�
    inline void Clear(void) { m_List.clear(); } //�ꗗ���N���A����
    int AddAddressFrom(HWND hWnd, FID field); //���[������A�h���X�ꗗ��ǉ�����
    int AddAddressFrom(const char* list); //�V���A���C�Y���ꂽ�A�h���X�ꗗ����ǉ�����
    inline std::vector<std::string>& GetList(void) { return m_List; } //�A�h���X�ꗗ�擾
    inline const char* GetSerialList(void) { return Serialize(); } //�A�h���X�ꗗ�擾
};


#endif // _ADDRESSLIST_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
