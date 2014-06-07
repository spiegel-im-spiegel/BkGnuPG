/************************************************************************/
/*  File name : GnuPGModule.cpp                                         */
/************************************************************************/
/*  Contents  : gpg.exe Control Class                                   */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.01    2000.09.14          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.31    2000.10.19          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "GnuPGModule.h"
#include "LogFile.h"

#include <string>
#include <fstream>
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Constants ****/
const DWORD CGnuPGModule::m_PipeSize = 32767; //�p�C�v�̃o�b�t�@�T�C�Y
const char* const CGnuPGModule::m_DefaultModuleName = "gpg.exe";  //�f�t�H���g�̃��W���[����
const char* const CGnuPGModule::m_DefaultOption = "--logger-fd 2 --batch --yes --verbose";  //�f�t�H���g�I�v�V����
    //   --logger-fd 2: ���O�̏o�͂�W���G���[�o�͂ɂ���
    //         --batch: �o�b�`���[�h�ŏ�������
    //           --yes: �₢���킹�ɑ΂��đS�āuYes�v�Ŏ�����������
    //       --verbose: �ڍו\��
const char* const CGnuPGModule::m_SignPGPOption = "--force-v3-sigs"; //���������pPGP�Ή��I�v�V����
    // --force-v3-sigs: Version 3 �ŏ������� (gpg �̃f�t�H���g�� Version 4)
const char* const CGnuPGModule::m_OpenPGPOption = "--openpgp"; //OpenPGP�Ή��I�v�V����
const char* const CGnuPGModule::m_ThrowKeyIDOption = "--throw-keyid"; //������M�ґΉ��I�v�V����
const char* const CGnuPGModule::m_RFC1991Option = "--rfc1991"; //RFC1991�����I�v�V����
const char* const CGnuPGModule::m_MdMD5Option = "--digest-algo MD5"; //���b�Z�[�W�_�C�W�F�X�g��MD5���g���I�v�V����
const char* const CGnuPGModule::m_MdSHA1Option = "--digest-algo SHA1"; //���b�Z�[�W�_�C�W�F�X�g��SHA-1���g���I�v�V����


/**** Prototyping ****/


/*----------------------------------------------------------------------*/
/* Purpose        : �f�t�H���g�R���X�g���N�^                            */
/* Inputs         : ���W���[���ւ̃p�X                                  */
/*----------------------------------------------------------------------*/
CGnuPGModule::CGnuPGModule(const char* moduleName)
    : m_hStdoutRead(NULL), m_hStdoutWrite(NULL),
        m_hStderrRead(NULL), m_hStderrWrite(NULL),
        m_hStdinRead(NULL), m_hStdinWrite(NULL),
        m_hProcess(NULL),
        m_bPGPMIME(true), m_bPGPCompatible(true),
        m_bOpenPGP(false), m_bThrowKeyID(false),
        m_bRFC1991(false), m_bMD5(false),
        m_bTextModeSig(false)
{
    if(moduleName==NULL) {
        m_ModuleName = m_DefaultModuleName;
    }
    else {
        m_ModuleName = moduleName;
    }
    m_DefUserID = ""; //�f�t�H���g���[�UID
    m_MDalgo = "SHA1"; //�f�t�H���g�̃��b�Z�[�W�_�C�W�F�X�g�A���S���Y����
}

