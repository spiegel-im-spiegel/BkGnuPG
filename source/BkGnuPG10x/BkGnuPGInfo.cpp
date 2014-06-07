/************************************************************************/
/*  File name : BkGnuPGInfo.cpp                                         */
/************************************************************************/
/*  Contents  : BkGnuPG ������A�N�Z�X�N���X                          */
/*                                                                      */
/*  Auther    : Yasuhiro ARAKAWA    Version 0.00    2000.09.12          */
/*                                  Version 0.10    2000.09.27          */
/*                                  Version 0.20    2000.10.03          */
/*                                  Version 0.30    2000.10.08          */
/*                                  Version 0.32    2000.10.26          */
/*                                  Version 0.40    2000.10.31          */
/*                                  Version 1.0.0   2000.11.08          */
/*  Modified  : Yasuhiro ARAKAWA, Canalian, hex                         */
/*                                  Version 1.0.4   2003.05.01          */
/************************************************************************/

/**** Incude Files ****/
#include "BkGnuPGInfo.h"
#include "BkGnuPGDef.h"

#include <string>
#include <sstream>
using namespace std;

#include "debug.h" //�Ō�ɒ�`


/**** Internal Macro ****/


/**** Typedef ****/


/**** Constants ****/


/**** Prototyping ****/
CBeckyAPI CBkGnuPGInfo::m_BkApi; //Becky! API �C���^�t�F�[�X�N���X (You can have only one instance in a project.)
bool CBkGnuPGInfo::m_InitFlag = false; //�����������t���O


