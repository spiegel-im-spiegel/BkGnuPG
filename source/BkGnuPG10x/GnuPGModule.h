/************************************************************************/
/*  File name : GnuPGModule.h                                           */
/************************************************************************/
/*  Contents  : GnuPG ���W���[�������b�s���O����                        */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.21    2000.10.04          */
/*                                  Version 0.30    2000.10.06          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/
#ifndef _GNUPGMODULE_H_
#define _GNUPGMODULE_H_

/**** Incude Files ****/
#include <windows.h>

#include <string>
#include <vector>


/**** Global Macro ****/


/**** Typedef ****/


/**** External Declarelation ****/


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Class Name : CGnuPGModule                                            */
/* Purpose    : GnuPG ���W���[�������b�s���O����N���X                  */
/*----------------------------------------------------------------------*/
class CGnuPGModule
{
// �����o�C���X�^���X
private:
    //�萔��`
    static const DWORD m_PipeSize; //�p�C�v�̃o�b�t�@�T�C�Y
    static const char* const m_DefaultModuleName; //�f�t�H���g�̃��W���[����
    static const char* const m_DefaultOption; //�f�t�H���g�I�v�V����
    static const char* const m_SignPGPOption; //���������pPGP�Ή��I�v�V����
    static const char* const m_OpenPGPOption; //OpenPGP�Ή��I�v�V����
    static const char* const m_ThrowKeyIDOption; //������M�ґΉ��I�v�V����
    static const char* const m_RFC1991Option; //�N���A���������pPGP�Ή��I�v�V����
    static const char* const m_MdMD5Option; //���b�Z�[�W�_�C�W�F�X�g��MD5���g���I�v�V����
    static const char* const m_MdSHA1Option; //���b�Z�[�W�_�C�W�F�X�g��SHA-1���g���I�v�V����
    //�����ϐ�
    //���o�̓��_�C���N�g
    HANDLE m_hStdoutRead;       //�W���o�͓ǂݍ��݃p�C�v
    HANDLE m_hStdoutWrite;      //�W���o�͏������݃p�C�v
    HANDLE m_hStderrRead;       //�W���G���[�o�͓ǂݍ��݃p�C�v
    HANDLE m_hStderrWrite;      //�W���G���[�o�͏������݃p�C�v
    HANDLE m_hStdinRead;        //�W�����͓ǂݍ��݃p�C�v
    HANDLE m_hStdinWrite;       //�W�����͏������݃p�C�v
    //����p�e��ϐ�
    HANDLE m_hProcess;          //�v���Z�X�ւ̃n���h��
    std::string m_ModuleName;   //���W���[����
    std::string m_OptionStr;    //�N���I�v�V����������
    std::string m_CommandStr;   //�N���R�}���h������
    std::string m_StdoutBuffer; //�W���o�̓o�b�t�@ (�\���p)
    std::string m_StderrBuffer; //�W���G���[�o�̓o�b�t�@ (�\���p)
    //�I�v�V����
    std::string m_DefUserID; //�f�t�H���g���[�UID
    std::string m_MDalgo; //���b�Z�[�W�_�C�W�F�X�g�A���S���Y����
    bool m_bPGPMIME; //PGP/MIME�Ή�
    bool m_bPGPCompatible; //PGP�݊��̏����E�Í������s�� (m_bOpenPGP, m_ThrowKey �͖����ɂȂ�)
    bool m_bOpenPGP; //OpenPGP�ɏ������������Í������s�� (m_bPGPCompatible �D��)
    bool m_bThrowKeyID; //��M�҂𓽖��ɂ��� (m_bPGPMIME==true �ł͖���)
    bool m_bRFC1991; //RFC1991�ɏ]�� (m_bPGPCompatible==true �ł̂ݗL��)
    bool m_bMD5; //���b�Z�[�W�_�C�W�F�X�g��MD5���g�p����
    bool m_bTextModeSig; //���������̍ۂɃe�L�X�g���[�h�ŏ�������
public:
    //�萔��`
    enum EMODE {    // ���샂�[�h
        Null = 0,       //����`
        Encrypt,        //�Í���
        Sign,           //����
        EncryptSign,    //�Í���������
        ExpPubKey,      //���J���̃G�N�X�|�[�g
        Decript,        //������
        Verify,         //�����̌���
        ImpPubKey       //���J���̃C���|�[�g
    };
    enum CHARSET {  //GnuPG�o�̓L�����N�^�Z�b�g
        DEFAULT = 0,    //�f�t�H���g(Shift-JIS)
        ISO_2022_IP,    //iso-2022-jp (JIS)
        EUC_JP,         // EUC-JP
        UTF_8           // UTF-8
    };

// �����o���\�b�h
private:
    //���������֐�
    void Copy(const CGnuPGModule& org); //�C���X�^���X�̃R�s�[
    bool Execute(void); //�v���Z�X���N������
    std::string& Quotation(std::string& str); //�����񂪋󔒕������܂ޏꍇ�̓_�u���N�H�[�e�V�����ň͂�
    std::string& Dequotation(std::string& str); //�_�u���N�H�[�e�V�����݂̈͂��O��
    void Terminate(void); //�v���Z�X���I������
    bool IsActive(DWORD tout=0); //�v���Z�X�͉ғ�����?
    void WaitforTerminate(void); //�v���Z�X���I������܂ő҂�
    bool Write(std::string& inpMsg); //�W�����͂ւ̓���
    bool Read(std::string& outpMsg, bool bBlock); //�W���o�͂���̃��b�Z�[�W
public:
    //�R���X�g���N�^�E�f�X�g���N�^
    CGnuPGModule(const char* moduleName=NULL); //�f�t�H���g�R���X�g���N�^
    CGnuPGModule(const CGnuPGModule& org) { Copy(org); } //�R�s�[�R���X�g���N�^
    virtual ~CGnuPGModule() { Terminate(); } //�f�X�g���N�^
    //���Z�q
    CGnuPGModule& operator=(const CGnuPGModule& org) { Copy(org); return *this; } //������Z�q
    //�C���^�t�F�[�X�֐�
    bool ExecEncrypt(const char* ascPath, const char* txtPath, std::vector<std::string>& addrList); //�Í�������
    bool ExecSign(const char* ascPath, const char* txtPath, const char* userID, const char* passPhrase, bool UseAgent); //��������
    bool ExecEncryptSign(const char* ascPath, const char* txtPath, const char* userID, std::vector<std::string>& addrList, const char* passPhrase, bool UseAgent); //�Í����Ə�������
    bool ExecDecrypt(const char* ascPath, const char* txtPath, const char* passPhrase, bool UseAgent); //����������
    bool ExecVerify(const char* ascPath, const char* txtPath); //�����̌���
    bool ExecImport(const char* ascPath); //���J���̃C���|�[�g
    bool ExecExport(const char* ascPath, const char* userID); //���J���̃G�N�X�|�[�g
    inline const char* GetOutputMsg(void) { return m_StderrBuffer.c_str(); }
    inline const char* GetStatusMsg(void) { return m_StdoutBuffer.c_str(); }
    inline static const char* GetDefModuleName(void) { return m_DefaultModuleName; } //���W���[�������擾����
    inline const char* GetModuleName(void) const { return m_ModuleName.c_str(); } //���W���[�������擾����
    inline void SetModuleName(const char* moduleName) { m_ModuleName = moduleName; Quotation(m_ModuleName); return; } //���W���[������ݒ肷��
    inline const char* GetDefUserID(void) const { return m_DefUserID.c_str(); } //�f�t�H���g���[�UID���擾����
    inline void SetDefUserID(const char* userID) { m_DefUserID = userID; return; } //�f�t�H���g���[�UID��ݒ肷��
    inline bool IsPGPMIME(void) const { return m_bPGPMIME; } //PGP/MIME �Ή���?
    inline void SetOptPGPMIME(const bool bPGPMIME) { m_bPGPMIME = bPGPMIME; return; } //PGP/MIME �Ή������ݒ�
    inline bool IsPGPCompatible(void) const { return m_bPGPCompatible; }  //PGP�݊�?
    inline void SetOptPGPCompatible(const bool bPGPCompatible) { m_bPGPCompatible = bPGPCompatible; return; } //PGP�݊������ݒ�
    inline bool IsOpenPGP(void) const { return m_bOpenPGP; }  //OpenPGP?
    inline void SetOptOpenPGP(const bool bOpenPGP) { m_bOpenPGP = bOpenPGP; return; } //OpenPGP�����ݒ�
    inline bool IsThrowKeyID(void) const { return m_bThrowKeyID; }  //��M�҂Ɠ����ɂ���?
    inline void SetOptThrowKeyID(const bool bThrowKeyID) { m_bThrowKeyID = bThrowKeyID; return; } //������M�ҏ����ݒ�
    inline bool IsRFC1991(void) const { return m_bRFC1991; }  //RFC1991�ɏ]��?
    inline void SetOptRFC1991(const bool bRFC1991) { m_bRFC1991 = bRFC1991; return; } //RFC1991�����ݒ�
    inline bool IsMD5(void) const { return m_bMD5; }  //���b�Z�[�W�_�C�W�F�X�g��MD5���g�p����
    inline void SetOptMD5(const bool bMD5) { m_bMD5 = bMD5; return; } //MD5�����ݒ�

    inline const char* IsMDalgo(void) const { return m_MDalgo.c_str(); }  //���b�Z�[�W�_�C�W�F�X�g�A���S���Y����
    inline void SetOptMDalgo(const char* strMDalgo) { m_MDalgo = strMDalgo; return; } //���b�Z�[�W�_�C�W�F�X�g�A���S���Y�����ݒ�
    const char* GetOptMDalgo(void); //���b�Z�[�W�_�C�W�F�X�g�A���S���Y�������擾����

    inline bool IsTextModeSig(void) const { return m_bTextModeSig; }  //���������̍ۂɃe�L�X�g���[�h�ŏ�������
    inline void SetOptTextModeSig(const bool bTextModeSig) { m_bTextModeSig = bTextModeSig; return; } //�������������ݒ�
    //�C���^�t�F�[�X�֐� (static)
    static std::string GetGPGHomeDir(void); //GnuPG�̃z�[���f�B���N�g�����擾����
    static std::string GetGPGModuleName(void); //GnuPG�̃v���O���������擾���� (by Canalian)

};


#endif // _GNUPGMODULE_H_
/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