/*----------------------------------------------------------------------*/
/* Purpose        : �Í�������                                          */
/* Inputs         : �Í����������b�Z�[�W���i�[����t�@�C���ւ̃p�X      */
/*                  �Í��Ώۃ��b�Z�[�W���i�[�����t�@�C���ւ̃p�X        */
/*                  ���[���A�h���X(=UserID)���X�g                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecEncrypt(const char* ascPath, const char* txtPath, vector<string>& addrList)
{
    //��������
    if(ascPath==NULL || txtPath==NULL || (addrList.size())==0) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "-ea ";
    m_CommandStr += Quotation(string(txtPath));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g
    if(m_bPGPCompatible==true) {
        if(m_bRFC1991==true) {
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath));
    for(vector<string>::iterator p=addrList.begin(); p!=addrList.end(); p++) {
        m_OptionStr += " -r ";
        m_OptionStr += Quotation(*p);
    }

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ��������                                            */
/* Inputs         : �����u���b�N���i�[����t�@�C���ւ̃p�X              */
/*                  �����Ώۃ��b�Z�[�W���i�[�����t�@�C���ւ̃p�X        */
/*                  �����Ɏg�� User ID                                  */
/*                  �p�X�t���[�Y                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecSign(const char* ascPath, const char* txtPath, const char* userID, const char* passPhrase, bool UseAgent)
{
    //��������
    if(ascPath==NULL || txtPath==NULL || passPhrase==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    if(m_bPGPMIME==true) {
        m_CommandStr = "-a "; //ASCII �o��
        if(m_bTextModeSig==true) { //���������̍ۂɃe�L�X�g���[�h�ŏ�������
            m_CommandStr += "--textmode ";
        }
        m_CommandStr += "--detach-sign "; //�����p�[�g�𕪗�����
    }
    else {
        m_CommandStr = "--clearsign "; //�N���A����
    }
    m_CommandStr += Quotation(string(txtPath));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP�݊��̂��߂̃I�v�V����
        if(m_bRFC1991==true) { //RFC1991�ɏ]��
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //�����A���S���Y��(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //�����A���S���Y��(SHA-1)
    //}
    m_OptionStr += " --digest-algo ";
    m_OptionStr += GetOptMDalgo();
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0"; //�p�X�t���[�Y�̓��͂�W�����͂ōs��
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent";
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)) + " "; //�����u���b�N�̏o�͐�
    if(userID!=NULL) {
        m_OptionStr += "-u "; //���[�UID�̎w��
        m_OptionStr += Quotation(string(userID)) + " ";
    }
    else if((m_DefUserID.length())>0) {
        m_OptionStr += "--default-key "; //�f�t�H���g���[�UID�̎w��
        m_OptionStr += Quotation(m_DefUserID) + " ";
    }

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    if (UseAgent==false) { // not Use GPG Agent
        Sleep(10); //�N����10ms�قǑ҂��Ă݂�
        //�p�X�t���[�Y����
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �Í����Ə�������                                    */
/* Inputs         : �����E�Í����u���b�N���i�[����t�@�C���ւ̃p�X      */
/*                  �����E�Í����Ώۃ��b�Z�[�W���i�[�����t�@�C���ւ̃p�X*/
/*                  �����Ɏg�� User ID                                  */
/*                  �Í��Ɏg�����[���A�h���X(=UserID)���X�g             */
/*                  �p�X�t���[�Y                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecEncryptSign(const char* ascPath, const char* txtPath, const char* userID, vector<string>& addrList, const char* passPhrase, bool UseAgent)
{
    //��������
    if(ascPath==NULL || txtPath==NULL || (addrList.size())==0 || passPhrase==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "-esa "; //�Í��� & ����
    m_CommandStr += Quotation(string(txtPath));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP�݊��̂��߂̃I�v�V����
        if(m_bRFC1991==true) { //RFC1991�ɏ]��
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
        if(m_bThrowKeyID==true) {
            m_OptionStr += " ";
            m_OptionStr += m_ThrowKeyIDOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //�����A���S���Y��(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //�����A���S���Y��(SHA-1)
    //}
    m_OptionStr += " --digest-algo ";
    m_OptionStr += GetOptMDalgo();
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0"; //�p�X�t���[�Y�̓��͂�W�����͂ōs��
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent";
    }
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)) + " "; //�����E�Í����p�P�b�g�̏o�͐�
    if(userID!=NULL) {
        m_OptionStr += "-u "; //���[�UID�̎w��
        m_OptionStr += Quotation(string(userID));
    }
    else if((m_DefUserID.length())>0) {
        m_OptionStr += "--default-key "; //�f�t�H���g���[�UID�̎w��
        m_OptionStr += Quotation(m_DefUserID);
    }
    for(vector<string>::iterator p=addrList.begin(); p!=addrList.end(); p++) {
        m_OptionStr += " -r ";
        m_OptionStr += Quotation(*p);
    }

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    if(UseAgent==false) { // not Use GPG Agent
        Sleep(10); //�N����10ms�قǑ҂��Ă݂�
        //�p�X�t���[�Y����
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���J���̃G�N�X�|�[�g                                */
/* Inputs         : ���J�����i�[����t�@�C���ւ̃p�X                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecExport(const char* ascPath, const char* userID)
{
    //��������
    if(ascPath==NULL || userID==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "-a --export ";
    m_CommandStr += Quotation(string(userID));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g
    if(m_bPGPCompatible==true) {
        m_OptionStr += " ";
        m_OptionStr += m_SignPGPOption; //PGP�݊��̂��߂̃I�v�V����
        if(m_bRFC1991==true) { //RFC1991�ɏ]��
            m_OptionStr += " ";
            m_OptionStr += m_RFC1991Option;
        }
    }
    else {
        if(m_bOpenPGP==true) {
            m_OptionStr += " ";
            m_OptionStr += m_OpenPGPOption;
        }
    }
    //m_OptionStr += " ";
    //if((m_bPGPCompatible==true || m_bOpenPGP==false) && m_bMD5==true) {
    //    m_OptionStr += m_MdMD5Option; //�����A���S���Y��(MD5)
    //}
    //else {
    //    m_OptionStr += m_MdSHA1Option; //�����A���S���Y��(SHA-1)
    //}
    m_OptionStr += " -o ";
    m_OptionStr += Quotation(string(ascPath)); //�������������b�Z�[�W�̏o�͐�

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ����������                                          */
/* Inputs         : �Í������b�Z�[�W���i�[�����t�@�C���ւ̃p�X          */
/*                  ���������b�Z�[�W���i�[����t�@�C���ւ̃p�X          */
/*                  �p�X�t���[�Y                                        */
/*                  Use GPG Agent                                       */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecDecrypt(const char* ascPath, const char* txtPath, const char* passPhrase, bool UseAgent)
{
    //��������
    if(ascPath==NULL || txtPath==NULL || passPhrase==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "-d ";
    m_CommandStr += Quotation(string(ascPath));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g
    if(UseAgent==false) { // not Use GPG Agent
        m_OptionStr += " --passphrase-fd 0 "; //�p�X�t���[�Y�̓��͂�W�����͂ōs��
        if((m_DefUserID.length())>0) {
            m_OptionStr += "--default-key "; //�f�t�H���g���[�UID�̎w��
            m_OptionStr += Quotation(m_DefUserID) + " ";
        }
    }
    else { // Use GPG Agent
        m_OptionStr += " --use-agent ";
    }
    m_OptionStr += "-o ";
    m_OptionStr += Quotation(string(txtPath)) + " "; //�������������b�Z�[�W�̏o�͐�

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    if(UseAgent==false) { // not Use GPG Agent
        Sleep(10); //�N����10ms�قǑ҂��Ă݂�
        //�p�X�t���[�Y����
        string sbuf = passPhrase;
        sbuf += "\n";
        Write(sbuf);
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �����̌���                                          */
/* Inputs         : �������i�[�����t�@�C���ւ̃p�X                      */
/*                  �폐�����b�Z�[�W���i�[�����t�@�C���ւ̃p�X          */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecVerify(const char* ascPath, const char* txtPath)
{
    //��������
    if(ascPath==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "--verify ";
    m_CommandStr += Quotation(string(ascPath));
    if(txtPath!=NULL) {
        m_CommandStr += " ";
        m_CommandStr += Quotation(string(txtPath));
    }
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���J���̃C���|�[�g                                  */
/* Inputs         : ���J�����i�[�����t�@�C���ւ̃p�X                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::ExecImport(const char* ascPath)
{
    //��������
    if(ascPath==NULL) { //�K�{�p�����[�^���Ȃ�
        return false;
    }
    m_StdoutBuffer = m_StderrBuffer = ""; //�W���o�̓o�b�t�@���N���A

    //�R�}���h�ݒ�
    m_CommandStr = "--import ";
    m_CommandStr += Quotation(string(ascPath));
    //�I�v�V�����ݒ�
    m_OptionStr = m_DefaultOption; //�f�t�H���g�̃I�v�V�������Z�b�g

    //�R�}���h�N��
    if((Execute())==false) {
        return false; //�N�����s
    }

    //�v���Z�X�I���܂ő҂�
    WaitforTerminate();

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : GnuPG�̃z�[���f�B���N�g�����擾����                 */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : GnuPG�̃z�[���f�B���N�g��                           */
/*----------------------------------------------------------------------*/
string CGnuPGModule::GetGPGHomeDir(void)
{
    HKEY hKey;
    string dir = "";
    if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GNU\\GnuPG", 0, KEY_QUERY_VALUE, &hKey))!=ERROR_SUCCESS) { //�L�[���Ȃ�
        return dir;
    }

    DWORD len;
    if((RegQueryValueEx(hKey, "HomeDir", NULL, NULL, NULL, &len))==ERROR_SUCCESS) {
        if(len>0) {
            len++;
            char* path = new char[len];
            if((RegQueryValueEx(hKey, "HomeDir", NULL, NULL, (LPBYTE)path, &len))==ERROR_SUCCESS) {
                dir = path;
                if(path[strlen(path)-1]!='\\') {
                    dir += "\\";
                }
            }
            delete [] path;
        }
    }
    RegCloseKey(hKey);

    return dir;
}

/*----------------------------------------------------------------------*/
/* Purpose        : GnuPG�̃v���O���������擾����                       */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : GnuPG�̃v���O������                                 */
/*                                                                      */
/* Auther         : Canalian    Version 1.0.4    2003.03.23             */
/*----------------------------------------------------------------------*/
string CGnuPGModule::GetGPGModuleName(void)
{
    HKEY hKey;
    string dir = m_DefaultModuleName;
    if((RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\GNU\\GnuPG", 0, KEY_QUERY_VALUE, &hKey))!=ERROR_SUCCESS) { //�L�[���Ȃ�
        return dir;
    }

    DWORD len;
    if((RegQueryValueEx(hKey, "gpgProgram", NULL, NULL, NULL, &len))==ERROR_SUCCESS) {
        if(len>0) {
            len++;
            char* path = new char[len];
            if((RegQueryValueEx(hKey, "gpgProgram", NULL, NULL, (LPBYTE)path, &len))==ERROR_SUCCESS) {
                dir = path;
            }
            delete [] path;
        }
    }
    RegCloseKey(hKey);

    return dir;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���b�Z�[�W�_�C�W�F�X�g�A���S���Y�������擾����      */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : ���b�Z�[�W�_�C�W�F�X�g�A���S���Y����                */
/*----------------------------------------------------------------------*/
const char* CGnuPGModule::GetOptMDalgo(void)
{
    if(m_bMD5==false) {
        return m_MDalgo.c_str();
    }
    else {
        return "MD5";
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �C���X�^���X�̃R�s�[                                */
/* Inputs         : �R�s�[���C���X�^���X                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CGnuPGModule::Copy(const CGnuPGModule& org)
{
    //����p�e��ϐ�
    m_ModuleName = org.m_ModuleName;   //���W���[����
    m_OptionStr = org.m_OptionStr;    //�N���I�v�V����������
    m_CommandStr = org.m_CommandStr;   //�N���R�}���h������
    m_StdoutBuffer = org.m_StdoutBuffer; //�W���o�̓o�b�t�@ (�\���p)
    m_StderrBuffer = org.m_StderrBuffer; //�W���G���[�o�̓o�b�t�@ (�\���p)
    //�I�v�V����
    m_DefUserID = org.m_DefUserID; //�f�t�H���g���[�UID
    m_bPGPMIME = org.m_bPGPMIME; //PGP/MIME�Ή�
    m_bPGPCompatible = org.m_bPGPCompatible; //PGP�݊��̏����E�Í������s��
    m_bOpenPGP = org.m_bOpenPGP; //OpenPGP�ɏ������������Í������s��
    m_bThrowKeyID = org.m_bThrowKeyID; //��M�҂𓽖��ɂ���
    m_bRFC1991 = org.m_bRFC1991; //RFC1991�ɏ]��
    m_bMD5 = org.m_bMD5; //MD5���g�p����
    m_bTextModeSig = org.m_bTextModeSig; //���������̍ۂɃe�L�X�g���[�h�ŏ�������
}

/*----------------------------------------------------------------------*/
/* Purpose        : �v���Z�X���N������                                  */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Execute(void)
{
    if((IsActive())==true) { // �v���Z�X���I�����Ă��Ȃ���΋����I��
        Terminate();
    }

    //�R�}���h���C���̐��`
    //char szCom[_MAX_PATH];
    //GetEnvironmentVariable("COMSPEC" , szCom, _MAX_PATH);
    //string cmdline = szCom;
    //cmdline += " /C ";
    //cmdline += Quotation(m_ModuleName);
    string cmdline = m_ModuleName;
    //Quotation(cmdline);
    if((m_OptionStr.length())>0) {
        cmdline += " ";
        cmdline += m_OptionStr;
    }
    if((m_CommandStr.length())>0) {
        cmdline += " ";
        cmdline += m_CommandStr;
    }

    //�p�����[�^�̏�����
    SECURITY_ATTRIBUTES satt;
    satt.nLength = sizeof(SECURITY_ATTRIBUTES);
    satt.lpSecurityDescriptor = NULL;
    satt.bInheritHandle = TRUE;

    STARTUPINFO si;
    ZeroMemory( &si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
#ifdef _DEBUG
    si.wShowWindow = SW_MINIMIZE;
    //si.wShowWindow = SW_HIDE;
#else
    si.wShowWindow = SW_HIDE;
#endif
    CreatePipe(&m_hStdinRead,  &m_hStdinWrite,  &satt, m_PipeSize);
    CreatePipe(&m_hStdoutRead, &m_hStdoutWrite, &satt, m_PipeSize);
    CreatePipe(&m_hStderrRead, &m_hStderrWrite, &satt, m_PipeSize);
    si.hStdOutput = m_hStdoutWrite;
    si.hStdError  = m_hStderrWrite;
    si.hStdInput  = m_hStdinRead;

    PROCESS_INFORMATION pi;
    ZeroMemory( &pi, sizeof(PROCESS_INFORMATION));

    //�v���Z�X�̋N��
    g_LogFile.AppendMessage(cmdline.c_str());
    if((CreateProcess(NULL, (LPTSTR)cmdline.c_str(), NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE | HIGH_PRIORITY_CLASS, NULL, NULL, &si, &pi))==FALSE) {
        Sleep(10); // �N�b�V�����H
        CloseHandle(pi.hThread);
        Terminate(); //�p�����[�^��߂�
        return false;
    }
    else {
        m_hProcess = pi.hThread;
        return true;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �v���Z�X���I������                                  */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CGnuPGModule::Terminate(void)
{
    //�v���Z�X�̏I��
    if(m_hProcess!=NULL) { //���쒆
        if((IsActive())==true) { // �v���Z�X���I�����Ă��Ȃ���΋����I��
            TerminateProcess(m_hProcess, 0);
            WaitForSingleObject(m_hProcess, 1000); //��������܂�1�b���炢�҂��Ă݂�
        }
        CloseHandle(m_hProcess);
        m_hProcess = NULL;
    }

    //�p�C�v�����
    if(m_hStdoutRead!=NULL) {
        CloseHandle(m_hStdoutRead);
        m_hStdoutRead = NULL;
    }
    if(m_hStdoutWrite!=NULL) {
        CloseHandle(m_hStdoutWrite);
        m_hStdoutWrite = NULL;
    }
    if(m_hStderrRead!=NULL) {
        CloseHandle(m_hStderrRead);
        m_hStderrRead = NULL;
    }
    if(m_hStderrWrite!=NULL) {
        CloseHandle(m_hStderrWrite);
        m_hStderrWrite = NULL;
    }
    if(m_hStdinRead!=NULL) {
        CloseHandle(m_hStdinRead);
        m_hStdinRead = NULL;
    }
    if(m_hStdinWrite!=NULL) {
        CloseHandle(m_hStdinWrite);
        m_hStdinWrite = NULL;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �����񂪋󔒕������܂ޏꍇ�̓_�u���N�H�[�e�V�����ň͂�*/
/* Inputs         : ������                                              */
/* Ootput         : �ϊ�������                                          */
/*----------------------------------------------------------------------*/
string& CGnuPGModule::Quotation(string& str)
{
    string::size_type len = str.find_first_of(" \t");
    if(len!=string::npos) { //�󔒂��܂ޕ�����
        Dequotation(str); //��������_�u���N�H�[�e�V�������O��
        string sbuf = "\"";
        sbuf += str + "\"";
        str = sbuf;
    }

    return str;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �_�u���N�H�[�e�V�����݂̈͂��O��                    */
/* Inputs         : ������                                              */
/* Ootput         : �ϊ�������                                          */
/*----------------------------------------------------------------------*/
string& CGnuPGModule::Dequotation(string& str)
{
    for(;;) {
        string::size_type len = str.length();
        if(len>=2 && str[0]=='\"' && str[len-1]=='\"') {
            str = str.substr(1, len-2);
        }
        else {
            break;
        }
    }

    return str;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �v���Z�X�͉ғ�����?                                 */
/* Inputs         : �^�C���A�E�g����                                    */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::IsActive(DWORD tout)
{
    if(m_hProcess!=NULL) { //���쒆
        if(WaitForSingleObject(m_hProcess, tout) != WAIT_OBJECT_0) { //�w�莞�ԑ҂��Ă��V�O�i���Ȃ�
            return true;
        }
        //DWORD dwStatus;
        //if((GetExitCodeProcess(m_hProcess, &dwStatus))!=FALSE) {
        //    if(dwStatus==STILL_ACTIVE) {
        //        return true;
        //    }
        //}
    }

    return false;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �v���Z�X���I������܂ő҂�                          */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
void CGnuPGModule::WaitforTerminate(void)
{
    IsActive(30000); //�ő�30�b�܂ő҂�?
    //for(int i=0; i<100 && (IsActive())==true; i++) { //�ő�10�b�܂ő҂�
    //for(int i=0; i<300 && (IsActive())==true; i++) { //�ő�30�b�܂ő҂�?
    //    Sleep(100);
    //}
    //Read(m_StdoutBuffer, true);
    Read(m_StderrBuffer, false);
    Terminate();
}

/*----------------------------------------------------------------------*/
/* Purpose        : �W�����͂ւ̓���                                    */
/* Inputs         : �C���v�b�g���b�Z�[�W                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Write(string& inpMsg)
{
    string::size_type len = inpMsg.length();
    if(len<=0) { //���̓��b�Z�[�W���Ȃ�
        return true;
    }
    char* msg = new char[len+1];
    strncpy(msg, inpMsg.c_str(), len);
    msg[len] = '\0';

    char* p = msg;
    DWORD dwRest = len;
    for(;;) {
        DWORD dwLen = 0;
        //if((IsActive())==false) { //�v���Z�X�͉ғ����Ă��Ȃ�
        //    delete [] msg;
        //    return false;
        //}
        if((WriteFile(m_hStdinWrite, p, dwRest, &dwLen, NULL))==FALSE) {
            delete [] msg;
            return false;
        }
        if(dwLen>=dwRest) { //�S�ď������݊���
            break;
        }
        p += dwLen;
        dwRest -= dwLen;
    }

    delete [] msg;
    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �W���o�͂���̃��b�Z�[�W                            */
/* Inputs         : �o�̓��b�Z�[�W�i�[��                                */
/* Ootput         : Boolearn                                            */
/*----------------------------------------------------------------------*/
bool CGnuPGModule::Read(string& outpMsg, bool bBlock)
{
    outpMsg = "";

    //�p�C�v�̏�Ԃ��擾����
    DWORD dwSize;
    if(bBlock==false) {
        DWORD dwLen;
        if((PeekNamedPipe(m_hStderrRead, NULL, 0, &dwLen, &dwSize, NULL))==FALSE) { //�ǂݎ�莸�s
            return false;
        }
        else if(dwSize<=0) { //�����Ȃ�����
            return true;
        }
        dwSize += 8; //strstr() �֐��΍�?
    }
    else {
        dwSize = m_PipeSize;
    }

    char* buf = new char[dwSize];
    if((ReadFile(m_hStderrRead, buf, dwSize, &dwSize, NULL))==FALSE) {
        delete [] buf;
        return false;
    }
    buf[dwSize] = '\0';
    outpMsg = buf;
    delete [] buf;

#ifdef _DEBUG
    g_LogFile.AppendMessage(outpMsg.c_str());
#endif
    return true;
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