/*----------------------------------------------------------------------*/
/* Purpose        : �f�t�H���g�R���X�g���N�^                            */
/* Inputs         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
CBkGnuPGInfo::CBkGnuPGInfo(void)
    : m_bEnableCcUser(false), m_bEnableBccUser(false), m_bEnableFromUser(false), m_bEnableDefUser(false), m_bShowText(false),
        m_CharSet(CGnuPGModule::DEFAULT), m_bDebugLog(false), m_bUseAgent(false)
{
    m_BaseDir = ".\\"; //��芸�����J�����g�f�B���N�g���ɂ��Ă���
    m_DataDir = ".\\"; //��芸�����J�����g�f�B���N�g���ɂ��Ă���
    m_TempDir = ".\\"; //��芸�����J�����g�f�B���N�g���ɂ��Ă���
    m_IniPath = m_BaseDir + "BkGnuPG.ini";
    m_LogPath = m_BaseDir + "BkGnuPG.log";
    m_UserID = m_PassPhrase = "";
    for(int i=0; i<MenuMax; i++) { //Menu ID
        m_MainMenuID[i] = m_LVMenuID[i] = m_CompMenuID[i] = 0;
    }
}

/*----------------------------------------------------------------------*/
/* Purpose        : �f�X�g���N�^                                        */
/* Inputs         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
CBkGnuPGInfo::~CBkGnuPGInfo(void)
{
    //TODO
}

/*----------------------------------------------------------------------*/
/* Purpose        : �C���X�^���X�̃R�s�[                                */
/* Inputs         : �R�s�[���C���X�^���X                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::Copy(const CBkGnuPGInfo& org)
{
    m_GPG = org.m_GPG;
    m_hInstance = org.m_hInstance;
    m_BaseDir = org.m_BaseDir;
    m_DataDir = org.m_DataDir;
    m_TempDir = org.m_TempDir;
    m_IniPath = org.m_IniPath;
    m_LogPath = org.m_LogPath;
    m_bEnableCcUser = org.m_bEnableCcUser;
    m_bEnableBccUser = org.m_bEnableBccUser;
    m_bEnableFromUser = org.m_bEnableFromUser; //�Í������̌��J��ID��From�A�h���X���܂߂�
    m_bEnableDefUser = org.m_bEnableDefUser; //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
    m_bShowText = org.m_bShowText; //�p�X�t���[�Y�\��
    m_bUseAgent = org.m_bUseAgent; // Use GPG Agent
    m_UserID = org.m_UserID;
    m_PassPhrase = org.m_PassPhrase;
    m_CharSet = org.m_CharSet;
    m_bDebugLog = org.m_bDebugLog;
    for(int i=0; i<MenuMax; i++) { //Menu ID
        m_MainMenuID[i] = org.m_MainMenuID[i];
        m_LVMenuID[i] = org.m_LVMenuID[i];
        m_CompMenuID[i] = org.m_CompMenuID[i];
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ��񏉊���                                          */
/* Inputs         : ���W���[���C���X�^���X�̃n���h��                    */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
bool CBkGnuPGInfo::InitInstance(HINSTANCE hModule)
{
    if(m_InitFlag==true) { //���ɏ������ς�
        return true;
    }

    //Becky! API �C���^�t�F�[�X�N���X�̏�����
    if (!m_BkApi.InitAPI()) {
        return false;
    }
    m_InitFlag = true;

    //�p�X���̎擾
    m_hInstance = hModule;
    char szPath[_MAX_PATH+2]; // Ini file to save your plugin settings.
    GetModuleFileName((HINSTANCE)hModule, szPath, _MAX_PATH);
    SetFilePath(szPath); //�e��t�@�C���ւ̃p�X��ݒ肷��

    //���ϐ��̎擾
    GetEnv(); //���ϐ��̎擾

    return true;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �e��t�@�C���ւ̃p�X��ݒ肷��                      */
/* Inputs         : ���W���[���ւ̃p�X                                  */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetFilePath(const char* modulePath)
{
    //�N���f�B���N�g���擾
    string path = modulePath;
    string::size_type len = path.find_last_of("\\");
    string fname;
    if(len!=string::npos) {
        m_BaseDir = path.substr(0, len+1);
        fname = path.substr(len+1);
    }
    else {
        m_BaseDir = ".\\";
        fname = path;
    }
    //�t�@�C����(�x�[�X��)�擾
    len = fname.find_last_of(".");
    if(len!=string::npos) {
        fname = fname.substr(0, len);
    }
    //�f�[�^�t�H���_�擾
    m_DataDir = m_BkApi.GetDataFolder();
    len = m_DataDir.length();
    if(len>0 && m_DataDir[len-1]!='\\') {
        m_DataDir += '\\';
    }
    //GnuPG�̃z�[���f�B���N�g�����擾����
    string gpgHome = CGnuPGModule::GetGPGHomeDir();
    len = gpgHome.length();
    if(len>0 && gpgHome[len-1]!='\\') {
        gpgHome += '\\';
    }

    //INI�t�@�C������
    if((IsFileExist(m_DataDir+"PlugIns\\"+fname+".ini"))==true) { //�f�[�^�t�H���_��INI�t�@�C��������
        m_IniPath = m_DataDir + "PlugIns\\";
    }
    else if((gpgHome.length())>0 && (IsFileExist(gpgHome+fname+".ini"))==true) { //GnuPG�̃z�[���f�B���N�g����INI�t�@�C��������
        m_IniPath = gpgHome;
    }
    else { //�N���f�B���N�g����INI�t�@�C����u��
        m_IniPath = m_BaseDir;
    }

    //�t�@�C���p�X�ݒ�
    m_IniPath += fname;
    m_LogPath = m_IniPath;
    m_IniPath += ".ini";
    m_LogPath += ".log";

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : �e���|�����f�B���N�g����ݒ肷��                    */
/* Inputs         : �e���|�����f�B���N�g��                              */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetTempPath(const char* tempPath)
{
    if(tempPath==NULL) {
        return;
    }
    int len = strlen(tempPath);
    if(len==0) {
        return;
    }
    m_TempDir = tempPath;
    if(tempPath[len-1]!='\\') {
        m_TempDir += "\\";
    }

    return;
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���ϐ��̎擾                                      */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::GetEnv(void)
{
    //GPG�R�}���h��
    char path[_MAX_PATH+2];
    string gpgPath = CGnuPGModule::GetGPGModuleName();
    //GetPrivateProfileString("Settings", "CmdName", m_GPG.GetDefModuleName(), path, _MAX_PATH, m_IniPath.c_str());
    GetPrivateProfileString("Settings", "CmdName", gpgPath.c_str(), path, _MAX_PATH, m_IniPath.c_str());
    if((strlen(path))>0) {
        m_GPG.SetModuleName(path); //GnuPG���W���[������ݒ肷��
    }
    else {
        //m_GPG.SetModuleName(m_GPG.GetDefModuleName()); //�f�t�H���g��GnuPG���W���[������ݒ肷��
        m_GPG.SetModuleName(gpgPath.c_str()); //���W�X�g���Œ�`���ꂽGnuPG�ւ̃p�X��ݒ肷��
    }
    //�f�t�H���g���[�UID
    GetPrivateProfileString("Settings", "DefUserID", m_GPG.GetDefUserID(), path, _MAX_PATH, m_IniPath.c_str());
    m_GPG.SetDefUserID(path); //�f�t�H���g���[�UID��ݒ肷��
    //���b�Z�[�W�_�C�W�F�X�g�A���S���Y��
    GetPrivateProfileString("Settings", "MDalgo", m_GPG.IsMDalgo(), path, _MAX_PATH, m_IniPath.c_str());
    m_GPG.SetOptMDalgo(path); //���b�Z�[�W�_�C�W�F�X�g�A���S���Y����ݒ肷��
    //PGP/MIME
    int chk = m_GPG.IsPGPMIME() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "PGPMIME", chk, m_IniPath.c_str());
    m_GPG.SetOptPGPMIME(chk!=0);
    //PGP�݊�
    chk = m_GPG.IsPGPCompatible() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "PGPCompatible", chk, m_IniPath.c_str());
    m_GPG.SetOptPGPCompatible(chk!=0);
    //OpenPGP
    chk = m_GPG.IsOpenPGP() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "OpenPGP", chk, m_IniPath.c_str());
    m_GPG.SetOptOpenPGP(chk!=0);
    //������M��
    chk = m_GPG.IsThrowKeyID() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "AnonymousRecipient", chk, m_IniPath.c_str());
    m_GPG.SetOptThrowKeyID(chk!=0);
    //RFC1991�ɏ]��
    chk = m_GPG.IsRFC1991() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "RFC1991", chk, m_IniPath.c_str());
    m_GPG.SetOptRFC1991(chk!=0);
    //���b�Z�[�W�_�C�W�F�X�g��MD5���g��
    chk = m_GPG.IsMD5() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "UseMD5", chk, m_IniPath.c_str());
    m_GPG.SetOptMD5(chk!=0);
    //�Í������̌��J��ID��Cc�A�h���X���܂߂�
    chk = m_bEnableCcUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableCcUser", chk, m_IniPath.c_str());
    m_bEnableCcUser = (chk!=0);
    //�Í������̌��J��ID��Bcc�A�h���X���܂߂�
    chk = m_bEnableBccUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableBccUser", chk, m_IniPath.c_str());
    m_bEnableBccUser = (chk!=0);
    //�Í������̌��J��ID��From�A�h���X���܂߂�
    chk = m_bEnableFromUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableFromUser", chk, m_IniPath.c_str());
    m_bEnableFromUser = (chk!=0);
    //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
    chk = m_bEnableDefUser ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableDefUser", chk, m_IniPath.c_str());
    m_bEnableDefUser = (chk!=0);
    //���������̍ۂɃe�L�X�g���[�h�ŏ�������
    chk = m_GPG.IsTextModeSig() ? 1:0;
    chk = GetPrivateProfileInt("Settings", "EnableTextModeSig", chk, m_IniPath.c_str());
    m_GPG.SetOptTextModeSig(chk!=0);
    //�p�X�t���[�Y�\��
    chk = m_bShowText ? 1:0;
    chk = GetPrivateProfileInt("Settings", "ShowPassphrase", chk, m_IniPath.c_str());
    m_bShowText = (chk!=0);
    // Agent settings
    chk = m_bUseAgent ? 1:0;
    chk = GetPrivateProfileInt("Settings", "UseAgent", chk, m_IniPath.c_str());
    m_bUseAgent = (chk != 0);
    //GnuPG�o��
    GetPrivateProfileString("Output", "Charset", "", path, _MAX_PATH, m_IniPath.c_str());
    m_CharSet = GetCharSet(path);
    //���O���o��
    chk = m_bDebugLog ? 1:0;
    chk = GetPrivateProfileInt("Debug", "LogFile", chk, m_IniPath.c_str());
    m_bDebugLog = (chk!=0);

}

