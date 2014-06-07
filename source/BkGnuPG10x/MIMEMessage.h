/************************************************************************/
/*  File name : MIMEMessage.h                                           */
/************************************************************************/
/*  Contents  : MIME ���[�����b�Z�[�W                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.20    2000.10.02          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 1.0.0   2000.11.08          */
/************************************************************************/
#ifndef _MIMEMESSAGE_H_
#define _MIMEMESSAGE_H_

/**** Incude Files ****/
#include <string>
#include <vector>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CBigBuffer                                              */
/* Purpose    : ���啶����o�b�t�@����N���X                            */
/*                                                                      */
/* �� �������̊m�ۂ� CBeckyAPI::Alloc ���g�p����̂Ŏ�舵������        */
/*----------------------------------------------------------------------*/
class CBigBuffer
{
// �����o�C���X�^���X
private:
    //�萔��`
    static const int m_Capacity; //�o�b�t�@�̃u���b�N�P��
    //�����ϐ�
    bool m_bBkAPI; //����������� Becky! API ���g�����ǂ���
    char* m_Buffer; //�o�b�t�@�ւ̃|�C���^
    int m_nBlock; //�q�[�v�������̃u���b�N��
    int m_nSize; //������T�C�Y
public:
    //�萔��`

// �����o���\�b�h
private:
    //�C���X�^���X�̃R�s�[����点�Ȃ����߂̑[�u
    CBigBuffer(const CBigBuffer& org); //�R�s�[�R���X�g���N�^
    CBigBuffer& operator=(const CBigBuffer& org); //������Z�q
    //���������֐�
    inline void Init(void) { m_Buffer=NULL; m_nBlock=0; m_nSize=0; } //������Ԃ̏�����
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CBigBuffer(bool bBkAPI=true) : m_Buffer(NULL), m_nBlock(0), m_nSize(0), m_bBkAPI(bBkAPI) {} //�f�t�H���g�R���X�g���N�^
    virtual ~CBigBuffer() { Reset(); } //�f�X�g���N�^
    //���Z�q
    //�C���^�t�F�[�X�֐�
    void Reset(bool bBkAPI=true); //�����o�b�t�@�����������������
    void AddStr(const char* str); //�o�b�t�@�ɕ������ǉ�����
    void RollBack(void); //�����̉��s�R�[�h���폜����
    const char* Reference(void) const { return m_Buffer; } //�o�b�t�@�̕\��(�������Ȃ�)
    char* Export(void); //�o�b�t�@�̃G�N�X�|�[�g
};

/*----------------------------------------------------------------------*/
/* Class Name : CMIMEMessage                                            */
/* Purpose    : MIME ���[�����b�Z�[�W�N���X                             */
/*----------------------------------------------------------------------*/
class CMIMEMessage
{
// �����o�C���X�^���X
private:
    //�萔��`
    //�����ϐ�
    //�֘A���郁�b�Z�[�W
    CMIMEMessage* m_pNext; //����MIME���b�Z�[�W
    CMIMEMessage* m_pChild; //�J�v�Z�������ꂽ�qMIME���b�Z�[�W
    //�w�b�_���
    std::string m_Type;
    std::string m_SubType;
    std::string m_Boundary;
    std::vector<std::string> m_lstHeaders;
    //std::vector<std::string> m_lstBody;
    //std::vector<std::string> m_lstTrail;
    CBigBuffer m_Body;
    CBigBuffer m_Trail;
public:
    //�萔��`

// �����o���\�b�h
private:
    //���������֐�
    void Init(void); //������
    void Copy(const CMIMEMessage& org); //�C���X�^���X�̃R�s�[
    const char* GetLine(const char* src, std::string& lineBuf); //��s���̕�������擾
    std::string& GetOptValue(std::string& option, std::string& opt, std::string& val); // �����񂩂� opt=val �̏��𔲂��o��
    void SetNext(const CMIMEMessage& next);
    void SetChild(const CMIMEMessage& child);
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CMIMEMessage() : m_pNext(NULL), m_pChild(NULL) { Init(); } //�f�t�H���g�R���X�g���N�^
    CMIMEMessage(const CMIMEMessage& org) : m_pNext(NULL), m_pChild(NULL) { Copy(org); } //�R�s�[�R���X�g���N�^
    virtual ~CMIMEMessage() { Init(); } //�f�X�g���N�^
    //���Z�q
    CMIMEMessage& operator=(const CMIMEMessage& org) { Copy(org); return *this; } //������Z�q
    //�C���^�t�F�[�X�֐�
    inline const std::string& GetType(void) const { return m_Type; }
    inline const std::string& SetType(const std::string& szType) { return m_Type = szType; }
    inline const std::string& GetSubType(void) const { return m_SubType; }
    inline const std::string& SetSubType(const std::string& szSubType) { return m_SubType = szSubType; }
    inline const std::string& GetBoundary(void) const { return m_Boundary; }
    inline const std::string& SetBoundary(const std::string& szBoundary) { return m_Boundary = szBoundary; }
    CMIMEMessage& GetBody(CMIMEMessage& item); //Body�֘A�̏��(Header�܂�)�𔲂��o��
    inline void ClearBody(void) { m_Body.Reset(); }
    inline void AddBody(const std::string& body) { m_Body.AddStr(body.c_str()); }
    inline CMIMEMessage* GetNext(void) const { return m_pNext; }
    inline void ClearNext(void) { delete m_pNext; m_pNext=NULL; }
    inline CMIMEMessage* GetChild(void) const { return m_pChild; }
    inline void ClearChild(void) { delete m_pChild; m_pChild=NULL; }
    void AddChild(const CMIMEMessage& child);
    void OverWrite(CMIMEMessage& item); //MIME���b�Z�[�W���㏑���R�s�[����
    const char* FromString(const char* src, const char* boundParent=NULL);
    char* ToString(void);
    std::string& GetHeader(const char* header, std::string& dataBuf);
    bool SetHeader(const char* header, const char* data);
    CMIMEMessage* FindCMIMEMessage(const char* type, const char* subType);
#if 0
    std::string& SepareteHeaderValue(std::string& value);
#endif
};


#endif // _MIMEMESSAGE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