/*----------------------------------------------------------------------*/
/* Purpose        : ���ϐ��̐ݒ�                                      */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetEnv(void)
{
    //GPG�R�}���h��
    //WritePrivateProfileString("Settings", "CmdName", m_GPG.GetModuleName(), m_IniPath.c_str());
    //�f�t�H���g���[�UID
    WritePrivateProfileString("Settings", "DefUserID", m_GPG.GetDefUserID(), m_IniPath.c_str());
    //PGP/MIME
    char* s = m_GPG.IsPGPMIME() ? "1":"0";
    WritePrivateProfileString("Settings", "PGPMIME", s, m_IniPath.c_str());
    //PGP�݊�
    s = m_GPG.IsPGPCompatible() ? "1":"0";
    WritePrivateProfileString("Settings", "PGPCompatible", s, m_IniPath.c_str());
    //OpenPGP
    s = m_GPG.IsOpenPGP() ? "1":"0";
    WritePrivateProfileString("Settings", "OpenPGP", s, m_IniPath.c_str());
    //������M��
    s = m_GPG.IsThrowKeyID() ? "1":"0";
    WritePrivateProfileString("Settings", "AnonymousRecipient", s, m_IniPath.c_str());
    //RFC1991�ɏ]��
    s = m_GPG.IsRFC1991() ? "1":"0";
    WritePrivateProfileString("Settings", "RFC1991", s, m_IniPath.c_str());
    //���b�Z�[�W�_�C�W�F�X�g��MD5���g��
    s = m_GPG.IsMD5() ? "1":"0";
    WritePrivateProfileString("Settings", "UseMD5", s, m_IniPath.c_str());
    //Use GPG Agent
    SetUseAgentFlag();
    //�Í������̌��J��ID��Cc�A�h���X���܂߂�
    s = m_bEnableCcUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableCcUser", s, m_IniPath.c_str());
    //�Í������̌��J��ID��Bcc�A�h���X���܂߂�
    s = m_bEnableBccUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableBccUser", s, m_IniPath.c_str());
    //�Í������̌��J��ID��From�A�h���X���܂߂�
    s = m_bEnableFromUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableFromUser", s, m_IniPath.c_str());
    //�Í������̌��J��ID�Ƀf�t�H���g���[�U���܂߂�
    s = m_bEnableDefUser ? "1":"0";
    WritePrivateProfileString("Settings", "EnableDefUser", s, m_IniPath.c_str());
}

/*----------------------------------------------------------------------*/
/* Purpose        : ���ϐ� (UseAgent) �̐ݒ�                          */
/* Inputs         : �Ȃ�                                                */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
void CBkGnuPGInfo::SetUseAgentFlag(void)
{
    //Use GPG Agent
    char* s = m_bUseAgent ? "1":"0";
    WritePrivateProfileString("Settings", "UseAgent", s, m_IniPath.c_str());
}

/*----------------------------------------------------------------------*/
/* Purpose        : �L�����N�^�Z�b�g�̎���                              */
/* Inputs         : �L�����N�^�Z�b�g������������                        */
/* Ootput         : �Ȃ�                                                */
/*----------------------------------------------------------------------*/
CGnuPGModule::CHARSET CBkGnuPGInfo::GetCharSet(const char* charset)
{
    if(charset==NULL || *charset=='\0') {
        return CGnuPGModule::DEFAULT;
    }

    if((stricmp(charset, "iso-2022-jp"))==0) {
        return CGnuPGModule::ISO_2022_IP;
    }
    else if((stricmp(charset, "euc-jp"))==0) {
        return CGnuPGModule::EUC_JP;
    }
    else if((stricmp(charset, "utf-8"))==0) {
        return CGnuPGModule::UTF_8;
    }
    else {
        return CGnuPGModule::DEFAULT;
    }
}

/* Copyright (C) Yasuhiro ARAKAWA  **************************************/
